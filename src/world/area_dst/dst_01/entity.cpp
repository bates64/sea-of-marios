#include "dst_01.h"
#include "entity.h"
#include "effects.h"

namespace dst_01 {

// #include "world/common/entity/Chest.inc.c"

// EvtScript EVS_OpenChest = EVT_OPEN_CHEST(ITEM_PYRAMID_STONE, GF_TRP00_Chest_PyramidStone);

// API_CALLABLE(PlayBigSmokePuff) {
//     Bytecode* args = script->ptrReadPos;
//     s32 x = evt_get_variable(script, *args++);
//     s32 y = evt_get_variable(script, *args++);
//     s32 z = evt_get_variable(script, *args++);

//     fx_big_smoke_puff(x, y, z);

//     return ApiStatus_DONE2;
// }

// EvtScript EVS_SpawnTreasureChest = {
//     Loop(0)
//         IfEq(MapVar(0), TRUE)
//             BreakLoop
//         EndIf
//         Wait(1)
//     EndLoop
//     Call((PlayBigSmokePuff), GEN_CHEST_VEC)
//     Call(MakeEntity, Ref(Entity_Chest), GEN_CHEST_PARAMS, MAKE_ENTITY_END)
//     Call(AssignChestFlag, GF_TRP00_Chest_PyramidStone)
//     Call(AssignScript, Ref(EVS_OpenChest))
//     Return
//     End
// };

EvtScript EVS_MakeEntities = {
    // Call(MakeEntity, Ref(Entity_HiddenYellowBlock), GEN_HIDDEN_ITEM_BLOCK1_PARAMS, MAKE_ENTITY_END)
    // Call(AssignBlockFlag, GF_TRP00_ItemBlock_Coin)
    Return
    End
};

}; // namespace dst_01
