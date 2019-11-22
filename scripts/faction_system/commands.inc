
flags:dipartimento(CMD_ALIVE_USER);
CMD:dipartimento(playerid, params[])
{
	if(isnull(params) || strlen(params) > 200)
		return SendClientMessage(playerid, COLOR_ERROR, "(/d)ipartimento <messaggio>");

	new factionid = Character_GetFaction(playerid), shortName[MAX_FACTION_SHORT_NAME], rankName[MAX_FACTION_RANK_NAME];
	
	if(Faction_GetType(factionid) != FACTION_TYPE_POLICE && Faction_GetType(factionid) != FACTION_TYPE_MEDICAL && Faction_GetType(factionid) != FACTION_TYPE_FIREFIGHTERS)
		return 0;
	
	if(!Character_IsFactionDuty(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Devi essere in servizio per poter utilizzare questo comando.");

	Faction_GetShortName(factionid, shortName);
	Faction_GetRankName(factionid, Character_GetRank(playerid), rankName);

	foreach(new i : Player)
	{
		factionid = Character_GetFaction(i);
		if(Faction_GetType(factionid) == FACTION_TYPE_POLICE || Faction_GetType(factionid) == FACTION_TYPE_MEDICAL || Faction_GetType(factionid) == FACTION_TYPE_FIREFIGHTERS)
		{
			SendFormattedMessage(i, 0xF07A7AFF, "[%s] %s %s dice: %s", shortName, rankName, Character_GetOOCName(playerid), params);
		}
	}
	ProxDetectorStr(playerid, 15.0, str_format("[Radio] %s dice: %s", Character_GetRolePlayName(playerid), params), COLOR_FADE1, COLOR_FADE2, COLOR_FADE3, COLOR_FADE4, COLOR_FADE5, false, false);
	return 1;
}
alias:dipartimento("d");

flags:fazione(CMD_ALIVE_USER);
CMD:fazione(playerid, params[])
{
	if(strlen(params) <= 0)
		return SendClientMessage(playerid, COLOR_ERROR, "(/f)azione <messaggio>");
	
	new factionid = Character_GetFaction(playerid);
	if(factionid == INVALID_FACTION_ID)
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei membro di una fazione.");
	if(Faction_GetType(factionid) == FACTION_TYPE_OTHERS)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando in questa fazione.");
	if(!Character_IsFactionOOCEnabled(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Hai disabilitato la chat OOC di fazione. Per attivarla usa /tog (f)chat");
	
	new rankName[16];
	Faction_GetRankName(factionid, Character_GetRank(playerid), rankName);
	new String:string = str_format("(( [FAZIONE]: %s %s (%d): %s ))", 
		rankName, Character_GetOOCName(playerid), playerid, params);
	Faction_SendMessageStr(factionid, string);
	return 1;
}
alias:fazione("f");


flags:fcreate(CMD_DEVELOPER);
CMD:fcreate(playerid, params[])
{
	new type, name[MAX_FACTION_NAME], shortName[MAX_FACTION_SHORT_NAME];
	if(sscanf(params, "ds[8]s[32]", type, shortName, name))
	{
		SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /fcreate <type> <short name> <name>");
		SendClientMessage(playerid, COLOR_ERROR, "[TIPI]: 0: Forze dell'ordine - 1: Medico - 2: Governo - 3: Illegale Import Armi - 4: Illegale Import Droga");
		return SendClientMessage(playerid, COLOR_ERROR, "[TIPI]: 4: Importo Cibo - 6: Altro");
	}

	if(type < 0 || type >= FACTION_TYPE_LAST)
	{
		SendClientMessage(playerid, COLOR_ERROR, "Tipo di fazione non valido.");
		SendClientMessage(playerid, COLOR_ERROR, "[TIPI]: 0: Forze dell'ordine - 1: Medico - 2: Governo - 3: Illegale Import Armi - 4: Illegale Import Droga");
		return SendClientMessage(playerid, COLOR_ERROR, "[TIPI]: 5: Importo Cibo - 6: Altro");
	}

	if(strlen(name) < 4 || strlen(name) > MAX_FACTION_NAME)
		return SendFormattedMessage(playerid, COLOR_ERROR, "Il nome della fazione non puo' essere più lungo di %d caratteri e più corto di 4 caratteri.", MAX_FACTION_NAME);

	if(strlen(shortName) < 3 || strlen(shortName) > MAX_FACTION_SHORT_NAME)
		return SendFormattedMessage(playerid, COLOR_ERROR, "L'acronimo della fazione non puo' essere più lungo di %d caratteri e più corto di 3 caratteri.", MAX_FACTION_SHORT_NAME);

	Faction_Create(name, shortName, type);

	SendMessageToAdmins(true, COLOR_YELLOW, "AdmAlert: %s (%d) ha creato una nuova fazione. (Tipo: %d)", Account_GetName(playerid), playerid, type);
	Log(Account_GetName(playerid), "", "/fcreate");
	return 1;
}

flags:fedit(CMD_DEVELOPER);
CMD:fedit(playerid, params[])
{
	new factionid, what[16];
	if(sscanf(params, "ds[16]", factionid, what))
	{
		SendClientMessage(playerid, COLOR_ERROR, "Ricorda che puoi utilizzare anche: /fedit <factionid> <spawn>");
		return Faction_ShowList(playerid);
	}
	if(!Faction_IsValid(factionid))
		return SendClientMessage(playerid, COLOR_ERROR, "La fazione inserita non è valida o non è inizializzata.");
	if(!strcmp(what, "spawn", true))
	{
		new name[MAX_FACTION_NAME];
		Faction_GetName(factionid, name);
		SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato le coordinate di spawn per la fazione %s qui.", name);
		SendMessageToAdmins(false, COLOR_ORANGE, "L'admin %s ha spostato le coordinate di spawn per la fazione %s (ID: %d).", Account_GetName(playerid), name, factionid);
		Log(Account_GetName(playerid), "", "/fedit <spawn>", factionid);
		new Float:x, Float:y, Float:z;
		GetPlayerPos(playerid, x, y, z);

		Faction_SetSpawnPos(factionid, x, y, z, GetPlayerInterior(playerid), Character_GetVirtualWorld(playerid));
	}
	else SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /fedit <spawn>");
	return 1;
}

flags:afactioncmds(CMD_DEVELOPER);
CMD:afactioncmds(playerid, params[])
{
	SendClientMessage(playerid, COLOR_GREEN, "[FAZIONI]: /fcreate - /fedit - /afactionskin - /afactionslist - /respawnfactionvehicles");
	return 1;
}

flags:afactionslist(CMD_DEVELOPER);
CMD:afactionslist(playerid, params[])
{
	return Faction_ShowList(playerid, false);
}