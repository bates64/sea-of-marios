#include "dst_01.h"
#include "entity.h"
#include "effects.h"

namespace dst_01 {

#include "world/common/entity/Chest.inc.c"

EvtScript EVS_OpenChest = EVT_OPEN_CHEST(ITEM_PYRAMID_STONE, GF_TRP00_Chest_PyramidStone);

EvtScript EVS_MakeEntities = {
    Call(MakeEntity, Ref(Entity_YellowBlock), GEN_YELLOW_BLOCK_PARAMS, MAKE_ENTITY_END)
    Call(AssignBlockFlag, GF_TRP00_ItemBlock_Coin)
    Call(MakeEntity, Ref(Entity_Chest), GEN_CHEST_PARAMS, MAKE_ENTITY_END)
    Call(AssignChestFlag, GF_TRP00_Chest_PyramidStone)
    Call(AssignScript, Ref(EVS_OpenChest))
    Return
    End
};

}; // namespace dst_01
