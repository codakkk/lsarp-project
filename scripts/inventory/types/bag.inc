#include <inventory\server>
#include <YSI_Coding\y_hooks>

hook OnCharacterUseItem(playerid, slotid, itemid)
{
    if(!ServerItem_IsBag(itemid))
        return 0;

    if(Character_HasBag(playerid))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Stai già indossando uno zaino! Usa '/rimuovi zaino' per rimuoverlo.");
        return 0;
    }

    Character_AMe(playerid, "indossa lo zaino.");
    //SendFormattedMessage(playerid, COLOR_GREEN, "Stai indossando '%s'. Usa '/rimuovi zaino' per rimuoverlo.", ServerItem_GetName(item_id));
    Character_SetBag(playerid, itemid);
    Player_InfoStr(playerid, str_format("Stai indossando: ~g~%s", ServerItem_GetName(itemid)), false);
    Character_DecreaseSlotAmount(playerid, slotid, 1);
    return 1;
}