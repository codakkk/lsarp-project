#include <YSI_Coding\y_hooks>

forward OnPlayerInventoryItemAdd(playerid, slotid, amount);
forward OnPlayerInventoryItemDecrease(playerid, slotid, amount);

forward OnPlayerInventoryChanged(playerid, slot_id);
forward OnPlayerInventoryItemUse(playerid, slot_id, item_id, item_type); // Hooked becomes: OnPlayerInvItemUse

stock Trigger_OnPlayerInvItemUse(playerid, item_id, amount, ITEM_TYPE:item_type)
{
    CallLocalFunction(#OnPlayerInventoryItemUse, "iiii", playerid, item_id, amount, _:item_type);
}

stock Trigger_OnPlayerInvChange(playerid, slot_id)
{
    CallLocalFunction(#OnPlayerInventoryChanged, "ii", playerid, slot_id);
}

public OnPlayerInventoryItemUse(playerid, slot_id, item_id)
{
    return 1;
}