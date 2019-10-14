
forward OnAntiCheatDetected(playerid, code);

enum
{
	AC_WEAPON_HACK,
	AC_AMMO_HACK,
	AC_NO_RELOAD_HACK,
	AC_SPEED_HACK,
	AC_AIR_BREAK,
	AC_HEALTH_CODE,
	AC_ARMOUR_CODE,
}

static AC_MAX_DETECTS[] = {
	3, // ANTI WEAPON HACK
	3, // ANTI AMMO HACK
	1, // NO RELOAD HACK
	5, // SPEED HACK
	1, // AIR BREAK
	3, // HEALTH HACK
	3, // ARMOUR HACK
};

new AC_Name[][24] = {
	"Weapon Hack", "Ammo Hack", "No Reload Hack", "Speed Hack", "AirBreak"
};

enum AC_INFO
{
	acDetected[10], // INCREASE THIS BY CODE NUMBERS
	acWeapons[13],
	acAmmo[13],
	acShotCounter,
}
new ACInfo[MAX_PLAYERS][AC_INFO];

hook OnCharacterDisconnected(playerid, reason)
{
	new Data[AC_INFO];
	ACInfo[playerid] = Data;
}

stock AC_Detect(playerid, code)
{
	if(!Character_IsLogged(playerid) || code < 0)
		return 0;
	ACInfo[playerid][acDetected][code]++;
	if(ACInfo[playerid][acDetected][code] >= AC_MAX_DETECTS[code])
	{
		CallLocalFunction(#OnAntiCheatDetected, "ii", playerid, code);
		ACInfo[playerid][acDetected][code] = 0;
	}
	return 1;
}