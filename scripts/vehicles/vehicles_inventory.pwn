#include <YSI\y_hooks>

stock Vehicle_AddItem(vehicleid, itemid, amount = 1, extra = 0)
{
    if(!ServerItem_IsValid(itemid))
        return INVENTORY_FAILED_INVALID_ITEM;
    if(!map_has_key(VehicleInventory, vehicleid))
    {
        printf("Error occurred: no inventory for %d vehicle id.", vehicleid);
        return INVENTORY_NO_SPACE;
    }
    
    new 
        item[E_INVENTORY_DATA];
    item[gInvItem] = itemid;
    item[gInvAmount] = amount;
    item[gInvExtra] = extra;

    new List:vehicleItems;

    map_get_safe(VehicleInventory, vehicleid, vehicleItems);
    //printf("Vehicle has %d items", list_size(vehicleItems));

    //printf("Adding item: %s - %d", ServerItem[itemid][sitemName], amount);
    list_add_arr(vehicleItems, item);
    //printf("Vehicle has %d items", list_size(vehicleItems));
    /*if(!ServerItem_IsValid(item_id))
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
            PlayerInventory[playerid][freeid][gInvItem] = item_id;
            PlayerInventory[playerid][freeid][gInvAmount] = 1;
            PlayerInventory[playerid][freeid][gInvExtra] = 0;
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
            PlayerInventory[playerid][hasItem_slot][gInvAmount] += amount;
            if(callback)
                CallLocalFunction("OnPlayerInventoryChanged", "ii", playerid, hasItem_slot);
            new diff = PlayerInventory[playerid][hasItem_slot][gInvAmount] - ServerItem_GetMaxStack(item_id);
            if(diff > 0) // if exceed maximum stack size, try to add them in another slot.
            {
                PlayerInventory[playerid][hasItem_slot][gInvAmount] = ServerItem[item_id][sitemMaxStack]; // Cap
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
            PlayerInventory[playerid][freeid][gInvItem] = item_id;
            PlayerInventory[playerid][freeid][gInvAmount] = amount;
            PlayerInventory[playerid][freeid][gInvExtra] = 0;
            Iter_Add(PlayerItemsSlot[playerid], freeid);
            if(callback)
            {
                CallLocalFunction("OnPlayerInventoryItemAdd", "iii", playerid, freeid, amount);
                CallLocalFunction("OnPlayerInventoryChanged", "ii", playerid, freeid);
            }
            if(PlayerInventory[playerid][freeid][gInvAmount] > ServerItem[item_id][sitemMaxStack])
            {
                new diff = PlayerInventory[playerid][freeid][gInvAmount] - ServerItem[item_id][sitemMaxStack];
                PlayerInventory[playerid][freeid][gInvAmount] = ServerItem[item_id][sitemMaxStack];
                return Character_GiveItem(playerid, item_id, diff);
            }
        }
    }
    Character_SaveInventory(playerid);*/
    return INVENTORY_ADD_SUCCESS;
}

stock Vehicle_HasSpaceForItem(vehicleid, itemid, amount)
{
    new
        tempAmount = amount,
        tempCurrentQuantity = 0,
        item[E_INVENTORY_DATA]
        ;
            
    if(!ServerItem_IsUnique(itemid))
    {
        new List:vehicleItems;
        map_get_safe(VehicleInventory, vehicleid, vehicleItems);
        for_list(i : vehicleItems)
        {
            iter_get_arr_safe(i, item);
            tempCurrentQuantity = item[gInvAmount];
            while(item[gInvItem] == itemid && tempCurrentQuantity < ServerItem_GetMaxStack(itemid))
            {
                tempCurrentQuantity++;
                tempAmount--;
            }
            //printf("Qnt: %d", tempCurrentQuantity);
            if(tempAmount <= 0)
                break;
        }
    }
    new currentFreeSlotCount = Vehicle_GetFreeSlotCount(vehicleid);
    if(tempAmount > 0 && currentFreeSlotCount == 0)
        return 0;
    else
    {
        new 
            occupiedSlots = 0;
        while(tempAmount > 0 && occupiedSlots < currentFreeSlotCount)
        {
            tempAmount -= ServerItem[itemid][sitemMaxStack];
            occupiedSlots++;
        }
    }
    return tempAmount <= 0;
}

stock Vehicle_GetItem(vehicleid, slotid)
{
    new List:items;
    if(slotid < 0 || !map_get_safe(VehicleInventory, vehicleid, items) || slotid >= list_size(items))
        return 0;
    new item[E_INVENTORY_DATA];
    if(list_get_arr_safe(items, slotid, item))
    {
        return item[gInvItem];
    }
    return 0;
}

stock Vehicle_GetFreeSlotCount(vehicleid)
{
    new List:items;
    if(map_get_safe(VehicleInventory, vehicleid, items))
    {
        return Vehicle_GetInventorySize(vehicleid) - list_size(items);
    }
    return 0;
}

// Initialize and adds inventory for vehicleid.
stock Vehicle_InitializeInventory(vehicleid)
{
    new List:items = list_new();
    map_add(VehicleInventory, vehicleid, items);
}

stock Vehicle_DeallocateInventory(vehicleid)
{
    map_remove_deep(VehicleInventory, vehicleid);
    printf("Vehicle %d inventory deallocated.");
}

stock Vehicle_GetInventorySize(vehicleid)
{
    return 4;
}