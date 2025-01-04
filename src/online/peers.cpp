#include "online.h"
#include "sprite/npc/Bandit.h"
#include "dx/debug_menu.h"

// Destructor support
// not handling this properly yet, but we should do it eventually or figure out libc++
void* __dso_handle = NULL;
extern "C" int __cxa_atexit(void (*func)(void*), void* arg, void* dso_handle) {
    // No-op implementation: do nothing and return success
    return 0;
}

namespace online::peers {

Vec<Peer> peers;

Peer::Peer(PeerId id) : id(id) {
    debug_printf("creating Peer");
    NpcBlueprint bp = {
        .flags = 0,
        .initialAnim = ANIM_Bandit_Idle,
        .onUpdate = NULL,
        .onRender = NULL,
    };
    npc = get_npc_by_index(create_basic_npc(&bp)); // TODO: when changing map, recreate npc, or add a flag to npcs to persist across maps
}

void Peer::free() {
    debug_printf("deleting Peer");
    if (npc != nullptr) free_npc(npc);
    npc = nullptr;
}

void Peer::update() {
    debug_printf("updating Peer %p", npc);
    //timeout += 1;
    if (timeout > 60) {
        free();
        peers.remove(*this);
    }
}

void Peer::move(Vec3f const& pos, f32 yaw, AnimID anim) {
    timeout = 0;
    npc->pos = pos;
    npc->yaw = yaw;
    npc->curAnim = anim;
}

Peer& Peer::get_by_id(PeerId id) {
    for (Peer& p : peers) {
        if (p.id == id) {
            return p;
        }
    }
    return peers.push(Peer(id));
}

void update() {
    static bool needsInit = true;
    if (needsInit) {
        peers = Vec<Peer>();
        needsInit = false;
    }

    online::rpc::set_player_state(gPlayerStatus.pos, gPlayerStatus.curYaw, gPlayerStatus.anim);

    debug_printf("peers: %d", peers.len());
    for (Peer& peer : peers) {
        peer.update();
    }
}

}
