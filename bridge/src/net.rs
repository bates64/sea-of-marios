use futures::{select, FutureExt};
use futures_timer::Delay;
use log::{debug, info, warn, error};
use matchbox_socket::{ChannelConfig, Error as SocketError, PeerId, PeerState, WebRtcSocket, WebRtcSocketBuilder};
use std::time::Duration;
use tokio::sync::mpsc::{Sender, Receiver};
use serde::{Serialize, Deserialize};
use crate::{gdb, gui};

const PLAYER_REGISTRY_CHANNEL: usize = 0; // reliable
const SYNC_DATA_CHANNEL: usize = 1; // unreliable

/// Commands from gdb to net
pub enum Command {
    Connect { room: String },
    Disconnect,
    SyncData(Vec<u8>),
}

pub async fn connect_and_retry(mut rx: Receiver<Command>, mut gdb: Sender<gdb::Command>, mut gui: Sender<gui::Command>) {
    while let Some(command) = rx.recv().await {
        if let Command::Connect { room } = command {
            connect(&room, &mut rx, &mut gdb, &mut gui).await;
        }
    }
}

/// A PlayerRegistry maps connected PeerIds to player indices (0..=16).
///
/// There is exactly one host, and all other peers are clients.
///
/// The host is responsible for manging the registry and syncs it to all clients over PROTOCOL_CHANNEL. Specifically,
/// when a new peer connects, the host assigns it a player index and sends the updated registry to all peers. When
/// a peer disconnects, the host removes it from the registry and sends the updated registry to all peers.
///
/// The first peer to connect becomes the host.
/// If the current host disconnects, the peer with the lowest player index becomes the new host.
#[derive(Debug, Serialize, Deserialize)]
struct PlayerRegistry {
    host: PeerId,
    players: [Option<PeerId>; 16],
}

impl PlayerRegistry {
    pub async fn initial_connect(socket: &mut WebRtcSocket) -> Self {
        if let Some((_, data)) = socket.channel_mut(PLAYER_REGISTRY_CHANNEL).receive().first() {
            // We are not the host. We are a client.
            info!("I am a client");
            serde_json::from_slice(data).unwrap()
        } else {
            // No host. We are the host.
            info!("I am the host");
            let host = socket.id().unwrap();
            PlayerRegistry {
                host,
                players: [
                    Some(host),
                    None, None, None, None, None, None, None, None, None, None, None, None, None, None, None,
                ],
            }
        }
    }

    pub fn broadcast(&self, socket: &mut WebRtcSocket) {
        if !self.is_host(socket) {
            return;
        }

        let data = serde_json::to_vec(self).unwrap();
        let peers = socket.connected_peers().collect::<Vec<_>>();
        for peer in peers {
            socket.channel_mut(PLAYER_REGISTRY_CHANNEL).send(data.as_slice().into(), peer);
        }
        debug!("bcast registry {:?}", &self);
    }

    pub fn is_host(&self, socket: &mut WebRtcSocket) -> bool {
        self.host == socket.id().unwrap()
    }

    pub fn check_for_updates(&mut self, socket: &mut WebRtcSocket) {
        for (_, data) in socket.channel_mut(PLAYER_REGISTRY_CHANNEL).receive() {
            let registry: PlayerRegistry = serde_json::from_slice(&data).unwrap();

            if self.is_host(socket) && registry.host != self.host {
                // Someone else is trying to coup us!
                // Allow them if their uuid is higher than ours
                if registry.host < self.host {
                    warn!("disallowing host coup attempt");
                    self.broadcast(socket); // Send them the correct registry
                    continue;
                }
                warn!("coup successful, we are no longer host");
            }

            *self = registry;
            debug!("recv registry {:?}", &self);
        }
    }

    pub fn update_peers(&mut self, socket: &mut WebRtcSocket) {
        if self.is_host(socket) {
            let mut change = false;
            for (peer, state) in socket.update_peers() {
                match state {
                    PeerState::Connected => {
                        if let Some(index) = self.players.iter().position(|&p| p.is_none()) {
                            self.players[index] = Some(peer);
                            change = true;
                        } else {
                            warn!("room is full, ignoring new joiner {peer}");
                            // TODO: tell joiner to leave
                        }
                    }
                    PeerState::Disconnected => {
                        for id in self.players.iter_mut() {
                            if let Some(p) = id {
                                if *p == peer {
                                    *id = None;
                                    change = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            if change {
                self.broadcast(socket);
            }
        } else {
            // Check for host disconnect
            for (peer, state) in socket.update_peers() {
                if state == PeerState::Disconnected && peer == self.host {
                    info!("Host disconnected");

                    // Remove host from registry
                    for id in self.players.iter_mut() {
                        if let Some(p) = id {
                            if *p == self.host {
                                *id = None;
                                break;
                            }
                        }
                    }

                    // Make the connected player with the lowest index the new host
                    let new_host = self.players.iter().filter_map(|&p| p).min().unwrap();
                    if new_host == socket.id().unwrap() {
                        info!("I am the new host");
                        self.host = new_host;
                        self.broadcast(socket); // Clients need to learn about the new host
                    }
                }
            }
        }
    }

    pub fn find(&self, peer: PeerId) -> Option<usize> {
        self.players.iter().position(|&p| p == Some(peer))
    }
}

async fn connect(room: &str, rx: &mut Receiver<Command>, gdb: &mut Sender<gdb::Command>, gui: &mut Sender<gui::Command>) {
    info!("Connecting to signaling server...");
    let (mut socket, loop_fut) = WebRtcSocketBuilder::new(format!("ws://match.bates64.com:3536/{room}"))
        .reconnect_attempts(None) // keep trying to reconnect
        .add_channel(ChannelConfig { ordered: false, max_retransmits: None }) // PROTOCOL_CHANNEL | reliable but not ordered
        .add_channel(ChannelConfig::unreliable()) // SYNC_DATA_CHANNEL
        .build();

    let loop_fut = async {
        match loop_fut.await {
            Ok(()) => info!("connection closed"),
            Err(e) => match e {
                SocketError::ConnectionFailed(e) => {
                    warn!("couldn't connect to signaling server, please check your connection: {e}");
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

    // Wait a bit to make sure we learn about all peers
    let timeout = Delay::new(Duration::from_secs(2));
    futures::pin_mut!(timeout);
    select! {
        _ = (&mut timeout).fuse() => (),
        _ = &mut loop_fut => return,
    };

    let mut registry = PlayerRegistry::initial_connect(&mut socket).await;
    let mut known_me = -1;

    'main: loop {
        registry.update_peers(&mut socket);
        registry.check_for_updates(&mut socket);

        // Let game know our player index
        let me = registry.find(my_id).map(|idx| idx as i32).unwrap_or(-1);
        if me != known_me {
            known_me = me;
            let _ = gdb.send(gdb::Command::SetMe(me)).await;
            info!("I am player {me}");
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
                let _ = gdb.send(gdb::Command::SyncData(index as u8, sync_data.into())).await;
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
                Command::Connect { room } => return Box::pin(connect(&room, rx, &mut gdb.clone(), gui)).await, // Reconnect
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

    let _ = gdb.send(gdb::Command::SetMe(-1)).await; // TODO: use error codes for room full, clean disconnect, network error, etc.

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
