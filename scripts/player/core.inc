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
#include <player\components\desync_checker>
// EFFECTS 
#include <player\components\effects\mask>
#include <player\components\effects\drag>
#include <player\components\effects\stun>
#include <player\components\effects\ame>

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

ptask SaveCharacterTimer[14000](playerid)
{
    if(!Character_IsLogged(playerid))
	   return 0;
    Character_Save(playerid);
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