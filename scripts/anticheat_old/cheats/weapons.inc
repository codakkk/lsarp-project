#include <YSI_Coding\y_hooks>

stock AC_GivePlayerWeapon(playerid, weaponid, ammo)
{
	new slot = Weapon_GetSlot(weaponid);
	if(2 <= slot <= 8) // Requires ammo?
	{
		if(ACInfo[playerid][acWeapons][slot] == weaponid)
		{
			if(ACInfo[playerid][acAmmo][slot] > 0)
			{
				ACInfo[playerid][acAmmo][slot] += ammo;
			}
			else
			{
				ACInfo[playerid][acAmmo][slot] = ammo;
			}
		}
		else
		{
			ACInfo[playerid][acAmmo][slot] = ammo;
		}
	}
	ACInfo[playerid][acWeapons][slot] = weaponid;
	return GivePlayerWeapon(playerid, weaponid, ammo);
}

// Reset all playerid's weapons.
// Weapons are removed from database too.
stock AC_ResetPlayerWeapons(playerid, bool:deleteFromDatabase = false)
{
	for(new i = 0; i < 13; ++i)
	{
		ACInfo[playerid][acWeapons][i] = ACInfo[playerid][acAmmo][i] = 0;
	}
	ResetPlayerWeapons(playerid);
	if(deleteFromDatabase)
		Character_DeleteAllWeapons(playerid);
	return 1;
}

stock AC_RemovePlayerWeapon(playerid, weaponid)
{
	new plyWeapons[12] = 0;
	new plyAmmo[12] = 0;
	for(new slot = 0; slot != 12; slot++)
	{
		new wep, ammo;
		AC_GetPlayerWeaponData(playerid, slot, wep, ammo);

		if(wep != weaponid && ammo != 0)
		{
			AC_GetPlayerWeaponData(playerid, slot, plyWeapons[slot], plyAmmo[slot]);
		}
		else if(wep == weaponid)
		{
			//ACInfo[playerid][acWeapons][slot] = 0; // AntiCheat
			//ACInfo[playerid][acAmmo][slot] = 0; // AntiCheat
		}
	}
	new query[128];
	format(query, sizeof(query), "DELETE FROM `character_weapons` WHERE character_id = '%d' AND weapon_id = '%d'", Character_GetID(playerid), weaponid);
	mysql_pquery(gMySQL, query);

	ResetPlayerWeapons(playerid);
	for(new slot = 0; slot != 12; slot++)
	{
		if(plyAmmo[slot] != 0)
		{
			GivePlayerWeapon(playerid, plyWeapons[slot], plyAmmo[slot]);
		}
	}
	return 1;
}

stock AC_RemovePlayerWeaponBySlot(playerid, slotid)
{
	new w, a;
	GetPlayerWeaponData(playerid, slotid, w, a);
	Character_RemoveWeapon(playerid, w);
	return 1;
}

stock AC_GetPlayerAmmo(playerid)
{
	new ammo = GetPlayerAmmo(playerid);
	if(!AC_AntiWeaponCheck(playerid, GetPlayerWeapon(playerid), ammo))
		return 0;
	return ammo;//ACInfo[playerid][acAmmo][slot];
}

stock AC_AntiWeaponCheck(playerid, weaponid, &ammo)
{
	new 
		slot = Weapon_GetSlot(weaponid),
		ac_w = ACInfo[playerid][acWeapons][slot],
		ac_a = ACInfo[playerid][acAmmo][slot];
	if(weaponid != ac_w && slot != 0 && weaponid != 0 && weaponid != 40 && weaponid != 46)
		return 0;
	if(weaponid == ac_w && 2 <= slot <= 8 && ammo > ac_a)
	{
		if(ac_a > 0)
			ammo = ac_a;
		else 
			return 0;
	}
	return 1;
}