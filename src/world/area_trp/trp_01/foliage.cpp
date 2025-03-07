#include "trp_01.h"
#include "foliage.hpp"

namespace trp_01 {

// DEFINE_TREE(Tree1, MODEL_TreeLeaves1, MODEL_TreeTrunk1, GEN_, 0.0f)

EvtScript EVS_SetFoliage = {
    // BIND_TREE(Tree1, COLLIDER_Tree1)
    Return
    End
};

}; // namespace trp_01
