flags:setadmin(CMD_RCON);
CMD:setadmin(playerid, params[])
{
	new id, lvl;
	if(sscanf(params, "k<u>d", id, lvl))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /setadmin <playerid/nome> <level (0 - 5)>");

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