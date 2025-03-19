#include "grs_01.h"
#include "entity.h"
#include "effects.h"

namespace grs_01 {

EvtScript EVS_FocusCam_OnChest = {
    Call(UseSettingsFrom, CAM_DEFAULT, LVar0, LVar1, LVar2)
    Call(SetCamSpeed, CAM_DEFAULT, LVarA)
    Call(SetCamDistance, CAM_DEFAULT, Float(350.0))
    Call(SetCamPitch, CAM_DEFAULT, Float(12.0), Float(-5.5))
    Call(SetCamPosB, CAM_DEFAULT, Float(500.0), Float(20.0))
    Call(SetPanTarget, CAM_DEFAULT, GEN_CHEST_VEC)
    Call(PanToTarget, CAM_DEFAULT, 0, TRUE)
    Call(WaitForCam, CAM_DEFAULT, Float(1.0))
    Return
    End
};

#include "world/common/entity/Chest.inc.c"

EvtScript EVS_OpenChest = EVT_OPEN_CHEST(ITEM_PYRAMID_STONE, GF_TRP00_Chest_PyramidStone);

API_CALLABLE(PlayBigSmokePuff) {
    Bytecode* args = script->ptrReadPos;
    s32 x = evt_get_variable(script, *args++);
    s32 y = evt_get_variable(script, *args++);
    s32 z = evt_get_variable(script, *args++);

    fx_big_smoke_puff(x, y, z);

    return ApiStatus_DONE2;
}

EvtScript EVS_SpawnChest = {
    Loop(0)
        IfEq(MV_EnemiesDefeated, 6)
            BreakLoop
        EndIf
        Wait(1)
    EndLoop
    Call(DisablePlayerInput, TRUE)
    Call((PlayBigSmokePuff), GEN_CHEST_VEC)
    Call(MakeEntity, Ref(Entity_Chest), GEN_CHEST_PARAMS, MAKE_ENTITY_END)
    Call(AssignChestFlag, GF_TRP00_Chest_PyramidStone)
    Call(AssignScript, Ref(EVS_OpenChest))
    SetF(LVarA, Float(3.0))
    ExecWait(EVS_FocusCam_OnChest)
    Wait(45)
    Call(DisablePlayerInput, FALSE)
    Return
    End
};

EvtScript EVS_MakeEntities = {
    Call(MakeEntity, Ref(Entity_YellowBlock), GEN_YELLOW_BLOCK_PARAMS, MAKE_ENTITY_END)
    Call(AssignBlockFlag, GF_TRP00_ItemBlock_Coin)
    Return
    End
};

}; // namespace grs_01
