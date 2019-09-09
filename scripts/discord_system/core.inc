#include <discord-connector>
#include <YSI_Coding\y_hooks>

static DCC_Channel:Channel;

hook OnGameModeInit()
{
    if(_:Channel == 0)
        Channel = DCC_FindChannelById("618468788702281740");
    printf("Channel: %d", _:Channel);
    DCC_SendChannelMessage(Channel, "Porcodio e un test.");
    return 1;
}

