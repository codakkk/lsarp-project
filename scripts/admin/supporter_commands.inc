
static 
	bool:HelpRequest[MAX_PLAYERS] = {false, ...},
	LastHelpRequestTime[MAX_PLAYERS] = {0, ...};

hook OnPlayerClearData(playerid)
{
	HelpRequest{playerid} = false;
	LastHelpRequestTime[playerid] = 0;
	return 1;
}

flags:tcmds(CMD_SUPPORTER);
CMD:tcmds(playerid, params[])
{
	SendClientMessage(playerid, -1, "[TESTER]: /tchat - /tduty - /taiuta");
	return 1;
}

flags:tduty(CMD_SUPPORTER);
CMD:tduty(playerid, params[])
{
	if(Account_IsAdminDuty(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se sei in servizio admin.");
	Account_SetSupporterDuty(playerid, !Account_IsSupporterDuty(playerid));
    if(Account_IsSupporterDuty(playerid))
	{
		SetPlayerColor(playerid, 0xa52a2aff);
		SendMessageToTesters(0, 0xa52a2aff, "[TESTERS]: %s è ora in servizio.", Character_GetOOCName(playerid));
	}
	else
	{
		SetPlayerColor(playerid, 0xFFFFFFFF);
		SendMessageToTesters(0, 0xa52a2aff, "[TESTERS]: %s non è più in servizio.", Character_GetOOCName(playerid));
	}
    return 1;
}


flags:tchat(CMD_SUPPORTER);
CMD:tchat(playerid, params[])
{
	if(pDisableAdminAlerts[playerid])
		return SendClientMessage(playerid, COLOR_ERROR, "Hai disabilitato la chat supporter.");
	if(isnull(params) || strlen(params) > 128)
		return SendClientMessage(playerid, COLOR_ERROR, "/tchat <testo>");
	
	SendMessageToTesters(0, 0xa52a2aff, "[TESTER-CHAT] %s (%s): %s", Account_GetName(playerid), Character_GetOOCName(playerid), params);
	return 1;
}

flags:chiediaiuto(CMD_USER);
CMD:chiediaiuto(playerid, params[])
{
	if(isnull(params) || strlen(params) > 128)
		return SendClientMessage(playerid, COLOR_ERROR, "/chiediaiuto <testo>");
	
	new seconds = (Account_GetPremiumLevel(playerid) > 0) ? 10 : 30;
    if(GetTickCount() - LastHelpRequestTime[playerid] < 1000 * seconds)
	   return SendFormattedMessage(playerid, COLOR_ERROR, "Puoi inviare una domanda ogni %d secondi.", seconds);

	SendMessageToTesters(1, COLOR_GREEN, "[RICHIESTA]: %s (%d) ha chiesto: %s", Character_GetOOCName(playerid), playerid, params);
	SendMessageToTesters(1, COLOR_GREEN, "[RICHIESTA]: digita \"/taiuta %d\" per aiutarlo.", playerid);
	
	LastHelpRequestTime[playerid] = GetTickCount();
	HelpRequest{playerid} = true;

	SendFormattedMessage(playerid, COLOR_GREEN, "Richiesta d'aiuto inviata: %s", params);
	return 1;
}

flags:taiuta(CMD_SUPPORTER);
CMD:taiuta(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_ERROR, "/taiuta <playerid/partofname>");
	
	if(!HelpRequest{id})
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non ha inviato una richiesta d'aiuto.");
	
	SendMessageToTesters(0, COLOR_GREEN, "%s (%d) ha accettato la richiesta d'aiuto di %s.", Character_GetOOCName(id));

	new Float:x, Float:y, Float:z;
	GetPlayerPos(id, x, y, z);
	SetPlayerPos(playerid, x, y, z);

	SetPlayerInterior(playerid, GetPlayerInterior(id));
	Character_SetVirtualWorld(playerid, Character_GetVirtualWorld(id));
	
	LastHelpRequestTime[id] = 0;
	HelpRequest{id} = false;
	return 1;
}