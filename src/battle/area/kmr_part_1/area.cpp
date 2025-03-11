#include "area.hpp"
#include "stage/kmr_bt04.hpp"
#include "stage/net_bt00.hpp"

#include "battle/common/actor/goomba_and_paragoomba.hpp"
#include "battle/common/actor/goomba_and_paragoomba.inc.cpp"

#include "battle/common/actor/koopa_troopa.hpp"
#include "battle/common/actor/koopa_troopa.inc.cpp"

#include "battle/common/actor/spear_guy.hpp"
#include "battle/common/actor/spear_guy.inc.cpp"

#include "battle/common/actor/jungle_fuzzy.hpp"
#include "battle/common/actor/jungle_fuzzy.inc.cpp"

#include "battle/common/actor/pokey.hpp"
#include "battle/common/actor/pokey.inc.cpp"

#include "battle/common/actor/bandit.hpp"
#include "battle/common/actor/bandit.inc.cpp"

#include "battle/common/actor/cleft.hpp"
#include "battle/common/actor/cleft.inc.cpp"

#include "battle/common/actor/captain_kuribo.hpp"
#include "battle/common/actor/captain_kuribo.inc.cpp"

#include "battle/common/actor/red_and_blue_mate.hpp"
#include "battle/common/actor/red_and_blue_mate.inc.cpp"

#include "battle/common/actor/mecha_blooper_and_baby_blooper.hpp"
#include "battle/common/actor/mecha_blooper_and_baby_blooper.inc.cpp"

using namespace battle::actor;

namespace battle::area::kmr_part_1 {

Vec3i CaptainKuriboPos = { 100, 0, 20 };

Vec3i BlooperPos = { 80, 45, -10 };

Formation Formation_Goomba = {
    ACTOR_BY_IDX(Goomba, BTL_POS_GROUND_B, 10),
};

Formation Formation_KoopaTroopa = {
    ACTOR_BY_IDX(KoopaTroopa, BTL_POS_GROUND_B, 10),
};

Formation Formation_SpearGuy = {
    ACTOR_BY_IDX(SpearGuy, BTL_POS_GROUND_B, 10, .var0 = 1),
};

Formation Formation_JungleFuzzy = {
    ACTOR_BY_IDX(JungleFuzzy, BTL_POS_GROUND_B, 10),
};

Formation Formation_Pokey = {
    ACTOR_BY_IDX(Pokey, BTL_POS_GROUND_B, 10),
};

Formation Formation_Bandit = {
    ACTOR_BY_IDX(Bandit, BTL_POS_GROUND_B, 10),
};

Formation Formation_Cleft = {
    ACTOR_BY_IDX(Cleft, BTL_POS_GROUND_B, 10),
};

Formation Formation_MechaBlooper = {
    ACTOR_BY_POS(MechaBlooper, BlooperPos, 10),
};

Formation Formation_CaptainKuribo = {
    ACTOR_BY_POS(CaptainKuribo, CaptainKuriboPos, 10),
};

BattleList Battles = {
    BATTLE(Formation_Goomba, ForestPath, "Goomba"),
    BATTLE(Formation_KoopaTroopa, ForestPath, "KoopaTroopa"),
    BATTLE(Formation_SpearGuy, ForestPath, "SpearGuy"),
    BATTLE(Formation_JungleFuzzy, ForestPath, "JungleFuzzy"),
    BATTLE(Formation_Pokey, ForestPath, "Pokey"),
    BATTLE(Formation_Bandit, ForestPath, "Bandit"),
    BATTLE(Formation_Cleft, ForestPath, "Cleft"),
    BATTLE(Formation_MechaBlooper, ForestPath, "MechaBlooper"),
    BATTLE(Formation_CaptainKuribo, ShipDeck, "CaptainKuribo"),
    {},
};

StageList Stages = {
    STAGE("ForestPath", ForestPath),
    STAGE("ShipDeck", ShipDeck),
    {},
};

}; // namespace battle::area::kmr_part_1
