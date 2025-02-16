# Network Bridge

Provides a synced-memory bridge between Paper Mario DX games over a P2P network. Mods can use this to sync
arbitrary data between games, such as player positions, health, etc.

Network-synced memory is located at 0x80700000. See src/net/net.cpp for more details.

## Usage

```
nix run .#bridge
```

## Development

```
nix develop .#bridge
```
