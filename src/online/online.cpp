#include "common.h"
#include "dx/debug_menu.h"
#include "online.h"

namespace online {

EXTERN_C void online_begin_step_game_loop() {
    comms::begin_step();
    peers::update();
}

EXTERN_C void online_end_step_game_loop() {
    comms::end_step();
}

} // namespace online
