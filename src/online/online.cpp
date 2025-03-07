#include "common.h"
#include "PR/os_libc.h"
#include "dx/debug_menu.h"
#include "online.h"
#include "enums.h"
#include "functions.h"
#include "game_modes.h"
#include "script_api/common.h"
#include "sprite/npc/Bandit.h"
#include "gcc/string.h"

namespace online {

struct Header {
    char magic[0x20];
    u32 heartbeat; // we set to 0, gdb increments
    char room[0x20];
    s32 me; // index of syncData that this client owns
    s32 host; // the host's index
    bool isMySyncDataValid; // if false, GDB wont read syncData[me]
    size_t sizeof_syncData;
    volatile SyncData syncData[MAX_PEERS];
};

// .text section is linked first, so header will be placed at the start of this segment
[[gnu::section(".text")]] Header header = {
    "PAPERMARIO-DX ONLINE",
    0,
    "",
    -1,
    -1,
    false,
    sizeof(SyncData),
    {0},
};

// If true, don't use bridge to connect to room, provide a local room instead with just us
bool offline = true;

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

            // Player prediction
            if (npcs[i] != nullptr) {
                npcs[i]->pos.x += info.x.velocity();
                npcs[i]->pos.y += info.y.velocity();
                npcs[i]->pos.z += info.z.velocity();

                npc_test_move_simple_with_slipping(npcs[i]->collisionChannel, &npcs[i]->pos.x, &npcs[i]->pos.y, &npcs[i]->pos.z, 0, npcs[i]->yaw, npcs[i]->collisionHeight, npcs[i]->collisionDiameter);

                if (npc_try_snap_to_ground(npcs[i], info.y.velocity())) {
                    info.y.set(npcs[i]->pos.y);
                }

                // don't set colliderPos because we want npc_do_world_collision to run

                npcs[i]->flags |= NPC_FLAG_DIRTY_SHADOW;
            }

            // Ship prediction
            if (sync.ship.slave == -1) {
                sync.ship.pos.x += sync.ship.velocity.x;
                sync.ship.pos.y += sync.ship.velocity.y;
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
                npcs[i] = get_npc_by_index(create_npc_with_index(&bp, NULL, TRUE, MAX_NPCS - 1 - i));
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

        if (header.syncData[i].frameCounter != 0 && info.is_connected()) {
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

s32 timeSinceHeartbeat = 0xFFFF;

static volatile SyncData::Host* host() {
    if (header.host < 0) return nullptr;
    return &header.syncData[header.host].host;
}

static bool i_am_host() {
    if (header.me < 0 || header.host < 0) return false;
    return header.me == header.host;
}

EXTERN_C void online_begin_step_game_loop() {
    receive_data();

    // Check heartbeat (bridge should write 1)
    if (header.heartbeat == 0 && !offline) {
        // bridge is dead?
        if (timeSinceHeartbeat < 0xFFFF) {
            timeSinceHeartbeat++;
        }
    } else {
        timeSinceHeartbeat = 0;
        header.heartbeat = 0;
    }

    // If connection lost, go to net_00
    if (!is_connected_to_room() && get_game_mode() == GAME_MODE_WORLD) { // TODO: do what in battle?
        s16 areaID;
        s16 mapID;
        get_map_IDs_by_name_checked("net_00", &areaID, &mapID);
        if (gGameStatus.areaID != areaID || gGameStatus.mapID != mapID) {
            gGameStatusPtr->areaID = areaID;
            gGameStatusPtr->mapID = mapID;
            gGameStatusPtr->entryID = 0;
            set_map_transition_effect(TRANSITION_STANDARD);
            set_game_mode(GAME_MODE_CHANGE_MAP);

            // Clear all data
            bzero((void*) &header.syncData, sizeof(header.syncData));
            bzero((void*) &peerInfo, sizeof(peerInfo));
            header.me = -1;
            header.host = -1;
            header.room[0] = '\0';
        }
    }
}

// at end of game loop, update our sync data
EXTERN_C void online_end_step_game_loop() {
    auto* sync = begin_writing_my_sync_data();
    if (sync == nullptr) {
        return;
    }

    bool first = sync->frameCounter == 0;

    sync->frameCounter = gGameStatus.frameCounter;
    sync->player = { gPlayerStatus.pos.x, gPlayerStatus.pos.y, gPlayerStatus.pos.z, gPlayerStatus.curYaw };
    sync->area = gGameStatus.areaID;
    sync->map = gGameStatus.mapID;
    sync->anim = gPlayerStatus.trueAnimation;
    sync->ship.slave = -1;

    if (i_am_host()) {
        if (first) {
            sync->host.seachartSeed = rand_int(0x7FFFFFFF);
        }
        sync->host.windDirection = 0.0f; // TODO: randomly change at random intervals
    } else if (auto* h = host()) {
        bcopy(&sync->host, (void*)h, sizeof(*h));
    }

    end_writing_my_sync_data(sync);
}

EXTERN_C void online_on_clear_npcs() {
    for (size_t i = 0; i < MAX_PEERS; i++) {
        npcs[i] = nullptr;
    }
}

bool is_connected_to_bridge() {
    return timeSinceHeartbeat < 60;
}

bool is_connected_to_room() {
    return is_connected_to_bridge() && header.host >= 0;
}

void connect_to_room(const char* room) {
    ASSERT(is_connected_to_bridge());
    ASSERT(!is_connected_to_room());

    if (offline) {
        header.me = 0;
        header.host = 0;
    } else {
        strcpy(header.room, room);
    }
}

}; // namespace online
