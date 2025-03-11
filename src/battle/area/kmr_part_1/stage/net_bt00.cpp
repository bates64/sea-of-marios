#include "../area.hpp"

#include "battle/battle.h"
#include "mapfs/net_bt00_shape.h"

#define GEN_TEX_PANNER_1 \
    TEX_PAN_PARAMS_ID(TEX_PANNER_1) \
    TEX_PAN_PARAMS_STEP(   175,   200,     0,     0) \
    TEX_PAN_PARAMS_FREQ(     1,     1,     0,     0) \
    TEX_PAN_PARAMS_INIT(     0,     0,     0,     0)

namespace battle::area::kmr_part_1 {

namespace net_00 {

#include "world/common/atomic/TexturePan.inc.c"

EvtScript EVS_PreBattle = {
    Call(SetSpriteShading, SHADING_NONE)
    Call(SetTexPanner, MODEL_WaterSurface, TEX_PANNER_1)
    Thread
        GEN_TEX_PANNER_1
        Exec(N(EVS_UpdateTexturePan))
    EndThread
    Return
    End
};

EvtScript EVS_PostBattle = {
    Return
    End
};

}; // namespace net_00

Stage ShipDeck = {
    .texture = "net_tex",
    .shape = "net_bt00_shape",
    .hit = "net_bt00_hit",
    .preBattle = &net_00::EVS_PreBattle,
    .postBattle = &net_00::EVS_PostBattle,
    .bg = "net_bg",
};


}; // namespace battle::area::kmr_part_1
