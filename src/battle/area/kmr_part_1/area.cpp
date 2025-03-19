#include "area.hpp"
#include "stage/kmr_bt04.hpp"
#include "stage/net_bt00.hpp"
#include "stage/nok_bt01.hpp"
#include "stage/sbk_bt02.hpp"
#include "stage/jan_bt00.hpp"

#include "battle/common/actor/goomba_and_paragoomba_and_spiked_goomba.hpp"
#include "battle/common/actor/goomba_and_paragoomba_and_spiked_goomba.inc.cpp"

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

#include "battle/common/actor/super_blooper_and_baby_blooper.hpp"
#include "battle/common/actor/super_blooper_and_baby_blooper.inc.cpp"

#include "battle/common/actor/clubbeard_and_clubbeards_heart.hpp"
#include "battle/common/actor/clubbeard_and_clubbeards_heart.inc.cpp"

#include "battle/common/actor/kent_c_koopa.hpp"
#include "battle/common/actor/kent_c_koopa.inc.cpp"

#include "battle/common/actor/buzzar.hpp"
#include "battle/common/actor/buzzar.inc.cpp"

#include "battle/common/actor/monstar.hpp"
#include "battle/common/actor/monstar.inc.cpp"

#include "battle/common/actor/mage_jr_troopa.hpp"
#include "battle/common/actor/mage_jr_troopa.inc.cpp"

using namespace battle::actor;

namespace battle::area::kmr_part_1 {

Vec3i CaptainKuriboPos = { 120, 0, 30 };
Vec3i RedMatePos = { 60, 0, 20 };
Vec3i BlueMatePos = { 10, 0, 10 };

Vec3i BlooperPos = { 80, 45, -10 };

Vec3i ClubbeardPos = { 75, 0, 10 };

Vec3i BuzzarPos = { 90, 70, 0 };

Vec3i MonstarPos = { 75, 16, 5 };

Formation Formation_Goomba1 = { // Formation 0
    ACTOR_BY_IDX(Goomba, BTL_POS_GROUND_B, 10),
    ACTOR_BY_IDX(Goomba, BTL_POS_GROUND_C, 9),
};

Formation Formation_Goomba2 = { // Formation 1
    ACTOR_BY_IDX(Goomba, BTL_POS_GROUND_B, 10),
    ACTOR_BY_IDX(Paragoomba, BTL_POS_AIR_C, 9),
};

Formation Formation_Goomba3 = { // Formation 2
    ACTOR_BY_IDX(Goomba, BTL_POS_GROUND_B, 10),
    ACTOR_BY_IDX(SpikedGoomba, BTL_POS_GROUND_C, 9),
};

Formation Formation_Goomba4 = { // Formation 3
    ACTOR_BY_IDX(Goomba, BTL_POS_GROUND_B, 10),
    ACTOR_BY_IDX(Paragoomba, BTL_POS_AIR_C, 9),
    ACTOR_BY_IDX(SpikedGoomba, BTL_POS_GROUND_D, 8),
};

Formation Formation_KoopaTroopa1 = { // Formation 4
    ACTOR_BY_IDX(KoopaTroopa, BTL_POS_GROUND_B, 10),
    ACTOR_BY_IDX(KoopaTroopa, BTL_POS_GROUND_C, 9),
};

Formation Formation_KoopaTroopa2 = { // Formation 5
    ACTOR_BY_IDX(KoopaTroopa, BTL_POS_GROUND_B, 10),
    ACTOR_BY_IDX(KoopaTroopa, BTL_POS_GROUND_C, 9),
    ACTOR_BY_IDX(KoopaTroopa, BTL_POS_GROUND_D, 8),
};

Formation Formation_SpearGuy = { // Formation 6
    ACTOR_BY_IDX(SpearGuy, BTL_POS_GROUND_B, 10, .var0 = 1),
};

Formation Formation_JungleFuzzy = { // Formation 7
    ACTOR_BY_IDX(JungleFuzzy, BTL_POS_GROUND_B, 10),
};

Formation Formation_Pokey = { // Formation 8
    ACTOR_BY_IDX(Pokey, BTL_POS_GROUND_B, 10),
    ACTOR_BY_IDX(Pokey, BTL_POS_GROUND_C, 9),
};

Formation Formation_Bandit = { // Formation 9
    ACTOR_BY_IDX(Bandit, BTL_POS_GROUND_B, 10),
    ACTOR_BY_IDX(Bandit, BTL_POS_GROUND_C, 9),
};

Formation Formation_Cleft = { // Formation 10
    ACTOR_BY_IDX(Cleft, BTL_POS_GROUND_B, 10),
};

Formation Formation_SuperBlooper = { // Formation 11
    ACTOR_BY_POS(SuperBlooper, BlooperPos, 10),
};

Formation Formation_CaptainKuribo = { // Formation 12
    ACTOR_BY_POS(CaptainKuribo, CaptainKuriboPos, 8),
    ACTOR_BY_POS(RedMate, RedMatePos, 9),
    ACTOR_BY_POS(BlueMate, BlueMatePos, 10),
};

Formation Formation_Clubbeard = { // Formation 13
    ACTOR_BY_POS(Clubbeard, ClubbeardPos, 10),
};

Formation Formation_KentCKoopa = { // Formation 14
    ACTOR_BY_IDX(KentCKoopa, BTL_POS_GROUND_B, 10),
};

Formation Formation_Buzzar = { // Formation 15
    ACTOR_BY_POS(Buzzar, BuzzarPos, 10),
};

Formation Formation_Monstar = { // Formation 16
    ACTOR_BY_POS(Monstar, MonstarPos, 10),
};

Formation Formation_MageJrTroopa = { // Formation 17
    ACTOR_BY_IDX(MageJrTroopa, BTL_POS_GROUND_C, 10),
};

BattleList Battles = {
    BATTLE(Formation_Goomba1, PleasantPath, "Goomba"),
    BATTLE(Formation_Goomba2, PleasantPath, "Paragoomba"),
    BATTLE(Formation_Goomba3, PleasantPath, "Spiked Goomba"),
    BATTLE(Formation_Goomba4, PleasantPath, "Goomba Trio"),
    BATTLE(Formation_KoopaTroopa1, PleasantPath, "Koopa Troopa Duo"),
    BATTLE(Formation_KoopaTroopa2, PleasantPath, "Koopa Troopa Trio"),
    BATTLE(Formation_SpearGuy, Beach, "Uno Spear Guy"),
    BATTLE(Formation_JungleFuzzy, Beach, "Uno Jungle Fuzzy"),
    BATTLE(Formation_Pokey, DryDryDesert, "Pokey"),
    BATTLE(Formation_Bandit, DryDryDesert, "Bandit"),
    BATTLE(Formation_Cleft, DryDryDesert, "Cleft"),
    BATTLE(Formation_SuperBlooper, ShipDeck, "Super Blooper"),
    BATTLE(Formation_CaptainKuribo, ShipDeck, "Captain Kuribo"),
    BATTLE(Formation_Clubbeard, ShipDeck, "Clubbeard"),
    BATTLE(Formation_KentCKoopa, ShipDeck, "Kent C. Koopa"),
    BATTLE(Formation_Buzzar, ShipDeck, "Buzzar"),
    BATTLE(Formation_Monstar, ShipDeck, "Monstar"),
    BATTLE(Formation_MageJrTroopa, ShipDeck, "Mage Jr. Troopa"),
    {},
};

StageList Stages = {
    STAGE("Forest Path", ForestPath), // Stage 0
    STAGE("Ship Deck", ShipDeck), // Stage 1
    STAGE("Pleasant Path", PleasantPath), // Stage 2
    STAGE("Dry Dry Desert", DryDryDesert), // Stage 3
    STAGE("Beach", Beach), // Stage 4
    {},
};

}; // namespace battle::area::kmr_part_1
