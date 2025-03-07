#include "trp_01.h"
#include "dx/debug_menu.h"

namespace trp_01 {

EntryList Entrances = { GEN_ENTRY_LIST };

#include "world/common/atomic/TexturePan.inc.c"

EvtScript EVS_TexPan = {
    SetGroup(EVT_GROUP_NEVER_PAUSE)
    Call(SetTexPanner, MODEL_Waves, TEX_PANNER_1)
    Thread
        GEN_TEX_PANNER_1
        Exec(EVS_UpdateTexturePan)
    EndThread
    Call(SetTexPanner, MODEL_Fire, TEX_PANNER_1)
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

// #include "world/common/npc/RussT.inc.c"

// EvtScript EVS_NpcInit_RussT = {

//     Return
//     End
// };

// NpcData NpcData_RussT = {
//     .id = NPC_DockNPC,
//     .settings = &NpcSettings_RussT,
//     .pos = { GEN_DOCK_NPC_VEC },
//     .flags = COMMON_PASSIVE_FLAGS,
//     .init = &EVS_NpcInit_RussT,
//     .yaw = GEN_DOCK_NPC_DIR,
//     .drops = NO_DROPS,
//     .animations = RUSS_T_ANIMS,
// };

#include "world/common/enemy/SpearGuy_Wander.inc.c"

EvtScript EVS_NpcDefeat_SpearGuy1 = {
    Call(GetBattleOutcome, LVar0)
    Switch(LVar0)
        CaseEq(OUTCOME_PLAYER_WON)
            // Set(MapVar(0), TRUE)
            Call(DoNpcDefeat)
        CaseEq(OUTCOME_PLAYER_LOST)
        CaseEq(OUTCOME_PLAYER_FLED)
    EndSwitch
    Return
    End
};

EvtScript EVS_NpcInit_SpearGuy1 = {
    Call(BindNpcDefeat, NPC_SELF, Ref(EVS_NpcDefeat_SpearGuy1))
    Return
    End
};

NpcData NpcData_SpearGuy1[] = {
    {
        .id = NPC_SpearGuy1,
        .settings = &N(NpcSettings_SpearGuy_Wander),
        .pos = { GEN_SPEAR_GUY1_VEC },
        .flags = ENEMY_FLAG_IGNORE_ENTITY_COLLISION | ENEMY_FLAG_FLYING,
        .init = &EVS_NpcInit_SpearGuy1,
        .yaw = GEN_SPEAR_GUY1_DIR,
        .drops = SPEAR_GUY_DROPS,
        .territory = GEN_SPEAR_GUY1_TERRITORY,
        .animations = SPEAR_GUY_ANIMS,
        .aiDetectFlags = AI_DETECT_SIGHT | AI_DETECT_SENSITIVE_MOTION,
        .extraAnimations = N(ExtraAnims_SpearGuy),
    },
    SPEAR_GUY_HITBOX(NPC_SpearGuy1Hitbox)
};

NpcGroupList DefaultNPCs = {
    // NPC_GROUP(NpcData_RussT),
    NPC_GROUP((NpcData_SpearGuy1), BTL_KMR_1_FORMATION_00, BTL_KMR_1_STAGE_04),
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
    Return
    End
};

}; // namespace trp_01

MapSettings trp_01_settings = {
    .main = &trp_01::EVS_Main,
    .entryList = &trp_01::Entrances,
    .entryCount = ENTRY_COUNT(trp_01::Entrances),
    .background = &gBackgroundImage,
};

s32 trp_01_map_init(void) {
    sprintf(wMapBgName, "net_bg");
    return FALSE;
}
