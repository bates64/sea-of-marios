#include "prt_00.h"
#include "dx/debug_menu.h"

namespace prt_00 {

EntryList Entrances = { GEN_ENTRY_LIST };

#include "world/common/atomic/TexturePan.inc.c"

EvtScript EVS_TexPan = {
    SetGroup(EVT_GROUP_NEVER_PAUSE)
    Call(SetTexPanner, MODEL_kaimen, TEX_PANNER_1)
    Thread
        GEN_TEX_PANNER_1
        Exec(EVS_UpdateTexturePan)
    EndThread
    Return
    End
};

#include "world/common/npc/Toad_Stationary.inc.c"

EvtScript EVS_NpcInit_HarryT = {

    Return
    End
};

NpcData NpcData_HarryT = {
    .id = NPC_HarryT,
    .settings = &N(NpcSettings_Toad_Stationary),
    .pos = { GEN_HARRY_T_VEC },
    .flags = COMMON_PASSIVE_FLAGS,
    .init = &EVS_NpcInit_HarryT,
    .yaw = GEN_HARRY_T_DIR,
    .drops = NO_DROPS,
    .animations = {
        .idle   = ANIM_HarryT_Idle,
        .walk   = ANIM_HarryT_Walk,
        .run    = ANIM_HarryT_Run,
        .chase  = ANIM_HarryT_Run,
        .anim_4 = ANIM_HarryT_Idle,
    },
};

#include "world/common/npc/Bobomb.inc.c"

EvtScript EVS_NpcInteract_F_Bobomb = {
    // Call(SpeakToPlayer, NPC_RipCheato, ANIM_RipCheato_Talk, ANIM_RipCheato_Idle, 0, MSG_MGM_0004)
    Return
    End
};

EvtScript EVS_NpcInit_F_Bobomb = {
    Call(BindNpcInteract, NPC_SELF, Ref(EVS_NpcInteract_F_Bobomb))
    Return
    End
};

NpcData NpcData_F_Bobomb = {
    .id = NPC_F_Bobomb,
    .settings = &N(NpcSettings_Bobomb),
    .pos = { GEN_F_BOBOMB_VEC },
    .flags = COMMON_PASSIVE_FLAGS,
    .init = &EVS_NpcInit_F_Bobomb,
    .yaw = GEN_F_BOBOMB_DIR,
    .drops = NO_DROPS,
    .animations = BOBOMB_GREEN_ANIMS,
};

EvtScript EVS_NpcInteract_RipCheato = {
    // Call(SpeakToPlayer, NPC_RipCheato, ANIM_RipCheato_Talk, ANIM_RipCheato_Idle, 0, MSG_MGM_0004)
    Return
    End
};

EvtScript EVS_NpcInit_RipCheato = {
    Call(BindNpcInteract, NPC_SELF, Ref(EVS_NpcInteract_RipCheato))
    Return
    End
};

NpcSettings NpcSettings_RipCheato = {
    .height = 30,
    .radius = 24,
    .level = ACTOR_LEVEL_NONE,
};

NpcData NpcData_RipCheato = {
    .id = NPC_RipCheato,
    .settings = &NpcSettings_RipCheato,
    .pos = { GEN_RIP_CHEATO_VEC },
    .flags = BASE_PASSIVE_FLAGS,
    .init = &EVS_NpcInit_RipCheato,
    .yaw = GEN_RIP_CHEATO_DIR,
    .drops = NO_DROPS,
    .animations = {
        .idle   = ANIM_RipCheato_Idle,
        .walk   = ANIM_RipCheato_Idle,
        .run    = ANIM_RipCheato_Idle,
        .chase  = ANIM_RipCheato_Idle,
        .anim_4 = ANIM_RipCheato_Idle,
        .anim_5 = ANIM_RipCheato_Idle,
        .death  = ANIM_RipCheato_Idle,
        .hit    = ANIM_RipCheato_Idle,
        .anim_8 = ANIM_RipCheato_Idle,
        .anim_9 = ANIM_RipCheato_Idle,
        .anim_A = ANIM_RipCheato_Idle,
        .anim_B = ANIM_RipCheato_Idle,
        .anim_C = ANIM_RipCheato_Idle,
        .anim_D = ANIM_RipCheato_Idle,
        .anim_E = ANIM_RipCheato_Idle,
        .anim_F = ANIM_RipCheato_Idle,
    },
};

EvtScript EVS_NpcInteract_ToadGuard = {
    // Call(SpeakToPlayer, NPC_RipCheato, ANIM_RipCheato_Talk, ANIM_RipCheato_Idle, 0, MSG_MGM_0004)
    Return
    End
};

EvtScript EVS_NpcInit_ToadGuard = {
    Call(BindNpcInteract, NPC_SELF, Ref(EVS_NpcInteract_ToadGuard))
    Return
    End
};

NpcData NpcData_ToadGuard = {
        .id = NPC_ToadGuard,
        .settings = &N(NpcSettings_Toad_Stationary),
        .pos = { GEN_TOAD_GUARD_VEC },
        .flags = COMMON_PASSIVE_FLAGS,
        .init = &EVS_NpcInit_ToadGuard,
        .yaw = GEN_TOAD_GUARD_DIR,
        .drops  = NO_DROPS,
        .animations = TOAD_GUARD_RED_ANIMS,
};

EvtScript EVS_NpcInteract_Fishmael = {
    // Call(SpeakToPlayer, NPC_RipCheato, ANIM_RipCheato_Talk, ANIM_RipCheato_Idle, 0, MSG_MGM_0004)
    Return
    End
};

EvtScript EVS_NpcInit_Fishmael = {
    Call(BindNpcInteract, NPC_SELF, Ref(EVS_NpcInteract_Fishmael))
    Return
    End
};

NpcData NpcData_Fishmael = {
    .id = NPC_Fishmael,
    .settings = &N(NpcSettings_Toad_Stationary),
    .pos = { GEN_FISHMAEL_VEC },
    .flags = COMMON_PASSIVE_FLAGS | ENEMY_FLAG_DO_NOT_AUTO_FACE_PLAYER,
    .init = &EVS_NpcInit_Fishmael,
    .yaw = GEN_FISHMAEL_DIR,
    .drops = NO_DROPS,
    .animations = {
        .idle   = ANIM_Fishmael_Idle,
    },
};

#include "world/common/enemy/Bandit.inc.c"

EvtScript EVS_NpcInteract_F_Bandit = {
    // Call(SpeakToPlayer, NPC_RipCheato, ANIM_RipCheato_Talk, ANIM_RipCheato_Idle, 0, MSG_MGM_0004)
    Return
    End
};

EvtScript EVS_NpcInit_F_Bandit = {
    Call(BindNpcInteract, NPC_SELF, Ref(EVS_NpcInteract_F_Bandit))
    Return
    End
};

NpcData NpcData_F_Bandit = {
    .id = NPC_F_Bandit,
    .settings = &N(NpcSettings_Bandit_Stationary),
    .pos = { GEN_F_BANDIT_VEC },
    .flags = COMMON_PASSIVE_FLAGS,
    .init = &EVS_NpcInit_F_Bandit,
    .yaw = GEN_F_BANDIT_DIR,
    .drops = NO_DROPS,
    .animations = BANDIT_ANIMS,
};

#include "world/common/enemy/Gloomba_Stationary.inc.c"

EvtScript EVS_NpcInteract_F_Gloomba = {
    // Call(SpeakToPlayer, NPC_RipCheato, ANIM_RipCheato_Talk, ANIM_RipCheato_Idle, 0, MSG_MGM_0004)
    Return
    End
};

EvtScript EVS_NpcInit_F_Gloomba = {
    Call(BindNpcInteract, NPC_SELF, Ref(EVS_NpcInteract_F_Gloomba))
    Return
    End
};

NpcData NpcData_F_Gloomba = {
    .id = NPC_F_Gloomba,
    .settings = &N(NpcSettings_Gloomba_Stationary),
    .pos = { GEN_F_GLOOMBA_VEC },
    .flags = COMMON_PASSIVE_FLAGS,
    .init = &EVS_NpcInit_F_Gloomba,
    .yaw = GEN_F_GLOOMBA_DIR,
    .drops = NO_DROPS,
    .animations = GLOOMBA_ANIMS,
};

NpcGroupList DefaultNPCs = {
    NPC_GROUP(NpcData_HarryT),
    NPC_GROUP(NpcData_F_Bobomb),
    NPC_GROUP(NpcData_RipCheato),
    NPC_GROUP(NpcData_ToadGuard),
    NPC_GROUP(NpcData_Fishmael),
    NPC_GROUP(NpcData_F_Bandit),
    NPC_GROUP(NpcData_F_Gloomba),
    {},
};

EvtScript EVS_Main = {
    Call(SetSpriteShading, SHADING_NONE)
    EVT_SETUP_CAMERA_NO_LEAD(0, 0, 0)
    // Call(SetMusicTrack, 0, SONG_SHOOTING_STAR_SUMMIT, 0, 8)
    Exec(EVS_TexPan)
    // Call(DisablePlayerInput, TRUE)
    // Call(DisablePlayerPhysics, TRUE)
    Call(MakeNpcs, TRUE, Ref(DefaultNPCs))
    Return
    End
};

}; // namespace prt_00

MapSettings prt_00_settings = {
    .main = &prt_00::EVS_Main,
    .entryList = &prt_00::Entrances,
    .entryCount = ENTRY_COUNT(prt_00::Entrances),
    .background = &gBackgroundImage,
};

s32 prt_00_map_init(void) {
    sprintf(wMapBgName, "net_bg");
    return FALSE;
}
