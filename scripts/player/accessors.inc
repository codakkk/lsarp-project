
stock Character_SetSkin(playerid, skinid)
{
	PlayerInfo[playerid][pSkin] = skinid;
	SetPlayerSkin(playerid, skinid);
}

stock Character_GetOOCName(playerid)
{
    new name[24];
    FixName(PlayerInfo[playerid][pName], name);
    return name;
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
		format(string, sizeof(string), "Sconosciuto #%07d", PlayerInfo[playerid][pID]);
    }
    else
    {
    	FixName(PlayerInfo[playerid][pName], string);
    }
    return string;
}

stock Character_HasBuildingKey(playerid)
{
    return PlayerInfo[playerid][pBuildingKey] > 0;
}

stock Character_GetBuildingKey(playerid)
{
    return PlayerInfo[playerid][pBuildingKey];
}

stock Character_HasHouseKey(playerid)
{
    return PlayerInfo[playerid][pHouseKey] > 0;
}

stock Character_GetHouseKey(playerid)
{
    return PlayerInfo[playerid][pHouseKey];
}

stock Character_GetID(playerid)
{
    return PlayerInfo[playerid][pID];
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
	return PlayerInfo[playerid][pFaction];
}

stock Character_SetFaction(playerid, factionid)
{
	PlayerInfo[playerid][pFaction] = factionid;
}

stock Character_GetRank(playerid)
{
	return PlayerInfo[playerid][pRank];
}

stock Character_SetRank(playerid, rank)
{
	PlayerInfo[playerid][pRank] = rank;
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
	PlayerInfo[playerid][pPayCheck] += amount;
}

stock Character_SetPayCheck(playerid, set)
{
	PlayerInfo[playerid][pPayCheck] = set;
}

stock Character_GetPayCheck(playerid)
{
	return PlayerInfo[playerid][pPayCheck];
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

stock Character_GetJailTime(playerid)
{
	return PlayerInfo[playerid][pJailTime];
}

stock Character_SetJailTime(playerid, timeInMinutes)
{
	PlayerInfo[playerid][pJailTime] = timeInMinutes * 60;
}

stock Character_SetJailTimeAsSeconds(playerid, timeAsSeconds)
{
	PlayerInfo[playerid][pJailTime] = timeAsSeconds;
}

stock Character_IsJailed(playerid)
{
	return ( PlayerInfo[playerid][pJailTime] > 0 );
}

stock Character_IsICJailed(playerid)
{
	return PlayerInfo[playerid][pJailIC];
}

stock Character_SetICJailed(playerid, value)
{
	PlayerInfo[playerid][pJailIC] = value;
}

stock Character_GetFightingStyle(playerid)
{
	return PlayerInfo[playerid][pFightStyle];
}

stock Character_SetFightingStyle(playerid, style)
{
	if(style != FIGHT_STYLE_NORMAL && style != FIGHT_STYLE_BOXING && style != FIGHT_STYLE_KUNGFU && style != FIGHT_STYLE_KNEEHEAD && style != FIGHT_STYLE_GRABKICK)
		return 0;
	SetPlayerFightingStyle(playerid, style);
	PlayerInfo[playerid][pFightStyle] = style;
	return 1;
}

stock Character_GetWalkingStyle(playerid)
{
	return PlayerInfo[playerid][pWalkingStyle];
}

stock Character_SetWalkingStyle(playerid, style)
{
	PlayerInfo[playerid][pWalkingStyle] = style;
	return 1;
}

stock Character_SetChatStyle(playerid, style)
{
	PlayerInfo[playerid][pChatStyle] = style;
}

stock Character_GetChatStyle(playerid)
{
	return PlayerInfo[playerid][pChatStyle];
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
	PlayerInfo[playerid][pBanned] = ban;
}

stock Character_IsBanned(playerid)
{
	return PlayerInfo[playerid][pBanned];
}

stock Character_SetBanExpiry(playerid, time)
{
	PlayerInfo[playerid][pBanExpiry] = time;
}

stock Character_GetBanExpiry(playerid)
{
	return PlayerInfo[playerid][pBanExpiry];
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
	return PlayerInfo[playerid][pLastChopShopTime];
}

stock Character_SetLastChopShopTime(playerid, time)
{
	PlayerInfo[playerid][pLastChopShopTime] = time;
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
	for(new i = 0; i < list_size(HouseList); ++i)
	{
		new owner = list_get(HouseList, i, hOwnerID);
		if(owner == PlayerInfo[playerid][pID])
			list_add(list, i);
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
		if(Character_GetNearestPickup(playerid, id, type) && type == ELEMENT_TYPE_HOUSE_ENTRANCE)
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

stock Character_GetNearBuildingID(playerid)
{
	if(Character_GetVirtualWorld(playerid) < BUILDING_START_WORLD)
		return -1;
	return Character_GetVirtualWorld(playerid) - BUILDING_START_WORLD;
}

stock Character_GetNearBuilding(playerid, bool:inside)
{
	if(inside)
	{
		/*foreach(new b : Buildings)
		{
			if(BuildingInfo[b][bExists] && Building_GetExitWorld(b) == Character_GetVirtualWorld(playerid))
				return b;
		}*/
		return Character_GetNearBuildingID(playerid);
	}
	else
	{
		new id, E_ELEMENT_TYPE:type;
		if(Character_GetNearestPickup(playerid, id, type) && (type == ELEMENT_TYPE_BUILDING_ENTRANCE || type == ELEMENT_TYPE_BUILDING_EXIT))
			return id;
	}
	return -1;
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