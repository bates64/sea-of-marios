#include "area.hpp"
#include "stage/kmr_bt04.hpp"

#include "battle/common/actor/player_goomba.hpp"
#include "battle/common/actor/player_goomba.inc.cpp"

#include "battle/common/actor/captain_goomba.hpp"
#include "battle/common/actor/captain_goomba.inc.cpp"

#include "battle/common/actor/goom_mate.hpp"
#include "battle/common/actor/goom_mate.inc.cpp"

using namespace battle::actor;

namespace battle::area::kmr_part_1 {

Vec3i PlayerGoomba1Pos = { -95, 0, 0 };
Vec3i PlayerGoomba2Pos = { 95, 0, 0 };
Vec3i CaptainGoombaPos = { 100, 0, 20 };

// Formation Formation_00 = {
//     ACTOR_BY_POS(PlayerGoomba, PlayerGoomba1Pos, 10),
//     ACTOR_BY_POS(PlayerGoomba, PlayerGoomba2Pos, 9),
// };

Formation Formation_00 = {
    ACTOR_BY_POS(PlayerGoomba, PlayerGoomba1Pos, 10),
    ACTOR_BY_POS(GoomMate, CaptainGoombaPos, 9),
};

BattleList Battles = {
    BATTLE(Formation_00, ForestPath, "クリボー"),
    {},
};

StageList Stages = {
    STAGE("ForestPath", ForestPath),
    {},
};

}; // namespace battle::area::kmr_part_1
