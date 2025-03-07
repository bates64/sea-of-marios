#include "dst_05.h"
#include "dx/debug_menu.h"

namespace dst_05 {

EntryList Entrances = { GEN_ENTRY_LIST };

EvtScript EVS_Main = {
    Call(SetSpriteShading, SHADING_NONE)
    EVT_SETUP_CAMERA_NO_LEAD(0, 0, 0)
    // Call(SetMusicTrack, 0, SONG_SHOOTING_STAR_SUMMIT, 0, 8)
    Return
    End
};

}; // namespace dst_05

MapSettings dst_05_settings = {
    .main = &dst_05::EVS_Main,
    .entryList = &dst_05::Entrances,
    .entryCount = ENTRY_COUNT(dst_05::Entrances),
};
