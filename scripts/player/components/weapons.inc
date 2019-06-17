#include <YSI_Coding\y_hooks>

hook OnCharacterSaveData(playerid)
{
	Character_SaveWeapons(playerid);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

stock Character_SaveWeapons(playerid)
{
	//Character_DeleteAllWeapons(playerid);
	new query[255];
	new weaponid, ammo;
	for(new i = 0; i < 13; ++i)
	{
		AC_GetPlayerWeaponData(playerid, i, weaponid, ammo);
		if(weaponid == 0/* || !AC_AntiWeaponCheck(playerid, weaponid, ammo)*/)
			continue;
		format(query, sizeof(query), "INSERT INTO `player_weapons` (CharacterID, WeaponID, Ammo) VALUES('%d', '%d', '%d') ON DUPLICATE KEY UPDATE Ammo = VALUES(Ammo);", Character_GetID(playerid), weaponid, ammo);
		mysql_pquery(gMySQL, query);
	}
}

stock Character_LoadWeapons(playerid)
{
	inline OnLoad()
	{
		new rows = cache_num_rows(), w, a;
		for(new i = 0; i < rows; ++i)
		{
			cache_get_value_index_int(i, 0, w);
			cache_get_value_index_int(i, 1, a);
			if(! (0 <= w <= 46))
			{
				printf("Character_LoadWeapons(%d) failed. invalid weapon id (id %d)", playerid, w);
				continue;
			}
			AC_GivePlayerWeapon(playerid, w, a);
		}
		printf("Player %d weapons loaded.", playerid);
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT WeaponID, Ammo FROM `player_weapons` WHERE CharacterID = '%d'", Character_GetID(playerid));
}

stock Character_DeleteAllWeapons(playerid)
{
	new query[256];
	format(query, sizeof(query), "DELETE FROM `player_weapons` WHERE CharacterID = '%d'", Character_GetID(playerid));
	mysql_pquery(gMySQL, query);
	return 1;
}

stock Character_HasWeaponInSlot(playerid, slotid)
{
	new weapon, ammo;
	AC_GetPlayerWeaponData(playerid, slotid, weapon, ammo);
	return weapon != 0 && ammo > 0 /*&& AC_AntiWeaponCheck(playerid, weapon, ammo)*/;
}

stock Character_GetWeaponInSlot(playerid, slotid)
{
	new weapon, ammo;
	GetPlayerWeaponData(playerid, slotid, weapon, ammo);
	if(ammo > 0)
		return weapon;
	return 0;
}