#include <YSI_Coding\y_hooks>

hook OnCharacterSpawn(playerid)
{
	//ClearAnimationsEx(playerid, false);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnCharacterDeath(playerid, killerid, reason)
{
	if(Character_IsAnimLoop(playerid))
	{
		Character_SetAnimLoop(playerid, false);
		HideTextAnimationHelper(playerid);
	}
	return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(newkeys & KEY_NO/* && (GetTickCount() - PlayerInfo[playerid][pKeyTick]) > 2500*/)
	{
		if(Character_IsAnimLoop(playerid))
		{
			ClearAnimations(playerid);
			ApplyAnimation(playerid, "CARRY", "crry_prtial", 2.0, 0, 0, 0, 0, 0);
			HideTextAnimationHelper(playerid);
			Character_SetAnimLoop(playerid, false);
		}
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}