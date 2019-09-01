
#include <YSI_Coding\y_hooks>

static enum e_Bit1_Data 
{
	e_pCharacterLogged,
    e_pFreezed,
	e_pToggleFactionOOC,
	e_pFactionDuty,
	e_pSelectingUniform,
	e_pCuffed,
	e_pDragged,
	e_pDragging,
	e_pMasked,
	e_pLegHit,
	e_pAnimLoop,
	e_pBuyingVehicle, // Is Buying something
	e_pCanChopShop,
	e_pCanGetAlive,

	e_pDisableAdminAlerts,
	e_pAdminDuty,
	e_pSupporterDuty,
};

static 
    BitArray:CharacterBitState[e_Bit1_Data]<MAX_PLAYERS>;

stock Character_IsFreezed(playerid)
{
    return Bit_Get(CharacterBitState[e_pFreezed], playerid);
}

stock Character_SetFreezed(playerid, bool:freeze)
{
    Bit_Set(CharacterBitState[e_pFreezed], playerid, freeze);
    TogglePlayerControllable(playerid, !freeze);
}

stock Character_SetLogged(playerid, bool:logged)
{
	Bit_Set(CharacterBitState[e_pCharacterLogged], playerid, logged);
}

stock Character_IsLogged(playerid)
{
	if(!IsPlayerConnected(playerid))
		return 0;
	return Bit_Get(CharacterBitState[e_pCharacterLogged], playerid);
}

stock Character_SetFactionOOCEnabled(playerid, bool:value)
{
	Bit_Set(CharacterBitState[e_pToggleFactionOOC], playerid, !value);
}

stock Character_IsFactionOOCEnabled(playerid)
{
	return !Bit_Get(CharacterBitState[e_pToggleFactionOOC], playerid);
}

stock Character_SetFactionDuty(playerid, bool:value)
{
	Bit_Set(CharacterBitState[e_pFactionDuty], playerid, value);
	mysql_tquery_f(gMySQL, "UPDATE `characters` SET faction_duty = '%d' WHERE id = '%d'", value?1:0, Character_GetID(playerid));
}

stock Character_IsFactionDuty(playerid)
{
	return Bit_Get(CharacterBitState[e_pFactionDuty], playerid);
}

stock Character_SetSelectingUniform(playerid, bool:value)
{
	Bit_Set(CharacterBitState[e_pSelectingUniform], playerid, value);
}

stock Character_IsSelectingUniform(playerid)
{
	return Bit_Get(CharacterBitState[e_pSelectingUniform], playerid);
}

stock Character_SetBuyingVehicle(playerid, bool:value)
{
	Bit_Set(CharacterBitState[e_pBuyingVehicle], playerid, value);
}

stock Character_IsBuyingVehicle(playerid)
{
	return Bit_Get(CharacterBitState[e_pBuyingVehicle], playerid);
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
	Bit_Set(CharacterBitState[e_pCuffed], playerid, value);
}

stock Character_IsCuffed(playerid)
{
	return Bit_Get(CharacterBitState[e_pCuffed], playerid);
}

stock Character_SetMasked(playerid, bool:masked)
{
	Bit_Set(CharacterBitState[e_pMasked], playerid, masked);
}

stock Character_IsMasked(playerid)
{
	return Bit_Get(CharacterBitState[e_pMasked], playerid);
}

stock Character_SetLegHit(playerid, bool:legHitted)
{
	Bit_Set(CharacterBitState[e_pLegHit], playerid, legHitted);
}

stock Character_IsLegHitted(playerid)
{
	return Bit_Get(CharacterBitState[e_pLegHit], playerid);
}

stock Character_ResetBitState(playerid)
{
	for(new i; e_Bit1_Data:i < e_Bit1_Data; i++)
	{
		Bit_Set(CharacterBitState[e_Bit1_Data:i], playerid, false);
	}
}

stock Character_HasChopShopPrivileges(playerid)
{
	return Bit_Get(CharacterBitState[e_pCanChopShop], playerid);
}

stock Character_SetChopShopPrivileges(playerid, bool:can)
{
	Bit_Set(CharacterBitState[e_pCanChopShop], playerid, can);
}

stock bool:Character_IsAnimLoop(playerid)
{
	return Bit_Get(CharacterBitState[e_pAnimLoop], playerid);
}

stock Character_SetAnimLoop(playerid, bool:set)
{
	Bit_Set(CharacterBitState[e_pAnimLoop], playerid, set);
}

stock Character_SetDragged(playerid, bool:dragged)
{
	Bit_Set(CharacterBitState[e_pDragged], playerid, dragged);
}

stock Character_IsDragged(playerid)
{
	return Bit_Get(CharacterBitState[e_pDragged], playerid);
}

stock Character_SetDragging(playerid, bool:dragging)
{
	Bit_Set(CharacterBitState[e_pDragging], playerid, dragging);
}

stock Character_IsDragging(playerid)
{
	return Bit_Get(CharacterBitState[e_pDragging], playerid);
}

stock Player_SetAdminDuty(playerid, bool:enable)
{
	Bit_Set(CharacterBitState[e_pAdminDuty], playerid, enable);
}

stock Player_IsAdminDuty(playerid)
{
	return Bit_Get(CharacterBitState[e_pAdminDuty], playerid);
}

stock Player_SetSupporterDuty(playerid, bool:enable)
{
	Bit_Set(CharacterBitState[e_pSupporterDuty], playerid, enable);
}

stock Player_IsSupporterDuty(playerid)
{
	return Bit_Get(CharacterBitState[e_pSupporterDuty], playerid);
}

stock Player_SetDisabledAdminAlerts(playerid, bool:enable)
{
	Bit_Set(CharacterBitState[e_pDisableAdminAlerts], playerid, enable);
}

stock Player_IsDisabledAdminAlerts(playerid)
{
	return Bit_Get(CharacterBitState[e_pDisableAdminAlerts], playerid);
}