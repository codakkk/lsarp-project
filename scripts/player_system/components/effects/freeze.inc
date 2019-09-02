
#include <YSI_Coding\y_hooks>

stock Character_FreezeForTime(playerid, timeInMillis)
{
	Character_SetFreezed(playerid, true);
	defer UnFreezeTimer(playerid, timeInMillis);
}

timer UnFreezeTimer[timeInMillis](playerid, timeInMillis)
{
	#pragma unused timeInMillis
	Character_SetFreezed(playerid, false);
	//ClearAnimations(playerid);
	//ApplyAnimation(playerid, "CARRY", "crry_prtial", 2.0, 0, 0, 0, 0, 0);
}