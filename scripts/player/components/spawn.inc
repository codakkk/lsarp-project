#include <YSI_Coding\y_hooks>

enum
{
	SPAWN_POINT_TYPE_NONE = 0,
	SPAWN_POINT_TYPE_HOUSE = 1,
	SPAWN_POINT_TYPE_FACTION = 2,
	SPAWN_POINT_TYPE_BUSINESS = 3
}

/*enum e_SPAWN_POINT
{
	spType,
	spID
}

static 
	SpawnPoint[MAX_PLAYERS][e_SPAWN_POINT];*/// = {SPAWN_POINT_TYPE_NONE, ...};

static 
	SpawnPoint[MAX_PLAYERS char] = {SPAWN_POINT_TYPE_NONE, ...},
	SpawnPointExtraID[MAX_PLAYERS] = {-1, ...};

hook OnPlayerClearData(playerid)
{
	SpawnPoint{playerid} = SPAWN_POINT_TYPE_NONE;
	SpawnPointExtraID[playerid] = -1;
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
	PlayerRestore[playerid][pFirstSpawn] = 0;

	Character_SetSkin(playerid, 46);

	SetPlayerPos(playerid, 1748.1887, -1860.0414, 13.5792);

	if(AccountInfo[playerid][aCharactersCount] <= 2)
	{
		static const characterFirstLoginMoney[] = {30000, 20000, 10000};
		Character_GiveMoney(playerid, characterFirstLoginMoney[AccountInfo[playerid][aCharactersCount]], "FirstSpawn");
		SendFormattedMessage(playerid, -1, "(( Ti sono stati dati {85bb65}$%d{FFFFFF} per cominciare. ))", characterFirstLoginMoney[AccountInfo[playerid][aCharactersCount]]);
	}
	
	new query[128];
	mysql_format(gMySQL, query, sizeof(query), "UPDATE `characters` SET FirstSpawn = '0' WHERE ID = '%d'", PlayerInfo[playerid][pID]);
	mysql_tquery(gMySQL, query);

	AccountInfo[playerid][aCharactersCount]++;
	
	mysql_format(gMySQL, query, sizeof(query), "UPDATE `accounts` SET CharactersCounter = '%d' WHERE ID = '%d'", AccountInfo[playerid][aCharactersCount], AccountInfo[playerid][aID]);
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
		if(!Account_IsAdminDuty(playerid) && !strcmp(AccountInfo[playerid][aName], "Coda", false))
		{
			pc_cmd_aduty(playerid, "");
		}
	#endif
	Character_ResetDeathState(playerid);
	AC_SetPlayerHealth(playerid, 100.0);
	AC_SetPlayerArmour(playerid, 0.0);

	Character_FreezeForTime(playerid, 2000);

	if(PlayerRestore[playerid][pFirstSpawn]) // First Login/Spawn
    {
		Character_HandleFirstSpawn(playerid);
    }
	else if(Character_IsJailed(playerid))
	{
		Character_SetToJailPos(playerid);
		PlayerTextDrawShow(playerid, pJailTimeText[playerid]);
	}
    else if(PlayerRestore[playerid][pSpawned] && PlayerRestore[playerid][pLastX] != 0 && PlayerRestore[playerid][pLastY] != 0 && PlayerRestore[playerid][pLastZ] != 0)
    {
		PlayerRestore[playerid][pSpawned] = 0;
		SetPlayerPos(playerid, PlayerRestore[playerid][pLastX], PlayerRestore[playerid][pLastY], PlayerRestore[playerid][pLastZ]);
		SetPlayerFacingAngle(playerid, PlayerRestore[playerid][pLastAngle]);
		SetPlayerInterior(playerid, PlayerRestore[playerid][pLastInterior]);
		Character_SetVirtualWorld(playerid, PlayerRestore[playerid][pLastVirtualWorld]);
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
			SetPlayerPos(playerid, 1723.3232, -1867.1775, 13.5705);
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