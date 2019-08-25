
#include <YSI_Coding\y_hooks>
// RANKS DOESN'T GET SAVED YET
static enum _:E_RANK_DATA
{
	Name[16],
	Salary
};

static
    Map:FactionRanksMap;

static 
	FactionRanksListItem[MAX_PLAYERS][20],
	SelectedRankId[MAX_PLAYERS char] = {0, ...};

hook OnGameModeInit()
{
    FactionRanksMap = map_new();
	
	mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS faction_ranks ( \
	`faction_id` INT(11) NOT NULL, \
	`rank_id` INT(11) DEFAULT '0', \
	`name` VARCHAR(16) NOT NULL, \
	`salary` INT(11) DEFAULT '0', \
	PRIMARY KEY (`faction_id`, `rank_id`), \
	CONSTRAINT `faction_id_ranks` FOREIGN KEY (`faction_id`) REFERENCES `factions` (`id`) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=latin1;");
    return 1;
}

hook OnFactionCreated(factionid)
{
    map_add(FactionRanksMap, factionid, map_new());
    return 1;
}

hook OnFactionLoaded(factionid)
{
    map_add(FactionRanksMap, factionid, map_new());
	Faction_LoadRanks(factionid);
    return 1;
}

hook OnFactionDeleted(factionid)
{
    map_remove_deep(FactionRanksMap, factionid);
    return 1;
}

flags:afranks(CMD_RCON);
CMD:afranks(playerid, params[])
{
	new factionid, command[32];
	if(sscanf(params, "ds[32]", factionid, command))
		return SendClientMessage(playerid, COLOR_ERROR, "/afranks <factionid> <(a)dd/(e)dit(r)emove/(l)ist>");
	
	if(!Faction_IsValid(factionid))
		return SendClientMessage(playerid, COLOR_ERROR, "La fazione inserita non esiste.");

	if(!strcmp(command, "list", true) || !strcmp(command, "l", true))
	{
		SendFormattedMessage(playerid, COLOR_GREEN, "Ranks per la fazione id %d", factionid);
		new name[MAX_FACTION_RANK_NAME];
		for_map(i : Faction_GetRanksMap(factionid))
		{
			new rankid = iter_get_key(i);
			Faction_GetRankName(factionid, rankid, name);
			SendFormattedMessage(playerid, COLOR_GREEN, "Rank id: %d - Nome: %s - Salario: $%d.", rankid, name, Faction_GetRankSalary(factionid, rankid));
		}
	}
	else if(!strcmp(command, "add", true) || !strcmp(command, "a", true))
	{
		new rankid, salary, name[MAX_FACTION_RANK_NAME];
		if(sscanf(command, "dds[16]", rankid, salary, name) || rankid < 1 || rankid > 10)
			return SendFormattedMessage(playerid, COLOR_ERROR, "/afranks %d add <rankid (1 - 10)> <salario> <nome (%d max caratteri)>", factionid, MAX_FACTION_RANK_NAME);
		if(salary < 0)
			salary = 0;
		if(Faction_AddRank(factionid, rankid, name, salary))
			SendFormattedMessage(playerid, COLOR_GREEN, "Hai aggiunto il rank id %d (\"%s\") con un salario di $%d alla fazione id %d.", rankid, name, salary, factionid);
		else
			SendFormattedMessage(playerid, COLOR_GREEN, "Esiste già un rank con rankid %d.", rankid);
	}
	else if(!strcmp(command, "remove", true) || !strcmp(command, "r", true))
	{
		new rankid;
		if(sscanf(command, "d", rankid) || rankid < 1 || rankid > 10)
			return SendFormattedMessage(playerid, COLOR_ERROR, "/afranks %d remove <rankid (1 - 10)>", factionid);
		if(Faction_RemoveRank(factionid, rankid))
			SendFormattedMessage(playerid, COLOR_GREEN, "Hai rimosso il rank id %d dalla fazione id %d.", rankid, factionid);
		else
			SendFormattedMessage(playerid, COLOR_GREEN, "Non esiste un rank con rankid %d.", rankid);
	}
	else if(!strcmp(command, "edit", true) || !strcmp(command, "e", true))
	{
		new rankid, what[8];
		if(sscanf(command, "ds[8]", rankid, what) || rankid < 1 || rankid > 10)
			return SendFormattedMessage(playerid, COLOR_ERROR, "/afranks %d edit <rankid (1 - 10)> <salary/name>", factionid);
		if(!strcmp(what, "salary", true))
		{
			new salary;
			if(sscanf(what, "d", salary) || salary <= 0)
				return SendFormattedMessage(playerid, COLOR_ERROR, "/afranks %d edit <rankid (1 - 10)> <salary (> 0)> <new-salary>", factionid);
			Faction_SetRankSalary(factionid, rankid, salary);
			SendFormattedMessage(playerid, COLOR_GREEN, "Hai modificato il salario del rank id %d alla fazione %d.", rankid, factionid);
		}
		else if(!strcmp(what, "name", true))
		{
			new name[MAX_FACTION_RANK_NAME];
			if(sscanf(what, "s[16]", name) || strlen(name) <= 0)
				return SendFormattedMessage(playerid, COLOR_ERROR, "/afranks %d edit <rankid (1 - 10)> <name> <new-rank-name>", factionid);
			Faction_SetRankName(factionid, rankid, name);
			SendFormattedMessage(playerid, COLOR_GREEN, "Hai modificato il nome del rank id %d alla fazione %d.", rankid, factionid);
		}
	}
	else SendClientMessage(playerid, COLOR_ERROR, "/afranks <factionid> <(a)dd/(e)dit(r)emove/(l)ist>");
	return 1;
}
stock Faction_ShowRankList(factionid, playerid)
{
	if(!Faction_IsValid(factionid) || Account_GetAdminLevel(playerid) < 5)
		return 0;
	new 
		String:content = @("ID\tNome\tSalario ({85bb65}${FFFFFF})\n"),
		rankid,
		rankName[16],
		salary,
		index = 0;
	for_map(i : Faction_GetRanksMap(factionid))
	{
		rankid = iter_get_key(i);
		salary = Faction_GetRankSalary(factionid, rankid);
		Faction_GetRankName(factionid, rankid, rankName);
		content += str_format("%d#\t%s\t{85bb65}$%d{FFFFFF}\n", rankid, rankName, salary);

		FactionRanksListItem[playerid][index] = rankid;

		index++;
	}
	return Dialog_Show_s(playerid, EditFactionRank, DIALOG_STYLE_TABLIST_HEADERS, @("Ranks"), content, "Modifica", "Indietro");
}

stock Faction_HasRank(factionid, rankid)
{
    new Map:list = Faction_GetRanksMap(factionid);
    if(!map_valid(list) || !map_has_key(list, rankid))
        return 0;
    return 1;
}

stock Faction_AddRank(factionid, rankid, const name[], salary)
{
	new result = Internal_AddRank(factionid, rankid, name, salary);
    if(result)
	{
		mysql_tquery_f(gMySQL, "INSERT INTO `faction_ranks` (faction_id, rank_id, name, salary) VALUES('%d', '%d', '%e', '%d')", factionid, rankid, name, salary);	
	}
    return result;
}

stock Faction_RemoveRank(factionid, rankid)
{
	new result = Internal_RemoveRank(factionid, rankid);
	if(result)
	{
		mysql_tquery_f(gMySQL, "DELETE FROM `faction_ranks` WHERE faction_id = '%d' AND rank_id = '%d'", factionid, rankid);
	}
	return result;
}

// Returns a PawnPlus Map with factionid's ranks list 
stock Map:Faction_GetRanksMap(factionid)
{
    if(!Faction_IsValid(factionid))
        return Map:0;
    return Map:map_get(FactionRanksMap, factionid);
}

stock Faction_GetRankName(factionid, rankid, name[MAX_FACTION_RANK_NAME])
{
	if(!Faction_IsValid(factionid) || !Faction_HasRank(factionid, rankid))
		return 0;
    new Map:map = Faction_GetRanksMap(factionid);
	new Iter:it = map_iter_at(map, rankid);
    iter_get_md_arr(it, {Name}, name);
	return 1;
}

/*stock String:Faction_GetRankNameStr(factionid, rank)
{
	if(rank == -1)
		return @("Indefinito");
	return str_new(FactionRankInfo[factionid][rank][rankName]);
}*/

stock Faction_SetRankName(factionid, rankid, name[])
{
	if(!Faction_IsValid(factionid) || !Faction_HasRank(factionid, rankid))
		return 0;
    
    new Map:map = Faction_GetRanksMap(factionid);
    new Iter:it = map_iter_at(map, rankid);
    iter_set_cells(it, Name, name, MAX_FACTION_RANK_NAME);
	mysql_tquery_f(gMySQL, "UPDATE `faction_ranks` SET name = '%e' WHERE faction_id = '%d' AND rank_id = '%d'", name, factionid, rankid);
    return 1;
}

stock Faction_GetRankSalary(factionid, rankid)
{
	if(!Faction_IsValid(factionid) || !Faction_HasRank(factionid, rankid))
		return 0;
    new Map:list = Faction_GetRanksMap(factionid);
	return map_get(list, rankid, Salary);
}

stock Faction_SetRankSalary(factionid, rankid, salary)
{
	if(!Faction_IsValid(factionid) || !Faction_HasRank(factionid, rankid))
		return 0;
    new Map:list = Faction_GetRanksMap(factionid);
    map_set_cell(list, rankid, Salary, salary);
	mysql_tquery_f(gMySQL, "UPDATE `faction_ranks` SET salary = '%d' WHERE faction_id = '%d' AND rank_id = '%d'", salary, factionid, rankid);
	return 1;
}

stock Faction_LoadRanks(factionid)
{
	inline OnLoad()
	{
		new 
            rows = cache_num_rows(),
            rankName[16],
            rankid,
            salary;
		for(new x = 0; x < rows; ++x)
		{
            cache_get_value_index_int(x, 1, rankid);
			cache_get_value_index(x, 2, rankName);
			cache_get_value_index_int(x, 3, salary);

            Internal_AddRank(factionid, rankid, rankName, salary);
		}
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `faction_ranks` WHERE faction_id = '%d' ORDER BY rank_id", Faction_GetID(factionid));
	return 1;
}

Dialog:EditFactionRank(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Faction_ShowOptions(pAdminSelectedFaction{playerid}, playerid);
	SelectedRankId{playerid} = FactionRanksListItem[playerid][listitem];
	new 
		salary,
		rankName[MAX_FACTION_RANK_NAME]
	;

	Faction_GetRankName(pAdminSelectedFaction{playerid}, SelectedRankId{playerid}, rankName);
	salary = Faction_GetRankSalary(pAdminSelectedFaction{playerid}, SelectedRankId{playerid});

	new String:info = str_format("%d (%s - $%d)", SelectedRankId{playerid}, rankName, salary);
	
	Dialog_Show_s(playerid, EditFactionRankActions, DIALOG_STYLE_LIST, info, @("Modifica Nome\nModifica Salario\n{FF0000}Rimuovi{FFFFFF}"), "Modifica", "Indietro");
	return 1;
}

Dialog:EditFactionRankActions(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Faction_ShowOptions(pAdminSelectedFaction{playerid}, playerid);
	if(listitem == 0)
		Dialog_Show(playerid, EditFactionRankName, DIALOG_STYLE_INPUT, "Nome Rank", "Inserisci il nome del rank (Max 15 caratteri).", "Modifica", "Indietro");
	else if(listitem == 1)
		Dialog_Show(playerid, EditFactionRankSalary, DIALOG_STYLE_INPUT, "Salario Rank", "Inserisci il salario del rank.", "Modifica", "Indietro");
	else if(listitem == 2)
		Dialog_Show(playerid, RemoveFactionRank, DIALOG_STYLE_MSGBOX, "Rimuovi Rank", "Sei sicuro di voler rimuovere questo rank?", "Rimuovi", "Indietro");
	return 1;
}

Dialog:RemoveFactionRank(playerid, response, listitem, inputtext[])
{
	new factionid = pAdminSelectedFaction{playerid};
	if(!response || !Faction_IsValid(factionid))
		return Faction_ShowOptions(factionid, playerid);
	
	if(Faction_RemoveRank(factionid, SelectedRankId{playerid}))
	{
		new name[MAX_FACTION_RANK_NAME];
		Faction_GetRankName(factionid, SelectedRankId{playerid}, name);
		SendFormattedMessage(playerid, COLOR_ERROR, "Hai deletato il rank id %d (Nome: %s) alla fazione id %d.", SelectedRankId{playerid}, name, factionid);

		Log(Account_GetName(playerid), "faction_delete", "/fedit", factionid);
	}
	return 1;
}


Dialog:EditFactionRankName(playerid, response, listitem, inputtext[])
{
	if(!response) return Faction_ShowOptions(pAdminSelectedFaction{playerid}, playerid);
	if(strlen(inputtext) <= 0 || strlen(inputtext) >= MAX_FACTION_RANK_NAME)
		return Dialog_Show(playerid, EditFactionRankName, DIALOG_STYLE_INPUT, "Nome Rank", "{FF0000}Il nome è troppo lungo o troppo corto.{FFFFFF}\nInserisci il nome del rank (Max 15 caratteri).", "Modifica", "Annulla");
	Faction_SetRankName(pAdminSelectedFaction{playerid}, SelectedRankId{playerid}, inputtext);
	SendFormattedMessage(playerid, COLOR_GREEN, "Hai cambiato il nome del rank. Nuovo nome: %s", inputtext);
	return 1;
}

Dialog:EditFactionRankSalary(playerid, response, listitem, inputtext[])
{
	if(!response) return Faction_ShowOptions(pAdminSelectedFaction{playerid}, playerid);
	new salary = strval(inputtext);
	if(salary <= 0)
		return Dialog_Show(playerid, EditFactionRankSalary, DIALOG_STYLE_INPUT, "Salario Rank", "{FF0000}Il salario inserito non è valido!{FFFFFF}\nInserisci il salario del rank.", "Modifica", "Annulla");
	Faction_SetRankSalary(pAdminSelectedFaction{playerid}, SelectedRankId{playerid}, salary);
	SendFormattedMessage(playerid, -1, "Hai cambiato il salario del rank. Nuovo salario: {85bb65}$%d{FFFFFF}", salary);
	return 1;
}

static 
	TempRankCreationState[MAX_PLAYERS char] = {0, ...},
	TempRankName[MAX_PLAYERS][MAX_FACTION_RANK_NAME],
	TempRankSalary[MAX_PLAYERS]
;

stock Faction_StartCreateRank(playerid)
{
	TempRankCreationState{playerid} = 0;
	return Dialog_Show(playerid, Dialog_CreateFactionRank, DIALOG_STYLE_INPUT, "Faction Rank Name", "Inserisci il nome del nuovo rank", "Continua", "Annulla");
}

Dialog:Dialog_CreateFactionRank(playerid, response, listitem, inputtext[])
{
	if(!response)
	{
		TempRankCreationState{playerid}--;
		if(TempRankCreationState{playerid} < 0)
			return Faction_ShowOptions(pAdminSelectedFaction{playerid}, playerid);
		if(TempRankCreationState{playerid} == 0)
			return Faction_StartCreateRank(playerid);
		if(TempRankCreationState{playerid} == 1)
			return Dialog_Show(playerid, Dialog_CreateFactionRank, DIALOG_STYLE_INPUT, "Faction Rank Salary", "Inserisci il salario del rank.", "Continua", "Annulla");
		if(TempRankCreationState{playerid} == 2)
			return Dialog_Show(playerid, Dialog_CreateFactionRank, DIALOG_STYLE_INPUT, "Faction Rank Id", "Inserisci il rank id.\nRank ID: da 1 a 10", "Crea", "Annulla");
		return 1;
	}
	if(TempRankCreationState{playerid} == 0)
	{
		if(!response)
			return Faction_ShowOptions(pAdminSelectedFaction{playerid}, playerid);
		if(strlen(inputtext) <= 0 || strlen(inputtext) >= MAX_FACTION_RANK_NAME)
			return Dialog_Show(playerid, Dialog_CreateFactionRank, DIALOG_STYLE_INPUT, "Faction Rank Name", "{FF0000}Il nome inserito è troppo lungo o troppo corto.{FFFFFF}\nInserisci il nome del nuovo rank", "Continua", "Annulla");
		set(TempRankName[playerid], inputtext);
		TempRankCreationState{playerid} = 1;
		Dialog_Show(playerid, Dialog_CreateFactionRank, DIALOG_STYLE_INPUT, "Faction Rank Salary", "Inserisci il salario del rank.", "Continua", "Annulla");
	}
	else if(TempRankCreationState{playerid} == 1)
	{
		TempRankSalary[playerid] = strval(inputtext);
		if(TempRankSalary[playerid] <= 0)
			return Dialog_Show(playerid, Dialog_CreateFactionRank, DIALOG_STYLE_INPUT, "Faction Rank Salary", "{FF0000}Il salario inserito è troppo basso.{FFFFFF}\nInserisci il salario del rank.", "Continua", "Annulla");
		TempRankCreationState{playerid} = 2;
		return Dialog_Show(playerid, Dialog_CreateFactionRank, DIALOG_STYLE_INPUT, "Faction Rank Id", "Inserisci il rank id.\nRank ID: da 1 a 10", "Crea", "Annulla");
	}
	else if(TempRankCreationState{playerid} == 2)
	{
		new rankid = strval(inputtext);
		if(rankid < 1 || rankid > 10)
			return Dialog_Show(playerid, Dialog_CreateFactionRank, DIALOG_STYLE_INPUT, "Faction Rank Id", "{FF0000}Il rank id inserito non è valido.{FFFFFF}\nInserisci il rank id.\nRank ID: da 1 a 10.", "Crea", "Annulla");
		new result = Faction_AddRank(pAdminSelectedFaction{playerid}, rankid, TempRankName[playerid], TempRankSalary[playerid]);
		if(result == 0)
			return Dialog_Show(playerid, Dialog_CreateFactionRank, DIALOG_STYLE_INPUT, "Faction Rank Id", "{FF0000}Il rank id inserito è già in uso.{FFFFFF}\nInserisci il rank id.\nRank ID: da 1 a 10.", "Crea", "Annulla");
		SendFormattedMessage(playerid, COLOR_GREEN, "Hai aggiunto il rank id %d (Nome: %s) con un salario di $%d alla fazione id %d.", rankid, TempRankName[playerid], TempRankSalary[playerid], pAdminSelectedFaction{playerid});
		Faction_ShowOptions(pAdminSelectedFaction{playerid}, playerid);
	}
	return 1;
}

stock Faction_GetLastValidRank(factionid)
{
	new valid = 0;
	for_map(i : Faction_GetRanksMap(factionid))
	{
		new rankid = iter_get_key(i);
		if(rankid > valid)
			valid = rankid;
	}
	return valid;
}

stock Faction_GetFirstValidRank(factionid)
{
	new valid = Faction_GetLastValidRank(factionid);
	for_map(i : Faction_GetRanksMap(factionid))
	{
		new rankid = iter_get_key(i);
		if(rankid < valid)
			valid = rankid;
	}
	return valid;
}

// 			INTERNALS

static Internal_AddRank(factionid, rankid, const name[], salary)
{
	new Map:map = Faction_GetRanksMap(factionid);
    if(!map_valid(map) || map_has_key(map, rankid))
        return 0;
    new data[E_RANK_DATA];
	set(data[Name], name);
	data[Salary] = salary;
    map_add_arr(map, rankid, data);
	return 1;
}

static Internal_RemoveRank(factionid, rankid)
{
	new Map:map = Faction_GetRanksMap(factionid);
	if(!map_valid(map) || !map_has_key(map, rankid))
		return 0;
	map_remove_deep(map, rankid);
	return 1;
}