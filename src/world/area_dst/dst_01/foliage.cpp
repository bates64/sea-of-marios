#include "dst_01.h"
#include "foliage.hpp"

namespace dst_01 {

DEFINE_TREE(Tree1, MODEL_TreeLeaves1, MODEL_TreeTrunk1, GEN_TREE1_VEC, 0.0f)

DEFINE_BUSH(Bush1, MODEL_Bush1, GEN_BUSH1_VEC)

EvtScript EVS_SetFoliage = {
    BIND_TREE(Tree1, COLLIDER_Tree1)
    BIND_BUSH(Bush1, COLLIDER_Bush1)
    Return
    End
};

}; // namespace dst_01
