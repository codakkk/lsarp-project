#include <a_samp>

main(){}

#include "weapon-config"

#define INJURED_TIME_AS_SECONDS		30
#define DEATH_TIME_AS_SECONDS 		30

static enum // e_DeathStates
{
	DEATH_STATE_NONE = 0,
	DEATH_STATE_INJURED = 1,
	DEATH_STATE_DEAD = 2
}

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
	pDeathVehicle,
};

static stock
	PlayerDeathState[MAX_PLAYERS][e_DeathState],
	DeathState[MAX_PLAYERS char] // 0 = Not dead, 1 = Waiting for healing, 2 = Death
;

public OnGameModeInit()
{
    SetRespawnTime(1);
    SetTimer("UpdateDeathSystem", 100, 1);
    return 1;
}

forward UpdateDeathSystem();
public UpdateDeathSystem()
{
    for(new playerid = 0; playerid < MAX_PLAYERS; ++playerid)
    {
        if(!Character_IsAlive(playerid))
        {

            new Float:x, Float:y, Float:z, Float:a, interior, world;
            GetPlayerPos(playerid, x, y, z);
            Character_GetDeathStateData(playerid, x, y, z, a, interior, world);
            
            if(GetPlayerDistanceFromPoint(playerid, x, y, z) > 3.0 || GetPlayerInterior(playerid) != interior || GetPlayerVirtualWorld(playerid) != world)
            {
                SetPlayerPos(playerid, x, y, z);
                SetPlayerFacingAngle(playerid, a);
                SetPlayerInterior(playerid, interior);
                SetPlayerVirtualWorld(playerid, world);
            }
            
            Internal_ApplyDeathAnim(playerid);
        }
    }
	return 1;
}

public OnPlayerSpawn(playerid)
{
    SetPlayerPos(playerid, 114.0, -78, 1.80);
    return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
    if(!strcmp(cmdtext, "/test", true))
    {
        Character_ResetDeathState(playerid);
        GivePlayerWeapon(playerid, 31, 100);
        ClearAnimations(playerid, 1);
    }
    return 1;
}

public OnPlayerDeath(playerid, killerid, reason)
{
	if(IsPlayerNPC(playerid))
		return 1;
	
	SetPlayerDrunkLevel(playerid, 0);

	if(Character_IsAlive(playerid))
	{
		new vid = GetPlayerVehicleID(playerid);
		if(reason == 51 && vid > 0) //(IsAHelicopter(vid) || IsAPlane(vid)))
		{
			Character_SetDeathState(playerid, DEATH_STATE_NONE);
		}
		else
		{
			Character_SetDeathState(playerid, DEATH_STATE_INJURED);
		}
	}
	else if(Character_IsInjured(playerid))
	{
		Character_SetDeathState(playerid, DEATH_STATE_NONE);
	}
	
	new newDeathState = Character_GetDeathState(playerid);

	GetPlayerPos(playerid, PlayerDeathState[playerid][pDeathX], PlayerDeathState[playerid][pDeathY], PlayerDeathState[playerid][pDeathZ]);
	GetPlayerFacingAngle(playerid, PlayerDeathState[playerid][pDeathA]);

	PlayerDeathState[playerid][pDeathInt] = GetPlayerInterior(playerid);
	PlayerDeathState[playerid][pDeathWorld] = GetPlayerVirtualWorld(playerid);
	PlayerDeathState[playerid][pDeathVehicle] = GetPlayerVehicleID(playerid);

	if(newDeathState == DEATH_STATE_INJURED)
	{
		Character_SetDeathTime(playerid, GetTickCount());
	}
	else
	{
		ResetPlayerWeapons(playerid);
	}
	Internal_UpdateDeathState(playerid);

	return 1;
}

static Internal_UpdateDeathState(playerid)
{
	new deathState = Character_GetDeathState(playerid);
	if(deathState > 0)
	{
		if(deathState == DEATH_STATE_INJURED)
		{

			SetPlayerHealth(playerid, 20.0);
            SetPlayerArmour(playerid, 0.0);

			SetCameraBehindPlayer(playerid);

			new Float:x, Float:y, Float:z, Float:a, interior, world;
			
			Character_GetDeathStateData(playerid, x, y, z, a, interior, world);

			SetPlayerPos(playerid, x, y, z);
			SetPlayerFacingAngle(playerid, a);
			SetPlayerInterior(playerid, interior);
			SetPlayerVirtualWorld(playerid, world);
		}
		Internal_ApplyDeathAnim(playerid);
	}
	return 1;
}

stock Character_ResetDeathState(playerid)
{
	new CleanPlayerDeathState[e_DeathState];
	PlayerDeathState[playerid] = CleanPlayerDeathState;
	DeathState{playerid} = DEATH_STATE_NONE;
}

stock Character_SetDeathState(playerid, s)
{
	DeathState{playerid} = s;
}

 // 0: Alive, 1: Injured, 2: Dead
stock Character_GetDeathState(playerid)
{
	return DeathState{playerid};
}

stock Character_IsAlive(playerid)
{
	return DeathState{playerid} == DEATH_STATE_NONE;  // 0: Alive, 1: Injured, 2: Dead
}

stock Character_IsInjured(playerid)
{
	return DeathState{playerid} == DEATH_STATE_INJURED; // 0: Alive, 1: Injured, 2: Dead
}

stock Character_IsDead(playerid)
{
	return DeathState{playerid} == DEATH_STATE_DEAD; // 0: Alive, 1: Injured, 2: Dead
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

stock Character_GetDeathStateVehicle(playerid)
{
	return PlayerDeathState[playerid][pDeathVehicle];
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

static stock Internal_ApplyDeathAnim(playerid)
{
	if( (Character_IsDead(playerid) || Character_IsInjured(playerid)) && GetPlayerAnimationIndex(playerid) != 1701)
	{
		ClearAnimations(playerid);
		ApplyAnimation(playerid, "WUZI", "CS_Dead_Guy", 4.0, 0, 0, 0, 1, 0, 1);
	}
}