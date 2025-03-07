#include "common.h"
#include "generated.h"
#include "message_ids.h"
#include "map.h"
#include "../dst.h"

#include "mapfs/dst_00_shape.h"
#include "mapfs/dst_00_hit.h"

#include "sprite/npc/RussT.h"

namespace dst_00 {

enum {
    NPC_DockRuss,
};

extern EvtScript EVS_Main;
extern EvtScript EVS_BindExitTriggers;
extern EvtScript EVS_TexPan;
extern EvtScript EVS_SetFoliage;

}; // namespace dst_00
