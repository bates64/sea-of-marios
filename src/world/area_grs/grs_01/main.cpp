#include "grs_01.h"
#include "dx/debug_menu.h"

namespace grs_01 {

EntryList Entrances = { GEN_ENTRY_LIST };

#include "world/common/atomic/TexturePan.inc.c"

EvtScript EVS_TexPan = {
    SetGroup(EVT_GROUP_NEVER_PAUSE)
    Call(SetTexPanner, MODEL_Waves, TEX_PANNER_1)
    Thread
        GEN_TEX_PANNER_1
        Exec(EVS_UpdateTexturePan)
    EndThread
    Call(SetTexPanner, MODEL_WaterSurface, TEX_PANNER_1)
    Thread
        GEN_TEX_PANNER_1
        Exec(EVS_UpdateTexturePan)
    EndThread
    Return
    End
};

#include "world/common/enemy/Goomba_Wander.inc.c"

NpcData NpcData_Goomba1 = {
    .id = NPC_Goomba1,
    .settings = &N(NpcSettings_Goomba_Wander),
    .pos = { GEN_GOOMBA1_VEC },
    .flags = ENEMY_FLAG_IGNORE_ENTITY_COLLISION | ENEMY_FLAG_FLYING,
    .yaw = GEN_GOOMBA1_DIR,
    .drops = GOOMBA_DROPS,
    .territory = GEN_GOOMBA1_TERRITORY,
    .animations = GOOMBA_ANIMS,
    .aiDetectFlags = AI_DETECT_SIGHT,
};

#include "world/common/enemy/KoopaTroopa_Wander.inc.c"

// EvtScript EVS_NpcDefeat_SpearGuy = {
//     Call(GetBattleOutcome, LVar0)
//     Switch(LVar0)
//         CaseEq(OUTCOME_PLAYER_WON)
//             Set(MapVar(0), TRUE)
//             Call(DoNpcDefeat)
//         CaseEq(OUTCOME_PLAYER_LOST)
//         CaseEq(OUTCOME_PLAYER_FLED)
//     EndSwitch
//     Return
//     End
// };

// EvtScript EVS_NpcInit_SpearGuy = {
//     Call(BindNpcDefeat, NPC_SELF, Ref(EVS_NpcDefeat_SpearGuy))
//     Return
//     End
// };

NpcData NpcData_KoopaTroopa1 = {
    .id = NPC_KoopaTroopa1,
    .settings = &N(NpcSettings_KoopaTroopa_Wander),
    .pos = { GEN_KOOPA_TROOPA1_VEC },
    .flags = ENEMY_FLAG_IGNORE_ENTITY_COLLISION | ENEMY_FLAG_FLYING,
    .yaw = GEN_KOOPA_TROOPA1_DIR,
    .drops = KOOPA_TROOPA_NOK_DROPS,
    .territory = GEN_KOOPA_TROOPA1_TERRITORY,
    .animations = KOOPA_TROOPA_ANIMS,
    .aiDetectFlags = AI_DETECT_SIGHT,
};

NpcGroupList DefaultNPCs = {
    NPC_GROUP((NpcData_Goomba1), BTL_KMR_1_FORMATION_00, BTL_KMR_1_STAGE_04),
    NPC_GROUP((NpcData_KoopaTroopa1), BTL_KMR_1_FORMATION_00, BTL_KMR_1_STAGE_04),
    {},
};

EvtScript EVS_Main = {
    Call(SetSpriteShading, SHADING_NONE)
    EVT_SETUP_CAMERA_NO_LEAD(0, 0, 0)
    // Call(SetMusicTrack, 0, SONG_SHOOTING_STAR_SUMMIT, 0, 8)
    Exec(EVS_TexPan)
    // Call(DisablePlayerInput, TRUE)
    // Call(DisablePlayerPhysics, TRUE)
    // Exec(EVS_SetFoliage)
    // Call(MakeNpcs, TRUE, Ref(DefaultNPCs))
    Return
    End
};

}; // namespace grs_01

MapSettings grs_01_settings = {
    .main = &grs_01::EVS_Main,
    .entryList = &grs_01::Entrances,
    .entryCount = ENTRY_COUNT(grs_01::Entrances),
    .background = &gBackgroundImage,
};

s32 grs_01_map_init(void) {
    sprintf(wMapBgName, "net_bg");
    return FALSE;
}
