
#include <YSI_Coding\y_hooks>

static 
	bool:HelpRequest[MAX_PLAYERS] = {false, ...},
	LastHelpRequestTime[MAX_PLAYERS] = {0, ...};

hook OnCharacterClearData(playerid)
{
	HelpRequest{playerid} = false;
	LastHelpRequestTime[playerid] = 0;
	return 1;
}

flags:tcmds(CMD_TESTER);
CMD:tcmds(playerid, params[])
{
	SendClientMessage(playerid, -1, "[TESTER]: (/t)chat - /tduty - /lr - /goto - /gethere");
	SendClientMessage(playerid, -1, "[TESTER]: /kick - /freeze - /unfreeze - /setinterior - /setvw");
	return 1;
}

flags:tduty(CMD_TESTER);
CMD:tduty(playerid, params[])
{
	if(Player_IsAdminDuty(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se sei in servizio admin.");
	Account_SetSupporterDuty(playerid, !Account_IsSupporterDuty(playerid));
    if(Account_IsSupporterDuty(playerid))
	{
		SetPlayerColor(playerid, 0xa52a2aff);
		SendMessageToTesters(0, 0xe9de98ff, "[TESTERS]: %s è ora in servizio.", Character_GetOOCName(playerid));
	}
	else
	{
		SetPlayerColor(playerid, 0xFFFFFFFF);
		SendMessageToTesters(0, 0xe9de98ff, "[TESTERS]: %s non è più in servizio.", Character_GetOOCName(playerid));
	}
    return 1;
}


flags:tchat(CMD_TESTER);
CMD:tchat(playerid, params[])
{
	if(Player_IsDisabledAdminAlerts(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Hai disabilitato la chat supporter.");
	if(isnull(params) || strlen(params) > 128)
		return SendClientMessage(playerid, COLOR_ERROR, "(/t)chat <testo>");
	
	SendMessageToTesters(0, 0xE9DE98FF, "[TESTER] %s (%s): %s", Account_GetName(playerid), Character_GetOOCName(playerid), params);
	return 1;
}
alias:tchat("t");

flags:kick(CMD_TESTER);
CMD:kick(playerid, params[])
{
	new id, reason[128];
	if(sscanf(params, "k<u>s[128]", id, reason) || strlen(reason) < 0 || strlen(reason) > 128)
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /kick [playerid/partofname] <motivo (max 127 caratteri)>");
	
	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è connesso.");

	if(Account_GetAdminLevel(id) > Account_GetAdminLevel(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su un admin di livello superiore.");
	SendFormattedMessage(id, COLOR_ADMIN, "L'admin %s ti ha kickato. Motivo: %s", Account_GetName(playerid), reason);
	SendMessageToAdmins(false, COLOR_ADMINCMD, "[ADMIN-CMD]: %s ha kickato %s. Motivo: %s", Account_GetName(playerid), Character_GetOOCName(id), reason);
	KickEx(id);

	LogCharacter(id, playerid, LOG_TYPE_KICK, reason, 0);
	Discord_SendKickMessage("L'admin **%s** ha _kickato_ **%s** (**%s**). Motivo: **%s**.", Account_GetName(playerid), Character_GetOOCName(id), Account_GetName(id), reason);
	return 1;
}

flags:gethere(CMD_TESTER);
CMD:gethere(playerid, params[])
{
    new id;
    if(sscanf(params, "k<u>", id))
	   return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /gethere [playerid/partofname]");
    
    if(id < 0 || id >= MAX_PLAYERS  || !Character_IsLogged(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");
    
	if(id == playerid)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su te stesso.");

	if(Player_GetSpectatingPlayer(id) != INVALID_PLAYER_ID)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi gotare un admin in recon.");

	new 
	   Float:x, Float:y, Float:z,
	   vw = Character_GetVirtualWorld(playerid), int = GetPlayerInterior(playerid);
    
    GetPlayerPos(playerid, x, y, z);

    if(GetPlayerState(id) == PLAYER_STATE_DRIVER)
	{
		SetVehiclePos(GetPlayerVehicleID(id),x+3,y,z);
		LinkVehicleToInterior(GetPlayerVehicleID(id), int);
		SetVehicleVirtualWorld(GetPlayerVehicleID(id), vw);
	}
	else
    	Character_SetSafePosition(id, x+2, y, z );
    return 1;
}

flags:freeze(CMD_TESTER);
CMD:freeze(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /freeze [playerid/partofname]");
	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");
	Character_SetFreezed(id, true);
	SendClientMessage(id, COLOR_ERROR, "Sei stato freezato da un admin.");
	SendClientMessage(playerid, COLOR_ERROR, "Hai freezato il giocatore.");
	return 1;
}

flags:unfreeze(CMD_TESTER);
CMD:unfreeze(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /unfreeze [playerid/partofname]");
	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");
	Character_SetFreezed(id, false);
	SendClientMessage(id, COLOR_ERROR, "Sei stato unfreezato da un admin.");
	SendClientMessage(playerid, COLOR_ERROR, "Hai unfreezato il giocatore.");
	return 1;
}

flags:goto(CMD_TESTER);
CMD:goto(playerid, params[])
{
    new id;
    if(sscanf(params, "k<u>", id))
	   return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /goto [playerid/partofname]");
    
    if(id < 0 || id >= MAX_PLAYERS  || !Character_IsLogged(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");

	if(id == playerid)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su te stesso.");

	if(Player_GetSpectatingPlayer(id) != INVALID_PLAYER_ID)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi gotarti da un admin in recon.");

    new 
	   Float:x, Float:y, Float:z,
	   vw = Character_GetVirtualWorld(id), int = GetPlayerInterior(id);


    GetPlayerPos(id, x, y, z);
    Streamer_UpdateEx(playerid, x, y, z, vw, int);
    
	Character_SetVirtualWorld(playerid, vw);
    SetPlayerInterior(playerid, int);

    if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
	{
		SetVehiclePos(GetPlayerVehicleID(playerid),x+3,y,z);
		LinkVehicleToInterior(GetPlayerVehicleID(playerid), int);
		SetVehicleVirtualWorld(GetPlayerVehicleID(playerid), vw);
	}
	else
    	SetPlayerPos(playerid, x+2, y, z );
	
    return 1;
}

flags:jetpack(CMD_TESTER);
CMD:jetpack(playerid, params[])
{
	new id;
	if(sscanf(params, "d", id))
	{
		id = playerid;
	}
	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "L'utente non è collegato.");
	if(GetPlayerSpecialAction(id) == SPECIAL_ACTION_USEJETPACK)
		SetPlayerSpecialAction(id, SPECIAL_ACTION_NONE);
	else
		SetPlayerSpecialAction(id, SPECIAL_ACTION_USEJETPACK);
	return 1;
}

flags:setvw(CMD_TESTER);
CMD:setvw(playerid, params[])
{
    new id, vw;
    if(sscanf(params, "k<u>d", id, vw))
	   return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /setvw <playerid/nome> <virtual world>");
	
	Character_SetVirtualWorld(id, vw);

	SendFormattedMessage(id, COLOR_GREEN, "L'admin %s ti ha settato il virtual world a %d.", Account_GetName(playerid), vw);
	SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato il virtual world di %s al valore %d.", Character_GetOOCName(id), vw);
	return 1;
}

flags:setinterior(CMD_TESTER);
CMD:setinterior(playerid, params[])
{
    new id, int;
    if(sscanf(params, "k<u>d", id, int))
	   return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /setinterior <playerid/nome> <interior>");
	
	SetPlayerInterior(id, int);

	SendFormattedMessage(id, COLOR_GREEN, "L'admin %s ti ha settato l'interior id %d.", Account_GetName(playerid), int);
	SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato interior id %s al valore %d.", Character_GetOOCName(id), int);
	return 1;
}