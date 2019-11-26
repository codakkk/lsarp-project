flags:setadmin(CMD_RCON);
CMD:setadmin(playerid, params[])
{
	new id, lvl, role;
	if(sscanf(params, "k<u>dD(0)", id, lvl, role))
	{
		SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /setadmin [playerid/nome] [level (0 - 5)] [facoltativo: ruolo]");
		SendClientMessage(playerid, COLOR_COMMAND_ERROR, "[RUOLI] 0: No - 1: proprietà - 2: fazioni");
		return 1;
	}

	if(!Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "L'utente non è collegato.");
	if(lvl < 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Livello Admin non valido.");
	if(lvl == 0)
	{
		if(Player_IsAdminDuty(id))
			callcmd::aduty(id, "");
		if(Player_IsSupporterDuty(id))
			callcmd::tduty(id, "");
		SendFormattedMessage(id, COLOR_ERROR, "AdmCmd: %s (%d) ti ha rimosso dallo staff.", Account_GetName(playerid), playerid);
		SendFormattedMessage(playerid, COLOR_ERROR, "Hai rimosso %s (%d) dallo staff.", Account_GetName(id), id);
		SendMessageToAdmins(true, COLOR_ADMINCMD, "AdmAlert: %s (%d) ha rimosso %s (%d) dallo staff.", Account_GetName(playerid), playerid, Account_GetName(id), id);
	}
	else
	{
		//new ln[] = ;
		SendFormattedMessage(id, COLOR_GREEN, "AdmCmd: %s (%d) ti ha settato %s.", Account_GetName(playerid), playerid, GetAdminLevelName(lvl));
		SendFormattedMessage(playerid, COLOR_GREEN, "%s (%d) è stato settato %s.", Account_GetName(id), id, GetAdminLevelName(lvl));
		SendMessageToAdmins(true, COLOR_ADMINCMD, "AdmAlert: %s (%d) ha settato %s (%d) %s.", Account_GetName(playerid), playerid, Account_GetName(id), id, GetAdminLevelName(lvl));
	}
	Log(Account_GetName(playerid), Account_GetName(id), "/setadmin", lvl);
	Account_SetAdminLevel(id, lvl);
	Account_SetAdminRole(id, role);
	return 1;
}

flags:givemoney(CMD_RCON);
CMD:givemoney(playerid, params[])
{
    new id, hp;
    if(sscanf(params, "k<u>d", id, hp))
	   return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /givemoney <playerid/nome> <ammontare>");
    
    if(id < 0 || id >= MAX_PLAYERS || !Character_IsLogged(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "L'utente non è collegato.");
    
    Character_GiveMoney(id, hp);
    // Send admin alert (?)
    SendMessageToAdmins(1, COLOR_YELLOW, "AdmCmd: %s (%s) ha dato $%d a %s (%d).", Account_GetName(playerid), Character_GetOOCName(playerid), hp, Character_GetOOCName(id), id);
    SendFormattedMessage(id, COLOR_GREEN, "L'admin %s (%s) ti ha givato $%d.", Account_GetName(playerid), Character_GetOOCName(playerid), hp);
    Log(Account_GetName(playerid), Character_GetOOCName(id), "/givemoney", hp);
    return 1;
}

flags:givebankmoney(CMD_RCON);
CMD:givebankmoney(playerid, params[])
{
    new id, hp;
    if(sscanf(params, "k<u>d", id, hp))
	   return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /givebankmoney <playerid/nome> <ammontare>");
    
    if(id < 0 || id >= MAX_PLAYERS || !Character_IsLogged(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "L'utente non è collegato.");
    
    Character_GiveBank(id, hp);
    // Send admin alert (?)
    SendMessageToAdmins(1, COLOR_YELLOW, "AdmCmd: %s (%s) ha dato $%d in banca a %s (%d).", Account_GetName(playerid), Character_GetOOCName(playerid), hp, Character_GetOOCName(id), id);
    SendFormattedMessage(id, COLOR_GREEN, "[ADMIN] %s (%s) ti ha givato $%d in banca.", Account_GetName(playerid), Character_GetOOCName(playerid), hp);
    Log(Account_GetName(playerid), Character_GetOOCName(id), "/givemoney", hp);
    return 1;
}

//flags:payday(CMD_RCON);
flags:payday(CMD_RCON);
CMD:payday(playerid, params[])
{
	foreach(new i : Player)
	{
		if(!Character_IsLogged(i))
			continue;
		Character_PayDay(i);
	}
    return 1;
}

flags:setpremium(CMD_RCON);
CMD:setpremium(playerid, params[])
{
	new id, premiumid, time;
	if(sscanf(params, "k<u>dd", id, premiumid, time))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /setpremium [playerid/partofname] <premium (0 - 3)> <giorni>");
	if(!IsPlayerConnected(id) || IsPlayerNPC(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido");
	if(premiumid < 0 || premiumid > 3)
		return SendClientMessage(playerid, COLOR_ERROR, "Premium ID non valido");
	if(time < 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Giorni < 0");
	
	Account_SetPremiumLevel(id, premiumid);
	Account_SetPremiumExpiry(id, time ? (gettime() + 3600 * 24 * time) : 0);

	SendMessageToAdmins(true, COLOR_ADMINCMD, "AdmAlert: %s (%d) ha settato %s (%d) premium livello %d.", 
	Account_GetName(playerid), playerid, Account_GetName(id), id, premiumid);
	return 1;
}

flags:givezpoints(CMD_RCON);
CMD:givezpoints(playerid, params[])
{
	new id, points;
	if(sscanf(params, "k<u>d", id, points))	
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /givezpoints [playerid/partofname] <points>");
	
	Account_AddPoints(id, points);

	SendMessageToAdmins(true, COLOR_ADMINCMD, "AdmCmd: %s (%d) ha dato %d Z-Points a %s (%d).", 
	Account_GetName(playerid), playerid, points, Account_GetName(id), id);

	return 1;
}