#![allow(unused)]

use std::ffi::CString;

use rmp::encode::{write_nil, write_str};
use tokio::net::TcpStream;
use tokio::io::{AsyncReadExt, AsyncWriteExt};
use tokio::sync::mpsc::{Sender, Receiver, error::TryRecvError};
use tokio::time::{sleep, Duration};
use thiserror::Error;
use log::{trace, debug, warn, info, error};

use crate::net;

const MESSAGE_SIZE: usize = 128;

pub async fn connect_and_retry(mut rx: Receiver<Command>, mut tx: Sender<net::Command>) {
    loop {
        match connect(&mut rx, &mut tx).await {
            Ok(()) | Err(Error::ConnectionClosed) => info!("connection closed cleanly"),
            Err(Error::Io(e)) if e.kind() == tokio::io::ErrorKind::ConnectionRefused => {
                warn!("connection refused, retrying in 5s");
                sleep(Duration::from_secs(5)).await;
            }
            Err(e) => {
                error!("{}, retrying in 5s", e);
                sleep(Duration::from_secs(1)).await;
            },
        }
        let _ = tx.send(net::Command::Disconnect).await;
    }
}

async fn connect(rx: &mut Receiver<Command>, tx: &mut Sender<net::Command>) -> Result<()> {
    let port = std::env::var("GDB_PORT").unwrap_or_else(|_| "9123".to_string());

    info!("connecting to gdb server on port {port}");
    let mut client = Client::new(&format!("[::1]:{port}")).await?;

    // Read online::comms::header
    let base = 0x80700000;
    let magic = client.read_cstr(base).await?;
    let out_message = client.read_u32(base + 0x20).await?; // Game's outgoing message (game -> client)
    let in_message = client.read_u32(base + 0x20 + 4).await?; // Game's incoming message (client -> game)
    let out_signal = client.read_u32(base + 0x20 + 4 * 2).await?; // Whether out_message is ready
    let in_signal = client.read_u32(base + 0x20 + 4 * 3).await?; // Whether in_message is ready
    debug!("online::comms::header::magic = {:?}", magic);

    if magic != c"PAPERMARIO-DX ONLINE".into() {
        return Err(Error::GameNotSupported);
    }
    if out_message & 0xFF000000 != 0x80000000 {
        error!("bad pointer: online::comms::header::out_message");
        return Err(Error::GameNotSupported);
    }
    if in_message & 0xFF000000 != 0x80000000 {
        error!("bad pointer: online::comms::header::in_message");
        return Err(Error::GameNotSupported);
    }

    let frame_duration = Duration::from_millis(1000 / 30);
    let latency = client.measure_latency().await?;
    if latency > frame_duration {
        warn!("high latency: {:?}", latency);
    }

    info!("connection with game established");
    let _ = tx.send(net::Command::Connect).await;

    // Every frame, exchange messages with the game
    let mut interval = tokio::time::interval(frame_duration);
    let mut in_signal_failures = 0;
    loop {
        interval.tick().await;

        trace!("frame");

        // Try to read message
        trace!("read out signal");
        let recv_size = client.read_u32(out_signal).await? as usize;
        if recv_size != 0 {
            if recv_size > MESSAGE_SIZE {
                return Err(Error::BadPacket(format!("message too large: {}", recv_size)));
            }
            trace!("out signal is set, reading message");
            // Read message and send it to net
            let mut recv_message = vec![0; recv_size];
            client.read_memory(out_message, &mut recv_message).await?;
            let _ = tx.send(net::Command::NewMessageFromGame(recv_message)).await;
            client.write_u32(out_signal, 0).await?;
        } else {
            trace!("no message from game");
        }

        trace!("read in signal");
        if client.read_u32(in_signal).await? != 0 {
            trace!("in_signal was not cleared. is game dead?");
            in_signal_failures += 1;
            if in_signal_failures > 30 * 5 { // 5 seconds
                error!("in_signal was not cleared for 5 seconds, assuming game is dead");
                return Err(Error::ConnectionClosed);
            }
            continue;
        }
        in_signal_failures = 0;

        // Check for messages to be sent, combining them into a single message
        let mut composite = vec![0xC0]; // nil
        let mut msg_count = 0usize;
        loop {
            match rx.try_recv() {
                Ok(Command::SendMesageToGame(msg)) => {
                    // Remove trailing nil terminator if there is one
                    if composite.last() == Some(&0xC0) {
                        composite.pop();
                    }

                    composite.extend_from_slice(&msg);
                    msg_count += 1;
                    break; // TEMP: only send one message per frame
                }
                Err(TryRecvError::Disconnected) => return Ok(()), // net thread is gone
                Err(TryRecvError::Empty) => break,
            }
        }

        assert!(composite.len() <= MESSAGE_SIZE);
        assert!(composite.last() == Some(&0xC0));

        if msg_count > 0 {
            debug!("sending {} messages to game: {:?}", msg_count, &composite);

            // Send message to game
            client.checked_write_memory(in_message, &composite).await?;
            client.write_u32(in_signal, 1).await?;
        }
    }
}

#[derive(Debug)]
pub enum Command {
    /// Write a msgpack message to the game. Make one of these with crate::rpc::Message::done().
    SendMesageToGame(Vec<u8>),
}

type Result<T> = std::result::Result<T, Error>;

/// A connection to a GDB server over the GDB Remote Serial Protocol. This is the client.
///
/// See https://sourceware.org/gdb/current/onlinedocs/gdb.html/Remote-Protocol.html
struct Client {
    stream: TcpStream,
}

// For an example of a GDB server, see https://github.com/ares-emulator/ares/tree/master/nall/gdb

#[derive(Error, Debug)]
enum Error {
    #[error(transparent)]
    Io(#[from] std::io::Error),

    #[error(transparent)]
    Utf8(#[from] std::str::Utf8Error),

    #[error("bad packet {0:?}")]
    BadPacket(String),

    #[error("bad cstring")]
    BadCString,

    #[error("connection closed")]
    ConnectionClosed,

    #[error("game is not Paper Mario DX with online support")]
    GameNotSupported,

    #[error("memory write failed")]
    WriteFailed,
}

impl Client {
    /// Connects to a GDB server at the given address.
    pub async fn new(address: &str) -> Result<Self> {
        let stream = TcpStream::connect(address).await?;
        //stream.set_nonblocking(true)?;
        let mut client = Self { stream };

        // Acknowledge the connection
        // https://github.com/ares-emulator/ares/blob/dd9c728a1277f586a663e415234f7b6b1c6dea55/nall/tcptext/tcptext-server.cpp#L18
        client.ack_recv().await?;

        // Start no ack mode; see https://sourceware.org/gdb/current/onlinedocs/gdb.html/Packet-Acknowledgment.html
        client.send_str("QStartNoAckMode").await?;
        client.ack_recv().await?;

        Ok(client)
    }

    async fn ack_recv(&mut self) -> Result<()> {
        self.stream.write_all(b"+").await?;
        self.stream.flush().await?;
        Ok(())
    }

    pub async fn wait_for_ok(&mut self) -> Result<()> {
        let mut ok = false;
        while !ok {
            let mut buffer = [0; 4096];
            let mut packet = Vec::new();

            loop {
                let bytes_read = self.stream.read(&mut buffer).await?;
                if bytes_read == 0 {
                    return Err(Error::ConnectionClosed);
                }

                self.stream.flush().await?; // We read it

                packet.extend_from_slice(&buffer[..bytes_read]);

                if packet.len() >= 3 && packet[packet.len() - 3] == b'#' {
                    break;
                }
            }

            // Ignore ack
            if packet[0] == b'+' {
                packet.remove(0);
            }

            // Ignore "OK" packets
            while packet.starts_with(b"$OK#9a") {
                packet.drain(..6);
                ok = true;
            }
            while packet.ends_with(b"$OK#9a") {
                packet.truncate(packet.len() - 6);
                ok = true;
            }

            if !packet.is_empty() {
                warn!("unexpected packet, expected OK: {:?}", std::str::from_utf8(&packet)?);
            }
        }
        Ok(())
    }

    /// Handles a single packet from the GDB server.
    pub async fn read_packet(&mut self) -> Result<String> {
        let mut buffer = [0; 4096];
        let mut packet = Vec::new();

        loop {
            let bytes_read = self.stream.read(&mut buffer).await?;
            if bytes_read == 0 {
                return Err(Error::ConnectionClosed);
            }

            self.stream.flush().await?; // We read it

            packet.extend_from_slice(&buffer[..bytes_read]);

            if packet.len() >= 3 && packet[packet.len() - 3] == b'#' {
                break;
            }
        }

        // Ignore ack
        if packet[0] == b'+' {
            packet.remove(0);
        }

        // Ignore "OK" packets
        while packet.starts_with(b"$OK#9a") {
            packet.drain(..6);
        }
        while packet.ends_with(b"$OK#9a") {
            packet.truncate(packet.len() - 6);
        }

        if packet.is_empty() {
            return Box::pin(self.read_packet()).await;
        }

        // Packets are in the form $...#..
        if packet[0] == b'$' {
            // Won't bother checking the checksum.

            // Remove the checksum
            let hash = packet.len() - 3; // #xx
            let packet = &packet[1..hash]; // 1 to skip the $

            let packet = std::str::from_utf8(packet)?.to_string();

            trace!("(gdb) <- {}", &packet);

            self.ack_recv().await?;

            Ok(packet.to_string())
        } else {
            let packet = std::str::from_utf8(&packet)?.to_string();
            Err(Error::BadPacket(packet))
        }
    }

    /// Writes a packet to the GDB server.
    // https://sourceware.org/gdb/current/onlinedocs/gdb.html/Packets.html
    async fn send(&mut self, packet: &[u8]) -> Result<()> {
        let checksum = packet
            .iter()
            .fold(0, |acc: u8, &byte| acc.wrapping_add(byte));

        self.stream.write_all(b"$").await?;
        self.stream.write_all(packet).await?;
        self.stream.write_all(b"#").await?;
        self.stream
            .write_all(format!("{:02}", checksum).as_bytes()).await?;
        self.stream.flush().await?;

        trace!("(gdb) -> {}", std::str::from_utf8(packet)?);

        Ok(())
    }

    async fn send_str(&mut self, packet: &str) -> Result<()> {
        self.send(packet.as_bytes()).await
    }

    // M addr,length:XX…
    pub async fn write_memory(&mut self, address: u32, data: &[u8]) -> Result<()> {
        let mut data_hex = String::with_capacity(data.len() * 2);
        for byte in data {
            data_hex.push_str(&format!("{:02X}", byte));
        }

        self.send_str(&format!("M{:08X},{}:{}", address, data.len(), data_hex)).await?;
        self.wait_for_ok().await
    }

    /// Some GDB stubs mess up memory writes sometimes, so we check the written data.
    pub async fn checked_write_memory(&mut self, address: u32, data: &[u8]) -> Result<()> {
        let mut attempts = 0;
        loop {
            self.write_memory(address, data).await?;

            let mut real = vec![0; data.len()];
            self.read_memory(address, &mut real).await?;

            if real == data {
                return Ok(());
            }

            if attempts >= 3 {
                error!("memory write failed after 3 attempts");
                return Err(Error::WriteFailed);
            }

            trace!("memory write failed, retrying");
            attempts += 1;
        }
    }

    pub async fn read_memory(&mut self, address: u32, data: &mut [u8]) -> Result<()> {
        self.send_str(&format!("m{:08X},{}", address, data.len())).await?;

        let response = self.read_packet().await?;
        if response.len() < data.len() * 2 {
            error!("response too short");
            return Err(Error::BadPacket(response));
        }
        // response is pairs of hex characters, each representing a byte
        for (i, byte) in response.as_bytes().chunks(2).enumerate() {
            if i >= data.len() {
                // response was too long, ignore the rest
                break;
            }

            data[i] = u8::from_str_radix(std::str::from_utf8(byte)?, 16)
                .map_err(|_| Error::BadPacket(response.clone()))?;
        }

        Ok(())
    }

    pub async fn read_cstr(&mut self, mut address: u32) -> Result<CString> {
        let mut buffer = Vec::new();
        loop {
            match self.read_u8(address).await? {
                0 => {
                    buffer.push(0);
                    break;
                },
                ch => buffer.push(ch),
            }
            address += 1;
        }
        CString::from_vec_with_nul(buffer).map_err(|_| Error::BadCString)
    }

    pub async fn read_u8(&mut self, address: u32) -> Result<u8> {
        let mut data = [0; 1];
        self.read_memory(address, &mut data).await?;
        Ok(data[0])
    }

    pub async fn read_u32(&mut self, address: u32) -> Result<u32> {
        let mut data = [0; 4];
        self.read_memory(address, &mut data).await?;
        Ok(u32::from_be_bytes(data))
    }

    pub async fn write_u32(&mut self, address: u32, value: u32) -> Result<()> {
        self.write_memory(address, &value.to_be_bytes()).await
    }

    pub async fn insert_breakpoint(&mut self, address: u32, kind: Breakpoint, size: u32) -> Result<()> {
        self.send_str(&format!("Z{},{:08X},{:X}", kind.as_int(), address, size)).await?;
        self.wait_for_ok().await
    }

    pub async fn measure_latency(&mut self) -> Result<Duration> {
        let start = std::time::Instant::now();
        self.send_str("qSupported").await?;
        self.read_packet().await?;
        Ok(start.elapsed())
    }
}

#[allow(unused)]
enum Breakpoint {
    Write,
    Read,
    Access,
}

impl Breakpoint {
    fn as_int(&self) -> u8 {
        match self {
            Breakpoint::Write => 2,
            Breakpoint::Read => 3,
            Breakpoint::Access => 4,
        }
    }
}
