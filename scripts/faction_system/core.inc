#include <YSI_Coding\y_hooks>
#include <faction_system\accessors>

hook OnGameModeInit()
{
	Faction_LoadAll();
	for(new i = 0, j = sizeof(PoliceArrestPositions); i < j; ++i)
	{
		Pickup_Create(1239, i, PoliceArrestPositions[i][aX], PoliceArrestPositions[i][aY], PoliceArrestPositions[i][aZ], ELEMENT_TYPE_ARREST, -1, PoliceArrestPositions[i][aInterior]);
		CreateDynamic3DTextLabel("Punto Arresto", COLOR_BLUE, PoliceArrestPositions[i][aX], PoliceArrestPositions[i][aY], PoliceArrestPositions[i][aZ], 15.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, -1, PoliceArrestPositions[i][aInterior]);
	}
	return 1;
}

stock Faction_Create(name[], shortName[], type)
{
	new factionid = Iter_Free(Factions);
	if(factionid == INVALID_FACTION_ID)
		return 0;
	FactionInfo[factionid][fCreated] = 1;
	format(FactionInfo[factionid][fName], 32, "%s", name); // set(data[fName], name);
	format(FactionInfo[factionid][fShortName], 8, "%s", shortName); // set(data[fShortName], shortName);
	FactionInfo[factionid][fType] = type;
	FactionInfo[factionid][fSpawnX] = FactionInfo[factionid][fSpawnY] = FactionInfo[factionid][fSpawnZ] = 0.0;
	FactionInfo[factionid][fInterior] = 0;
	FactionInfo[factionid][fWorld] = 0;
	inline OnInsert()
	{
		FactionInfo[factionid][fID] = cache_insert_id();
		FactionSkinsCount{factionid} = 0;
		for(new i = 0; i < MAX_FACTION_RANKS; ++i)
		{
			Faction_SetRankName(factionid, i, "Nessuno");
			Faction_SetRankSalary(factionid, i, 0);
			mysql_tquery_f(gMySQL, "INSERT INTO `faction_ranks` (FactionID, Rank, Name, Salary) VALUES('%d', '%d', 'Nessuno', '0')", FactionInfo[factionid][fID], i);
		}
		for(new i = 0; i < MAX_FACTION_SKINS; ++i)
		{
			Faction_SetSkin(factionid, i, 0);
			mysql_tquery_f(gMySQL, "INSERT INTO `faction_skins` (FactionID, SkinID, SlotID) VALUES('%d', '0', '%d')", FactionInfo[factionid][fID], i);
		}

		Iter_Add(Factions, factionid);
	}
	MySQL_TQueryInline(gMySQL, using inline OnInsert, "INSERT INTO `factions` (Name, ShortName, Type) VALUES('%e', '%e', '%d')", name, shortName, type);
	return 1;
}

stock Faction_Reset(factionid)
{
	
	if(!Faction_IsValid(factionid))
		return 0;

	mysql_tquery_f(gMySQL, "UPDATE `characters` SET Faction = '-1', FactionRank = '0' WHERE Faction = '%d'", factionid);
	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET Faction = '-1' WHERE Faction = '%d'", factionid);
	
	foreach(new b : Buildings)
	{
		if(!Building_IsValid(b)) continue;
		if(Building_GetFaction(b) == factionid)
		{
			Building_SetFaction(b, -1);
		}
	}

	foreach(new i : Player)
	{
		if(!Character_IsLogged(i))
			continue;
		if(Character_GetFaction(i) == factionid)
		{
			Character_SetFaction(i, -1);
			Character_SetRank(i, 0);
			SendClientMessage(i, COLOR_GREEN, "La fazione a cui facevi parte non esiste più. Sei stato rimosso.");
		}
	}

	for(new i = 0; i < MAX_FACTION_RANKS; ++i)
	{
		Faction_SetRankName(factionid, i, "Nessuno");
		Faction_SetRankSalary(factionid, i, 0);
	}

	for(new i = 0; i < MAX_FACTION_SKINS; ++i)
	{
		Faction_SetSkin(factionid, i, 0);
	}
	
	new data[E_FACTION_DATA];
	data[fID] = FactionInfo[factionid][fID];
	set(data[fName], "----------");
	set(data[fShortName], "--");
	FactionInfo[factionid] = data;

	Faction_Save(factionid);
	return 1;
}

stock Faction_ShowList(playerid, edit = true)
{
	new String:content = @("ID\tNome\tAcronimo\n");

	for(new i = 0; i < MAX_FACTIONS; ++i)
	{
		if(FactionInfo[i][fCreated])
			content += str_format("#%d\t%S\t%S\n", i, Faction_GetNameStr(i), Faction_GetShortNameStr(i));
		else
			content += str_format("#%d\tSlot Libero\t---\n", i);
	}

	if(edit)
		Dialog_Show_s(playerid, Dialog_FactionList, DIALOG_STYLE_TABLIST_HEADERS, @("Lista Fazioni"), content, "Seleziona", "Annulla");
	else
		Dialog_Show_s(playerid, DialogNull, DIALOG_STYLE_TABLIST_HEADERS, @("Lista Fazioni"), content, "Seleziona", "Annulla");
	return 1;
}

stock Faction_ShowOptions(factionid, playerid)
{
	if(!Faction_IsValid(factionid) || AccountInfo[playerid][aAdmin] < 5)
		return 0;
	pAdminSelectedFaction{playerid} = factionid;
	new String:content = @("Modifica Nome\nModifica Acronimo\nModifica Tipo\nModifica Ranks\nModifica Skins\n{FF0000}Resetta Fazione{FFFFFF}");
	Dialog_Show_s(playerid, Dialog_FactionActions, DIALOG_STYLE_LIST, Faction_GetNameStr(pAdminSelectedFaction{playerid}), content, "Avanti", "Indietro");
	return 1;
}

stock Faction_ShowSkinList(factionid, playerid)
{
	if(!Faction_IsValid(factionid) || AccountInfo[playerid][aAdmin] < 5)
		return 0;
	new String:content = @("ID\tSkin ID\n");
	for(new i = 0; i < MAX_FACTION_SKINS; ++i)
	{
		new skinid = FactionSkins[factionid][i];
		if(skinid == 0)
			content += str_format("{FF0000}%d{FFFFFF}\t{FF0000}Non Settata{FFFFFF}\n", i);
		else
			content += str_format("%d\t%d\n", i, skinid);
	}
	Dialog_Show_s(playerid, Dialog_EditFactionSkin, DIALOG_STYLE_TABLIST_HEADERS, @("Skins"), content, "Modifica", "Annulla");
	return 1;
}

stock Faction_ShowRankList(factionid, playerid)
{
	if(!Faction_IsValid(factionid) || AccountInfo[playerid][aAdmin] < 5)
		return 0;
	new String:content = @("ID\tNome\tSalario ({85bb65}${FFFFFF})\n");
	for(new i = 0; i < MAX_FACTION_RANKS; ++i)
	{
		content += str_format("%d#\t%S\t{85bb65}$%d{FFFFFF}\n", i+1, Faction_GetRankNameStr(factionid, i), Faction_GetRankSalary(factionid, i));
	}
	return Dialog_Show_s(playerid, EditFactionRank, DIALOG_STYLE_TABLIST_HEADERS, @("Ranks"), content, "Modifica", "Indietro");
}


stock Faction_LoadAll()
{
	inline OnLoad()
	{
		printf("Loading factions...");
		new rows = cache_num_rows(), data[E_FACTION_DATA];
		if(rows >= MAX_FACTIONS)
			rows = MAX_FACTIONS;
		for(new i = 0; i < rows; ++i)
		{
			data[fCreated] = 1;
			cache_get_value_index_int(i, 0, data[fID]);
			cache_get_value_index(i, 1, data[fName]);
			cache_get_value_index(i, 2, data[fShortName]);
			cache_get_value_index_int(i, 3, data[fType]);
			cache_get_value_index_float(i, 4, data[fSpawnX]);
			cache_get_value_index_float(i, 5, data[fSpawnY]);
			cache_get_value_index_float(i, 6, data[fSpawnZ]);
			cache_get_value_index_int(i, 7, data[fInterior]);
			cache_get_value_index_int(i, 8, data[fWorld]);

			FactionInfo[i] = data;

			Faction_LoadRanks(i);
			Faction_LoadSkins(i);

			Iter_Add(Factions, i);
		}
		printf("%d factions loaded.", rows);
	}
	MySQL_PQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `factions` ORDER BY ID");
}

stock Faction_LoadRanks(i)
{
	printf("Loading ranks");
	inline OnLoad()
	{
		new rows = cache_num_rows();
		for(new x = 0; x < rows; ++x)
		{
			new rank;
			cache_get_value_index_int(x, 2, rank);
			if(rank >= MAX_FACTION_RANKS)
				continue;
			cache_get_value_index(x, 3, FactionRankInfo[i][rank][rankName]);
			//printf("%s",  FactionRankInfo[i][rank][rankName]);
			cache_get_value_index_int(x, 4, FactionRankInfo[i][rank][rankSalary]);
		}
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `faction_ranks` WHERE FactionID = '%d' ORDER BY Rank", Faction_GetID(i));
	return 1;
}

stock Faction_LoadSkins(factionid)
{
	inline OnLoad()
	{
		new rows = cache_num_rows();
		if(rows >= MAX_FACTION_SKINS)
			rows = MAX_FACTION_SKINS;
		for(new x = 0; x < rows; ++x)
		{
			cache_get_value_index_int(x, 2, FactionSkins[factionid][x]);
		}
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `faction_skins` WHERE FactionID = '%d' ORDER BY SlotID", Faction_GetID(factionid));
}

stock Faction_Save(factionid)
{
	inline OnSave()
	{
		printf("Faction %d saved.", factionid);
	}
	if(Faction_IsValid(factionid))
	{
		MySQL_TQueryInline(gMySQL, using inline OnSave, "UPDATE `factions` SET \
		Name = '%e', ShortName = '%e', \
		Type = '%d', \
		SpawnX = '%f', SpawnY = '%f', SpawnZ = '%f', \
		SpawnInterior = '%d', SpawnWorld = '%d' \
		WHERE ID = '%d'",
		FactionInfo[factionid][fName], FactionInfo[factionid][fShortName],
		FactionInfo[factionid][fType],
		FactionInfo[factionid][fSpawnX], FactionInfo[factionid][fSpawnY], FactionInfo[factionid][fSpawnZ],
		FactionInfo[factionid][fInterior], FactionInfo[factionid][fWorld],
		FactionInfo[factionid][fID]);
		Faction_SaveRanks(factionid);
		Faction_SaveSkins(factionid);
	}
}

stock Faction_SaveRanks(factionid)
{
	if(!Faction_IsValid(factionid))
		return 0;
	for(new i = 0; i < MAX_FACTION_RANKS; ++i)
	{
		inline OnSave()
		{
			//printf("Faction ID: %d - Rank: %d -> Saved", factionid, i);
		}
		MySQL_TQueryInline(gMySQL, using inline OnSave, "UPDATE `faction_ranks` SET \
		Name = '%e', Salary = '%d' WHERE FactionID = '%d' AND Rank = '%d'", 
		FactionRankInfo[factionid][i][rankName], FactionRankInfo[factionid][i][rankSalary],
		Faction_GetID(factionid), i);
	}
	return 1;
}

stock Faction_SaveSkins(factionid)
{
	if(!Faction_IsValid(factionid))
		return 0;
	for(new x = 0; x < MAX_FACTION_SKINS; ++x)
	{
		inline OnSave(){}
		MySQL_TQueryInline(gMySQL, using inline OnSave, "UPDATE `faction_skins` SET \
			SkinID = '%d' \
			WHERE FactionID = '%d' AND SlotID = '%d'",
			Faction_GetSkin(factionid, x),
			Faction_GetID(factionid), x);
	}
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

/*stock Faction_SaveSkins(factionid)
{
	if(!Faction_IsValid(factionid))
		return 0;
	
	return 1;
}*/