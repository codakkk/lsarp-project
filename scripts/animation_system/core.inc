
#include <animation_system\commands>

stock AnimationCheck(playerid)
{
	return (GetPlayerState(playerid) == PLAYER_STATE_ONFOOT && !Character_IsDead(playerid) && !Character_IsFreezed(playerid) && !Character_IsCuffed(playerid) && !Character_IsStunned(playerid));
}

stock ClearAnimationsEx(playerid, bool:animCheck = true)
{
	if(animCheck && !AnimationCheck(playerid))
		return 0;
	ClearAnimations(playerid);
	ApplyAnimation(playerid, "CARRY", "crry_prtial", 2.0, 0, 0, 0, 0, 0);
	SetPlayerSpecialAction(playerid, SPECIAL_ACTION_NONE);
	HideTextAnimationHelper(playerid);
	Character_SetFreezed(playerid, false);
	Character_SetAnimLoop(playerid, false);
	return 1;
}

// Used for looped animations.
stock ApplyAnimationEx(playerid, animlib[], animname[], Float:fDelta, loop, lockx, locky, freeze, time, forcesync = 0)
{
	if(Character_IsAnimLoop(playerid)) HideTextAnimationHelper(playerid);
	ApplyAnimation(playerid, animlib, animname, fDelta, loop, lockx, locky, freeze, time, forcesync);

	Character_SetAnimLoop(playerid, true);
	Player_Info(playerid, "Premi ~y~N~w~ per fermare l'animazione.");
	//sHOWTextAnimationHelper(playerid);
	return 1;
}

stock PreloadAnimations(playerid)
{
	PreloadAnimLib(playerid,"DANCING");
	PreloadAnimLib(playerid,"HEIST9");
	PreloadAnimLib(playerid,"BOMBER");
	PreloadAnimLib(playerid,"RAPPING");
	PreloadAnimLib(playerid,"SHOP");
	PreloadAnimLib(playerid,"BEACH");
	PreloadAnimLib(playerid,"SMOKING");
	PreloadAnimLib(playerid,"FOOD");
	PreloadAnimLib(playerid,"ON_LOOKERS");
	PreloadAnimLib(playerid,"DEALER");
	PreloadAnimLib(playerid,"CRACK");
	PreloadAnimLib(playerid,"CARRY");
	PreloadAnimLib(playerid,"COP_AMBIENT");
	PreloadAnimLib(playerid,"PARK");
	PreloadAnimLib(playerid,"INT_HOUSE");
	PreloadAnimLib(playerid,"FOOD" );
	PreloadAnimLib(playerid,"PED" );
	PreloadAnimLib(playerid,"MISC" );
	PreloadAnimLib(playerid,"POLICE" );
	PreloadAnimLib(playerid,"GRAVEYARD" );
	PreloadAnimLib(playerid,"WUZI" );
	PreloadAnimLib(playerid,"SUNBATHE" );
	PreloadAnimLib(playerid,"PLAYIDLES" );
	PreloadAnimLib(playerid,"CAMERA" );
	PreloadAnimLib(playerid,"RIOT" );
	PreloadAnimLib(playerid,"DAM_JUMP" );
	PreloadAnimLib(playerid,"JST_BUISNESS" );
	PreloadAnimLib(playerid,"KISSING" );
	PreloadAnimLib(playerid,"GANGS" );
	PreloadAnimLib(playerid,"GHANDS" );
	PreloadAnimLib(playerid,"BLOWJOBZ" );
	PreloadAnimLib(playerid,"SWEET" );
	return 1;
}

// Preload animations
// Hehe Figo
// Sono Forte
stock PreloadAnimLib(playerid, animlib[])
{
	ApplyAnimation(playerid, animlib, "null", 0.0, 0, 0, 0, 0, 0, 0);
}