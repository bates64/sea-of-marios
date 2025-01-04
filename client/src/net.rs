use futures::{select, FutureExt};
use futures_timer::Delay;
use log::{debug, info, warn};
use matchbox_socket::{Error as SocketError, PeerId, PeerState, WebRtcSocket};
use std::time::Duration;
use tokio::sync::mpsc::{Sender, Receiver};
use crate::gdb;
use crate::rpc::Message;

const CHANNEL_ID: usize = 0;

/// Commands from gdb to net
pub enum Command {
    Connect,
    Disconnect,
    NewMessageFromGame(Vec<u8>),
}

pub async fn connect_and_retry(mut rx: Receiver<Command>, mut tx: Sender<gdb::Command>) {
    while let Some(command) = rx.recv().await {
        if let Command::Connect = command {
            connect(&mut rx, &mut tx).await;
        }
    }
}

async fn connect(rx: &mut Receiver<Command>, tx: &mut Sender<gdb::Command>) {
    info!("Connecting to signaling server...");
    let (mut socket, loop_fut) = WebRtcSocket::new_reliable("ws://localhost:3536/");

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
    let id = loop {
        if let Some(id) = socket.id() {
            break id;
        }
        select! {
            _ = (&mut timeout).fuse() => (),
            _ = &mut loop_fut => return,
        }
    };
    info!("Connected! Assigned id: {id}");

    // Let game know our id
    let mut message = Message::new();
    message.set_peer_id(id);
    let _ = tx.send(gdb::Command::SendMesageToGame(message.done())).await;

    'main: loop {
        // Handle any new peers
        for (peer, state) in socket.update_peers() {
            match state {
                PeerState::Connected => {
                    info!("Peer joined: {peer}");

                    // Let game know this peer connected
                    let mut message = Message::new();
                    message.peer_connected(peer);
                    let _ = tx.send(gdb::Command::SendMesageToGame(message.done())).await;

                    // Send new peer a peer_connected message so they know we're here
                    let mut message = Message::new();
                    message.peer_connected(id);
                    socket.channel_mut(CHANNEL_ID).send(message.done().into(), peer);
                }
                PeerState::Disconnected => {
                    info!("Peer left: {peer}");

                    // Let game know this peer disconnected
                    let mut message = Message::new();
                    message.peer_disconnected(peer);
                    let _ = tx.send(gdb::Command::SendMesageToGame(message.done())).await;
                }
            }
        }

        // Accept any messages incoming from peers
        for (_peer, message) in socket.channel_mut(CHANNEL_ID).receive() {
            let _ = tx.send(gdb::Command::SendMesageToGame(message.into())).await;
        }

        // Handle any new messages from game over gdb
        while let Ok(command) = rx.try_recv() {
            match command {
                Command::NewMessageFromGame(message) => {
                    // Broadcast message to all peers
                    let peers: Vec<PeerId> = socket.connected_peers().collect();
                    let channel = socket.channel_mut(CHANNEL_ID);
                    debug!("num peers: {}", peers.len());
                    for peer in peers {
                        //channel.send(message.as_slice().into(), peer);
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
