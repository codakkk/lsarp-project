
#include <YSI_Coding\y_hooks>

static InvincibleTime[MAX_PLAYERS char];

hook OnPlayerClearData(playerid)
{
    InvincibleTime{playerid} = 0;
    return 1;
}

ptask OnInvicibleTimeUpdate[1000](playerid) 
{
    if(InvincibleTime{playerid} > 0)
    {
        InvincibleTime{playerid}--;
    }
    return 1;   
}

stock Character_SetInvincibleForTime(playerid, timeAsSeconds)
{
    if(timeAsSeconds > 255)
        return;
    InvincibleTime{playerid} = timeAsSeconds;
}

stock Character_IsInvincible(playerid)
{
    return InvincibleTime{playerid} > 0;
}