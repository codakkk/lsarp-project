#include <YSI_Coding\y_hooks>
 
static bool:PlayerCP[MAX_PLAYERS char] = {false, ...};
 
flags:disablecheckpoints(CMD_USER);
CMD:disablecheckpoints(playerid, params[])
{
 
    if(!PlayerCP{playerid}) SendClientMessage(playerid, COLOR_YELLOW, "La visualizzazione dei checkpoints è stata abilitata.");
    else SendClientMessage(playerid, COLOR_YELLOW, "La visualizzazione dei checkpoints è stata disabilitata.");
 
    PlayerCP{playerid} = !PlayerCP{playerid};
 
    TogglePlayerAllDynamicCPs(playerid, PlayerCP{playerid});
 
    return 1;
}

hook OnCharacterClearData(playerid)
{
    PlayerCP{playerid} = true;
    TogglePlayerAllDynamicCPs(playerid, true);
    return 1;
}