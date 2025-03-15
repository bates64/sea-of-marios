#include "net_01.hpp"
#include "effects.h"
#include "dx/debug_menu.h"

namespace net_01 {

EntryList Entrances = { GEN_ENTRY_LIST };

EvtScript EVS_Scene_MeetCaptainKuribo = {
    Set(GB_NET01_Intro, 1)
    Call(DisablePlayerInput, TRUE)
    Call(GetNpcPos, NPC_CaptainKuribo, LVar0, LVar1, LVar2)
    Call(UseSettingsFrom, CAM_DEFAULT, LVar0, LVar1, LVar2)
    Call(SetCamDistance, CAM_DEFAULT, 325)
    Call(SetCamSpeed, CAM_DEFAULT, Float(4.0))
    Call(SetPanTarget, CAM_DEFAULT, LVar0, LVar1, LVar2)
    Call(PanToTarget, CAM_DEFAULT, 0, TRUE)
    Call(WaitForCam, CAM_DEFAULT, Float(1.0))
    Call(SpeakToNpc, NPC_CaptainKuribo, ANIM_CaptainKuribo_Talk, ANIM_CaptainKuribo_Idle, 0, NPC_CaptainKuribo, MSG_Intro_WorldCK_Talk_One)
    Call(SpeakToNpc, NPC_Goomate_Red, ANIM_Goomates_Red_Talk, ANIM_Goomates_Red_Idle, 0, NPC_CaptainKuribo, MSG_Intro_WorldRM_Talk_Zero)
    Call(SpeakToNpc, NPC_CaptainKuribo, ANIM_CaptainKuribo_Talk, ANIM_CaptainKuribo_Angry, 0, NPC_Goomate_Red, MSG_Intro_WorldCK_Talk_Two)
    Call(SpeakToNpc, NPC_Goomate_Blue, ANIM_Goomates_Blue_Talk, ANIM_Goomates_Blue_Idle, 0, NPC_CaptainKuribo, MSG_Intro_WorldBM_Talk_Zero)
    Call(SpeakToNpc, NPC_CaptainKuribo, ANIM_CaptainKuribo_Talk, ANIM_CaptainKuribo_Angry, 0, NPC_Goomate_Blue, MSG_Intro_WorldCK_Talk_Three)
    Call(SpeakToNpc, NPC_Goomate_Red, ANIM_Goomates_Red_Talk, ANIM_Goomates_Red_Idle, 0, NPC_CaptainKuribo, MSG_Intro_WorldRM_Talk_One)
    Call(SpeakToPlayer, NPC_CaptainKuribo, ANIM_CaptainKuribo_Talk, ANIM_CaptainKuribo_Angry, 0, MSG_Intro_WorldCK_Talk_Four)
    Call(SpeakToNpc, NPC_Goomate_Blue, ANIM_Goomates_Blue_Talk, ANIM_Goomates_Blue_Idle, 0, NPC_CaptainKuribo, MSG_Intro_WorldBM_Talk_One)
    Call(SpeakToPlayer, NPC_CaptainKuribo, ANIM_CaptainKuribo_Talk, ANIM_CaptainKuribo_Angry, 0, MSG_Intro_WorldCK_Talk_Five)
    Call(SetNpcVar, NPC_CaptainKuribo, 0, 1)
    Call(DisablePlayerInput, FALSE)
    Return
    End
};

API_CALLABLE(MakeLensFlare) {
    Bytecode* args = script->ptrReadPos;
    f32 x = evt_get_float_variable(script, *args++);
    f32 y = evt_get_float_variable(script, *args++);
    f32 z = evt_get_float_variable(script, *args++);
    s32 duration = evt_get_variable(script, *args++);

    fx_lens_flare(0, x, y, z, duration);
    return ApiStatus_DONE2;
}

// EvtScript EVS_CaptainKuriboBlastoff = {
//     Wait(10)
//     Thread
//         Call(MakeLerp, 100, 10, 150, EASING_LINEAR)
//         Label(0)
//         Call(UpdateLerp)
//         MulF(LVar0, Float(0.01))
//         Call(SetNpcScale, NPC_CaptainKuribo, LVar0, LVar0, LVar0)
//         Wait(1)
//         IfEq(LVar1, 1)
//             Goto(0)
//         EndIf
//     EndThread
//     Call(SetNpcJumpscale, NPC_CaptainKuribo, Float(0.3))
//     Call(GetNpcPos, NPC_CaptainKuribo, LVar0, LVar1, LVar2)
//     Add(LVar0, 50)
//     Add(LVar1, 300)
//     Add(LVar2, 50)
//     Call(NpcJump0, NPC_CaptainKuribo, LVar0, LVar1, LVar2, 150)
//     Call(PlaySoundAt, SOUND_LARGE_LENS_FLARE, SOUND_SPACE_DEFAULT, LVar0, LVar1, LVar2)
//     Call(MakeLensFlare, LVar0, LVar1, LVar2, 40)
//     Call(SetNpcPos, NPC_CaptainKuribo, NPC_DISPOSE_LOCATION)
//     Call(RemoveNpc, NPC_CaptainKuribo)
//     Return
//     End
// };

#include "common/FadeBackgroundDarken.inc.c"
#include "common/FadeBackgroundLighten.inc.c"

// API_CALLABLE(SetBackgroundAlpha) {
//     Bytecode* args = script->ptrReadPos;

//     mdl_set_shroud_tint_params(0, 0, 0, evt_get_variable(script, *args++));
//     return ApiStatus_DONE2;
// }

EvtScript EVS_Scene_BossDefeated = {
    Call(DisablePlayerInput, TRUE)
    Call(SetEncounterStatusFlags, ENCOUNTER_FLAG_THUMBS_UP, FALSE)
    Call(SetNpcAnimation, NPC_CaptainKuribo, ANIM_CaptainKuribo_Dead)
    Call(SetNpcAnimation, NPC_Goomate_Red, ANIM_Goomates_Red_BurnStill)
    Call(SetNpcAnimation, NPC_Goomate_Blue, ANIM_Goomates_Blue_CryStill)
    Call(N(FadeBackgroundDarken))
    PlayEffect(EFFECT_STOP_WATCH, 0, 0, 0, 0, Float(1.0), 200, 0)
    Call(PlaySoundAtNpc, NPC_CaptainKuribo, SOUND_TIME_OUT, SOUND_SPACE_DEFAULT)
    Wait(200)
    // Call((SetBackgroundAlpha), 0)
    Call(SetNpcPos, NPC_CaptainKuribo, NPC_DISPOSE_LOCATION)
    Call(SetNpcPos, NPC_Goomate_Red, NPC_DISPOSE_LOCATION)
    Call(SetNpcPos, NPC_Goomate_Blue, NPC_DISPOSE_LOCATION)
    Wait(30)
    Call(N(FadeBackgroundLighten))
    Call(SetZoneEnabled, ZONE_Floor, TRUE)
    Call(SetZoneEnabled, ZONE_FloorCutscene, FALSE)
    Call(GetPlayerPos, LVar0, LVar1, LVar2)
    Call(UseSettingsFrom, CAM_DEFAULT, LVar0, LVar1, LVar2)
    Call(SetCamSpeed, CAM_DEFAULT, Float(4.0))
    Call(SetPanTarget, CAM_DEFAULT, LVar0, LVar1, LVar2)
    Call(PanToTarget, CAM_DEFAULT, 0, TRUE)
    Call(WaitForCam, CAM_DEFAULT, Float(1.0))
    Call(RemoveNpc, NPC_CaptainKuribo)
    Call(RemoveNpc, NPC_Goomate_Red)
    Call(RemoveNpc, NPC_Goomate_Blue)
    Call(DisablePlayerInput, FALSE)
    Return
    End
};

NpcSettings NpcSettings_CaptainKuribo = {
    .height = 24,
    .radius = 24,
    .onHit = &EnemyNpcHit,
    .onDefeat = &EnemyNpcDefeat,
    .level = ACTOR_LEVEL_CAPTAIN_KURIBO,
};

NpcSettings NpcSettings_GoomMate = {
    .height = 20,
    .radius = 23,
    .onHit = &EnemyNpcHit,
    .onDefeat = &EnemyNpcDefeat,
    .level = ACTOR_LEVEL_RED_MATE,
};

EvtScript EVS_NpcIdle_CaptainKuribo = {
    Loop(0)
        Call(GetSelfVar, 0, LVar0)
        IfEq(LVar0, 1)
            BreakLoop
        EndIf
        Wait(1)
    EndLoop
    Call(DisablePlayerInput, TRUE)
    Call(SetNpcAnimation, NPC_SELF, ANIM_CaptainKuribo_Angry)
    Call(StartBossBattle, SONG_GOOMBA_KING_BATTLE)
    Call(DisablePlayerInput, FALSE)
    Return
    End
};

EvtScript EVS_NpcDefeat_CaptainKuribo = {
    Call(GetBattleOutcome, LVar0)
    Switch(LVar0)
        CaseEq(OUTCOME_PLAYER_WON)
            Exec(EVS_Scene_BossDefeated)
        CaseEq(OUTCOME_PLAYER_LOST)
        CaseEq(OUTCOME_PLAYER_FLED)
    EndSwitch
    Call(DisablePlayerInput, FALSE)
    Return
    End
};

EvtScript EVS_NpcInit_CaptainKuribo = {
    Call(BindNpcIdle, NPC_SELF, Ref(EVS_NpcIdle_CaptainKuribo))
    Call(BindNpcDefeat, NPC_SELF, Ref(EVS_NpcDefeat_CaptainKuribo))
    Return
    End
};

EvtScript EVS_NpcIdle_RedMate = {
    Return
    End
};

EvtScript EVS_NpcDefeat_RedMate = {
    Call(GetBattleOutcome, LVar0)
    Switch(LVar0)
        CaseEq(OUTCOME_PLAYER_WON)
        CaseEq(OUTCOME_PLAYER_LOST)
        CaseEq(OUTCOME_PLAYER_FLED)
    EndSwitch
    Return
    End
};

EvtScript EVS_NpcInit_RedMate = {
    Call(BindNpcIdle, NPC_SELF, Ref(EVS_NpcIdle_RedMate))
    Call(BindNpcDefeat, NPC_SELF, Ref(EVS_NpcDefeat_RedMate))
    Return
    End
};

EvtScript EVS_NpcIdle_BlueMate = {
    Return
    End
};

EvtScript EVS_NpcDefeat_BlueMate = {
    Call(GetBattleOutcome, LVar0)
    Switch(LVar0)
        CaseEq(OUTCOME_PLAYER_WON)
        CaseEq(OUTCOME_PLAYER_LOST)
        CaseEq(OUTCOME_PLAYER_FLED)
    EndSwitch
    Return
    End
};

EvtScript EVS_NpcInit_BlueMate = {
    Call(BindNpcIdle, NPC_SELF, Ref(EVS_NpcIdle_BlueMate))
    Call(BindNpcDefeat, NPC_SELF, Ref(EVS_NpcDefeat_BlueMate))
    Return
    End
};

NpcData NpcData_CaptainKuribo[] = {
    {
        .id = NPC_CaptainKuribo,
        .settings = &NpcSettings_CaptainKuribo,
        .pos = { GEN_CAPTAIN_KURIBO_VEC },
        .flags = ENEMY_FLAG_DO_NOT_KILL | ENEMY_FLAG_IGNORE_WORLD_COLLISION | ENEMY_FLAG_NO_DELAY_AFTER_FLEE | ENEMY_FLAG_ACTIVE_WHILE_OFFSCREEN | ENEMY_FLAG_NO_DROPS,
        .init = &EVS_NpcInit_CaptainKuribo,
        .yaw = GEN_CAPTAIN_KURIBO_DIR,
        .drops = NO_DROPS,
        .animations = {
            .idle   = ANIM_CaptainKuribo_Idle,
            .walk   = ANIM_CaptainKuribo_Walk,
            .run    = ANIM_CaptainKuribo_Run,
            .chase  = ANIM_CaptainKuribo_Run,
            .anim_4 = ANIM_CaptainKuribo_Idle,
            .anim_5 = ANIM_CaptainKuribo_Idle,
            .death  = ANIM_CaptainKuribo_Dead,
            .hit    = ANIM_CaptainKuribo_Dead,
            .anim_8 = ANIM_CaptainKuribo_Run,
            .anim_9 = ANIM_CaptainKuribo_Run,
            .anim_A = ANIM_CaptainKuribo_Run,
            .anim_B = ANIM_CaptainKuribo_Run,
            .anim_C = ANIM_CaptainKuribo_Run,
            .anim_D = ANIM_CaptainKuribo_Run,
            .anim_E = ANIM_CaptainKuribo_Run,
            .anim_F = ANIM_CaptainKuribo_Run,
        },
    },
    {
        .id = NPC_Goomate_Red,
        .settings = &NpcSettings_GoomMate,
        .pos = { GEN_RED_MATE_VEC },
        .flags = ENEMY_FLAG_DO_NOT_KILL | ENEMY_FLAG_IGNORE_WORLD_COLLISION | ENEMY_FLAG_NO_DELAY_AFTER_FLEE | ENEMY_FLAG_ACTIVE_WHILE_OFFSCREEN | ENEMY_FLAG_NO_DROPS,
        .init = &EVS_NpcInit_RedMate,
        .yaw = GEN_RED_MATE_DIR,
        .drops = NO_DROPS,
        .animations = {
            .idle   = ANIM_Goomates_Red_Idle,
            .walk   = ANIM_Goomates_Red_Walk,
            .run    = ANIM_Goomates_Red_Run,
            .chase  = ANIM_Goomates_Red_Run,
            .anim_4 = ANIM_Goomates_Red_Idle,
            .anim_5 = ANIM_Goomates_Red_Idle,
            .death  = ANIM_Goomates_Red_Hurt,
            .hit    = ANIM_Goomates_Red_Hurt,
            .anim_8 = ANIM_Goomates_Red_Run,
            .anim_9 = ANIM_Goomates_Red_Run,
            .anim_A = ANIM_Goomates_Red_Run,
            .anim_B = ANIM_Goomates_Red_Run,
            .anim_C = ANIM_Goomates_Red_Run,
            .anim_D = ANIM_Goomates_Red_Run,
            .anim_E = ANIM_Goomates_Red_Run,
            .anim_F = ANIM_Goomates_Red_Run,
        },
    },
    {
        .id = NPC_Goomate_Blue,
        .settings = &NpcSettings_GoomMate,
        .pos = { GEN_BLUE_MATE_VEC },
        .flags = ENEMY_FLAG_DO_NOT_KILL | ENEMY_FLAG_IGNORE_WORLD_COLLISION | ENEMY_FLAG_NO_DELAY_AFTER_FLEE | ENEMY_FLAG_ACTIVE_WHILE_OFFSCREEN | ENEMY_FLAG_NO_DROPS,
        .init = &EVS_NpcInit_BlueMate,
        .yaw = GEN_BLUE_MATE_DIR,
        .drops = NO_DROPS,
        .animations = {
            .idle   = ANIM_Goomates_Blue_Idle,
            .walk   = ANIM_Goomates_Blue_Walk,
            .run    = ANIM_Goomates_Blue_Run,
            .chase  = ANIM_Goomates_Blue_Run,
            .anim_4 = ANIM_Goomates_Blue_Idle,
            .anim_5 = ANIM_Goomates_Blue_Idle,
            .death  = ANIM_Goomates_Blue_Hurt,
            .hit    = ANIM_Goomates_Blue_Hurt,
            .anim_8 = ANIM_Goomates_Blue_Run,
            .anim_9 = ANIM_Goomates_Blue_Run,
            .anim_A = ANIM_Goomates_Blue_Run,
            .anim_B = ANIM_Goomates_Blue_Run,
            .anim_C = ANIM_Goomates_Blue_Run,
            .anim_D = ANIM_Goomates_Blue_Run,
            .anim_E = ANIM_Goomates_Blue_Run,
            .anim_F = ANIM_Goomates_Blue_Run,
        },
    },
};

NpcGroupList IntroNPCs = {
    NPC_GROUP(NpcData_CaptainKuribo, BTL_KMR_1_FORMATION_08, BTL_KMR_1_STAGE_01),
    {}
};

EvtScript EVS_EnterMap = {
    Call(GetEntryID, LVar0)
    Switch(LVar0)
    CaseEq(0)
        Switch(GB_NET01_Intro)
        CaseEq(0)
            Call(SetZoneEnabled, ZONE_FloorCutscene, TRUE)
            Call(SetZoneEnabled, ZONE_Floor, FALSE)
            Call(MakeNpcs, TRUE, Ref(IntroNPCs))
            ExecWait(EVS_Scene_MeetCaptainKuribo)
        CaseEq(1)
            Call(SetZoneEnabled, ZONE_FloorCutscene, FALSE)
            Return
        EndSwitch
    EndSwitch
    Return
    End
};

EvtScript EVS_Main = {
    Call(SetSpriteShading, SHADING_NONE)
    EVT_SETUP_CAMERA_NO_LEAD(0, 0, 0)
    Call(ClearAmbientSounds, 250)
    Call(FadeOutMusic, 0, 1000)
    Call(PlayAmbientSounds, AMBIENT_SEA)
    Call(EnableGroup, MODEL_SmallShip, FALSE)
    Exec(EVS_EnterMap)
    Return
    End
};

}; // namespace net_01

MapSettings net_01_settings = {
    .main = &net_01::EVS_Main,
    .entryList = &net_01::Entrances,
    .entryCount = ENTRY_COUNT(net_01::Entrances),
    .background = &gBackgroundImage,
};
