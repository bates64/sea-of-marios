#include "dst_00.h"
#include "dx/debug_menu.h"

namespace dst_00 {

EntryList Entrances = { GEN_ENTRY_LIST };

#include "world/common/atomic/TexturePan.inc.c"

EvtScript EVS_TexPan = {
    SetGroup(EVT_GROUP_NEVER_PAUSE)
    Call(SetTexPanner, MODEL_Waves, TEX_PANNER_1)
    Thread
        GEN_TEX_PANNER_1
        Exec(EVS_UpdateTexturePan)
    EndThread
    Call(SetTexPanner, MODEL_WaterSurface, TEX_PANNER_2)
    Thread
        GEN_TEX_PANNER_2
        Exec(EVS_UpdateTexturePan)
    EndThread
    Return
    End
};

#include "world/common/npc/RussT.inc.c"

EvtScript EVS_NpcInit_RussT = {

    Return
    End
};

NpcData NpcData_RussT = {
    .id = NPC_DockRuss,
    .settings = &NpcSettings_RussT,
    .pos = { GEN_DOCK_NPC_VEC },
    .flags = COMMON_PASSIVE_FLAGS,
    .init = &EVS_NpcInit_RussT,
    .yaw = GEN_DOCK_NPC_DIR,
    .drops = NO_DROPS,
    .animations = RUSS_T_ANIMS,
};

NpcGroupList DefaultNPCs = {
    NPC_GROUP(NpcData_RussT),
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

}; // namespace dst_00

MapSettings dst_00_settings = {
    .main = &dst_00::EVS_Main,
    .entryList = &dst_00::Entrances,
    .entryCount = ENTRY_COUNT(dst_00::Entrances),
    .background = &gBackgroundImage,
};

s32 dst_00_map_init(void) {
    sprintf(wMapBgName, "net_bg");
    return FALSE;
}
