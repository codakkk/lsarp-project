#include <YSI_Coding\y_hooks>

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

stock Player_LoadOptions(playerid)
{
	inline OnLoad()
	{
		new t;
		if(cache_num_rows() > 0)
		{
			cache_get_value_index_int(0, 1, t);
			Account_SetHotKeysEnabled(playerid, t ? true : false);

			cache_get_value_index_int(0, 2, t);
			Account_SetInvModeEnabled(playerid, t ? true : false);

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
	Account_HasHotKeysEnabled(playerid)?1:0,
	Account_HasInvModeEnabled(playerid)?1:0);
}