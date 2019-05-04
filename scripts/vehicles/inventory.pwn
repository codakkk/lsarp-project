#include <YSI_Coding\y_hooks>

hook OnPlayerClearData(playerid)
{
    pCurrentVehicleInventory[playerid] = 0;
    return 1;
}

stock Inventory:Vehicle_InitializeInventory(vehicleid)
{
    //if(VehicleInfo[vehicleid][vModel] == 0)
	   //return 0;
    new Inventory:inv = Inventory_New(2);
    map_add(VehicleInventory, vehicleid, List:inv);
    printf("Vehicle %d Inventory initialized", vehicleid);
    return inv;
}

stock Vehicle_UnloadInventory(vehicleid)
{
    map_remove_deep(VehicleInventory, vehicleid);
    printf("Vehicle %d Inventory unloaded", vehicleid);
    return 1;
}

stock Inventory:Vehicle_GetInventory(vehicleid)
{
	new Inventory:inv;
	if(map_has_key(VehicleInventory, vehicleid))
	{
    	map_get_safe(VehicleInventory, vehicleid, List:inv);
	}
	else
	{
		inv = Vehicle_InitializeInventory(vehicleid);
	}
	return inv;
}

stock bool:Vehicle_HasInventory(vehicleid)
{
	new Inventory:inv = Vehicle_GetInventory(vehicleid);
	if(inv == Inventory:0)
		return false;
    return list_valid(Vehicle_GetInventory(vehicleid));
}

hook OnPlayerVehicleSaved(vehicleid)
{
    Vehicle_SaveInventory(vehicleid);
    return 1;
}

hook OnPlayerVehicleUnLoaded(vehicleid)
{
    Vehicle_UnloadInventory(vehicleid);
    return 1;
}

hook OnPlayerVehicleLoaded(vehicleid)
{
    Vehicle_InitializeInventory(vehicleid);
    Vehicle_LoadInventory(vehicleid);
    return 1;
}

stock Vehicle_AddItem(vehicleid, itemid, amount = 1, extra = 0)
{
    new Inventory:vehicleInventory = Vehicle_GetInventory(vehicleid);
	//if(vehicleInventory == Inventory:0)
		//return INVENTORY_INVALID;
    new result = Inventory_AddItem(vehicleInventory, itemid, amount, extra);
    Vehicle_SaveInventory(vehicleid);
    return result;
}

stock Vehicle_DecreaseSlotAmount(vehicleid, slotid, amount = 1)
{
    new Inventory:inventory = Vehicle_GetInventory(vehicleid);
    //if(inventory == Inventory:0)
		//return INVENTORY_INVALID;
    new result = Inventory_DecreaseSlotAmount(inventory, slotid, amount);

    Vehicle_SaveInventory(vehicleid);
    return result;
}

stock Vehicle_DecreaseItemAmount(vehicleid, itemid, amount = 1)
{
    new Inventory:inventory = Vehicle_GetInventory(vehicleid);
    
    new result = Inventory_DecreaseItemAmount(inventory, itemid, amount);

    Vehicle_SaveInventory(vehicleid);
    return result;
}

stock bool:Vehicle_HasSpaceForItem(vehicleid, itemid, amount)
{
    new Inventory:inventory = Vehicle_GetInventory(vehicleid);
    if(!list_valid(inventory))
	   return false;
    return Inventory_HasSpaceForItem(inventory, itemid, amount);
}

stock bool:Vehicle_IsSlotUsed(vehicleid, slotid)
{
    return Inventory_IsSlotUsed(Vehicle_GetInventory(vehicleid), slotid);
}

stock Vehicle_GetItemID(vehicleid, slotid)
{
    new Inventory:inventory = Vehicle_GetInventory(vehicleid);
    if(!list_valid(inventory))
	   return 0;
    new result = Inventory_GetSlotItem(inventory, slotid);
    return result;
}

stock Vehicle_GetSlotAmount(vehicleid, slotid)
{
    return Inventory_GetSlotAmount(Vehicle_GetInventory(vehicleid), slotid);
}

stock Vehicle_GetSlotExtra(vehicleid, slotid) // Should it be called "GetSlotExtra"?
{
    return Inventory_GetSlotExtra(Vehicle_GetInventory(vehicleid), slotid);
}

stock Vehicle_GetInventorySize(vehicleid)
{
    #pragma unused vehicleid
    return 4;
}

stock bool:Vehicle_ShowInventory(vehicleid, playerid)
{
    if(!Character_IsLogged(playerid))
	   return false;
    new Inventory:inventory = Vehicle_GetInventory(vehicleid);
    if(!list_valid(inventory))
	   return false;
    
    new 
	   string[4096],
	   temp[16],
	   tempItem[E_ITEM_DATA];
    for_inventory(i : inventory)
    {
	   if(iter_sizeof(i) == 0) // If no item
	   {
		  tempItem[gInvItem] = tempItem[gInvAmount] = tempItem[gInvExtra] = 0;
		  format(string, sizeof(string), "%s{808080}Slot Libero\t{808080}--\t{808080}--\n", string);
	   }
	   else
	   {
		  iter_get_arr(i, tempItem);
		  new itemid = tempItem[gInvItem],
			 itemAmount = tempItem[gInvAmount];
		  if(ServerItem_IsUnique(itemid))
			 temp = "--";
		  else
			 format(temp, sizeof(temp), "%d", itemAmount);
		  format(string, sizeof(string), "%s{FFFFFF}%s\t{FFFFFF}%s\t{FFFFFF}%s\n", string, ServerItem[itemid][sitemName], temp, ServerItem_GetTypeName(itemid));
	   }
    }
    new title[48];
    format(title, sizeof(title), "Inventario (%d/%d)", Inventory_GetUsedSpace(inventory), Inventory_GetSpace(inventory));
    Dialog_Show(playerid, Dialog_VehicleInventory, DIALOG_STYLE_TABLIST_HEADERS, title, "Nome\tQuantità\tTipo\n%s", "Avanti", "Chiudi", string);
    pCurrentVehicleInventory[playerid] = vehicleid;
    return true;
}

Dialog:Dialog_VehicleInventory(playerid, response, listitem, inputtext[])
{
    new vehicleid = pCurrentVehicleInventory[playerid];
    if(!response || vehicleid == 0)
	   return 0;
    if(Vehicle_IsSlotUsed(vehicleid, listitem) == false)
	   return Vehicle_ShowInventory(vehicleid, playerid);
    new itemid = Vehicle_GetItemID(vehicleid, listitem);
    new title[48];
    format(title, sizeof(title), "%s (%d)", ServerItem_GetName(itemid), Vehicle_GetSlotAmount(vehicleid, listitem));
    Dialog_Show(playerid, D_VehicleInventoryItem, DIALOG_STYLE_LIST, title, "Preleva", "Avanti", "Chiudi");
    pSelectedListItem[playerid] = listitem;
    return 1;
}

Dialog:D_VehicleInventoryItem(playerid, response, listitem, inputtext[])
{
    if(!response || pSelectedListItem[playerid] == -1)
	   return 0;
    new vehicleid = pCurrentVehicleInventory[playerid],
	   slotid = pSelectedListItem[playerid],
	   itemid = Vehicle_GetItemID(vehicleid, slotid);
    if(itemid == 0)
	   return 0;
    if(!IsPlayerInRangeOfVehicle(playerid, vehicleid, 5.0))
	   return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al veicolo!");
    switch(listitem)
    {
	   //case 0: // use
	   //{
		  //Trigger_OnPlayerInvItemUse(playerid, slotid, itemid, ServerItem_GetType(itemid));
	   //}
	   case 0: // withdraw
	   {
		  printf("%s Unique: %s", ServerItem_GetName(itemid), (ServerItem_IsUnique(itemid) ? ("Yes") : ("No")));
		  if(ServerItem_IsUnique(itemid))
		  {
			 new title[48];
			 format(title, sizeof(title), "%s (%d)", ServerItem_GetName(itemid), Vehicle_GetSlotAmount(vehicleid, slotid));
			 Dialog_Show(playerid, D_VehInvItemAmount, DIALOG_STYLE_MSGBOX, title, "Sei sicuro di voler prelevare %s dal tuo veicolo?", "Preleva", "Chiudi", ServerItem_GetName(itemid));
		  }
		  else
		  {
			 new title[48];
			 format(title, sizeof(title), "%s (%d)", ServerItem_GetName(itemid), Vehicle_GetSlotAmount(vehicleid, slotid));
			 Dialog_Show(playerid, D_VehInvItemAmount, DIALOG_STYLE_INPUT, title, "Inserisci la quantità da ritirare.", "Avanti", "Chiudi");
		  }
	   }
    }
    return 1;
}

Dialog:D_VehInvItemAmount(playerid, response, listitem, inputtext[])
{
    if(!response)
	   return 0;
    new 
	   amount = 1, 
	   vehicleid = pCurrentVehicleInventory[playerid],
	   slotid = pSelectedListItem[playerid],
	   itemid = Vehicle_GetItemID(vehicleid, slotid),
	   extra = Vehicle_GetSlotExtra(vehicleid, slotid),
	   slotAmount = Vehicle_GetSlotAmount(vehicleid, slotid);
    if(!ServerItem_IsUnique(itemid) && (sscanf(inputtext, "d", amount) || amount < 1 || amount > slotAmount))
    {
	   new title[48];
	   format(title, sizeof(title), "%s (%d)", ServerItem_GetName(itemid), slotAmount);
	   Dialog_Show(playerid, D_VehInvItemAmount, DIALOG_STYLE_INPUT, title, "{FF0000}La quantità inserita non è valida!\n{FFFFFF}Inserisci la quantità da ritirare.", "Avanti", "Chiudi");
	   return 1;
    }
    if(!Character_HasSpaceForItem(playerid, itemid, amount))
    {
	   new title[48];
	   format(title, sizeof(title), "%s (%d)", ServerItem_GetName(itemid), slotAmount);
	   if(!ServerItem_IsUnique(itemid))
		  Dialog_Show(playerid, D_VehInvItemAmount, DIALOG_STYLE_INPUT, title, "{FF0000}Non hai abbastanza spazio nell'inventario.\n{FFFFFF}Inserisci la quantità da ritirare.", "Avanti", "Chiudi");
	   else
		  SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario.");
	   return 1;
    }
    if(Vehicle_DecreaseSlotAmount(vehicleid, slotid, amount))
    {
	   SendFormattedMessage(playerid, COLOR_GREEN, "Hai ritirato l'oggetto (%s, %d) dal veicolo.", ServerItem_GetName(itemid), amount);
	   Character_GiveItem(playerid, itemid, amount, extra);
    }
    return 1;
}


stock Vehicle_SaveInventory(vehicleid)
{
	if(!Vehicle_GetID(vehicleid) || !list_valid(Vehicle_GetInventory(vehicleid)))
		return 0;
	Inventory_SaveInDatabase(Vehicle_GetInventory(vehicleid), "vehicle_inventory", "VehicleID", Vehicle_GetID(vehicleid));
	return 1;
}

stock Vehicle_LoadInventory(vehicleid)
{
	if(!Vehicle_GetID(vehicleid))
		return 0;
	Inventory_LoadFromDatabase(Vehicle_GetInventory(vehicleid), "vehicle_inventory", "VehicleID", Vehicle_GetID(vehicleid));
	return 1;
}