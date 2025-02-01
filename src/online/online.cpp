#include "common.h"
#include "dx/debug_menu.h"
#include "online.h"
#include "sprite/npc/Bandit.h"

namespace online {

struct Header {
    char magic[0x20];
    s32 me; // index of syncData that this client owns
    bool isMySyncDataValid; // if false, GDB wont read syncData[me]
    size_t sizeof_syncData;
    volatile SyncData syncData[MAX_PEERS];
};

// .text section is linked first, so header will be placed at the start of this segment
[[gnu::section(".text")]] Header header = {
    "PAPERMARIO-DX ONLINE",
    -1,
    false,
    sizeof(SyncData),
    {0},
};

SyncData* begin_writing_my_sync_data() {
    if (header.me < 0) return nullptr;

    debug_printf("this is player %d", header.me);

    header.isMySyncDataValid = false;
    return (SyncData*) &header.syncData[header.me]; // cast required to make non-volatile
}

void end_writing_my_sync_data(SyncData* sync) {
    if (sync == nullptr) return;
    sync->checksum = sync->calc_checksum();
    header.isMySyncDataValid = true;
}

struct PeerInfo {
    u16 frameCounter;
    u16 timeSinceRecv;

    bool is_connected() const {
        return timeSinceRecv < 60;
    }
} peerInfo[MAX_PEERS] = {0};

Npc* npcs[MAX_PEERS] = {0};

void receive_data() {
    osInvalDCache(&header, sizeof(header)); // possibly unnecessary

    for (size_t i = 0; i < MAX_PEERS; i++) {
        auto& sync = header.syncData[i];
        auto& info = peerInfo[i];

        if (info.timeSinceRecv < /* max u16 */ 0xFFFF) {
            info.timeSinceRecv++;
        }

        if (i == header.me) continue; // skip our own data

        if (sync.frameCounter == 0) continue; // no peer (they disconnected)

        if (!sync.is_checksum_valid()) {
            // either:
            // - data was corrupted during transmission
            // - gdb is in the middle of writing to this data
            debug_printf("checksum invalid for player %d", i);
            continue;
        }

        // Treat frameCounter like a nonce: if it hasn't changed, we don't need to read the data
        if (sync.frameCounter == info.frameCounter) {
            // no new data
            continue;
        }
        if (sync.frameCounter < info.frameCounter) {
            // data is out of order
            continue;
        }
        info.frameCounter = sync.frameCounter;
        info.timeSinceRecv = 0;

        // Create NPC if we're in the same map, otherwise free it
        if (sync.area == gGameStatus.areaID && sync.map == gGameStatus.mapID) {
            if (npcs[i] == nullptr) {
                NpcBlueprint bp = {
                    .flags = 0,
                    .initialAnim = ANIM_Bandit_Idle,
                    .onUpdate = NULL,
                    .onRender = NULL,
                };
                npcs[i] = get_npc_by_index(create_basic_npc(&bp));
            }

            // Update NPC state to match
            npcs[i]->colliderPos = npcs[i]->pos = { sync.player.x, sync.player.y, sync.player.z };
            set_npc_yaw(npcs[i], sync.player.yaw);
            npcs[i]->flags |= NPC_FLAG_DIRTY_SHADOW;
        } else if (npcs[i] != nullptr) {
            free_npc(npcs[i]);
            npcs[i] = nullptr;
        }
    }

    // Free NPCs of disconnected peers
    for (size_t i = 0; i < MAX_PEERS; i++) {
        if (!peerInfo[i].is_connected() && npcs[i] != nullptr) {
            free_npc(npcs[i]);
            npcs[i] = nullptr;
        }
    }
}

EXTERN_C void online_begin_step_game_loop() {
    receive_data();
}

// at end of game loop, update our sync data
EXTERN_C void online_end_step_game_loop() {
    auto* sync = begin_writing_my_sync_data();
    if (sync == nullptr) {
        debug_printf("disconnected");
        return;
    }

    sync->frameCounter = gGameStatus.frameCounter;
    sync->player = { gPlayerStatus.pos.x, gPlayerStatus.pos.y, gPlayerStatus.pos.z, gPlayerStatus.curYaw };
    sync->area = gGameStatus.areaID;
    sync->map = gGameStatus.mapID;

    end_writing_my_sync_data(sync);
}

EXTERN_C void online_on_clear_npcs() {
    for (size_t i = 0; i < MAX_PEERS; i++) {
        npcs[i] = nullptr;
    }
}

bool is_connected_to_gateway() {
    return true; // TODO: add some kind of check to see if gdb is connected
}

bool is_connected_to_server() {
    // TODO: make this check for room, not peers
    for (size_t i = 0; i < MAX_PEERS; i++) {
        if (i == header.me) continue;
        if (peerInfo[i].is_connected()) {
            return true;
        }
    }
    return false;
}

}; // namespace online
