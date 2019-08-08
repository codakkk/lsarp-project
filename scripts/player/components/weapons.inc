#include <YSI_Coding\y_hooks>

hook OnCharacterDisconnected(playerid)
{
	Character_SaveWeapons(playerid);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnCharacterLateLoad(playerid)
{
	Character_LoadWeapons(playerid);
	return 1;
}

stock Character_SaveWeapons(playerid)
{
	mysql_tquery(gMySQL, "START TRANSACTION;");
	//Character_DeleteAllWeapons(playerid);
	new query[255];
	new weaponid, ammo;
	for(new i = 0; i < 13; ++i)
	{
		Character_GetWeaponData(playerid, i, weaponid, ammo);
		if(!weaponid)
			continue;
		if(ammo == 0)
		{
			// removes weaponid from database if empty.
			format(query, sizeof(query), "DELETE FROM `character_weapons` WHERE CharacterID = '%d' AND WeaponID = '%d';", Character_GetID(playerid), weaponid);
		}
		else
		{
			format(query, sizeof(query), "INSERT INTO `character_weapons` (CharacterID, WeaponID, Ammo) VALUES('%d', '%d', '%d') ON DUPLICATE KEY UPDATE Ammo = VALUES(Ammo);", Character_GetID(playerid), weaponid, ammo);
		}
		mysql_pquery(gMySQL, query);
	}
	mysql_tquery(gMySQL, "COMMIT;");
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
			Character_GiveWeapon(playerid, w, a);
		}
		printf("%d weapons loaded for player %d.", rows, playerid);
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT WeaponID, Ammo FROM `character_weapons` WHERE CharacterID = '%d'", Character_GetID(playerid));
}

stock Character_HasWeaponInSlot(playerid, slotid)
{
	new weapon, ammo;
	Character_GetWeaponData(playerid, slotid, weapon, ammo);
	return weapon != 0 && ammo > 0 /*&& AC_AntiWeaponCheck(playerid, weapon, ammo)*/;
}

stock Character_GetWeaponInSlot(playerid, slotid)
{
	new weapon, ammo;
	Character_GetWeaponData(playerid, slotid, weapon, ammo);
	if(ammo > 0)
		return weapon;
	return 0;
}

stock Character_RemoveWeapon(playerid, weaponid)
{
	new 
		w[12] = {0, ...},
		a[12] = {0, ...}
	;

	for(new slot = 0; slot != 12; slot++)
	{
		GetPlayerWeaponData(playerid, slot, w[slot], a[slot]);
		if(w[slot] == weaponid)
		{
			w[slot] = 0;
			a[slot] = 0;
		}
	}

	static query[128];
	format(query, sizeof(query), "DELETE FROM `character_weapons` WHERE WeaponID = '%d' AND CharacterID = '%d';", weaponid, Character_GetID(playerid));
	mysql_pquery(gMySQL, query);

	Character_ResetWeapons(playerid);

	for(new slot = 0; slot != 12; slot++)
	{
		Character_GiveWeapon(playerid, w[slot], a[slot]);
	}
	return 1;
}

stock Character_ResetWeapons(playerid)
{
	if(!IsPlayerConnected(playerid) || !Character_IsLogged(playerid))
		return 0;
	ResetPlayerWeapons(playerid);
	new query[256];
	format(query, sizeof(query), "DELETE FROM `character_weapons` WHERE CharacterID = '%d'", Character_GetID(playerid));
	mysql_pquery(gMySQL, query);
	return 1;
}

stock Character_GiveWeapon(playerid, weaponid, ammo)
{
	if(!IsPlayerConnected(playerid) || !Character_IsLogged(playerid))
		return 0;
	GivePlayerWeapon(playerid, weaponid, ammo);
	Character_SaveWeapons(playerid);
	return 1;
}

stock Character_GetWeapon(playerid)
{
	new weaponid, a;
	Character_GetCurrentWeaponData(playerid, weaponid, a);
	return weaponid;
	//return AntiCheatGetWeapon(playerid);
}

stock Character_GetCurrentWeaponData(playerid, &weaponid, &ammo)
{
	return Character_GetWeaponData(playerid, Weapon_GetSlot(AntiCheatGetWeapon(playerid)), weaponid, ammo);
}

stock Character_HasWeapon(playerid, weaponid)
{
	new slotid = Weapon_GetSlot(weaponid), acW, acA;
	return Character_GetWeaponData(playerid, slotid, acW, acA) && weaponid == acW;
}

stock Character_GetAmmo(playerid)
{
	new weaponid, a;
	Character_GetCurrentWeaponData(playerid, weaponid, a);
	return a;//GetPlayerAmmo(playerid);
}

stock Character_SetWeaponAmmo(playerid, weaponid, ammo)
{
	if(!IsPlayerConnected(playerid) || !Character_IsLogged(playerid))
		return 0;
	new slotid = Weapon_GetSlot(weaponid);
	SetPlayerAmmo(playerid, weaponid, ammo);
	Character_SaveWeapons(playerid);
	return 1;
}

stock Character_GetWeaponData(playerid, slotid, &weaponid, &ammo)
{
	if(!IsPlayerConnected(playerid) || !Character_IsLogged(playerid))
		return 0;
	new res = AntiCheatGetWeaponData(playerid, slotid, weaponid, ammo);
	return res;
}