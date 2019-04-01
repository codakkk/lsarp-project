#include <YSI\y_hooks>

// Remember that we're using set instead of add for inventory size

// Overload operators for the new container "Inventory:" (must be on top of everything)

stock List:operator=(Inventory:l) return List:l;

stock Inventory:Inventory_New(space)
{
    new Inventory:inventory = Inventory:list_new();
    list_resize_var(inventory, space, VAR_NULL);
    return inventory;
}

stock bool:Inventory_Resize(Inventory:inventory, new_space)
{
    if(!list_valid(inventory))
        return false;
    list_resize_var(inventory, new_space, VAR_NULL);
    return true;
}

stock Inventory_Delete(Inventory:inventory)
{
    list_delete(inventory);
}

stock bool:Inventory_SetItem(Inventory:inventory, slotid, itemid, amount, extra)
{
    if(!list_valid(inventory) || (itemid != 0 && amount <= 0) || !ServerItem_IsValid(itemid) || slotid >= list_size(inventory))
        return false;
    
    if(ServerItem_GetMaxStack(itemid) > amount)
        amount = ServerItem_GetMaxStack(itemid);
    
    if(ServerItem_IsUnique(itemid)) // probably Useless
        amount = 1;

    if(itemid != 0)
    {
        new item[E_ITEM_DATA];
        item[gInvItem] = itemid;
        item[gInvAmount] = amount;
        item[gInvExtra] = extra;
        list_set_arr(inventory, slotid, item);
    }
    else
    {
        list_set_var(inventory, slotid, VAR_NULL);
    }
    return true;
}

stock Inventory_AddItem(Inventory:inventory, itemid, amount, extra)
{
    if(!list_valid(inventory))
    {
        printf("Inventory_AddItem/list_valid = false");
        return 0;
    }
    if(!ServerItem_IsValid(itemid))
        return INVENTORY_FAILED_INVALID_ITEM;
    if(amount < 0)
        return INVENTORY_FAILED_INVALID_AMOUNT;
    if(!Inventory_HasSpaceForItem(inventory, itemid, amount))
        return INVENTORY_NO_SPACE;
    new 
        item[E_ITEM_DATA],
        maxStack = ServerItem_GetMaxStack(itemid)
    ;
    if(ServerItem_IsUnique(itemid))
    {
        new tempAmount = amount;
        while(tempAmount > 0)
        {
            Inventory_SetItem(inventory, Inventory_GetFreeSlot(inventory), itemid, amount, extra);
            tempAmount--;
        }
    }
    else
    {
        new 
            hasItem_slot = Inventory_HasItem(inventory, itemid);
        if(hasItem_slot != -1 && Inventory_GetSlotAmount(inventory, hasItem_slot) < maxStack)
        {
            list_get_arr_safe(inventory, hasItem_slot, item);
            item[gInvAmount] += amount;

            new diff = item[gInvAmount] - maxStack; // Calculate difference

            if(item[gInvAmount] > maxStack)
            {
                item[gInvAmount] = maxStack;
                list_set_arr(inventory, hasItem_slot, item);
            }

            while(diff > 0) // While diff is greater than 0, add it to inventory
            {
                new amountToAdd = diff;
                
                if(diff > maxStack)
                    amountToAdd = maxStack;
                
                Inventory_SetItem(inventory, Inventory_GetFreeSlot(inventory), itemid, amountToAdd, extra);
                diff -= amountToAdd;
            }
        }
        else // If no items with same id and enough amount, add item in new slot
        {
            new 
                tempAmount = amount,
                amountToAdd = tempAmount;
            while(tempAmount > 0)
            {
                amountToAdd = tempAmount;
                if(amountToAdd > maxStack)
                    amountToAdd = maxStack;
                Inventory_SetItem(inventory, Inventory_GetFreeSlot(inventory), itemid, amountToAdd, extra);
                tempAmount -= amountToAdd;
            }
        }
    }
    //Inventory_Print(inventory);
    return INVENTORY_ADD_SUCCESS;
}

stock bool:Inventory_DecreaseSlotAmount(Inventory:inventory, slotid, amount = 1)
{
    if(slotid < 0 || slotid >= Inventory_GetSpace(inventory))
        return false;
    new item[E_ITEM_DATA];
    if(list_get_arr_safe(inventory, slotid, item))
    {
        item[gInvAmount] -= amount;
        if(item[gInvAmount] <= 0)
            list_set_var(inventory, slotid, VAR_NULL);
        else
            list_set_arr(inventory, slotid, item);
    }
    return true;
}

stock Inventory_DecreaseItemAmount(Inventory:inventory, itemid, amount = 1)
{
    if(itemid == 0 || amount <= 0)
        return INVENTORY_DECREASE_SUCCESS;
    new 
        item[E_ITEM_DATA],
        tempDecreaseAmount = amount;
    for_inventory(i : inventory)
    {
        if(iter_sizeof(i) == 0) 
            continue;
        
        iter_get_arr(i, item);
        
        if(itemid != item[gInvItem]) 
            continue; // Safeness
        
        new diff = tempDecreaseAmount - item[gInvAmount];
        if(diff >= 0)
        {
            //iter_set_value_arr(i, VAR_NULL);
            iter_set_var(i, VAR_NULL); // must be used for variadics
            tempDecreaseAmount = diff;

            printf("Size: %d", iter_sizeof(i));
        }
        else 
        {
            item[gInvAmount] -= tempDecreaseAmount;
            iter_set_arr(i, item);
            tempDecreaseAmount = 0;
        }
    }
    return INVENTORY_DECREASE_SUCCESS;
}

stock Inventory_HasSpaceForItem(Inventory:inventory, itemid, amount)
{
    new 
        inv_size = Inventory_GetSpace(inventory);
    if(inv_size == 0)
        return 1;
    new
        usedSpace = Inventory_GetUsedSpace(inventory),
        tempAmount = amount,
        tempCurrentQuantity = 0,
        item[E_ITEM_DATA]
        ;
            
    if(!ServerItem_IsUnique(itemid))
    {
        for_list(i : inventory)
        {
            iter_get_arr_safe(i, item);
            tempCurrentQuantity = item[gInvAmount];
            while(item[gInvItem] == itemid && tempCurrentQuantity < ServerItem_GetMaxStack(itemid))
            {
                tempCurrentQuantity++;
                tempAmount--;
            }
            if(tempAmount <= 0)
                break;
        }
    }
    new currentFreeSlotCount = (inv_size - usedSpace);
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

// useful function, because weapons are stored in a specific way
stock Inventory_HasSpaceForWeapon(Inventory:inventory, weaponid, ammo)
{
    return Inventory_HasSpaceForItem(inventory, weaponid, 1) && (ammo >= 0 && Inventory_HasSpaceForItem(inventory, Weapon_GetAmmoType(weaponid), ammo));
}

stock Inventory_HasItem(Inventory:inventory, itemid, min = 1)
{
    new item[E_ITEM_DATA];
    for(new i = 0; i < list_size(inventory); ++i)
    {
        list_get_arr_safe(inventory, i, item);
        if(item[gInvItem] == itemid && item[gInvAmount] >= min)
            return i;
    }
    return -1;
}

stock bool:Inventory_IsSlotUsed(Inventory:inventory, slotid)
{
    return list_valid(inventory) && (list_sizeof(inventory, slotid) != 0 || Inventory_GetSlotItem(inventory, slotid) != 0 );
}

stock Inventory_GetItemData(Inventory:inventory, slotid, item[E_ITEM_DATA])
{
    list_get_arr_safe(inventory, slotid, item);
}

stock Inventory_GetSlotItem(Inventory:inventory, slotid)
{
    new item[E_ITEM_DATA];
    Inventory_GetItemData(inventory, slotid, item);
    return item[gInvItem];
}

stock Inventory_GetSlotAmount(Inventory:inventory, slotid)
{
    new item[E_ITEM_DATA];
    Inventory_GetItemData(inventory, slotid, item);
    return item[gInvAmount];
}

stock Inventory_GetSlotExtra(Inventory:inventory, slotid)
{
    new item[E_ITEM_DATA];
    Inventory_GetItemData(inventory, slotid, item);
    return item[gInvExtra];
}

stock Inventory_GetItemAmount(Inventory:inventory, itemid)
{
    new item[E_ITEM_DATA], count = 0;
    for_inventory(i : inventory)
    {
        if(iter_sizeof(i) == 0 || !iter_get_arr_safe(i, item) || item[gInvItem] != itemid)
            continue;
        count += item[gInvAmount];
    }
    return count;
}

stock Inventory_GetSpace(Inventory:inventory)
{
    if(!list_valid(inventory))
        return 0;
    return list_size(inventory);
}

stock Inventory_GetUsedSpace(Inventory:inventory)
{
    new space = 0;
    for_inventory(i : inventory)
    {
        if(iter_sizeof(i) != 0)
            space++;
    }
    return space;
}

stock bool:Inventory_IsEmpty(Inventory:inventory)
{
    for_inventory(i : inventory)
    {
        if(iter_sizeof(i) != 0)
            return false;
    }
    return true;
}

stock Inventory_GetFreeSlot(Inventory:inventory)
{
    new index = 0, tempItem[E_ITEM_DATA];
    for_inventory(i : inventory)
    {
        iter_get_arr_safe(i, tempItem);
        if(iter_sizeof(i) == 0 || tempItem[gInvItem] == 0 || tempItem[gInvAmount] == 0)
            break;
        index++;
    }
    return index;
}

/*stock bool:Inventory_ParseForDialog(Inventory:inventory, string[2048])
{
    if(!list_valid(inventory))
        return false;
    format(string, sizeof(string), ""); // Clears string
    new tempItem[E_ITEM_DATA], temp[16];
    for_inventory(i : inventory)
    {
        iter_get_arr(i, tempItem);
        if(iter_sizeof(i) == 0 || tempItem[gInvItem] == 0) // If no item
        {
            tempItem[gInvItem] = tempItem[gInvAmount] = tempItem[gInvExtra] = 0;
            format(string, sizeof(string), "%s{808080}Slot Libera\t{808080}--\t{808080}--\n", string);
        }
        else
        {
            new itemid = tempItem[gInvItem],
                itemAmount = tempItem[gInvAmount];
            if(ServerItem_IsUnique(itemid))
                temp = "--";
            else
                format(temp, sizeof(temp), "%d", itemAmount);
            format(string, sizeof(string), "%s{FFFFFF}%s\t{FFFFFF}%s\t{FFFFFF}%s\n", string, ServerItem[itemid][sitemName], temp, ServerItem_GetTypeName(itemid));
        }
    }
    return true;
}*/

stock String:Inventory_ParseForDialog(Inventory:inventory)
{
    if(!list_valid(inventory))
        return STRING_NULL;
    new String:string = @(""),
        tempItem[E_ITEM_DATA];
    for_inventory(i : inventory)
    {
        iter_get_arr(i, tempItem);
        if(iter_sizeof(i) == 0 || tempItem[gInvItem] == 0) // If no item
        {
            tempItem[gInvItem] = tempItem[gInvAmount] = tempItem[gInvExtra] = 0;
            string += @("{808080}Slot Libera\t{808080}--\t{808080}--\n");
        }
        else
        {
            new itemid = tempItem[gInvItem],
                itemAmount = tempItem[gInvAmount];
            string += str_format("{FFFFFF}%s\t{FFFFFF}%d\t{FFFFFF}%s\n", ServerItem_GetName(itemid), itemAmount, ServerItem_GetTypeName(itemid));
        }
    }
    return string;
}

stock bool:Inventory_ParseForSave(Inventory:inventory, tempItems[128], tempAmounts[128], tempExtras[128])
{
    if(!list_valid(inventory))
        return false;
    new
        tempItem[E_ITEM_DATA]
        ;
    format(tempItems, sizeof(tempItems), "");
    format(tempAmounts, sizeof(tempAmounts), "");
    format(tempExtras, sizeof(tempExtras), "");
    for_inventory(i : inventory)
    {
        // First we get the arr
        iter_get_arr(i, tempItem);
        // We check if VAR_NULL or if gInvItem is == 0
        if(iter_sizeof(i) == 0 || tempItem[gInvItem] == 0 || tempItem[gInvAmount] == 0)
        {
            tempItem[gInvItem] = tempItem[gInvAmount] = tempItem[gInvExtra] = 0;
        }
        format(tempItems, sizeof(tempItems), "%s%d%c", tempItems, tempItem[gInvItem], '|');
        format(tempAmounts, sizeof(tempAmounts), "%s%d%c", tempAmounts, tempItem[gInvAmount], '|');
        format(tempExtras, sizeof(tempExtras), "%s%d%c", tempExtras, tempItem[gInvExtra], '|');
    }
    return true;
}

stock Inventory_Print(Inventory:inventory)
{
    if(!list_valid(inventory))
        return printf("Invalid inventory!");
    new item[E_ITEM_DATA];
    for_inventory(i : inventory)
    {
        if(iter_sizeof(i) == 0)
            continue;
        iter_get_arr(i, item);
        printf("Item Name: %s - Amount: %d", ServerItem[item[gInvItem]][sitemName], item[gInvAmount]);
    }
    printf("Space: %d - Space Used: %d", Inventory_GetSpace(inventory), Inventory_GetUsedSpace(inventory));
    printf("Total free space: %d", Inventory_GetSpace(inventory) - Inventory_GetUsedSpace(inventory));
    return 1;
}
