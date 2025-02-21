#include "trp_00.h"
#include "entity.h"

namespace trp_00 {

EvtScript EVS_MakeEntities = {
    Call(MakeEntity, Ref(Entity_HiddenYellowBlock), GEN_HIDDEN_ITEM_BLOCK1_PARAMS, MAKE_ENTITY_END)
    Call(AssignBlockFlag, GF_TRP00_ItemBlock_Coin)
    Return
    End
};

}; // namespace trp_00
