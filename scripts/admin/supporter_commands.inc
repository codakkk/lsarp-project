
#include <YSI_Coding\y_hooks>

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
	SendClientMessage(playerid, -1, "[TESTER]: (/t)chat - /tduty - /lr - /goto - /gethere");
	SendClientMessage(playerid, -1, "[TESTER]: /kick - /freeze - /unfreeze - /setinterior - /setvw");
	return 1;
}

flags:tduty(CMD_SUPPORTER);
CMD:tduty(playerid, params[])
{
	if(Player_IsAdminDuty(playerid))
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
	if(Player_IsDisabledAdminAlerts(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Hai disabilitato la chat supporter.");
	if(isnull(params) || strlen(params) > 128)
		return SendClientMessage(playerid, COLOR_ERROR, "(/t)chat <testo>");
	
	SendMessageToTesters(0, 0xa52a2aff, "[TESTER-CHAT] %s (%s): %s", Account_GetName(playerid), Character_GetOOCName(playerid), params);
	return 1;
}
alias:tchat("t");