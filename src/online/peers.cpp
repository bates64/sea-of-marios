#include "online.h"
#include "sprite/npc/Bandit.h"
#include "dx/debug_menu.h"

namespace online::peers {

Peer peers[MAX_PEERS];

void Peer::connect(PeerId const& newId) {
    id = newId;
    connected = true;

    NpcBlueprint bp = {
        .flags = 0,
        .initialAnim = ANIM_Bandit_Idle,
        .onUpdate = NULL,
        .onRender = NULL,
    };
    npc = get_npc_by_index(create_basic_npc(&bp)); // TODO: when changing map, recreate npc, or add a flag to npcs to persist across maps
}

void Peer::disconnect() {
    connected = false;

    if (npc != nullptr) free_npc(npc);
    npc = nullptr;
}

void Peer::update() {
    if (!connected) return;
}

void Peer::move(Vec3f const& pos, f32 yaw, AnimID anim) {
    npc->pos = pos;
    npc->yaw = yaw;
    npc->curAnim = anim;
}

Peer& Peer::get_mut(PeerId const& id) {
    for (size_t i = 0; i < MAX_PEERS; i++) {
        if (peers[i].id == id && peers[i].is_connected()) return peers[i];
    }

    u64 most, least;
    id.lld_pair(most, least);
    PANIC_MSG("peer not found: %016llx-%016llx", most, least);
}

Peer const& Peer::get(PeerId const& id) {
    return get_mut(id);
}

Peer& Peer::upsert_mut(PeerId const& id) {
    for (size_t i = 0; i < MAX_PEERS; i++) {
        if (peers[i].id == id && peers[i].is_connected()) return peers[i];
    }

    // Not found, find a disconnected peer to replace
    for (size_t i = 0; i < MAX_PEERS; i++) {
        if (!peers[i].is_connected()) {
            peers[i].connect(id);
            return peers[i];
        }
    }
    PANIC_MSG("max peers connected, refusing to add another");
}

void Peer::disconnect_all() {
    for (size_t i = 0; i < MAX_PEERS; i++) {
        peers[i].disconnect();
    }
}

void update() {
    static bool needsInit = true;
    if (needsInit) {
        bzero(peers, sizeof(peers));
        needsInit = false;
    }

    online::rpc::set_player_state(gPlayerStatus.pos, gPlayerStatus.curYaw, gPlayerStatus.anim);

    s32 connectedPeers = 0;
    for (size_t i = 0; i < MAX_PEERS; i++) {
        if (peers[i].is_connected()) {
            peers[i].update();
            connectedPeers++;
            peers[i].id.print();
        }
    }
    debug_printf("connected peers: %d", connectedPeers);
}

}
