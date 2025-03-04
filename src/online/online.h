#pragma once

#include "common.h"
#include "dx/debug_menu.h"

EXTERN_C void online_begin_step_game_loop();
EXTERN_C void online_end_step_game_loop();
EXTERN_C void online_on_clear_npcs();

#ifdef __cplusplus

namespace online {
    constexpr size_t MAX_PEERS = 16;

    // Data owned by each player
    struct SyncData {
        u8 checksum;
        u16 frameCounter;
        Vec4f player;
        u8 area;
        u8 map;
        s32 anim;
        struct Ship {
            // If this player is the captain, -1. If this player is on another player's team, that player's syncData index.
            s8 slave;

            Vec2f pos;
            Vec2f velocity;
            f32 yaw;
            f32 sailYaw;
            f32 sailLength;
        } ship;

        // Data owned by the host and replicated by all players
        struct Host {
            u32 seachartSeed;
            f32 windDirection;
        } host;

        u8 calc_checksum() const volatile {
            u8 sum = 0;
            u8* ptr = (u8*) this;
            for (size_t i = 1; i < sizeof(SyncData); i++) {
                sum += ptr[i];
            }
            return sum;
        }

        bool is_checksum_valid() const volatile {
            return calc_checksum() == checksum;
        }
    };

    bool is_connected_to_bridge();
    bool is_connected_to_room();
    void connect_to_room(const char* room);
};

#endif
