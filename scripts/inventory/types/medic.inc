#include <YSI_Coding\y_hooks>

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
    
    current += float(healthGain);

    if(current > 100.0)
        current = 100.0;
    
    AC_SetPlayerHealth(playerid, current);
    Player_InfoStr(playerid, str_format("Hai recuperato ~r~%d HP~w~ utilizzando %s.", healthGain, ServerItem[itemid][sitemName]), true, 5000);

    Character_DecreaseSlotAmount(playerid, slotid, 1);
    return 1;
}