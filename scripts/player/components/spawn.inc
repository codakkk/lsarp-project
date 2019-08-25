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
	Float:LastHealth,
	Float:LastArmour,
    LastInterior,
    LastVirtualWorld,
};
static 
	PlayerSpawnData[MAX_PLAYERS][E_PLAYER_SPAWN_DATA],
	CleanSpawnData[E_PLAYER_SPAWN_DATA],

	bool:IsSpawned[MAX_PLAYERS char] = {false, ...},
	bool:IsFirstSpawn[MAX_PLAYERS char] = {false, ...},
	SpawnPointType[MAX_PLAYERS char] = {SPAWN_POINT_TYPE_NONE, ...},
	SpawnPointID[MAX_PLAYERS] = {0, ...}
;

hook OnGameModeInit()
{
	mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `character_spawn_data` ( \
						`character_id` int(11) NOT NULL, \
						`is_first_spawn` int(11) NOT NULL, \
						`is_spawned` int(1) NOT NULL DEFAULT '0', \
						`last_x` float NOT NULL DEFAULT '0', \
						`last_y` float NOT NULL DEFAULT '0', \
						`last_z` float NOT NULL DEFAULT '0', \
						`last_a` float NOT NULL DEFAULT '0', \
						`last_interior` int(11) NOT NULL DEFAULT '0', \
						`last_world` int(11) NOT NULL DEFAULT '0', \
						`last_health` float NOT NULL DEFAULT '100', \
						`last_armour` float NOT NULL DEFAULT '0', \
						`spawn_type` tinyint(4) NOT NULL DEFAULT '0', \
						`spawn_id` smallint(6) NOT NULL DEFAULT '0', \
						`spawn_x` float NOT NULL DEFAULT '0', \
						`spawn_y` float NOT NULL DEFAULT '0', \
						`spawn_z` float NOT NULL DEFAULT '0', \
						`spawn_a` float NOT NULL DEFAULT '0', \
						`spawn_interior` int(11) NOT NULL DEFAULT '0', \
						`spawn_world` int(11) NOT NULL DEFAULT '0', \
						PRIMARY KEY (`character_id`) \
						) ENGINE=InnoDB DEFAULT CHARSET=latin1;");
	return 1;
}

hook OnPlayerClearData(playerid)
{
	IsSpawned{playerid} = IsFirstSpawn{playerid} = false;
	SpawnPointType{playerid} = SPAWN_POINT_TYPE_NONE;
	SpawnPointID[playerid] = 0;
	PlayerSpawnData[playerid] = CleanSpawnData;
	return 1;
}

hook OnCharacterDisconnected(playerid)
{
	Internal_SavePos(playerid);
	return 1;
}

hook OnCharacterCreated(playerid, character_db_id)
{
	mysql_tquery_f(gMySQL, "INSERT INTO `character_spawn_data` (character_id, is_first_spawn, spawn_type, spawn_id, spawn_x, spawn_y, spawn_z, spawn_a, spawn_interior, spawn_world) VALUES('%d', 1, '0', '0', '%f', '%f', '%f', '0.0', '0', '0')", character_db_id, DEFAULT_SPAWN_X, DEFAULT_SPAWN_Y, DEFAULT_SPAWN_Z);
	return 1;
}

hook OnCharacterLoad(playerid)
{
	Character_LoadSpawnData(playerid);
	return 1;
}


ptask SavePlayerPositionTime[30000](playerid)
{
    if(!Character_IsLogged(playerid))
	   return 0;
    Internal_SavePos(playerid);
    return 1;
}

static Internal_SavePos(playerid)
{
	new 
		Float:x, Float:y, Float:z, Float:a,
		Float:health, Float:armour,
		interior = GetPlayerInterior(playerid),
		world = Character_GetVirtualWorld(playerid),
		isSpawned = Character_IsAlive(playerid) ? 1 : 0
	;

	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, a);

	AC_GetPlayerHealth(playerid, health);
	AC_GetPlayerArmour(playerid, armour);

	mysql_tquery_f(gMySQL, "UPDATE `character_spawn_data` SET \
	last_x = '%f', last_y = '%f', last_z = '%f', last_a = '%f', last_interior = '%f', last_world = '%f', \
	last_health = '%f', last_armour = '%f', \
	is_spawned = '%d' \
	WHERE \
	character_id = '%d'", 
	x, y, z, a, interior, world,
	health, armour,
	isSpawned,
	Character_GetID(playerid));
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
		
		Character_SetSpawnPointType(playerid, SPAWN_POINT_TYPE_HOUSE);
		Character_SetSpawnPointID(playerid, extraid);

		SendClientMessage(playerid, COLOR_GREEN, "Hai cambiato il tuo spawn point. La prossima volta respawnerai a casa tua.");
	}
	else if(spawnType == SPAWN_POINT_TYPE_BUSINESS)
	{
		if(!Character_HasBuildingKey(playerid))
			return SendClientMessage(playerid, COLOR_ERROR, "Non possiedi un business.");
		
		Character_SetSpawnPointType(playerid, SPAWN_POINT_TYPE_BUSINESS);
		Character_SetSpawnPointID(playerid, Character_GetBuildingKey(playerid));

		SendClientMessage(playerid, COLOR_GREEN, "Hai cambiato il tuo spawn point. La prossima volta respawnerai al tuo negozio.");
	}
	else if(spawnType == SPAWN_POINT_TYPE_FACTION)
	{
		if(Character_GetFaction(playerid) == INVALID_FACTION_ID)
			return SendClientMessage(playerid, COLOR_ERROR, "Non fai parte di una fazione.");
		
		Character_SetSpawnPointType(playerid, SPAWN_POINT_TYPE_FACTION);

		SendClientMessage(playerid, COLOR_GREEN, "Hai cambiato il tuo spawn point. La prossima volta respawnerai all'HQ della tua fazione.");
	}
	else
	{
		SpawnPointType{playerid} = spawnType;
		SpawnPointID[playerid] = 0;
		if(spawnType == SPAWN_POINT_TYPE_NONE)
			SendClientMessage(playerid, COLOR_GREEN, "Hai cambiato il tuo spawn point. La prossima volta respawnerai al Mall.");
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

		SetPlayerHealth(playerid, PlayerSpawnData[playerid][LastHealth]);
		SetPlayerArmour(playerid, PlayerSpawnData[playerid][LastArmour]);
    }
    else
    {
		if(Character_GetSpawnPointType(playerid) == SPAWN_POINT_TYPE_HOUSE && Character_GetSpawnPointID(playerid) != -1)
		{
			new housekey = Character_GetSpawnPointID(playerid);
			if(House_GetOwnerID(housekey) != Character_GetID(playerid) && Character_GetHouseKey(playerid) != housekey)
			{
				SendClientMessage(playerid, COLOR_ERROR, "Non possiedi più la casa. Il tuo spawn-point è stato resettato al Mall.");
				
				Character_SetSpawnPointType(playerid, SPAWN_POINT_TYPE_NONE);
				Character_SetSpawnPointID(playerid, 0);

				Streamer_UpdateEx(playerid, DEFAULT_SPAWN_X, DEFAULT_SPAWN_Y, DEFAULT_SPAWN_Z);

				SetPlayerPos(playerid, DEFAULT_SPAWN_X, DEFAULT_SPAWN_Y, DEFAULT_SPAWN_Z);
				SetPlayerInterior(playerid, 0);
				Character_SetVirtualWorld(playerid, 0);
			}
			else
			{
				new Float:x, Float:y, Float:z;
				House_GetEnterPosition(housekey, x, y, z);

				Streamer_UpdateEx(playerid, x, y, z);

				SetPlayerPos(playerid, x, y, z);
				SetPlayerInterior(playerid, House_GetEnterInterior(housekey));
				Character_SetVirtualWorld(playerid, House_GetEnterWorld(housekey));
			}
		}
		else if(Character_GetSpawnPointType(playerid) == SPAWN_POINT_TYPE_BUSINESS)
		{
			new buildingkey = Character_GetBuildingKey(playerid);
			if(!Building_IsValid(buildingkey) || Building_GetOwnerID(buildingkey) != Character_GetID(playerid))
			{
				SendClientMessage(playerid, COLOR_ERROR, "Non possiedi più il business. Il tuo spawn-point è stato resettato al Mall.");
				
				Character_SetSpawnPointType(playerid, SPAWN_POINT_TYPE_NONE);
				Character_SetSpawnPointID(playerid, 0);

				Streamer_UpdateEx(playerid, DEFAULT_SPAWN_X, DEFAULT_SPAWN_Y, DEFAULT_SPAWN_Z);

				SetPlayerPos(playerid, DEFAULT_SPAWN_X, DEFAULT_SPAWN_Y, DEFAULT_SPAWN_Z);
				SetPlayerInterior(playerid, 0);
				Character_SetVirtualWorld(playerid, 0);
			}
			else
			{
				new Float:x, Float:y, Float:z;
				Building_GetEnterPos(buildingkey, x, y, z);
				
				Streamer_UpdateEx(playerid, x, y, z);

				SetPlayerPos(playerid, x, y, z);
				SetPlayerInterior(playerid, Building_GetEnterInterior(buildingkey));
				Character_SetVirtualWorld(playerid, Building_GetEnterWorld(buildingkey));
			}
		}
		else if(Character_GetSpawnPointType(playerid) == SPAWN_POINT_TYPE_FACTION && Character_GetFaction(playerid) != INVALID_FACTION_ID)
		{
			new Float:x, Float:y, Float:z, factionid = Character_GetFaction(playerid);
			Faction_GetSpawnPos(factionid, x, y, z);

			Streamer_UpdateEx(playerid, x, y, z);

			SetPlayerPos(playerid, x, y, z);
			SetPlayerInterior(playerid, Faction_GetSpawnInterior(factionid));
			Character_SetVirtualWorld(playerid, Faction_GetSpawnWorld(factionid));
		}
		else
		{
			Streamer_UpdateEx(playerid, DEFAULT_SPAWN_X, DEFAULT_SPAWN_Y, DEFAULT_SPAWN_Z);

			SetPlayerPos(playerid, DEFAULT_SPAWN_X, DEFAULT_SPAWN_Y, DEFAULT_SPAWN_Z);
			SetPlayerInterior(playerid, 0);
			Character_SetVirtualWorld(playerid, 0);
		}
		AC_SetPlayerHealth(playerid, 100.0);
		AC_SetPlayerArmour(playerid, 0.0);
    }
	defer LoadCharacterDataAfterTime(playerid);
	CallLocalFunction(#OnCharacterSpawn, "d", playerid);
	return 1;
}

// Sets Character's Spawn Point.
// spawnid:
// 1: means house (if any)
// 2: means faction (if any)
// 3: means business (if any)
// from 4 to INFINITE: means a specific spawn point.
stock Character_SetSpawnPointType(playerid, spawnid)
{
	if(spawnid < SPAWN_POINT_TYPE_NONE)
		return 0;
	SpawnPointType{playerid} = spawnid;
	mysql_tquery_f(gMySQL, "UPDATE `character_spawn_data` SET spawn_type = '%d' WHERE character_id = '%d'", spawnid, Character_GetID(playerid));
	return 1;
}

stock Character_SetSpawnPointID(playerid, extraid)
{
	SpawnPointID[playerid] = extraid;
	mysql_tquery_f(gMySQL, "UPDATE `character_spawn_data` SET spawn_id = '%d' WHERE character_id = '%d'", extraid, Character_GetID(playerid));
}

stock Character_GetSpawnPointID(playerid)
{
	return SpawnPointID[playerid];
}

stock Character_GetSpawnPointType(playerid)
{
	return SpawnPointType{playerid};
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

		cache_get_value_name_int(0, "is_first_spawn", temp);
		IsFirstSpawn{playerid} = temp ? true : false;

		cache_get_value_name_int(0, "is_spawned", temp);
		IsSpawned{playerid} = temp ? true : false;

		cache_get_value_name_float(0, "last_x", PlayerSpawnData[playerid][LastX]);
		cache_get_value_name_float(0, "last_y", PlayerSpawnData[playerid][LastY]);
		cache_get_value_name_float(0, "last_z", PlayerSpawnData[playerid][LastZ]);
		cache_get_value_name_float(0, "last_a", PlayerSpawnData[playerid][LastA]);
		cache_get_value_name_int(0, "last_interior", PlayerSpawnData[playerid][LastInterior]);
		cache_get_value_name_int(0, "last_world", PlayerSpawnData[playerid][LastVirtualWorld]);

		cache_get_value_name_float(0, "last_health", PlayerSpawnData[playerid][LastHealth]);
		cache_get_value_name_float(0, "last_armour", PlayerSpawnData[playerid][LastArmour]);
		
		new tmp = 0;
		cache_get_value_name_int(0, "spawn_type", tmp);
		SpawnPointType{playerid} = tmp;

		cache_get_value_name_int(0, "spawn_id", SpawnPointID[playerid]);

		cache_get_value_name_float(0, "spawn_x", PlayerSpawnData[playerid][SpawnX]);
		cache_get_value_name_float(0, "spawn_y", PlayerSpawnData[playerid][SpawnY]);
		cache_get_value_name_float(0, "spawn_z", PlayerSpawnData[playerid][SpawnZ]);
		cache_get_value_name_float(0, "spawn_a", PlayerSpawnData[playerid][SpawnA]);
		cache_get_value_name_int(0, "spawn_interior", PlayerSpawnData[playerid][SpawnInterior]);
		cache_get_value_name_int(0, "spawn_world", PlayerSpawnData[playerid][SpawnVirtualWorld]);
	
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `character_spawn_data` WHERE character_id = '%d'", Character_GetID(playerid));
	return 1;
}

timer LoadCharacterDataAfterTime[2000](playerid)
{
	CallLocalFunction("OnCharacterLateLoad", "d", playerid);
}
