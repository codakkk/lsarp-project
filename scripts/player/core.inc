#include <player\accessors>
#include <player\functions>
#include <player\textdraws>
#include <player\components\editobject>
#include <player\components\death_system>
#include <player\components\walk>
#include <player\components\chat>
#include <player\components\inventory>
#include <player\components\drop>
#include <player\components\payday>
#include <player\components\interaction>
#include <player\components\vehicle>
#include <player\components\weapons>
#include <player\components\pickup>
#include <player\components\jail>
#include <player\components\animation>
#include <player\components\request>
#include <player\components\property>
#include <player\components\care>
#include <player\components\options>
#include <player\components\afk>
#include <player\components\damage_system>
#include <player\components\customanims>
#include <player\components\drugs>
#include <player\components\hunger>

// EFFECTS 
#include <player\components\effects\mask>
#include <player\components\effects\drag>
#include <player\components\effects\stun>

// OTHERS
#include <player\components\spawn>
#include <YSI_Coding\y_hooks>

forward OnPlayerVirtualWorldChange(playerid, oldWorld, newWorld);

hook OnPlayerClearData(playerid)
{
    Character_Clear(playerid);
	Character_ResetBitState(playerid);
    return 1;
}

hook OnCharacterDisconnected(playerid)
{
    Character_Save(playerid, _, true);
    return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnCharacterSpawn(playerid)
{
	SetPlayerSkin(playerid, PlayerInfo[playerid][pSkin]);

	if(Character_GetFightingStyle(playerid) == 0)
		Character_SetFightingStyle(playerid, FIGHT_STYLE_NORMAL);
	
	SetPlayerFightingStyle(playerid, Character_GetFightingStyle(playerid));
    return Y_HOOKS_CONTINUE_RETURN_1;
}

ptask OnCharacterUpdateTimer[1000](playerid)
{
	if(!Character_IsLogged(playerid))
		return 1;

	if(gPlayerAMeExpiry[playerid] > 0)
	{
	   gPlayerAMeExpiry[playerid]--;
	   if(!gPlayerAMeExpiry[playerid])
	   {
		  UpdateDynamic3DTextLabelText(gPlayerAMe3DText[playerid], 0xFFFFFFFF, " ");
	   }
	}

	//Clock System
    new hour, minute, second, string[64];
    gettime(hour, minute, second);
	// hour += TIME_ZONE; // TIME_ZONE = 0
	if(hour > 24) hour -= 24;
	if(hour < 0) hour += 24;
	format(string,25,"%02d:%02d:%02d",hour, minute, second);
    TextDrawSetString(Clock, string);
	return 1;
}

stock Character_SetVirtualWorld(playerid, virtualWorld)
{
	new old = Character_GetVirtualWorld(playerid);
	SetPlayerVirtualWorld(playerid, virtualWorld);
	CallLocalFunction(#OnPlayerVirtualWorldChange, "ddd", playerid, old, virtualWorld);
	return 1;
}

stock Character_GetVirtualWorld(playerid)
{
    return GetPlayerVirtualWorld(playerid);
}