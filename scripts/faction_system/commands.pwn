#include <faction_system\commands\police.pwn>
#include <faction_system\commands\common.pwn>

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
		return SendClientMessage(playerid, COLOR_ERROR, "Hai disabilitato la chat OOC di fazione! Per attivarla usa /togf");
	
	new String:string = str_format("(( [FAZIONE]: %S %s (%d): %s ))", 
		Faction_GetRankNameStr(factionid, Character_GetRank(playerid)-1), 
		Character_GetOOCName(playerid), 
		playerid, params);
	Faction_SendMessageStr(playerid, COLOR_SLATEBLUE, string);
	return 1;
}
alias:fazione("f");


flags:fcreate(CMD_DEVELOPER);
CMD:fcreate(playerid, params[])
{
	new type, name[32], shortName[8];
	if(sscanf(params, "ds[8]s[32]", type, shortName, name))
	{
		SendClientMessage(playerid, COLOR_ERROR, "/fcreate <type> <short name> <name>");
		SendClientMessage(playerid, COLOR_ERROR, "[TIPI]: 1: Forze dell'ordine - 2: Medico - 3: Governo - 4: Illegale Import Armi - 5: Illegale Import Droga");
		return SendClientMessage(playerid, COLOR_ERROR, "[TIPI]: 6: Importo Cibo - 7: Altro");
	}

	if(type < 0 || type >= FACTION_TYPE_LAST)
	{
		SendClientMessage(playerid, COLOR_ERROR, "Tipo di fazione non valido.");
		SendClientMessage(playerid, COLOR_ERROR, "[TIPI]: 0: Forze dell'ordine - 1: Medico - 2: Governo - 3: Illegale Import Armi - 4: Illegale Import Droga");
		return SendClientMessage(playerid, COLOR_ERROR, "[TIPI]: 5: Importo Cibo - 6: Altro");
	}

	if(strlen(name) < 4 || strlen(name) > 32)
		return SendClientMessage(playerid, COLOR_ERROR, "Il nome della fazione non puo' essere più lungo di 32 caratteri e più corto di 4 caratteri.");

	if(strlen(shortName) < 3 || strlen(shortName) > 8)
		return SendClientMessage(playerid, COLOR_ERROR, "L'acronimo della fazione non puo' essere più lungo di 8 caratteri e più corto di 3 caratteri.");

	Faction_Create(name, shortName, type);

	SendMessageToAdmins(true, COLOR_YELLOW, "[ADMIN-ALERT]: %s (%d) ha creato una nuova fazione. (Tipo: %d)", AccountInfo[playerid][aName], playerid, type);
	Log(AccountInfo[playerid][aName], "", "/fcreate");
	return 1;
}

flags:fedit(CMD_DEVELOPER);
CMD:fedit(playerid, params[])
{
	return Faction_ShowList(playerid);
}

flags:afactioncmds(CMD_DEVELOPER);
CMD:afactioncmds(playerid, params[])
{
	SendClientMessage(playerid, COLOR_GREEN, "[FAZIONI]: /fcreate - /fedit - /setfaction - /afactionslist");
	return 1;
}

flags:setfaction(CMD_ADMIN);
CMD:setfaction(playerid, params[])
{
	new id, factionid, rankid;
	if(sscanf(params, "k<u>dd", id, factionid, rankid))
	{
		SendClientMessage(playerid, COLOR_ERROR, "/setfaction <playerid/partofname> <factionid> <rank (1 - 10)>");
		SendClientMessage(playerid, COLOR_ERROR, "Usa /afactionslist per una lista completa.");
		return 1;	
	}
	if(!Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è connesso.");
	if(!Faction_IsValid(factionid))
		return SendClientMessage(playerid, COLOR_ERROR, "Fazione non valida! Usa /afactionslist per una lista completa.");
	if(rankid < 1 || rankid > 10)
		return SendClientMessage(playerid, COLOR_ERROR, "Rank non valido. (1 - 10)");
	Character_SetFaction(id, factionid);
	Character_SetRank(id, rankid);
	SendClientMessageStr(id, COLOR_GREEN, str_format("%s (%d) ti ha settato nella fazione %S.", AccountInfo[playerid][aName], playerid, Faction_GetNameStr(factionid)));
	SendMessageToAdminsStr(0, COLOR_YELLOW, str_format("[ADMIN-ALERT] %s (%d) ha settato %s (%d) nella fazione %S.", AccountInfo[playerid][aName], playerid, Character_GetOOCName(id), id, Faction_GetNameStr(factionid)));
	Character_Save(id);
	return 1;
}

flags:afactionslist(CMD_ADMIN);
CMD:afactionslist(playerid, params[])
{
	return Faction_ShowList(playerid, false);
}