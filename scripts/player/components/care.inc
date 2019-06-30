#include <YSI_Coding\y_hooks>

static
	pCare[MAX_PLAYERS],
	pInCare[MAX_PLAYERS char],
	pCareTime[MAX_PLAYERS]
;

hook OnPlayerClearData(playerid)
{
	Character_SetCarePlayer(playerid, INVALID_PLAYER_ID);
	Character_SetCareTime(playerid, 0);
	Character_SetCareTick(playerid, 0);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

ptask OnCharacterCareUpdateTimer[1000](playerid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	if(Character_GetCareTicks(playerid) > 0)
	{
		Character_DecreaseCareTick(playerid);
		if(Character_GetCareTicks(playerid) <= 0)
		{
			for(new i = 0; i < 13; ++i)
			{
				Character_GiveWeapon(playerid, pTempWeapons[playerid][i], pTempAmmo[playerid][i]);
			}
			SetCameraBehindPlayer(playerid);
			Character_SetFreezed(playerid, false);
			if(Character_IsCuffed(playerid))
			{
				Character_SetCuffed(playerid, true);
			}
			SetPlayerDrunkLevel(playerid, 0);
		}
	}
	return 1;
}

stock Character_AcceptCare(playerid)
{
	new senderid = Character_GetCarePlayer(playerid);
	if(senderid == INVALID_PLAYER_ID)
		return 0;
	
	if(!IsPlayerInRangeOfPlayer(playerid, senderid, 5.0) || !Character_IsLogged(senderid))
	{
		SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore che ti ha inviato la richiesta.");
		return 0;
	}
	
	Character_SetFreezed(playerid, false);
	
	Character_SetCarePlayer(playerid, INVALID_PLAYER_ID);

	if(!pAdminDuty[playerid] && !pAdminDuty[senderid])
	{
		ApplyAnimation(senderid, "MEDIC", "CPR", 4.1, 0, 0, 0, 0, 0, 1);
		ApplyAnimation(playerid, "PED", "KO_shot_stom", 4.0, 0, 1, 1, 1, 0, 1);
		Character_SetCareTick(playerid, 10);
	}
	else 
		Character_SetCareTick(playerid, 1);

	Character_SetDeathState(playerid, DEATH_STATE_NONE);
	Character_SetLegHit(playerid, false);
	
	Character_ResetDeathState(playerid);
	
	PlayerTextDrawHide(playerid, pDeathTextDraw[playerid]);

	SendFormattedMessage(playerid, COLOR_GREEN, "Hai accettato le cure di %s.", Character_GetRolePlayName(senderid));
	SendFormattedMessage(senderid, COLOR_GREEN, "%s ha accettato le tue cure.", Character_GetRolePlayName(playerid));
	Log(Character_GetOOCName(senderid), Character_GetOOCName(playerid), "/accetta cure");
	return 1;
}

stock Character_GetCarePlayer(playerid)
{
	return pCare[playerid];
}

stock Character_SetCarePlayer(playerid, id)
{
	pCare[playerid] = id;
}

stock Character_GetCareTime(playerid)
{
	return pCareTime[playerid];
}

stock Character_SetCareTime(playerid, time)
{
	pCareTime[playerid] = time;
}

stock Character_GetCareTicks(playerid)
{
	return pInCare{playerid};
}

stock Character_SetCareTick(playerid, ticks)
{
	pInCare{playerid} = ticks;
}

stock Character_DecreaseCareTick(playerid)
{
	pInCare{playerid}--;
}