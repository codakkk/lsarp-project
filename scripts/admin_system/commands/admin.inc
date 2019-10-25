flags:vsave(CMD_ADMIN);
CMD:vsave(playerid, params[])
{
	new vehicleid = GetPlayerVehicleID(playerid);
	if(vehicleid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo.");
	if(!Vehicle_IsTemporary(vehicleid))
		return SendClientMessage(playerid, COLOR_ERROR, "Questo veicolo è già inserito nel database.");
	if(Vehicle_InsertInDatabase(vehicleid))
	{
		SendMessageToAdmins(false, COLOR_ADMINCMD, "AdmAlert: %s ha aggiunto il veicolo id %d nel database.", Account_GetName(playerid), vehicleid);
	}
	else
	{
		SendClientMessage(playerid, COLOR_ERROR, "Non è stato possibile aggiungere il veicolo nel database.");
	}
	return 1;
}

flags:vfaction(CMD_ADMIN);
CMD:vfaction(playerid, params[])
{
	new vehicleid = GetPlayerVehicleID(playerid);
	if(vehicleid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo.");
	new factionid;
	if(sscanf(params, "d", factionid))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /vfaction <factionid>");
	if(!Faction_IsValid(factionid))
		return SendClientMessage(playerid, COLOR_ERROR, "Fazione non valida.");

	Vehicle_SetFaction(vehicleid, factionid);

	SendMessageToAdmins(false, COLOR_ADMINCMD, "AdmAlert: %s ha settato il veicolo id %d nella fazione id %d.", Account_GetName(playerid), vehicleid, factionid);
	Log(Account_GetName(playerid), "", "/vfaction", factionid);
	return 1;
}

flags:avmaxhp(CMD_ADMIN);
CMD:avmaxhp(playerid, params[])
{
	new id, health;
	if(sscanf(params, "dd", id, health) || health < 1000 || health > 10000)
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /avmaxhp <vehicleid> <max health (1000 - 10000)>");
	
	if(!Vehicle_IsValid(id))
		return SendClientMessage(playerid, COLOR_ERROR, "L'ID inserito non è valido.");

	Vehicle_SetMaxHealth(id, float(health));

	SendMessageToAdmins(false, COLOR_ADMINCMD, "AdmAlert: %s ha settato gli HP massimi del veicolo id %d a %f.", Account_GetName(playerid), id, float(health));
	Log(Account_GetName(playerid), "", "/avmaxhp", id);
	return 1;
}


flags:vdespawn(CMD_ADMIN);
CMD:vdespawn(playerid, params[])
{
	new vehicleid = GetPlayerVehicleID(playerid);
	if(vehicleid == 0 || !Vehicle_IsValid(vehicleid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo.");
	
	printf("Before Count: %d", Iter_Count(Vehicle));
	Vehicle_Despawn(vehicleid);
	printf("After Count: %d", Iter_Count(Vehicle));

	SendMessageToAdmins(false, COLOR_ADMINCMD, "AdmAlert: %s ha despawnato il veicolo id %d.", Account_GetName(playerid), vehicleid);
	return 1;
}

flags:giveitem(CMD_ADMIN);
CMD:giveitem(playerid, params[])
{
	new id, itemid, quantity;
	if(sscanf(params, "k<u>k<item>d", id, itemid, quantity))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /giveitem [playerid/partofname] <itemid/item_name> <quantità>");

	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è connesso.");

	if(itemid == INVALID_ITEM_ID || !ServerItem_IsValid(itemid) || itemid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "L'item inserito non è corretto.");

	if(! (-1000 <= quantity <= 1000))
		return SendClientMessage(playerid, COLOR_ERROR, "La quantità inserita è troppo grande. (Range: |-1000| - |1000| )");

	if(itemid == gItem_Money && !IsPlayerAdmin(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi givarti quest'item.");

	if(quantity > 0)
	{
		new result, extra = 0;
		
		if(itemid == gItem_Phone)
			result = Character_GivePhone(id);
		else
			result = Character_GiveItem(id, itemid, quantity, extra);
		if(result == INVENTORY_ADD_SUCCESS)
		{
			SendMessageToAdmins(0, COLOR_YELLOW, "AdmCmd: %s (%s) ha givato %s (Qnt: %d) a %s (%d).", 
			Account_GetName(playerid), Character_GetOOCName(playerid), ServerItem_GetName(itemid), quantity, Character_GetOOCName(id), id);
		}
		else if(result == INVENTORY_NO_SPACE)
		{
			SendFormattedMessage(playerid, COLOR_ERROR, "%s (%d) non ha abbastanza spazio nell'inventario.", Character_GetOOCName(id), id);
		}
    }
    else if(quantity < 0)
    {
		quantity = -quantity;
		Character_DecreaseItemAmount(id, itemid, quantity);
		SendMessageToAdmins(0, COLOR_YELLOW, "AdmCmd: %s (%s) ha rimosso %s (Qnt: %d) a %s (%d).", 
		Account_GetName(playerid), Character_GetOOCName(playerid), ServerItem_GetName(itemid), quantity, Character_GetOOCName(id), id);
	}
	//Inventory_Print(Character_GetInventory(id));
	return 1;
}



flags:giveweapon(CMD_ADMIN);
CMD:giveweapon(playerid, params[])
{
	new id, wid, ammo;
	if(sscanf(params, "k<u>k<weapon>i", id, wid, ammo))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /giveweapon [playerid/partofname] <weaponid/weapon name> <ammo>");
	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è connesso.");
	if(ammo < 0 || ammo > 1000)
		return SendClientMessage(playerid, COLOR_ERROR, "Valore munizioni errato. (0 - 1000)");
	if(wid < 0)
		return SendClientMessage(playerid, COLOR_ERROR, "ID arma errato.");
	Character_GiveWeapon(id, wid, ammo);
	SendMessageToAdmins(false, COLOR_ADMINCMD, "AdmAlert: %s ha dato l'arma id %d con %d munizioni a %s.", Account_GetName(playerid), wid, ammo, Character_GetOOCName(id));
	SendFormattedMessage(id, COLOR_GREEN, "[ADMIN]: %s ti ha dato l'arma id %d con %d munizioni.", Account_GetName(playerid), wid, ammo);
	Log(Account_GetName(playerid), Character_GetOOCName(id), "/giveweapon", wid);
	return 1;
}

flags:vowner(CMD_ADMIN);
CMD:vowner(playerid, params[])
{
	new vehicleid = GetPlayerVehicleID(playerid);
	if(vehicleid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo.");
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /vowner [playerid/partofname]");
	if(!Vehicle_IsOwnable(vehicleid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su questo veicolo.");
	if(Vehicle_IsTemporary(vehicleid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare il comando su questo veicolo. Prima utilizza /vsave.");
	if(!IsPlayerConnected(id)  || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è connesso.");
	if(Character_GetOwnedVehicleCount(id) == MAX_VEHICLES_PER_PLAYER)
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore possiede già il massimo di veicoli personali.");
	
	Character_AddOwnedVehicle(id, vehicleid);
	
	SendFormattedMessage(id, COLOR_GREEN, "L'admin %s (%d) ti ha settato proprietario del veicolo %d.", Account_GetName(playerid), playerid, vehicleid);
	SendMessageToAdmins(true, COLOR_YELLOW, "[ADMIN ALERT]: %s (%d) ha settato %s (%d) proprietario del veicolo %d.", Account_GetName(playerid), playerid, Character_GetOOCName(id), id, vehicleid);
	return 1;
}

flags:givevehicleitem(CMD_ADMIN);
CMD:givevehicleitem(playerid, params[])
{
    new id, itemid, quantity;
    if(sscanf(params, "dk<item>d", id, itemid, quantity))
	   return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /givevehicleitem <vehicleid> <itemid/item_name> <quantità>");
    
    if(id < 0 || id >= MAX_VEHICLES)
	   return SendClientMessage(playerid, COLOR_ERROR, "Il veicolo non è valido.");
    
    if(!Vehicle_HasInventory(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su questo veicolo.");

    if(id == INVALID_ITEM_ID || !ServerItem_IsValid(itemid))
	   return SendClientMessage(playerid, COLOR_ERROR, "L'item inserito non è corretto.");
    
    if(quantity > 0)
    {
	   new result = Vehicle_GiveItem(id, itemid, quantity);
	   if(result == INVENTORY_ADD_SUCCESS)
	   {
		  SendMessageToAdmins(0, COLOR_YELLOW, "AdmCmd: %s (%s) ha givato %s (Qnt: %d) nel veicolo id %d.", 
		  Account_GetName(playerid), Character_GetOOCName(playerid), ServerItem_GetName(itemid), quantity, id);
	   }
	   else if(result == INVENTORY_NO_SPACE)
	   {
		  SendFormattedMessage(playerid, COLOR_ERROR, "Il veicolo %d non ha abbastanza spazio nell'inventario.", id);
	   }
    }
    else if(quantity < 0)
    {
	   quantity = -quantity;
	   Vehicle_DecreaseItemAmount(id, itemid, quantity);
	   SendMessageToAdmins(0, COLOR_YELLOW, "AdmCmd: %s (%s) ha rimosso %s (Qnt: %d) al veicolo %d.", 
	   Account_GetName(playerid), Character_GetOOCName(playerid), ServerItem_GetName(itemid), quantity, id);
    }
    //Inventory_Print(Character_GetInventory(id));
    return 1;
}

flags:serialban(CMD_ADMIN);
CMD:serialban(playerid, params[])
{
	new id, reason[128];
	if(sscanf(params, "k<u>s[128]", id, reason))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /serialban [playerid/partofname] <motivo>");

	if(!IsPlayerConnected(id) || IsPlayerNPC(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");

	SendMessageToAdmins(true, COLOR_ADMINCMD, "[ADMIN-ALERTS]: %s ha serial-bannato %s (%s). Motivo: %s", Account_GetName(playerid), Character_GetOOCName(id), Account_GetName(id), reason);
	SendFormattedMessage(id, COLOR_ORANGE, "Sei stato bannato permanentemente da %s. Motivo: %s", Account_GetName(playerid), reason);

	new ip[16], serial[41];
	
	GetPlayerIp(id, ip, sizeof(ip));
	gpci(id, serial, sizeof(serial));

	mysql_tquery_f(gMySQL, "INSERT INTO `bans` (ip, serial) VALUES('%e', '%e')", ip, serial);

	KickEx(id);
	return 1;
}

flags:banip(CMD_ADMIN);
CMD:banip(playerid, params[])
{
	new id, reason[128];
	if(sscanf(params, "k<u>s[128]", id, reason))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /banip [playerid/partofname] <motivo>");
	if(!IsPlayerConnected(id) || IsPlayerNPC(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");
	
	SendMessageToAdmins(true, COLOR_ADMINCMD, "AdmCmd: %s ha bannato %s (%s) per IP. Motivo: %s", Account_GetName(playerid), Character_GetOOCName(id), Account_GetName(id), reason);
	SendFormattedMessage(id, COLOR_ORANGE, "Sei stato bannato permanentemente (IP) da %s. Motivo: %s", Account_GetName(playerid), reason);

	new ip[16], serial[41];
	
	GetPlayerIp(id, ip, sizeof(ip));
	gpci(playerid, serial, sizeof(serial));

	mysql_tquery_f(gMySQL, "INSERT INTO `bans` (ip, serial) VALUES('%e', '%e')", ip, serial);

	KickEx(id);
	
	Discord_SendBanMessage("L'admin **%s** ha _bannato_ **%s** (**%s**) per **IP**. Motivo: __**%s**.", Account_GetName(playerid), Character_GetOOCName(id), Account_GetName(id), reason);
	return 1;
}

flags:unbanip(CMD_ADMIN);
CMD:unbanip(playerid, params[])
{
	new ip[16], reason[128];
	if(sscanf(params, "s[16]s[128]", ip, reason))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /unbanip <ip> <motivo>");	
	
	inline OnSelect()
	{
		if(cache_affected_rows() > 0)
		{
			SendMessageToAdmins(true, COLOR_ADMINCMD, "AdmCmd: %s ha sbannato l'IP %s. Motivo: %s", Account_GetName(playerid), ip, reason);
			Discord_SendBanMessage("L'admin **%s** ha _sbannato_ l'**IP**. Motivo: __**%s**.", Account_GetName(playerid), ip, reason);
		}
		else
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non è stato questo Ip tra quelli bannati.");
		}
	}

	MySQL_TQueryInline(gMySQL, using inline OnSelect, "DELETE FROM `bans` WHERE ip = '%e'", ip);
	return 1;
}

flags:removechopshop(CMD_ADMIN);
CMD:removechopshop(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /removechopshop [playerid/partofname]");
	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Giocatore non connesso.");
	SendFormattedMessage(id, COLOR_ORANGE, "L'admin %s (%d) ti ha rimosso i permessi per il Chop Shop.", Account_GetName(playerid), playerid);
	SendFormattedMessage(playerid, COLOR_ORANGE, "Hai rimosso i permessi per il Chop Shop a %s (%d).", Character_GetOOCName(id), id);
	SendMessageToAdmins(true, COLOR_ADMINCMD, "[ADMIN-CMD]: %s (%d) ha rimosso i permessi per il Chop Shop a %s (%d).", Account_GetName(playerid), playerid, Character_GetOOCName(id), id);
	Character_SetChopShopPrivileges(id, false);
	Character_SavePrivileges(id);

	LogCharacter(id, playerid, LOG_TYPE_PRIVILEGE, "ChopShop", 0);
	return 1;
}

flags:getprivileges(CMD_ADMIN);
CMD:getprivileges(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /getprivileges [playerid/partofname]");
	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Giocatore non connesso.");
	SendFormattedMessage(playerid, COLOR_ORANGE, "- Privilegi di %s -", Character_GetOOCName(id));
	SendFormattedMessage(playerid, COLOR_ORANGE, "ChopShop: %s", (Character_HasChopShopPrivileges(id) ? ("Si") : ("No")));
	return 1;
}


flags:setchopshop(CMD_ADMIN);
CMD:setchopshop(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /setchopshop [playerid/partofname]");
	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Giocatore non connesso.");
	SendFormattedMessage(id, COLOR_ORANGE, "L'admin %s (%d) ti ha garantito i permessi per il Chop Shop.", Account_GetName(playerid), playerid);
	SendFormattedMessage(playerid, COLOR_ORANGE, "Hai garantito i permessi per il Chop Shop a %s (%d).", Character_GetOOCName(id), id);
	SendMessageToAdmins(true, COLOR_ADMINCMD, "[ADMIN-CMD]: %s (%d) ha garantito i permessi per il Chop Shop a %s (%d).", Account_GetName(playerid), playerid, Character_GetOOCName(id), id);
	Character_SetChopShopPrivileges(id, true);
	Character_SavePrivileges(id);

	LogCharacter(id, playerid, LOG_TYPE_PRIVILEGE, "ChopShop", 1);
	return 1;
}

flags:settester(CMD_ADMIN);
CMD:settester(playerid, params[])
{
	new id, lvl;
	if(sscanf(params, "k<u>d", id, lvl))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /settester <playerid/nome>");

	if(!Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");
	
	if(Account_GetAdminLevel(id) > 1)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su un utente che è già moderatore o più.");
	
	if(Account_GetAdminLevel(id) == 1)
	{
		SendFormattedMessage(id, COLOR_ADMINCMD, "AdmCmd: %s (%d) ti ha rimosso dallo staff.", Account_GetName(playerid), playerid);
		SendFormattedMessage(playerid, COLOR_ADMINCMD, "Hai rimosso %s (%d) dallo staff.", Account_GetName(id), id);
		SendMessageToAdmins(true, COLOR_ADMINCMD, "AdmAlert: %s (%d) ha rimosso %s (%d) dallo staff.", Account_GetName(playerid), playerid, Account_GetName(id), id);
		Account_SetAdminLevel(id, 0);
	}
	else
	{
		SendFormattedMessage(id, COLOR_ADMINCMD, "AdmCmd: %s (%d) ti ha settato Tester.", Account_GetName(playerid), playerid, GetAdminLevelName(lvl));
		SendFormattedMessage(playerid, COLOR_ADMINCMD, "%s (%d) è stato settato Tester.", Account_GetName(id), id, GetAdminLevelName(lvl));
		SendMessageToAdmins(true, COLOR_ADMINCMD, "AdmAlert: %s (%d) ha settato %s (%d) Tester.", Account_GetName(playerid), playerid, Account_GetName(id), id);
		Account_SetAdminLevel(id, 1);
	}
	return 1;
}

flags:amm(CMD_ADMIN);
CMD:amm(playerid, params[])
{
	if(Player_IsDisabledAdminAlerts(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Hai disabilitato la chat admin.");
	if(isnull(params) || strlen(params) > 128)
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /amm <testo>");
	
	foreach(new i : Player)
	{
		if(!Character_IsLogged(i) || Player_IsDisabledAdminAlerts(i))
			continue;
		if(Account_GetAdminLevel(i) < 3)
			continue;
		SendFormattedMessage(i, 0x876200FF, "[ADMIN-CHAT] %s (%s): %s", Account_GetName(playerid), Character_GetOOCName(playerid), params);
	}
	return 1;
}