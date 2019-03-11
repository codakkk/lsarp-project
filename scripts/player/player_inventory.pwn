#include <YSI\y_hooks>

hook OnCharacterSaveData(playerid)
{
    Character_SaveInventory(playerid);
    return 1;
}

hook OnPlayerCharacterLoad(playerid)
{
    printf("player_inventory.pwn/OnPlayerCharacterLoad");
    Character_LoadInventory(playerid);
    return 1;
}

hook OnPlayerClearData(playerid)
{
    Character_ResetInventory(playerid);
    printf("player_inventory.pwn/OnPlayerClearData");
    return 1;
}

// Returns success or not.
// if greater than 0, returns the difference.
stock Character_GiveItem(playerid, item_id, amount = 1, bool:callback = false)
{
    if(!ServerItem_IsValid(item_id))
        return INVENTORY_FAILED_INVALID_ITEM;
    if(amount < 0)
        return Character_DecreaseItemAmount(playerid, item_id, amount);
    if(!Character_HasSpaceForItem(playerid, item_id, amount))
        return INVENTORY_NO_SPACE;
    if(ServerItem_IsUnique(item_id))
    {
        new freeid = -1, tempAmount = amount; //Character_GetFreeSlot(playerid);
        while(tempAmount > 0 && (freeid = Character_GetFreeSlot(playerid)) >= 0)
        {
            //if(freeid < 0) // Should never happen
                //return INVENTORY_NO_SPACE; // Should never happen
            PlayerInventory[playerid][freeid][pInvItem] = item_id;
            PlayerInventory[playerid][freeid][pInvAmount] = 1;
            PlayerInventory[playerid][freeid][pInvExtra] = 0;
            Iter_Add(PlayerItemsSlot[playerid], freeid);
            //CallLocalFunction("OnPlayerInventoryItemAdd", "iii", playerid, freeid, 1);
            tempAmount--;
        }
        if(callback)
            CallLocalFunction("OnPlayerInventoryChanged", "ii", playerid, freeid);
    }
    else
    {
        new 
            hasItem_slot = Character_HasItem(playerid, item_id);
        if(hasItem_slot != -1 && Character_GetItemAmount(playerid, hasItem_slot) < ServerItem_GetMaxStack(item_id))
        {
            PlayerInventory[playerid][hasItem_slot][pInvAmount] += amount;
            if(callback)
                CallLocalFunction("OnPlayerInventoryChanged", "ii", playerid, hasItem_slot);
            new diff = PlayerInventory[playerid][hasItem_slot][pInvAmount] - ServerItem_GetMaxStack(item_id);
            if(diff > 0) // if exceed maximum stack size, try to add them in another slot.
            {
                PlayerInventory[playerid][hasItem_slot][pInvAmount] = ServerItem[item_id][sitemMaxStack]; // Cap
                // Check if playerid has a free slot to add the difference.
                //new freeid = Character_GetFreeSlot(playerid);
                //if(freeid == -1) // If playerid has no free slot, returns the difference
                    //return diff;
                    // Diff isn't used anymore, because now player should have space anyway,
                    // because we check for it earlier
                // Else, recall GiveItem and give item_id by difference..
                return Character_GiveItem(playerid, item_id, diff);
            }
        }
        else
        {
            new freeid = Character_GetFreeSlot(playerid);
            // if(freeid < 0) // This should never happen too now.
                //return INVENTORY_NO_SPACE; // This should never happen too now.
            PlayerInventory[playerid][freeid][pInvItem] = item_id;
            PlayerInventory[playerid][freeid][pInvAmount] = amount;
            PlayerInventory[playerid][freeid][pInvExtra] = 0;
            Iter_Add(PlayerItemsSlot[playerid], freeid);
            if(callback)
            {
                CallLocalFunction("OnPlayerInventoryItemAdd", "iii", playerid, freeid, amount);
                CallLocalFunction("OnPlayerInventoryChanged", "ii", playerid, freeid);
            }
            if(PlayerInventory[playerid][freeid][pInvAmount] > ServerItem[item_id][sitemMaxStack])
            {
                new diff = PlayerInventory[playerid][freeid][pInvAmount] - ServerItem[item_id][sitemMaxStack];
                PlayerInventory[playerid][freeid][pInvAmount] = ServerItem[item_id][sitemMaxStack];
                return Character_GiveItem(playerid, item_id, diff);
            }
        }
    }
    Character_SaveInventory(playerid);
    return INVENTORY_ADD_SUCCESS;
}

stock Character_HasSpaceForItem(playerid, itemid, amount)
{
    new 
        tempAmount = amount,
        tempCurrentQuantity = 0;
    if(!ServerItem_IsUnique(itemid))
    {
        foreach(new i : PlayerItemsSlot[playerid])
        {
            tempCurrentQuantity = Character_GetItemAmount(playerid, i);
            while(Character_GetItem(playerid, i) == itemid && tempCurrentQuantity < ServerItem_GetMaxStack(itemid))
            {
                tempCurrentQuantity++;
                tempAmount--;
            }
            //printf("Qnt: %d", tempCurrentQuantity);
            if(tempAmount <= 0)
                break;
        }
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
    if(itemid == 0 || amount <= 0)
        return INVENTORY_DECREASE_SUCCESS;
    new 
        tempItemId = 0,
        tempItemAmount = 0,
        tempDecreaseAmount = amount;
    foreach(new i : PlayerItemsSlot[playerid])
    {
        if(tempDecreaseAmount <= 0)
            break;
        tempItemId = Character_GetItem(playerid, i);
        if(itemid != tempItemId) continue; // Safeness
        tempItemAmount = Character_GetItemAmount(playerid, i);
        
        new diff = tempDecreaseAmount - tempItemAmount;
        if(diff >= 0)
        {
            PlayerInventory[playerid][i][pInvItem] = 0;
            PlayerInventory[playerid][i][pInvAmount] = 0;
            PlayerInventory[playerid][i][pInvExtra] = 0;
            Iter_SafeRemove(PlayerItemsSlot[playerid], i, i);
            tempDecreaseAmount = diff;
        }
        else 
        {
            PlayerInventory[playerid][i][pInvAmount] -= tempDecreaseAmount;
            tempDecreaseAmount = 0;
        }
    }
    CallLocalFunction("OnPlayerInventoryItemDecrease", "iii", playerid, itemid, amount);
    //CallLocalFunction("OnPlayerInventoryChanged", "ii", playerid, -1);
    Character_SaveInventory(playerid);
    return INVENTORY_DECREASE_SUCCESS;
}

/*stock Character_EquipBag(playerid, slotid)
{
    //new currentInventorySize = Character_GetInventorySize(playerid, slotid);
    new
        itemid = Character_GetItem(playerid, slotid);
    
    if(!ServerItem_IsBag(itemid))
        return 0;
    
    pInventoryBag[playerid] = itemid;
    return 1;
}
*/
/*
stock Character_DequipBag(playerid)
{
    if(pInventoryBag[playerid] == 0)
        return 0;
    if(Character_GetInventoryUsedSpace(playerid) > PLAYER_INVENTORY_START_SIZE)
        return 0;
    Character_GiveItem(playerid, pInventoryBag[playerid], 1);
    pInventoryBag[playerid] = 0;
    return 1;
}*/

stock Character_GetInventoryMaxSize(playerid)
{
    new 
        add = 0,
        bagItemId = pInventoryBag[playerid];
    if(bagItemId != 0 && ServerItem_IsBag(bagItemId))
    {
        new extra = ServerItem[bagItemId][sitemExtraData][0];
        add += extra;
    }
    return PLAYER_INVENTORY_START_SIZE + add;
}

stock Character_GetInventoryUsedSpace(playerid)
{
    return Iter_Count(PlayerItemsSlot[playerid]);
}

stock Character_RemoveItem(playerid, slotid)
{
    if(slotid < 0 || slotid >= MAX_ITEMS_PER_PLAYER)
        return 0;
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
    if(slotid < 0 || slotid >= MAX_ITEMS_PER_PLAYER)
        return 0;
    return PlayerInventory[playerid][slotid][pInvItem];
}

stock Character_GetItemAmount(playerid, slotid)
{
    if(slotid < 0 || slotid >= MAX_ITEMS_PER_PLAYER)
        return 0;
    return PlayerInventory[playerid][slotid][pInvAmount];
}

stock Character_GetFreeSlot(playerid)
{
    if(Iter_Count(PlayerItemsSlot[playerid]) >= Character_GetInventoryMaxSize(playerid))
        return -1;
    new f = Iter_Free(PlayerItemsSlot[playerid]);
    return f;
}

stock Character_GetFreeSlotCount(playerid)
{
    return Character_GetInventoryMaxSize(playerid) - Iter_Count(PlayerItemsSlot[playerid]);
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
    format(title, sizeof(title), "Inventario (%d/%d)", count, Character_GetInventoryMaxSize(playerid));
    Dialog_Show(targetid, Dialog_InventoryItemList, DIALOG_STYLE_TABLIST_HEADERS, title, "Nome\tQuantità\tTipo\n%s", "Avanti", "Chiudi", string);
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
    pInventorySelectedListItem[playerid] = listitem;
    format(title, sizeof(title), "%s (Quantità: %d)", ServerItem_GetName(itemid), Character_GetItemAmount(playerid, slotid));
    Dialog_Show(playerid, Dialog_InvItemAction, DIALOG_STYLE_LIST, title, "Usa\nGetta\nDai a un giocatore", "Continua", "Annulla");
    return 1;
}

Dialog:Dialog_InvItemAction(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
        return Character_ShowInventory(playerid, playerid);
    }
    switch(listitem)
    {
        case 0:
        {
            new selected = pInventorySelectedListItem[playerid],
                slotid = pInventoryListItem[playerid][selected],
                itemid = Character_GetItem(playerid, slotid);
            Trigger_OnPlayerInvItemUse(playerid, slotid, itemid, ServerItem_GetType(itemid));
            return 1;
        }
        case 1: return 1;
        case 2: return 1;
    }
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
    pInventoryBag[playerid] = 0;
    memset(pInventoryListItem[playerid], -1);
    Iter_Clear(PlayerItemsSlot[playerid]);
    return 1;
}

stock Character_SaveInventory(playerid)
{
    if(!gCharacterLogged[playerid])
        return 0;

    new
        query[1024],
        tempItems[256],
        tempAmounts[256]
        ;
    foreach(new i : PlayerItemsSlot[playerid])
    {
        format(tempItems, sizeof(tempItems), "%s%d%c", tempItems, Character_GetItem(playerid, i), '|');
        format(tempAmounts, sizeof(tempAmounts), "%s%d%c", tempAmounts, Character_GetItemAmount(playerid, i), '|');
    }
    mysql_format(gMySQL, query, sizeof(query), "INSERT INTO `character_inventory` \
    (CharacterID, Items, ItemsAmount, EquippedBag) VALUES('%d', '%e', '%e', '%d') ON DUPLICATE KEY UPDATE \
    Items = VALUES(Items), ItemsAmount = VALUES(ItemsAmount), EquippedBag = VALUES(EquippedBag)",
    PlayerInfo[playerid][pID], tempItems, tempAmounts, pInventoryBag[playerid]
    );
    mysql_tquery(gMySQL, query);
    return 1;
}

stock Character_LoadInventory(playerid)
{
    if(!gCharacterLogged[playerid])
        return 0;
    inline OnLoad()
    {
        Character_ResetInventory(playerid); // Probably useless.
        if(cache_num_rows() > 0)
        {
            new 
                temp[256], 
                tempItems[MAX_ITEMS_PER_PLAYER],
                tempAmounts[MAX_ITEMS_PER_PLAYER],
                tempExtraData[MAX_ITEMS_PER_PLAYER],
                sscanf_format[16];

            format(sscanf_format, sizeof(sscanf_format), "p<|>a<i>[%d]", MAX_ITEMS_PER_PLAYER);

            // Load Items Ids
            cache_get_value_index(0, 0, temp, sizeof(temp));
            sscanf(temp, sscanf_format, tempItems);
            // Load Items Amounts
            cache_get_value_index(0, 1, temp, sizeof(temp));
            sscanf(temp, sscanf_format, tempAmounts);

            cache_get_value_index(0, 2, temp, sizeof(temp));
            sscanf(temp, sscanf_format, tempExtraData);
            
            for(new i = 0; i < MAX_ITEMS_PER_PLAYER; ++i)
            {
                if(tempItems[i] == 0 || tempAmounts[i] <= 0)
                    continue;
                PlayerInventory[playerid][i][pInvItem] = tempItems[i];
                PlayerInventory[playerid][i][pInvAmount] = tempAmounts[i];
                PlayerInventory[playerid][i][pInvExtra] = tempExtraData[i];
                Iter_Add(PlayerItemsSlot[playerid], i);
            }
            cache_get_value_index_int(0, 3, pInventoryBag[playerid]);
        }
    }
    new query[256];
    mysql_format(gMySQL, query, sizeof(query), "SELECT Items, ItemsAmount, ItemsExtraData, EquippedBag FROM `character_inventory` WHERE CharacterID = '%d'", PlayerInfo[playerid][pID]);
    mysql_tquery_inline(gMySQL, query, using inline OnLoad);
    return 1;
}

CMD:resetinv(playerid, params[])
{
    Character_ResetInventory(playerid);
    return 1;
}