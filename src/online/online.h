#pragma once

#include "common.h"
#include "dx/debug_menu.h"

EXTERN_C void online_begin_step_game_loop();
EXTERN_C void online_end_step_game_loop();
EXTERN_C void online_on_clear_npcs();

#ifdef __cplusplus

namespace online {
    constexpr size_t MAX_PEERS = 16;

    struct SyncData {
        u8 checksum;
        u16 frameCounter;
        Vec4f player;
        u8 area;
        u8 map;

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

    bool is_connected_to_gateway();
    bool is_connected_to_server();
};

#endif
