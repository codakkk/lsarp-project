#include <YSI_Coding\y_hooks>

flags:invmode(CMD_USER);
CMD:invmode(playerid, params[])
{
	Player_SetInvModeEnabled(playerid, !Player_HasInvModeEnabled(playerid));
	if(Player_HasInvModeEnabled(playerid))
	{
		SendClientMessage(playerid, COLOR_GREEN, "Hai impostato l'inventario in chat mode.");
	}
	else
	{
		SendClientMessage(playerid, COLOR_GREEN, "Ora l'inventario è mostrato a dialogs!");
	}
	Dialog_Close(playerid);
	return 1;
}

flags:inventario(CMD_USER);
CMD:inventario(playerid, params[])
{
	if(Player_HasInvModeEnabled(playerid))
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
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta attiva!");
	new slotid;
	if(sscanf(params, "d", slotid))
		return SendClientMessage(playerid, COLOR_ERROR, "/usa <slotid>");
	new itemid = Character_GetSlotItem(playerid, slotid);
	if(itemid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Slot non valida!");
	Character_UseInventoryItem(playerid, slotid);
	return 1;
}

flags:passa(CMD_ALIVE_USER);
CMD:passa(playerid, params[])
{
	if(PendingRequestInfo[playerid][rdPending])
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta attiva.");
	new slotid, id, amount;
	if(sscanf(params, "udD(1)", id, slotid, amount))
		return SendClientMessage(playerid, COLOR_ERROR, "/passa <playerid/partofname> <slotid> <quantità (default 1)>");

	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è connesso.");

	if(!IsPlayerInRangeOfPlayer(playerid, id, 2.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");

	new itemid = Character_GetSlotItem(playerid, slotid);

	if(itemid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Slot non valida.");

	new slotAmount = Character_GetSlotAmount(playerid, slotid);

	if(amount < 1 || amount > slotAmount)
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai la quantità necessaria.");
	if(Character_HasRequest(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore ha già una richiesta attiva.");

	Character_SetRequest(playerid, id, PENDING_TYPE_ITEM, itemid, amount, slotid);
	
	SendFormattedMessage(id, COLOR_GREEN, "%s (%d) vuole darti %d %s. Digita \"/accetta oggetto\" per accettare.", Character_GetOOCName(playerid), playerid, amount, ServerItem_GetName(itemid));
	SendFormattedMessage(playerid, COLOR_GREEN, "Hai proposto di dare %d %s a %s (%d).", amount, ServerItem_GetName(itemid), Character_GetOOCName(id), id);
	return 1;
}

flags:getta(CMD_ALIVE_USER);
CMD:getta(playerid, params[])
{
	if(!Character_IsAlive(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
	if(PendingRequestInfo[playerid][rdPending])
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta attiva!");
	if(IsPlayerInAnyVehicle(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando all'interno di un veicolo!");
	new slotid, amount;
	if(sscanf(params, "dD(1)", slotid, amount))
		return SendClientMessage(playerid, COLOR_ERROR, "/getta <slotid> <quantità (default 1)>");
	
	if(!Character_DropItem(playerid, slotid, amount))
		return SendClientMessage(playerid, COLOR_ERROR, "Slot non valida o quantità non valida!");
	return 1;
}

flags:deposita(CMD_ALIVE_USER);
CMD:deposita(playerid, params[])
{
	if(!Character_IsAlive(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
	if(PendingRequestInfo[playerid][rdPending])
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta attiva!");
	new itemid = GetPlayerWeapon(playerid);
	if(itemid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando senza un'arma");
	
	if(PendingRequestInfo[playerid][rdPending])
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta attiva!");
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
		Player_InfoStr(playerid, str_format("Hai depositato: ~g~%s~w~~n~nell'inventario~n~con %d munizioni.", Weapon_GetName(itemid), ammo), true);
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
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta attiva!");
	new 
		slotid, itemid, ammo;
	if(sscanf(params, "d", slotid))
	{
		itemid = GetPlayerWeapon(playerid);
		ammo = AC_GetPlayerAmmo(playerid);
		if(itemid == 0 || ammo == 0)
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando senza un'arma.");
			return SendClientMessage(playerid, COLOR_ERROR, "Altrimenti usa /disassembla <slotid> per disassemblare un'arma nell'inventario.");
		}
		if(!Weapon_CanBeDisassembled(itemid))
			return SendClientMessage(playerid, COLOR_ERROR, "Non puoi disassemblare quest'arma!");
		if(!Character_HasSpaceForItem(playerid, itemid, 1) || !Character_HasSpaceForItem(playerid, Weapon_GetAmmoType(itemid), ammo))
			return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario.");
		AC_RemovePlayerWeapon(playerid, itemid);
	}
	else
	{	
		if(!Character_IsValidSlot(playerid, slotid))
			return SendClientMessage(playerid, COLOR_ERROR, "Slot non valida!");
		itemid = Character_GetSlotItem(playerid, slotid);
		ammo = Character_GetSlotExtra(playerid, slotid);
		if(ServerItem_GetType(itemid) != ITEM_TYPE_WEAPON)
			return SendClientMessage(playerid, COLOR_ERROR, "L'oggetto selezionato non è un'arma!");
		if(!Weapon_CanBeDisassembled(itemid))
			return SendClientMessage(playerid, COLOR_ERROR, "Non puoi disassemblare quest'arma!");
		if(ammo == 0)
			return SendClientMessage(playerid, COLOR_ERROR, "L'arma selezionata è già disassemblata!");
		//if(!Character_HasSpaceForItem(playerid, itemid, 1) || !Character_HasSpaceForItem(playerid, Weapon_GetAmmoType(itemid), ammo))
			//return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario.");
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
	return 1;
}
alias:disassembla("dis");

flags:gettaarma(CMD_ALIVE_USER);
CMD:gettaarma(playerid, params[])
{
	if(!Character_IsAlive(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
	if(PendingRequestInfo[playerid][rdPending])
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta attiva!");
	new 
		weaponid = GetPlayerWeapon(playerid), 
		ammo = GetPlayerAmmo(playerid);
	if(weaponid == 0 || ammo == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai un'arma da gettare.");
	new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	Drop_Create(x, y, z - 0.9, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), weaponid, 1, ammo, Character_GetOOCNameStr(playerid));
	AC_RemovePlayerWeapon(playerid, weaponid);
	SendClientMessage(playerid, COLOR_GREEN, "Hai gettato l'arma.");
	ApplyAnimation(playerid, "GRENADE", "WEAPON_throwu", 3.0, 0, 0, 0, 0, 0);
	return 1;
}