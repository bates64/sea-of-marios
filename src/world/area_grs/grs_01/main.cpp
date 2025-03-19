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

EvtScript EVS_NpcDefeat_Goomba1 = {
    Call(GetBattleOutcome, LVar0)
    Switch(LVar0)
        CaseEq(OUTCOME_PLAYER_WON)
            Add(MV_EnemiesDefeated, 1)
            Call(DoNpcDefeat)
        CaseEq(OUTCOME_PLAYER_LOST)
        CaseEq(OUTCOME_PLAYER_FLED)
    EndSwitch
    Return
    End
};

EvtScript EVS_NpcInit_Goomba1 = {
    Call(BindNpcDefeat, NPC_SELF, Ref(EVS_NpcDefeat_Goomba1))
    Return
    End
};

NpcData NpcData_Goomba1 = {
    .id = NPC_Goomba1,
    .settings = &N(NpcSettings_Goomba_Wander),
    .pos = { GEN_GOOMBA1_VEC },
    .flags = ENEMY_FLAG_IGNORE_ENTITY_COLLISION | ENEMY_FLAG_FLYING,
    .init = &EVS_NpcInit_Goomba1,
    .yaw = GEN_GOOMBA1_DIR,
    .drops = GOOMBA_DROPS,
    .territory = GEN_GOOMBA1_TERRITORY,
    .animations = GOOMBA_ANIMS,
    .aiDetectFlags = AI_DETECT_SIGHT,
};

EvtScript EVS_NpcDefeat_Goomba2 = {
    Call(GetBattleOutcome, LVar0)
    Switch(LVar0)
        CaseEq(OUTCOME_PLAYER_WON)
            Add(MV_EnemiesDefeated, 1)
            Call(DoNpcDefeat)
        CaseEq(OUTCOME_PLAYER_LOST)
        CaseEq(OUTCOME_PLAYER_FLED)
    EndSwitch
    Return
    End
};

EvtScript EVS_NpcInit_Goomba2 = {
    Call(BindNpcDefeat, NPC_SELF, Ref(EVS_NpcDefeat_Goomba2))
    Return
    End
};

NpcData NpcData_Goomba2 = {
    .id = NPC_Goomba2,
    .settings = &N(NpcSettings_Goomba_Wander),
    .pos = { GEN_GOOMBA2_VEC },
    .flags = ENEMY_FLAG_IGNORE_ENTITY_COLLISION | ENEMY_FLAG_FLYING,
    .init = &EVS_NpcInit_Goomba2,
    .yaw = GEN_GOOMBA2_DIR,
    .drops = GOOMBA_DROPS,
    .territory = GEN_GOOMBA2_TERRITORY,
    .animations = GOOMBA_ANIMS,
    .aiDetectFlags = AI_DETECT_SIGHT,
};

EvtScript EVS_NpcDefeat_Goomba3 = {
    Call(GetBattleOutcome, LVar0)
    Switch(LVar0)
        CaseEq(OUTCOME_PLAYER_WON)
            Add(MV_EnemiesDefeated, 1)
            Call(DoNpcDefeat)
        CaseEq(OUTCOME_PLAYER_LOST)
        CaseEq(OUTCOME_PLAYER_FLED)
    EndSwitch
    Return
    End
};

EvtScript EVS_NpcInit_Goomba3 = {
    Call(BindNpcDefeat, NPC_SELF, Ref(EVS_NpcDefeat_Goomba3))
    Return
    End
};

NpcData NpcData_Goomba3 = {
    .id = NPC_Goomba3,
    .settings = &N(NpcSettings_Goomba_Wander),
    .pos = { GEN_GOOMBA3_VEC },
    .flags = ENEMY_FLAG_IGNORE_ENTITY_COLLISION | ENEMY_FLAG_FLYING,
    .init = &EVS_NpcInit_Goomba3,
    .yaw = GEN_GOOMBA3_DIR,
    .drops = GOOMBA_DROPS,
    .territory = GEN_GOOMBA3_TERRITORY,
    .animations = GOOMBA_ANIMS,
    .aiDetectFlags = AI_DETECT_SIGHT,
};

EvtScript EVS_NpcDefeat_Goomba4 = {
    Call(GetBattleOutcome, LVar0)
    Switch(LVar0)
        CaseEq(OUTCOME_PLAYER_WON)
            Add(MV_EnemiesDefeated, 1)
            Call(DoNpcDefeat)
        CaseEq(OUTCOME_PLAYER_LOST)
        CaseEq(OUTCOME_PLAYER_FLED)
    EndSwitch
    Return
    End
};

EvtScript EVS_NpcInit_Goomba4 = {
    Call(BindNpcDefeat, NPC_SELF, Ref(EVS_NpcDefeat_Goomba4))
    Return
    End
};

NpcData NpcData_Goomba4 = {
    .id = NPC_Goomba4,
    .settings = &N(NpcSettings_Goomba_Wander),
    .pos = { GEN_GOOMBA4_VEC },
    .flags = ENEMY_FLAG_IGNORE_ENTITY_COLLISION | ENEMY_FLAG_FLYING,
    .init = &EVS_NpcInit_Goomba4,
    .yaw = GEN_GOOMBA4_DIR,
    .drops = GOOMBA_DROPS,
    .territory = GEN_GOOMBA4_TERRITORY,
    .animations = GOOMBA_ANIMS,
    .aiDetectFlags = AI_DETECT_SIGHT,
};

#include "world/common/enemy/KoopaTroopa_Wander.inc.c"

EvtScript EVS_NpcDefeat_KoopaTroopa1 = {
    Call(GetBattleOutcome, LVar0)
    Switch(LVar0)
        CaseEq(OUTCOME_PLAYER_WON)
            Add(MV_EnemiesDefeated, 1)
            Call(DoNpcDefeat)
        CaseEq(OUTCOME_PLAYER_LOST)
        CaseEq(OUTCOME_PLAYER_FLED)
    EndSwitch
    Return
    End
};

EvtScript EVS_NpcInit_KoopaTroopa1 = {
    Call(BindNpcDefeat, NPC_SELF, Ref(EVS_NpcDefeat_KoopaTroopa1))
    Return
    End
};

NpcData NpcData_KoopaTroopa1 = {
    .id = NPC_KoopaTroopa1,
    .settings = &N(NpcSettings_KoopaTroopa_Wander),
    .pos = { GEN_KOOPA_TROOPA1_VEC },
    .flags = ENEMY_FLAG_IGNORE_ENTITY_COLLISION | ENEMY_FLAG_FLYING,
    .init = &EVS_NpcInit_KoopaTroopa1,
    .yaw = GEN_KOOPA_TROOPA1_DIR,
    .drops = KOOPA_TROOPA_NOK_DROPS,
    .territory = GEN_KOOPA_TROOPA1_TERRITORY,
    .animations = KOOPA_TROOPA_ANIMS,
    .aiDetectFlags = AI_DETECT_SIGHT,
};

EvtScript EVS_NpcDefeat_KoopaTroopa2 = {
    Call(GetBattleOutcome, LVar0)
    Switch(LVar0)
        CaseEq(OUTCOME_PLAYER_WON)
            Add(MV_EnemiesDefeated, 1)
            Call(DoNpcDefeat)
        CaseEq(OUTCOME_PLAYER_LOST)
        CaseEq(OUTCOME_PLAYER_FLED)
    EndSwitch
    Return
    End
};

EvtScript EVS_NpcInit_KoopaTroopa2 = {
    Call(BindNpcDefeat, NPC_SELF, Ref(EVS_NpcDefeat_KoopaTroopa2))
    Return
    End
};

NpcData NpcData_KoopaTroopa2 = {
    .id = NPC_KoopaTroopa2,
    .settings = &N(NpcSettings_KoopaTroopa_Wander),
    .pos = { GEN_KOOPA_TROOPA2_VEC },
    .flags = ENEMY_FLAG_IGNORE_ENTITY_COLLISION | ENEMY_FLAG_FLYING,
    .init = &EVS_NpcInit_KoopaTroopa2,
    .yaw = GEN_KOOPA_TROOPA2_DIR,
    .drops = KOOPA_TROOPA_NOK_DROPS,
    .territory = GEN_KOOPA_TROOPA2_TERRITORY,
    .animations = KOOPA_TROOPA_ANIMS,
    .aiDetectFlags = AI_DETECT_SIGHT,
};

NpcGroupList DefaultNPCs = {
    NPC_GROUP((NpcData_Goomba1), 0, 2),
    NPC_GROUP((NpcData_Goomba2), 1, 2),
    NPC_GROUP((NpcData_Goomba3), 2, 2),
    NPC_GROUP((NpcData_Goomba4), 3, 2),
    NPC_GROUP((NpcData_KoopaTroopa1), 4, 2),
    NPC_GROUP((NpcData_KoopaTroopa2), 5, 2),
    {},
};

EvtScript EVS_Main = {
    Call(SetSpriteShading, SHADING_NONE)
    EVT_SETUP_CAMERA_NO_LEAD(0, 0, 0)
    // Call(SetMusicTrack, 0, SONG_SHOOTING_STAR_SUMMIT, 0, 8)
    Exec(EVS_TexPan)
    // Call(DisablePlayerInput, TRUE)
    // Call(DisablePlayerPhysics, TRUE)
    Exec(EVS_SetFoliage)
    Call(MakeNpcs, TRUE, Ref(DefaultNPCs))
    ExecWait(EVS_MakeEntities)
    Exec(EVS_SpawnChest)
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
