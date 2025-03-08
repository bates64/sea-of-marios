#include "area.hpp"
#include "stage/kmr_bt04.hpp"

#include "battle/common/actor/goomba_and_paragoomba.hpp"
#include "battle/common/actor/goomba_and_paragoomba.inc.cpp"

#include "battle/common/actor/captain_goomba.hpp"
#include "battle/common/actor/captain_goomba.inc.cpp"

#include "battle/common/actor/goom_mate.hpp"
#include "battle/common/actor/goom_mate.inc.cpp"

#include "battle/common/actor/spear_guy.hpp"
#include "battle/common/actor/spear_guy.inc.cpp"

#include "battle/common/actor/jungle_fuzzy.hpp"
#include "battle/common/actor/jungle_fuzzy.inc.cpp"

using namespace battle::actor;

namespace battle::area::kmr_part_1 {

Vec3i CaptainGoombaPos = { 100, 0, 20 };

Formation Formation_Goomba = {
    ACTOR_BY_IDX(Goomba, BTL_POS_GROUND_B, 10),
};

Formation Formation_SpearGuy = {
    ACTOR_BY_IDX(SpearGuy, BTL_POS_GROUND_B, 10, .var0 = 1),
};

Formation Formation_JungleFuzzy = {
    ACTOR_BY_IDX(JungleFuzzy, BTL_POS_GROUND_B, 10),
};

BattleList Battles = {
    BATTLE(Formation_Goomba, ForestPath, "Goomba"),
    BATTLE(Formation_SpearGuy, ForestPath, "SpearGuy"),
    BATTLE(Formation_JungleFuzzy, ForestPath, "JungleFuzzy"),
    {},
};

StageList Stages = {
    STAGE("ForestPath", ForestPath),
    {},
};

}; // namespace battle::area::kmr_part_1
