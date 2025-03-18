#include "net_01.hpp"
#include "effects.h"
#include "dx/debug_menu.h"

namespace net_01 {

EntryList Entrances = { GEN_ENTRY_LIST };

EvtScript EVS_EnterMap = {
    Call(GetEntryID, LVar0)
    Switch(LVar0)
    CaseEq(0)
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
