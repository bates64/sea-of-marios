use futures::{select, FutureExt};
use futures_timer::Delay;
use log::{debug, info, warn};
use matchbox_socket::{ChannelConfig, Error as SocketError, PeerId, PeerState, WebRtcSocketBuilder};
use std::time::Duration;
use tokio::sync::mpsc::{Sender, Receiver};
use crate::gdb;

const PROTOCOL_CHANNEL: usize = 0; // reliable
const SYNC_DATA_CHANNEL: usize = 1; // unreliable

/// Commands from gdb to net
pub enum Command {
    Connect,
    Disconnect,
    SyncData(Vec<u8>),
}

pub async fn connect_and_retry(mut rx: Receiver<Command>, mut tx: Sender<gdb::Command>) {
    while let Some(command) = rx.recv().await {
        if let Command::Connect = command {
            connect(&mut rx, &mut tx).await;
        }
    }
}

struct PeerRegistry {
    everyone: Vec<PeerId>, // including me
}

impl PeerRegistry {
    pub fn new() -> Self {
        Self {
            everyone: Vec::new(),
        }
    }

    pub fn connect(&mut self, peer: PeerId) {
        self.everyone.push(peer);
        self.everyone.sort_unstable();
    }

    pub fn find(&self, peer: PeerId) -> Option<usize> {
        self.everyone.iter().position(|&p| p == peer)
    }

    pub fn disconnect(&mut self, peer: PeerId) {
        if let Some(index) = self.find(peer) {
            self.everyone.remove(index);
            self.everyone.sort_unstable();
        }
    }
}

async fn connect(rx: &mut Receiver<Command>, tx: &mut Sender<gdb::Command>) {
    info!("Connecting to signaling server...");
    let (mut socket, loop_fut) = WebRtcSocketBuilder::new("ws://match.bates64.com:3536/")
        .reconnect_attempts(None) // keep trying to reconnect
        .add_channel(ChannelConfig { ordered: false, max_retransmits: None }) // PROTOCOL_CHANNEL | reliable but not ordered
        .add_channel(ChannelConfig::unreliable()) // SYNC_DATA_CHANNEL
        .build();

    let loop_fut = async {
        match loop_fut.await {
            Ok(()) => info!("Exited cleanly :)"),
            Err(e) => match e {
                SocketError::ConnectionFailed(e) => {
                    warn!("couldn't connect to signaling server, please check your connection: {e}");
                    // todo: show prompt and reconnect?
                }
                SocketError::Disconnected(e)  => {
                    warn!("you were kicked, or your connection went down, or the signaling server stopped: {e}");
                }
            },
        }
    }
    .fuse();
    futures::pin_mut!(loop_fut);

    let timeout = Delay::new(Duration::from_millis(30));
    futures::pin_mut!(timeout);

    // Wait for our id
    info!("Waiting for id to be assigned...");
    let my_id = loop {
        if let Some(id) = socket.id() {
            break id;
        }
        select! {
            _ = (&mut timeout).fuse() => (),
            _ = &mut loop_fut => return,
        }
    };
    info!("Connected! Assigned id: {my_id}");

    let mut registry = PeerRegistry::new();
    let mut registry_changed = true;
    registry.connect(my_id);

    'main: loop {
        // Handle any new peers
        for (peer, state) in socket.update_peers() {
            match state {
                PeerState::Connected => {
                    registry.connect(peer);
                    registry_changed = true;
                    info!("Peer connected ({peer})");
                }
                PeerState::Disconnected => {
                    registry.disconnect(peer);
                    registry_changed = true;
                    info!("Peer disconnected ({peer})");
                }
            }
        }

        // Update Header::me if the registry has changed
        // We could have moved to a different index
        if registry_changed {
            let me_index = registry.find(my_id).unwrap();
            let _ = tx.send(gdb::Command::SetMe(me_index as u8)).await;
            registry_changed = false;
        }

        // Accept any messages incoming from peers
        for (peer, sync_data) in socket.channel_mut(SYNC_DATA_CHANNEL).receive() {
            let index = match registry.find(peer) {
                Some(index) => index,
                None => {
                    warn!("Ignoring sync data from unregistered peer {peer}");
                    continue;
                },
            };

            if check_checksum(&sync_data) {
                let _ = tx.send(gdb::Command::SyncData(index as u8, sync_data.into())).await;
            } else {
                warn!("Invalid checksum from peer {peer}");
            }
        }

        // Handle any new messages from game over gdb
        while let Ok(command) = rx.try_recv() {
            match command {
                Command::SyncData(sync_data) => {
                    if !check_checksum(&sync_data) {
                        warn!("Invalid checksum from gdb");
                        continue;
                    }

                    // Broadcast message to all peers
                    let peers: Vec<PeerId> = socket.connected_peers().collect();
                    let channel = socket.channel_mut(SYNC_DATA_CHANNEL);
                    for peer in peers {
                        channel.send(sync_data.as_slice().into(), peer);
                    }
                }
                Command::Connect => return Box::pin(connect(rx, tx)).await, // Reconnect
                Command::Disconnect => break 'main,
            }
        }

        select! {
            // Restart this loop every 30ms (30 FPS = 33.3ms)
            _ = (&mut timeout).fuse() => {
                timeout.reset(Duration::from_millis(30));
            }

            // Or break if the message loop ends (disconnected, closed, etc.)
            _ = &mut loop_fut => {
                break;
            }
        }
    }

    info!("connection closed");
}

/// syncData checksum is sum of all bytes except the first, which is the checksum itself
fn check_checksum(data: &[u8]) -> bool {
    let checksum = data[0];
    let calculated: u8 = data.iter()
        .skip(1)
        .fold(0, |acc, &byte| acc.wrapping_add(byte));
    let ok = checksum == calculated;
    if !ok {
        debug!("bad checksum: {checksum:02X}, calculated: {calculated:02X}");
    }
    ok
}
