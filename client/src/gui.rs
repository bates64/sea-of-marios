use eframe::CreationContext;
use egui::{TextEdit, CentralPanel};
use egui_tracing::EventCollector;
use tokio::sync::mpsc::Sender;

use crate::{gdb, net, rpc};

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
        egui::CentralPanel::default().show(ctx, |ui| {
            //ui.add(egui_tracing::Logs::new(self.collector.clone()))

            CentralPanel::default().show(ctx, |ui| {
                ui.label("Debug message");
                ui.add(TextEdit::singleline(&mut self.dbg_print_text).hint_text("Type here..."));

                if ui.button("Send").clicked() {
                    let mut message = rpc::Message::new();
                    message.dbg_print(&self.dbg_print_text);
                    tokio::task::block_in_place(|| {
                        let rt = tokio::runtime::Handle::current();
                        let _ = rt.block_on(self.gdb_tx.send(gdb::Command::SendMesageToGame(message.done())));
                    });
                    //self.input_text.clear();
                }
            });
        });
    }
}
