#include <YSI_Coding\y_hooks>

hook OnCharacterClearData(playerid)
{
	ResetPlayerWeapons(playerid);
	return 1;
}

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

/*ptask SavePlayerWeaponsTask[1000](playerid) 
{
	if(!Character_IsLogged(playerid))
		return 0;
	Character_SaveWeapons(playerid);
	return 1;
}*/

stock Character_SaveWeapons(playerid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	mysql_tquery(gMySQL, "START TRANSACTION;");
	//Character_DeleteAllWeapons(playerid);
	new query[255];
	new weaponid, ammo;
	for(new i = 0; i < 13; ++i)
	{

		Character_GetWeaponData(playerid, i, weaponid, ammo);
		
		if(weaponid <= 0)
			continue;
		
		if(i == 2 && Character_HasTaser(playerid))
		{
			Character_GetCachedWeaponTaser(playerid, weaponid, ammo);
			//printf("Cached taser: %d - %d", weaponid, ammo);
		}
		else if(i == 3 && Character_HasBeanBag(playerid))
		{
			Character_GetCachedWeaponBean(playerid, weaponid, ammo);
			//printf("Cached bean: %d - %d", weaponid, ammo);
		}

		//printf("Saving: %d - Ammo: %d", weaponid, ammo);
		
		if(i > 1 && i != 10 && ammo <= 0)
		{
			format(query, sizeof(query), "DELETE FROM `character_weapons` WHERE character_id = '%d' AND weapon_id = '%d';", Character_GetID(playerid), weaponid);
		}
		else
		{
			format(query, sizeof(query), "INSERT INTO `character_weapons` (character_id, weapon_id, ammo) VALUES('%d', '%d', '%d') ON DUPLICATE KEY UPDATE ammo = VALUES(ammo);", Character_GetID(playerid), weaponid, ammo);
			printf("Saving weapon id %d with %d ammo for character id %d.", weaponid, ammo, Character_GetID(playerid));
		}
		mysql_pquery(gMySQL, query);
	}
	mysql_tquery(gMySQL, "COMMIT;");
	return 1;
}

stock Character_LoadWeapons(playerid)
{
	inline OnLoad()
	{
		new rows = cache_num_rows(), cid, w, a;
		for(new i = 0; i < rows; ++i)
		{
			cache_get_value_name_int(i, "character_id", cid);
			
			if(cid != Character_GetID(playerid))
				continue;
			
			cache_get_value_name_int(i, "weapon_id", w);
			cache_get_value_name_int(i, "ammo", a);
			if(! (0 <= w <= 46))
			{
				printf("Character_LoadWeapons(%d) failed. invalid weapon id (id %d)", playerid, w);
				continue;
			}
			ac_GivePlayerWeapon(playerid, w, a);
			//Character_GiveWeapon(playerid, w, a);
		}
		printf("%d weapons loaded for player %d (DBID: %d).", rows, playerid, Character_GetID(playerid));
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT character_id, weapon_id, ammo FROM `character_weapons` WHERE character_id = '%d'", Character_GetID(playerid));
}

stock Character_HasWeaponInSlot(playerid, slotid)
{
	new weapon, ammo;
	Character_GetWeaponData(playerid, slotid, weapon, ammo);
	return weapon != 0 && ammo > 0;
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
	/*new 
		w[12] = {0, ...},
		a[12] = {0, ...}
	;

	for(new slot = 0; slot != 12; slot++)
	{
		//GetPlayerWeaponData(playerid, slot, w[slot], a[slot]);
		Character_GetWeaponData(playerid, slot, w[slot], a[slot]);
		if(w[slot] == weaponid)
		{
			w[slot] = 0;
			a[slot] = 0;
		}
	}*/

	static query[128];
	format(query, sizeof(query), "DELETE FROM `character_weapons` WHERE weapon_id = '%d' AND character_id = '%d';", weaponid, Character_GetID(playerid));
	mysql_pquery(gMySQL, query);

	ac_RemovePlayerWeapon(playerid, weaponid);

	/*ResetPlayerWeapons(playerid);

	for(new slot = 0; slot != 12; slot++)
	{
		Character_GiveWeapon(playerid, w[slot], a[slot]);
	}*/
	return 1;
}

stock Character_ResetWeapons(playerid)
{
	if(!IsPlayerConnected(playerid) || !Character_IsLogged(playerid))
		return 0;
	ResetPlayerWeapons(playerid);
	new query[256];
	format(query, sizeof(query), "DELETE FROM `character_weapons` WHERE character_id = '%d'", Character_GetID(playerid));
	mysql_pquery(gMySQL, query);
	return 1;
}

stock Character_GiveWeapon(playerid, weaponid, ammo)
{
	if(!Character_IsLogged(playerid))
		return 0;
	ac_GivePlayerWeapon(playerid, weaponid, ammo);
	TriggerSaveAfterDelay(playerid);
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
	if(!Character_IsLogged(playerid))
		return 0;
	new slotid = Weapon_GetSlot(weaponid);
	SetPlayerAmmo(playerid, weaponid, ammo);
	TriggerSaveAfterDelay(playerid);
	return 1;
}

stock Character_GetWeaponData(playerid, slotid, &weaponid, &ammo)
{
	if(!Character_IsLogged(playerid))
		return 0;
	new res = AntiCheatGetWeaponData(playerid, slotid, weaponid, ammo);
	return res;
}

// Saved after delay  because nex-ac sets weapons after a delay
static TriggerSaveAfterDelay(playerid)
{
	defer SaveWeaponAfterDelay(playerid);
}

timer SaveWeaponAfterDelay[500](playerid) 
{
	Character_SaveWeapons(playerid);
}