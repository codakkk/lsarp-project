
flags:tcmds(CMD_SUPPORTER);
CMD:tcmds(playerid, params[])
{
	SendClientMessage(playerid, -1, "[TESTER]: /tchat - /tduty");
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
	SendMessageToAdmins(0, COLOR_YELLOW, "[TESTER-CHAT] %s (%s): %s", AccountInfo[playerid][aName], Character_GetOOCName(playerid), params);
	return 1;
}

flags:testers(CMD_USER);
CMD:testers(playerid, params[])
{
	foreach(new i : Player)
	{
		if(Account_GetAdminLevel(i) == 1 && Account_IsSupporterDuty(i))
			SendFormattedMessage(playerid, 0xa52a2aff, "(%s) %s [ID: %d]", AccountInfo[i][aName], Character_GetOOCName(i), i);
	}	
	return 1;
}

flags:chiediaiuto(CMD_USER);
CMD:chiediaiuto(playerid, params[])
{
	if(isnull(params) || strlen(params) > 128)
		return SendClientMessage(playerid, COLOR_ERROR, "/chiediaiuto <testo>");
	
	//SendMessageToTesters(1, color, str[], Float, _:...})
	return 1;
}