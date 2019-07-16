#include <YSI_Coding\y_hooks>

hook OnPlayerCharacterLoad(playerid)
{
    Character_LoadPrivileges(playerid);
    return 1;
}

stock Character_SavePrivileges(playerid)
{
	// This should never happen.
    if(!Character_IsLogged(playerid))
	   return 0;
	mysql_tquery_f(gMySQL, 
		"INSERT INTO `character_privileges` (character_id, chop_shop) VALUES('%d', '%d') \
		ON DUPLICATE KEY UPDATE \
		chop_shop = VALUES(chop_shop);", 
		Character_HasChopShopPrivileges(playerid),
		Character_GetID(playerid));
	return 1;
}

stock Character_LoadPrivileges(playerid)
{
	// This should never happen.
    if(!Character_IsLogged(playerid))
	   return 0;
	inline OnLoad()
	{
		if(cache_num_rows() <= 0)
			return 0;
		new temp = 0;
		
		cache_get_value_index_int(0, 1, temp);
		Character_SetChopShopPrivileges(playerid, temp?true:false);
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `character_privileges` WHERE character_id = '%d'", Character_GetID(playerid));	
	return 1;
}