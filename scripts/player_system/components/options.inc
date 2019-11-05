#include <YSI_Coding\y_hooks>

static enum e_Bit1_OptionsData 
{
    e_pHotKeys,
	e_pInvMode, // 0: Dialog - 1: Chat
	e_pToggleHUD,
	e_pVehMode, // 0: Dialog - 1: Chat
	e_pCheatAlerts
};
static BitArray:OptionsBitSet[e_Bit1_OptionsData]<MAX_PLAYERS>;

hook OnGameModeInit()
{
	mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `account_options` ( \
						`account_id` int(11) NOT NULL, \
						`hotkeys` int(11) NOT NULL DEFAULT '0', \
						`inv_mode` int(11) NOT NULL DEFAULT '0', \
						`veh_mode` int(11) NOT NULL DEFAULT '0', \
						PRIMARY KEY (`account_id`), \
						CONSTRAINT `AccountOptions` FOREIGN KEY (`account_id`) REFERENCES `accounts` (`id`) ON DELETE CASCADE \
						) ENGINE=InnoDB DEFAULT CHARSET=latin1;");
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
		if(cache_num_rows() <= 0)
		{
			mysql_pquery_f(gMySQL, "INSERT INTO `account_options` (account_id, hotkeys, inv_mode, veh_mode) VALUES('%d', '%d', '%d', '%d')", Account_GetID(playerid), 0, 0, 0);
			return 1;
		}
		new t;
		
		cache_get_value_name_int(0, "hotkeys", t);
		Player_SetHotKeysEnabled(playerid, t ? true : false);

		cache_get_value_name_int(0, "inv_mode", t);
		Player_SetInvModeEnabled(playerid, t ? true : false);

		cache_get_value_name_int(0, "veh_mode", t);
		Player_SetVehModeEnabled(playerid, t ? true : false);
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `account_options` WHERE account_id = '%d'", Account_GetID(playerid));
	return 1;
}

stock Player_SaveOptions(playerid)
{
	mysql_pquery_f(gMySQL, 
	"INSERT INTO `account_options` (account_id, hotkeys, inv_mode) VALUES('%d', '%d', '%d') \
	ON DUPLICATE KEY UPDATE \
	hotkeys = VALUES(hotkeys), \
	inv_mode = VALUES(inv_mode), \
	veh_mode = VALUES(veh_mode);", 
	Account_GetID(playerid),
	Player_HasHotKeysEnabled(playerid)?1:0,
	Player_HasInvModeEnabled(playerid)?1:0,
	Player_HasVehModeEnabled(playerid)?1:0);
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

stock Player_SetVehModeEnabled(playerid, bool:enable)
{
	Bit_Set(OptionsBitSet[e_pVehMode], playerid, enable);
	Player_SaveOptions(playerid);
}

stock Player_HasVehModeEnabled(playerid)
{
	return Bit_Get(OptionsBitSet[e_pVehMode], playerid);
}

stock Player_SetCheatAlertsEnabled(playerid, bool:enable)
{
	Bit_Set(OptionsBitSet[e_pCheatAlerts], playerid, !enable);
}

stock Player_HasCheatAlertsEnabled(playerid)
{
	return !Bit_Get(OptionsBitSet[e_pCheatAlerts], playerid);
}