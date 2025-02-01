#include "mac_06.h"
#include "online/online.h"

namespace mac_06 {

API_CALLABLE(AwaitGateway) {
    if (online::is_connected_to_gateway()) {
        return ApiStatus_DONE2;
    } else {
        return ApiStatus_BLOCK;
    }
}

API_CALLABLE(AwaitServer) {
    if (online::is_connected_to_server()) {
        return ApiStatus_DONE2;
    } else {
        return ApiStatus_BLOCK;
    }
}

extern "C" EvtScript mac_06_EVS_EstablishOnlineConnection = {
    Call(ShowMessageAtScreenPos, MSG_Online_SearchingForGateway, 160, 40)
    Call(AwaitGateway)
    Call(SwitchMessage, MSG_Online_ConnectingToServer)
    Call(AwaitServer)
    Call(SwitchMessage, MSG_Online_Connected)
    Call(CloseMessage)
    Wait(20)
    Call(PlayAmbientSounds, AMBIENT_SILENCE)
    Call(GotoMapSpecial, Ref("machi"), 0, TRANSITION_MARIO_BLACK)
    Return
    End
};

}; // namespace mac_06
