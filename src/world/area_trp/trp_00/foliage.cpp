#include "trp_00.h"
#include "foliage.hpp"

namespace trp_00 {

DEFINE_TREE(Tree1, MODEL_TreeLeaves, MODEL_TreeTrunk, GEN_TREE1_VEC, 0.0f)

EvtScript EVS_SetFoliage = {
    BIND_TREE(Tree1, COLLIDER_Tree)
    Return
    End
};

}; // namespace trp_00
