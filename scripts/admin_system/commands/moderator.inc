flags:sethp(CMD_MODERATOR);
CMD:sethp(playerid, params[])
{
    new id, hp;
    if(sscanf(params, "k<u>d", id, hp))
	   return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /sethp <playerid/nome> <health>");
    
    if(id < 0 || id >= MAX_PLAYERS  || !Character_IsLogged(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "L'utente non � collegato.");
    
    if(hp < 0 || hp > 200.0)
	   return SendClientMessage(playerid, COLOR_ERROR, "Valore HP non valido.");

	if(Character_IsDead(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su un giocatore in sistema morte.");

    Character_SetHealth(id, hp);
    SendMessageToAdmins(0, COLOR_ADMINCMD, "AdmCmd: %s (%s) ha settato gli HP di %s (%d) a %d.", Account_GetName(playerid), Character_GetOOCName(playerid), Character_GetOOCName(id), id, hp);
    SendFormattedMessage(id, COLOR_GREEN, "[ADMIN] %s (%s) ti ha settato gli HP a %d.", Account_GetName(playerid), Character_GetOOCName(playerid), hp);
    Log(Account_GetName(playerid), Character_GetOOCName(id), "/sethp", hp);
    return 1;
}
alias:sethp("sethealth");

flags:setarmour(CMD_MODERATOR);
CMD:setarmour(playerid, params[])
{
    new id, hp;
    if(sscanf(params, "k<u>d", id, hp))
	   return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /setarmour <playerid/nome> <armour>");
    
    if(id < 0 || id >= MAX_PLAYERS  || !Character_IsLogged(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "L'utente non � collegato.");

    if(hp < 0 || hp > 200)
	   return SendClientMessage(playerid, COLOR_ERROR, "Valore Armatura non valido.");

    Character_SetArmour(id, hp);
    SendMessageToAdmins(0, COLOR_ADMINCMD, "AdmCmd: %s (%s) ha settato l'armatura di %s (%d) a %d.", Account_GetName(playerid), Character_GetOOCName(playerid), Character_GetOOCName(id), id, hp);
    SendFormattedMessage(id, COLOR_GREEN, "[ADMIN] %s (%s) ti ha settato l'armatura a %d.", Account_GetName(playerid), Character_GetOOCName(playerid), hp);
    Log(Account_GetName(playerid), Character_GetOOCName(id), "/setarmour", hp);
    return 1;
}
alias:setarmour("setap");



flags:setvhp(CMD_MODERATOR);
CMD:setvhp(playerid, params[])
{
	new id, Float:hp;
	if(sscanf(params, "df", id, hp))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /setvhp <vehicleid> <health>");

	if(id < 0 || id >= MAX_VEHICLES)
		return SendClientMessage(playerid, COLOR_ERROR, "Il veicolo non esiste.");

	if(hp < 0.0 || hp > Vehicle_GetMaxHealth(id))
		return SendClientMessage(playerid, COLOR_ERROR, "La salute inserita per il veicolo non � valida.");
	//RepairVehicle(id);
	SetVehicleHealth(id, hp);
	SendMessageToAdmins(0, COLOR_YELLOW, "AdmCmd: %s (%s) ha settato gli HP a %f del veicolo %d.", Account_GetName(playerid), Character_GetOOCName(playerid), hp, id);
	Log(Account_GetName(playerid), "", "/setvhp", id);
	return 1;
}
alias:setvhp("setvhealth");

flags:fixveh(CMD_MODERATOR);
CMD:fixveh(playerid, params[])
{
	new id;
	if(sscanf(params, "d", id))
	{
    	id = GetPlayerVehicleID(playerid);
	}

    if(!Vehicle_IsValid(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "L'ID inserito non � valido o non sei su un veicolo.");

    Vehicle_Repair(id);

    SendMessageToAdmins(0, COLOR_YELLOW, "AdmCmd: %s (%d) ha riparato il veicolo id %d.", Account_GetName(playerid), playerid, id);
    Log(Account_GetName(playerid), "", "/fixveh", id);
    return 1;
}
alias:fixveh("fix");

flags:avcolor(CMD_MODERATOR);
CMD:avcolor(playerid, params[])
{
	new id, color1, color2;
	if(sscanf(params, "ddd", id, color1, color2))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /avcolor <vehicleid> <color1> <color2>");

	if(!IsValidVehicle(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il veicolo inserito non � valido.");

	if(color1 < 0 || color1 > 255)
		return SendClientMessage(playerid, COLOR_ERROR, "Il colore 1 inserito non � valido (0 - 255).");

	if(color2 < 0 || color2 > 255)
		return SendClientMessage(playerid, COLOR_ERROR, "Il colore 2 inserito non � valido (0 - 255).");
	
	Vehicle_SetColors(id, color1, color2);

	SendMessageToAdmins(false, COLOR_ADMINCMD, "AdmAlert: %s ha modificato il colore del veicolo id %d.", Account_GetName(playerid), id);
	return 1;
}

flags:gotovehicle(CMD_MODERATOR);
CMD:gotovehicle(playerid, params[])
{
    new id;
    if(sscanf(params, "d", id))
	   return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /gotov <vehicleid>");
    
    if(!Vehicle_IsValid(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "Il veicolo non esiste.");

    new Float:x, Float:y, Float:z;
    GetVehiclePos(id, x, y, z);
    Character_SetSafePosition(playerid, x, y, z+2.0, 0, GetVehicleVirtualWorld(id));
    return 1;
}
alias:gotovehicle("gotov");

flags:getvhere(CMD_MODERATOR);
CMD:getvhere(playerid, params[])
{
    new id;
    if(sscanf(params, "d", id))
	   return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /getvhere <vehicleid>");
    
    if(id < 0 || id >= MAX_VEHICLES)
	   return SendClientMessage(playerid, COLOR_ERROR, "Il veicolo non esiste.");

    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    SetVehiclePos(id, x, y+5.0, z);
	SetVehicleVirtualWorld(id, Character_GetVirtualWorld(playerid));
	LinkVehicleToInterior(id, GetPlayerInterior(playerid));
    return 1;
}

flags:apark(CMD_MODERATOR);
CMD:apark(playerid, params[])
{
    new vehicleid = GetPlayerVehicleID(playerid);
    if(vehicleid == 0)
	   return SendClientMessage(playerid, COLOR_ERROR, "Non sei su un veicolo.");
    if(!Vehicle_GetModel(vehicleid))
	   return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su questo veicolo.");
    new Float:x, Float:y, Float:z, Float:a;
    GetVehiclePos(vehicleid, x, y, z);
    GetVehicleZAngle(vehicleid, a);

    Vehicle_Park(vehicleid, x, y, z, a);
	if(Vehicle_GetOwnerID(vehicleid) > 0)
	{
		new pid = -1;
		foreach(new i : Player)
		{
			if(!Character_IsLogged(i) || Character_GetID(i) != Vehicle_GetOwnerID(vehicleid))
				continue;
			pid = i;
			break;
		}

    	if(IsPlayerConnected(pid) && Character_IsLogged(pid))
			SendFormattedMessage(pid, COLOR_ERROR, "AdmAlert: %s [%d] ha parcheggiato il tuo veicolo altrove. Motivo: %s", Account_GetName(playerid), playerid, params);
	}
	SendFormattedMessage(playerid, COLOR_GREEN, "[ADMIN] Hai parcheggiato qui questo veicolo (%d).", vehicleid);
    Log(Account_GetName(playerid), "", "/apark", vehicleid);
    return 1;
}

flags:vfuel(CMD_MODERATOR);
CMD:vfuel(playerid, params[])
{
	new fuel = 0, vid;
	if(sscanf(params, "d", fuel))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /vfuel <fuel>");
	if( (vid = GetPlayerVehicleID(playerid)) == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo.");
	if(fuel < 0 || fuel > 100)
		return SendClientMessage(playerid, COLOR_ERROR, "Valore benzina non valido. (0 - 100).");
	Vehicle_SetFuel(vid, fuel);
	SendMessageToAdmins(false, COLOR_YELLOW, "AdmAlert: %s (%d) ha settato la benzina del veicolo %d. Benzina: %0.2f.", Account_GetName(playerid), playerid, vid, fuel);
	return 1;
}

flags:setskin(CMD_MODERATOR);
CMD:setskin(playerid, params[])
{
    new id, skid;
    if(sscanf(params, "k<u>d", id, skid))
	   return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /setskin <playerid/nome> <skin-id>");
    
    if(id < 0 || id >= MAX_PLAYERS || !Character_IsLogged(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "L'utente non � collegato.");
    
	if(skid < 1 || (skid > 311 && skid < 20000) || skid > 30000)
		return SendClientMessage(playerid, COLOR_ERROR, "Inserisci un ID compreso tra 1 e 311. (20000 - 30000 per le custom skin)");
    
	Character_SetSkin(id, skid);
    SetPlayerSkin(id, skid);
    return 1;
}

flags:resetinv(CMD_MODERATOR);
CMD:resetinv(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /resetinv [playerid/partofname]");
	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non � collegato.");
	Inventory_Reset(Character_GetInventory(id));
	Character_SaveInventory(id);
	SendMessageToAdmins(false, COLOR_ADMINCMD, "AdmAlert: %s ha resettato l'inventario di %s.", Account_GetName(playerid), Character_GetOOCName(id));
	SendFormattedMessage(id, COLOR_ERROR, "[ADMIN]: %s ti ha resettato l'inventario.", Account_GetName(playerid));
	return 1;
}

flags:ajail(CMD_MODERATOR);
CMD:ajail(playerid, params[])
{
	new id, time, reason[64];
	if(sscanf(params, "k<u>ds[64]", id, time, reason))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /ajail [playerid/partofname] <minuti> <motivo>");
	if(!Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");
	
	if(time < 1)
		return SendClientMessage(playerid, COLOR_ERROR, "Minuti non validi. (1 - +inf)");

	Character_ResetDeathState(id);
	
	SetCameraBehindPlayer(id);

	ClearAnimations(id);
	Character_OffDuty(id);

	Character_SetJailTime(id, time);
	Character_SetToJailPos(id);

	Character_ResetWeapons(id);

	Character_SetHealth(id, 100.0);
	Character_SetArmour(id, 0.0);

	SendFormattedMessage(id, COLOR_ERROR, "Sei stato jailato da %s (%d) per %d minuti. Motivo: %s", Account_GetName(playerid), playerid, time, reason);

	SendMessageToAdmins(false, COLOR_ADMINCMD, "AdmCmd: %s (%d) ha jailato %s (%d) per %d minuti. Motivo: %s", Account_GetName(playerid), playerid, Character_GetOOCName(id), id, time, reason);
	LogCharacter(id, playerid, LOG_TYPE_JAIL, reason, time);
	Log(Character_GetOOCName(playerid), Character_GetOOCName(id), "/ajail", time);
	return 1;
}

flags:unjail(CMD_MODERATOR);
CMD:unjail(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /unjail [playerid/partofname]");
	if(!Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");
	if(!Character_IsJailed(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non sta scontando una pena.");
	
	Character_SetJailTimeAsSeconds(id, 2);
	SendFormattedMessage(id, COLOR_ERROR, "Sei stato unjailato da %s", Account_GetName(playerid));

	SendMessageToAdmins(false, COLOR_ADMINCMD, "AdmCmd: %s (%d) ha unjailato %s (%d).", Account_GetName(playerid), playerid, Character_GetOOCName(id), id);
	LogCharacter(id, playerid, LOG_TYPE_UNJAIL, "", 0);
	Log(Character_GetOOCName(playerid), Character_GetOOCName(id), "/unjail");
	return 1;
}

flags:banaccount(CMD_MODERATOR);
CMD:banaccount(playerid, params[])
{
	new id, reason[128];
	if(sscanf(params, "k<u>s[128]", id, reason))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /banaccount [playerid/partofname] <motivo>");
	if(!IsPlayerConnected(id) || IsPlayerNPC(id) || !Account_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");
	
	BanAccount(id, playerid, reason);
	return 1;
}

flags:banofflineaccount(CMD_MODERATOR);
CMD:banofflineaccount(playerid, params[])
{
	new name[MAX_PLAYER_NAME];
	if(sscanf(params, "s[24]", name))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /banofflineaccount <nome>");
	inline FindAccount()
	{
		if(cache_num_rows() > 0)
		{
			new id, e_name[MAX_PLAYER_NAME];
			cache_get_value_index_int(0, 0, id);
			cache_get_value_index(0, 1, e_name);
			mysql_pquery_f(gMySQL, "UPDATE `accounts` SET banned = '1' WHERE id = '%d'", id);
			SendFormattedMessage(playerid, COLOR_ORANGE, "L'account \"%s\" � stato bannato.", e_name);
			SendMessageToAdmins(true, COLOR_ORANGE, "AdmAlert: %s ha bannato l'account \"%s\".", Account_GetName(playerid), e_name);
			Log(Account_GetName(playerid), e_name, "/banofflineaccount");
			Discord_SendBanMessage("L'admin **%s** ha _bannato_ l'account **%s** permanentemente.", Account_GetName(playerid), e_name);
		}
		else
		{
			SendFormattedMessage(playerid, COLOR_ERROR, "L'account non esiste.");
		}
	}
	MySQL_TQueryInline(gMySQL, using inline FindAccount, "SELECT id, name FROM `accounts` WHERE LOWER(name) = LOWER('%e')", name);
	return 1;
}

flags:unbanaccount(CMD_MODERATOR);
CMD:unbanaccount(playerid, params[])
{
	new name[MAX_PLAYER_NAME];
	if(sscanf(params, "s[24]", name))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /unbanaccount <nome>");
	inline FindAccount()
	{
		if(cache_num_rows() > 0)
		{
			new id, e_name[MAX_PLAYER_NAME];
			cache_get_value_index_int(0, 0, id);
			cache_get_value_index(0, 1, e_name);
			mysql_pquery_f(gMySQL, "UPDATE `accounts` SET banned = '0', ban_expiry = '0' WHERE id = '%d'", id);
			SendFormattedMessage(playerid, COLOR_ORANGE, "L'account \"%s\" � stato sbannato.", e_name);
			SendMessageToAdmins(true, COLOR_ORANGE, "AdmAlert: %s ha sbannato l'account \"%s\".", Account_GetName(playerid), e_name);
			Log(Account_GetName(playerid), e_name, "/unbanaccount");
			Discord_SendBanMessage("L'admin **%s** ha _sbannato_ l'account **%s**.", Account_GetName(playerid), e_name);
		}
		else
		{
			SendFormattedMessage(playerid, COLOR_ERROR, "L'account non esiste o � gi� sbannato.");
		}
	}
	MySQL_TQueryInline(gMySQL, using inline FindAccount, "SELECT id, name FROM `accounts` WHERE LOWER(name) = LOWER('%e') AND banned = '1'", name);
	return 1;
}

flags:bancharacter(CMD_MODERATOR);
CMD:bancharacter(playerid, params[])
{
	new id, days, reason[128];
	if(sscanf(params, "k<u>ds[128]", id, days, reason))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /bancharacter [playerid/partofname] <giorni> <motivo>");
	if(!IsPlayerConnected(id) || IsPlayerNPC(id) || !Account_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");
	if(days < 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Giorni non validi. Usa 0 per bannare permanentemente.");
	BanCharacter(id, playerid, days, reason);
	return 1;
}

flags:unbancharacter(CMD_MODERATOR);
CMD:unbancharacter(playerid, params[])
{
	new name[MAX_PLAYER_NAME];
	if(sscanf(params, "s[24]", name))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /unbancharacter <nome>");
	inline FindAccount()
	{
		if(cache_num_rows() > 0)
		{
			new id, e_name[MAX_PLAYER_NAME];
			cache_get_value_index_int(0, 0, id);
			cache_get_value_index(0, 1, e_name);
			mysql_pquery_f(gMySQL, "UPDATE `characters` SET banned = '0', ban_expiry = '0' WHERE id = '%d'", id);
			SendFormattedMessage(playerid, COLOR_ORANGE, "Il personaggio \"%s\" � stato sbannato.", e_name);
			SendMessageToAdmins(true, COLOR_ORANGE, "AdmAlert: %s ha sbannato il personaggio \"%s\".", Account_GetName(playerid), e_name);
			Log(Account_GetName(playerid), e_name, "/unban");

			Discord_SendBanMessage("L'admin **%s** ha _sbannato_ il personaggio **%s**.", Account_GetName(playerid), e_name);
		}
		else
		{
			SendFormattedMessage(playerid, COLOR_ERROR, "Il personaggio non esiste.");
		}
	}
	MySQL_TQueryInline(gMySQL, using inline FindAccount, "SELECT id, name FROM `characters` WHERE LOWER(name) = LOWER('%e')", name);
	return 1;
}

stock BanCharacter(playerid, adminid, days, reason[])
{
	new banTime = (days > 0) ? (gettime() + 60 * 60 * 24 * days) : cellmax;
	
	if(days > 0)
	{
		SendFormattedMessage(playerid, COLOR_ORANGE, "Sei stato bannato da %s per %d giorni. Motivo: %s", Account_GetName(adminid), days, reason);
		SendMessageToAdmins(true, COLOR_ADMINCMD, "AdmAlert: %s ha bannato %s per %d giorni. Motivo: %s", Account_GetName(adminid), Character_GetOOCName(playerid), days, reason);
		Discord_SendBanMessage("L'admin **%s** ha _bannato_ il personaggio **%s** appartenente all'account **%s** per **%d* giorni. Motivo: __**%s**.", Account_GetName(adminid), Character_GetOOCName(playerid), Account_GetName(playerid), days, reason);
	}
	else
	{
		SendFormattedMessage(playerid, COLOR_ORANGE, "Sei stato bannato da %s permanentemente. Motivo: %s", Account_GetName(adminid), reason);
		SendMessageToAdmins(true, COLOR_ADMINCMD, "AdmAlert: %s ha bannato %s permanentemente. Motivo: %s", Account_GetName(adminid), Character_GetOOCName(playerid), reason);
		Discord_SendBanMessage("L'admin **%s** ha _bannato_ il personaggio **%s** appartenente all'account **%s** permanentemente. Motivo: __**%s**.", Account_GetName(adminid), Character_GetOOCName(playerid), Account_GetName(playerid), reason);
	}

	Log(Account_GetName(adminid), Account_GetName(playerid), "BanCharacter", days);
	LogCharacter(playerid, adminid, LOG_TYPE_BAN, reason, days);
	mysql_pquery_f(gMySQL, "UPDATE `characters` SET banned = '1', ban_expiry = '%d' WHERE id = '%d'", banTime, Character_GetID(playerid));

	KickEx(playerid);
	return 1;
}

stock BanAccount(playerid, adminid, reason[])
{
	mysql_pquery_f(gMySQL, "UPDATE `accounts` SET banned = '1' WHERE id = '%d'", Account_GetID(playerid));

	SendFormattedMessage(playerid, COLOR_ORANGE, "Il tuo account (%s) � stato bannato da %s. Motivo: %s", Account_GetName(playerid), Account_GetName(adminid), reason);

	SendMessageToAdmins(true, COLOR_ADMINCMD, "AdmAlert: %s ha bannato l'account di %s (%s). Motivo: %s",
	Account_GetName(adminid), Account_GetName(playerid), Character_GetOOCName(playerid), reason);

	Log(Account_GetName(adminid), Account_GetName(playerid), "BanAccount");

	KickEx(playerid);

	Discord_SendBanMessage("L'admin **%s** ha _bannato_ l'account **%s**. Motivo: __**%s**.", Account_GetName(adminid), Account_GetName(playerid), reason);
	return 1;
}

flags:respawnfactionvehicles(CMD_MODERATOR);
CMD:respawnfactionvehicles(playerid, params[])
{
	new factionid;
	if(sscanf(params, "d", factionid))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /respawnfactionvehicles <factionid>");
	
	if(!Faction_IsValid(factionid))
		return SendClientMessage(playerid, COLOR_ERROR, "Fazione non valida.");

	new bool:isOccupied = false;
	foreach(new v : Vehicle)
	{
		isOccupied = false;
		foreach(new p : Player)
		{
			if(GetPlayerVehicleID(p) == v)
			{
				isOccupied = true;
				break;
			}
		}
		
		if(isOccupied)
			continue;
		
		if(Vehicle_GetFaction(v) == factionid)
		{
			Vehicle_Respawn(v);
			Vehicle_SetFuel(v, 100.0);
		}
	}

	new name[MAX_FACTION_NAME];
	Faction_GetName(factionid, name);
	SendMessageToAdmins(0, COLOR_ORANGE, "AdmCmd: %s ha respawnato i veicoli della fazione id %d (%s).", Account_GetName(playerid), factionid, name);
	Faction_SendMessage(factionid, "(( %s (%d) ha respawnato tutti i veicoli non occupati ))", Account_GetName(playerid), playerid);
	return 1;
}

flags:check(CMD_MODERATOR);
CMD:check(playerid, params[])
{
    new id;
    if(sscanf(params, "k<u>", id))
	   return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /check [playerid/partofname]");
    if(id < 0 || id >= MAX_PLAYERS  || !Character_IsLogged(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non � collegato.");
    Character_ShowStats(id, playerid);

	Character_ShowVehicleList(playerid, id, true);

	new ip[16];
	GetPlayerIp(id, ip, sizeof(ip));
	SendFormattedMessage(playerid, COLOR_GREEN, "L'IP del giocatore � %s", ip);
	if(Character_IsJailed(id)) 
		SendFormattedMessage(id, COLOR_ORANGE, "Il giocatore � in jail per %d minuti", Character_GetJailTime(id));
	SendClientMessage(playerid, COLOR_GREEN, "Per l'inventario, digita /invcheck [playerid/partofname]");
	SendClientMessage(playerid, COLOR_GREEN, "Per le case, digita /housecheck [playerid/partofname]");
	SendClientMessage(playerid, COLOR_GREEN, "Per i biz, digita /bizcheck [playerid/partofname]");
    return 1;
}

flags:invcheck(CMD_MODERATOR);
CMD:invcheck(playerid, params[])
{
    new id;
    if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /invcheck [playerid/partofname]");
    if(id < 0 || id >= MAX_PLAYERS  || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non � collegato.");
	new str[64], Inventory:inv = Character_GetInventory(id);
	format(str, sizeof(str), "Inventario di %s (%d/%d)",  Character_GetOOCName(id), Inventory_GetUsedSpace(inv), Inventory_GetSpace(inv));
	
	Inventory_Show(inv, playerid, str, DialogNull, "Chiudi", "Chiudi");
	// Inventory_ShowInChatStr(Character_GetInventory(id), playerid, Character_GetOOCNameStr(id));
	return 1;
}

flags:housecheck(CMD_MODERATOR);
CMD:housecheck(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /housecheck [playerid/partofname]");
	
	if(!Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non � collegato.");

	if(Character_GetOwnedHouseCount(id) <= 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non possiede case.");
	
	SendFormattedMessage(playerid, 0xFF6347FF, "Le case di %s", Character_GetOOCName(id));

	new List:houses = Character_GetOwnedHousesList(id);
	for_list(i : houses)
	{
		SendFormattedMessage(playerid, 0xFF6347FF, "- %s", House_GetAddress(House:iter_get(i)));
	}
	return 1;
}

flags:bizcheck(CMD_MODERATOR);
CMD:bizcheck(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /bizcheck [playerid/partofname]");
	
	if(!Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non � collegato.");

	if(Character_GetOwnedHouseCount(id) <= 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non possiede biz.");
	
	SendFormattedMessage(playerid, 0xFF6347FF, "I Biz di %s", Character_GetOOCName(id));

	new List:buildings = Character_GetOwnedBuildingsList(id);
	for_list(i : buildings)
	{
		SendFormattedMessage(playerid, 0xFF6347FF, "- %s", Building_GetAddress(Building:iter_get(i)));
	}
	return 1;
}

flags:setweather(CMD_MODERATOR);
CMD:setweather(playerid, params[])
{
	if(isnull(params))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /setweather <weather id>");
	new w = strval(params);
	if(0 <= w <= 20)
	{
		Server_SetNextWeather(w);
		Server_UpdateWeather();
		SendMessageToAdmins(false, COLOR_ADMINCMD, "%s (%d) ha cambiato il tempo metereologico. ID: %d.", Account_GetName(playerid), playerid, w);
	}
	else 
		SendClientMessage(playerid, COLOR_ERROR, "ID Errato.");
	return 1;
}

flags:respawnjobvehicles(CMD_MODERATOR);
CMD:respawnjobvehicles(playerid, params[])
{
	new jobType;
	if(sscanf(params, "d", jobType))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /respawnjobvehicles <factionid>");
	
	if(!(JOB_TYPE_NONE < JobType:jobType <= JobType))
		return SendClientMessage(playerid, COLOR_ERROR, "Lavoro non valido.");

	new bool:isOccupied = false;
	foreach(new v : Vehicle)
	{
        if(Vehicle_GetJob(v) == JOB_TYPE_NONE)
            continue;
        
		isOccupied = false;
		foreach(new p : Player)
		{
			if(GetPlayerVehicleID(p) == v)
			{
				isOccupied = true;
				break;
			}
		}
		
		if(isOccupied)
			continue;
		
		if(Vehicle_GetJob(v) == JobType:jobType)
		{
			Vehicle_Respawn(v);
            Vehicle_SetFuel(v, 100.0);
		}
	}

	new name[32];
	Job_GetName(JobType:jobType, name);
	SendMessageToAdmins(0, COLOR_ORANGE, "AdmCmd: %s ha respawnato i veicoli del lavoro id %d (%s).", Account_GetName(playerid), jobType, name);
	return 1;
}

flags:setlevel(CMD_MODERATOR);
CMD:setlevel(playerid, params[])
{
	new id, level;
    if(sscanf(params, "k<u>d", id, level))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /setlevel [playerid/partofname] [level]");
    if(!Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non � collegato.");
	Character_SetLevel(id, level);
	SendMessageToAdmins(false, COLOR_ADMINCMD, "AdmCmd: %s ha settato il livello di %s a %d.", Account_GetName(playerid), Character_GetOOCName(id), level);
	return 1;
}