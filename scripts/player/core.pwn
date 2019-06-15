#include <player\accessors.pwn>
#include <player\functions.pwn>
#include <player\textdraws.pwn>
#include <player\components\death_system.pwn>
#include <player\components\walk.pwn>
#include <player\components\chat.pwn>
#include <player\components\mask.pwn>
#include <player\components\inventory.pwn>
#include <player\components\drop.pwn>
#include <player\components\payday.pwn>
#include <player\components\interaction.pwn>
#include <player\components\vehicle.pwn>
#include <player\components\weapons.pwn>
#include <player\components\pickup.pwn>
#include <player\components\jail.pwn>
#include <player\components\animation.pwn>
#include <player\components\request.pwn>
#include <player\components\building_and_house.pwn>
#include <player\components\care.pwn>
#include <player\components\options.pwn>
#include <player\components\afk.pwn>
#include <player\components\damage_system.pwn>
#include <player\components\customanims.pwn>
#include <player\components\drugs.pwn>
#include <player\components\hunger.pwn>
#include <player\components\drag.pwn>
#include <player\components\spawn.pwn>
#include <YSI_Coding\y_hooks>


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

