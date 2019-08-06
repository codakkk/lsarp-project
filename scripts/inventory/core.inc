#include <inventory\types\ammo>
#include <inventory\types\bag>
#include <inventory\types\fuel>
#include <inventory\types\medic>
#include <inventory\types\money>
#include <inventory\types\weapon>
#include <inventory\callbacks>
#include <YSI_Coding\y_hooks>

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
    if(!list_valid(inventory) || (itemid != 0 && amount <= 0) || !ServerItem_IsValid(itemid) || slotid < 0 || slotid >= list_size(inventory))
	   return false;
    
    if(amount > ServerItem_GetMaxStack(itemid))
	   amount = ServerItem_GetMaxStack(itemid);
    
    if(ServerItem_IsUnique(itemid)) // probably Useless
	   amount = 1;

    if(itemid == 0)
    {
	   list_set_var(inventory, slotid, VAR_NULL);
    }
    else
    {
	   new item[E_ITEM_DATA];
	   item[gInvItem] = itemid;
	   item[gInvAmount] = amount;
	   item[gInvExtra] = extra;
	   list_set_arr(inventory, slotid, item);
    }
    return true;
}

stock Inventory_Reset(Inventory:inventory)
{
	for(new i = 0, j = Inventory_GetSpace(inventory); i < j; ++i)
	{
		Inventory_SetItem(inventory, i, 0, 0, 0);
	}
}

stock Inventory_AddItem(Inventory:inventory, itemid, amount, extra)
{
	if(!list_valid(inventory))
	{
		return 0;
	}
	if(!ServerItem_IsValid(itemid))
		return INVENTORY_FAILED_INVALID_ITEM;
    if(amount < 0)
		return INVENTORY_FAILED_INVALID_AMOUNT;
    if(!Inventory_HasSpaceForItem(inventory, itemid, amount))
		return INVENTORY_NO_SPACE;
    new 
		maxStack = ServerItem_GetMaxStack(itemid),
		tempAmount = amount
	;
	if(ServerItem_IsUnique(itemid))
	{
		while(tempAmount > 0)
		{
			Inventory_SetItem(inventory, Inventory_GetFreeSlot(inventory), itemid, amount, extra);
			tempAmount--;
		}
	}
	else
	{
		new List:hasItemSlots = Inventory_HasItemBySlots(inventory, itemid);
		tempAmount = amount;
		for_list(i : hasItemSlots)
		{
			if(tempAmount <= 0)
				break;
			new 
				slotid = iter_get(i), 
				itemAmount = Inventory_GetSlotAmount(inventory, slotid);
			
			if(itemAmount >= maxStack)
				continue;

			new preAdd = itemAmount;

			itemAmount += amount;

			if(itemAmount > maxStack)
				itemAmount = maxStack;

			tempAmount -= (itemAmount - preAdd);

			list_set_cell(inventory, slotid, gInvAmount, itemAmount);
			list_set_cell(inventory, slotid, gInvExtra, extra);
		}
		if(tempAmount > 0)// If no items with same id and enough amount, add item in new slot
		{
			new
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
		list_delete_deep(hasItemSlots);
	}
	//Inventory_Print(inventory);
	return INVENTORY_ADD_SUCCESS;
}

stock bool:Inventory_DecreaseSlotAmount(Inventory:inventory, slotid, amount = 1)
{
    if(slotid < 0 || slotid >= Inventory_GetSpace(inventory) || list_sizeof(inventory, slotid) == 0)
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
		return 0;
	if(itemid == 0 || amount <= 0)
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
			while(item[gInvItem] == itemid && tempCurrentQuantity < ServerItem_GetMaxStack(itemid) && tempAmount > 0) // tempAmount > 0 added after money bug.
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

stock Inventory_HasSpaceForItems(Inventory:inventory, items[10], amounts[10])
{
	new 
		inv_size = Inventory_GetSpace(inventory);
	if(inv_size == 0)
		return 0;
	new
		usedSpace = Inventory_GetUsedSpace(inventory),
		tempAmount = 0,
		tempCurrentQuantity = 0,
		item[E_ITEM_DATA]
	;
	
	for(new x = 0, j = 10; x < j; ++x)
	{
		new itemid = items[x];
		if(itemid == 0 || amounts[x] <= 0)
			continue;
		tempAmount = amounts[x];
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
				{
					break;
				}
			}
		}
		new currentFreeSlotCount = (inv_size - usedSpace);
		if(tempAmount > 0 && currentFreeSlotCount == 0)
		{
			return 0;
		}
		else
		{
			new 
				occupiedSlots = 0;
			while(tempAmount > 0 && occupiedSlots < currentFreeSlotCount)
			{
				tempAmount -= ServerItem[itemid][sitemMaxStack];
				occupiedSlots++;
			}
			usedSpace += occupiedSlots;
		}
		if(tempAmount > 0)
			return 0;
	}
	return 1; //tempAmount <= 0;
}

stock Inventory_HasItem(Inventory:inventory, itemid, min = 1)
{
	new item[E_ITEM_DATA];
	for(new i = 0, j = list_size(inventory); i < j; ++i)
	{
		Inventory_GetItemData(inventory, i, item);
		if(item[gInvItem] == itemid && item[gInvAmount] >= min)
			return 1;
	}
	return 0;
}

// Returns a List with all slots that contains this itemid.
stock List:Inventory_HasItemBySlots(Inventory:inventory, itemid)
{
	new List:list = list_new(), item[E_ITEM_DATA];
	for(new i = 0, j = list_size(inventory); i < j; ++i)
	{
		if(list_sizeof(inventory, i) == 0)
			continue;
		Inventory_GetItemData(inventory, i, item);
		if(item[gInvItem] == itemid && item[gInvAmount] > 0)
			list_add(list, i);
	}
	return list;
}

stock bool:Inventory_IsSlotUsed(Inventory:inventory, slotid)
{
    return list_valid(inventory) && (list_sizeof(inventory, slotid) != 0 || Inventory_GetSlotItem(inventory, slotid) != 0 );
}

// Checks if slotid is in the inventory boundaries (0 <= slotid <= Inventory_GetSpace(inventory))
stock Inventory_IsValidSlot(Inventory:inventory, slotid)
{
	return 0 <= slotid < Inventory_GetSpace(inventory);
}

stock Inventory_GetItemData(Inventory:inventory, slotid, item[E_ITEM_DATA])
{
    list_get_arr_safe(inventory, slotid, item);
}

stock Inventory_GetSlotItem(Inventory:inventory, slotid)
{
	if(!Inventory_IsValidSlot(inventory, slotid) || list_sizeof(inventory, slotid) == 0)
		return 0;
    return list_get(inventory, slotid, gInvItem);
}

stock Inventory_GetSlotAmount(Inventory:inventory, slotid)
{
	if(!Inventory_IsValidSlot(inventory, slotid) || list_sizeof(inventory, slotid) == 0)
		return 0;
    return list_get(inventory, slotid, gInvAmount);
}

stock Inventory_GetSlotExtra(Inventory:inventory, slotid)
{
    if(!Inventory_IsValidSlot(inventory, slotid) || list_sizeof(inventory, slotid) == 0)
		return 0;
    return list_get(inventory, slotid, gInvExtra);
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
			return index;
		index++;
	}
    return -1;
}

stock Inventory_ShowInChatStr(Inventory:inventory, playerid, String:title)
{
	if(!list_valid(inventory))
		return 0;
	//if(title != STRING_NULL || str_len(title) > 0)
		//SendClientMessageStr(playerid, COLOR_GREEN, str_format("_______________[%S (%d/%d)]_______________", title, Inventory_GetUsedSpace(inventory), Inventory_GetSpace(inventory)));
	//else
	new String:str = str_format("_______________[%S]_______________", title);

	SendClientMessageStr(playerid, COLOR_GREEN, str);
	
	new tempItem[E_ITEM_DATA], slotid = -1;
	for_inventory(i : inventory)
	{
		new String:s;
		slotid++;
		iter_get_arr(i, tempItem);
		if(iter_sizeof(i) == 0 || tempItem[gInvItem] == 0) // If no item
		{
			s = str_format("Slot {0080FF}%d{FFFFFF} - Slot Libero", slotid);
		}
		else
		{
			new itemid = tempItem[gInvItem],
				itemAmount = tempItem[gInvAmount],
				extra = tempItem[gInvExtra];
			
			s = str_format("Slot {0080FF}%d{FFFFFF} - {FFFFFF}%s{FFFFFF} ({FFFFFF}%d{FFFFFF})", slotid, ServerItem_GetName(itemid), itemAmount);
			
			if(ServerItem_GetType(itemid) == ITEM_TYPE:ITEM_TYPE_WEAPON)
			{
				s += @(" - ");
				if(extra <= 0)
					s += @("Vuota");
				else
					s += str_val(extra);
			}
		}
		SendClientMessageStr(playerid, -1, s);
	}
	// just a way to conform the underlines. It's useless
	new String:s = @("________________________________");
	for(new i = 0, j = str_len(title); i < j; ++i)
	{
		s += @("_");
	}
	SendClientMessageStr(playerid, COLOR_GREEN, s);
	return 1;
}

stock Inventory_ShowInChat(Inventory:inventory, playerid, const title[] = "")
{
	if(strlen(title) > 0)
		return Inventory_ShowInChatStr(inventory, playerid, str_new(title));
	return Inventory_ShowInChatStr(inventory, playerid, STRING_NULL);
}

#define Inventory_Show(%0,%1,%2,%3, Inventory_InternalShow(%0, %1, %2, #%3,
#define Inventory_ShowStr(%0,%1,%2,%3, Inventory_InternalShowStr(%0, %1, %2, #%3,

stock Inventory_InternalShowStr(Inventory:inventory, playerid, String:title, const dialog[] = "", const button1[] = "", const button2[] = "")
{
	new ptr[1][] = {{}}, size = str_len(title) + 1, Var:var = amx_alloc(size);
    amx_to_ref(var, ptr);
    str_get(title, ptr[0], .size=size);

    new result = Inventory_InternalShow(inventory, playerid, ptr[0], dialog, button1, button2);

    amx_free(var);
    amx_delete(var);
	return result;
}

stock Inventory_InternalShow(Inventory:inventory, playerid, const title[], const dialog[] = "", const button1[] = "", const button2[] = "")
{
	if(!list_valid(inventory))
		return 0;
	if(Account_HasInvModeEnabled(playerid))
	{
		return Inventory_ShowInChat(inventory, playerid, title);
		//SendClientMessage(playerid, COLOR_GREEN, "Should show text inventory");
	}
	else
	{
		new String:string = @("Nome\tQuantità\tTipo\tAltro\n"),
			tempItem[E_ITEM_DATA], slotid = 0;
		for_inventory(i : inventory)
		{
			iter_get_arr(i, tempItem);
			if(iter_sizeof(i) == 0 || tempItem[gInvItem] == 0) // If no item
			{
				tempItem[gInvItem] = tempItem[gInvAmount] = tempItem[gInvExtra] = 0;
				string += @("{808080}Slot Libero\t{808080}--\t{808080}--\t--\n");
			}
			else
			{
				new itemid = tempItem[gInvItem],
					itemAmount = tempItem[gInvAmount],
					extra = tempItem[gInvExtra];
				
				new String:s = str_format("{FFFFFF}%s\t{FFFFFF}%d\t{FFFFFF}%s\t", ServerItem_GetName(itemid), itemAmount, ServerItem_GetTypeName(itemid));
				if(ServerItem_GetType(itemid) == ITEM_TYPE:ITEM_TYPE_WEAPON && (Weapon_RequireAmmo(itemid) || Weapon_CanBeDisassembled(itemid)))
				{
					if(extra == 0)
						s += @("Vuota");
					else
						s += str_val(extra);
				}
				else
					s += @("--");
				s += @("\n");
				string += s;
			}
			slotid++;
		}
		Dialog_Open_s(playerid, dialog, DIALOG_STYLE_TABLIST_HEADERS, str_new(title), string, button1, button2); 
	}
	return 1;
}

stock Inventory_Print(Inventory:inventory)
{
    if(!list_valid(inventory))
	   return printf("Invalid inventory.");
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

stock Inventory_LoadFromDatabase(Inventory:inventory, const databaseName[], const entityKeyName[], id)
{
	if(!list_valid(inventory))
		return false;
	inline OnLoad()
	{
		new rows = cache_num_rows(), 
			slotid, itemid, amount, extra;
		for(new i = 0; i < rows; ++i)
		{
			cache_get_value_index_int(i, 0, slotid);
			cache_get_value_index_int(i, 1, itemid);
			cache_get_value_index_int(i, 2, amount);
			cache_get_value_index_int(i, 3, extra);
			if(itemid == 0 || amount == 0)
				continue;
			Inventory_SetItem(inventory, slotid, itemid, amount, extra);
		}
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT SlotID, Item, Amount, Extra FROM `%e` WHERE %e = '%d' ORDER BY SlotID", databaseName, entityKeyName, id);
	return true;
}

stock Inventory_SaveInDatabase(Inventory:inventory, const databaseName[], const entityKeyName[], entityId)
{
	new
		itemid, amount, extra;
	
	mysql_tquery(gMySQL, "START TRANSACTION;");

	for(new i = 0, j = Inventory_GetSpace(inventory); i < j; ++i)
	{
		itemid = Inventory_GetSlotItem(inventory, i);
		amount = Inventory_GetSlotAmount(inventory, i);
		extra = Inventory_GetSlotExtra(inventory, i);
		if(itemid == 0 || amount == 0)
		{
			mysql_tquery_f(gMySQL, "DELETE FROM `%e` WHERE %e = '%d' AND SlotID = '%d';", databaseName, entityKeyName, entityId, i);
		}
		else
		{
			mysql_tquery_f(gMySQL, 
			"INSERT INTO `%e` (%e, SlotID, Item, Amount, Extra) VALUES('%d', '%d', '%d', '%d', '%d') \
			ON DUPLICATE KEY UPDATE \
			Item = VALUES(Item), \
			Amount = VALUES(Amount), \
			Extra = VALUES(Extra);",
			databaseName, entityKeyName,
			entityId, i, itemid, amount, extra);
		}
	}
	
	mysql_tquery(gMySQL, "COMMIT;");
	return 1;
}