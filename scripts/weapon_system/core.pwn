#include <YSI\y_hooks>

hook OnGameModeInit()
{
    printf("Initializing Weapons...");
    Weapon_InitializeType(WEAPON_COLT45, gItem_LightAmmo, 25);
    Weapon_InitializeType(WEAPON_SILENCED, gItem_LightAmmo, 30);
    Weapon_InitializeType(WEAPON_DEAGLE, gItem_HeavyAmmo, 45);

    Weapon_InitializeType(WEAPON_SHOTGUN, gItem_BuckShotAmmo, 50);
    Weapon_InitializeType(WEAPON_SAWEDOFF, gItem_BuckShotAmmo, 15);
    Weapon_InitializeType(WEAPON_SHOTGSPA, gItem_BuckShotAmmo, 15);

    Weapon_InitializeType(WEAPON_UZI, gItem_LightAmmo, 15);
    Weapon_InitializeType(WEAPON_TEC9, gItem_LightAmmo, 15);
    Weapon_InitializeType(WEAPON_MP5, gItem_LightAmmo, 25);

    Weapon_InitializeType(WEAPON_AK47, gItem_HeavyAmmo, 40);
    Weapon_InitializeType(WEAPON_M4, gItem_HeavyAmmo, 35);

    Weapon_InitializeType(WEAPON_RIFLE, gItem_RifleAmmo, 70);
    Weapon_InitializeType(WEAPON_SNIPER, gItem_RifleAmmo, 500);
    printf("Weapons initialized");
    return 1;
}

hook OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart)
{
    if(pAdminDuty[playerid] && AccountInfo[playerid][aAdmin] > 1)
        return 1;

    if(issuerid != INVALID_PLAYER_ID && GetPlayerWeapon(issuerid) != weaponid/*!AC_HasPlayerWeapon(issuerid, weapon, ammo)*/ && AccountInfo[playerid][aAdmin] < 1)
        return 1;

    new 
        Float:weaponDamage,
        Float:currentHealth,
        Float:currentArmour;

    //
    weaponDamage = Weapon_GetDamage(weaponid);
    
    if(weaponDamage == 0)
        weaponDamage = amount;

    AC_GetPlayerHealth(playerid, currentHealth);
    AC_GetPlayerArmour(playerid, currentArmour);
    if(weaponDamage)
    {
        if(currentArmour >= weaponDamage) 
            AC_SetPlayerArmour(playerid, weaponDamage);
        else if(currentArmour && currentArmour < weaponDamage)
        {
            AC_SetPlayerArmour(playerid, 0);
            AC_SetPlayerHealth(playerid, (weaponDamage - currentArmour) - currentHealth);
        }
        else 
            AC_SetPlayerHealth(playerid, currentHealth - weaponDamage);
    }
    return 1;
}

stock Weapon_InitializeType(weaponid, ammoItem, damages, damageRatio = 1)
{
    WeaponInfo[weaponid][wInitialized] = 1;
    WeaponInfo[weaponid][wDamage] = damages;
    WeaponInfo[weaponid][wDamageRatio] = damageRatio;
    WeaponInfo[weaponid][wAmmoItem] = ammoItem;
}

stock Weapon_GetDamage(weaponid)
{
    if(weaponid <= 0 || weaponid >= 47 || !WeaponInfo[weaponid][wInitialized])
        return 0;
    return WeaponInfo[weaponid][wDamage];
}

stock Weapon_GetAmmoType(weaponid)
{
    if(weaponid <= 0 || weaponid >= 47 || !WeaponInfo[weaponid][wInitialized])
        return gItem_LightAmmo;
    return WeaponInfo[weaponid][wAmmoItem];
}

stock bool:Weapon_RequireAmmo(weaponid)
{
    if(
        Weapon_GetSlot(weaponid) == 0 || 
        Weapon_GetSlot(weaponid) == 1 || 
        Weapon_GetSlot(weaponid) == 10 ||
        weaponid == 40 ||
        weaponid == 44 || weaponid == 45 || weaponid == 46) 
        return false;

    return true;
}

stock Weapon_GetSlot(weaponid)
{
    switch(weaponid)
    {
        case 0 .. 1: return 0;
        case 2 .. 9: return 1;
        case 10 .. 15: return 10;
        case 16 .. 18, 39: return 8;
        case 22 .. 24: return 2;
        case 25 .. 27: return 3;
        case 28 .. 29, 32: return 4;
        case 30 .. 31: return 5;
        case 33, 34: return 6;
        case 35 .. 38: return 7;
        case 40: return 12;
        case 41 .. 43: return 9;
        case 44 ..46: return 11;
    }
    return 0;
}

stock Weapon_GetObjectModel(weaponid)
{
    switch(weaponid)
    {
        case 1: return 331;
        case 2 .. 8: return 331 + weaponid;
        case 9: return 341;
        case 10: return 321;
        case 11: return 320;
        case 12: return 319;
        case 13: return 318;
        case 14: return 325;
        case 15: return 326;
        case 16 .. 18: return 342 + (weaponid - 16);
        case 22 .. 24: return 346 + (weaponid - 22);
        case 25: return 349;
        case 26: return 351;
        case 27: return 350;
        case 28: return 352;
        case 29: return 353;
        case 30: return 355;
        case 31: return 356;
        case 32: return 372;
        case 33 .. 46: return 357 + (weaponid - 33);
    }
    return 0;
}