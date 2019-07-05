
#include <YSI_Coding\y_hooks>

#define INJURED_TIME_AS_SECONDS		60
#define DEATH_TIME_AS_SECONDS 		30

static enum e_DeathState
{
	pDeathTime,
	Float:pDeathX,
	Float:pDeathY,
	Float:pDeathZ,
	Float:pDeathA,
	pDeathInt,
	pDeathWorld,
	pDeathKiller,
	Text3D:pDeathText
};

static 
	PlayerDeathState[MAX_PLAYERS][e_DeathState],
	pDeathState[MAX_PLAYERS char] // 0 = Not dead, 1 = Waiting for healing, 2 = Death
;


hook OnPlayerClearData(playerid)
{
	Character_SetDeathState(playerid, DEATH_STATE_NONE);
	if(IsValidDynamic3DTextLabel(PlayerDeathState[playerid][pDeathText]))
		DestroyDynamic3DTextLabelEx(PlayerDeathState[playerid][pDeathText]);
	
	Character_ResetDeathState(playerid);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnCharacterSpawn(playerid)
{
	Character_ResetDeathState(playerid);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	if(!Character_IsAlive(playerid))
	{
		ClearAnimations(playerid);
		ApplyAnimation(playerid, "WUZI", "CS_Dead_Guy", 4.0, 0, 0, 0, 1, 0, 1);
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

// Causes death system to get bugged
ptask OnPlayerDeathSystemUpdate[1000](playerid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	if(!Character_IsAlive(playerid))
	{
		if(GetPlayerAnimationIndex(playerid) != 1701)
			ApplyAnimation(playerid, "WUZI", "CS_Dead_Guy", 4.0, 0, 0, 0, 1, 0, 1);

		new Float:x, Float:y, Float:z, Float:a, interior, world;
		Character_GetDeathStateData(playerid, x, y, z, a, interior, world);

		if(Character_IsInjured(playerid))
		{
			new deathTime = GetTickCount() - Character_GetDeathTime(playerid);
			if(deathTime < INJURED_TIME_AS_SECONDS * 1000/*120000*/)
			{
				PlayerTextDrawSetStringStr(playerid, pDeathTextDraw[playerid], str_format("Tra %d secondi puoi digitare ~g~/accetta morte~w~.", INJURED_TIME_AS_SECONDS - deathTime/1000));
			}
			else
			{
				PlayerTextDrawSetStringStr(playerid, pDeathTextDraw[playerid], @("Digita ~g~/accetta morte~w~ per accettare la morte."));
			}
		}
		else if(Character_IsDead(playerid))
		{
			new deathTime = GetTickCount() - Character_GetDeathTime(playerid);
			if(deathTime < DEATH_TIME_AS_SECONDS * 1000/*120000*/)
			{
				PlayerTextDrawSetStringStr(playerid, pDeathTextDraw[playerid], str_format("Tra %d secondi puoi digitare ~g~/respawnme~w~.", DEATH_TIME_AS_SECONDS - deathTime/1000));
			}
			else
			{
				PlayerTextDrawSetStringStr(playerid, pDeathTextDraw[playerid], @("Digita ~g~/respawnme~w~ per respawnare."));
			}
			AC_SetPlayerHealth(playerid, 9999.0);
		}

		if(!IsPlayerInRangeOfPoint(playerid, 3.0, x, y, z) || GetPlayerInterior(playerid) != interior || Character_GetVirtualWorld(playerid) != world)
		{
			SetPlayerPos(playerid, x, y, z);
			SetPlayerFacingAngle(playerid, a);

			SetPlayerInterior(playerid, interior);
			Character_SetVirtualWorld(playerid, world);
		}

		if(GetPlayerState(playerid) != PLAYER_STATE_ONFOOT) 
			RemovePlayerFromVehicle(playerid);
	}
	return 1;
}

hook OnPlayerDeath(playerid, killerid, reason)
{
	printf("OnCharacterDeathStart");
	new deathState = Character_GetDeathState(playerid) < 2 ? Character_GetDeathState(playerid)+1 : 0;
	Character_SetDeathState(playerid, deathState);

	GetPlayerPos(playerid, PlayerDeathState[playerid][pDeathX], PlayerDeathState[playerid][pDeathY], PlayerDeathState[playerid][pDeathZ]);
	GetPlayerFacingAngle(playerid, PlayerDeathState[playerid][pDeathA]);

	PlayerDeathState[playerid][pDeathInt] = GetPlayerInterior(playerid);
	PlayerDeathState[playerid][pDeathWorld] = Character_GetVirtualWorld(playerid);
	new weapons[13], ammo[13];
	for(new x = 0; x < 13; x++)
	{
		Character_GetWeaponData(playerid, x, weapons[x], ammo[x]);
		pTempWeapons[playerid][x] = weapons[x];
		pTempAmmo[playerid][x] = ammo[x];
	}

	if(deathState == DEATH_STATE_INJURED)
	{
		if(killerid != INVALID_PLAYER_ID)
			SendFormattedMessage(playerid, COLOR_ERROR, "(( Sei stato ucciso da \"%s\". ))", Character_GetRolePlayName(killerid));
		SendFormattedMessage(playerid, COLOR_GREEN, "Digita /danni %d per lo storico danni.", playerid);
		Character_SetDeathTime(playerid, GetTickCount());
	}
	else if(deathState == DEATH_STATE_DEAD)
	{
		Character_SetDeathTime(playerid, GetTickCount());
		Character_OffDuty(playerid);
	}
	printf("OnCharacterDeath End");
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerSpawn(playerid)
{
	new deathState = Character_GetDeathState(playerid);
	//printf("Death State: %d", deathState);
	if(deathState > 0)
	{
		PlayerTextDrawShow(playerid, pDeathTextDraw[playerid]);
		if(deathState == DEATH_STATE_INJURED)
		{
			if(IsValidDynamic3DTextLabel(PlayerDeathState[playerid][pDeathText]))
				DestroyDynamic3DTextLabelEx(PlayerDeathState[playerid][pDeathText]);
			
			new str[64];
			format(str, sizeof(str), "(( QUESTO GIOCATORE È FERITO. /danni %d ))", playerid);
			PlayerDeathState[playerid][pDeathText] = CreateDynamic3DTextLabel(str, COLOR_ADMIN, 0.0, 0.0, 0.5, 30.0, playerid, INVALID_VEHICLE_ID, 1, -1, -1, -1, 100.0);
			
			AC_SetPlayerHealth(playerid, 20.0);
			AC_SetPlayerArmour(playerid, 0.0);

			SetCameraBehindPlayer(playerid);

			new Float:x, Float:y, Float:z, Float:a, interior, world;
			
			Character_GetDeathStateData(playerid, x, y, z, a, interior, world);
			
			SetPlayerPos(playerid, x, y, z);
			SetPlayerFacingAngle(playerid, a);
			SetPlayerInterior(playerid, interior);
			Character_SetVirtualWorld(playerid, world);

			ApplyAnimation(playerid, "WUZI", "CS_Dead_Guy", 4.0, 0, 0, 0, 1, 0, 1);

			Character_AMe(playerid, "è in gravi condizioni.");

			SendClientMessage(playerid, COLOR_ERROR, "Sei in gravi condizioni, tra 60 secondi puoi accettare la morte utilizzando \"/accetta morte\".");
			CallLocalFunction(#OnCharacterInjured, "d", playerid);
		}
		else if(deathState == DEATH_STATE_DEAD)
		{
			Character_OffDuty(playerid);
			Character_ResetWeapons(playerid);
			
			AC_SetPlayerHealth(playerid, 9999.0); // Sets invincible
			
			new str[64];
			format(str, sizeof(str), "(( QUESTO GIOCATORE È MORTO. /danni %d ))", playerid);
			UpdateDynamic3DTextLabelText(PlayerDeathState[playerid][pDeathText], COLOR_ADMIN, str);

			SendFormattedMessage(playerid, COLOR_ADMIN, "(( Sei morto. Usa \"/respawnme\" tra %d secondi per respawnare. ))", DEATH_TIME_AS_SECONDS);

			new Float:x, Float:y, Float:z, Float:a, interior, world;
			
			Character_GetDeathStateData(playerid, x, y, z, a, interior, world);
			
			SetPlayerPos(playerid, x, y, z);
			SetPlayerFacingAngle(playerid, a);
			SetPlayerInterior(playerid, interior);
			Character_SetVirtualWorld(playerid, world);

			ApplyAnimation(playerid, "WUZI", "CS_Dead_Guy", 4.0, 0, 0, 0, 1, 0, 1);
			CallLocalFunction(#OnCharacterDeath, "d", playerid);
		}
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart)
{
	if(Character_IsInjured(playerid)) 
	{
		AC_SetPlayerHealth(playerid, 0.0);
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

stock Character_AcceptDeathState(playerid)
{
	new deathTime = GetTickCount() - Character_GetDeathTime(playerid);
	if(deathTime < INJURED_TIME_AS_SECONDS * 1000/*120000*/)
		return SendFormattedMessage(playerid, COLOR_ERROR, "Devi aspettare ancora %d secondi prima di poter digitare il comando.", INJURED_TIME_AS_SECONDS - deathTime/1000);
	
	if(!Character_IsInjured(playerid)) return SendClientMessage(playerid, COLOR_ERROR, "Non sei in sistema ferito.");

	AC_SetPlayerHealth(playerid, 0.0);
	Character_SetDeathTime(playerid, 0);
	return 1;
}

flags:respawnme(CMD_USER);
CMD:respawnme(playerid, params[])
{
	if(Character_GetDeathState(playerid) != DEATH_STATE_DEAD) return 1;
	new death_time = GetTickCount() - Character_GetDeathTime(playerid);
	if(death_time < DEATH_TIME_AS_SECONDS * 1000/*120000*/)
		return SendFormattedMessage(playerid, COLOR_ERROR, "Devi aspettare ancora %d secondi prima di poter digitare il comando.", DEATH_TIME_AS_SECONDS - death_time/1000);
	Character_ResetWeapons(playerid);
	Character_Spawn(playerid);
	return 1;
}

stock Character_ResetDeathState(playerid)
{
	if(IsValidDynamic3DTextLabel(PlayerDeathState[playerid][pDeathText]))
		DestroyDynamic3DTextLabelEx(PlayerDeathState[playerid][pDeathText]);
	PlayerTextDrawHide(playerid, pDeathTextDraw[playerid]);
	new CleanPlayerDeathState[e_DeathState];
	PlayerDeathState[playerid] = CleanPlayerDeathState;
}

stock Character_SetDeathState(playerid, s)
{
	pDeathState{playerid} = s;
}

 // 0: Alive, 1: Injured, 2: Dead
stock Character_GetDeathState(playerid)
{
	return pDeathState{playerid};
}

stock Character_IsAlive(playerid)
{
	return pDeathState{playerid} == DEATH_STATE_NONE;  // 0: Alive, 1: Injured, 2: Dead
}

stock Character_IsInjured(playerid)
{
	return pDeathState{playerid} == DEATH_STATE_INJURED; // 0: Alive, 1: Injured, 2: Dead
}

stock Character_IsDead(playerid)
{
	return pDeathState{playerid} == DEATH_STATE_DEAD; // 0: Alive, 1: Injured, 2: Dead
}

stock Character_GetDeathStateData(playerid, &Float:x, &Float:y, &Float:z, &Float:a, &int, &world)
{
	if(Character_IsAlive(playerid))
		return 0;
	x = PlayerDeathState[playerid][pDeathX];
	y = PlayerDeathState[playerid][pDeathY];
	z = PlayerDeathState[playerid][pDeathZ];
	a = PlayerDeathState[playerid][pDeathA];
	int = PlayerDeathState[playerid][pDeathInt];
	world = PlayerDeathState[playerid][pDeathWorld];
	return 1;
}

stock Character_SetDeathTime(playerid, time)
{
	PlayerDeathState[playerid][pDeathTime] = time;
}

stock Character_GetDeathTime(playerid)
{
	return PlayerDeathState[playerid][pDeathTime];
}

stock Character_GetDeathKillerID(playerid)
{
	return PlayerDeathState[playerid][pDeathKiller];
}

stock Character_SetDeathKillerID(playerid, killerid)
{
	PlayerDeathState[playerid][pDeathKiller] = killerid;
}