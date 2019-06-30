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
	if(bool:map_has_key(VehicleInventory, vehicleid))
	{
		printf("Unable to initialize vehicle id %d inventory.", vehicleid);
		return Inventory:0;	
	}
    new Inventory:inv = Inventory_New(2);
    map_add(VehicleInventory, vehicleid, List:inv);
    printf("Vehicle %d Inventory initialized", vehicleid);
    return inv;
}

stock Vehicle_UnloadInventory(vehicleid)
{
	if(Vehicle_IsValid(vehicleid) && map_has_key(VehicleInventory, vehicleid))
	{
    	map_remove_deep(VehicleInventory, vehicleid);
    	printf("Vehicle %d Inventory unloaded", vehicleid);
	}
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

// Checks if slotid is in the inventory boundaries (0 <= slotid <= Inventory_GetSpace(inventory))
stock Vehicle_IsValidSlot(vehicleid, slotid)
{
	return Inventory_IsValidSlot(Vehicle_GetInventory(vehicleid), slotid);
}

stock bool:Vehicle_HasInventory(vehicleid)
{
	new Inventory:inv = Vehicle_GetInventory(vehicleid);
	if(inv == Inventory:0)
		return false;
    return list_valid(Vehicle_GetInventory(vehicleid));
}

hook OnVehicleSaved(vehicleid)
{
    Vehicle_SaveInventory(vehicleid);
    return 1;
}

hook OnVehicleUnloaded(vehicleid)
{
    Vehicle_UnloadInventory(vehicleid);
    return 1;
}

hook OnVehicleLoaded(vehicleid)
{
    Vehicle_InitializeInventory(vehicleid);
    Vehicle_LoadInventory(vehicleid);
    return 1;
}

stock Vehicle_GiveItem(vehicleid, itemid, amount = 1, extra = 0)
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

stock Vehicle_HasSpaceForItem(vehicleid, itemid, amount)
{
    new Inventory:inventory = Vehicle_GetInventory(vehicleid);
    if(!list_valid(inventory))
	   return 0;
    return Inventory_HasSpaceForItem(inventory, itemid, amount);
}

stock bool:Vehicle_IsSlotUsed(vehicleid, slotid)
{
    return Inventory_IsSlotUsed(Vehicle_GetInventory(vehicleid), slotid);
}

stock Vehicle_GetSlotItem(vehicleid, slotid)
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
	pCurrentVehicleInventory[playerid] = vehicleid;
    new Inventory:inventory = Vehicle_GetInventory(vehicleid);
	Inventory_ShowStr(inventory, playerid, str_format("Inventario Veicolo (%d/%d)", Inventory_GetUsedSpace(inventory), Inventory_GetSpace(inventory)), Dialog_VehicleInventory, "Avanti", "Chiudi");
	return true;
}

Dialog:Dialog_VehicleInventory(playerid, response, listitem, inputtext[])
{
	new vehicleid = pCurrentVehicleInventory[playerid];
	if(!response || vehicleid == 0)
		return 0;
	
	pSelectedListItem[playerid] = listitem;

	new itemid = Vehicle_GetSlotItem(vehicleid, listitem); 
	if(itemid == 0)
	{
		Dialog_Show(playerid, Dialog_VehicleInvAction, DIALOG_STYLE_LIST, "Slot Libero", "Deposita (da inventario)\nDeposita arma\nDisassembla arma", "Continua", "Annulla");
	}
	else
	{
		new title[48];
		new extra = Vehicle_GetSlotExtra(vehicleid, listitem);
		if( (ServerItem_IsWeapon(itemid) && extra > 0) || (Weapon_IsGrenade(itemid)))
		{
			if(Weapon_IsGrenade(itemid))
				format(title, sizeof(title), "%s (Munizioni: %d)", ServerItem_GetName(itemid), Vehicle_GetSlotAmount(vehicleid, listitem));
			else if(extra > 0)
				format(title, sizeof(title), "%s (Munizioni: %d)", ServerItem_GetName(itemid), extra);
			Dialog_Show(playerid, Dialog_VehicleInvAction, DIALOG_STYLE_LIST, title, "Preleva\nDeposita\nEquipaggia", "Avanti", "Chiudi");
		}
		else
		{
			format(title, sizeof(title), "%s (Quantità: %d)", ServerItem_GetName(itemid), Vehicle_GetSlotAmount(vehicleid, listitem));
			Dialog_Show(playerid, Dialog_VehicleInvAction, DIALOG_STYLE_LIST, title, "Preleva\nDeposita", "Avanti", "Chiudi");
		}
	}
	return 1;
}

Dialog:Dialog_VehicleInvAction(playerid, response, listitem, inputtext[])
{
	if(!response || pSelectedListItem[playerid] == -1)
		return Vehicle_ShowInventory(pCurrentVehicleInventory[playerid], playerid);
	
	new vehicleid = pCurrentVehicleInventory[playerid];

	if(!IsPlayerInRangeOfVehicle(playerid, vehicleid, 5.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al veicolo.");

	new itemid = Vehicle_GetSlotItem(vehicleid, pSelectedListItem[playerid]);

	if(itemid == 0)
	{
		if(listitem == 0)
			Inventory_Show(Character_GetInventory(playerid), playerid, "Seleziona oggetto da depositare", Dialog_CharInvDepInVeh, "Deposita", "Annulla"); 
		else if(listitem == 1)
			pc_cmd_vdeposita(playerid, "");
		else if(listitem == 2)
			pc_cmd_vdisassembla(playerid, "");
	}
	else
	{
		if(listitem == 0) // Preleva
		{
			if(ServerItem_IsUnique(itemid))
			{
				new title[48];
				format(title, sizeof(title), "%s", ServerItem_GetName(itemid));
				Dialog_Show(playerid, D_VehInvItemAmount, DIALOG_STYLE_MSGBOX, title, "Sei sicuro di voler prelevare %s dal tuo veicolo?", "Preleva", "Chiudi", ServerItem_GetName(itemid));
			}
			else
			{
				new title[48];
				format(title, sizeof(title), "%s (%d)", ServerItem_GetName(itemid), Vehicle_GetSlotAmount(vehicleid, pSelectedListItem[playerid]));
				Dialog_Show(playerid, D_VehInvItemAmount, DIALOG_STYLE_INPUT, title, "Inserisci la quantità da ritirare.", "Avanti", "Chiudi");
			}
		}
		else if(listitem == 1)
		{
			if(ServerItem_IsUnique(itemid))
				return SendClientMessage(playerid, COLOR_ERROR, "Non puoi depositare altri oggetti di questo tipo in questo slot.");
			new characterItemAmount = Inventory_GetItemAmount(Character_GetInventory(playerid), itemid);
			if(characterItemAmount <= 0)
				return SendClientMessage(playerid, COLOR_ERROR, "Non hai altri oggetti di questo tipo nel tuo inventario.");
			pSelectedListItem[playerid] = itemid;
			Dialog_Show(playerid, Dialog_VehInvAddDep, DIALOG_STYLE_INPUT, "Deposita", "Inserisci la quantità di %s che vuoi depositare.\n{00FF00}Disponibilità: %d{FFFFFF}", "Deposita", "Annulla", ServerItem_GetName(itemid), characterItemAmount);
		}
		else if(listitem == 2)
		{
			if(Character_HasWeaponInSlot(playerid, Weapon_GetSlot(itemid)) && !Weapon_IsGrenade(itemid))
				return SendClientMessage(playerid, COLOR_ERROR, "Hai già un'arma equipaggiata.");
			if(Weapon_IsGrenade(itemid) && Character_GetWeaponInSlot(playerid, 8) != itemid)
				return SendClientMessage(playerid, COLOR_ERROR, "Hai già un'arma equipaggiata.");
			new ammoType = Weapon_GetAmmoType(itemid),
				extra = Vehicle_GetSlotExtra(vehicleid, pSelectedListItem[playerid]),
				ammo = 0;
				//invAmmo = Inventory_GetItemAmount(Character_GetInventory(playerid), ammoType);
			#pragma unused ammoType
			if(Weapon_RequireAmmo(itemid))
			{
				if(extra <= 0)
					return SendClientMessage(playerid, COLOR_ERROR, "L'arma è scarica e non puo' essere equipaggiata.");
				ammo = extra;
			}
			else if(Weapon_IsGrenade(itemid))
			{
				ammo = Vehicle_GetSlotAmount(vehicleid, pSelectedListItem[playerid]);
			}
			else 
				ammo = 1;
			if(ammo > 0)
			{
				SendFormattedMessage(playerid, COLOR_GREEN, "Hai preso %s (Munizioni: %d) dal veicolo.", ServerItem_GetName(itemid), ammo);
				Character_AMe(playerid, "prende un'arma dal veicolo.");
				Character_GiveWeapon(playerid, itemid, ammo);
				if(Weapon_IsGrenade(itemid))
					Vehicle_DecreaseSlotAmount(vehicleid, pSelectedListItem[playerid], ammo);
				else
					Vehicle_DecreaseSlotAmount(vehicleid, pSelectedListItem[playerid], 1);
			}
		}
	}
	return 1;
}

Dialog:Dialog_VehInvAddDep(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Vehicle_ShowInventory(pCurrentVehicleInventory[playerid], playerid);
	new 
		vehicleid = pCurrentVehicleInventory[playerid],
		amount = 1,
		itemid = pSelectedListItem[playerid],
		totalAmount = Inventory_GetItemAmount(Character_GetInventory(playerid), itemid);
	if(!ServerItem_IsUnique(itemid) && sscanf(inputtext, "d", amount) || amount < 1 || amount > totalAmount)
	{
		new title[48];
		format(title, sizeof(title), "%s (%d)", ServerItem_GetName(itemid), totalAmount);
		Dialog_Show(playerid, Dialog_VehInvAddDep, DIALOG_STYLE_INPUT, title, "{FF0000}La quantità inserita non è valida!\n{FFFFFF}Inserisci la quantità da depositare.\n{00FF00}Disponibilità: %d{FFFFFF}", "Avanti", "Chiudi", totalAmount);
		return 1;
	}
	if(!Vehicle_HasSpaceForItem(playerid, itemid, amount))
	{
		new title[48];
		format(title, sizeof(title), "%s (%d)", ServerItem_GetName(itemid), totalAmount);
		Dialog_Show(playerid, Dialog_VehInvAddDep, DIALOG_STYLE_INPUT, title, "{FF0000}Il veicolo non ha abbastanza spazio.\n{FFFFFF}Inserisci la quantità da depositare.\n{00FF00}Disponibilità: %d{FFFFFF}", "Avanti", "Chiudi", totalAmount);
		return 1;
	}
	if(Character_DecreaseItemAmount(playerid, itemid, amount))
	{
		SendFormattedMessage(playerid, COLOR_GREEN, "Hai depositato %d %s nel veicolo.", amount, ServerItem_GetName(itemid));
		Vehicle_GiveItem(vehicleid, itemid, amount, 0);
		Character_AMe(playerid, "deposita qualcosa all'interno del veicolo.");
	}
	return 1;
}

Dialog:ABC(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Vehicle_ShowInventory(pCurrentVehicleInventory[playerid], playerid);
	new 
		vehicleid = pCurrentVehicleInventory[playerid],
		amount = 1, 
		slotid = pSelectedListItem[playerid],
		itemid = Character_GetSlotItem(playerid, slotid),
		slotAmount = Character_GetSlotAmount(playerid, slotid),
		extra = Character_GetSlotExtra(playerid, slotid);
	if(!ServerItem_IsUnique(itemid) && sscanf(inputtext, "d", amount) || amount < 1 || amount > slotAmount)
	{
		new title[48];
		format(title, sizeof(title), "%s (%d)", ServerItem_GetName(itemid), slotAmount);
		Dialog_Show(playerid, ABC, DIALOG_STYLE_INPUT, title, "{FF0000}La quantità inserita non è valida!\n{FFFFFF}Inserisci la quantità da depositare.", "Avanti", "Chiudi");
		return 1;
	}
	if(!Vehicle_HasSpaceForItem(playerid, itemid, amount))
	{
		new title[48];
		format(title, sizeof(title), "%s (%d)", ServerItem_GetName(itemid), slotAmount);
		if(!ServerItem_IsUnique(itemid))
			Dialog_Show(playerid, ABC, DIALOG_STYLE_INPUT, title, "{FF0000}Il veicolo non ha abbastanza spazio.\n{FFFFFF}Inserisci la quantità da depositare.", "Avanti", "Chiudi");
		else
			SendClientMessage(playerid, COLOR_ERROR, "Il veicolo non ha abbastanza spazio nell'inventario.");
		return 1;
	}
	if(Character_DecreaseItemAmount(playerid, itemid, amount))
	{
		SendFormattedMessage(playerid, COLOR_GREEN, "Hai depositato %d %s nel veicolo.", amount, ServerItem_GetName(itemid));
		Vehicle_GiveItem(vehicleid, itemid, amount, extra);
		Character_AMe(playerid, "deposita qualcosa all'interno del veicolo.");
	}
	return 1;
}


Dialog:Dialog_CharInvDepInVeh(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Vehicle_ShowInventory(pCurrentVehicleInventory[playerid], playerid);
	new vehicleid = pCurrentVehicleInventory[playerid];
	if(!IsPlayerInRangeOfVehicle(playerid, vehicleid, 5.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al veicolo.");
	new itemid = Character_GetSlotItem(playerid, listitem);
	if(itemid == 0)
	{
		SendClientMessage(playerid, COLOR_ERROR, "Slot Vuoto.");
		new Inventory:inventory = Vehicle_GetInventory(vehicleid);
		return Inventory_ShowStr(inventory, playerid, str_format("Inventario Veicolo (%d/%d)", Inventory_GetUsedSpace(inventory), Inventory_GetSpace(inventory)), Dialog_VehicleInventory, "Avanti", "Chiudi");
	}
	else
	{
		pSelectedListItem[playerid] = listitem;
		new 
			slotAmount = Character_GetSlotAmount(playerid, listitem);
		if(ServerItem_IsUnique(itemid))
		{
			Dialog_Show(playerid, ABC, DIALOG_STYLE_MSGBOX, "Deposita", "Vuoi depositare %s nel veicolo?", "Deposita", "Annulla", ServerItem_GetName(itemid));
		}
		else
		{
			Dialog_Show(playerid, ABC, DIALOG_STYLE_INPUT, "Deposita", "Inserisci la quantità di %s che vuoi depositare.\n{00FF00}Disponibilità: %d{FFFFFF}", "Deposita", "Annulla", ServerItem_GetName(itemid), slotAmount);
		}
	}
	return 1;
}


Dialog:D_VehInvItemAmount(playerid, response, listitem, inputtext[])
{
    if(!response)
	   return Vehicle_ShowInventory(pCurrentVehicleInventory[playerid], playerid);
	new 
		amount = 1, 
		vehicleid = pCurrentVehicleInventory[playerid],
		slotid = pSelectedListItem[playerid],
		itemid = Vehicle_GetSlotItem(vehicleid, slotid),
		extra = Vehicle_GetSlotExtra(vehicleid, slotid),
		slotAmount = Vehicle_GetSlotAmount(vehicleid, slotid);
	if(!IsPlayerInRangeOfVehicle(playerid, vehicleid, 5.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al veicolo.");
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
		Character_AMe(playerid, "prende qualcosa dall'interno del veicolo.");
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