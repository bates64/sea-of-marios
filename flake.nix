{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

    # This commit uses binutils 2.39. We don't want binutils 2.40+ because of a performance regression in ld.
    nixpkgs-binutils-2_39.url = "github:NixOS/nixpkgs/55070e598e0e03d1d116c49b9eff322ef07c6ac6";

    flake-utils.url = "github:numtide/flake-utils";

    star-rod.url = "github:z64a/star-rod/9339cb4e867514267ff8ab404b00b53e5a5e67dd";
    star-rod.inputs.nixpkgs.follows = "nixpkgs";
  };
  nixConfig = {
    extra-substituters = [
      "https://papermario-dx.cachix.org"
      "https://papermario-dx-aarch64-darwin.cachix.org"
    ];
    extra-trusted-public-keys = [
      "papermario-dx.cachix.org-1:VTXILrqiajck9s5U2O3nDJH0pAI64GAJK41b2pt1JIk="
      "papermario-dx-aarch64-darwin.cachix.org-1:Tr3Kx63xvrTDCOELacSPjMC3Re0Nwg2WBRSprH3eMU0="
    ];
  };
  outputs = { self, nixpkgs, flake-utils, nixpkgs-binutils-2_39, star-rod }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        crossSystem = {
          config = "mips-linux-gnu"; # prefix expected by scripts in tools/
          system = "mips64-elf";
          gcc.arch = "vr4300";
          gcc.tune = "vr4300";
          gcc.abi = "32";
        };
        pkgs = import nixpkgs { inherit system; };
        pkgsCross = import nixpkgs { inherit system crossSystem; };
        binutils2_39 = (import nixpkgs-binutils-2_39 { inherit system crossSystem; }).buildPackages.binutilsNoLibc;
        baseRom = pkgs.requireFile {
          name = "papermario.us.z64";
          message = ''
            ==== MISSING BASE ROM =======================================================

            Paper Mario DX only supports the US version of Paper Mario. You need to
            provide a copy of the ROM in z64 format to build the project.

            Please rename your ROM to papermario.us.z64 and add it to the Nix store using
                nix-store --add-fixed sha256 papermario.us.z64
            then rerun nix-shell.

            If you don't have a ROM, dump it from your own cartridge.

            If you have a ROM but it is in n64 or v64 format, convert it to z64 first:
                https://hack64.net/tools/swapper.php

            If you added your ROM to the Nix store already, but it's still not working,
            your ROM must be incorrect. Make sure the version is US, the format is z64,
            and that it has not been modified.
          '';
          sha256 = "9ec6d2a5c2fca81ab86312328779fd042b5f3b920bf65df9f6b87b376883cb5b";
        };
        libraries = with pkgs; lib.optionals stdenv.isLinux [
          wayland
          libxkbcommon
          xorg.libX11
          xorg.libXcursor
          xorg.libXrandr
          xorg.libXi
          vulkan-loader
        ];
      in {
        packages = {
          bridge = pkgs.rustPlatform.buildRustPackage {
            pname = "papermario-dx-online-bridge";
            version = (builtins.fromTOML (builtins.readFile ./bridge/Cargo.toml)).package.version;

            nativeBuildInputs = with pkgs; lib.optionals stdenv.isLinux [
              mold-wrapped
            ];
            buildInputs = libraries;
            env = {
              RUSTFLAGS = pkgs.lib.optionalString pkgs.stdenv.isLinux "-C link-arg=-fuse-ld=mold";
              CARGO_INCREMENTAL = "0";
            };

            src = ./bridge;
            cargoLock.lockFile = ./bridge/Cargo.lock;
          };
        };

        devShells = {
          default = pkgsCross.mkShell {
            name = "papermario-dx";
            venvDir = "./venv";
            packages = with pkgs; [
              ninja # needed for ninja -t compdb in run, as n2 doesn't support it
              n2 # same as ninja, but with prettier output
              zlib
              libyaml
              python3
              python3Packages.virtualenv
              ccache
              git
              iconv
              gcc # for n64crc
              (callPackage ./tools/pigment64.nix {})
              (callPackage ./tools/crunch64.nix {})
              star-rod.packages.${system}.default
            ] ++ (if pkgs.stdenv.isLinux then [ pkgs.flips ] else []); # https://github.com/NixOS/nixpkgs/issues/373508
            shellHook = ''
              rm -f ./ver/us/baserom.z64 && cp ${baseRom} ./ver/us/baserom.z64
              export PAPERMARIO_LD="${binutils2_39}/bin/mips-linux-gnu-ld"

              # Install python packages (TODO: use derivations)x
              virtualenv venv --quiet
              source venv/bin/activate
              pip install -r ${./requirements.txt} --quiet
              pip install -r ${./requirements_extra.txt} --quiet
            '';
          };

          bridge = let
            # on macOS and Linux, use faster parallel linkers
            rustLinkerFlags =
              if pkgs.stdenv.isLinux
              then ["-fuse-ld=mold" "-Wl,--compress-debug-sections=zstd"]
              else if pkgs.stdenv.isDarwin
              then
                # on darwin, /usr/bin/ld actually looks at the environment variable
                # $DEVELOPER_DIR, which is set by the nix stdenv, and if set,
                # automatically uses it to route the `ld` invocation to the binary
                # within. in the devShell though, that isn't what we want; it's
                # functional, but Xcode's linker as of ~v15 (not yet open source)
                # is ultra-fast and very shiny; it is enabled via -ld_new, and on by
                # default as of v16+
                ["--ld-path=$(unset DEVELOPER_DIR; /usr/bin/xcrun --find ld)" "-ld_new"]
              else [];

            rustLinkerFlagsString =
              pkgs.lib.concatStringsSep " "
             (pkgs.lib.concatMap (x: ["-C" "link-arg=${x}"]) rustLinkerFlags);
        in
          pkgs.mkShell {
            name = "papermario-dx-online-bridge";
            packages = with pkgs; [
              cargo
              rustc
              cargo-watch
            ] ++ lib.optionals stdenv.isLinux [
              mold-wrapped
              xorg.libX11
            ];
            LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath libraries;
            env = {
              RUST_BACKTRACE = 1;
            };
            # The `RUSTFLAGS` environment variable is set in `shellHook` instead of `env`
            # to allow the `xcrun` command above to be interpreted by the shell.
            shellHook = ''
              export RUSTFLAGS="${rustLinkerFlagsString}"
            '';
          };
        };
      }
    );
}
