use eframe::CreationContext;
use egui::{CentralPanel};
use egui_tracing::EventCollector;
use tokio::sync::mpsc::Sender;

use crate::{gdb, net};

pub struct App {
    collector: EventCollector,
    gdb_tx: Sender<gdb::Command>,
    net_tx: Sender<net::Command>,
    dbg_print_text: String,
}

impl App {
    pub fn new(cc: &CreationContext, collector: EventCollector, gdb_tx: Sender<gdb::Command>, net_tx: Sender<net::Command>) -> Self {
        egui_extras::install_image_loaders(&cc.egui_ctx);
        Self {
            collector, gdb_tx, net_tx,
            dbg_print_text: String::from("Hello, world!"),
        }
    }
}

impl eframe::App for App {
    fn update(&mut self, ctx: &egui::Context, _frame: &mut eframe::Frame) {
        CentralPanel::default().show(ctx, |ui| {
            ui.add(egui_tracing::Logs::new(self.collector.clone()));
        });
    }
}
