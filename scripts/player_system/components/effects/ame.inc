#include <YSI_Coding\y_hooks>

static
    PlayerAMeExpiry[MAX_PLAYERS char],
    Text3D:PlayerAMe3DText[MAX_PLAYERS]
;

hook OnPlayerClearData(playerid)
{
    DestroyDynamic3DTextLabelEx(PlayerAMe3DText[playerid]);
	PlayerAMeExpiry{playerid} = 0;
    return 1;
}

hook OnCharacterLoad(playerid)
{
    PlayerAMe3DText[playerid] = CreateDynamic3DTextLabel(" ", 0xFFFFFFFF, 0.0, 0.0, 0.30, 15.0, playerid, _, 1);
    return 1;
}

ptask OnAMeUpdateTimer[1000](playerid) 
{
    if(!Character_IsLogged(playerid))
		return 1;
    if(PlayerAMeExpiry{playerid} > 0)
	{
	   PlayerAMeExpiry{playerid}--;
	   if(!PlayerAMeExpiry{playerid})
	   {
		  UpdateDynamic3DTextLabelText(PlayerAMe3DText[playerid], 0xFFFFFFFF, " ");
	   }
	}
    return 1;
}

stock Character_SetAMeExpiryTime(playerid, timeAsSeconds)
{
    PlayerAMeExpiry{playerid} = timeAsSeconds;
}

stock Character_GetAMeExpiryTime(playerid)
{
    return PlayerAMeExpiry{playerid};
}

stock Character_AMe(playerid, text[], GLOBAL_TAG_TYPES:...)
{
	Character_SetAMeExpiryTime(playerid, 4); 
    new 
	   textString[148],
	   temp[148];
    format(textString, sizeof(textString), text, ___2);

    format(temp, sizeof(temp), "> %s ", textString);
    SendTwoLinesMessage(playerid, 0xD0AEEBFF, "%s %s ", Character_GetRolePlayName(playerid), textString); //0xD6C3E3FF
    
    format(temp, 120, "%s", textString);
	if(strlen(temp) > 43)
	    strins(temp, "-\n-", 43, sizeof(temp));
	if(strlen(temp) > 95)
	    strins(temp, "-\n-", 90, sizeof(temp));
	strins(temp, "* ", 0, sizeof(temp));
	strins(temp, " *", strlen(temp), sizeof(temp));

    UpdateDynamic3DTextLabelText(PlayerAMe3DText[playerid], 0xD0AEEBFF, temp);
    

    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    foreach(new i : Player)
    {
	   if(IsPlayerInRangeOfPoint(playerid, 25.0, x, y, z))
		  Streamer_Update(i);
    }
    return 1;
}