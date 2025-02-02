#include "net_00.h"

namespace net_00 {

EntryList Entrances = { GEN_ENTRY_LIST };

#include "world/common/atomic/TexturePan.inc.c"

EvtScript EVS_TexPan = {
    SetGroup(EVT_GROUP_NEVER_PAUSE)
    Call(SetTexPanner, MODEL_bg, TEX_PANNER_C)
    Thread
        TEX_PAN_PARAMS_ID(TEX_PANNER_C)
        TEX_PAN_PARAMS_STEP(  -64,  -60,   32,   16)
        TEX_PAN_PARAMS_FREQ(    1,    1,    1,    1)
        TEX_PAN_PARAMS_INIT(    0,    0,    0,    0)
        Exec(EVS_UpdateTexturePan)
    EndThread
    Return
    End
};

#include "world/common/npc/StarSpirit.inc.c"

EvtScript EVS_NpcInit_Muskular = {
    // TODO: personality quiz
    Return
    End
};

NpcData NpcData_Muskular = {
    .id = NPC_Muskular,
    .settings = &NpcSettings_StarSpirit,
    .pos = { GEN_MUSKULAR_VEC },
    .flags = COMMON_PASSIVE_FLAGS,
    .init = &EVS_NpcInit_Muskular,
    .yaw = 0,
    .drops = NO_DROPS,
    .animations = MUSKULAR_ANIMS,
};

NpcGroupList DefaultNPCs = {
    NPC_GROUP(N(NpcData_Muskular)),
    {},
};

EvtScript EVS_Main = {
    Call(SetSpriteShading, SHADING_NONE)
    EVT_SETUP_CAMERA_NO_LEAD(0, 0, 0)
    Call(SetMusicTrack, 0, SONG_SHOOTING_STAR_SUMMIT, 0, 8)
    Exec(EVS_TexPan)
    Call(DisablePlayerInput, TRUE)
    Call(MakeNpcs, TRUE, Ref(DefaultNPCs))
    Return
    End
};

}; // namespace net_00

MapSettings net_00_settings = {
    .main = &net_00::EVS_Main,
    .entryList = &net_00::Entrances,
    .entryCount = ENTRY_COUNT(net_00::Entrances),
    .background = &gBackgroundImage,
};

 s32 net_00_map_init(void) {
    sprintf(wMapTexName, "hos_tex");
    return FALSE;
 }
