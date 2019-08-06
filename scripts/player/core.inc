#include <player\functions>
#include <player\textdraws>

#include <player\components\money>
#include <player\components\fightstyle>
#include <player\components\skin>
#include <player\components\walk>
#include <player\components\chat>

#include <player\components\editobject>
#include <player\components\death_system>
#include <player\components\inventory>
#include <player\components\drop>
#include <player\components\payday>
#include <player\components\interaction>
#include <player\components\vehicle>
#include <player\components\weapons>
#include <player\components\pickup>
#include <player\components\jail>
#include <player\components\animation>
#include <player\components\request>
#include <player\components\property>
#include <player\components\care>
#include <player\components\options>
#include <player\components\afk>
#include <player\components\damage_system>
#include <player\components\customanims>
#include <player\components\drugs>
#include <player\components\hunger>
#include <player\components\desync_checker>
#include <player\components\wear_objects>
#include <player\components\privileges>
#include <player\components\radio>
// EFFECTS 
#include <player\components\effects\mask>
#include <player\components\effects\drag>
#include <player\components\effects\stun>
#include <player\components\effects\ame>
#include <player\components\effects\blindfold>
#include <player\components\effects\freeze>

#include <player\components\spawn>

// OTHERS
#include <YSI_Coding\y_hooks>

static enum E_PLAYER_DATA
{
    ID,

	Float:LastHealth,
	Float:LastArmour,
	
    Float:Health,
    Float:Armour,

    BuildingKey,
    HouseKey,

	Faction,
	Rank,

	PayCheck,
	
	Banned,
	BanExpiry,

	LastChopShopTime,

    LootZone, // MUST REMOVE FROM HERE SOON
};
static
	PlayerInfo[MAX_PLAYERS][E_PLAYER_DATA],
	CharacterName[MAX_PLAYERS][MAX_PLAYER_NAME]
;

forward OnCharacterLoad(playerid); // Called when a Player select a character.
forward OnCharacterLateLoad(playerid);

forward OnCharacterPreSaveData(playerid, disconnect);
forward OnCharacterSaveData(playerid);

forward OnPlayerClearData(playerid); // Callbacks called when character's data (or account data) must/can be cleared.

forward OnCharacterDisconnected(playerid);

forward OnCharacterSelected(playerid); // Called when a player select a character during login.
forward OnCharacterSpawn(playerid); // Called when Character_Spawn is called.



forward OnPlayerVirtualWorldChange(playerid, oldWorld, newWorld);

hook OnPlayerClearData(playerid)
{
	Character_EnableSomeAntiCheats(playerid, 1);
    Character_Clear(playerid);
	Character_ResetBitState(playerid);
    return 1;
}

hook OnCharacterDisconnected(playerid)
{
    Character_Save(playerid, true);
    return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnCharacterLateLoad(playerid)
{
	AC_SetPlayerHealth(playerid, PlayerInfo[playerid][LastHealth]);
	AC_SetPlayerArmour(playerid, PlayerInfo[playerid][LastArmour]);
	return 1;
}

hook OnCharacterLoad(playerid)
{
	AC_SetPlayerHealth(playerid, 100);
	SetPlayerName(playerid, CharacterName[playerid]);
	return 1;
}

ptask SaveCharacterTimer[14000](playerid)
{
    if(!Character_IsLogged(playerid))
	   return 0;
    Character_Save(playerid);
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
	return !(Character_IsStunned(playerid) || Character_IsCuffed(playerid) || Character_IsDragged(playerid) || Character_IsDead(playerid));
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
	
    SendClientMessage(targetid, COLOR_GREEN, "_____________________[STATISTICHE - %d/%d/%d @ %d/%d]_____________________", day, month, year, hours, minutes);
	SendFormattedMessage(targetid, -1, "[Account] Account: %s - Livello: %d - E-Mail: %s", Account_GetName(playerid), Account_GetLevel(playerid), Account_GetEMail(playerid));
    
	if(Character_GetFaction(playerid) != INVALID_FACTION_ID)
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
    SendFormattedMessage(targetid, -1, "[Salute] HP: %.2f - Armatura: %.2f - Alimentazione: %d", hp, armour, Character_GetHunger(playerid));
    
	if(Character_HasBuildingKey(playerid) && Character_HasHouseKey(playerid))
	   SendFormattedMessage(targetid, COLOR_YELLOW, "[Proprietà]: Edificio: %d - Casa: %d", Character_GetBuildingKey(playerid), Character_GetHouseKey(playerid));
    else if(Character_HasBuildingKey(playerid))
	   SendFormattedMessage(targetid, COLOR_YELLOW, "[Proprietà]: Edificio: %d", Character_GetBuildingKey(playerid));
    else if(Character_HasHouseKey(playerid))
	   SendFormattedMessage(targetid, COLOR_YELLOW, "[Proprietà]: Casa: %d", Character_GetHouseKey(playerid));
	
	SendFormattedMessage(targetid, -1, "[Altro] Soldi: $%d - Stipendio: $%d - Interior: %d - VW: %d", Character_GetMoney(playerid), Character_GetPayCheck(playerid), GetPlayerInterior(playerid), Character_GetVirtualWorld(playerid));
	
	if(Account_GetPremiumLevel(playerid) > 0 && Account_GetPremiumExpiry(playerid) > gettime())
	{
		static premiumNames[][8] = {"Bronze", "Silver", "Gold"};
		new year, month, day, hour, minute, second;
		TimestampToDate(Account_GetPremiumExpiry(playerid), year, month, day, hour, minute, second, 0);

		SendFormattedMessage(targetid, COLOR_YELLOW, "Premium %s - Scadenza: %d/%d/%d - %d:%d", premiumNames[Account_GetPremiumLevel(playerid)-1], day, month, year, hour, minute);
	}
	
	if(Account_GetAdminLevel(playerid) > 0)
		SendFormattedMessage(targetid, COLOR_YELLOW, "[Staff]: %s", GetAdminLevelName(Account_GetAdminLevel(playerid)));

	//SendFormattedMessage(targetid, -1, "Ti mancano %d/%d punti esperienza per salire di livello.", Character_GetExp(playerid), expForNewLevel);
    SendFormattedMessage(targetid, COLOR_YELLOW, "Tempo rimanente al PayDay: %d minuti", 60 - Account_GetPayDay(playerid));
    SendClientMessage(targetid, COLOR_GREEN, "_______________________________________________________");
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

stock Character_HasBuildingKey(playerid)
{
    return PlayerInfo[playerid][BuildingKey] > 0;
}

stock Character_GetBuildingKey(playerid)
{
    return PlayerInfo[playerid][BuildingKey];
}

stock Character_SetBuildingKey(playerid, buildingid)
{
    PlayerInfo[playerid][BuildingKey] = buildingid;
	mysql_tquery_f(gMySQL, "UPDATE `characters` SET building_key = '%d' WHERE id = '%d'", buildingid, Character_GetID(playerid));
}

stock Character_HasHouseKey(playerid)
{
    return PlayerInfo[playerid][HouseKey] > 0;
}

stock Character_GetHouseKey(playerid)
{
    return PlayerInfo[playerid][HouseKey];
}

stock Character_SetHouseKey(playerid, houseid)
{
    PlayerInfo[playerid][HouseKey] = houseid;
	mysql_tquery_f(gMySQL, "UPDATE `characters` SET house_key = '%d' WHERE id = '%d'", houseid, Character_GetID(playerid));
}

stock Character_GetID(playerid)
{
	if(!( 0 <= playerid < INVALID_PLAYER_ID))
		return 0;
    return PlayerInfo[playerid][ID];
}

stock Character_IsFreezed(playerid)
{
    return Bit_Get(gCharacterBitState[e_pFreezed], playerid);
}

stock Character_SetFreezed(playerid, bool:freeze)
{
    Bit_Set(gCharacterBitState[e_pFreezed], playerid, freeze);
    TogglePlayerControllable(playerid, !freeze);
}

stock Character_SetLogged(playerid, bool:logged)
{
	Bit_Set(gCharacterBitState[e_pCharacterLogged], playerid, logged);
}

stock Character_IsLogged(playerid)
{
	return Bit_Get(gCharacterBitState[e_pCharacterLogged], playerid);
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
	mysql_tquery_f(gMySQL, "UPDATE `characters` SET rank = '%d' WHERE id = '%d'", rank, Character_GetID(playerid));
}

stock Character_SetFactionOOCEnabled(playerid, bool:value)
{
	Bit_Set(gCharacterBitState[e_pToggleFactionOOC], playerid, !value);
}

stock Character_IsFactionOOCEnabled(playerid)
{
	return !Bit_Get(gCharacterBitState[e_pToggleFactionOOC], playerid);
}

stock Character_SetFactionDuty(playerid, bool:value)
{
	Bit_Set(gCharacterBitState[e_pFactionDuty], playerid, value);
}

stock Character_IsFactionDuty(playerid)
{
	return Bit_Get(gCharacterBitState[e_pFactionDuty], playerid);
}

stock Character_SetSelectingUniform(playerid, bool:value)
{
	Bit_Set(gCharacterBitState[e_pSelectingUniform], playerid, value);
}

stock Character_IsSelectingUniform(playerid)
{
	return Bit_Get(gCharacterBitState[e_pSelectingUniform], playerid);
}

stock Character_SetBuyingVehicle(playerid, bool:value)
{
	Bit_Set(gCharacterBitState[e_pBuyingVehicle], playerid, value);
}

stock Character_IsBuyingVehicle(playerid)
{
	return Bit_Get(gCharacterBitState[e_pBuyingVehicle], playerid);
}

stock Character_AddPayCheck(playerid, amount)
{
	PlayerInfo[playerid][PayCheck] += amount;
}

stock Character_SetPayCheck(playerid, set)
{
	PlayerInfo[playerid][PayCheck] = set;
}

stock Character_GetPayCheck(playerid)
{
	return PlayerInfo[playerid][PayCheck];
}

stock Character_SetCuffed(playerid, bool:value)
{
	new cuffed = Character_IsCuffed(playerid);
	if(value && !cuffed)
	{
		SetPlayerSpecialAction(playerid, SPECIAL_ACTION_CUFFED);
		SetPlayerAttachedObject(playerid, 4, 19418, 6, -0.011000, 0.028000, -0.022000, -15.600012, -33.699977, -81.700035, 0.891999, 1.000000, 1.168000);
	}
	else if(!value && cuffed)
	{
		SetPlayerSpecialAction(playerid, SPECIAL_ACTION_NONE);
		RemovePlayerAttachedObject(playerid, 4);
	}
	Bit_Set(gCharacterBitState[e_pCuffed], playerid, value);
}

stock Character_IsCuffed(playerid)
{
	return Bit_Get(gCharacterBitState[e_pCuffed], playerid);
}

stock Character_SetMasked(playerid, bool:masked)
{
	Bit_Set(gCharacterBitState[e_pMasked], playerid, masked);
}

stock Character_IsMasked(playerid)
{
	return Bit_Get(gCharacterBitState[e_pMasked], playerid);
}

stock Character_SetLegHit(playerid, bool:legHitted)
{
	Bit_Set(gCharacterBitState[e_pLegHit], playerid, legHitted);
}

stock Character_IsLegHitted(playerid)
{
	return Bit_Get(gCharacterBitState[e_pLegHit], playerid);
}

stock Character_SetBanned(playerid, ban)
{
	PlayerInfo[playerid][Banned] = ban;
}

stock Character_IsBanned(playerid)
{
	return PlayerInfo[playerid][Banned];
}

stock Character_SetBanExpiry(playerid, time)
{
	PlayerInfo[playerid][BanExpiry] = time;
}

stock Character_GetBanExpiry(playerid)
{
	return PlayerInfo[playerid][BanExpiry];
}

stock Character_ResetBitState(playerid)
{
	for(new i; e_Bit1_Data:i < e_Bit1_Data; i++)
	{
		Bit_Set(gCharacterBitState[e_Bit1_Data:i], playerid, false);
	}
}

stock Character_GetLastChopShopTime(playerid)
{
	return PlayerInfo[playerid][LastChopShopTime];
}

stock Character_SetLastChopShopTime(playerid, time)
{
	if(!Character_IsLogged(playerid))
		return 0;
	PlayerInfo[playerid][LastChopShopTime] = time;
	mysql_tquery_f(gMySQL, "UPDATE `characters` SET last_chop_shop_time = '%d' WHERE id = '%d'", Character_GetLastChopSopTime(playerid), Character_GetID(playerid));
	return 1;
}

stock Character_HasChopShopPrivileges(playerid)
{
	return Bit_Get(gCharacterBitState[e_pCanChopShop], playerid);
}

stock Character_SetChopShopPrivileges(playerid, bool:can)
{
	Bit_Set(gCharacterBitState[e_pCanChopShop], playerid, can);
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
		//if(Pickup_GetInfo(pickupid, id, type) && (type == ELEMENT_TYPE_HOUSE_EXIT) && IsPlayerInRangeOfPickup(playerid, pickupid, 3.0))
			//return id;
	}
	else
	{
		new id, E_ELEMENT_TYPE:type;
		if(Character_GetNearestPickup(playerid, id, type) && (type == ELEMENT_TYPE_HOUSE_ENTRANCE || type == ELEMENT_TYPE_HOUSE_EXIT))
			return id;
	}
	return -1;
}

stock Character_GetNearHouseIDMenu(playerid)
{
	new houseid = Character_GetNearHouseID(playerid, true);
	if(houseid == -1)
		houseid = Character_GetNearHouseID(playerid, false);
	if(houseid != -1)
	{
		if(Account_IsAdminDuty(playerid))
			return houseid;
		if(House_GetOwnerID(houseid) == Character_GetID(playerid))
			return houseid;
		if(House_GetID(houseid) == Character_GetHouseKey(playerid))
			return houseid;
	}
	return -1;
}

stock Character_GetHouseID(playerid)
{
	if(Character_GetVirtualWorld(playerid) < HOUSE_START_WORLD)
		return -1;
	return Character_GetVirtualWorld(playerid) - HOUSE_START_WORLD;
}

stock Character_SetToJailPos(playerid)
{
	if(!Character_IsJailed(playerid))
		return 0;
	//AC_ResetPlayerWeapons(playerid);
	if(Character_IsICJailed(playerid))
	{
		SetPlayerInterior(playerid, 2);
		Character_SetVirtualWorld(playerid, 5258);
		SetPlayerPos(playerid, IC_JAIL_X, IC_JAIL_Y, IC_JAIL_Z);
		return 1;
	}
	
	Character_SetVirtualWorld(playerid, 255+playerid);
	SetPlayerInterior(playerid, OOC_JAIL_INT);
	return SetPlayerPos(playerid, OOC_JAIL_X, OOC_JAIL_Y, OOC_JAIL_Z);
}

stock LoadCharacterResult(playerid)
{
    new count;
    cache_get_row_count(count);
    if(count > 0)
    {
		cache_get_value_index_int(0, 0, PlayerInfo[playerid][pID]);
		cache_get_value_index(0, 2, PlayerInfo[playerid][pName]);

		cache_get_value_index_int(0, 7, PlayerInfo[playerid][pBuildingKey]);
		cache_get_value_index_int(0, 8, PlayerInfo[playerid][pHouseKey]);

		cache_get_value_index_int(0, 9, PlayerInfo[playerid][pFaction]);
		cache_get_value_index_int(0, 10, PlayerInfo[playerid][pRank]);

		cache_get_value_index_int(0, 13, PlayerInfo[playerid][pPayCheck]);

		cache_get_value_index_int(0, 15, PlayerInfo[playerid][pChatStyle]);

		cache_get_value_index_int(0, 17, PlayerInfo[playerid][pBanned]);
		cache_get_value_index_int(0, 18, PlayerInfo[playerid][pBanExpiry]);

		cache_get_value_index_int(0, 19, PlayerInfo[playerid][pLastChopShopTime]);

		cache_get_value_index_int(0, 21, tmp);
		Character_SetFactionDuty(playerid, tmp ? true : false);
		return 1;
    }
    return 0;
}

stock Character_Clear(playerid)
{
    new CleanData[E_PLAYER_DATA];
    PlayerInfo[playerid] = CleanData;

	pLastPMTime[playerid] = 0;
}