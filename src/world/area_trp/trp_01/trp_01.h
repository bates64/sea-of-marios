#include "common.h"
#include "generated.h"
#include "message_ids.h"
#include "map.h"
#include "../trp.h"

#include "mapfs/trp_01_shape.h"
#include "mapfs/trp_01_hit.h"

#include "sprite/npc/Fuzzy.h"
#include "sprite/npc/SpearGuy.h"

namespace trp_01 {

enum {
    NPC_JungleFuzzy1,
    NPC_SpearGuy1,
    NPC_SpearGuy1Hitbox,
    NPC_SpearGuy2,
    NPC_SpearGuy2Hitbox,
    NPC_JungleFuzzy2,
    NPC_SpearGuy3,
    NPC_SpearGuy3Hitbox,
};

enum {
    MV_EnemiesDefeated = MapVar(0),
};

extern EvtScript EVS_Main;
extern EvtScript EVS_BindExitTriggers;
extern EvtScript EVS_TexPan;
extern EvtScript EVS_SetFoliage;
extern EvtScript EVS_MakeEntities;
extern EvtScript EVS_SpawnChests;

}; // namespace trp_01
