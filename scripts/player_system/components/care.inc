#include <YSI_Coding\y_hooks>

static
	CareTicks[MAX_PLAYERS char]
;

hook OnCharacterClearData(playerid)
{
	CareTicks{playerid} = 0;
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnCharacterSpawn(playerid)
{
	CareTicks{playerid} = 0;
	if(Request_IsPending(playerid, REQUEST_TYPE_CARE))
	{
		new sender = Request_GetSender(playerid, REQUEST_TYPE_CARE);
		new receiver = Request_GetReceiver(playerid, REQUEST_TYPE_CARE);
		Request_Reset(sender, REQUEST_TYPE_CARE);
		Request_Reset(receiver, REQUEST_TYPE_CARE);
	}
	return 1;
}

hook OnCharacterDeath(playerid, killerid, reason)
{
	if(Request_IsPending(playerid, REQUEST_TYPE_CARE))
	{
		new sender = Request_GetSender(playerid, REQUEST_TYPE_CARE);
		new receiver = Request_GetReceiver(playerid, REQUEST_TYPE_CARE);
		Request_Reset(sender, REQUEST_TYPE_CARE);
		Request_Reset(receiver, REQUEST_TYPE_CARE);
	}
	return 1;
}

ptask OnCharacterCareUpdateTimer[1000](playerid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	if(CareTicks{playerid} > 0)
	{
		CareTicks{playerid}--;
		if(CareTicks{playerid} <= 0)
		{
			// With weapon-config, this isn't needed.
			/*for(new i = 0; i < 13; ++i)
			{
				Character_GiveWeapon(playerid, pTempWeapons[playerid][i], pTempAmmo[playerid][i]);
			}*/
			SetCameraBehindPlayer(playerid);
			Character_SetFreezed(playerid, false);
			if(Character_IsCuffed(playerid))
			{
				Character_SetCuffed(playerid, false); // FIrst reset state
				Character_SetCuffed(playerid, true); // Then reapply
			}
			SetPlayerHealth(playerid, 100.0);
			SetPlayerDrunkLevel(playerid, 0);
		}
	}
	return 1;
}

hook OnCharacterAccept(playerid, stringHash)
{
    if(stringHash != _I<cure>)
		return 0;
	
	if(!Request_IsPending(playerid, REQUEST_TYPE_CARE) || !Character_CanAcceptRequest(playerid, REQUEST_TYPE_CARE))
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai una richiesta cure attiva.");
	new senderid = Request_GetSender(playerid, REQUEST_TYPE_CARE);
	
	if(!Character_CanInteract(senderid))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non puo' curarti in questo momento."), 0;

	if(!IsPlayerInRangeOfPlayer(playerid, senderid, 5.0) || !Character_IsLogged(senderid))
	{
		SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore che ti ha inviato la richiesta.");
		return 0;
	}
	
	Character_SetFreezed(playerid, false);
	
	Request_Reset(playerid, REQUEST_TYPE_CARE);
	Request_Reset(senderid, REQUEST_TYPE_CARE);

	if(!Player_IsAdminDuty(playerid) && !Player_IsAdminDuty(senderid))
	{
		ApplyAnimation(senderid, "MEDIC", "CPR", 4.1, 0, 0, 0, 0, 0, 1);
		ApplyAnimation(playerid, "PED", "KO_shot_stom", 4.0, 0, 1, 1, 1, 0, 1);
		CareTicks{playerid} = 10;
	}
	else 
		CareTicks{playerid} = 1;

	Character_SetLegHit(playerid, false);
	Character_ResetDamages(playerid);
	Character_ResetDeathState(playerid);
	
	PlayerTextDrawHide(playerid, pDeathTextDraw[playerid]);

	SendFormattedMessage(playerid, COLOR_ACCEPT_REQUEST, "Hai accettato le cure di %s.", Character_GetRolePlayName(senderid));
	SendFormattedMessage(senderid, COLOR_ACCEPT_REQUEST, "%s ha accettato le tue cure.", Character_GetRolePlayName(playerid));
	Log(Character_GetOOCName(senderid), Character_GetOOCName(playerid), "/accetta cure");
	return 1;
}