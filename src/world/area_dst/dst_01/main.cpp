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

NpcGroupList DefaultNPCs = {
    // NPC_GROUP(NpcData_RussT),
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
    // Call(MakeNpcs, TRUE, Ref(DefaultNPCs))
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
