flags:aduty(CMD_JR_MODERATOR);
CMD:aduty(playerid, params[])
{
	static LastColor[MAX_PLAYERS] = {-1, ...};
	if(Account_IsSupporterDuty(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se sei in servizio supporter.");

	if(Player_IsAdminDuty(playerid))
	{
		Player_SetAdminDuty(playerid, false);
		SetPlayerName(playerid, Character_GetOOCName(playerid));
		
		Character_SetHealth(playerid, 100);
		Character_SetArmour(playerid, 0);
		
		SendMessageToAdmins(0, 0xFF6347FF, "AdmCmd: %s (%d) non è più in servizio.", Account_GetName(playerid), playerid);
		
		SetPlayerColor(playerid, LastColor[playerid]);
		LastColor[playerid] = 0xFFFFFFFF;

		Character_ResetDeathState(playerid);
		Character_EnableSomeAntiCheats(playerid, 1);
	}    
	else
	{
		if(Character_IsMasked(playerid))
			callcmd::sconosciuto(playerid, "");
		
		Character_SetMasked(playerid, false);
		Character_SetHealth(playerid, 100);
		Character_SetArmour(playerid, 100);

		Player_SetAdminDuty(playerid, true);
		SetPlayerName(playerid, Account_GetName(playerid));
		
		//callcmd::jetpack(playerid, NULL);
		SendMessageToAdmins(0, 0xFF6347FF, "AdmCmd: %s (%d) è in servizio.", Account_GetName(playerid), playerid);
		
		LastColor[playerid] = GetPlayerColor(playerid);
		
		SetPlayerColor(playerid, 0xFF6347FF);

		Character_ResetDeathState(playerid);
		Character_EnableSomeAntiCheats(playerid, 0);
	}
	foreach(new i : Player)
	{
		if(!Character_IsLogged(i))
			continue;
		if(Character_IsMasked(i))
		{
			if(Player_IsAdminDuty(playerid))
			{
				SetPlayerMarkerForPlayer(playerid, i, 0xFFFF00AA);
				ShowPlayerNameTagForPlayer(playerid, i, true);
			}
			else
				ShowPlayerNameTagForPlayer(playerid, i, false);
		}
		else ShowPlayerNameTagForPlayer(playerid, i, true);
	}
    return 1;
}

flags:gotopos(CMD_JR_MODERATOR);
CMD:gotopos(playerid, params[])
{
    new Float:x, Float:y, Float:z, int, vw;
    if(sscanf(params, "fffdd", x, y, z, int, vw))
	   return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /gotopos <x> <y> <z> <int> <vw>");
    
    Character_SetVirtualWorld(playerid, vw);
    
	Character_SetSafePosition(playerid, x, y, z, int, vw);
    return 1;
}

flags:randomweather(CMD_JR_MODERATOR);
CMD:randomweather(playerid, params[])
{
	Server_SetCurrentWeather(Server_GetNextWeather());
	Server_UpdateWeather();
	SendMessageToAdmins(0, COLOR_ADMIN, "AdmCmd: %s ha forzato un weather random.", Account_GetName(playerid));
	return 1;
}

flags:settimeadd(CMD_JR_MODERATOR);
CMD:settimeadd(playerid, params[])
{
	new w = strval(params);
	if(isnull(params) || w < 0 || w > 24)
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /settimeadd <hour (0-24)>");
	SendMessageToAdmins(true, COLOR_ADMINCMD, "%s ha utilizzato il comando /settimeadd. Ore aggiunte: %d", Account_GetName(playerid), w);
	Server_SetTimeAdd(w);
	return 1;
}

flags:an(CMD_JR_MODERATOR);
CMD:an(playerid, params[])
{
	if(isnull(params) || strlen(params) > 250)
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /an <testo>");
	SendMessageToAdmins(1, 0x6A5ACDFF, "[ANNUNCIO STAFF] %s: %s", Account_GetName(playerid), params);
	return 1;
}

flags:aslap(CMD_JR_MODERATOR);
CMD:aslap(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /aslap [playerid/partofname]");
	
	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è connesso.");

	if(Account_GetAdminLevel(id) > Account_GetAdminLevel(playerid) && id != playerid)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su un admin di livello superiore.");
	
	SendFormattedMessage(id, COLOR_ADMIN, "L'admin %s ti ha slappato.", Account_GetName(playerid));
	SendMessageToAdmins(false, COLOR_ADMINCMD, "[ADMIN-CMD]: %s ha slappato %s.", Account_GetName(playerid), Character_GetOOCName(id));

	new Float:x, Float:y, Float:z;
	GetPlayerPos(id, x, y, z);
	SetPlayerPos(id, x, y, z + 4);
	return 1;
}

flags:staff(CMD_JR_MODERATOR);
CMD:staff(playerid, params[])
{
	foreach(new i : Player)
	{
		if(Account_GetAdminLevel(i) > 0)
		{
			new level = Account_GetAdminLevel(i);
			if(level > 5)
				level = 5;
			SendFormattedMessage(playerid, 0xa52a2aff, "(%s) %s [ID: %d] - %s", Account_GetName(i), Character_GetOOCName(i), i, GetAdminLevelName(level));
		}
	}	
	return 1;
}

flags:a(CMD_JR_MODERATOR);
CMD:a(playerid, params[])
{
	if(Player_IsDisabledAdminAlerts(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Hai disabilitato la chat mod.");
	if(isnull(params) || strlen(params) > 128)
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /a <testo>");
	SendMessageToAdmins(0, 0xE1A400FF, "ModChat: %s (%s): %s", Account_GetName(playerid), Character_GetOOCName(playerid), params);
	return 1;
}

flags:disablealerts(CMD_JR_MODERATOR);
CMD:disablealerts(playerid, params[])
{
	Player_SetDisabledAdminAlerts(playerid, !Player_IsDisabledAdminAlerts(playerid));
	if(Player_IsDisabledAdminAlerts(playerid))
		SendClientMessage(playerid, COLOR_YELLOW, "Hai disabilitato i messaggi admin.");
	else
		SendClientMessage(playerid, COLOR_YELLOW, "Hai abilitato i messaggi admin.");
	return 1;
}