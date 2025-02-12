#include "net_01.h"
#include "effects.h"
#include "dx/debug_menu.h"

namespace net_01 {

EntryList Entrances = { GEN_ENTRY_LIST };

#include "world/common/atomic/TexturePan.inc.c"

// EvtScript EVS_TexPan = {
//     SetGroup(EVT_GROUP_NEVER_PAUSE)
//     Call(SetTexPanner, MODEL_bg, TEX_PANNER_C)
//     Thread
//         TEX_PAN_PARAMS_ID(TEX_PANNER_C)
//         TEX_PAN_PARAMS_STEP(  -64,  -60,   32,   16)
//         TEX_PAN_PARAMS_FREQ(    1,    1,    1,    1)
//         TEX_PAN_PARAMS_INIT(    0,    0,    0,    0)
//         Exec(EVS_UpdateTexturePan)
//     EndThread
//     Return
//     End
// };

EvtScript EVS_Main = {
    Call(SetSpriteShading, SHADING_NONE)
    EVT_SETUP_CAMERA_NO_LEAD(0, 0, 0)
    Call(ClearAmbientSounds, 250)
    Call(FadeOutMusic, 0, 1000)
    Call(PlayAmbientSounds, AMBIENT_SEA)
    // Exec(EVS_TexPan)
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
