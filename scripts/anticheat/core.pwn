#include <YSI_Coding\y_hooks>

// Should I use Nex-AC? To add a better layer of security?


// Is this Timer expansive?
// probably yes, but still better than OnPlayerUpdate (called each tick)
ptask AntiCheatTimer[250](playerid) 
{
    if(!gAccountLogged[playerid] || !gCharacterLogged[playerid] /*|| AccountInfo[playerid][aAdmin] > 2*/)
        return 0;

    new 
        Float:hp, 
        Float:ac_hp,
        Float:armour,
        Float:ac_armour;
    
    GetPlayerHealth(playerid, hp);
    GetPlayerArmour(playerid, armour);

    AC_GetPlayerHealth(playerid, ac_hp);
    AC_GetPlayerArmour(playerid, ac_armour);
    //printf("%f - %f", hp, ac_hp);
    if(hp > ac_hp)
    {
        //AC_SetPlayerHealth(playerid, ac_hp);
    }
    else // Lower (like for falling damage or weapons damage)
    {
        //AC_SetPlayerHealth(playerid, hp);
    }

    if(armour > ac_armour)
    {
        //AC_SetPlayerArmour(playerid, ac_armour);
    }
    else // like hp
    {
        //AC_SetPlayerArmour(playerid, armour);
    }

    if(GetPlayerMoney(playerid) != AC_GetPlayerMoney(playerid))
    {
        AC_GivePlayerMoney(playerid, 0); // Resets player money
    }

	if(!pDeathState[playerid])
	{
		new currentWeapon = GetPlayerWeapon(playerid),
		currentAmmo = GetPlayerAmmo(playerid),
		slot = Weapon_GetSlot(currentWeapon),
		anticheatWeapon = ACInfo[playerid][acWeapons][slot],
		anticheatAmmo = ACInfo[playerid][acAmmo][slot];
		if(currentWeapon != anticheatWeapon && slot != 0 && currentWeapon != 0 && currentWeapon != WEAPON_BOMB && currentWeapon != WEAPON_PARACHUTE)
		{
			if(35 <= currentWeapon <= 39)
			{
				KickEx(playerid);
			}
			else if(slot == 1 || slot == 10)
				RemovePlayerWeapon(playerid, currentWeapon);
			else
				SetPlayerAmmo(playerid, slot, 0);
			AC_Detect(playerid, AC_WEAPON_HACK);
			SetPlayerArmedWeapon(playerid, 0);
		}
		else if(currentWeapon == anticheatWeapon && currentWeapon != 0 && 2 <= slot <= 8 && !IsPlayerInAnyVehicle(playerid))
		{
			if(currentAmmo > anticheatAmmo)
			{
				if(anticheatAmmo > 0)
				{
					SetPlayerArmedWeapon(playerid, 0);
					SetPlayerAmmo(playerid, currentWeapon, anticheatAmmo);
				}
				else
				{
					SetPlayerAmmo(playerid, slot, 0);
				}
				AC_Detect(playerid, AC_AMMO_HACK);
			}
			else if(currentAmmo < anticheatAmmo)
			{
				pAmmoSync{playerid}++;
				if(pAmmoSync{playerid} >= 5)
				{
					ACInfo[playerid][acAmmo][slot] = currentAmmo;
					pAmmoSync{playerid} = 0;
				}
			}
			else
				pAmmoSync{playerid} = 0;
		}

		static lastWeapon;
		currentWeapon = GetPlayerWeapon(playerid);

		if(lastWeapon != currentWeapon)
		{
			ACInfo[playerid][acShotCounter] = 0;
			lastWeapon = currentWeapon;
		}
	}

    // Anti-cheats we can pass-on if we're admins goes here.
    if(AccountInfo[playerid][aAdmin] > 1)
        return 1;
    
    if(GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_USEJETPACK && !gPlayerJetpack[playerid])
    {
        ClearAnimations(playerid, 1);
        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);
        SetPlayerPos(playerid, x, y, z);
    }
    return 1;
}

CMD:trigger(playerid, params[])
{
	GivePlayerWeapon(playerid, 24, 1);
	return 1;
}

stock AC_GetPlayerMoney(playerid)
{
    return PlayerInfo[playerid][pMoney];
}

stock AC_SetPlayerSkin(playerid, skinid, reason[] = "")
{
    #pragma unused reason
    if(!gCharacterLogged[playerid])
        return 0;
    SetPlayerSkin(playerid, skinid);
    PlayerInfo[playerid][pSkin] = skinid;
    return 1;
}

stock AC_GivePlayerMoney(playerid, money, reason[] = "")
{
    if(!gAccountLogged[playerid] || !gCharacterLogged[playerid])
        return 0;
    PlayerInfo[playerid][pMoney] += money;
    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, PlayerInfo[playerid][pMoney]);

    // Should I log here?
    #pragma unused reason
    return 1;
}

stock AC_ResetPlayerMoney(playerid, reason[]="")
{
    if(!gCharacterLogged[playerid])
        return 0;
    PlayerInfo[playerid][pMoney] = 0;
    ResetPlayerMoney(playerid);
    return 1;
}

stock AC_SetPlayerHealth(playerid, Float:health)
{
    PlayerInfo[playerid][pHealth] = health;
    SetPlayerHealth(playerid, health); 
    return 1;
}

stock AC_GetPlayerHealth(playerid, &Float:h)
{
    if(!gCharacterLogged[playerid])
        return 0;
    h = PlayerInfo[playerid][pHealth];
    return 1;
}

stock AC_SetPlayerArmour(playerid, Float:armour)
{
    PlayerInfo[playerid][pArmour] = armour;
    SetPlayerArmour(playerid, armour);
    return 1;
}

stock AC_GetPlayerArmour(playerid, &Float:a)
{
    if(!gCharacterLogged[playerid])
        return 0;
    a = PlayerInfo[playerid][pArmour];
    return 1;
}

stock AC_GivePlayerWeapon(playerid, weaponid, ammo)
{
	new slot = Weapon_GetSlot(weaponid);
	if(2 <= slot <= 8) // Requires ammo?
	{
		if(ACInfo[playerid][acWeapons][slot] == weaponid)
		{
			if(ACInfo[playerid][acAmmo][slot] > 0)
				ACInfo[playerid][acAmmo][slot] += ammo;
			else
				ACInfo[playerid][acAmmo][slot] = ammo;
		}
		else
		{
			ACInfo[playerid][acAmmo][slot] = ammo;
		}
	}
	ACInfo[playerid][acWeapons][slot] = weaponid;
    return GivePlayerWeapon(playerid, weaponid, ammo);
}

stock AC_ResetPlayerWeapons(playerid)
{
	for(new i = 0; i < 13; ++i)
	{
		ACInfo[playerid][acWeapons][i] = ACInfo[playerid][acAmmo][i] = 0;
	}
    ResetPlayerWeapons(playerid);
    return 1;
}

stock AC_RemovePlayerWeapon(playerid, weaponid)
{
	return RemovePlayerWeapon(playerid, weaponid);
}

stock AC_GetPlayerAmmo(playerid)
{
    return GetPlayerAmmo(playerid);
}

stock AC_SetPlayerAmmo(playerid, weaponid, ammo)
{
	return SetPlayerAmmo(playerid, weaponid, ammo);
}

stock AC_GivePlayerAmmo(playerid, weaponid, ammo)
{
	new w, a;
	GetPlayerWeaponData(playerid, Weapon_GetSlot(weaponid), w, a);
	return AC_SetPlayerAmmo(playerid, weaponid, a + ammo);
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