#include "net_01.hpp"
#include "effects.h"
#include "dx/debug_menu.h"

namespace net_01 {

EntryList Entrances = { GEN_ENTRY_LIST };

EvtScript EVS_SetupMusic = {
    Call(FadeOutMusic, 0, 1000)
    Return
    End
};

EvtScript EVS_JrTroopaMusic = {
    Call(SetMusicTrack, 0, SONG_JR_TROOPA_THEME, 0, 8)
    Return
    End
};

EvtScript EVS_Scene_MeetJrTroopa = {
    Set(GB_NET01_Intro, 1)
    Call(DisablePlayerInput, TRUE)
    ExecWait(EVS_JrTroopaMusic)
    // Call(WaitForCam, CAM_DEFAULT, Float(1.0))
    // Call(SpeakToNpc, NPC_CaptainKuribo, ANIM_CaptainKuribo_Talk, ANIM_CaptainKuribo_Idle, 0, NPC_CaptainKuribo, MSG_Intro_WorldCK_Talk_One)
    // Call(SpeakToNpc, NPC_Goomate_Red, ANIM_Goomates_Red_Talk, ANIM_Goomates_Red_Idle, 0, NPC_CaptainKuribo, MSG_Intro_WorldRM_Talk_Zero)
    // Call(SpeakToNpc, NPC_CaptainKuribo, ANIM_CaptainKuribo_Talk, ANIM_CaptainKuribo_Angry, 0, NPC_Goomate_Red, MSG_Intro_WorldCK_Talk_Two)
    // Call(SpeakToNpc, NPC_Goomate_Blue, ANIM_Goomates_Blue_Talk, ANIM_Goomates_Blue_Idle, 0, NPC_CaptainKuribo, MSG_Intro_WorldBM_Talk_Zero)
    // Call(SpeakToNpc, NPC_CaptainKuribo, ANIM_CaptainKuribo_Talk, ANIM_CaptainKuribo_Angry, 0, NPC_Goomate_Blue, MSG_Intro_WorldCK_Talk_Three)
    // Call(SpeakToNpc, NPC_Goomate_Red, ANIM_Goomates_Red_Talk, ANIM_Goomates_Red_Idle, 0, NPC_CaptainKuribo, MSG_Intro_WorldRM_Talk_One)
    // Call(UseSettingsFrom, CAM_DEFAULT, GEN_CAMERA1_VEC)
    // Call(SetCamSpeed, CAM_DEFAULT, Float(90.0))
    // Call(SetPanTarget, CAM_DEFAULT, GEN_MARIO_POS_VEC)
    // Call(PanToTarget, CAM_DEFAULT, 0, TRUE)
    // Call(WaitForCam, CAM_DEFAULT, Float(1.0))
    // Call(SpeakToPlayer, NPC_CaptainKuribo, ANIM_CaptainKuribo_Talk, ANIM_CaptainKuribo_Angry, 0, MSG_Intro_WorldCK_Talk_Four)
    // Call(SpeakToNpc, NPC_Goomate_Blue, ANIM_Goomates_Blue_Talk, ANIM_Goomates_Blue_Idle, 0, NPC_CaptainKuribo, MSG_Intro_WorldBM_Talk_One)
    // Call(SpeakToPlayer, NPC_CaptainKuribo, ANIM_CaptainKuribo_Talk, ANIM_CaptainKuribo_Angry, 0, MSG_Intro_WorldCK_Talk_Five)
    Call(SetNpcVar, NPC_JrTroopa, 0, 1)
    Call(DisablePlayerInput, FALSE)
    Return
    End
};

EvtScript EVS_Scene_BossDefeated = {
    Call(DisablePlayerInput, TRUE)
    // Call(SetEncounterStatusFlags, ENCOUNTER_FLAG_THUMBS_UP, FALSE)
    // Call(SetPlayerAnimation, ANIM_Mario1_Still)
    // Call(SetNpcAnimation, NPC_CaptainKuribo, ANIM_CaptainKuribo_Dead)
    // Call(SetNpcAnimation, NPC_Goomate_Red, ANIM_Goomates_Red_BurnStill)
    // Call(SetNpcAnimation, NPC_Goomate_Blue, ANIM_Goomates_Blue_CryStill)
    // Call(N(FadeBackgroundDarken))
    // PlayEffect(EFFECT_STOP_WATCH, 0, 0, 0, 0, Float(1.0), 200, 0)
    // Call(PlaySoundAtNpc, NPC_CaptainKuribo, SOUND_TIME_OUT, SOUND_SPACE_DEFAULT)
    // Wait(200)
    // // Call((SetBackgroundAlpha), 0)
    // Call(SetNpcPos, NPC_CaptainKuribo, NPC_DISPOSE_LOCATION)
    // Call(SetNpcPos, NPC_Goomate_Red, NPC_DISPOSE_LOCATION)
    // Call(SetNpcPos, NPC_Goomate_Blue, NPC_DISPOSE_LOCATION)
    // Wait(30)
    // Call(N(FadeBackgroundLighten))
    // Call(GetPlayerPos, LVar0, LVar1, LVar2)
    // Call(UseSettingsFrom, CAM_DEFAULT, LVar0, LVar1, LVar2)
    // Call(SetCamSpeed, CAM_DEFAULT, Float(4.0))
    // Call(SetPanTarget, CAM_DEFAULT, LVar0, LVar1, LVar2)
    // Call(PanToTarget, CAM_DEFAULT, 0, TRUE)
    // // Call(WaitForCam, CAM_DEFAULT, Float(1.0))
    // Call(ResetCam, CAM_DEFAULT, Float(3.0))
    // Call(RemoveNpc, NPC_CaptainKuribo)
    // Call(RemoveNpc, NPC_Goomate_Red)
    // Call(RemoveNpc, NPC_Goomate_Blue)
    // Call(SetPlayerAnimation, ANIM_Mario1_Idle)
    ExecWait(EVS_SetupMusic)
    Call(DisablePlayerInput, FALSE)
    Return
    End
};

#include "world/common/npc/JrTroopa.inc.c"

EvtScript EVS_NpcIdle_JrTroopa = {
    Loop(0)
        Call(GetSelfVar, 0, LVar0)
        IfEq(LVar0, 1)
            BreakLoop
        EndIf
        Wait(1)
    EndLoop
    Call(DisablePlayerInput, TRUE)
    Call(SetNpcAnimation, NPC_SELF, ANIM_JrTroopa_ChargeArmsUp)
    Call(StartBossBattle, SONG_JR_TROOPA_BATTLE)
    Call(DisablePlayerInput, FALSE)
    Return
    End
};

EvtScript EVS_NpcDefeat_JrTroopa = {
    Call(GetBattleOutcome, LVar0)
    Switch(LVar0)
        CaseEq(OUTCOME_PLAYER_WON)
            Exec(EVS_Scene_BossDefeated)
        CaseEq(OUTCOME_PLAYER_LOST)
        CaseEq(OUTCOME_PLAYER_FLED)
    EndSwitch
    // Call(DisablePlayerInput, FALSE)
    Return
    End
};

EvtScript EVS_NpcInit_JrTroopa = {
    Call(BindNpcIdle, NPC_SELF, Ref(EVS_NpcIdle_JrTroopa))
    Call(BindNpcDefeat, NPC_SELF, Ref(EVS_NpcDefeat_JrTroopa))
    Return
    End
};

NpcData NpcData_JrTroopa[] = {
    {
        .id = NPC_JrTroopa,
        .settings = &NpcSettings_JrTroopa,
        .pos = { GEN_JR_TROOPA_VEC },
        .flags = ENEMY_FLAG_DO_NOT_KILL | ENEMY_FLAG_IGNORE_WORLD_COLLISION | ENEMY_FLAG_NO_DELAY_AFTER_FLEE | ENEMY_FLAG_ACTIVE_WHILE_OFFSCREEN | ENEMY_FLAG_NO_DROPS,
        .init = &EVS_NpcInit_JrTroopa,
        .yaw = GEN_JR_TROOPA_DIR,
        .drops = NO_DROPS,
        .animations = JR_TROOPA_ANIMS,
    },
};

NpcGroupList IntroNPCs = {
    NPC_GROUP(NpcData_JrTroopa, BTL_KMR_1_FORMATION_08, BTL_KMR_1_STAGE_01),
    {}
};

EvtScript EVS_EnterMap = {
    Call(GetEntryID, LVar0)
    Switch(LVar0)
    CaseEq(0)
        Switch(GB_NET01_Intro)
        CaseEq(0)
            Call(UseSettingsFrom, CAM_DEFAULT, GEN_CAMERA0_VEC)
            Call(SetCamSpeed, CAM_DEFAULT, Float(90.0))
            Call(SetPanTarget, CAM_DEFAULT, GEN_MARIO_POS_VEC)
            Call(PanToTarget, CAM_DEFAULT, 0, TRUE)
            Call(MakeNpcs, TRUE, Ref(IntroNPCs))
            Call(SetPlayerPos, GEN_MARIO_POS_VEC)
            ExecWait(EVS_Scene_MeetJrTroopa)
        CaseEq(1)
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
    Wait(1)
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
