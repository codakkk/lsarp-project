

#include <faction_system\skins>
#include <faction_system\ranks>

#include <faction_system\national_guard\core>

#include <faction_system\commands>
#include <faction_system\commands\police>
#include <faction_system\commands\common>



#include <YSI_Coding\y_hooks>

forward OnFactionLoaded(factionid);
forward OnFactionSaved(factionid);
forward OnFactionCreated(factionid);
forward OnFactionDeleted(factionid);

static enum _:E_FACTION_DATA
{
	ID,
	Created,
	Name[MAX_FACTION_NAME],
	ShortName[MAX_FACTION_SHORT_NAME],
	Type,
	Float:SpawnX,
	Float:SpawnY,
	Float:SpawnZ,
	Interior,
	World,
};

static
	Map:Factions;

static FactionListItem[MAX_PLAYERS][20];

hook OnGameModeInit()
{
	Factions = map_new();
	mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS factions \
						(`id` INT(11) NOT NULL AUTO_INCREMENT, \
						`name` VARCHAR(32) NULL, \
						`short_name` VARCHAR(8) NULL, \
						`type` INT(11) DEFAULT '0', \
						`spawn_x` FLOAT DEFAULT '0.0', \
						`spawn_y` FLOAT DEFAULT '0.0', \
						`spawn_z` FLOAT DEFAULT '0.0', \
						`spawn_interior` INT(11) DEFAULT '0', \
						`spawn_world` INT(11) DEFAULT '0', \
						PRIMARY KEY (`id`)) ENGINE=InnoDB DEFAULT CHARSET=latin1;", false);
	Faction_LoadAll();
	return 1;
}

hook OnPlayerClearData(playerid)
{
	memset(FactionListItem[playerid], 0);
	return 1;
}

stock Faction_Create(name[MAX_FACTION_NAME], shortName[MAX_FACTION_SHORT_NAME], type)
{
	new 
		Cache:result = mysql_query_f(gMySQL, true, "INSERT INTO `factions` (name, short_name, type) VALUES('%e', '%e', '%d')", name, shortName, type),
		id = cache_insert_id(), 
		data[E_FACTION_DATA]
	;

	data[ID] = id;
	format(data[Name], MAX_FACTION_NAME, "%s", name); // set(data[fName], name);
	format(data[ShortName], MAX_FACTION_SHORT_NAME, "%s", shortName); // set(data[fShortName], shortName);
	data[Type] = type;
	data[SpawnX] = data[SpawnY] = data[SpawnZ] = 0.0;
	data[Interior] = 0;
	data[World] = 0;
	
	if(!map_add_arr(Factions, id, data))
		return 0;
	
	CallLocalFunction(#OnFactionCreated, "d", id);

	cache_delete(result);
	return 1;
}

stock Faction_Delete(factionid)
{
	if(!Faction_IsValid(factionid))
		return 0;

	mysql_tquery_f(gMySQL, "UPDATE `characters` SET faction = '%d', faction_rank = '0' WHERE faction = '%d'", INVALID_FACTION_ID, factionid);
	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET faction = '%d' WHERE faction = '%d'", INVALID_FACTION_ID, factionid);
	
	for_map(b : GetBuildingsMap())
	{
		new key = iter_get_key(b);
		if(Building_GetFaction(key) == factionid)
			Building_SetFaction(key, -1);
	}

	foreach(new i : Player)
	{
		if(!Character_IsLogged(i))
			continue;
		if(Character_GetFaction(i) == factionid)
		{
			Character_SetFaction(i, -1);
			Character_SetRank(i, 0);
			SendClientMessage(i, COLOR_GREEN, "La fazione di cui facevi parte non esiste più. Sei stato rimosso.");
		}
	}
	
	mysql_tquery_f(gMySQL, "DELETE FROM `factions` WHERE id = '%d'", factionid);

	CallLocalFunction(#OnFactionDeleted, "d", factionid);

	map_remove_deep(Factions, factionid);
	return 1;
}

stock Faction_ShowList(playerid, edit = true)
{
	new String:content = @("ID\tNome\tAcronimo\n"), name[MAX_FACTION_NAME], shortName[MAX_FACTION_SHORT_NAME], count;
	for_map(i : Factions)
	{
		new factionid = iter_get_key(i);
		printf("Faction id: %d", factionid);
		Faction_GetName(factionid, name);
		Faction_GetShortName(factionid, shortName);

		FactionListItem[playerid][count] = factionid;

		content += str_format("#%d\t%s\t%s\n", factionid, name, shortName);
		
		count++;
	}

	if(edit)
		Dialog_Show_s(playerid, Dialog_FactionList, DIALOG_STYLE_TABLIST_HEADERS, @("Lista Fazioni (Edit Mode)"), content, "Seleziona", "Annulla");
	else
		Dialog_Show_s(playerid, DialogNull, DIALOG_STYLE_TABLIST_HEADERS, @("Lista Fazioni"), content, "Seleziona", "Annulla");
	return 1;
}

stock Faction_ShowOptions(factionid, playerid)
{
	if(!Faction_IsValid(factionid) || Account_GetAdminLevel(playerid) < 5)
		return 0;
	pAdminSelectedFaction{playerid} = factionid;
	printf("Selected: %d", factionid);
	new String:content = @("Modifica Nome\nModifica Acronimo\nModifica Tipo\nAggiungi Rank\nModifica Ranks\nModifica Skins\n{FF0000}Resetta Fazione{FFFFFF}");
	Dialog_Show_s(playerid, Dialog_FactionActions, DIALOG_STYLE_LIST, Faction_GetNameStr(pAdminSelectedFaction{playerid}), content, "Avanti", "Indietro");
	return 1;
}


stock Faction_LoadAll()
{
	inline OnLoad()
	{
		printf("Loading factions...");
		new rows = cache_num_rows(), data[E_FACTION_DATA];
		
		for(new i = 0; i < rows; ++i)
		{
			data[Created] = 1;
			cache_get_value_index_int(i, 0, data[ID]);
			cache_get_value_index(i, 1, data[Name]);
			cache_get_value_index(i, 2, data[ShortName]);
			cache_get_value_index_int(i, 3, data[Type]);
			cache_get_value_index_float(i, 4, data[SpawnX]);
			cache_get_value_index_float(i, 5, data[SpawnY]);
			cache_get_value_index_float(i, 6, data[SpawnZ]);
			cache_get_value_index_int(i, 7, data[Interior]);
			cache_get_value_index_int(i, 8, data[World]);

			map_add_arr(Factions, data[ID], data);

			CallLocalFunction(#OnFactionLoaded, "d", data[ID]);
		}
		printf("%d factions loaded.", rows);
	}
	MySQL_PQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `factions` ORDER BY id");
}

stock Faction_Save(factionid)
{
	if(!Faction_IsValid(factionid))
		return 0;

	new Float:x, Float:y, Float:z, name[MAX_FACTION_NAME], shortName[MAX_FACTION_SHORT_NAME];
	Faction_GetSpawnPos(factionid, x, y, z);
	Faction_GetName(factionid, name);
	Faction_GetShortName(factionid, shortName);
	
	mysql_tquery_f(gMySQL, "UPDATE `factions` SET \
	name = '%e', short_name = '%e', \
	type = '%d', \
	spawn_x = '%f', spawn_y = '%f', spawn_z = '%f', \
	spawn_interior = '%d', spawn_world = '%d' \
	WHERE id = '%d'",
	name, shortName,
	Faction_GetType(factionid),
	x, y, z,
	Faction_GetSpawnInterior(factionid), Faction_GetSpawnWorld(factionid),
	Faction_GetID(factionid));

	CallLocalFunction(#OnFactionSaved, "d", factionid);
	return 1;
}

stock Faction_SendMessage(factionid, color, const str[], GLOBAL_TAG_TYPES:...)
{
	new String:string = str_format(str, ___(3));
	return Faction_SendMessageStr(factionid, color, string);
}

stock Faction_SendMessageStr(factionid, color, String:string)
{
	foreach(new i : Player)
	{
		if(Character_GetFaction(i) != factionid || !Character_IsFactionOOCEnabled(i))
			continue;
		SendTwoLinesMessageStr(i, color, string);
	}
	return 1;
}

// ACCESSORS
stock Faction_IsValid(factionid)
{
	return map_has_key(Factions, factionid);
}

stock Faction_IsCreated(factionid)
{
	if(!Faction_IsValid(factionid))
		return 0;
	return map_get(Factions, factionid, Created);
}

stock Faction_GetID(factionid)
{
	if(!Faction_IsValid(factionid))
		return INVALID_FACTION_ID;
	return map_get(Factions, factionid, ID);
}

stock Faction_SetName(factionid, name[])
{
	if(!Faction_IsValid(factionid))
		return 0;
	new Iter:it = map_iter_at(Factions, factionid);
	iter_set_cells(it, Name, name, MAX_FACTION_NAME);
	return 1;
}

stock Faction_GetName(factionid, name[MAX_FACTION_NAME])
{
	if(!Faction_IsValid(factionid))
		return 0;
	new Iter:it = map_iter_at(Factions, factionid);
    iter_get_md_arr(it, {Name}, name);
	return 1;
}

stock String:Faction_GetNameStr(factionid)
{
	if(!Faction_IsValid(factionid))
		return STRING_NULL;
	new name[MAX_FACTION_NAME];
	Faction_GetName(factionid, name);
	return str_new(name);
}

stock Faction_SetShortName(factionid, name[])
{
	if(!Faction_IsValid(factionid))
		return 0;
	new Iter:it = map_iter_at(Factions, factionid);
	iter_set_cells(it, ShortName, name, MAX_FACTION_SHORT_NAME);
	return 1;
}

stock Faction_GetShortName(factionid, shortName[MAX_FACTION_SHORT_NAME])
{
	if(!Faction_IsValid(factionid))
		return 0;
	new Iter:it = map_iter_at(Factions, factionid);
    iter_get_md_arr(it, {ShortName}, shortName);
	return 1;
}

stock String:Faction_GetShortNameStr(factionid)
{
	new shortName[8];
	if(!Faction_GetShortName(factionid, shortName))
		return STRING_NULL;
	return str_new(shortName);
}

stock Faction_SetType(factionid, type)
{
	if(!Faction_IsValid(factionid))
		return 0;
	map_set_cell(Factions, factionid, Type, type);
	return 1;
}

stock Faction_GetType(factionid)
{
	if(!Faction_IsValid(factionid))
		return -1;
	return map_get(Factions, factionid, Type);
}

stock Faction_GetSpawnPos(factionid, &Float:x, &Float:y, &Float:z)
{
	if(!Faction_IsValid(factionid))
		return 0;
	x = Float:map_get(Factions, factionid, SpawnX);
	y = Float:map_get(Factions, factionid, SpawnY);
	z = Float:map_get(Factions, factionid, SpawnZ);
	return 1;
}

stock Faction_SetSpawnPos(factionid, Float:x, Float:y, Float:z, interior, world)
{
	if(!Faction_IsValid(factionid))
		return 0;
	map_set_cell(Factions, factionid, SpawnX, x);
	map_set_cell(Factions, factionid, SpawnY, y);
	map_set_cell(Factions, factionid, SpawnZ, z);
	map_set_cell(Factions, factionid, Interior, interior);
	map_set_cell(Factions, factionid, World, world);
	return 1;
}

stock Faction_GetSpawnInterior(factionid)
{
	if(!Faction_IsValid(factionid))
		return 0;
	return map_get(Factions, factionid, Interior);
}

stock Faction_GetSpawnWorld(factionid)
{
	if(!Faction_IsValid(factionid))
		return 0;
	return map_get(Factions, factionid, World);
}

// CHARACTER
stock Character_IsPolice(playerid)
{
	return Faction_GetType(Character_GetFaction(playerid)) == FACTION_TYPE_POLICE;
}

Dialog:Dialog_FactionList(playerid, response, listitem, inputtext[])
{
	if(!response || Account_GetAdminLevel(playerid) < 5)
		return 0;
	return Faction_ShowOptions(FactionListItem[playerid][listitem], playerid);
}

Dialog:Dialog_ChangeFactionName(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Faction_ShowOptions(pAdminSelectedFaction{playerid}, playerid);
	if(strlen(inputtext) < 4 || strlen(inputtext) > 32)
		return Dialog_Show(playerid, Dialog_ChangeFactionName, DIALOG_STYLE_INPUT, "Modifica Nome", "{FF0000}Il nome inserito è troppo lungo o troppo corto!\n{FFFFFF}Inserisci il nuovo nome della fazione!\n{00FF00}Lunghezza Minima: 4.\n{00FF00}Lunghezza Massima: 32.", "Cambia", "Annulla");
	new fid = pAdminSelectedFaction{playerid};
	Faction_SetName(fid, inputtext);
	Faction_Save(fid);
	Faction_ShowOptions(fid, playerid);
	return 1;
}

Dialog:ChangeFactionShortName(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Faction_ShowOptions(pAdminSelectedFaction{playerid}, playerid);
	if(strlen(inputtext) <3 || strlen(inputtext) > 8)
		return Dialog_Show(playerid, ChangeFactionShortName, DIALOG_STYLE_INPUT, "Modifica Acronimo", "{FF0000}Il nome inserito è troppo lungo o troppo corto!\n{FFFFFF}Inserisci il nuovo nome della fazione!\n{00FF00}Lunghezza Minima: 3.\n{00FF00}Lunghezza Massima: 8.", "Cambia", "Annulla");
	new fid = pAdminSelectedFaction{playerid};
	Faction_SetShortName(fid, inputtext);
	Faction_Save(fid);
	Faction_ShowOptions(fid, playerid);
	return 1;
}

Dialog:ChangeFactionType(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Faction_ShowOptions(pAdminSelectedFaction{playerid}, playerid);
	new val = strval(inputtext),
		factionid = pAdminSelectedFaction{playerid};
	if(!IsNumeric(inputtext) || val < 0 || val >= FACTION_TYPE_LAST)
		return Dialog_Show(playerid, ChangeFactionType, DIALOG_STYLE_INPUT, "Tipo Fazione", "{FF0000}Il tipo inserito non è valido!\n{FFFFFF}Inserisci il nuovo tipo per la fazione\nTipo Attuale: %d.\nTipi: 0 - %d", "Modifica", "Annulla", Faction_GetType(factionid), FACTION_TYPE_LAST-1);
	Faction_SetType(factionid, val);
	Faction_Save(factionid);
	SendMessageToAdmins(true, COLOR_ADMIN, "[ADM-CMD] %s (%d) ha modificato il tipo della fazione %d. Nuovo tipo: %d", Account_GetName(playerid), playerid, factionid, val);
	return 1;
}


Dialog:Dialog_DeleteFactionConfirm(playerid, response, listitem, inputtext[])
{
	if(Account_GetAdminLevel(playerid) < 5) return 0;
	if(!response)
		return Faction_ShowList(playerid);
	new factionid = pAdminSelectedFaction{playerid};
	SendMessageToAdmins(true, COLOR_YELLOW, "[ADMIN-ALERT]: %s (%d) ha rimosso la fazione %d (Tipo: %d).", 
	Account_GetName(playerid), playerid, factionid, Faction_GetType(factionid));
	Faction_Delete(factionid);
	return Faction_ShowList(playerid);
}

Dialog:Dialog_FactionActions(playerid, response, listitem, inputtext[])
{
	if(!response || Account_GetAdminLevel(playerid) < 5)
		return Faction_ShowList(playerid);
	new factionid = pAdminSelectedFaction{playerid};
	switch(listitem)
	{
		case 0: // Modify Name
		{
			return Dialog_Show(playerid, Dialog_ChangeFactionName, DIALOG_STYLE_INPUT, "Modifica Nome", "{FFFFFF}Inserisci il nuovo nome della fazione!\n{00FF00}Lunghezza Minima: 4.\n{00FF00}Lunghezza Massima: 32.", "Cambia", "Annulla");
		}
		case 1: // Modifi Short Name
		{
			return Dialog_Show(playerid, ChangeFactionShortName, DIALOG_STYLE_INPUT, "Modifica Acronimo", "{FFFFFF}Inserisci il nuovo acronimo della fazione!\n{00FF00}Lunghezza Minima: 3.\n{00FF00}Lunghezza Massima: 8.", "Cambia", "Annulla");
		}
		case 2: // Modify Type
		{
			return Dialog_Show(playerid, ChangeFactionType, DIALOG_STYLE_INPUT, "Tipo Fazione", "Inserisci il nuovo tipo per la fazione\nTipo Attuale: %d.\nTipi: 0 - %d", "Modifica", "Annulla", Faction_GetType(factionid), FACTION_TYPE_LAST-1);
		}
		case 3: // Add Rank
		{
			return Faction_StartCreateRank(playerid);
		}
		case 4:
		{
			return Faction_ShowRankList(factionid, playerid);
		}
		case 5:
		{
			return Faction_ShowSkinList(factionid, playerid);
		}
		case 6:
		{
			new String:title = str_format("%S", Faction_GetNameStr(factionid));
			return Dialog_Show_s(playerid, Dialog_DeleteFactionConfirm, DIALOG_STYLE_MSGBOX, title, @("{FF0000}Sei sicuro di voler rimuovere la fazione definitivamente?{FFFFFF}"), "Rimuovi", "Annulla");
		}
	}
	return 1;
}