use eframe::CreationContext;
use egui::*;
use egui_tracing::EventCollector;
use tokio::sync::mpsc::{Receiver, Sender};
use log::error;

use crate::{gdb, net};

pub struct App {
    collector: EventCollector,
    rx: Receiver<Command>,
    gdb: Sender<gdb::Command>,
    net: Sender<net::Command>,
    logs_open: bool,
    state: State,
    gdb_port_input: String,
    error_message: Option<String>,
}

enum State {
    Welcome,

    Ares,
    Project64,
    Nintendo64,

    Connected,

    ConnectionLost,
}

pub enum Command {
    GdbConnected,
    GdbDisconnected(Option<gdb::Error>),
}

impl App {
    pub fn new(cc: &CreationContext, collector: EventCollector, rx: Receiver<Command>, gdb: Sender<gdb::Command>, net: Sender<net::Command>) -> Self {
        egui_extras::install_image_loaders(&cc.egui_ctx);
        Self {
            collector, rx, gdb, net,
            logs_open: false,
            state: State::Welcome,
            gdb_port_input: std::env::var("GDB_PORT").unwrap_or_else(|_| "9123".to_string()),
            error_message: None,
        }
    }

    fn welcome(&mut self, ui: &mut Ui) {
        ui.vertical(|ui| {
            ui.label("Select emulator");

            ui.columns_const(|[col_1, col_2, col_3]| {
                if col_1.add(Image::new(include_image!("../assets/ares.png")).sense(Sense::click())).clicked() {
                    self.state = State::Ares;
                }
                if col_2.add(Image::new(include_image!("../assets/project64.png")).sense(Sense::click())).clicked() {
                    self.state = State::Project64;
                }
                if col_3.add(Image::new(include_image!("../assets/n64.png")).sense(Sense::click())).clicked() {
                    self.state = State::Nintendo64;
                }
            });
        });
    }

    fn ares(&mut self, ui: &mut Ui) {
        ui.hyperlink("https://ares-emu.net/");
        ui.label("Open the patched ROM in ares and set these settings:");

        // TODO: use LayoutJob
        ui.label("Check Options > Nintendo 64 Settings > 4MB Expansion Pak");
        ui.label("Set Drivers > Input > When focus lost to Block input or Allow input");
        ui.label("Set Debug > GDB-Server > Port to 9123");
        ui.label("Check Debug > GDB-Server > Enabled");

        ui.add(TextEdit::singleline(&mut self.gdb_port_input).hint_text("Port"));

        if self.gdb_port_input.parse::<u16>().is_err() {
            ui.label(RichText::new("Invalid port").color(Color32::LIGHT_RED));
        } else {
            ui.horizontal(|ui| {
                if ui.button("Connect").clicked() {
                    let addr = format!("[::1]:{}", self.gdb_port_input);
                    let gdb = self.gdb.clone();
                    tokio::spawn(async move {
                        if let Err(e) = gdb.send(gdb::Command::ConnectGdb(addr)).await {
                            error!("failed to send to gdb thread: {}", e);
                        }
                    });
                }

                if let Some(e) = &self.error_message {
                    ui.label(RichText::new(e).color(Color32::LIGHT_RED));
                }
            });
        }

        if ui.button("Back").clicked() {
            self.state = State::Welcome;
        }
    }

    fn project64(&mut self, ui: &mut Ui) {
        ui.label("Not recommended");
        // TODO: walk through, or script, project64.js setup

        if ui.button("Back").clicked() {
            self.state = State::Welcome;
        }
    }

    fn nintendo64(&mut self, ui: &mut Ui) {
        ui.label("Not yet implemented, sorry");
        if ui.button("Back").clicked() {
            self.state = State::Welcome;
        }
    }

    fn connected(&mut self, ui: &mut Ui) {
        ui.label("Communication established with the game!");
        ui.label("The bridge is now active.");
        ui.label("You can go back to the game now. Don't close this window.");
    }

    fn connection_lost(&mut self, ui: &mut Ui) {
        ui.label("Communication with the game has been lost.");
        if let Some(e) = &self.error_message {
            ui.label("Reason:");
            ui.label(RichText::new(e).color(Color32::LIGHT_RED));
        }
        ui.label("Please restart the game and try again.");
        if ui.button("OK").clicked() {
            self.state = State::Welcome;
        }
    }
}

impl eframe::App for App {
    fn update(&mut self, ctx: &egui::Context, _frame: &mut eframe::Frame) {
        match self.rx.try_recv() {
            Ok(Command::GdbConnected) => {
                self.state = State::Connected;
            }
            Ok(Command::GdbDisconnected(e)) => {
                self.error_message = e.map(|e| e.to_string());
                if matches!(self.state, State::Connected) {
                    self.state = State::ConnectionLost;
                }
            }
            Err(tokio::sync::mpsc::error::TryRecvError::Empty) => {}
            Err(tokio::sync::mpsc::error::TryRecvError::Disconnected) => panic!("gui channel closed"),
        }

        CentralPanel::default().show(ctx, |ui| {
            if ui.button("Show logs").clicked() {
                self.logs_open = !self.logs_open;
            }
            Window::new("Logs").open(&mut self.logs_open).show(ctx, |ui| {
                ui.add(egui_tracing::Logs::new(self.collector.clone()));
            });

            ui.separator();

            match self.state {
                State::Welcome => self.welcome(ui),
                State::Ares => self.ares(ui),
                State::Project64 => self.project64(ui),
                State::Nintendo64 => self.nintendo64(ui),
                State::Connected => self.connected(ui),
                State::ConnectionLost => self.connection_lost(ui),
            }
        });
    }
}
