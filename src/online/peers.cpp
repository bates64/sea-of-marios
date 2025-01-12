#include "online.h"
#include "sprite/npc/Bandit.h"
#include "sprite/player.h"
#include "dx/debug_menu.h"

namespace online::peers {

Peer peers[MAX_PEERS];
bool queueForceUpdate = false;

void Peer::connect(PeerId const& newId) {
    id = newId;
    connected = true;
    queueForceUpdate = true;
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
    if (npc == nullptr) return;
    npc->colliderPos = npc->pos = pos;
    set_npc_yaw(npc, yaw);
    npc->curAnim = anim;
    npc->flags |= NPC_FLAG_DIRTY_SHADOW;
}

void Peer::set_map(s16 mapID, s16 areaID) {
    //debug_printf("peer moved to %s", gAreas[areaID].maps[mapID].id);

    // Make npc if we're in the same map, otherwise destroy it
    if (mapID == gGameStatusPtr->mapID && areaID == gGameStatusPtr->areaID) {
        if (npc == nullptr) {
            NpcBlueprint bp = {
                .flags = 0,
                .initialAnim = ANIM_Bandit_Idle,
                .onUpdate = NULL,
                .onRender = NULL,
            };
            npc = get_npc_by_index(create_basic_npc(&bp));
        }

        // Reply to the peer with our current state
        queueForceUpdate = true;
    } else {
        if (npc != nullptr) {
            free_npc(npc);
            npc = nullptr;
        }
    }
}

Peer* Peer::get_ptr(PeerId const& id) {
    for (size_t i = 0; i < MAX_PEERS; i++) {
        if (peers[i].id == id && peers[i].is_connected()) return &peers[i];
    }
    return NULL;
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

void Peer::clear_npcs() {
    for (size_t i = 0; i < MAX_PEERS; i++) {
        if (peers[i].npc != nullptr) {
            // note: already freed
            peers[i].npc = nullptr;
        }
    }
}

// TODO: move this to a common header
bool operator==(Vec3f const& a, Vec3f const& b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

void send_rpcs(bool force) {
    // Map
    static s16 prevMapID, prevAreaID;
    if (force || prevMapID != gGameStatusPtr->mapID || prevAreaID != gGameStatusPtr->areaID) {
        prevMapID = gGameStatusPtr->mapID;
        prevAreaID = gGameStatusPtr->areaID;
        online::rpc::set_map(gGameStatusPtr->mapID, gGameStatusPtr->areaID);
    }

    // Player pos/yaw/anim
    static Vec3f prevPos = { 0.0f, -1000.0f, 0.0f };
    static f32 prevYaw = 0.0f;
    static AnimID prevAnim = ANIM_Bandit_Idle;
    AnimID anim = gPlayerStatus.anim == ANIM_Mario1_Idle ? ANIM_Bandit_Idle : ANIM_Bandit_Run; // TODO: map more anims
    if (force || gPlayerStatus.pos != prevPos || gPlayerStatus.curYaw != prevYaw || anim != prevAnim) {
        prevPos = gPlayerStatus.pos;
        prevYaw = gPlayerStatus.curYaw;
        prevAnim = gPlayerStatus.anim;
        online::rpc::set_player_state(gPlayerStatus.pos, gPlayerStatus.curYaw, gPlayerStatus.anim);
    }
}

void update() {
    static bool needsInit = true;
    if (needsInit) {
        bzero(peers, sizeof(peers));
        needsInit = false;
    }

    s32 connectedPeers = 0;
    for (size_t i = 0; i < MAX_PEERS; i++) {
        if (peers[i].is_connected()) {
            peers[i].update();
            connectedPeers++;
        }
    }
    debug_printf("connected peers: %d", connectedPeers);

    bool forceUpdate = queueForceUpdate;
    queueForceUpdate = true; // TEMP

    if (forceUpdate || connectedPeers > 0) {
        send_rpcs(forceUpdate);
    }
}

}
