#include <YSI_Coding\y_hooks>

static 
	PlayerWeapons[MAX_PLAYERS][13 char],
	PlayerAmmo[MAX_PLAYERS][13]
;

ptask OnWeaponHackTimer[500](playerid)
{
	if(!Character_IsLogged(playerid) || Character_IsDead(playerid))
		return 1;
	new currentWeapon = GetPlayerWeapon(playerid);
	if(currentWeapon == -1)
		return 1;
	new currentSlot = Weapon_GetSlot(currentWeapon);
	for(new slot = 0, ac_weapon = 0, ac_ammo = 0; slot != 12; ++slot)
	{
		GetPlayerWeaponData(playerid, slot, ac_weapon, ac_ammo);

		if(slot != currentSlot)
		{
			if(ac_weapon != PlayerWeapons[playerid]{slot})
			{
				Character_RemoveWeapon(playerid, ac_weapon);
				Character_GiveWeapon(playerid, PlayerWeapons[playerid]{slot}, PlayerAmmo[playerid]{slot});
				SendMessageToAdmins(true, COLOR_ERROR, "Il giocatore %s (%d) è sospetto di Weapon Hack.", Character_GetOOCName(playerid), playerid);
			}
		}
		else if(ac_weapon == 38 || 22 <= ac_weapon <= 34)
		{
			if(ac_ammo > PlayerAmmo[playerid][slot] || ac_ammo < 0 <= PlayerAmmo[playerid][slot])
			{
				SendMessageToAdmins(true, COLOR_ERROR, "Il giocatore %s (%d) è sospetto di Ammo Hack.", Character_GetOOCName(playerid), playerid);
				Character_RemoveWeapon(playerid, ac_weapon);
				Character_GiveWeapon(playerid, PlayerWeapons[playerid]{slot}, PlayerAmmo[playerid]{slot});
			}
		}
	}
	return 1;
}

hook OnPlayerClearData(playerid)
{
	for(new i = 0; i < 13; ++i)
	{
		PlayerWeapons[playerid]{i} = PlayerAmmo[playerid][i] = 0;
	}
	return 1;
}

hook OnCharacterSaveData(playerid)
{
	Character_SaveWeapons(playerid);
	return Y_HOOKS_CONTINUE_RETURN_1;
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
		if(weaponid == 0 || ammo == 0)
		{
			// probably causes weapons bug on respawn.
			format(query, sizeof(query), "DELETE FROM `player_weapons` WHERE weaponid = '%d' AND CharacterID = '%d';", weaponid, Character_GetID(playerid));
		}
		else
		{
			format(query, sizeof(query), "INSERT INTO `player_weapons` (CharacterID, WeaponID, Ammo) VALUES('%d', '%d', '%d') ON DUPLICATE KEY UPDATE Ammo = VALUES(Ammo);", Character_GetID(playerid), weaponid, ammo);
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
		plyWeapons[12] = {0, ...},
		plyAmmo[12] = {0, ...}
	;
	for(new slot = 0; slot != 12; slot++)
	{
		new wep, ammo;
		Character_GetWeaponData(playerid, slot, wep, ammo);
		if(wep != weaponid)
		{
			plyWeapons[slot] = wep;
			plyAmmo[slot] = ammo;
		}
	}

	new query[128];
	format(query, sizeof(query), "DELETE FROM `player_weapons` WHERE CharacterID = '%d' AND WeaponID = '%d'", Character_GetID(playerid), weaponid);
	mysql_pquery(gMySQL, query);

	ResetPlayerWeapons(playerid);

	for(new slot = 0; slot != 12; slot++)
	{
		//if(plyWeapons[slot] != 0 && plyAmmo[slot] > 0)
		//{
			printf("Give: %d - %d", plyWeapons[slot], plyAmmo[slot]);
		Character_GiveWeapon(playerid, plyWeapons[slot], plyAmmo[slot]);
		//}
	}
	return 1;
}

stock Character_GiveWeapon(playerid, weaponid, ammo)
{
	if(0 <= weaponid <= 18 || 22 <= weaponid <= 46)
	{
		new weaponSlot = Weapon_GetSlot(weaponid);
		if(16 <= weaponid <= 18 || 22 <= weaponid <= 43)
		{
			if(3 <= weaponSlot <= 5 || PlayerWeapons[playerid]{weaponSlot} == weaponid)
			{
				PlayerAmmo[playerid][weaponSlot] += ammo;
			}
			else
			{
				PlayerWeapons[playerid]{weaponSlot} = weaponid;
				PlayerAmmo[playerid][weaponSlot] = ammo;
			}
		}

	}
	return GivePlayerWeapon(playerid, weaponid, ammo);
}

stock Character_GetWeapon(playerid)
{
	return GetPlayerWeapon(playerid);
}

stock Character_GetAmmo(playerid)
{
	return GetPlayerAmmo(playerid);
}

stock Character_GetWeaponData(playerid, slotid, &weaponid, &ammo)
{
	if(!IsPlayerConnected(playerid) || !Character_IsLogged(playerid))
		return 0;
	weaponid = PlayerWeapons[playerid]{slotid};
	ammo = PlayerAmmo[playerid][slotid];
	return 1;
}