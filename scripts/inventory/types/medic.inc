#include <inventory\server>
#include <YSI_Coding\y_hooks>

static Timer:GainHealthTimer[MAX_PLAYERS] = {Timer:0, ...};

hook OnPlayerClearData(playerid)
{
    if(GainHealthTimer[playerid] != Timer:0)
        stop GainHealthTimer[playerid];
    GainHealthTimer[playerid] = Timer:0;
    return 1;
}

hook OnCharacterDeath(playerid, killerid, reason)
{
    if(GainHealthTimer[playerid] != Timer:0)
        stop GainHealthTimer[playerid];
    GainHealthTimer[playerid] = Timer:0;
    return 1;
}

hook OnCharacterSpawn(playerid)
{
    if(GainHealthTimer[playerid] != Timer:0)
        stop GainHealthTimer[playerid];
    GainHealthTimer[playerid] = Timer:0;
    return 1;
}

hook OnCharacterUseItem(playerid, slotid, itemid)
{
    if(!ServerItem_IsMedic(itemid))
        return 0;
    new
        healthGain = ServerItem_GetExtra(itemid, 0)
    ;
    if(healthGain == 0)
        return 1;
    
    new Float:current;
    AC_GetPlayerHealth(playerid, current);
    
    if(current >= 100.0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo oggetto con 100 o più HP.");
    
    SendFormattedMessage(playerid, -1, "Hai utilizzato l'oggetto (%s). Avrà effetto tra 10 secondi.", ServerItem_GetName(itemid));

    GainHealthTimer[playerid] = defer GainHealthAfterDelay(playerid, itemid);

    Character_DecreaseSlotAmount(playerid, slotid, 1);
    return 1;
}

timer GainHealthAfterDelay[10000](playerid, itemid) 
{
    new
        healthGain = ServerItem_GetExtra(itemid, 0)
    ;
    
    new Float:current;
    AC_GetPlayerHealth(playerid, current);

    current += float(healthGain);

    if(current > 100.0)
        current = 100.0;
    
    AC_SetPlayerHealth(playerid, current);

    Player_InfoStr(playerid, str_format("Hai recuperato ~r~%d HP~w~ utilizzando %s.", healthGain, ServerItem_GetName(itemid)), true, 5000);

    GainHealthTimer[playerid] = Timer:0;
}