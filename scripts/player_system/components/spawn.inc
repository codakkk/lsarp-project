#include <YSI_Coding\y_hooks>

forward OnCharacterFirstSpawn(playerid);
forward OnCharacterSpawn(playerid); // Called when Character_Spawn is called.
forward OnCharacterSpawned(playerid); // Called when a Character is spawned (Character_Spawn is fully executed).


#define DEFAULT_SPAWN_X 		1678.2424
#define DEFAULT_SPAWN_Y 		1446.8389
#define DEFAULT_SPAWN_Z 		10.7752

enum
{
	SPAWN_POINT_TYPE_NONE = 0,
	SPAWN_POINT_TYPE_HOUSE = 1,
	SPAWN_POINT_TYPE_FACTION = 2,
	SPAWN_POINT_TYPE_BUSINESS = 3,
	SPAWN_POINT_TYPE_LAST = 4
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

hook OnCharacterClearData(playerid)
{
	IsSpawned{playerid} = IsFirstSpawn{playerid} = false;
	SpawnPointType{playerid} = SPAWN_POINT_TYPE_NONE;
	SpawnPointID[playerid] = 0;
	PlayerSpawnData[playerid] = CleanSpawnData;
	return 1;
}

hook OnCharacterDisconnected(playerid, reason)
{
	Internal_SavePos(playerid, reason == 0);
	printf("Disconnected");
	return 1;
}

hook OnCharacterLoad(playerid)
{
	Character_LoadSpawnData(playerid);
	return 1;
}

ptask SavePlayerPositionTime[10000](playerid)
{
    if(!Character_IsLogged(playerid))
	   return 0;
    Internal_SavePos(playerid);
    return 1;
}

static Internal_SavePos(playerid, bool:isCrashed = false)
{
	new 
		Float:x, Float:y, Float:z, Float:a,
		interior = GetPlayerInterior(playerid),
		world = Character_GetVirtualWorld(playerid)
	;

	new bool:isSpawned = Character_IsAlive(playerid) && (isCrashed || SpawnPointType{playerid} == SPAWN_POINT_TYPE_LAST);

	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, a);

	mysql_pquery_f(gMySQL, "UPDATE `character_spawn_data` SET \
	last_x = '%f', last_y = '%f', last_z = '%f', last_a = '%f', last_interior = '%d', last_world = '%d', \
	last_health = '%f', last_armour = '%f', \
	is_spawned = '%d' \
	WHERE \
	character_id = '%d'", 
	x, y, z, a, interior, world,
	Character_GetHealth(playerid), Character_GetArmour(playerid),
	isSpawned,
	Character_GetID(playerid));
	return 1;
}

CMD:spawnpoint(playerid, params[])
{
	new spawnType = 0, extraid;
	if(sscanf(params, "dD(-1)", spawnType, extraid) || !(SPAWN_POINT_TYPE_NONE < spawnType <= SPAWN_POINT_TYPE_LAST))
	{
		SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /spawnpoint <type>");
		SendClientMessage(playerid, COLOR_ERROR, "Tipi - 1: Casa, 2: Fazione, 3: Business, 4: Ultima posizione");
		return 1;
	}
	if(spawnType == SPAWN_POINT_TYPE_HOUSE)
	{
		if(extraid == -1)
		{
			SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /spawnpoint 1 <houseid>");
			return 1;
		}
		new House:house = House_Find(extraid);
		
		if(Character_GetRentHouse(playerid) != house && !Character_DoesOwnHouse(playerid, house))
			return SendClientMessage(playerid, COLOR_ERROR, "Non sei nè il proprietario nè un affittuario di questa casa.");
		
		Character_SetSpawnPointType(playerid, SPAWN_POINT_TYPE_HOUSE);
		Character_SetSpawnPointID(playerid, extraid);

		SendClientMessage(playerid, COLOR_GREEN, "Hai cambiato il tuo spawn point. La prossima volta respawnerai a casa tua.");
	}
	else if(spawnType == SPAWN_POINT_TYPE_BUSINESS)
	{
		if(extraid == -1)
		{
			SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /spawnpoint 3 <businessid>");
			return 1;
		}
		new Building:building = Building_Find(extraid);
		if(!Character_DoesOwnBuilding(playerid, building))
			return SendClientMessage(playerid, COLOR_ERROR, "Non possiedi questo business.");
		
		Character_SetSpawnPointType(playerid, SPAWN_POINT_TYPE_BUSINESS);
		Character_SetSpawnPointID(playerid, extraid);

		SendClientMessage(playerid, COLOR_GREEN, "Hai cambiato il tuo spawn point. La prossima volta respawnerai al tuo negozio.");
	}
	else if(spawnType == SPAWN_POINT_TYPE_FACTION)
	{
		new factionid = Character_GetFaction(playerid);
		if(!Faction_IsValid(factionid))
			return SendClientMessage(playerid, COLOR_ERROR, "Non fai parte di una fazione.");
		
		new Float:x, Float:y, Float:z;
		Faction_GetSpawnPos(factionid, x, y, z);

		if(x == 0.0 && y == 0.0 && z == 0.0)
			return SendClientMessage(playerid, COLOR_ERROR, "La tua fazione non ha ancora settato una posizione di spawn.");
		Character_SetSpawnPointType(playerid, SPAWN_POINT_TYPE_FACTION);
		SendClientMessage(playerid, COLOR_GREEN, "Hai cambiato il tuo spawn point. La prossima volta respawnerai all'HQ della tua fazione.");
	}
	else if(spawnType == SPAWN_POINT_TYPE_LAST)
	{
		Character_SetSpawnPointType(playerid, SPAWN_POINT_TYPE_LAST);
		Character_SetSpawnPointID(playerid, 0);
		SendClientMessage(playerid, COLOR_GREEN, "Hai cambiato il tuo spawn point. La prossima volta respawnerai nel posto in cui esegui il log-out.");
	}
	return 1;
}

static Character_HandleFirstSpawn(playerid)
{
	if(!IsFirstSpawn{playerid})
		return 0;

	SetCameraBehindPlayer(playerid);
	
	if(IsFirstSpawn{playerid})
	{
		IsFirstSpawn{playerid} = false;
		if(Account_GetCharactersCount(playerid) <= 2)
		{
			new count = Account_GetCharactersCount(playerid);
			static const characterFirstLoginMoney[] = {30000, 20000, 10000};
			Character_GiveMoney(playerid, characterFirstLoginMoney[count], "FirstSpawn");
			SendFormattedMessage(playerid, -1, "(( Hai ricevuto {85bb65}$%d{FFFFFF} per cominciare la tua esperienza. ))", characterFirstLoginMoney[count]);
			SendClientMessage(playerid, -1, "Utilizza ({85bb65}/inv{FFFFFF})entario per interagire con il tuo inventario.");
		}
		
		Character_SetHealth(playerid, 100.0);

		new query[128];
		mysql_format(gMySQL, query, sizeof(query), "UPDATE `character_spawn_data` SET is_first_spawn = '0' WHERE character_id = '%d'", Character_GetID(playerid));
		mysql_pquery(gMySQL, query);

		Account_AddCharactersCount(playerid, 1);

		mysql_format(gMySQL, query, sizeof(query), "UPDATE `accounts` SET characters_counter = '%d' WHERE id = '%d'", Account_GetCharactersCount(playerid), Account_GetID(playerid));
		mysql_pquery(gMySQL, query);

		CallLocalFunction(#OnCharacterFirstSpawn, "d", playerid);
	}
	return 1;
}

hook OnPlayerSpawn(playerid)
{
	if(!Character_IsLogged(playerid))
		return 1;
	if(Character_IsAlive(playerid))
	{
		// This timer is required to avoid conflicts with Nex-AC
		defer CallOnPlayerSpawnAfterDelay(playerid);
	}
	return 1;
}

static Character_Spawn(playerid)
{
	PreloadAnimations(playerid);

	CallLocalFunction(#OnCharacterSpawn, "d", playerid);

	Character_SetHealth(playerid, Character_GetMaxHealth(playerid));
	Character_SetArmour(playerid, PlayerSpawnData[playerid][LastArmour]);
	
	Character_FreezeForTime(playerid, 3000);

	SetCameraBehindPlayer(playerid);

	if(IsFirstSpawn{playerid}) // First Login/Spawn
    {
		Character_HandleFirstSpawn(playerid);
    }
	new Float:x = DEFAULT_SPAWN_X, Float:y = DEFAULT_SPAWN_Y, Float:z = DEFAULT_SPAWN_Z, Float:a = 0.0,
		world = 0, interior = 0;
	if(Character_GetSpawnPointType(playerid) == SPAWN_POINT_TYPE_LAST && Character_IsLastPositionValid(playerid))
	{
		if(IsSpawned{playerid})
		{
			IsSpawned{playerid} = false;
			Character_SetHealth(playerid, PlayerSpawnData[playerid][LastHealth]);
			
			mysql_pquery_f(gMySQL, "UPDATE `character_spawn_data` SET last_x = '0.0', last_y = '0.0', last_z = '0.0' WHERE character_id = '%d'", Character_GetID(playerid));
			x = PlayerSpawnData[playerid][LastX];
			y = PlayerSpawnData[playerid][LastY];
			z = PlayerSpawnData[playerid][LastZ];
			a = PlayerSpawnData[playerid][LastA];
			world = PlayerSpawnData[playerid][LastVirtualWorld];
			interior = PlayerSpawnData[playerid][LastInterior];
			PlayerSpawnData[playerid][LastX] = PlayerSpawnData[playerid][LastY] = PlayerSpawnData[playerid][LastZ] = 0.0;
		}
	}
	else if(Character_GetSpawnPointType(playerid) == SPAWN_POINT_TYPE_HOUSE)
	{
		new House:house = House_Find(Character_GetSpawnPointID(playerid));
		if(House_IsValid(house) && (Character_DoesOwnHouse(playerid, house) || Character_GetRentHouse(playerid) == house))
		{
			House_GetEnterPosition(house, x, y, z);
			world = House_GetEnterWorld(house);
			interior = House_GetEnterInterior(house);
		}
	}
	else if(Character_GetSpawnPointType(playerid) == SPAWN_POINT_TYPE_BUSINESS)
	{
		new Building:building = Building_Find(Character_GetSpawnPointID(playerid));
		if(Building_IsValid(building) && Character_DoesOwnBuilding(playerid, building))
		{
			Building_GetEnterPos(building, x, y, z);
			world = Building_GetEnterWorld(building);
			interior = Building_GetEnterInterior(building);
		}
	}
	else if(Character_GetSpawnPointType(playerid) == SPAWN_POINT_TYPE_FACTION)
	{
		new factionid = Character_GetFaction(playerid);
		if(Faction_IsValid(factionid) && Faction_IsValidSpawnPosition(factionid))
		{
			Faction_GetSpawnPos(factionid, x, y, z);
			world = Faction_GetSpawnWorld(factionid);
			interior = Faction_GetSpawnInterior(factionid);
		}
	}

	Character_SetSafePosition(playerid, x, y, z, interior, world);
	SetPlayerFacingAngle(playerid, a);

	CallLocalFunction(#OnCharacterSpawned, "d", playerid);
	return 1;
}

// Sets player pos to StartSpawn position.
/*static Character_ForceStartSpawnPos(playerid)
{
	Character_SetSafePosition(playerid, DEFAULT_SPAWN_X, DEFAULT_SPAWN_Y, DEFAULT_SPAWN_Z, 0, 0);
	return 1;
}*/

// Sets Character's Spawn Point.
// spawnid:
// 1: means house (if any)
// 2: means faction (if any)
// 3: means business (if any)
// from 4 to INFINITE: means a specific spawn point.
stock Character_SetSpawnPointType(playerid, spawnid)
{
	SpawnPointType{playerid} = spawnid;
	mysql_pquery_f(gMySQL, "UPDATE `character_spawn_data` SET spawn_type = '%d' WHERE character_id = '%d'", spawnid, Character_GetID(playerid));
	return 1;
}

stock Character_SetSpawnPointID(playerid, extraid)
{
	SpawnPointID[playerid] = extraid;
	mysql_pquery_f(gMySQL, "UPDATE `character_spawn_data` SET spawn_id = '%d' WHERE character_id = '%d'", extraid, Character_GetID(playerid));
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
		if(cache_num_rows() <= 0)
		{
			mysql_pquery_f(gMySQL, "INSERT INTO `character_spawn_data` (character_id, is_first_spawn, spawn_type, spawn_id, spawn_x, spawn_y, spawn_z, spawn_a, spawn_interior, spawn_world) VALUES('%d', 1, '0', '0', '%f', '%f', '%f', '0.0', '0', '0')", Character_GetID(playerid), DEFAULT_SPAWN_X, DEFAULT_SPAWN_Y, DEFAULT_SPAWN_Z);
			SetSpawnInfo(playerid, 0, Character_GetSkin(playerid), DEFAULT_SPAWN_X, DEFAULT_SPAWN_Y, DEFAULT_SPAWN_Z, 90.0, 0, 0, 0, 0, 0, 0);
			IsFirstSpawn{playerid} = true;
		}
		else
		{
			new 
				temp = 0
			;

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

			SetSpawnInfo(playerid, 0, Character_GetSkin(playerid), 0, 0, 0, 90.0, 0, 0, 0, 0, 0, 0);
		}
		TogglePlayerSpectating(playerid, false);
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `character_spawn_data` WHERE character_id = '%d'", Character_GetID(playerid));
	return 1;
}

static Character_IsLastPositionValid(playerid)
{
	return (PlayerSpawnData[playerid][LastX] != 0.0 && PlayerSpawnData[playerid][LastY] != 0.0 && PlayerSpawnData[playerid][LastZ] != 0.0);
}

timer CallOnPlayerSpawnAfterDelay[1](playerid)
{
	Character_Spawn(playerid);
	defer LoadCharacterDataAfterTime(playerid);
}

timer LoadCharacterDataAfterTime[200](playerid)
{
	CallLocalFunction("OnCharacterLateLoad", "d", playerid);
	SetCameraBehindPlayer(playerid);
}