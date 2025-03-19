#include "dst_01.h"
#include "dx/debug_menu.h"

namespace dst_01 {

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

#include "world/common/enemy/Pokey.inc.c"

EvtScript EVS_NpcDefeat_Pokey1 = {
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

EvtScript EVS_NpcInit_Pokey1 = {
    Call(BindNpcDefeat, NPC_SELF, Ref(EVS_NpcDefeat_Pokey1))
    Return
    End
};

NpcData NpcData_Pokey1 = {
    .id = NPC_Pokey1,
    .settings = &N(NpcSettings_Pokey),
    .pos = { GEN_POKEY1_VEC },
    .flags = ENEMY_FLAG_FLYING,
    .init = &EVS_NpcInit_Pokey1,
    .yaw = GEN_POKEY1_DIR,
    .drops = POKEY_DROPS,
    .territory = GEN_POKEY1_TERRITORY,
    .animations = POKEY_ANIMS,
    .aiDetectFlags = AI_DETECT_SIGHT,
};

EvtScript EVS_NpcDefeat_Pokey2 = {
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

EvtScript EVS_NpcInit_Pokey2 = {
    Call(BindNpcDefeat, NPC_SELF, Ref(EVS_NpcDefeat_Pokey2))
    Return
    End
};

NpcData NpcData_Pokey2 = {
    .id = NPC_Pokey2,
    .settings = &N(NpcSettings_Pokey),
    .pos = { GEN_POKEY2_VEC },
    .flags = ENEMY_FLAG_FLYING,
    .init = &EVS_NpcInit_Pokey2,
    .yaw = GEN_POKEY2_DIR,
    .drops = POKEY_DROPS,
    .territory = GEN_POKEY2_TERRITORY,
    .animations = POKEY_ANIMS,
    .aiDetectFlags = AI_DETECT_SIGHT,
};

#include "world/common/enemy/Bandit.inc.c"

EvtScript EVS_NpcDefeat_Bandit1 = {
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

EvtScript EVS_NpcInit_Bandit1 = {
    Call(BindNpcDefeat, NPC_SELF, Ref(EVS_NpcDefeat_Bandit1))
    Return
    End
};

NpcData NpcData_Bandit1 = {
    .id = NPC_Bandit1,
    .settings = &N(NpcSettings_Bandit),
    .pos = { GEN_BANDIT1_VEC },
    .flags = ENEMY_FLAG_IGNORE_ENTITY_COLLISION | ENEMY_FLAG_FLYING | ENEMY_FLAG_NO_SHADOW_RAYCAST,
    .init = &EVS_NpcInit_Bandit1,
    .yaw = GEN_BANDIT1_DIR,
    .drops = BANDIT_DROPS,
    .territory = GEN_BANDIT1_TERRITORY,
    .animations = BANDIT_ANIMS,
    .aiDetectFlags = AI_DETECT_SIGHT,
};

EvtScript EVS_NpcDefeat_Bandit2 = {
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

EvtScript EVS_NpcInit_Bandit2 = {
    Call(BindNpcDefeat, NPC_SELF, Ref(EVS_NpcDefeat_Bandit2))
    Return
    End
};

NpcData NpcData_Bandit2 = {
    .id = NPC_Bandit2,
    .settings = &N(NpcSettings_Bandit),
    .pos = { GEN_BANDIT2_VEC },
    .flags = ENEMY_FLAG_IGNORE_ENTITY_COLLISION | ENEMY_FLAG_FLYING | ENEMY_FLAG_NO_SHADOW_RAYCAST,
    .init = &EVS_NpcInit_Bandit2,
    .yaw = GEN_BANDIT2_DIR,
    .drops = BANDIT_DROPS,
    .territory = GEN_BANDIT2_TERRITORY,
    .animations = BANDIT_ANIMS,
    .aiDetectFlags = AI_DETECT_SIGHT,
};

#include "world/common/enemy/Cleft.inc.c"

EvtScript EVS_NpcDefeat_Cleft = {
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

EvtScript EVS_NpcInit_Cleft = {
    Call(BindNpcDefeat, NPC_SELF, Ref(EVS_NpcDefeat_Cleft))
    Return
    End
};

NpcData NpcData_Cleft = {
    .id = NPC_Cleft,
    .settings = &N(NpcSettings_Cleft),
    .pos = { GEN_CLEFT_VEC },
    .flags = ENEMY_FLAG_IGNORE_ENTITY_COLLISION | ENEMY_FLAG_FLYING,
    .init = &EVS_NpcInit_Cleft,
    .yaw = GEN_CLEFT_DIR,
    .drops = CLEFT_DROPS,
    .territory = GEN_CLEFT_TERRITORY,
    .animations = CLEFT_ANIMS,
    .aiDetectFlags = AI_DETECT_SIGHT,
};

NpcGroupList DefaultNPCs = {
    NPC_GROUP((NpcData_Pokey1), 8, 3),
    NPC_GROUP((NpcData_Pokey2), 8, 3),
    NPC_GROUP((NpcData_Bandit1), 9, 3),
    NPC_GROUP((NpcData_Bandit2), 9, 3),
    NPC_GROUP((NpcData_Cleft), 10, 3),
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

}; // namespace dst_01

MapSettings dst_01_settings = {
    .main = &dst_01::EVS_Main,
    .entryList = &dst_01::Entrances,
    .entryCount = ENTRY_COUNT(dst_01::Entrances),
    .background = &gBackgroundImage,
};

s32 dst_01_map_init(void) {
    sprintf(wMapBgName, "net_bg");
    return FALSE;
}
