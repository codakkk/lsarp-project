#include <YSI_Coding\y_hooks>

#define DEFAULT_SPAWN_X 		1723.3232 
#define DEFAULT_SPAWN_Y 		-1867.1775
#define DEFAULT_SPAWN_Z 		13.5705

enum
{
	SPAWN_POINT_TYPE_NONE = 0,
	SPAWN_POINT_TYPE_HOUSE = 1,
	SPAWN_POINT_TYPE_FACTION = 2,
	SPAWN_POINT_TYPE_BUSINESS = 3
}

static enum E_PLAYER_SPAWN_DATA
{
	Float:SpawnX,
	Float:SpawnY,
	Float:SpawnZ,
	Float:SpawnA,
	SpawnInterior,
	SpawnVirtualWorld,
    Float:LastX,
    Float:LastY,
    Float:LastZ,
    Float:LastA,
    LastInterior,
    LastVirtualWorld,
};
static 
	PlayerSpawnData[MAX_PLAYERS][E_PLAYER_SPAWN_DATA],
	CleanSpawnData[E_PLAYER_SPAWN_DATA],

	bool:IsSpawned[MAX_PLAYERS char] = {false, ...},
	bool:IsFirstSpawn[MAX_PLAYERS char] = {false, ...},
	SpawnPoint[MAX_PLAYERS char] = {SPAWN_POINT_TYPE_NONE, ...},
	SpawnPointExtraID[MAX_PLAYERS] = {-1, ...}
;

hook OnPlayerClearData(playerid)
{
	IsSpawned{playerid} = IsFirstSpawn{playerid} = false;
	SpawnPoint{playerid} = SPAWN_POINT_TYPE_NONE;
	SpawnPointExtraID[playerid] = -1;
	PlayerSpawnData[playerid] = CleanSpawnData;
	return 1;
}

hook OnCharacterCreated(playerid, character_db_id)
{
	mysql_tquery_f(gMySQL, "INSERT INTO `character_spawn_data` (character_id, is_first_spawn, spawn_x, spawn_y, spawn_z, spawn_a, spawn_interior, spawn_world) VALUES('%d', 1, '%f', '%f', '%f', '0.0', '0', '0')", character_db_id, DEFAULT_SPAWN_X, DEFAULT_SPAWN_Y, DEFAULT_SPAWN_Z);
	return 1;
}

hook OnCharacterLoad(playerid)
{
	Character_LoadSpawnData(playerid);
	return 1;
}

CMD:spawnpoint(playerid, params[])
{
	new spawnType = 0, extraid;
	if(sscanf(params, "dD(-1)", spawnType, extraid))
	{
		SendClientMessage(playerid, COLOR_ERROR, "/spawnpoint <type>");
		SendClientMessage(playerid, COLOR_ERROR, "Tipi - 0: Mall, 1: Casa, 2: Fazione, 3: Business");
		return 1;
	}
	if(spawnType == SPAWN_POINT_TYPE_HOUSE)
	{
		if(extraid == -1)
		{
			SendClientMessage(playerid, COLOR_ERROR, "/spawnpoint 1 <houseid>");
			SendClientMessage(playerid, COLOR_ERROR, "Tipi - 0: Mall, 1: Casa, 2: Fazione, 3: Business");
			return 1;
		}
		if(!House_IsValid(extraid))
			return SendClientMessage(playerid, COLOR_ERROR, "La casa inserita non esiste.");
		
		if(Character_GetHouseKey(playerid) != extraid && House_GetOwnerID(extraid) != Character_GetID(playerid))
			return SendClientMessage(playerid, COLOR_ERROR, "Non sei nè il proprietario nè un affittuario di questa casa.");
		
		SpawnPoint{playerid} = spawnType;
		SpawnPointExtraID[playerid] = extraid;
		SendClientMessage(playerid, COLOR_GREEN, "Hai cambiato il tuo spawn point. La prossima volta respawnerai a casa tua.");
	}
	else
	{
		SpawnPoint{playerid} = spawnType;
		SpawnPointExtraID[playerid] = -1;
		if(spawnType == SPAWN_POINT_TYPE_NONE)
			SendClientMessage(playerid, COLOR_GREEN, "Hai cambiato il tuo spawn point. La prossima volta respawnerai al Mall.");
		else if(spawnType == SPAWN_POINT_TYPE_FACTION)
			SendClientMessage(playerid, COLOR_GREEN, "Hai cambiato il tuo spawn point. La prossima volta respawnerai all'HQ della tua fazione.");
		else if(spawnType == SPAWN_POINT_TYPE_BUSINESS)
			SendClientMessage(playerid, COLOR_GREEN, "Hai cambiato il tuo spawn point. La prossima volta respawnerai al tuo negozio.");
	}
	return 1;
}

CMD:spawntest(playerid, params[])
{
	Character_Spawn(playerid);
	return 1;
}

stock Character_HandleFirstSpawn(playerid)
{
	if(!IsFirstSpawn{playerid})
		return 0;
	
	IsFirstSpawn{playerid} = false;

	Character_SetSkin(playerid, 46);

	SetPlayerPos(playerid, 1748.1887, -1860.0414, 13.5792);
	
	if(Account_GetCharactersCount(playerid) <= 2)
	{
		new count = Account_GetCharactersCount(playerid);
		static const characterFirstLoginMoney[] = {30000, 20000, 10000};
		Character_GiveMoney(playerid, characterFirstLoginMoney[count], "FirstSpawn");
		SendFormattedMessage(playerid, -1, "(( Ti sono stati dati {85bb65}$%d{FFFFFF} per cominciare. ))", characterFirstLoginMoney[count]);
	}
	
	new query[128];
	mysql_format(gMySQL, query, sizeof(query), "UPDATE `character_spawn_data` SET is_first_spawn = '0' WHERE character_id = '%d'", Character_GetID(playerid));
	mysql_tquery(gMySQL, query);

	Account_AddCharactersCount(playerid, 1);

	mysql_format(gMySQL, query, sizeof(query), "UPDATE `accounts` SET characters_counter = '%d' WHERE id = '%d'", Account_GetCharactersCount(playerid), Account_GetID(playerid));
	mysql_tquery(gMySQL, query);

	Character_Save(playerid);
	return 1;
}

hook OnPlayerSpawn(playerid)
{
	if(Character_IsAlive(playerid))
		Character_Spawn(playerid);
	return 1;
}

stock Character_Spawn(playerid)
{
	PreloadAnimations(playerid);
	#if defined LSARP_DEBUG
		if(!Account_IsAdminDuty(playerid) && !strcmp(Account_GetName(playerid), "Coda", false))
		{
			pc_cmd_aduty(playerid, "");
		}
	#endif
	Character_ResetDeathState(playerid);
	AC_SetPlayerHealth(playerid, 100.0);
	AC_SetPlayerArmour(playerid, 0.0);

	Character_FreezeForTime(playerid, 2000);

	if(IsFirstSpawn{playerid}) // First Login/Spawn
    {
		Character_HandleFirstSpawn(playerid);
    }
	else if(Character_IsJailed(playerid))
	{
		Character_SetToJailPos(playerid);
		PlayerTextDrawShow(playerid, pJailTimeText[playerid]);
	}
    else if(IsSpawned{playerid} && PlayerSpawnData[playerid][LastX] != 0.0 && PlayerSpawnData[playerid][LastY] != 0.0 && PlayerSpawnData[playerid][LastZ] != 0.0)
    {
		IsSpawned{playerid} = false;
		SetPlayerPos(playerid, PlayerSpawnData[playerid][LastX], PlayerSpawnData[playerid][LastY], PlayerSpawnData[playerid][LastZ]);
		SetPlayerFacingAngle(playerid, PlayerSpawnData[playerid][LastA]);
		SetPlayerInterior(playerid, PlayerSpawnData[playerid][LastInterior]);
		Character_SetVirtualWorld(playerid, PlayerSpawnData[playerid][LastVirtualWorld]);
		defer LoadCharacterDataAfterTime(playerid);
    }
    else
    {
		if(Character_GetSpawnPoint(playerid) == SPAWN_POINT_TYPE_HOUSE && Character_GetSpawnPointExtraID(playerid) != -1)
		{
			new housekey = Character_GetSpawnPointExtraID(playerid);
			if(House_GetOwnerID(housekey) != Character_GetID(playerid) && Character_GetHouseKey(playerid) != housekey)
			{
				SendClientMessage(playerid, COLOR_ERROR, "La casa in cui sei spawnato non è più tua o non ne sei piu' affittuario.");
				SendClientMessage(playerid, COLOR_ERROR, "Il tuo spawn-point è stato resettato al Mall.");
				Character_SetSpawnPoint(playerid, SPAWN_POINT_TYPE_NONE);
				SpawnPointExtraID[playerid] = -1;
				Character_Save(playerid);
			}
			new Float:x, Float:y, Float:z;
			House_GetEnterPosition(housekey, x, y, z);
			SetPlayerPos(playerid, x, y, z);
			SetPlayerInterior(playerid, House_GetEnterInterior(housekey));
			Character_SetVirtualWorld(playerid, House_GetEnterWorld(housekey));
		}
		else if(Character_GetSpawnPoint(playerid) == SPAWN_POINT_TYPE_FACTION && Character_GetFaction(playerid) != INVALID_FACTION_ID)
		{
			new Float:x, Float:y, Float:z, factionid = Character_GetFaction(playerid);
			Faction_GetSpawnPos(factionid, x, y, z);
			SetPlayerPos(playerid, x, y, z);
			SetPlayerInterior(playerid, Faction_GetSpawnInterior(factionid));
			Character_SetVirtualWorld(playerid, Faction_GetSpawnWorld(factionid));
		}
		else
		{
			SetPlayerPos(playerid, DEFAULT_SPAWN_X, DEFAULT_SPAWN_Y, DEFAULT_SPAWN_Z);
			SetPlayerInterior(playerid, 0);
			Character_SetVirtualWorld(playerid, 0);
		}
		AC_SetPlayerHealth(playerid, 100.0);
		AC_SetPlayerArmour(playerid, 0.0);
    }
	//ClearAnimations(playerid);
	//ApplyAnimation(playerid, "CARRY", "crry_prtial", 2.0, 0, 0, 0, 0, 0);
	CallLocalFunction(#OnCharacterSpawn, "d", playerid);
	return 1;
}

// Sets Character's Spawn Point.
// spawnid:
// 1: means house (if any)
// 2: means faction (if any)
// 3: means business (if any)
// from 4 to INFINITE: means a specific spawn point.
stock Character_SetSpawnPoint(playerid, spawnid)
{
	if(spawnid < SPAWN_POINT_TYPE_NONE)
		return 0;
	SpawnPoint{playerid} = spawnid;
	return 1;
}

stock Character_SetSpawnPointExtraID(playerid, extraid)
{
	SpawnPointExtraID[playerid] = extraid;
}

stock Character_GetSpawnPointExtraID(playerid)
{
	return SpawnPointExtraID[playerid];
}

stock Character_GetSpawnPoint(playerid)
{
	return SpawnPoint{playerid};
}

static Character_SaveSpawnData(playerid)
{
	return 1;
}

static Character_LoadSpawnData(playerid)
{
	inline OnLoad()
	{
		new 
			rows = cache_num_rows(),
			temp = 0
		;

		if(rows <= 0)
		{
			mysql_tquery_f(gMySQL, "INSERT INTO `character_spawn_data` (character_id, is_first_spawn) VALUES('%d', 0)", Character_GetID(playerid));
			return 1;
		}

		cache_get_value_index_int(0, 1, temp);
		IsFirstSpawn{playerid} = temp ? true : false;

		cache_get_value_index_int(0, 2, temp);
		IsSpawned{playerid} = temp ? true : false;

		cache_get_value_index_float(0, 3, PlayerSpawnData[playerid][LastX]);
		cache_get_value_index_float(0, 4, PlayerSpawnData[playerid][LastY]);
		cache_get_value_index_float(0, 5, PlayerSpawnData[playerid][LastZ]);
		cache_get_value_index_float(0, 6, PlayerSpawnData[playerid][LastA]);
		cache_get_value_index_int(0, 7, PlayerSpawnData[playerid][LastInterior]);
		cache_get_value_index_int(0, 8, PlayerSpawnData[playerid][LastVirtualWorld]);

		cache_get_value_index_float(0, 9, PlayerSpawnData[playerid][SpawnX]);
		cache_get_value_index_float(0, 10, PlayerSpawnData[playerid][SpawnY]);
		cache_get_value_index_float(0, 11, PlayerSpawnData[playerid][SpawnZ]);
		cache_get_value_index_float(0, 12, PlayerSpawnData[playerid][SpawnA]);
		cache_get_value_index_int(0, 13, PlayerSpawnData[playerid][SpawnInterior]);
		cache_get_value_index_int(0, 14, PlayerSpawnData[playerid][SpawnVirtualWorld]);
	
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `character_spawn_data` WHERE character_id = '%d'", Character_GetID(playerid));
	return 1;
}

timer LoadCharacterDataAfterTime[2000](playerid)
{
	
	CallLocalFunction("OnCharacterLateLoad", "d", playerid);
}
