#include <YSI_Coding\y_hooks>

#define MAX_ACCUMULATED_SHOTS 		5

hook OnPlayerUpdate(playerid)
{
	static lastWeapon;
	new currentWeapon = GetPlayerWeapon(playerid);

	if(lastWeapon != currentWeapon)
	{
		ACInfo[playerid][acShotCounter] = 0;
		lastWeapon = currentWeapon;
	}
	else if(WEAPON_COLT45 <= currentWeapon <= WEAPON_TEC9 && currentWeapon != WEAPON_SHOTGUN && GetPlayerWeaponState(playerid) == WEAPONSTATE_RELOADING)
	{
		ACInfo[playerid][acShotCounter] = 0;
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}


/*hook OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
	if(GetServerTickRate() < 100)
		return Y_HOOKS_CONTINUE_RETURN_1;
	AntiCheat_NoReloadCheck(playerid, weaponid);
	return Y_HOOKS_CONTINUE_RETURN_1;
}*/


stock AntiCheat_NoReloadCheck(playerid, weaponid)
{
	if( WEAPON_COLT45 <= weaponid <= WEAPON_TEC9 && weaponid != WEAPON_SHOTGUN)
	{
		ACInfo[playerid][acShotCounter]++;
		new magSize = Weapon_GetMagSize(weaponid);
		if(ACInfo[playerid][acShotCounter] - magSize >= MAX_ACCUMULATED_SHOTS && magSize > 1)
		{
			AC_Detect(playerid, AC_NO_RELOAD_HACK);
			ACInfo[playerid][acShotCounter] = 0;
			return 1;
		}
	}
	return 0;
}