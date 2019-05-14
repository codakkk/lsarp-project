#include <YSI_Coding\y_hooks>

flags:invmode(CMD_USER);
CMD:invmode(playerid, params[])
{
	Account_SetInvModeEnabled(playerid, !Account_HasInvModeEnabled(playerid));
	if(Account_HasInvModeEnabled(playerid))
	{
		SendClientMessage(playerid, COLOR_GREEN, "Hai impostato l'inventario in chat mode.");
	}
	else
	{
		SendClientMessage(playerid, COLOR_GREEN, "Ora l'inventario è mostrato a dialogs.");
	}
	Dialog_Close(playerid);
	return 1;
}

flags:inventario(CMD_USER);
CMD:inventario(playerid, params[])
{
	if(Account_HasInvModeEnabled(playerid))
	{
		new Inventory:playerInventory = Character_GetInventory(playerid);
		Inventory_ShowInChat(playerInventory, playerid, "Inventario");
	}
	else Character_ShowInventory(playerid, playerid);
	return 1;
}
alias:inventario("inv");

flags:usa(CMD_ALIVE_USER);
CMD:usa(playerid, params[])
{
	if(!Character_IsAlive(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
	if(PendingRequestInfo[playerid][rdPending])
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta attiva.");
	new slotid;
	if(sscanf(params, "d", slotid))
		return SendClientMessage(playerid, COLOR_ERROR, "/usa <slotid>");
	new itemid = Character_GetSlotItem(playerid, slotid);
	if(itemid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Slot non valida.");
	Character_UseInventoryItem(playerid, slotid);
	return 1;
}

flags:passa(CMD_ALIVE_USER);
CMD:passa(playerid, params[])
{
	if(PendingRequestInfo[playerid][rdPending])
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta attiva.");
	new slotid, id, amount;
	if(sscanf(params, "k<u>dD(1)", id, slotid, amount))
		return SendClientMessage(playerid, COLOR_ERROR, "/passa <playerid/partofname> <slotid> <quantità (default 1)>");

	Character_SendItemRequest(playerid, id, slotid, amount);
	return 1;
}

flags:getta(CMD_ALIVE_USER);
CMD:getta(playerid, params[])
{
	new slotid, amount;
	if(sscanf(params, "dD(1)", slotid, amount))
		return SendClientMessage(playerid, COLOR_ERROR, "/getta <slotid> <quantità (default 1)>");
	
	Character_DropItem(playerid, slotid, amount);
	return 1;
}

flags:deposita(CMD_ALIVE_USER);
CMD:deposita(playerid, params[])
{
	if(!Character_IsAlive(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
	if(PendingRequestInfo[playerid][rdPending])
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta attiva.");
	new itemid = GetPlayerWeapon(playerid);
	if(itemid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando senza un'arma.");
	
	new ammo = AC_GetPlayerAmmo(playerid);
	if(Weapon_IsGrenade(itemid))
	{
		if(!Character_HasSpaceForItem(playerid, itemid, ammo))
			return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario.");
		Character_GiveItem(playerid, itemid, ammo);
		Player_InfoStr(playerid, str_format("Hai depositato: ~g~%d ~y~%s~w~~n~nell'inventario.", ammo, Weapon_GetName(itemid)), true);
	}
	else
	{
		if(!Character_HasSpaceForItem(playerid, itemid, 1))
			return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario.");
		Character_GiveItem(playerid, itemid, 1, ammo);
		Player_InfoStr(playerid, str_format("Hai depositato: ~g~%s~w~~n~nell'inventario~n~con ~y~%d~w~ munizioni.", Weapon_GetName(itemid), ammo), true);
	}
	AC_RemovePlayerWeapon(playerid, itemid);
	return 1;
}
alias:deposita("dep");

flags:disassembla(CMD_ALIVE_USER);
CMD:disassembla(playerid, params[])
{
	if(!Character_IsAlive(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
	if(PendingRequestInfo[playerid][rdPending])
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta attiva.");
	new 
		slotid, itemid, ammo;
	if(sscanf(params, "d", slotid))
	{
		itemid = GetPlayerWeapon(playerid);
		ammo = AC_GetPlayerAmmo(playerid);
		if(itemid == 0 || ammo == 0)
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando senza un'arma.");
			return SendClientMessage(playerid, COLOR_ERROR, "Altrimenti usa (/dis)assembla <slotid> per disassemblare un'arma nell'inventario.");
		}
		if(!Weapon_CanBeDisassembled(itemid))
			return SendClientMessage(playerid, COLOR_ERROR, "Non puoi disassemblare quest'arma.");
		new data[10], amounts[10];
		data[0] = itemid;
		data[1] = Weapon_GetAmmoType(itemid);
		amounts[0] = 1;
		amounts[1] = ammo;
		if(!Inventory_HasSpaceForItems(Character_GetInventory(playerid), data, amounts))
			return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario.");
		AC_RemovePlayerWeapon(playerid, itemid);
	}
	else
	{	
		if(!Character_IsValidSlot(playerid, slotid))
			return SendClientMessage(playerid, COLOR_ERROR, "Slot non valida.");
		itemid = Character_GetSlotItem(playerid, slotid);
		ammo = Character_GetSlotExtra(playerid, slotid);
		if(ServerItem_GetType(itemid) != ITEM_TYPE_WEAPON)
			return SendClientMessage(playerid, COLOR_ERROR, "L'oggetto selezionato non è un'arma.");
		if(!Weapon_CanBeDisassembled(itemid))
			return SendClientMessage(playerid, COLOR_ERROR, "Non puoi disassemblare quest'arma.");
		if(ammo == 0)
			return SendClientMessage(playerid, COLOR_ERROR, "L'arma selezionata è già disassemblata.");
		new data[10], amounts[10];
		data[0] = itemid;
		data[1] = Weapon_GetAmmoType(itemid);
		amounts[0] = 1;
		amounts[1] = ammo;
		if(!Inventory_HasSpaceForItems(Character_GetInventory(playerid), data, amounts))
			return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario.");
		Character_DecreaseSlotAmount(playerid, slotid, 1);
	}
	Character_GiveItem(playerid, itemid, 1);
	Character_GiveItem(playerid, Weapon_GetAmmoType(itemid), ammo);
	SendFormattedMessage(playerid, COLOR_GREEN, "Hai disassemblato la tua arma (%s). Munizioni: %d", ServerItem_GetName(itemid), ammo);
	Character_AMe(playerid, "disassembla un'arma.");
	return 1;
}
alias:disassembla("dis");

flags:gettaarma(CMD_ALIVE_USER);
CMD:gettaarma(playerid, params[])
{
	if(!Character_IsAlive(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
	if(PendingRequestInfo[playerid][rdPending])
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta attiva.");
	new 
		weaponid = GetPlayerWeapon(playerid), 
		ammo = GetPlayerAmmo(playerid);
	if(weaponid == 0 || ammo == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai un'arma da gettare.");
	new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	new result = Drop_Create(x, y, z - 0.9, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), weaponid, 1, ammo, Character_GetOOCNameStr(playerid));
	if(result != -1)
	{
		AC_RemovePlayerWeapon(playerid, weaponid);
		SendClientMessage(playerid, COLOR_GREEN, "Hai gettato l'arma.");
		ApplyAnimation(playerid, "GRENADE", "WEAPON_throwu", 3.0, 0, 0, 0, 0, 0);
	}
	return 1;
}