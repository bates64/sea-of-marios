#include "common.h"
#include "generated.h"
#include "message_ids.h"
#include "map.h"
#include "../grs.h"

#include "mapfs/grs_01_shape.h"
#include "mapfs/grs_01_hit.h"

#include "sprite/npc/SpearGuy.h"

namespace grs_01 {

enum {
    NPC_Goomba1,
    NPC_KoopaTroopa1,
    NPC_Goomba2,
    NPC_Goomba3,
    NPC_Goomba4,
    NPC_KoopaTroopa2,
};

enum {
    MV_EnemiesDefeated = MapVar(0),
};

extern EvtScript EVS_Main;
extern EvtScript EVS_BindExitTriggers;
extern EvtScript EVS_TexPan;
extern EvtScript EVS_SetFoliage;
extern EvtScript EVS_MakeEntities;
extern EvtScript EVS_SpawnChest;

}; // namespace grs_01
