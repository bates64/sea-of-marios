#include "mac_06.h"

extern "C" EvtScript mac_06_EVS_EstablishOnlineConnection = {
    Call(ShowMessageAtScreenPos, MSG_Online_SearchingForGateway, 160, 40)
    WaitSecs(5)
    Call(SwitchMessage, MSG_Online_ConnectingToInternet)
    WaitSecs(5)
    //Call(CloseMessage)
    Return
    End
};
