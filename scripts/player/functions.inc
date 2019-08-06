#include <YSI_Coding\y_hooks>


stock Character_LoadData(playerid)
{
    if(LoadCharacterResult(playerid))
	{
		Character_SetLogged(playerid, true);
		
		CallLocalFunction(#OnCharacterLoad, "i", playerid);
		

		if(Character_IsBanned(playerid))
		{
			if(gettime() < Character_GetBanExpiry(playerid))
			{
				Dialog_Show(playerid, DialogNull, DIALOG_STYLE_MSGBOX, "Personaggio Bannato", "Questo personaggio risulta bannato.\nData di scadenza: %s.\nSe pensi sia stato un errore, visita www.lsarp.it", "Ok", "", date(Character_GetBanExpiry(playerid)));
				KickEx(playerid);
				return 0;
			}
			else
			{
				Character_SetBanned(playerid, 0),
				Character_SetBanExpiry(playerid, 0);
			}
		}

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

stock Character_Save(playerid, disconnected = false)
{
    // This should never happen.
    if(!Character_IsLogged(playerid))
	   return 0;
    
    CallLocalFunction("OnCharacterPreSaveData", "ii", playerid, disconnected ? 1 : 0);
    
    new
		Float:_x, Float:_y, Float:_z,
		Float:angle,
		Float:hp, Float:armour
	;

    GetPlayerPos(playerid, _x, _y, _z);
    GetPlayerFacingAngle(playerid, angle);
    AC_GetPlayerHealth(playerid, hp);
    AC_GetPlayerArmour(playerid, armour);

	if(Account_IsAdminDuty(playerid))
	{
		hp = 100.0;
		armour = 0.0;
	}
    
	mysql_tquery_f(gMySQL, "UPDATE `characters` SET \
		health = '%f', armour = '%f', \
		paycheck = '%d', \
		banned = '%d', ban_expiry = '%d', \
		faction_duty = '%d' \
	   	WHERE id = '%d'",
	   	hp, armour,
		Character_GetFaction(playerid), Character_GetRank(playerid),
		Character_GetPayCheck(playerid),
		Character_IsBanned(playerid), Character_GetBanExpiry(playerid),
		Character_IsFactionDuty(playerid),
	   	Character_GetID(playerid));
    // Save A' Mammt
    CallLocalFunction(#OnCharacterSaveData, "d", playerid);
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
			House_Save(houseid);
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