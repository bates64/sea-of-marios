use std::ffi::CString;

use tokio::net::TcpStream;
use tokio::io::{AsyncReadExt, AsyncWriteExt};
use tokio::sync::mpsc::{Sender, Receiver, error::TryRecvError};
use tokio::time::{sleep, Duration};
use thiserror::Error;
use log::{trace, debug, warn, info, error};

use crate::net;

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
    }
}

async fn connect(rx: &mut Receiver<Command>, tx: &mut Sender<net::Command>) -> Result<()> {
    let port = std::env::var("GDB_PORT").unwrap_or_else(|_| "9123".to_string());

    info!("connecting to gdb server on port {port}");
    let mut client = Client::new(&format!("[::1]:{port}")).await?;

    // Read online::comms::header
    let base = 0x80700000;
    let magic = client.read_cstr(base).await?;
    let write_signal_addr = client.read_u32(base + 0x20).await?;
    let message_addr = client.read_u32(base + 0x20 + 4).await?;
    debug!("online::comms::header::magic = {:?}", magic);
    debug!("online::comms::header::message = {:08X}", message_addr);

    if magic != c"PAPERMARIO-DX ONLINE".into() {
        return Err(Error::GameNotSupported);
    }
    if message_addr & 0xFF000000 != 0x80000000 {
        error!("bad pointer: online::comms::header::message");
        return Err(Error::GameNotSupported);
    }

    // When the game writes to this address, it is signaling that it's ready to exchange messages
    client.insert_breakpoint(write_signal_addr, Breakpoint::Write, 4).await?;

    info!("connection with game established");
    let _ = tx.send(net::Command::Connect).await;

    loop {
        // Check for breakpoint hit
        if let Ok(packet) = client.read_packet().await {
            if packet.starts_with("T05") {
                let message_length = client.read_u32(message_addr).await?;
                if message_length > 0 {
                    // Read message and send it to net
                    let mut recv_message = vec![0; message_length as usize];
                    client.read_memory(message_addr + 4, &mut recv_message).await?;
                    let _ = tx.send(net::Command::NewMessageFromGame(recv_message)).await;

                    // Check for messages to be sent, combining them into a single message
                    let mut composite = vec![0xC0]; // Send a nil if there are no messages, so the game doesn't read its own messages
                    loop {
                        match rx.try_recv() {
                            Ok(Command::SendMesageToGame(msg)) => {
                                // Remove trailing nil terminator if there is one
                                if composite.last() == Some(&0xC0) {
                                    composite.pop();
                                }

                                composite.extend_from_slice(&msg);
                            }
                            Err(TryRecvError::Disconnected) => return Ok(()), // net thread is gone
                            Err(TryRecvError::Empty) => break,
                        }
                    }

                    if composite.len() > 1 {
                        debug!("sending message to game: {:?}", &composite);
                    }

                    // Align composite to 4 bytes
                    while composite.len() % 4 != 0 {
                        composite.push(0);
                    }

                    // Send message to game
                    //client.write_memory(message_addr + 4, &composite).await?;
                    // ares is being buggy(?) so we'll write in 4 byte chunks instead
                    for (i, chunk) in composite.chunks(4).enumerate() {
                        client.write_memory(message_addr + 4 + (i as u32 * 4), chunk).await?;
                    }
                    #[cfg(debug_assertions)]
                    {
                        // Sanity check
                        let mut sanity_check = vec![0; composite.len()];
                        client.read_memory(message_addr + 4, &mut sanity_check).await?;
                        if composite != sanity_check {
                            error!("message was not written correctly");
                        }
                    }

                    // Reset message length to tell game that GDB has read the message
                    client.write_u32(message_addr, 0).await?;
                }

                client.send_str("c").await?; // Continue
            }
        }

        tokio::task::yield_now().await;
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
