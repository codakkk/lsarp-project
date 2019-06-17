#include <YSI_Coding\y_hooks>

hook OnCharacterSpawn(playerid)
{
	ClearAnimationsEx(playerid, false);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(newkeys & KEY_NO/* && (GetTickCount() - PlayerInfo[playerid][pKeyTick]) > 2500*/)
	{
		if(pAnimLoop{playerid} == true)
		{
			ClearAnimations(playerid);
			ApplyAnimation(playerid, "CARRY", "crry_prtial", 2.0, 0, 0, 0, 0, 0);
			TextDrawHideForPlayer(playerid, txtAnimHelper);
			pAnimLoop{playerid} = false;
		}
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}