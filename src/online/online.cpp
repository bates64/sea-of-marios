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

    header.isMySyncDataValid = false;
    return (SyncData*) &header.syncData[header.me]; // cast required to make non-volatile
}

void end_writing_my_sync_data(SyncData* sync) {
    if (sync == nullptr) return;
    sync->checksum = sync->calc_checksum();
    header.isMySyncDataValid = true;
}

class MovingAverage {
    size_t index = 0;
    f32 values[6]; // 200ms @ 30fps

public:
    void set(f32 value) {
        for (size_t i = 0; i < ARRAY_COUNT(values); i++) {
            values[i] = value;
        }
        index = 0;
    }

    void push(f32 value) {
        values[index] = value;
        index = (index + 1) % ARRAY_COUNT(values);
    }

    f32 average() const {
        f32 sum = 0.0f;
        for (size_t i = 0; i < ARRAY_COUNT(values); i++) {
            sum += values[i];
        }
        return sum / (f32)ARRAY_COUNT(values);
    }

    f32 velocity() const {
        f32 diff = 0.0f;
        for (size_t i = 1; i < ARRAY_COUNT(values); i++) {
            diff += values[i] - values[i-1];
        }
        return diff / (f32)(ARRAY_COUNT(values) - 1);
    }
};

struct PeerInfo {
    u16 frameCounter; // TODO: detect save state cheating by massive changes in frameCounter
    u16 timeSinceRecv;
    u16 droppedPackets;
    u32 connectionTime;

    // Prediction data
    MovingAverage x;
    MovingAverage y;
    MovingAverage z;

    bool is_connected() const {
        return timeSinceRecv < 60;
    }
} peerInfo[MAX_PEERS] = {0};

Npc* npcs[MAX_PEERS] = {0};

extern "C" s32 create_npc_with_index(NpcBlueprint* blueprint, AnimID* animList, s32 isPeachNpc, s32 i);

void receive_data() {
    osInvalDCache(&header, sizeof(header)); // possibly unnecessary

    for (size_t i = 0; i < MAX_PEERS; i++) {
        auto& sync = header.syncData[i];
        auto& info = peerInfo[i];

        if (info.timeSinceRecv < /* max u16 */ 0xFFFF) {
            info.timeSinceRecv++;
        }

        if (i == (size_t)header.me) continue; // skip our own data

        if (sync.frameCounter == 0) continue; // no peer (they disconnected)

        // Check for dropped/corrupted packet
        // Treat frameCounter like a nonce: if it hasn't changed, we don't need to read the data
        if (!sync.is_checksum_valid() || sync.frameCounter == info.frameCounter) {
            dropped:
            info.droppedPackets++;

            // Prediction
            if (npcs[i] != nullptr) {
                // TODO: use collision detection to prevent clipping through walls or the floor
                npcs[i]->colliderPos.x += info.x.velocity();
                npcs[i]->colliderPos.y += info.y.velocity();
                npcs[i]->colliderPos.z += info.z.velocity();
                npcs[i]->colliderPos = npcs[i]->pos;
                npcs[i]->flags |= NPC_FLAG_DIRTY_SHADOW;
            }
            continue;
        }

        // Enforce strict ordering (UDP does not)
        if (sync.frameCounter < info.frameCounter) {
            debug_printf("recv data out of order for player %d", i);
            goto dropped;
        }
        info.frameCounter = sync.frameCounter;
        info.timeSinceRecv = 0;

        if (info.connectionTime == 0) {
            debug_printf("player %d connected", i);
            info.droppedPackets = 0;
            info.x.set(sync.player.x);
            info.y.set(sync.player.y);
            info.z.set(sync.player.z);
        } else {
            info.x.push(sync.player.x);
            info.y.push(sync.player.y);
            info.z.push(sync.player.z);
        }

        if (info.droppedPackets > 0) info.droppedPackets--;

        // Create NPC if we're in the same map, otherwise free it
        if (sync.area == gGameStatus.areaID && sync.map == gGameStatus.mapID) {
            if (npcs[i] == nullptr) {
                // Wait until we're in the world to create NPCs
                if (gGameStatusPtr->context != CONTEXT_WORLD) {
                    continue;
                }

                NpcBlueprint bp = {
                    .flags = 0,
                    .initialAnim = sync.anim,
                    .onUpdate = NULL,
                    .onRender = NULL,
                };
                npcs[i] = get_npc_by_index(create_npc_with_index(&bp, NULL, FALSE, MAX_NPCS - 1 - i));
                npcs[i]->pos = { sync.player.x, sync.player.y, sync.player.z };

                // They just entered the map, discard position history
                info.x.set(sync.player.x);
                info.y.set(sync.player.y);
                info.z.set(sync.player.z);
            }

            // Update NPC state to match
            if (info.droppedPackets > 0) {
                // Packets were dropped - smoothly interpolate to the correct position
                npcs[i]->pos.x += (sync.player.x - npcs[i]->pos.x) / 3.0f;
                npcs[i]->pos.y += (sync.player.y - npcs[i]->pos.y) / 3.0f;
                npcs[i]->pos.z += (sync.player.z - npcs[i]->pos.z) / 3.0f;
            } else {
                npcs[i]->pos = { sync.player.x, sync.player.y, sync.player.z };
            }
            npcs[i]->colliderPos = npcs[i]->pos;
            npcs[i]->yaw = sync.player.yaw;
            npcs[i]->flags |= NPC_FLAG_DIRTY_SHADOW;
            npcs[i]->curAnim = sync.anim;
        } else if (npcs[i] != nullptr) {
            free_npc(npcs[i]);
            npcs[i] = nullptr;
        }
    }

    // Update peers regardless of sync data
    for (size_t i = 0; i < MAX_PEERS; i++) {
        auto& info = peerInfo[i];

        if (info.is_connected()) {
            info.connectionTime++;
        } else {
            if (npcs[i] != nullptr) {
                free_npc(npcs[i]);
                npcs[i] = nullptr;
            }
            info.frameCounter = 0; // avoid 'out of order' on new connection in this slot
            if (info.connectionTime != 0) {
                debug_printf("player %d disconnected", i);
                info.connectionTime = 0;
                bzero((void*) &header.syncData[i], sizeof(SyncData));
            }
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
        return;
    }

    sync->frameCounter = gGameStatus.frameCounter;
    sync->player = { gPlayerStatus.pos.x, gPlayerStatus.pos.y, gPlayerStatus.pos.z, gPlayerStatus.curYaw };
    sync->area = gGameStatus.areaID;
    sync->map = gGameStatus.mapID;
    sync->anim = gPlayerStatus.trueAnimation;

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
        if (i == (size_t)header.me) continue;
        if (peerInfo[i].is_connected()) {
            return true;
        }
    }
    return false;
}

}; // namespace online
