
flags:dipartimento(CMD_ALIVE_USER);
CMD:dipartimento(playerid, params[])
{
	if(isnull(params) || strlen(params) > 200)
		return SendClientMessage(playerid, COLOR_ERROR, "(/d)ipartimento <messaggio>");

	new factionid = Character_GetFaction(playerid), shortName[MAX_FACTION_SHORT_NAME], rankName[MAX_FACTION_RANK_NAME];
	
	if(Faction_GetType(factionid) != FACTION_TYPE_POLICE && Faction_GetType(factionid) != FACTION_TYPE_MEDICAL)
		return 0;
	
	Faction_GetShortName(factionid, shortName);
	Faction_GetRankName(factionid, Character_GetRank(playerid), rankName);

	foreach(new i : Player)
	{
		factionid = Character_GetFaction(i);
		if(Faction_GetType(factionid) == FACTION_TYPE_POLICE || Faction_GetType(factionid) == FACTION_TYPE_MEDICAL)
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
	SendClientMessage(playerid, COLOR_GREEN, "[FAZIONI]: /fcreate - /fedit - /afactionskin - /afactionslist");
	return 1;
}

flags:setfaction(CMD_MODERATOR);
CMD:setfaction(playerid, params[])
{
	new id, factionid, rankid;
	if(sscanf(params, "k<u>dd", id, factionid, rankid))
	{
		SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /setfaction <playerid/partofname> <factionid> <rank (1 - 10)>");
		SendClientMessage(playerid, COLOR_ERROR, "Usa /afactionslist per una lista completa.");
		return 1;	
	}
	if(!Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è connesso.");

	if(!Faction_IsValid(factionid))
		return SendClientMessage(playerid, COLOR_ERROR, "Fazione non valida. Usa /afactionslist per una lista completa.");

	if(!Faction_HasRank(factionid, rankid))
		return SendClientMessage(playerid, COLOR_ERROR, "Rank non valido.");
	
	Character_OffDuty(id);

	Character_SetFaction(id, factionid);
	Character_SetRank(id, rankid);

	new name[MAX_FACTION_NAME], rankName[MAX_FACTION_RANK_NAME];
	Faction_GetName(factionid, name);
	Faction_GetRankName(factionid, rankid, rankName);

	SendFormattedMessage(id, COLOR_GREEN, "%s (%d) ti ha settato nella fazione %s. Rank: %s", Account_GetName(playerid), playerid, name, rankName);
	SendMessageToAdmins(0, COLOR_ADMINCMD, "AdmCmd: %s (%d) ha settato %s (%d) nella fazione %s. Rank: %s", Account_GetName(playerid), playerid, Character_GetOOCName(id), id, name, rankName);
	return 1;
}

flags:afactionkick(CMD_MODERATOR);
CMD:afactionkick(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
	{
		SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /afactionkick <playerid/partofname>");
		return 1;	
	}
	if(!Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è connesso.");
	if(!Faction_IsValid(Character_GetFaction(playerid)))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è in una fazione.");
	
	Character_OffDuty(id);

	Character_SetFaction(id, INVALID_FACTION_ID);
	Character_SetRank(id, 0);
	SendClientMessageStr(id, COLOR_GREEN, str_format("%s (%d) ti ha kickato dalla tua fazione.", Account_GetName(playerid), playerid));
	SendMessageToAdminsStr(0, COLOR_YELLOW, str_format("AdmCmd: %s (%d) ha kickato %s (%d) dalla sua fazione.", Account_GetName(playerid), playerid, Character_GetOOCName(id), id));
	return 1;
}

flags:afactionslist(CMD_MODERATOR);
CMD:afactionslist(playerid, params[])
{
	return Faction_ShowList(playerid, false);
}