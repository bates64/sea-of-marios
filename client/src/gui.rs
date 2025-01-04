use eframe::CreationContext;
use egui_tracing::EventCollector;

pub struct App {
    collector: EventCollector,
}

impl App {
    pub fn new(cc: &CreationContext, collector: EventCollector) -> Self {
        egui_extras::install_image_loaders(&cc.egui_ctx);

        Self { collector }
    }
}

impl eframe::App for App {
    fn update(&mut self, ctx: &egui::Context, _frame: &mut eframe::Frame) {
        egui::CentralPanel::default().show(ctx, |ui| {
            ui.add(egui_tracing::Logs::new(self.collector.clone()))
        });
    }
}
