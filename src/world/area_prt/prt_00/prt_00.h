#include "common.h"
#include "generated.h"
#include "message_ids.h"
#include "map.h"
#include "../prt.h"

#include "mapfs/prt_00_shape.h"
#include "mapfs/prt_00_hit.h"

#include "sprite/npc/HarryT.h"
#include "sprite/npc/SpearGuy.h"
#include "sprite/npc/RipCheato.h"
#include "sprite/npc/Fishmael.h"

namespace prt_00 {

enum {
    NPC_HarryT,
    NPC_F_Bobomb,
    NPC_RipCheato,
    NPC_ToadGuard,
    NPC_Fishmael,
    NPC_Dryite_InnOwner,
    NPC_F_Bandit,
    NPC_F_Gloomba,
};

extern EvtScript EVS_Main;
extern EvtScript EVS_BindExitTriggers;
extern EvtScript EVS_TexPan;

}; // namespace prt_00
