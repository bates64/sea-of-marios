#![allow(unused)]

use std::ffi::CString;

use tokio::net::{TcpListener, TcpStream};
use tokio::io::{AsyncReadExt, AsyncWriteExt};
use tokio::select;
use tokio::sync::mpsc::{Sender, Receiver, error::TryRecvError};
use tokio::time::{sleep, Duration};
use thiserror::Error;
use log::{trace, debug, warn, info, error};

use crate::net;

const MESSAGE_SIZE: usize = 128;

pub async fn connect_and_retry(mut rx: Receiver<Command>, mut tx: Sender<net::Command>) {
    let gdb_port = std::env::var("GDB_PORT").unwrap_or_else(|_| "9123".to_string());
    let gdb_addr = format!("[::1]:{}", gdb_port);

    loop {
        let result = /*select! {
            client = Gdb::new(&gdb_addr) => {
                match client {
                    Ok(client) => handle_client(client, &mut rx, &mut tx).await,
                    Err(error) => Err(error),
                }
            },
            client = Project64::new("127.0.0.1:65432") => {
                match client {
                    Ok(client) => handle_client(client, &mut rx, &mut tx).await,
                    Err(error) => Err(error),
                }
            },
        };*/ {
            let client = Project64::new("127.0.0.1:65432").await;
            match client {
                Ok(client) => handle_client(client, &mut rx, &mut tx).await,
                Err(error) => Err(error),
            }
        };
        match result {
            Ok(()) | Err(Error::ConnectionClosed) => info!("connection closed cleanly"),
            Err(e) => {
                error!("{}, retrying in 5s", e);
                sleep(Duration::from_secs(1)).await;
            },
        }
        let _ = tx.send(net::Command::Disconnect).await;
    }
}

async fn handle_client<T: Client>(mut client: T, rx: &mut Receiver<Command>, tx: &mut Sender<net::Command>) -> Result<()> {
    trace!("handle_client");

    // Read online::comms::header
    let base = 0x80700000;
    let magic = client.read_cstr(base).await?;
    let ptr_me = base + 0x20;
    let ptr_is_my_sync_data_valid = base + 0x20 + 4;
    let sizeof_sync_data = client.read_u32(base + 0x20 + 4 * 2).await?;
    let ptr_sync_data = base + 0x20 + 4 * 3;
    debug!("online::comms::header::magic = {:?}", magic);
    debug!("sizeof(SyncData) = {}", sizeof_sync_data);

    if magic != c"PAPERMARIO-DX ONLINE".into() {
        return Err(Error::GameNotSupported);
    }

    let frame_duration = Duration::from_millis(1000 / 30);

    info!("connection with game established");
    let _ = tx.send(net::Command::Connect).await;

    let mut me = 99u8;

    // Every frame, exchange messages with the game
    let mut interval = tokio::time::interval(frame_duration);
    loop {
        interval.tick().await;

        trace!("frame");

        // Read syncData[me]
        if (0..16).contains(&me) {
            let is_valid = client.read_u32(ptr_is_my_sync_data_valid).await? as usize;
            if is_valid != 0 {
                // Read syncData and send it to net
                let mut data = vec![0; sizeof_sync_data as usize];
                client.read_memory(ptr_sync_data + (me as u32) * sizeof_sync_data, &mut data).await?;
                let _ = tx.send(net::Command::SyncData(data)).await;
            } else {
                trace!("isMySyncDataValid = false");
            }
        }

        loop {
            match rx.try_recv() {
                Ok(Command::SetMe(new_me)) => {
                    me = new_me;
                    client.write_u32(ptr_me, new_me as u32).await?;
                }
                Ok(Command::SyncData(index, data)) => {
                    if data.len() != sizeof_sync_data as usize {
                        warn!("sync data size mismatch");
                        continue;
                    }
                    client.write_memory(ptr_sync_data + (index as u32) * sizeof_sync_data, &data).await?;
                }
                Err(TryRecvError::Disconnected) => return Ok(()), // net thread is gone
                Err(TryRecvError::Empty) => break,
            }
        }
    }
}

#[derive(Debug)]
pub enum Command {
    /// Set Header::me
    SetMe(u8),

    /// Set Header::syncData[peer]
    SyncData(u8, Vec<u8>),
}

type Result<T> = std::result::Result<T, Error>;

/// A connection to a GDB server over the GDB Remote Serial Protocol. This is the client.
///
/// See https://sourceware.org/gdb/current/onlinedocs/gdb.html/Remote-Protocol.html
struct Gdb {
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

trait Client {
    async fn read_memory(&mut self, address: u32, data: &mut [u8]) -> Result<()>;

    async fn write_memory(&mut self, address: u32, data: &[u8]) -> Result<()>;

    /// Some clients (old ares) can mess up memory writes, so we check the written data.
    async fn checked_write_memory(&mut self, address: u32, data: &[u8]) -> Result<()> {
        let mut attempts = 0;
        loop {
            self.write_memory(address, data).await?;

            let mut real = vec![0; data.len()];
            self.read_memory(address, &mut real).await?;

            if real == data {
                return Ok(());
            }

            if attempts >= 10 {
                error!("memory write failed after 10 attempts");
                return Err(Error::WriteFailed);
            }

            trace!("memory write failed, retrying");
            attempts += 1;
        }
    }

    async fn read_cstr(&mut self, mut address: u32) -> Result<CString> {
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

    async fn read_u8(&mut self, address: u32) -> Result<u8> {
        let mut data = [0; 1];
        self.read_memory(address, &mut data).await?;
        Ok(data[0])
    }

    async fn read_u32(&mut self, address: u32) -> Result<u32> {
        let mut data = [0; 4];
        self.read_memory(address, &mut data).await?;
        Ok(u32::from_be_bytes(data))
    }

    async fn write_u32(&mut self, address: u32, value: u32) -> Result<()> {
        self.write_memory(address, &value.to_be_bytes()).await
    }
}

impl Gdb {
    /// Connects to a GDB server at the given address.
    pub async fn new(address: &str) -> Result<Self> {
        let stream = loop {
            match TcpStream::connect(address).await {
                Ok(stream) => break stream,
                Err(e) if e.kind() == tokio::io::ErrorKind::ConnectionRefused => {
                    sleep(Duration::from_secs(1)).await;
                },
                Err(e) => return Err(e.into()),
            }
        };

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

impl Client for Gdb {
    // M addr,length:XX…
    async fn write_memory(&mut self, address: u32, data: &[u8]) -> Result<()> {
        let mut data_hex = String::with_capacity(data.len() * 2);
        for byte in data {
            data_hex.push_str(&format!("{:02X}", byte));
        }

        self.send_str(&format!("M{:08X},{}:{}", address, data.len(), data_hex)).await?;
        self.wait_for_ok().await
    }

    async fn read_memory(&mut self, address: u32, data: &mut [u8]) -> Result<()> {
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

pub struct Project64 {
    listener: TcpListener,
    stream: TcpStream,
}

const COMMAND_READ_MEMORY: u8 = 0;
const COMMAND_WRITE_MEMORY: u8 = 1;

impl Project64 {
    async fn new(address: &str) -> Result<Self> {
        let listener = TcpListener::bind(address).await?;
        let (stream, _) = listener.accept().await?;
        trace!("connected to Project64");
        Ok(Self { listener, stream })
    }

    async fn send(&mut self, command: u8, address: u32, data: &[u8]) -> Result<()> {
        // We have to write to an in-memory buffer because the client expects the head of the packet to be
        // send in a single TCP packet, not spread across multiple (which can happen if we don't write in a
        // single `stream.write_all` operation).
        let mut buffer = Vec::new();
        buffer.push(command);
        buffer.extend_from_slice(&address.to_be_bytes());
        buffer.extend_from_slice(&(data.len() as u32).to_be_bytes());
        buffer.extend_from_slice(data);

        trace!("(pj64) -> command={}, address={:08X}, len={}, data={:?}", command, address, data.len(), data);

        self.stream.write_all(&buffer).await?;
        self.stream.flush().await?;
        Ok(())
    }

    async fn recv(&mut self) -> Result<(u8, u32, Vec<u8>)> {
        let mut command = [0; 1];
        self.stream.read_exact(&mut command).await?;
        let command = command[0];

        let mut address = [0; 4];
        self.stream.read_exact(&mut address).await?;
        let address = u32::from_be_bytes(address);

        let mut len = [0; 4];
        self.stream.read_exact(&mut len).await?;
        let len = u32::from_be_bytes(len) as usize;

        let mut data = vec![0; len];
        self.stream.read_exact(&mut data).await?;

        trace!("(pj64) <- command={}, address={:08X}, len={}, data={:?}", command, address, len, data);

        Ok((command, address, data))
    }
}

impl Client for Project64 {
    async fn read_memory(&mut self, address: u32, data: &mut [u8]) -> Result<()> {
        trace!("pj64 read_memory");
        self.send(COMMAND_READ_MEMORY, address, &(data.len() as u32).to_be_bytes()).await?;
        let (command, read_address, read_data) = self.recv().await?;
        if command != COMMAND_READ_MEMORY {
            return Err(Error::BadPacket(format!("expected command {}, got {}", COMMAND_READ_MEMORY, command)));
        }
        if read_address != address {
            return Err(Error::BadPacket(format!("expected address {:08X}, got {:08X}", address, read_address)));
        }
        data.copy_from_slice(&read_data);
        Ok(())
    }

    async fn write_memory(&mut self, address: u32, data: &[u8]) -> Result<()> {
        trace!("pj64 write_memory");
        self.send(COMMAND_WRITE_MEMORY, address, data).await?;
        Ok(())
    }
}
