#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")] // hide console window on Windows in release

mod net;
mod gui;
mod gdb;
mod rpc;

use tokio::runtime::Runtime;
use tokio::sync::mpsc;
use futures::join;

fn main() -> eframe::Result {
    // Setup logging
    use tracing_subscriber::prelude::*;
    let collector = egui_tracing::EventCollector::default();
    tracing_subscriber::registry()
        .with(
            tracing_subscriber::EnvFilter::try_from_default_env()
                .unwrap_or_else(|_| "client=debug,matchbox_socket=info".into()),
        )
        .with(tracing_subscriber::fmt::layer())
        .with(collector.clone())
        .init();

    // Setup tokio
    let rt = Runtime::new().expect("unable to create async runtime");
    let (gdb_tx, gdb_rx) = mpsc::channel::<gdb::Command>(256);
    let (net_tx, net_rx) = mpsc::channel::<net::Command>(256);
    let _enter = rt.enter();
    std::thread::spawn(move || {
        rt.block_on(async {
            join!(net::connect_and_retry(net_rx, gdb_tx), gdb::connect_and_retry(gdb_rx, net_tx));
        })
    });

    let native_options = eframe::NativeOptions {
        viewport: egui::ViewportBuilder::default()
            .with_inner_size([600.0, 200.0])
            .with_min_inner_size([400.0, 200.0])
            /*.with_icon(
                // NOTE: Adding an icon is optional
                eframe::icon_data::from_png_bytes(&include_bytes!("../assets/macos_icon.png")[..])
                    .expect("Failed to load icon"),
            )*/
            .with_title("Paper Mario Online"),
        ..Default::default()
    };
    eframe::run_native(
        "Paper Mario Online",
        native_options,
        Box::new(|cc| Ok(Box::new(gui::App::new(cc, collector)))),
    )
}
