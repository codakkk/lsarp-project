
#include <YSI_Coding\y_hooks>

static DCC_Channel:Channel;

hook OnGameModeInit()
{
    DCC_Connect("NjIwMzMzMzMyODk0NTgwNzM2.XXVR1Q.SnXVh8Q7MUMXtmCniEH-MN_Delk");
    Channel = DCC_FindChannelById("513395781026185227");
    DCC_SendChannelMessage(Channel, "Porcodio è un test.");
    return 1;
}

