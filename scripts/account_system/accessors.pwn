stock Account_ResetBitState(playerid)
{
	for(new i = 0; e_Bit1_AccountData:i < e_Bit1_AccountData; i++)
	{
		Bit_Set(gAccountBitState[e_Bit1_AccountData:i], playerid, false);
	}
}

stock Account_SetLogged(playerid, bool:logged)
{
	Bit_Set(gAccountBitState[e_pAccountLogged], playerid, logged);
}

stock Account_IsLogged(playerid)
{
	return Bit_Get(gAccountBitState[e_pAccountLogged], playerid);
}

stock Account_SetPMAllEnabled(playerid, bool:enable)
{
	Bit_Set(gAccountBitState[e_pTogglePMAll], playerid, !enable);
}

stock Account_HasPMAllEnabled(playerid)
{
	return !Bit_Get(gAccountBitState[e_pTogglePMAll], playerid);
}

stock Account_SetHUDEnabled(playerid, bool:enable)
{
	Bit_Set(gAccountBitState[e_pToggleHUD], playerid, !enable);
}

stock Account_HasHUDEnabled(playerid)
{
	return !Bit_Get(gAccountBitState[e_pToggleHUD], playerid);
}

stock Account_SetOOCEnabled(playerid, bool:enable)
{
	Bit_Set(gAccountBitState[e_pToggleOOCAll], playerid, !enable);
}

stock Account_HasOOCEnabled(playerid)
{
	return !Bit_Get(gAccountBitState[e_pToggleOOCAll], playerid);
}

stock Account_HasOOCEnabledForPlayer(playerid, targetid)
{
	return !Iter_Contains(pToggleOOC[playerid], targetid);
}

stock Account_SetHotKeysEnabled(playerid, bool:enable)
{
	Bit_Set(gAccountBitState[e_pHotKeys], playerid, enable);
}

stock Account_HasHotKeysEnabled(playerid)
{
	return Bit_Get(gAccountBitState[e_pHotKeys], playerid);
}

stock Account_SetInvModeEnabled(playerid, bool:enable)
{
	Bit_Set(gAccountBitState[e_pInvMode], playerid, enable);
}

stock Account_HasInvModeEnabled(playerid)
{
	return Bit_Get(gAccountBitState[e_pInvMode], playerid);
}


stock Account_GetID(playerid)
{
	return AccountInfo[playerid][aID];
}

stock Account_SetAdminLevel(playerid, level)
{
	AccountInfo[playerid][aAdmin] = level;
}

stock Account_GetAdminLevel(playerid)
{
	return AccountInfo[playerid][aAdmin];
}

stock Account_SetPremiumLevel(playerid, level)
{
	AccountInfo[playerid][aPremium] = level;
}

stock Account_GetPremiumLevel(playerid)
{
	return AccountInfo[playerid][aPremium];
}

stock Account_SetPremiumExpiry(playerid, time)
{
	AccountInfo[playerid][aPremiumExpiry] = time;
}

stock Account_GetPremiumExpiry(playerid)
{
	return AccountInfo[playerid][aPremiumExpiry];
}

stock Account_GetCharactersSlot(playerid)
{
	return AccountInfo[playerid][aCharactersSlot];
}

stock Account_SetCharactersSlot(playerid, setValue)
{
	AccountInfo[playerid][aCharactersSlot] = setValue;
}

stock Account_IncreaseCharactersSlot(playerid, amount)
{
	AccountInfo[playerid][aCharactersSlot] += amount;
}

stock Account_GetCharactersCount(playerid)
{
	return AccountInfo[playerid][aCharactersCount];
}

stock Account_SetZPoints(playerid, points)
{
	AccountInfo[playerid][aZPoints] = points;
}

stock Account_AddPoints(playerid, points)
{
	AccountInfo[playerid][aZPoints] += points;
}

stock Account_GetZPoints(playerid)
{
	return AccountInfo[playerid][aZPoints];
}

stock Account_SetBanned(playerid, ban)
{
	AccountInfo[playerid][aBanned] = ban;
}

stock Account_IsBanned(playerid)
{
	return AccountInfo[playerid][aBanned];
}

stock Account_SetBanExpiry(playerid, time)
{
	AccountInfo[playerid][aBanExpiry] = time;
}

stock Account_GetBanExpiry(playerid)
{
	return AccountInfo[playerid][aBanExpiry];
}

stock RewardInfo_GetPoints(rewardid)
{
	return RewardInfo[rewardid][rdPoints];
}

stock RewardInfo_GetName(rewardid)
{
	return RewardInfo[rewardid][rdName];
}

