#include <YSI_Coding\y_hooks>

static 
    Stunned[MAX_PLAYERS char] = {0, ...};

hook OnCharacterClearData(playerid)
{
    Stunned{playerid} = 0;
    return 1;
}

ptask OnCharacterStunUpdateTimer[1000](playerid)
{
    if(Stunned{playerid} > 0)
	{
		Stunned{playerid}--;
		if(!Stunned{playerid})
			ClearAnimationsEx(playerid);
		
		if(GetPlayerAnimationIndex(playerid) != 388)
			ApplyAnimation(playerid, "CRACK", "crckdeth4", 4.0, 0, 0, 0, 1, 0, 1);
	}
    return 1;
}

stock Character_GetStunTime(playerid)
{
    return Stunned{playerid};
}

stock Character_IsStunned(playerid)
{
    return Character_GetStunTime(playerid) > 0;
}

stock Character_SetStunned(playerid, time)
{
    Stunned{playerid} = time;
}