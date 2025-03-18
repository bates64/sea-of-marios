#include "common.h"
#include "generated.h"
#include "message_ids.h"
#include "map.h"
#include "../net.h"

#include "mapfs/net_01_shape.h"
#include "mapfs/net_01_hit.h"

#include "sprite/player.h"
#include "sprite/npc/JrTroopa.h"
// #include "sprite/npc/CaptainKuribo.h"
// #include "sprite/npc/Goomates.h"

namespace net_01 {

enum {
    NPC_JrTroopa,
};

enum {
    MV_Intro           = MapVar(0),
};

extern EvtScript EVS_Main;

}; // namespace net_01
