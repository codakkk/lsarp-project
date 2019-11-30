
#if defined _included_PLAYER
    #endinput
#endif 

#define _included_PLAYER 

#include <house_system\core>

#include <player_system\enum>

#include <player_system\textdraws>
#include <player_system\state>

#include <player_system\components\request>

#include <player_system\components\money>
#include <player_system\components\fightstyle>
#include <player_system\components\skin>
#include <player_system\components\walk>
#include <player_system\components\chat>
#include <player_system\components\paycheck>
#include <player_system\components\editobject>
#include <player_system\components\death_system>
#include <player_system\components\death_history>
#include <player_system\components\inventory>
#include <player_system\components\drop>
#include <player_system\components\property>
#include <player_system\components\payday>
#include <player_system\components\interaction>
#include <player_system\components\vehicle>
#include <player_system\components\weapons>
#include <player_system\components\pickup>
#include <player_system\components\jail>
#include <player_system\components\animation>
#include <player_system\components\care>
#include <player_system\components\options>
#include <player_system\components\afk>
#include <player_system\components\damage_system>
#include <player_system\components\customanims>
#include <player_system\components\drugs>
#include <player_system\components\upgrade>
#include <player_system\components\notifications>
//#include <player_system\components\hunger>
#include <player_system\components\desync_checker>
#include <player_system\components\wear_objects>
#include <player_system\components\privileges>
#include <player_system\components\radio>
#include <player_system\components\help_requests>
#include <player_system\components\phone_system\core>
#include <player_system\components\licenses>

// EFFECTS 
#include <player_system\components\effects\mask>
#include <player_system\components\effects\drag>
#include <player_system\components\effects\stun>
#include <player_system\components\effects\ame>
#include <player_system\components\effects\blindfold>
#include <player_system\components\effects\freeze>
#include <player_system\components\effects\smoking>
#include <player_system\components\effects\invincible>

#include <player_system\components\spawn>

#include <player_system\components\commands\core>

// OTHERS

#include <house_system\core>

#include <YSI_Coding\y_hooks>

forward OnCharacterLoad(playerid); // Called when a Player select a character.
forward OnCharacterLoaded(playerid); // Called just after each OnCharacterLoad are called.
forward OnCharacterLateLoad(playerid); // Called after 1 second OnCharacterLoad is called.

forward OnCharacterClearData(playerid); // Callbacks called when character's data (or account data) must/can be cleared.

forward OnCharacterDisconnected(playerid, reason);



static enum E_PLAYER_DATA
{
    ID,
	Level,
	Experience,
	
	MinutesPlayed,

    Float:Health,
    Float:Armour,

	Faction,
	Rank,

    LootZone, // MUST REMOVE FROM HERE SOON
};
static
	CharacterInfo[MAX_PLAYERS][E_PLAYER_DATA],
	CharacterName[MAX_PLAYERS][MAX_PLAYER_NAME]
;

forward OnPlayerVirtualWorldChange(playerid, oldWorld, newWorld);

hook OnGameModeInit()
{
	mysql_pquery_f(gMySQL, "UPDATE `characters` SET is_online = '0' WHERE 1");
	return 1;
}

hook OnGameModeExit()
{
	mysql_pquery_f(gMySQL, "UPDATE `characters` SET is_online = '0' WHERE 1");
	return 1;
}

hook OnCharacterClearData(playerid)
{
	Character_EnableSomeAntiCheats(playerid, 1);
    Character_Clear(playerid);
	Character_ResetBitState(playerid);
    return 1;
}

hook OnCharacterDisconnected(playerid, reason)
{
	mysql_pquery_f(gMySQL, "UPDATE `characters` SET is_online = '0' WHERE id = '%d'", Character_GetID(playerid));
	return 1;
}

hook OnCharacterLoad(playerid)
{
	mysql_pquery_f(gMySQL, "UPDATE `characters` SET is_online = '1' WHERE id = '%d'", Character_GetID(playerid));
	return 1;
}

// Needed for anti-tp bug that makes player being kicked random.
hook OnPlayerRequestClass(playerid, classid)
{
	//SetSpawnInfo(playerid, NO_TEAM, 0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0, 0);
	return 1;
}

stock Character_SetVirtualWorld(playerid, virtualWorld)
{
	new old = Character_GetVirtualWorld(playerid);
	SetPlayerVirtualWorld(playerid, virtualWorld);
	CallLocalFunction(#OnPlayerVirtualWorldChange, "ddd", playerid, old, virtualWorld);
	return 1;
}

stock Character_GetVirtualWorld(playerid)
{
    return GetPlayerVirtualWorld(playerid);
}

// Returns true if character isn't stunned or other things
stock Character_CanInteract(playerid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	if(Character_IsStunned(playerid))
		return 0;
	if(Character_IsCuffed(playerid))
		return 0;
	if(Character_IsDragged(playerid))
		return 0;
	if(Character_IsDead(playerid))
		return 0;
	if(Character_IsFreezed(playerid))
		return 0;
	return 1;
}

stock Character_ShowStats(playerid, targetid)
{
	if(!Character_IsLogged(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");

	new year, month, day, hours, minutes, seconds,
		Float:x, Float:y, Float:z;

	GetPlayerPos(playerid, x, y, z);
	
	new factionName[MAX_FACTION_NAME] = "N/A", rankName[MAX_FACTION_RANK_NAME] = "N/A", jobName[32] = "";

    if(Faction_IsValid(Character_GetFaction(playerid)))
	{
		Faction_GetName(Character_GetFaction(playerid), factionName);
		Faction_GetRankName(Character_GetFaction(playerid), Character_GetRank(playerid), rankName);
	}

	Job_GetName(Character_GetJobType(playerid), jobName);

	new timePlayed[32];

	new minutesPlayed = Character_GetMinutesPlayed(playerid);

	if(minutesPlayed > 60)
	{
		new hoursPlayed = minutesPlayed/60;
		new tempMins = minutesPlayed - (hoursPlayed * 60);
		format(timePlayed, sizeof(timePlayed), "%d ore e %d min.", hoursPlayed, tempMins);
	}
	else
	{
		format(timePlayed, sizeof(timePlayed), "%d min.", minutesPlayed);
	}

	new String:string = str_format("[Account] %s | Admin Level: [%d] | Admin Role: [%d] | E-Mail: %s ", Account_GetName(playerid), Account_GetAdminLevel(playerid), Account_GetAdminRole(playerid), Account_GetEMail(playerid));

	new expire = Account_GetPremiumExpiry(playerid);
	if(expire > 0)
	{
		TimestampToDate(expire, year, month, day, hours, minutes, seconds, 1);
		str_append_format(string, "| Premium Level: [%d] | Scadenza: [%02d/%02d/%02d %02d:%02d]", Account_GetPremiumLevel(playerid), day, month, year, hours, minutes);
	}
	getdate(year, month, day);
	gettime(hours, minutes, seconds);
    SendFormattedMessage(targetid, 0x33AA33FF, "____________________________________[Stats Help - %02d/%02d/%02d @ %02d:%02d:%02d]____________________________________", day, month, year, hours, minutes, seconds);
	SendClientMessageStr(targetid, 0xCCE6E6FF, string);
	SendFormattedMessage(targetid, 0xCCE6E6FF, "[Personaggio] Livello: [%d] | Esperienza: [%d] | Lavoro: [%s] | Fazione: [%s] | Rank: [%s]", Character_GetLevel(playerid), Character_GetExperience(playerid), jobName, factionName, rankName);
    SendFormattedMessage(targetid, 0xCCE6E6FF, "[Personaggio] HP: [%.2f] | Armatura: [%.2f] | Max HP: [%.2f] | Max AP: [%.2f] | Ore di gioco: [%s]", Character_GetHealth(playerid), Character_GetArmour(playerid), Character_GetMaxHealth(playerid), Character_GetMaxArmour(playerid), timePlayed);
	SendFormattedMessage(targetid, 0xCCE6E6FF, "[Personaggio] Soldi: [$%s] | Banca: [$%s] | Stipendio: [$%s]", FormatNumber(Character_GetMoney(playerid)), FormatNumber(Character_GetBank(playerid)), FormatNumber(Character_GetPayCheck(playerid)));
    SendFormattedMessage(targetid, 0xCCE6E6FF, "[Altro] X: [%.2f] | Y: [%.2f] | Z: [%.2f] | Interior: [%d] | VW: [%d]", x, y, z, GetPlayerInterior(playerid), Character_GetVirtualWorld(playerid));
    SendClientMessage(targetid, 0x33AA33FF,    "_______________________________________________________________________________________________________");
    return 1;
}

stock Character_GetOOCName(playerid)
{
    return CharacterName[playerid];
}

stock String:Character_GetOOCNameStr(playerid)
{
    return @(Character_GetOOCName(playerid));
}

// Gets current player name
// Eg: if player has mask, returns something like "Sconosciuto"
stock Character_GetRolePlayName(playerid)
{
    // Is it necessary? I don't think so
    //if(!Character_IsLogged(playerid))
	   //return "";

    new 
	   string[40];
    if(Character_IsMasked(playerid))
    {
		format(string, sizeof(string), "Sconosciuto #%07d", Character_GetID(playerid));
    }
    else
    {
    	FixName(CharacterName[playerid], string);
    }
    return string;
}

stock Character_GetID(playerid)
{
	if(!IsPlayerConnected(playerid))
		return 0;
    return CharacterInfo[playerid][ID];
}

stock Character_GetFaction(playerid)
{
	return CharacterInfo[playerid][Faction];
}

stock Character_SetFaction(playerid, factionid)
{
	CharacterInfo[playerid][Faction] = factionid;
	mysql_pquery_f(gMySQL, "UPDATE `characters` SET faction = '%d' WHERE id = '%d'", factionid, Character_GetID(playerid));
}

stock Character_GetRank(playerid)
{
	return CharacterInfo[playerid][Rank];
}

stock Character_SetRank(playerid, rank)
{
	CharacterInfo[playerid][Rank] = rank;
	mysql_pquery_f(gMySQL, "UPDATE `characters` SET faction_rank = '%d' WHERE id = '%d'", rank, Character_GetID(playerid));
}

stock bool:Character_TryGetHouse(playerid, &House:house)
{
	house = House_Find(Character_GetHouseID(playerid));
	if(!House_IsValid(house))
	{
		new checkpointid = Character_GetCheckpoint(playerid);
		Checkpoint_GetInfoOfType(checkpointid, CHECKPOINT_TYPE_HOUSE, house);
	}
	return House_IsValid(house);
}

stock bool:Character_IsInsideHouse(playerid, House:house)
{
	return (Character_GetVirtualWorld(playerid) - HOUSE_START_WORLD) == House_GetID(house);
}

stock bool:Character_IsNearHouseEnter(playerid, House:house)
{
	new Float:x, Float:y, Float:z;
	House_GetEnterPosition(house, x, y, z);
	return IsPlayerInRangeOfPoint(playerid, 1.5, x, y, z) > 0;
}

stock bool:Character_IsNearHouseExit(playerid, House:house)
{
	new Float:x, Float:y, Float:z;
	House_GetExitPosition(house, x, y, z);
	return IsPlayerInRangeOfPoint(playerid, 1.5, x, y, z) > 0;
}

stock bool:Character_CanUseHouseMenu(playerid, House:house, bool:onlyOwner = false)
{
	if(!House_IsValid(house))
		return false;
	if(Player_IsAdminDuty(playerid))
		return true;
    if(!onlyOwner && Character_GetRentHouse(playerid) == house)
        return true;
    if(Character_DoesOwnHouse(playerid, house))
        return true;
	return false;
}

stock Character_GetHouseID(playerid)
{
	if(Character_GetVirtualWorld(playerid) < HOUSE_START_WORLD)
		return INVALID_HOUSE_ID;
	return Character_GetVirtualWorld(playerid) - HOUSE_START_WORLD;
}

stock LoadCharacterResult(playerid)
{
    new count, tmp;
    cache_get_row_count(count);
    if(count > 0)
    {
		cache_get_value_name_int(0, "id", CharacterInfo[playerid][ID]);
		cache_get_value_name(0, "name", CharacterName[playerid]);
		
		cache_get_value_name_int(0, "minutes_played", CharacterInfo[playerid][MinutesPlayed]);

		cache_get_value_name_int(0, "faction", CharacterInfo[playerid][Faction]);
		cache_get_value_name_int(0, "faction_rank", CharacterInfo[playerid][Rank]);
		
		if(Faction_IsValid(Character_GetFaction(playerid)))
		{
			cache_get_value_name_int(0, "faction_duty", tmp);
			Character_SetFactionDuty(playerid, tmp ? true : false);
			SetPlayerColor(playerid, Faction_GetColor(Character_GetFaction(playerid)));
		}
		else
		{
			Character_SetFactionDuty(playerid, false);
		}

		new isBanned, banTime;
		cache_get_value_name_int(0, "banned", isBanned);
		cache_get_value_name_int(0, "ban_expiry", banTime);

		cache_get_value_name_int(0, "level", CharacterInfo[playerid][Level]);
		cache_get_value_name_int(0, "experience", CharacterInfo[playerid][Experience]);

		SetPlayerScore(playerid, Character_GetLevel(playerid));
		if(isBanned == 2) // Is Freezed?
		{
			SendClientMessage(playerid, COLOR_ORANGE, "SERVER: Questo personaggio risulta freezato. Visita l'UCP per sfreezarlo.");
			KickEx(playerid);
			return 0;
		}
		else if(isBanned == 1)
		{
			if(gettime() < banTime)
			{
				SendFormattedMessage(playerid, COLOR_ORANGE, "SERVER: QUesto personaggio risulta bannato fino al %s.", date(banTime));
				SendClientMessage(playerid, COLOR_ORANGE, "SERVER: Se pensi sia stato un errore, visita www.lv-rp.it/.");
				KickEx(playerid);
				return 0;
			}
			else
			{
				mysql_pquery_f(gMySQL, "UPDATE `characters` SET banned = '0', ban_expiry = '0' WHERE id = '%d'", Character_GetID(playerid));
			}
		}

		new bool:isValidated = false;
		cache_is_value_name_null(0, "validated_at", isValidated);
		isValidated = !isValidated;
		if(!isValidated)
		{
			SendClientMessage(playerid, COLOR_ORANGE, "SERVER: Questo personaggio non è stato ancora convalidato da un amministratore.");
			KickEx(playerid);
			return 0;
		}
		return 1;
    }
    return 0;
}

stock Character_SetSafePosition(playerid, Float:x, Float:y, Float:z, interior = -1, world = -1)
{
	if(!Character_IsLogged(playerid))
		return 0;

	if(interior == -1)
		interior = GetPlayerInterior(playerid);
	if(world == -1)
		world = Character_GetVirtualWorld(playerid);
	
	// Just a workaround for teleport hack
	SetSpawnInfo(playerid, 0, GetPlayerSkin(playerid), x, y, z, 0.0, 0, 0, 0, 0, 0, 0);

	SetPlayerInterior(playerid, interior);
	Character_SetVirtualWorld(playerid, world);
	Streamer_UpdateEx(playerid, x, y, z, world, interior, 2000, 1);
	return 1;
}

stock Character_Clear(playerid)
{
    new CleanData[E_PLAYER_DATA];
    CharacterInfo[playerid] = CleanData;
}

stock Character_LoadData(playerid)
{
    if(LoadCharacterResult(playerid))
	{
		Character_SetLogged(playerid, true);

		CancelSelectTextDraw(playerid);

		SetPlayerName(playerid, CharacterName[playerid]);
		
		CallLocalFunction(#OnCharacterLoad, "i", playerid);

		CallLocalFunction(#OnCharacterLoaded, "i", playerid);
		//TogglePlayerSpectating(playerid, 0);
	}
	else return KickEx(playerid);
	return 1;
}

stock Character_Delete(playerid, character_db_id, character_name[])
{
    #pragma unused playerid
    new query[256];
    mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `characters` WHERE id = '%d' AND LOWER(name) = LOWER('%e')", character_db_id, character_name);
    mysql_tquery(gMySQL, query);

    mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `character_inventory` WHERE character_id = '%d'", character_db_id);
    mysql_tquery(gMySQL, query);

	format(query, sizeof(query), "DELETE FROM `character_weapons` WHERE character_id = '%d'", character_db_id);
	mysql_pquery(gMySQL, query);

    inline OnVehicles()
    {
		new count = cache_num_rows(), vid;
		for(new i = 0; i < count; ++i)
		{
			cache_get_value_index_int(i, 0, vid);
			mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `vehicle_inventory` WHERE vehicle_id = '%d'", vid);
			mysql_tquery(gMySQL, query);
		}
		mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `vehicles` WHERE owner_id = '%d'", character_db_id);
		mysql_tquery(gMySQL, query);
    }
    MySQL_TQueryInline(gMySQL, using inline OnVehicles, "SELECT id FROM `vehicles` WHERE owner_id = '%d'", character_db_id);

	for_building(building)
	{
		if(Building_GetOwnerID(building) == Character_GetID(playerid))
		{
			Building_ResetOwner(building);
		}
	}

	for_house(house)
	{
		new ownerid = House_GetOwnerID(house);
		if(character_db_id == ownerid)
		{
			House_ResetOwner(house);
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
	static const cheatCodes[] = {0, 1, 2, 3, 5, 6, 7, 8, 9, 10, 19, 20};
	for(new i = 0, j = sizeof(cheatCodes); i < j; ++i)
		EnableAntiCheatForPlayer(playerid, cheatCodes[i], enabled);
}

stock Character_IsIDConnected(dbid)
{
	new playerid = INVALID_PLAYER_ID;
	foreach(new i : Player)
	{
		if(Character_GetID(i) == dbid)
		{
			playerid = i;
			break;
		}
	}
	return playerid;
}

stock Character_SetLevel(playerid, level)
{
	CharacterInfo[playerid][Level] = level;
	SetPlayerScore(playerid, level);
	mysql_pquery_f(gMySQL, "UPDATE `characters` SET level = '%d' WHERE id = '%d'", level, Character_GetID(playerid));
}

stock Character_IncreaseLevel(playerid, amount)
{
	new newLevel = CharacterInfo[playerid][Level] + amount;
	Character_SetLevel(playerid, newLevel);
}

stock Character_GetLevel(playerid)
{
	return CharacterInfo[playerid][Level];
}

stock Character_SetExperience(playerid, exp)
{
	CharacterInfo[playerid][Experience] = exp;
	mysql_pquery_f(gMySQL, "UPDATE `characters` SET experience = '%d' WHERE id = '%d'", exp, Character_GetID(playerid));
}

stock Character_AddExperience(playerid, amount)
{
	new newExp = Character_GetExperience(playerid) + amount;
	Character_SetExperience(playerid, newExp);
}

stock Character_GetExperience(playerid)
{
	return CharacterInfo[playerid][Experience];
}

stock Character_SetHealth(playerid, Float:health)
{
	SetPlayerHealth(playerid, health);
	return 1;
}

stock Float:Character_GetHealth(playerid)
{
	new Float:health;
	GetPlayerHealth(playerid, health);
	return health;
}

stock Character_SetArmour(playerid, Float:armour)
{
	SetPlayerArmour(playerid, armour);
	return 1;
}

stock Float:Character_GetArmour(playerid)
{
	new Float:armour;
	GetPlayerArmour(playerid, armour);
	return armour;
}

stock Character_AddMinutesPlayed(playerid, add)
{
	CharacterInfo[playerid][MinutesPlayed] += add;
	mysql_pquery_f(gMySQL, "UPDATE `characters` SET minutes_played = '%d' WHERE id = '%d'", Character_GetMinutesPlayed(playerid), Character_GetID(playerid));
	return 1;
}

stock Character_GetMinutesPlayed(playerid)
{
	return CharacterInfo[playerid][MinutesPlayed];
}

hook function bool:Entity_GetWorldPosition(EntityType:entityType, entityId, &Float:x, &Float:y, &Float:z)
{
	if(entityType != ENTITY_TYPE_PLAYER)
		return bool:continue(_:entityType, entityId, x, y, z);
	
	new playerid = entityId;

	if(!Character_IsLogged(playerid))
		return false;

	x = y = z = 0.0;

	new world = Character_GetVirtualWorld(playerid);
    if(world < HOUSE_START_WORLD)
    {
        GetPlayerPos(playerid, x, y, z);
    }
    else if(world < BUILDING_START_WORLD)
    {
        new House:house = House_Find(Character_GetHouseID(playerid));
        if(House_IsValid(house))
        {
        	House_GetEnterPosition(house, x, y, z);
		} 
    }
    else if(world >= BUILDING_START_WORLD)
    {
        new Building:building;
        if(Character_TryGetBuilding(playerid, building))
        {
        	Building_GetEnterPos(building, x, y, z);
		}
    }
	return x != 0.0 && y != 0.0 && z != 0.0;
}