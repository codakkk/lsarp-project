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
		SendClientMessage(playerid, COLOR_GREEN, "Ora l'inventario è mostrato a dialogs.");
	}
	Dialog_Close(playerid);
	return 1;
}

flags:inventario(CMD_USER);
CMD:inventario(playerid, params[])
{
	Character_ShowInventory(playerid, playerid);
	return 1;
}
alias:inventario("inv");

flags:usa(CMD_ALIVE_USER);
CMD:usa(playerid, params[])
{
	if(!Character_CanInteract(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
	if(Request_IsPending(playerid, REQUEST_TYPE_ITEM))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta d'oggetto attiva.");
	new slotid;
	if(sscanf(params, "k<slotid>", slotid))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /usa <slotid>");

	new itemid = Character_GetSlotItem(playerid, slotid);
	if(itemid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Slot non valida.");
	
	Character_UseInventoryItem(playerid, slotid);
	return 1;
}

flags:passa(CMD_ALIVE_USER);
CMD:passa(playerid, params[])
{
	if(!Character_CanInteract(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
	
	if(Request_IsPending(playerid, REQUEST_TYPE_ITEM))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta d'oggetto attiva.");
	new slotid, id, amount;
	if(sscanf(params, "k<u>k<slotid>D(1)", id, slotid, amount))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /passa [playerid/partofname] <slotid> <quantità (default 1)>");

	Character_SendItemRequest(playerid, id, slotid, amount);
	return 1;
}

flags:getta(CMD_ALIVE_USER);
CMD:getta(playerid, params[])
{
	if(!Character_CanInteract(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
	
	if(Character_IsFactionDuty(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se sei in servizio");
	
	new slotid, amount;
	if(sscanf(params, "k<slotid>D(1)", slotid, amount))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /getta <slotid> <quantità (default 1)>");
	
	Character_DropItem(playerid, slotid, amount);
	return 1;
}

flags:depositadenaro(CMD_ALIVE_USER);
CMD:depositadenaro(playerid, params[])
{
	new money;
	if(sscanf(params, "d", money))
		return SendClientMessage(playerid, COLOR_ERROR, "(/dep)osita(den)aro <ammontare>"), 0;
	if(money < 0 || money > Character_GetMoney(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai questo ammontare di denaro.");
	Character_DepositMoneyInInv(playerid, money);
	return 1;
}
alias:depositadenaro("depden", "depositaden", "depdenaro");

flags:depositaarma(CMD_ALIVE_USER | CMD_SLOW_MODE);
CMD:depositaarma(playerid, params[])
{

	if(!Character_CanInteract(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
	
	if(Character_IsFactionDuty(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se sei in servizio");

	if(Request_IsPending(playerid, REQUEST_TYPE_WEAPON))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta d'arma attiva.");
	
	new itemid , ammo;
	Character_GetWeaponData(playerid, Weapon_GetSlot(Character_GetWeapon(playerid)), itemid, ammo);
	if(itemid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando senza un'arma.");

	if(Weapon_IsGrenade(itemid))
	{
		if(!Character_HasSpaceForItem(playerid, itemid, ammo))
			return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario.");
		Character_GiveItem(playerid, itemid, ammo);
		//SetPlayerArmedWeapon(playerid, 0);
		Player_InfoStr(playerid, str_format("Hai depositato: ~g~%d ~y~%s~w~~n~nell'inventario.", ammo, Weapon_GetName(itemid)), true);
	}
	else
	{
		if(!Character_HasSpaceForItem(playerid, itemid, 1))
			return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario.");
		Character_GiveItem(playerid, itemid, 1, ammo);
		//SetPlayerArmedWeapon(playerid, 0);
		Player_InfoStr(playerid, str_format("Hai depositato: ~g~%s~w~~n~nell'inventario~n~con ~y~%d~w~ munizioni.", Weapon_GetName(itemid), ammo), true);
	}
	Character_RemoveWeapon(playerid, itemid);
	SetPlayerArmedWeapon(playerid, 0);
	return 1;
}
alias:depositaarma("dep");

flags:disassembla(CMD_ALIVE_USER);
CMD:disassembla(playerid, params[])
{
	new slotid;
	if(sscanf(params, "k<slotid>", slotid))
	{
		Character_DisassembleWeapon(playerid, -1);
	}
	else
	{
		Character_DisassembleWeapon(playerid, slotid);
	}
	return 1;
}
alias:disassembla("dis");

stock bool:Character_DisassembleWeapon(playerid, slotid = -1)
{
	if(!Character_CanInteract(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questa funzionalità ora."), false;
	
	if(Character_IsFactionDuty(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi disassemblare un'arma se sei in servizio."), false;

	if(Request_IsPending(playerid, REQUEST_TYPE_WEAPON))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi disassemblare un'arma se hai una richiesta d'arma attiva."), false;
	
	new itemid, ammo;
	if(slotid == -1)
	{
		Character_GetCurrentWeaponData(playerid, itemid, ammo);
		if(itemid == 0 || ammo == 0)
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando senza un'arma.");
			return SendClientMessage(playerid, COLOR_ERROR, "Altrimenti usa (/dis)assembla <slotid> per disassemblare un'arma nell'inventario."), false;
		}
		if(!Weapon_CanBeDisassembled(itemid))
			return SendClientMessage(playerid, COLOR_ERROR, "Non puoi disassemblare quest'arma."), false;
		new data[10], amounts[10];
		data[0] = itemid;
		data[1] = Weapon_GetAmmoType(itemid);
		amounts[0] = 1;
		amounts[1] = ammo;
		if(!Inventory_HasSpaceForItems(Character_GetInventory(playerid), data, amounts))
			return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario."), false;
		Character_RemoveWeapon(playerid, itemid);
	}
	else
	{	
		if(!Character_IsValidSlot(playerid, slotid))
			return SendClientMessage(playerid, COLOR_ERROR, "Slot non valida."), false;
		itemid = Character_GetSlotItem(playerid, slotid);
		ammo = Character_GetSlotExtra(playerid, slotid);
		if(ServerItem_GetType(itemid) != ITEM_TYPE_WEAPON)
			return SendClientMessage(playerid, COLOR_ERROR, "L'oggetto selezionato non è un'arma."), false;
		if(!Weapon_CanBeDisassembled(itemid))
			return SendClientMessage(playerid, COLOR_ERROR, "Non puoi disassemblare quest'arma."), false;
		if(ammo == 0)
			return SendClientMessage(playerid, COLOR_ERROR, "L'arma selezionata è giè disassemblata."), false;
		new data[10], amounts[10];
		data[0] = itemid;
		data[1] = Weapon_GetAmmoType(itemid);
		amounts[0] = 1;
		amounts[1] = ammo;
		if(!Inventory_HasSpaceForItems(Character_GetInventory(playerid), data, amounts))
			return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario."), false;
		Character_DecreaseSlotAmount(playerid, slotid, 1);
	}
	Character_GiveItem(playerid, itemid, 1);
	Character_GiveItem(playerid, Weapon_GetAmmoType(itemid), ammo);
	SendFormattedMessage(playerid, COLOR_GREEN, "Hai disassemblato la tua arma (%s). Munizioni: %d", ServerItem_GetName(itemid), ammo);
	Character_AMe(playerid, "disassembla un'arma.");
	return true;
}

flags:gettaarma(CMD_ALIVE_USER);
CMD:gettaarma(playerid, params[])
{
	if(!Character_CanInteract(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");

	if(Character_IsFactionDuty(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se sei in servizio");

	if(Request_IsPending(playerid, REQUEST_TYPE_WEAPON) && Request_GetSender(playerid, REQUEST_TYPE_WEAPON) == playerid)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta d'arma attiva.");
	new 
		weaponid, 
		ammo;
	if(!Character_GetCurrentWeaponData(playerid, weaponid, ammo) || weaponid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai un'arma da gettare.");
	new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	new result = Drop_Create(Character_GetOOCNameStr(playerid), x, y, z - 0.9, Character_GetVirtualWorld(playerid), GetPlayerInterior(playerid), weaponid, 1, ammo);
	if(result != -1 && Character_RemoveWeapon(playerid, weaponid))
	{
		SendClientMessage(playerid, COLOR_GREEN, "Hai gettato l'arma.");
		SetPlayerArmedWeapon(playerid, 0);
		ApplyAnimation(playerid, "GRENADE", "WEAPON_throwu", 3.0, 0, 0, 0, 0, 0);
	}
	return 1;
}