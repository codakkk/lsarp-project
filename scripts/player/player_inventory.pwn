#include <YSI\y_hooks>

hook OnCharacterSaveData(playerid)
{
    Character_SaveInventory(playerid);
    return 1;
}

hook OnPlayerClearData(playerid)
{
    Character_ResetInventory(playerid);
    return 1;
}

// Returns success or not.
// if greater than 0, returns the difference.
stock Character_GiveItem(playerid, item_id, amount = 1)
{
    if(!ServerItem_IsValid(item_id))
        return INVENTORY_FAILED_INVALID_ITEM;
    if(amount <= 0)
        return INVENTORY_FAILED_INVALID_AMOUNT;
    if(Character_HasSpaceForItem(playerid, item_id, amount))
        return 0;
    if(ServerItem_IsUnique(item_id))
    {
        new freeid = Character_GetFreeSlot(playerid);
        if(freeid < 0)
            return INVENTORY_NO_SPACE;
        PlayerInventory[playerid][freeid][pInvItem] = item_id;
        PlayerInventory[playerid][freeid][pInvAmount] = 1;
        PlayerInventory[playerid][freeid][pInvExtra] = 0;
        Iter_Add(PlayerItemsSlot[playerid], freeid);
        CallLocalFunction("OnPlayerInventoryChanged", "ii", playerid, freeid);
        //CallLocalFunction("OnPlayerInventoryItemAdd", "iii", playerid, freeid, 1);
    }
    else
    {
        new 
            hasItem_slot = Character_HasItem(playerid, item_id);
        if(hasItem_slot != -1 && PlayerInventory[playerid][hasItem_slot][pInvAmount] < ServerItem[item_id][sitemMaxStack])
        {
            PlayerInventory[playerid][hasItem_slot][pInvAmount] += amount;
            //CallLocalFunction("OnPlayerInventoryItemAdd", "iii", playerid, freeid, amount);
            CallLocalFunction("OnPlayerInventoryChanged", "ii", playerid, hasItem_slot);
            new diff = PlayerInventory[playerid][hasItem_slot][pInvAmount] - ServerItem_GetMaxStack(item_id);
            if(diff > 0) // if exceed maximum stack size, try to add them in another slot.
            {
                PlayerInventory[playerid][hasItem_slot][pInvAmount] = ServerItem[item_id][sitemMaxStack]; // Cap
                // Check if playerid has a free slot to add the difference.
                new freeid = Character_GetFreeSlot(playerid);
                if(freeid == -1) // If playerid has no free slot, returns the difference
                    return diff;
                // Else, recall GiveItem and give item_id by difference..
                return Character_GiveItem(playerid, item_id, diff);
            }
        }
        else
        {
            new freeid = Character_GetFreeSlot(playerid);
            if(freeid < 0)
                return INVENTORY_NO_SPACE;
            PlayerInventory[playerid][freeid][pInvItem] = item_id;
            PlayerInventory[playerid][freeid][pInvAmount] = amount;
            PlayerInventory[playerid][freeid][pInvExtra] = 0;
            Iter_Add(PlayerItemsSlot[playerid], freeid);
            //CallLocalFunction("OnPlayerInventoryItemAdd", "iii", playerid, freeid, amount);
            CallLocalFunction("OnPlayerInventoryChanged", "ii", playerid, freeid);
            if(PlayerInventory[playerid][freeid][pInvAmount] > ServerItem[item_id][sitemMaxStack])
            {
                new diff = PlayerInventory[playerid][freeid][pInvAmount] - ServerItem[item_id][sitemMaxStack];
                PlayerInventory[playerid][freeid][pInvAmount] = ServerItem[item_id][sitemMaxStack];
                return Character_GiveItem(playerid, item_id, diff);
            }
        }
    }
    return INVENTORY_ADD_SUCCESS;
}

stock Character_HasSpaceForItem(playerid, itemid, amount)
{
    new 
        tempAmount = amount,
        tempCurrentQuantity = 0;
    foreach(new i : PlayerItemsSlot[playerid])
    {
        tempCurrentQuantity = Character_GetItemAmount(playerid, i);
        while(Character_GetItem(playerid, i) == itemid && tempCurrentQuantity < ServerItem_GetMaxStack(itemid))
        {
            tempCurrentQuantity++;
            tempAmount--;
        }
        if(tempAmount <= 0)
            break;
    }
    new currentFreeSlotCount = Character_GetFreeSlotCount(playerid);
    if(tempAmount > 0 && currentFreeSlotCount == 0)
        return 0;
    else
    {
        new 
            occupiedSlots = 0;
        for(new i = 0; i < MAX_ITEMS_PER_PLAYER; ++i)
        {
            if(tempAmount == 0 || occupiedSlots >= currentFreeSlotCount)
                break;
            if(Character_GetItem(playerid, i) == 0)
            {
                tempAmount -= ServerItem[itemid][sitemMaxStack];
                occupiedSlots++;
            }
        }
    }
    return tempAmount <= 0;
}

stock Character_DecreaseItemAmount(playerid, itemid, amount = 1)
{
    return Character_DecreaseItemBySlot(playerid, Character_HasItem(playerid, itemid), amount);
}

stock Character_DecreaseItemBySlot(playerid, slotid, amount = 1)
{
    if(slotid == -1 || PlayerInventory[playerid][slotid][pInvItem] == 0 || amount <= 0)
        return INVENTORY_DECREASE_FAILED;
    new diff = amount - PlayerInventory[playerid][slotid][pInvAmount];
    PlayerInventory[playerid][slotid][pInvAmount] -= amount;
    CallLocalFunction("OnPlayerInventoryItemRemove", "iii", playerid, slotid, amount);
    CallLocalFunction("OnPlayerInventoryChanged", "ii", playerid, slotid);
    if(PlayerInventory[playerid][slotid][pInvAmount] <= 0)
    {
        new itemid = Character_GetItem(playerid, slotid);
        Character_RemoveItem(playerid, slotid);
        // Search for others if diff > 0
        if(diff > 0)
        {
            return Character_DecreaseItemAmount(playerid, itemid, diff);
        }
    }
    return INVENTORY_DECREASE_SUCCESS;
}

stock Character_GetInventorySize(playerid)
{
    new 
        add = 0,
        bagItemId = pInventoryBag[playerid];
    if(bagItemId != 0 && ServerItem_GetType(bagItemId) == ITEM_TYPE:ITEM_TYPE_BAG)
    {
        add += ServerItem[bagItemId][sitemExtraData][EXTRA_TYPE_ID:EXTRA_BAG_SIZE];
    }
    return PLAYER_INVENTORY_START_SIZE + add;
}

stock Character_RemoveItem(playerid, slotid)
{
    PlayerInventory[playerid][slotid][pInvItem] = 0;
    PlayerInventory[playerid][slotid][pInvAmount] = 0;
    PlayerInventory[playerid][slotid][pInvExtra] = 0;
    Iter_Remove(PlayerItemsSlot[playerid], slotid);
    return 1;
}

stock Character_HasItem(playerid, itemid)
{
    foreach(new i : PlayerItemsSlot[playerid])
    {
        if(PlayerInventory[playerid][i][pInvItem] == itemid)
            return i;
    }
    return -1;
}

stock Character_GetItem(playerid, slotid)
{
    return PlayerInventory[playerid][slotid][pInvItem];
}

stock Character_GetItemAmount(playerid, slotid)
{
    return PlayerInventory[playerid][slotid][pInvAmount];
}

stock Character_GetFreeSlot(playerid)
{
    if(Iter_Count(PlayerItemsSlot[playerid]) >= Character_GetInventorySize(playerid))
        return -1;
    new f = Iter_Free(PlayerItemsSlot[playerid]);
    return f;
}

stock Character_GetFreeSlotCount(playerid)
{
    return Character_GetInventorySize(playerid) - Iter_Count(PlayerItemsSlot[playerid]);
}

stock Character_ShowInventory(playerid, targetid)
{
    if(!gCharacterLogged[playerid])
        return 0;
    if(Iter_Count(PlayerItemsSlot[playerid]) == 0)
        return SendClientMessage(playerid, COLOR_ERROR, "L'inventario è vuoto!");
    new
        string[4096],
        temp[16],
        count = 0
    ;
    foreach(new slotid : PlayerItemsSlot[playerid])
    {
        new itemid = Character_GetItem(playerid, slotid);
        if(ServerItem_IsUnique(itemid))
            temp = "--";
        else
            format(temp, sizeof(temp), "%d", Character_GetItemAmount(playerid, slotid));
        format(string, sizeof(string), "%s%s\t%s\t%s\n", string, ServerItem[itemid][sitemName], temp, ServerItem_GetTypeName(itemid));
        pInventoryListItem[playerid][count++] = slotid;
    }
    new title[48];
    format(title, sizeof(title), "Inventario (%d/%d)", count, Character_GetInventorySize(playerid));
    Dialog_Show(playerid, Dialog_InventoryItemList, DIALOG_STYLE_TABLIST_HEADERS, title, "Nome\tQuantità\tTipo\n%s", "Avanti", "Chiudi", string);
    return 1;
}

Dialog:Dialog_InventoryItemList(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    new 
        title[48],
        slotid = pInventoryListItem[playerid][listitem],
        itemid = Character_GetItem(playerid, slotid);
    format(title, sizeof(title), "%s (Quantità: %d)", ServerItem_GetName(itemid), Character_GetItemAmount(playerid, slotid));
    Dialog_Show(playerid, Dialog_InventoryItemAction, DIALOG_STYLE_LIST, title, "Usa\nGetta\nDai a un giocatore", "Continua", "Annulla");
    return 1;
}


stock Character_ResetInventory(playerid)
{
    /*
        new x[E_PLAYER_INVENTORY_DATA];
        PlayerInventory[playerid] = x;
    */
    for(new i = 0; i < MAX_ITEMS_PER_PLAYER; ++i)
    {
        PlayerInventory[playerid][i][pInvItem] = 0;
        PlayerInventory[playerid][i][pInvAmount] = 0;
        PlayerInventory[playerid][i][pInvExtra] = 0;
    }
    memset(pInventoryListItem[playerid], -1);
    return 1;
}

stock Character_SaveInventory(playerid)
{
    if(!gCharacterLogged[playerid])
        return 0;
    return 1;
}