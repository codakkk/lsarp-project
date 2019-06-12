#include <YSI_Coding\y_hooks>

hook AntiCheatTimer(playerid)
{
	new Float:hp, Float:serverHP;

	if(Character_IsAlive(playerid))
	{
		GetPlayerHealth(playerid, hp);
		AC_GetPlayerHealth(playerid, serverHP);
		// printf("HP: %f - SHP: %f", hp, serverHP);
		if(hp > serverHP)
		{
			AC_SetPlayerHealth(playerid, serverHP);
		}
		/*else if(hp < serverHP)
		{
			AC_SetPlayerHealth(playerid, hp);
		}*/

		GetPlayerArmour(playerid, hp);
		AC_GetPlayerArmour(playerid, serverHP);
		
		if(hp > serverHP)
		{
			AC_SetPlayerArmour(playerid, serverHP);
		}
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

stock AC_SetPlayerHealth(playerid, Float:health)
{
	PlayerInfo[playerid][pHealth] = health;
	SetPlayerHealth(playerid, health); 
	return 1;
}

stock AC_GetPlayerHealth(playerid, &Float:h)
{
	h = PlayerInfo[playerid][pHealth];
	return 1;
}

stock AC_SetPlayerArmour(playerid, Float:armour)
{
	PlayerInfo[playerid][pArmour] = armour;
	SetPlayerArmour(playerid, armour);
	return 1;
}

stock AC_GetPlayerArmour(playerid, &Float:a)
{
	a = PlayerInfo[playerid][pArmour];
	return 1;
}