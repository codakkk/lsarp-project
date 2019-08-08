#include <YSI_Coding\y_hooks>


stock Character_LoadData(playerid)
{
    if(LoadCharacterResult(playerid))
	{
		Character_SetLogged(playerid, true);
		
		CallLocalFunction(#OnCharacterLoad, "i", playerid);
		

		

		SetSpawnInfo(playerid, NO_TEAM, 0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0, 0);
		SpawnPlayer(playerid);
		
	}
	else return KickEx(playerid);
	return 1;
}

// Needed for anti-tp bug that makes player being kicked random.
hook OnPlayerRequestClass(playerid, classid)
{
	SetSpawnInfo(playerid, NO_TEAM, 0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0, 0);
	return 1;
}

stock Character_Delete(playerid, character_db_id, character_name[])
{
    #pragma unused playerid
    new query[256];
    mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `characters` WHERE id = '%d' AND LOWER(name) = LOWER('%e')", character_db_id, character_name);
    mysql_tquery(gMySQL, query);

    mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `character_inventory` WHERE CharacterID = '%d'", character_db_id);
    mysql_tquery(gMySQL, query);

	format(query, sizeof(query), "DELETE FROM `character_weapons` WHERE CharacterID = '%d'", character_db_id);
	mysql_pquery(gMySQL, query);

    inline OnVehicles()
    {
		new count = cache_num_rows(), vid;
		for(new i = 0; i < count; ++i)
		{
			cache_get_value_index_int(i, 0, vid);
			mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `vehicle_inventory` WHERE VehicleID = '%d'", vid);
			mysql_tquery(gMySQL, query);
		}
		mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `vehicles` WHERE owner_id = '%d'", character_db_id);
		mysql_tquery(gMySQL, query);
    }
    MySQL_TQueryInline(gMySQL, using inline OnVehicles, "SELECT id FROM `vehicles` WHERE owner_id = '%d'", character_db_id);

	for_map(b : GetBuildingsMap())
	{
		new key = iter_get_key(b);
		if(Building_GetOwnerID(key) == Character_GetID(playerid))
		{
			Building_ResetOwner(key);
			break;
		}
	}

	for_map(i : GetHousesMap())
	{
		new houseid = iter_get_key(i);
		new ownerid = House_GetOwnerID(houseid);
		if(character_db_id == ownerid)
		{
			House_ResetOwner(houseid);
			break;
		}
	}

    // Delete all others

    return 1;
}


// 283,75
// 284

stock Character_GetNameFromDatabase(userdbid, bool:removeUnderscore)
{
	new tmpName[MAX_PLAYER_NAME] = "Nessuno";
	if(userdbid > 0)
	{
		new query[76];
		mysql_format(gMySQL, query, sizeof(query), "SELECT name FROM `characters` WHERE id = '%d';", userdbid);
		new Cache:cache = mysql_query(gMySQL, query, true);
		if(cache_num_rows() > 0)
		{
			cache_get_value_index(0, 0, tmpName, sizeof(tmpName));
			if(removeUnderscore)
				for(new i = 0, j = strlen(tmpName); i < j; ++i) 
					if(tmpName[i] == '_') 
						tmpName[i] = ' ';
		}
		cache_delete(cache);
	}
	return tmpName;
}

// Mainly used by /aduty 
stock Character_EnableSomeAntiCheats(playerid, enabled)
{
	static const cheatCodes[] = {0, 1, 2, 3, 5, 6, 7, 8, 9, 10};
	for(new i = 0, j = sizeof(cheatCodes); i < j; ++i)
		EnableAntiCheatForPlayer(playerid, cheatCodes[i], enabled);
}