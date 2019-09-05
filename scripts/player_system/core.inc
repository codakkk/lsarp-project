
#if defined _included_PLAYER
    #endinput
#endif 

#define _included_PLAYER 

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
#include <player_system\components\payday>
#include <player_system\components\interaction>
#include <player_system\components\vehicle>
#include <player_system\components\weapons>
#include <player_system\components\pickup>
#include <player_system\components\jail>
#include <player_system\components\animation>
#include <player_system\components\property>
#include <player_system\components\care>
#include <player_system\components\options>
#include <player_system\components\afk>
#include <player_system\components\damage_system>
#include <player_system\components\customanims>
#include <player_system\components\drugs>
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

#include <player_system\commands>
// OTHERS

#include <house_system\core>

#include <YSI_Coding\y_hooks>

forward OnCharacterLoad(playerid); // Called when a Player select a character.
forward OnCharacterLateLoad(playerid);

forward OnPlayerClearData(playerid); // Callbacks called when character's data (or account data) must/can be cleared.

forward OnCharacterDisconnected(playerid);

forward OnCharacterSelected(playerid); // Called when a player select a character during login.
forward OnCharacterSpawn(playerid); // Called when Character_Spawn is called.

static enum E_PLAYER_DATA
{
    ID,
	
    Float:Health,
    Float:Armour,

	Faction,
	Rank,

    LootZone, // MUST REMOVE FROM HERE SOON
};
static
	PlayerInfo[MAX_PLAYERS][E_PLAYER_DATA],
	CharacterName[MAX_PLAYERS][MAX_PLAYER_NAME]
;

forward OnPlayerVirtualWorldChange(playerid, oldWorld, newWorld);

hook OnPlayerClearData(playerid)
{
	Character_EnableSomeAntiCheats(playerid, 1);
    Character_Clear(playerid);
	Character_ResetBitState(playerid);
    return 1;
}

// Needed for anti-tp bug that makes player being kicked random.
hook OnPlayerRequestClass(playerid, classid)
{
	SetSpawnInfo(playerid, NO_TEAM, 0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0, 0);
	return 1;
}

hook OnCharacterLoad(playerid)
{
	SetPlayerName(playerid, CharacterName[playerid]);
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
	return 1;
}

stock Character_ShowStats(playerid, targetid)
{
	if(!Character_IsLogged(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");
	new
		Float:hp, Float:armour;
	
    AC_GetPlayerHealth(playerid, hp);
    AC_GetPlayerArmour(playerid, armour);

	new year, month, day, hours, minutes, seconds;

	getdate(year, month, day);
	gettime(hours, minutes, seconds);
	
    SendFormattedMessage(targetid, COLOR_GREEN, "_____________________[STATISTICHE - %d/%d/%d @ %d:%d]_____________________", day, month, year, hours, minutes);
	SendFormattedMessage(targetid, -1, "[Account] Nome: %s - Livello: %d - E-Mail: %s", Account_GetName(playerid), Account_GetLevel(playerid), Account_GetEMail(playerid));
    
	if(Faction_IsValid(Character_GetFaction(playerid)))
	{
		new factionName[MAX_FACTION_NAME], rankName[MAX_FACTION_RANK_NAME];
		Faction_GetName(Character_GetFaction(playerid), factionName);
		Faction_GetRankName(Character_GetFaction(playerid), Character_GetRank(playerid), rankName);
		SendFormattedMessage(targetid, -1, "[Personaggio] Nome: %s - Skin: %d - Fazione: %s - Rank: %s", Character_GetOOCName(playerid), Character_GetSkin(playerid), factionName, rankName);
	}
	else
	{
		SendFormattedMessage(targetid, -1, "[Personaggio] Nome: %s - Skin: %d", Character_GetOOCName(playerid), Character_GetSkin(playerid));
	}
    SendFormattedMessage(targetid, -1, "[Salute] HP: %.2f - Armatura: %.2f", hp, armour);
    
	if(Character_HasBuildingKey(playerid) && Character_HasHouseKey(playerid))
	   SendFormattedMessage(targetid, -1, "[Proprietà] Edificio: %d - Casa: %d", Character_GetBuildingKey(playerid), Character_GetHouseKey(playerid));
    else if(Character_HasBuildingKey(playerid))
	   SendFormattedMessage(targetid, -1, "[Proprietà] Edificio: %d", Character_GetBuildingKey(playerid));
    else if(Character_HasHouseKey(playerid))
	   SendFormattedMessage(targetid, -1, "[Proprietà] Casa: %d", Character_GetHouseKey(playerid));
	
	SendFormattedMessage(targetid, -1, "[Altro] Soldi: $%d - Banca: $%d - Stipendio: $%d - Interior: %d - VW: %d", Character_GetMoney(playerid), Character_GetBank(playerid), Character_GetPayCheck(playerid), GetPlayerInterior(playerid), Character_GetVirtualWorld(playerid));
	
	if(Account_GetPremiumLevel(playerid) > 0 && Account_GetPremiumExpiry(playerid) > gettime())
	{
		static premiumNames[][16] = {"Nerone", "Caligola", "Giulio Cesare"};
		TimestampToDate(Account_GetPremiumExpiry(playerid), year, month, day, hours, minutes, seconds, 1);

		SendFormattedMessage(targetid, -1, "Premium %s - Scadenza: %d/%d/%d - %d:%d", premiumNames[Account_GetPremiumLevel(playerid)-1], day, month, year, hours, minutes);
	}
	
	if(Account_GetAdminLevel(playerid) > 0)
		SendFormattedMessage(targetid, -1, "[Staff] %s", GetAdminLevelName(Account_GetAdminLevel(playerid)));

	//SendFormattedMessage(targetid, -1, "Ti mancano %d/%d punti esperienza per salire di livello.", Character_GetExp(playerid), expForNewLevel);
    SendFormattedMessage(targetid, -1, "Tempo rimanente al PayDay: %d minuti", 60 - Account_GetPayDay(playerid));
    //SendClientMessage(targetid, COLOR_GREEN, "_______________________________________________________");
    return 1;
}

stock Character_GetOOCName(playerid)
{
    new name[24];
    FixName(CharacterName[playerid], name);
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
	if(!( 0 <= playerid < INVALID_PLAYER_ID))
		return 0;
    return PlayerInfo[playerid][ID];
}

stock Character_GetFaction(playerid)
{
	return PlayerInfo[playerid][Faction];
}

stock Character_SetFaction(playerid, factionid)
{
	PlayerInfo[playerid][Faction] = factionid;
	mysql_tquery_f(gMySQL, "UPDATE `characters` SET faction = '%d' WHERE id = '%d'", factionid, Character_GetID(playerid));
}

stock Character_GetRank(playerid)
{
	return PlayerInfo[playerid][Rank];
}

stock Character_SetRank(playerid, rank)
{
	PlayerInfo[playerid][Rank] = rank;
	mysql_tquery_f(gMySQL, "UPDATE `characters` SET faction_rank = '%d' WHERE id = '%d'", rank, Character_GetID(playerid));
}

// Returns a PP List with all house ids owned (not affittate) by playerid.
// Remember to delete List with list_delete/list_delete_deep
stock List:Character_GetOwnedHouses(playerid)
{
	new List:list = list_new();
	for_map(i : GetHousesMap())
	{
		new houseid = iter_get_key(i);
		if(House_GetOwnerID(houseid) == Character_GetID(playerid))
			list_add(list, houseid);
	}
	return list;
}

stock Character_GetNearHouseID(playerid, bool:inside = false)
{
	if(inside)
	{
		return Character_GetHouseID(playerid);
	}
	else
	{
		new id, E_ELEMENT_TYPE:type;
		if(Character_GetNearestPickup(playerid, id, type) && (type == ELEMENT_TYPE_HOUSE_ENTRANCE || type == ELEMENT_TYPE_HOUSE_EXIT))
			return id;
	}
	return INVALID_HOUSE_ID;
}

stock Character_GetNearHouseIDMenu(playerid)
{
	new houseid = Character_GetNearHouseID(playerid, true);
	if(!House_IsValid(houseid))
		houseid = Character_GetNearHouseID(playerid, false);
	if(House_IsValid(houseid))
	{
		if(Player_IsAdminDuty(playerid))
			return houseid;
		if(House_GetOwnerID(houseid) == Character_GetID(playerid))
			return houseid;
		if(House_GetID(houseid) == Character_GetHouseKey(playerid))
			return houseid;
	}
	return INVALID_HOUSE_ID;
}

stock Character_GetHouseID(playerid)
{
	if(Character_GetVirtualWorld(playerid) < HOUSE_START_WORLD)
		return INVALID_HOUSE_ID;
	return Character_GetVirtualWorld(playerid) - HOUSE_START_WORLD;
}

stock Character_SetToJailPos(playerid)
{
	if(!Character_IsJailed(playerid))
		return 0;
	
	PlayerTextDrawShow(playerid, JailTimePlayerText[playerid]);
	
	if(Character_IsICJailed(playerid))
	{
		SetPlayerInterior(playerid, IC_JAIL_INT);
		Character_SetVirtualWorld(playerid, IC_JAIL_VW);
		SetPlayerPos(playerid, IC_JAIL_X, IC_JAIL_Y, IC_JAIL_Z);
		return 1;
	}
	
	Character_SetVirtualWorld(playerid, 255+playerid);
	SetPlayerInterior(playerid, OOC_JAIL_INT);
	return SetPlayerPos(playerid, OOC_JAIL_X, OOC_JAIL_Y, OOC_JAIL_Z);
}

stock LoadCharacterResult(playerid)
{
    new count, tmp;
    cache_get_row_count(count);
    if(count > 0)
    {
		cache_get_value_name_int(0, "id", PlayerInfo[playerid][ID]);
		cache_get_value_name(0, "name", CharacterName[playerid]);

		cache_get_value_name_int(0, "faction", PlayerInfo[playerid][Faction]);
		cache_get_value_name_int(0, "faction_rank", PlayerInfo[playerid][Rank]);
		
		cache_get_value_name_int(0, "faction_duty", tmp);
		Character_SetFactionDuty(playerid, tmp ? true : false);

		new isBanned, banTime;
		cache_get_value_name_int(0, "banned", isBanned);
		cache_get_value_name_int(0, "ban_expiry", banTime);

		if(isBanned)
		{
			if(gettime() < banTime)
			{
				Dialog_Show(playerid, DialogNull, DIALOG_STYLE_MSGBOX, "Personaggio Bannato", "Questo personaggio risulta bannato.\nData di scadenza: %s.\nSe pensi sia stato un errore, visita www.lsarp.it", "Ok", "", date(banTime));
				KickEx(playerid);
				return 0;
			}
			else
			{
				mysql_tquery_f(gMySQL, "UPDATE `characters` SET banned = '0', ban_expiry = '0' WHERE id = '%d'", Character_GetID(playerid));
			}
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
	
	Streamer_UpdateEx(playerid, x, y, z, world, interior);
	SetPlayerPos(playerid, x, y, z);
	SetPlayerInterior(playerid, interior);
	Character_SetVirtualWorld(playerid, world);
	Character_FreezeForTime(playerid, 2000);
	return 1;
}

stock Character_Clear(playerid)
{
    new CleanData[E_PLAYER_DATA];
    PlayerInfo[playerid] = CleanData;
}

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