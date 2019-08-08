#include <YSI_Coding\y_hooks>

static enum e_Bit1_OptionsData 
{
    e_pHotKeys,
	e_pInvMode, // 0: Dialog - 1: Chat
	e_pToggleHUD
};
static BitArray:OptionsBitSet[e_Bit1_OptionsData]<MAX_PLAYERS>;

hook OnAccountCreated(playerid, account_db_id)
{
	mysql_tquery_f(gMySQL, "INSERT INTO `account_options` (account_id, hotkeys, inv_mode) VALUES('%d', '%d', '%d')", Account_GetID(playerid), 0, 0);
	return 1;
}

hook OnCharacterLoad(playerid)
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
			Player_SetHotKeysEnabled(playerid, t ? true : false);

			cache_get_value_index_int(0, 2, t);
			Player_SetInvModeEnabled(playerid, t ? true : false);

		}
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `account_options` WHERE account_id = '%d'", Account_GetID(playerid));
	return 1;
}

stock Player_SaveOptions(playerid)
{
	mysql_tquery_f(gMySQL, 
	"INSERT INTO `account_options` (account_id, hotkeys, inv_mode) VALUES('%d', '%d', '%d') \
	ON DUPLICATE KEY UPDATE \
	hotkeys = VALUES(hotkeys), \
	inv_mode = VALUES(inv_mode);", 
	Account_GetID(playerid),
	Player_HasHotKeysEnabled(playerid)?1:0,
	Player_HasInvModeEnabled(playerid)?1:0);
}

stock Player_SetHotKeysEnabled(playerid, bool:enable)
{
	Bit_Set(OptionsBitSet[e_pHotKeys], playerid, enable);
	Player_SaveOptions(playerid);
}

stock Player_HasHotKeysEnabled(playerid)
{
	return Bit_Get(OptionsBitSet[e_pHotKeys], playerid);
}

stock Player_SetInvModeEnabled(playerid, bool:enable)
{
	Bit_Set(OptionsBitSet[e_pInvMode], playerid, enable);
	Player_SaveOptions(playerid);
}

stock Player_HasInvModeEnabled(playerid)
{
	return Bit_Get(OptionsBitSet[e_pInvMode], playerid);
}

stock Player_SetHUDEnabled(playerid, bool:enable)
{
	Bit_Set(OptionsBitSet[e_pToggleHUD], playerid, !enable);
	Player_SaveOptions(playerid);
}

stock Player_HasHUDEnabled(playerid)
{
	return !Bit_Get(OptionsBitSet[e_pToggleHUD], playerid);
}
