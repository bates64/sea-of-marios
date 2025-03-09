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

NpcData NpcData_Pokey1 = {
    .id = NPC_Pokey1,
    .settings = &N(NpcSettings_Pokey),
    .pos = { GEN_POKEY1_VEC },
    .flags = ENEMY_FLAG_FLYING,
    .yaw = GEN_POKEY1_DIR,
    .drops = POKEY_DROPS,
    .territory = GEN_POKEY1_TERRITORY,
    .animations = POKEY_ANIMS,
    .aiDetectFlags = AI_DETECT_SIGHT,
};

NpcData NpcData_Pokey2 = {
    .id = NPC_Pokey2,
    .settings = &N(NpcSettings_Pokey),
    .pos = { GEN_POKEY2_VEC },
    .flags = ENEMY_FLAG_FLYING,
    .yaw = GEN_POKEY2_DIR,
    .drops = POKEY_DROPS,
    .territory = GEN_POKEY2_TERRITORY,
    .animations = POKEY_ANIMS,
    .aiDetectFlags = AI_DETECT_SIGHT,
};

#include "world/common/enemy/Bandit.inc.c"

NpcData NpcData_Bandit1 = {
    .id = NPC_Bandit1,
    .settings = &N(NpcSettings_Bandit),
    .pos = { GEN_BANDIT1_VEC },
    .flags = ENEMY_FLAG_IGNORE_ENTITY_COLLISION | ENEMY_FLAG_FLYING | ENEMY_FLAG_NO_SHADOW_RAYCAST,
    .yaw = GEN_BANDIT1_DIR,
    .drops = BANDIT_DROPS,
    .territory = GEN_BANDIT1_TERRITORY,
    .animations = BANDIT_ANIMS,
    .aiDetectFlags = AI_DETECT_SIGHT,
};

NpcData NpcData_Bandit2 = {
    .id = NPC_Bandit2,
    .settings = &N(NpcSettings_Bandit),
    .pos = { GEN_BANDIT2_VEC },
    .flags = ENEMY_FLAG_IGNORE_ENTITY_COLLISION | ENEMY_FLAG_FLYING | ENEMY_FLAG_NO_SHADOW_RAYCAST,
    .yaw = GEN_BANDIT2_DIR,
    .drops = BANDIT_DROPS,
    .territory = GEN_BANDIT2_TERRITORY,
    .animations = BANDIT_ANIMS,
    .aiDetectFlags = AI_DETECT_SIGHT,
};

#include "world/common/enemy/Cleft.inc.c"

NpcData NpcData_Cleft = {
    .id = NPC_Cleft,
    .settings = &N(NpcSettings_Cleft),
    .pos = { GEN_CLEFT_VEC },
    .flags = ENEMY_FLAG_IGNORE_ENTITY_COLLISION | ENEMY_FLAG_FLYING,
    .yaw = GEN_CLEFT_DIR,
    .drops = CLEFT_DROPS,
    .territory = GEN_CLEFT_TERRITORY,
    .animations = CLEFT_ANIMS,
    .aiDetectFlags = AI_DETECT_SIGHT,
};

NpcGroupList DefaultNPCs = {
    NPC_GROUP((NpcData_Pokey1), BTL_KMR_1_FORMATION_04, BTL_KMR_1_STAGE_00),
    NPC_GROUP((NpcData_Pokey2), BTL_KMR_1_FORMATION_04, BTL_KMR_1_STAGE_00),
    NPC_GROUP((NpcData_Bandit1), BTL_KMR_1_FORMATION_05, BTL_KMR_1_STAGE_00),
    NPC_GROUP((NpcData_Bandit2), BTL_KMR_1_FORMATION_05, BTL_KMR_1_STAGE_00),
    NPC_GROUP((NpcData_Cleft), BTL_KMR_1_FORMATION_06, BTL_KMR_1_STAGE_00),
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
