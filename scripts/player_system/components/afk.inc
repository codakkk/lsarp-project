#include <YSI_Coding\y_hooks>

static pAFKTime[MAX_PLAYERS];

ptask OnCharacterAFKTimeUpdate[1000](playerid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	pAFKTime[playerid]++;
	return 1;
}

hook OnPlayerUpdate(playerid)
{
	pAFKTime[playerid] = 0;
	return 1;
}

stock Character_IsAFK(playerid)
{
	return pAFKTime[playerid] > 1;
}

stock Character_GetAFKTime(playerid)
{
	return pAFKTime[playerid];
}