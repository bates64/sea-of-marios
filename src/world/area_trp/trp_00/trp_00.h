#include "common.h"
#include "generated.h"
#include "message_ids.h"
#include "map.h"
#include "../trp.h"

#include "mapfs/trp_00_shape.h"
#include "mapfs/trp_00_hit.h"

#include "sprite/npc/RussT.h"
#include "sprite/npc/SpearGuy.h"

namespace trp_00 {

enum {
    NPC_DockNPC,
    NPC_Enemy1,
};

extern EvtScript EVS_Main;
extern EvtScript EVS_BindExitTriggers;
extern EvtScript EVS_TexPan;
extern EvtScript EVS_SetFoliage;
extern EvtScript EVS_MakeEntities;
extern EvtScript EVS_SpawnTreasureChest;

}; // namespace trp_00
