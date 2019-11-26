#include <YSI_Coding\y_hooks>

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(GetPlayerCameraMode(playerid) == 55 && newkeys & KEY_CROUCH)
	{
        HandleState(playerid);
		
	}
	
	return 1;
}	

static HandleState(playerid)
{
    ApplyAnimation(playerid, "PED", "CAR_GETIN_RHS", 4.1, 0, 0, 0, 0, 1, 1);
    SetPlayerArmedWeapon(playerid, 0);
    
    wait_ms(650);

    new weaponid = GetPlayerWeapon(playerid);
	SetPlayerArmedWeapon(playerid, weaponid);
}

CMD:leanin(playerid, params[])
{
	if(IsPlayerInAnyVehicle(playerid) && GetPlayerState(playerid) == PLAYER_STATE_PASSENGER)
	{	   
		HandleState(playerid);
	}

	return 1;
}