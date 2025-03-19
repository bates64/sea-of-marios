#include "../area.hpp"

#include "battle/battle.h"
#include "mapfs/nok_bt01_shape.h"

namespace battle::area::kmr_part_1 {

namespace nok_01 {

EvtScript EVS_AnimateFlower = {
    Set(LVarA, LVar0)
    Label(0)
        Call(TranslateModel, LVarA, 0, 2, 0)
        Wait(5)
        Call(TranslateModel, LVarA, 0, 0, 0)
        Wait(5)
        Call(TranslateModel, LVarA, 0, 2, 0)
        Wait(5)
        Call(TranslateModel, LVarA, 0, 0, 0)
        Wait(5)
        Call(RandInt, 30, LVar0)
        Add(LVar0, 30)
        Wait(LVar0)
        Goto(0)
    Return
    End
};

EvtScript EVS_PreBattle = {
    Call(SetSpriteShading, SHADING_NONE)
    Set(LVar0, MODEL_h1)
    Exec(EVS_AnimateFlower)
    Set(LVar0, MODEL_h2)
    Exec(EVS_AnimateFlower)
    Set(LVar0, MODEL_h3)
    Exec(EVS_AnimateFlower)
    Set(LVar0, MODEL_h5)
    Exec(EVS_AnimateFlower)
    Set(LVar0, MODEL_h7)
    Exec(EVS_AnimateFlower)
    Set(LVar0, MODEL_h8)
    Exec(EVS_AnimateFlower)
    Return
    End
};

EvtScript EVS_PostBattle = {
    Return
    End
};

s32 ForegroundModels[] = {
    MODEL_ha3,
    MODEL_hap,
    MODEL_kusa3,
    STAGE_MODEL_LIST_END
};

}; // namespace nok_01

Stage PleasantPath = {
    .texture = "nok_tex",
    .shape = "nok_bt01_shape",
    .hit = "nok_bt01_hit",
    .preBattle = &nok_01::EVS_PreBattle,
    .postBattle = &nok_01::EVS_PostBattle,
    .bg = "net_bg",
    .foregroundModelList = nok_01::ForegroundModels,
};

}; // namespace battle::area::kmr_part_1
