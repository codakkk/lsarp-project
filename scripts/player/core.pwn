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

#include <YSI_Coding\y_hooks>


hook OnCharacterPreSaveData(playerid, disconnect)
{
	if(disconnect)
	{
		if(pAdminDuty[playerid])
		{
			AC_SetPlayerHealth(playerid, 100.0);
			AC_SetPlayerArmour(playerid, 0.0);
		}
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnCharacterSaveData(playerid)
{
	Player_SaveOptions(playerid);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerCharacterLoad(playerid)
{
	Player_LoadOptions(playerid);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerClearData(playerid)
{
    Character_Clear(playerid);
    return 1;
}

hook OnCharacterDisconnected(playerid)
{
    Character_Save(playerid, _, 1);
    return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerSpawn(playerid)
{
	SetPlayerSkin(playerid, PlayerInfo[playerid][pSkin]);

	if(Character_GetFightingStyle(playerid) == 0)
		Character_SetFightingStyle(playerid, FIGHT_STYLE_NORMAL);
	
	SetPlayerFightingStyle(playerid, Character_GetFightingStyle(playerid));
    return Y_HOOKS_CONTINUE_RETURN_1;
}

stock Player_LoadOptions(playerid)
{
	inline OnLoad()
	{
		new t;
		if(cache_num_rows() > 0)
		{
			cache_get_value_index_int(0, 1, t);
			Player_SetHotKeysEnabled(playerid, t ? true : false);

			cache_get_value_index_int(0, 2, t);
			Player_SetInvModeEnabled(playerid, t ? true : false);

		}
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `account_options` WHERE AccountID = '%d'", Account_GetID(playerid));
	return 1;
}

stock Player_SaveOptions(playerid)
{
	mysql_tquery_f(gMySQL, 
	"INSERT INTO `account_options` (AccountID, HotKeys, InvMode) VALUES('%d', '%d', '%d') \
	ON DUPLICATE KEY UPDATE \
	HotKeys = VALUES(HotKeys), \
	InvMode = VALUES(InvMode);", 
	Account_GetID(playerid),
	Player_HasHotKeysEnabled(playerid),
	Player_HasInvModeEnabled(playerid));
}