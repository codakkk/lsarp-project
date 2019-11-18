// Remember that we're using set instead of add for inventory size

#if defined _included_INVENTORY_CORE
    #endinput
#endif 

#define _included_INVENTORY_CORE 

#include <inventory_system\server>
#include <entity\core>
#define for_inventory(%0:%1) for_list(%0:%1)

forward OnInventoryLoaded(Inventory:inventory);
forward OnInventoryChanged(Inventory:inventory);
//#define for_inventories(%0:%1) for(new EntityType:_____i = EntityType:0; _____i < EntityType; ++_____i) \
 //							   for_map(_____b:Inventory_GetMap(%0))
#define Inventory_Show(%0,%1,%2,%3, Inventory_InternalShow(%0, %1, %2, #%3,
#define Inventory_ShowStr(%0,%1,%2,%3, Inventory_InternalShowStr(%0, %1, %2, #%3,

#define MAX_SLOT_EXTRA			(5)


static enum _:E_INVENTORY_DATA
{
	EntityType:iEntityType,
	EntityID,
	List:Items
};

enum // Error Type
{
	//INVENTORY_RESULT_ENUM = 0, // NOT TRUE: -50 because we need positive numbers for diff.
	INVENTORY_INVALID,
	INVENTORY_ADD_SUCCESS,
	INVENTORY_NO_SPACE,
	INVENTORY_INVALID_SLOTID,
	INVENTORY_FAILED_INVALID_ITEM,
	INVENTORY_FAILED_INVALID_AMOUNT,
	INVENTORY_ADD_INVALID_ITEM,
	INVENTORY_DECREASE_SUCCESS,
	INVENTORY_DECREASE_FAILED,
};

static enum _:E_ITEM_DATA
{
	InvItem,
	InvAmount,
	InvExtra[MAX_SLOT_EXTRA], // I should use a Map:<string, data>
	Map:TempExtra
};

#include "types\types"
#include "callbacks"

#include <YSI_Coding\y_hooks>

static 
	List:InventoriesList,
	Map:InventoriesMap[EntityType];


stock Map:Inventory_GetMap(EntityType:entityType)
{
	return InventoriesMap[entityType];
}

//stock List:operator=(Inventory:l) return List:l;

hook OnGameModeInit()
{
	for(new EntityType:i = EntityType:0; i < EntityType; ++i)
	{
		InventoriesMap[i] = map_new();
	}
	InventoriesList = list_new();
}

stock Inventory:Inventory_New(EntityType:entityType, entityId, space)
{
	if(map_has_key(InventoriesMap[entityType], entityId))
	{
		printf("Trying to create another inventory for entity type %d id %d.", _:entityType, entityId);
		return Inventory:0;
	}
	
	new List:items = list_new();
    list_resize_var(items, space, VAR_NULL);

	new Inventory:inventory = Inventory:var_new_buf(E_INVENTORY_DATA);
	var_acquire(Variant:inventory);
	var_set_cell(Variant:inventory, iEntityType, _:entityType);
	var_set_cell(Variant:inventory, EntityID, entityId);
	var_set_cell(Variant:inventory, Items, items);

	map_add(InventoriesMap[entityType], entityId, inventory);
	list_add(InventoriesList, inventory);

	//printf("Inventory created for entity type %d id %d", _:entityType, entityId);
    return inventory;
}

stock bool:Inventory_IsValid(Inventory:inv)
{
	return var_valid(Variant:inv);
}

stock bool:Inventory_Delete(Inventory:inventory)
{
	if(!Inventory_IsValid(inventory))
		return false;
	new EntityType:entityType = Inventory_GetEntityType(inventory);
	new entityId = Inventory_GetEntityID(inventory);
	new List:items = Inventory_GetItems(inventory);

	printf("Deleting inventory %d for entity %d id %d", _:inventory, _:entityType, entityId);
	printf("Items List: %d", _:items);
	
	list_delete(items);

	map_remove(InventoriesMap[entityType], entityId);
	
	printf("InventoriesMap entity removed.");
	if(map_has_key(InventoriesMap[entityType], entityId))
		printf("But entity is still present.");

	new index = list_find(InventoriesList, inventory);
	if(index != -1)
		list_remove(InventoriesList, index);
	
	var_release(Variant:inventory);
	var_delete(Variant:inventory);
	return true;
}

stock Inventory:Inventory_Get(EntityType:entityType, entityId)
{
	if(!map_has_key(InventoriesMap[entityType], entityId))
		return Inventory:0;
	return Inventory:map_get(InventoriesMap[entityType], entityId);
}

stock EntityType:Inventory_GetEntityType(Inventory:inventory)
{
	return EntityType:var_get(Variant:inventory, iEntityType);
}

stock Inventory_GetEntityID(Inventory:inventory)
{
	return var_get(Variant:inventory, EntityID);
}

stock List:Inventory_GetItems(Inventory:inventory)
{
	return List:var_get(Variant:inventory, Items);
}

stock bool:Inventory_Resize(Inventory:inventory, new_space)
{
    list_resize_var(Inventory_GetItems(inventory), new_space, VAR_NULL);
    return true;
}

static bool:Inventory_SetItem(Inventory:inventory, slotid, itemid, amount, extra, Map:tempExtra = Map:0)
{
    if((itemid != 0 && amount <= 0) || !ServerItem_IsValid(itemid) || slotid < 0 || slotid >= list_size(Inventory_GetItems(inventory)))
	   return false;
    
    if(amount > ServerItem_GetMaxStack(itemid))
	   amount = ServerItem_GetMaxStack(itemid);
    
    if(ServerItem_IsUnique(itemid)) // probably Useless
	   amount = 1;

    if(itemid == 0)
    {
		if(map_valid(Inventory_GetSlotTempExtraMap(inventory, slotid)))
			map_delete(Inventory_GetSlotTempExtraMap(inventory, slotid));
		list_set_var(Inventory_GetItems(inventory), slotid, VAR_NULL);
    }
    else
    {
		new item[E_ITEM_DATA];
		item[InvItem] = itemid;
		item[InvAmount] = amount;
		item[InvExtra][0] = extra;
		if(map_valid(tempExtra))
			item[TempExtra] = tempExtra;
		else
	   		item[TempExtra] = map_new();
	   	list_set_arr(Inventory_GetItems(inventory), slotid, item);
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

stock Inventory_AddItem(Inventory:inventory, itemid, amount, extra, Map:tempExtra = Map:0)
{
	if(!ServerItem_IsValid(itemid))
		return INVENTORY_FAILED_INVALID_ITEM;
    if(amount < 0)
		return INVENTORY_FAILED_INVALID_AMOUNT;
    if(!Inventory_HasSpaceForItem(inventory, itemid, amount))
		return INVENTORY_NO_SPACE;
    new 
		maxStack = ServerItem_GetMaxStack(itemid),
		tempAmount = amount,
		List:items = Inventory_GetItems(inventory)
	;
	if(ServerItem_IsUnique(itemid))
	{
		while(tempAmount > 0)
		{
			Inventory_SetItem(inventory, Inventory_GetFreeSlot(inventory), itemid, amount, extra, tempExtra);
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

			list_set_cell(items, slotid, InvAmount, itemAmount);
			list_set_cell(items, slotid, InvExtra + 0, extra);
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
		hasItemSlots = INVALID_LIST;
	}
	CallLocalFunction(#OnInventoryChanged, "d", _:inventory);
	//Inventory_Print(inventory);
	return INVENTORY_ADD_SUCCESS;
}

stock bool:Inventory_DecreaseSlotAmount(Inventory:inventory, slotid, amount = 1)
{
    if(slotid < 0 || slotid >= Inventory_GetSpace(inventory) || list_sizeof(Inventory_GetItems(inventory), slotid) == 0)
	   return false;
	
    new item[E_ITEM_DATA];
    if(list_get_arr_safe(Inventory_GetItems(inventory), slotid, item))
    {
	   item[InvAmount] -= amount;
	   if(item[InvAmount] <= 0)
		  list_set_var(Inventory_GetItems(inventory), slotid, VAR_NULL);
	   else
		  list_set_arr(Inventory_GetItems(inventory), slotid, item);
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
    for_list(i : Inventory_GetItems(inventory))
    {
	   if(iter_sizeof(i) == 0) 
		  continue;
	   
	   iter_get_arr(i, item);
	   
	   if(itemid != item[InvItem]) 
		  continue; // Safeness
	   
	   new diff = tempDecreaseAmount - item[InvAmount];
	   if(diff >= 0)
	   {
		  //iter_set_value_arr(i, VAR_NULL);
		  iter_set_var(i, VAR_NULL); // must be used for variadics
		  tempDecreaseAmount = diff;
	   }
	   else 
	   {
		  item[InvAmount] -= tempDecreaseAmount;
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
		for_list(i : Inventory_GetItems(inventory))
		{
			iter_get_arr_safe(i, item);
			tempCurrentQuantity = item[InvAmount];
			while(item[InvItem] == itemid && tempCurrentQuantity < ServerItem_GetMaxStack(itemid) && tempAmount > 0) // tempAmount > 0 added after money bug.
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
			tempAmount -= ServerItem_GetMaxStack(itemid);// ServerItem[itemid][sitemMaxStack];
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
			for_list(i : Inventory_GetItems(inventory))
			{
				iter_get_arr_safe(i, item);
				tempCurrentQuantity = item[InvAmount];
				while(item[InvItem] == itemid && tempCurrentQuantity < ServerItem_GetMaxStack(itemid))
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
				tempAmount -= ServerItem_GetMaxStack(itemid);
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
	for(new i = 0, j = Inventory_GetSpace(inventory); i < j; ++i)
	{
		Inventory_GetItemData(inventory, i, item);
		if(item[InvItem] == itemid && item[InvAmount] >= min)
			return 1;
	}
	return 0;
}

// Returns a List with all slots that contains this itemid.
stock List:Inventory_HasItemBySlots(Inventory:inventory, itemid)
{
	new List:list = list_new(), item[E_ITEM_DATA];
	for(new i = 0, j = Inventory_GetSpace(inventory); i < j; ++i)
	{
		if(list_sizeof(Inventory_GetItems(inventory), i) == 0)
			continue;
		Inventory_GetItemData(inventory, i, item);
		if(item[InvItem] == itemid && item[InvAmount] > 0)
			list_add(list, i);
	}
	return list;
}

stock bool:Inventory_IsSlotUsed(Inventory:inventory, slotid)
{
    return (list_sizeof(Inventory_GetItems(inventory), slotid) != 0 || Inventory_GetSlotItem(inventory, slotid) != 0 );
}

// Checks if slotid is in the inventory boundaries (0 <= slotid <= Inventory_GetSpace(inventory))
stock Inventory_IsValidSlot(Inventory:inventory, slotid)
{
	return 0 <= slotid < Inventory_GetSpace(inventory);
}

stock Inventory_GetItemData(Inventory:inventory, slotid, item[E_ITEM_DATA])
{
    list_get_arr_safe(Inventory_GetItems(inventory), slotid, item);
}

stock Inventory_GetSlotItem(Inventory:inventory, slotid)
{
	if(!Inventory_IsValidSlot(inventory, slotid) || list_sizeof(Inventory_GetItems(inventory), slotid) == 0)
		return 0;
    return list_get(Inventory_GetItems(inventory), slotid, InvItem);
}

stock Inventory_GetSlotAmount(Inventory:inventory, slotid)
{
	if(!Inventory_IsValidSlot(inventory, slotid) || list_sizeof(Inventory_GetItems(inventory), slotid) == 0)
		return 0;
    return list_get(Inventory_GetItems(inventory), slotid, InvAmount);
}

stock Inventory_GetItemAmount(Inventory:inventory, itemid)
{
    new item[E_ITEM_DATA], count = 0;
    for_list(i : Inventory_GetItems(inventory))
    {
	   if(iter_sizeof(i) == 0 || !iter_get_arr_safe(i, item) || item[InvItem] != itemid)
		  continue;
	   count += item[InvAmount];
    }
    return count;
}

stock Inventory_GetSpace(Inventory:inventory)
{
	if(!Inventory_IsValid(inventory))
		return 0;
    return list_size(Inventory_GetItems(inventory));
}

stock Inventory_GetUsedSpace(Inventory:inventory)
{
    new space = 0;
    for_inventory(i : Inventory_GetItems(inventory))
    {
	   if(iter_sizeof(i) != 0)
		  space++;
    }
    return space;
}

stock bool:Inventory_IsEmpty(Inventory:inventory)
{
    for_inventory(i : Inventory_GetItems(inventory))
    {
	   if(iter_sizeof(i) != 0)
		  return false;
    }
    return true;
}

stock Inventory_GetFreeSlot(Inventory:inventory)
{
	new index = 0, tempItem[E_ITEM_DATA];
	for_inventory(i : Inventory_GetItems(inventory))
	{
		iter_get_arr_safe(i, tempItem);
		if(iter_sizeof(i) == 0 || tempItem[InvItem] == 0 || tempItem[InvAmount] == 0)
			return index;
		index++;
	}
    return -1;
}

static Inventory_ShowInChatStr(Inventory:inventory, playerid, String:title)
{
	new String:str = str_format("_______________[%S]_______________", title);

	SendClientMessageStr(playerid, 0xFF6347FF, str);
	
	new tempItem[E_ITEM_DATA], slotid = 0;
	for_inventory(i : Inventory_GetItems(inventory))
	{
		slotid++;
		new String:s = str_format("{FF6347}[{FFFFFF} %d. ", slotid);
		
		iter_get_arr(i, tempItem);
		if(iter_sizeof(i) == 0 || tempItem[InvItem] == 0) // If no item
		{
			s += @("Libero");
		}
		else
		{
			new itemid = tempItem[InvItem],
				itemAmount = tempItem[InvAmount],
				extra = tempItem[InvExtra][0];
			
			
			s += str_format("%s (%d)", ServerItem_GetName(itemid), itemAmount);
			
			if(ServerItem_GetType(itemid) == ITEM_TYPE:ITEM_TYPE_WEAPON)
			{
				s += @(" - ");
				if(extra <= 0)
					s += @("Vuota");
				else
					s += str_val(extra);
			}
			else if(itemid == gItem_Phone)
			{
				s += @(" - ");
				if(extra <= 0)
					s += @("Nessun Numero");
				else
					s += str_val(extra);
			}

		}
		s += @(" {FF6347}]{FFFFFF}");
		SendClientMessageStr(playerid, -1, s);
	}
	return 1;
}

static Inventory_ShowInChat(Inventory:inventory, playerid, const title[] = "")
{
	if(strlen(title) > 0)
		return Inventory_ShowInChatStr(inventory, playerid, str_new(title));
	return Inventory_ShowInChatStr(inventory, playerid, STRING_NULL);
}

static Inventory_ShowInDialog(Inventory:inventory, playerid, const title[], const dialog[] = "", const button1[] = "", const button2[] = "")
{
	new String:string = @("Nome\tQuantità\tTipo\tAltro\n"),
		tempItem[E_ITEM_DATA], slotid = 0;
	for_inventory(i : Inventory_GetItems(inventory))
	{
		iter_get_arr(i, tempItem);
		if(iter_sizeof(i) == 0 || tempItem[InvItem] == 0) // If no item
		{
			tempItem[InvItem] = tempItem[InvAmount] = tempItem[InvExtra] = 0;
			str_append(string, @("{808080}Slot Libero\t{808080}--\t{808080}--\t--\n"));
		}
		else
		{
			new itemid = tempItem[InvItem],
				itemAmount = tempItem[InvAmount],
				extra = tempItem[InvExtra][0];
	
			str_append_format(string, "{FFFFFF}%s\t{FFFFFF}%d\t{FFFFFF}%s\t%S\n", ServerItem_GetName(itemid), itemAmount, ServerItem_GetTypeName(itemid), GetItemExtraStr(itemid, itemAmount, extra));
		}
		slotid++;
	}
	Dialog_Open_s(playerid, dialog, DIALOG_STYLE_TABLIST_HEADERS, str_new(title), string, button1, button2);
	return 1;
}

stock String:GetItemExtraStr(itemid, quantity, extra)
{
	#pragma unused itemid, quantity, extra
	return @("--");
}

stock Inventory_InternalShowStr(Inventory:inventory, playerid, String:title, const dialog[] = "", const button1[] = "", const button2[] = "", bool:forceDialog = false)
{
	/*new ptr[1][] = {{}}, size = str_len(title) + 1, Var:var = amx_alloc(size);
    amx_to_ref(var, ptr);
    str_get(title, ptr[0], .size=size);*/
	static buffer[2048];
	str_get(title, buffer);
    new result = Inventory_InternalShow(inventory, playerid, /*ptr[0]*/buffer, dialog, button1, button2, forceDialog);

    /*amx_free(var);
    amx_delete(var);*/
	return result;
}

stock Inventory_InternalShow(Inventory:inventory, playerid, const title[], const dialog[] = "", const button1[] = "", const button2[] = "", bool:forceDialog = false)
{
	if(Player_HasInvModeEnabled(playerid) && !forceDialog)
	{
		Inventory_ShowInChat(inventory, playerid, title);
	}
	else
	{
		Inventory_ShowInDialog(inventory, playerid, title, dialog, button1, button2); 
	}
	return 1;
}

stock Inventory_LoadFromDatabase(Inventory:inventory, const databaseName[], const entityKeyName[], id)
{
	await mysql_aquery_s(gMySQL, str_format("SELECT slot_id, item, amount, extra FROM `%e` WHERE %e = '%d' ORDER BY slot_id", databaseName, entityKeyName, id));

	Inventory_Reset(inventory);
	new rows = cache_num_rows(), 
		slotid, itemid, amount, extra;
	for(new i = 0; i < rows; ++i)
	{
		cache_get_value_name_int(i, "slot_id", slotid);
		cache_get_value_name_int(i, "item", itemid);
		cache_get_value_name_int(i, "amount", amount);
		cache_get_value_name_int(i, "extra", extra);
		if(itemid == 0 || amount == 0)
			continue;
		Inventory_SetItem(inventory, slotid, itemid, amount, extra);
	}
	if(Inventory_IsValid(inventory))
		CallLocalFunction(#OnInventoryLoaded, "d", _:inventory);
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
			mysql_pquery_f(gMySQL, "DELETE FROM `%e` WHERE %e = '%d' AND slot_id = '%d';", databaseName, entityKeyName, entityId, i);
		}
		else
		{
			mysql_pquery_f(gMySQL, 
			"INSERT INTO `%e` (%e, slot_id, item, amount, extra) VALUES('%d', '%d', '%d', '%d', '%d') \
			ON DUPLICATE KEY UPDATE \
			item = VALUES(item), \
			amount = VALUES(amount), \
			extra = VALUES(extra);",
			databaseName, entityKeyName,
			entityId, i, itemid, amount, extra);
		}
	}
	
	mysql_tquery(gMySQL, "COMMIT;");
	return 1;
}

stock bool:Inventory_SetSlotExtra(Inventory:inventory, slotid, index, value)
{
	if(!Inventory_IsValid(inventory) || !(0 <= index < MAX_SLOT_EXTRA) || !Inventory_IsValidSlot(inventory, slotid))
		return false;
	if(list_sizeof(Inventory_GetItems(inventory), slotid) == 0)
		return false;
	list_set_cell(Inventory_GetItems(inventory), slotid, InvExtra + index, value);
	return true;
}

stock Inventory_GetSlotExtra(Inventory:inventory, slotid, index = 0)
{
    if(!Inventory_IsValid(inventory) || !Inventory_IsValidSlot(inventory, slotid) || list_sizeof(Inventory_GetItems(inventory), slotid) == 0 || !(0 <= index < MAX_SLOT_EXTRA))
		return 0;
    return list_get(Inventory_GetItems(inventory), slotid, InvExtra + index);
}

stock Map:Inventory_GetSlotTempExtraMap(Inventory:inventory, slotid)
{
	if(!Inventory_IsValid(inventory) || !Inventory_IsValidSlot(inventory, slotid) || list_sizeof(Inventory_GetItems(inventory), slotid) == 0)
		return Map:0;
	new Map:map = Map:list_get(Inventory_GetItems(inventory), slotid, TempExtra);
	return map;
}

stock bool:Inventory_RemoveSlotTempExtra(Inventory:inventory, slotid, key[])
{
	if(!Inventory_IsValid(inventory) || !Inventory_IsValidSlot(inventory, slotid) || list_sizeof(Inventory_GetItems(inventory), slotid) == 0)
		return false;
	new Map:map = Map:list_get(Inventory_GetItems(inventory), slotid, TempExtra);
	if(!map_valid(map))
		return false;
	if(!map_has_key(map, key))
		return false;
	map_str_remove_deep(map, key);
	return true;
}

stock bool:Inventory_SetSlotTempExtra(Inventory:inventory, slotid, key[], data)
{
	if(!Inventory_IsValid(inventory) || !Inventory_IsValidSlot(inventory, slotid) || list_sizeof(Inventory_GetItems(inventory), slotid) == 0)
		return false;
	new Map:map = Map:list_get(Inventory_GetItems(inventory), slotid, TempExtra);
	if(!map_valid(map))
		return false;
	if(map_has_str_key(map, key))
		map_str_set(map, key, data);
	else
		map_str_add(map, key, data);
	return true;
}

stock bool:Inventory_SetSlotTempExtraArr(Inventory:inventory, slotid, key[], data[])
{
	if(!Inventory_IsValid(inventory) || !Inventory_IsValidSlot(inventory, slotid) || list_sizeof(Inventory_GetItems(inventory), slotid) == 0)
		return 0;
	new Map:map = Map:list_get(Inventory_GetItems(inventory), slotid, TempExtra);
	if(!map_valid(map))
		return false;
	if(map_has_str_key(map, key))
		map_set_arr(map, key, data);
	else
		map_str_add(map, key, data);
	return true;
}

stock Inventory_GetSlotTempExtra(Inventory:inventory, slotid, key[])
{
	if(!Inventory_IsValid(inventory) || !Inventory_IsValidSlot(inventory, slotid) || list_sizeof(Inventory_GetItems(inventory), slotid) == 0)
		return 0;
	new Map:map = Map:list_get(Inventory_GetItems(inventory), slotid, TempExtra);
	if(!map_valid(map) || !map_has_str_key(map, key))
		return 0;
	return map_str_get(map, key);
}

stock bool:Inventory_GetSlotTempExtraArr(Inventory:inventory, slotid, key[], data[])
{
	if(!Inventory_IsValid(inventory) || !Inventory_IsValidSlot(inventory, slotid) || list_sizeof(Inventory_GetItems(inventory), slotid) == 0)
		return false;
	new Map:map = Map:list_get(Inventory_GetItems(inventory), slotid, TempExtra);
	if(!map_valid(map) || !map_has_str_key(map, key))
		return false;
	map_get_arr(map, key, data)
	return true;
}

stock bool:Inventory_HasSlotTempExtra(Inventory:inventory, slotid, key[])
{
	if(!Inventory_IsValid(inventory) || !Inventory_IsValidSlot(inventory, slotid) || list_sizeof(Inventory_GetItems(inventory), slotid) == 0)
		return false;
	new Map:map = Map:list_get(Inventory_GetItems(inventory), slotid, TempExtra);
	if(!map_valid(map))
		return false;
	return map_has_str_key(map, key);
}

stock bool:World_FindItem(itemid, extra, &EntityType:entityType, &entityId, &slotid)
{
	entityId = 0;
	entityType = ENTITY_TYPE_NONE;
	new bool:found = false, index = 0;
	static data[E_ITEM_DATA];
	data[InvItem] = data[InvAmount] = 0;
	for_list(i : InventoriesList)
	{
		new Inventory:inventory = Inventory:iter_get(i);
		if(!Inventory_IsValid(inventory))
			continue;
		for_list(item : Inventory_GetItems(inventory))
		{
			iter_get_arr(item, data);
			if(iter_sizeof(item) != 0 && data[InvItem] == itemid && data[InvExtra][0] == extra && data[InvAmount] > 0)
			{
				found = true;
				entityType = Inventory_GetEntityType(inventory);
				entityId = Inventory_GetEntityID(inventory);
				slotid = index;
				break;
			}
			index++;
		}
		if(found) break;
		index = 0;
	}
	return found;
}