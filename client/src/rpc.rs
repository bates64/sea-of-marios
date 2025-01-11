use matchbox_socket::PeerId;
use rmp::encode::*;

// enum class Procedure
const PEER_CONNECTED: u8 = 1;
const PEER_DISCONNECTED: u8 = 2;
const SET_PEER_ID: u8 = 3;
const DBG_PRINT: u8 = 4;

#[derive(Debug)]
pub struct Message {
    buf: ByteBuf,
}

impl Message {
    pub fn new() -> Self {
        Message { buf: ByteBuf::new() }
    }

    pub fn done(mut self) -> Vec<u8> {
        write_nil(&mut self.buf).unwrap();
        self.buf.into_vec()
    }

    pub fn peer_connected(&mut self, peer: PeerId) -> &mut Self {
        let (most_significant, least_significant) = peer.0.as_u64_pair();
        write_uint8(&mut self.buf, PEER_CONNECTED).unwrap();
        write_uint(&mut self.buf, most_significant).unwrap();
        write_uint(&mut self.buf, least_significant).unwrap();
        self
    }

    pub fn peer_disconnected(&mut self, peer: PeerId) -> &mut Self {
        let (most_significant, least_significant) = peer.0.as_u64_pair();
        write_uint8(&mut self.buf, PEER_DISCONNECTED).unwrap();
        write_uint(&mut self.buf, most_significant).unwrap();
        write_uint(&mut self.buf, least_significant).unwrap();
        self
    }

    /// An instruction to our own game so it knows what its own peer ID is
    pub fn set_peer_id(&mut self, peer: PeerId) -> &mut Self {
        let (most_significant, least_significant) = peer.0.as_u64_pair();
        write_uint8(&mut self.buf, SET_PEER_ID).unwrap();
        write_uint(&mut self.buf, most_significant).unwrap();
        write_uint(&mut self.buf, least_significant).unwrap();
        self
    }

    pub fn dbg_print(&mut self, message: &str) -> &mut Self {
        write_uint8(&mut self.buf, DBG_PRINT).unwrap();
        write_str(&mut self.buf, message).unwrap();
        self
    }
}
