#include <YSI\y_hooks>

forward OnPlayerInventoryItemAdd(playerid, slotid, amount);
forward OnPlayerInventoryItemDecrease(playerid, slotid, amount);

forward OnPlayerInventoryChanged(playerid, slot_id);
forward OnPlayerInventoryItemUse(playerid, slot_id, item_id, item_type); // Hooked becomes: OnPlayerInvItemUse

stock Trigger_OnPlayerInvItemUse(playerid, slot_id, item_id, ITEM_TYPE:item_type)
{
    CallLocalFunction(#OnPlayerInventoryItemUse, "iiii", playerid, slot_id, item_id, _:item_type);
}

stock Trigger_OnPlayerInvChange(playerid, slot_id)
{
    CallLocalFunction(#OnPlayerInventoryChanged, "ii", playerid, slot_id);
}

public OnPlayerInventoryItemUse(playerid, slot_id, item_id)
{
    printf("inventory_callbacks.pwn/OnPlayerInvItem", playerid);
    /*if(ServerItem_IsBag(item_id))
    {
        //Character_EquipBag(playerid, slotid);
        Character_AMe(playerid, "indossa lo zaino");
        SendClientMessage(playerid, COLOR_GREEN, "Stai indossando uno zaino. Usa /rimuovi zaino per rimuoverlo.");
        pInventoryBag[playerid] = item_id;
    }*/
    return 1;
}