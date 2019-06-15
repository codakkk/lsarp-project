#include <YSI_Coding\y_hooks>
// Called when Character_Spawn is called.
forward OnCharacterSpawn(playerid);

stock OnCharacterLoad(playerid)
{
	Character_SetLogged(playerid, true);

    LoadCharacterResult(playerid);


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

    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, PlayerInfo[playerid][pMoney]);

    SetPlayerScore(playerid, PlayerInfo[playerid][pLevel]);

    SetPlayerName(playerid, PlayerInfo[playerid][pName]); // disable to avoid GTA restart everytime during "debugging"
    gPlayerAMe3DText[playerid] = CreateDynamic3DTextLabel(" ", 0xFFFFFFFF, 0.0, 0.0, 0.30, 15.0, playerid, _, 1);
    
    AC_SetPlayerHealth(playerid, 100);

    //SetSpawnInfo(playerid, NO_TEAM, 0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0, 0);

    SpawnPlayer(playerid);
    
    CallLocalFunction("OnPlayerCharacterLoad", "i", playerid);

	//wait_ticks(1);
	Character_Spawn(playerid);
    return 1;
}

// Needed for anti-tp bug that makes player being kicked random.
hook OnPlayerRequestClass(playerid, classid)
{
	//SetSpawnInfo(playerid, NO_TEAM, 0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0, 0);
	return 1;
}

stock LoadCharacterResult(playerid)
{
    new count;
    cache_get_row_count(count);
    if(count > 0)
    {
		cache_get_value_index_int(0, 0, PlayerInfo[playerid][pID]);
		cache_get_value_index(0, 2, PlayerInfo[playerid][pName]);
		cache_get_value_index_int(0, 3, PlayerInfo[playerid][pMoney]);
		cache_get_value_index_int(0, 4, PlayerInfo[playerid][pLevel]);
		cache_get_value_index_int(0, 5, PlayerInfo[playerid][pAge]);
		cache_get_value_index_int(0, 6, PlayerInfo[playerid][pSex]);

		cache_get_value_index_float(0, 7, PlayerRestore[playerid][pLastX]);
		cache_get_value_index_float(0, 8, PlayerRestore[playerid][pLastY]);
		cache_get_value_index_float(0, 9, PlayerRestore[playerid][pLastZ]);
		cache_get_value_index_int(0, 10, PlayerRestore[playerid][pLastInterior]);
		cache_get_value_index_int(0, 11, PlayerRestore[playerid][pLastVirtualWorld]);

		cache_get_value_index_int(0, 12, PlayerRestore[playerid][pFirstSpawn]);

		cache_get_value_index_float(0, 13, PlayerRestore[playerid][pLastHealth]);
		cache_get_value_index_float(0, 14, PlayerRestore[playerid][pLastArmour]);

		cache_get_value_index_int(0, 15, PlayerInfo[playerid][pSkin]);

		cache_get_value_index_float(0, 16, PlayerRestore[playerid][pLastAngle]);
		cache_get_value_index_int(0, 17, PlayerRestore[playerid][pSpawned]);

		cache_get_value_index_int(0, 18, PlayerInfo[playerid][pPayDay]);
		cache_get_value_index_int(0, 19, PlayerInfo[playerid][pExp]);

		cache_get_value_index_int(0, 20, PlayerInfo[playerid][pBuildingKey]);
		cache_get_value_index_int(0, 21, PlayerInfo[playerid][pHouseKey]);

		cache_get_value_index_int(0, 22, PlayerInfo[playerid][pFaction]);
		cache_get_value_index_int(0, 23, PlayerInfo[playerid][pRank]);

		cache_get_value_index_int(0, 24, PlayerInfo[playerid][pJailTime]);
		cache_get_value_index_int(0, 25, PlayerInfo[playerid][pJailIC]);

		cache_get_value_index_int(0, 26, PlayerInfo[playerid][pPayCheck]);

		cache_get_value_index_int(0, 27, PlayerInfo[playerid][pFightStyle]);
		cache_get_value_index_int(0, 28, PlayerInfo[playerid][pChatStyle]);
		cache_get_value_index_int(0, 29, PlayerInfo[playerid][pWalkingStyle]);

		cache_get_value_index_int(0, 30, PlayerInfo[playerid][pBanned]);
		cache_get_value_index_int(0, 31, PlayerInfo[playerid][pBanExpiry]);

		cache_get_value_index_int(0, 32, PlayerInfo[playerid][pLastChopShopTime]);

		new tmp;
		cache_get_value_index_int(0, 33, tmp);
		Character_SetHunger(playerid, tmp);
		return 1;
    }
    return 0;
}

stock Character_Save(playerid, spawned = true, disconnected = false)
{
    // This should never happen.
    if(!Character_IsLogged(playerid))
	   return 0;
    
    CallLocalFunction("OnCharacterPreSaveData", "ii", playerid, disconnected ? 1 : 0);
    
    new
		Float:_x, Float:_y, Float:_z,
		Float:angle,
		Float:hp, Float:armour,
		isSpawned = false;

    //if() 
    isSpawned = Character_IsAlive(playerid) && spawned;

    GetPlayerPos(playerid, _x, _y, _z);
    GetPlayerFacingAngle(playerid, angle);
    AC_GetPlayerHealth(playerid, hp);
    AC_GetPlayerArmour(playerid, armour);

	if(pAdminDuty[playerid])
	{
		hp = 100.0;
		armour = 0.0;
	}
    
	mysql_tquery_f(gMySQL, "UPDATE `characters` SET \
	   	Money = '%d', Level = '%d', Age = '%d', Sex = '%d', \
	   	LastX = '%f', LastY = '%f', LastZ = '%f', LastAngle = '%f', LastInterior = '%d', LastVirtualWorld = '%d', Health = '%f', Armour = '%f', Skin = '%d', \
	   	Spawned = '%d', PayDay = '%d', Exp = '%d',  \
	   	BuildingKey = '%d', HouseKey = '%d', \
		Faction = '%d', FactionRank = '%d', \
		JailTime = '%d', JailIC = '%d', \
		PayCheck = '%d', \
		FightStyle = '%d', ChatStyle = '%d', WalkStyle = '%d', \
		Banned = '%d', BanExpiry = '%d', \
		LastChopShopTime = '%d', \
		Hunger = '%d' \
	   	WHERE ID = '%d'", 
	   	Character_GetMoney(playerid), Character_GetLevel(playerid), PlayerInfo[playerid][pAge], PlayerInfo[playerid][pSex], 
	   	_x, _y, _z, angle, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid),
	   	hp, armour, PlayerInfo[playerid][pSkin],
	   	isSpawned, PlayerInfo[playerid][pPayDay], Character_GetExp(playerid),
	   	Character_GetBuildingKey(playerid), Character_GetHouseKey(playerid),
		Character_GetFaction(playerid), Character_GetRank(playerid),
		Character_GetJailTime(playerid), Character_IsICJailed(playerid),
		Character_GetPayCheck(playerid),
		Character_GetFightingStyle(playerid), Character_GetChatStyle(playerid), Character_GetWalkingStyle(playerid),
		Character_IsBanned(playerid), Character_GetBanExpiry(playerid),
		Character_GetLastChopShopTime(playerid),
		Character_GetHunger(playerid),
	   	Character_GetID(playerid));
	
    // Save A' Mammt
    CallLocalFunction(#OnCharacterSaveData, "d", playerid);
    return 1;
}

stock Character_Delete(playerid, character_db_id, character_name[])
{
    #pragma unused playerid
    new query[256];
    mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `characters` WHERE ID = '%d' AND LOWER(Name) = LOWER('%e')", character_db_id, character_name);
    mysql_tquery(gMySQL, query);

    mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `character_inventory` WHERE CharacterID = '%d'", character_db_id);
    mysql_tquery(gMySQL, query);

	format(query, sizeof(query), "DELETE FROM `player_weapons` WHERE CharacterID = '%d'", character_db_id);
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
		mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `vehicles` WHERE OwnerID = '%d'", character_db_id);
		mysql_tquery(gMySQL, query);
    }
    MySQL_TQueryInline(gMySQL, using inline OnVehicles, "SELECT ID FROM `vehicles` WHERE OwnerID = '%d'", character_db_id);


	foreach(new b : Buildings)
	{
		if(Building_GetOwnerID(b) == character_db_id)
		{
			Building_ResetOwner(b);
			break;
		}
	}

	for(new i = 0; i < list_size(HouseList); ++i)
	{
		new ownerid = House_GetOwnerID(i);
		if(character_db_id == ownerid)
		{
			House_ResetOwner(i);
			House_Save(i);
			break;
		}
	}

    // Delete all others

    return 1;
}

stock Character_ShowStats(playerid, targetid)
{
	if(!Character_IsLogged(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non � collegato.");
	new
		Float:hp, Float:armour,
		expForNewLevel = (PlayerInfo[playerid][pLevel]+1) * 2;
	
    AC_GetPlayerHealth(playerid, hp);
    AC_GetPlayerArmour(playerid, armour);

    SendClientMessage(targetid, COLOR_GREEN, "_____________________[STATISTICHE]_____________________");
	SendFormattedMessage(targetid, -1, "[Account] Account: %s - E-Mail: %s", AccountInfo[playerid][aName], AccountInfo[playerid][aEMail]);
    SendFormattedMessage(targetid, -1, "[Personaggio] Nome: %s - Livello: %d - Skin: %d", Character_GetOOCName(playerid), Character_GetLevel(playerid), GetPlayerSkin(playerid));
	if(Character_GetFaction(playerid) != -1)
		SendClientMessageStr(targetid, -1, str_format("[Personaggio] Fazione: %S - Rank: %S", Faction_GetNameStr(Character_GetFaction(playerid)), Faction_GetRankNameStr(Character_GetFaction(playerid), Character_GetRank(playerid)-1)));
    SendFormattedMessage(targetid, -1, "[Denaro] Soldi: $%d - Stipendio: $%d", Character_GetMoney(playerid), Character_GetPayCheck(playerid));
    SendFormattedMessage(targetid, -1, "[Salute] HP: %.2f - Armatura: %.2f", hp, armour);
	SendFormattedMessage(targetid, -1, "[Alimentazione] Fame: %d", Character_GetHunger(playerid));
    
	if(Character_HasBuildingKey(playerid) && Character_HasHouseKey(playerid))
	   SendFormattedMessage(targetid, COLOR_YELLOW, "[Propriet�]: Edificio: %d - Casa: %d", Character_GetBuildingKey(playerid), Character_GetHouseKey(playerid));
    else if(Character_HasBuildingKey(playerid))
	   SendFormattedMessage(targetid, COLOR_YELLOW, "[Propriet�]: Edificio: %d", Character_GetBuildingKey(playerid));
    else if(Character_HasHouseKey(playerid))
	   SendFormattedMessage(targetid, COLOR_YELLOW, "[Propriet�]: Casa: %d", Character_GetHouseKey(playerid));
	
	SendFormattedMessage(targetid, -1, "[Altro] Interior: %d - VW: %d", GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));
	
	if(Account_GetPremiumLevel(playerid) > 0 && Account_GetPremiumExpiry(playerid) > gettime())
	{
		static premiumNames[][8] = {"Bronze", "Silver", "Gold"};
		new year, month, day, hour, minute, second;
		TimestampToDate(Account_GetPremiumExpiry(playerid), year, month, day, hour, minute, second, 0);

		SendFormattedMessage(playerid, COLOR_YELLOW, "Premium %s - Scadenza: %d/%d/%d - %d:%d", premiumNames[Account_GetPremiumLevel(playerid)-1], day, month, year, hour, minute);
	}
	
	SendFormattedMessage(targetid, -1, "Ti mancano %d/%d punti esperienza per salire di livello.", Character_GetExp(playerid), expForNewLevel);
    SendFormattedMessage(targetid, COLOR_YELLOW, "Tempo rimanente al PayDay: %d minuti", 60 - PlayerInfo[playerid][pPayDay]);
    SendClientMessage(targetid, COLOR_GREEN, "_______________________________________________________");
    return 1;
}

stock Character_Clear(playerid)
{
    new CleanData[E_PLAYER_DATA];
    PlayerInfo[playerid] = CleanData;

	PlayerInfo[playerid][pHouseKey] = -1;
	PlayerInfo[playerid][pBuildingKey] = -1;

	new CleanRestoreData[E_PLAYER_RESTORE_DATA];
	PlayerRestore[playerid] = CleanRestoreData;

	pLastPMTime[playerid] = 0;
}
// 283,75
// 284

stock Character_GetNameFromDatabase(userdbid, bool:removeUnderscore)
{
	new tmpName[MAX_PLAYER_NAME] = "Nessuno";
	if(userdbid > 0)
	{
		new query[76];
		mysql_format(gMySQL, query, sizeof(query), "SELECT Name FROM `characters` WHERE ID = '%d';", userdbid);
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

stock Character_FreezeForTime(playerid, timeInMillis)
{
	Character_SetFreezed(playerid, true);
	defer UnFreezeTimer(playerid, timeInMillis);
}

timer UnFreezeTimer[timeInMillis](playerid, timeInMillis)
{
	#pragma unused timeInMillis
	Character_SetFreezed(playerid, false);
}

timer LoadCharacterDataAfterTime[2000](playerid)
{
	AC_SetPlayerHealth(playerid, PlayerRestore[playerid][pLastHealth]);
	AC_SetPlayerArmour(playerid, PlayerRestore[playerid][pLastArmour]);
	Character_LoadWeapons(playerid);
}