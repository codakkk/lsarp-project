#define Inventory List@Inventory

#define for_inventory(%0:%1) for_list(%0:%1)

// Overload operators for the new container "Inventory:" (must be on top of everything)

stock List:operator=(Inventory:l) return List:l;

stock Inventory:Inventory_New(space)
{
    new Inventory:inventory = Inventory:list_new();
    list_resize_var(inventory, space, VAR_NULL);
    return inventory;
}

stock bool:Inventory_IncreaseSize(Inventory:inventory, new_space)
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

static stock bool:Inventory_InternalInsertItem(Inventory:inventory, slotid, itemid, amount, extra)
{
    if(!list_valid(inventory) || amount <= 0 || !ServerItem_IsValid(itemid) || slotid >= list_size(inventory))
        return false;
    //if(ServerItem_IsUnique(itemid) && amount > 1)
        //amount = 1;
    if(ServerItem_GetMaxStack(itemid) > amount)
        amount = ServerItem_GetMaxStack(itemid);
    
    new item[E_INVENTORY_DATA];
    item[gInvItem] = itemid;
    item[gInvAmount] = amount;
    item[gInvExtra] = extra;
    list_set_arr(inventory, slotid, item);
    return true;
}

stock Inventory_AddItem(Inventory:inventory, itemid, amount, extra)
{
    if(!list_valid(inventory))
        return 0;
    if(!ServerItem_IsValid(itemid))
        return INVENTORY_FAILED_INVALID_ITEM;
    if(amount < 0)
        return INVENTORY_FAILED_INVALID_AMOUNT;
    if(!Inventory_HasSpaceForItem(inventory, itemid, amount))
        return INVENTORY_NO_SPACE;
    new 
        item[E_INVENTORY_DATA],
        maxStack = ServerItem_GetMaxStack(itemid)
    ;
    if(ServerItem_IsUnique(itemid))
    {
        new tempAmount = amount;
        while(tempAmount > 0)
        {
            Inventory_InternalInsertItem(inventory, Inventory_GetFreeSlot(inventory), itemid, amount, extra);
            tempAmount--;
        }
    }
    else
    {
        new 
            hasItem_slot = Inventory_HasItem(inventory, itemid);
        if(hasItem_slot != -1 && Inventory_GetItemAmount(inventory, hasItem_slot) < maxStack)
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
                
                Inventory_InternalInsertItem(inventory, Inventory_GetFreeSlot(inventory), itemid, amountToAdd, extra);
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
                Inventory_InternalInsertItem(inventory, Inventory_GetFreeSlot(inventory), itemid, amountToAdd, extra);
                tempAmount -= amountToAdd;
            }
        }
    }
    return INVENTORY_ADD_SUCCESS;
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
        item[E_INVENTORY_DATA]
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

stock Inventory_HasItem(Inventory:inventory, itemid)
{
    new item[E_INVENTORY_DATA];
    for(new i = 0; i < list_size(inventory); ++i)
    {
        list_get_arr_safe(inventory, i, item);
        if(item[gInvItem] == itemid)
            return i;
    }
    return -1;
}

stock Inventory_GetItemAmount(Inventory:inventory, slotid)
{
    new item[E_INVENTORY_DATA];
    if(list_get_arr_safe(inventory, slotid, item))
        return item[gInvAmount];
    return 0;
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

stock Inventory_GetFreeSlot(Inventory:inventory)
{
    new index = 0;
    for_inventory(i : inventory)
    {
        if(iter_sizeof(i) == 0)
            break;
        index++;
    }
    return index;
}

stock Inventory_Print(Inventory:inventory)
{
    if(!list_valid(inventory))
        return printf("Invalid inventory!");
    new item[E_INVENTORY_DATA];
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
