#include "online.h"
#include "dx/debug_menu.h"

namespace online::rpc {

enum class Procedure : u8 {
    nop,
    peer_connected,
    peer_disconnected,
    set_peer_id,
    set_player_state,
};

void read::mpack_read_rpc(mpack_reader_t* reader) {
    Procedure procedure = (Procedure)mpack_expect_u8(reader);
    switch (procedure) {
        case Procedure::nop: return;
        case Procedure::peer_connected: return peer_connected(reader);
        case Procedure::peer_disconnected: return peer_disconnected(reader);
        case Procedure::set_peer_id: return set_peer_id(reader);
        case Procedure::set_player_state: return set_player_state(reader);
        default: debug_printf("unknown RPC procedure: %d", (u8)procedure);
    }
}

void read::peer_connected(mpack_reader_t* reader) {
    PeerId peer(mpack_expect_u64(reader), mpack_expect_u64(reader));
    peers::Peer::get_by_id(peer);
}

void read::peer_disconnected(mpack_reader_t* reader) {
    PeerId peer(mpack_expect_u64(reader), mpack_expect_u64(reader));
    auto& p = peers::Peer::get_by_id(peer);
    p.free();
    peers::remove(p);
}

void read::set_peer_id(mpack_reader_t* reader) {
    auto& message = online::comms::get_message();
    message.set_peer_id(PeerId(reader));
    debug_printf("got peer id");
    message.peer_id().print();
}

void set_player_state(Vec3f const& pos, f32 yaw, AnimID anim) {
    auto& message = online::comms::get_message();
    mpack_write_u8(&message.writer, (u8)Procedure::set_player_state);
    message.peer_id().mpack_write(&message.writer);
    mpack_write_float(&message.writer, pos.x);
    mpack_write_float(&message.writer, pos.y);
    mpack_write_float(&message.writer, pos.z);
    mpack_write_float(&message.writer, yaw);
    mpack_write_u32(&message.writer, anim);
}

void read::set_player_state(mpack_reader_t* reader) {
    PeerId peer(reader);
    f32 x = mpack_expect_float(reader);
    f32 y = mpack_expect_float(reader);
    f32 z = mpack_expect_float(reader);
    f32 yaw = mpack_expect_float(reader);
    AnimID anim = mpack_expect_u32(reader);
    peers::Peer::get_by_id(peer).move({x, y, z}, yaw, anim);
}

}; // namespace online::rpc
