#include <YSI_Coding\y_hooks>

static enum E_WEAPON_DATA
{
    bool:wInitialized,
    Float:wDamage,
    wAmmoItem,
	bool:wCanBeDisassembled,
};
static 
    WeaponInfo[47][E_WEAPON_DATA];

hook OnGameModeInit()
{
	//Weapon_InitializeType(WEAPON_SPRAYCAN, gItem_Empty, 0.0, false, false);

	Weapon_InitializeType(WEAPON_COLT45, gItem_LightAmmo, 25.0, true);
	Weapon_InitializeType(WEAPON_SILENCED, gItem_LightAmmo, 30.0, true);
	Weapon_InitializeType(WEAPON_DEAGLE, gItem_LightAmmo, 45.0, true);

	Weapon_InitializeType(WEAPON_SHOTGUN, gItem_BuckShotAmmo, 3.0, true);
	Weapon_InitializeType(WEAPON_SAWEDOFF, gItem_BuckShotAmmo, 6.0, true);
	Weapon_InitializeType(WEAPON_SHOTGSPA, gItem_BuckShotAmmo, 8.0, true);

	Weapon_InitializeType(WEAPON_UZI, gItem_LightAmmo, 15.0, true);
	Weapon_InitializeType(WEAPON_TEC9, gItem_LightAmmo, 15.0, true);
	Weapon_InitializeType(WEAPON_MP5, gItem_LightAmmo, 25.0, true);

	Weapon_InitializeType(WEAPON_AK47, gItem_HeavyAmmo, 40.0, true);
	Weapon_InitializeType(WEAPON_M4, gItem_HeavyAmmo, 35.0, true);

	Weapon_InitializeType(WEAPON_RIFLE, gItem_RifleAmmo, 70.0, true);
	Weapon_InitializeType(WEAPON_SNIPER, gItem_RifleAmmo, 500.0, true);

	for(new i = 0; i < 15; ++i)
	{
		SetWeaponArmourRule(i, false);
	}
	return 1;
}

stock Weapon_InitializeType(weaponid, ammoItem, Float:damages, bool:canBeDisassembled = false, bool:affectArmour = true)
{
	WeaponInfo[weaponid][wInitialized] = true;
	WeaponInfo[weaponid][wAmmoItem] = ammoItem;
	WeaponInfo[weaponid][wDamage] = damages;
	WeaponInfo[weaponid][wCanBeDisassembled] = canBeDisassembled;
	SetWeaponArmourRule(weaponid, affectArmour);
	SetWeaponDamage(weaponid, DAMAGE_TYPE_STATIC, damages);
}

stock Float:Weapon_GetDamage(weaponid)
{
	if(weaponid <= 0 || weaponid >= 47 || !WeaponInfo[weaponid][wInitialized])
		return Float:0;
	return WeaponInfo[weaponid][wDamage];
}

stock bool:Weapon_CanBeDisassembled(weaponid)
{
	if(weaponid <= 0 || weaponid >= 47 || !WeaponInfo[weaponid][wInitialized])
		return false;
	return WeaponInfo[weaponid][wCanBeDisassembled];
}

stock Weapon_GetAmmoType(weaponid)
{
	if(weaponid <= 0 || weaponid >= 47 || !WeaponInfo[weaponid][wInitialized])
		return 0;
	return WeaponInfo[weaponid][wAmmoItem];
}

stock bool:Weapon_RequireAmmo(weaponid)
{
	if(
		Weapon_GetSlot(weaponid) == 0 || 
		Weapon_GetSlot(weaponid) == 1 || 
		Weapon_GetSlot(weaponid) == 10 ||
		Weapon_IsGrenade(weaponid) ||
		weaponid == 40 ||
		weaponid == 44 || weaponid == 45 || weaponid == 46) 
		return false;

	return true;
}

stock bool:Weapon_IsGrenade(weaponid)
{
	return Weapon_GetSlot(weaponid) == 8;
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
		case 26: return 350;
		case 27: return 351;
		case 28: return 352;
		case 29: return 353;
		case 30: return 355;
		case 31: return 356;
		case 32: return 372;
		case 33 .. 46: return 357 + (weaponid - 33);
	}
	return 0;
}

stock Weapon_GetMagSize(weaponid)
{
	static WeaponMagSizes[17] = {
		// Pistols
		017, // 22 M9 x2 WHEN DUAL
		017, // 23 M9 SD
		007, // 24 Desert Eagle

		// Shotgun
		001, // 25 Shotgun
		002, // 26 Sawnoff x2 WHEN DUAL
		007, // 27 Spas 12

		// Automatic
		050, // 28 Mac 10 x2 WHEN DUAL
		030, // 29 MP5
		030, // 30 AK-47
		050, // 31 M16
		050, // 32 Tec 9 x2 WHEN DUAL

		// Rifle
		001, // 33 Rifle
		001, // 34 Sniper

		// Heavy
		001, // 35 RPG
		001, // 36 Heatseeker
		500, // 37 Flamer
		500 // 38 Minigun
	};
	return WeaponMagSizes[weaponid - 22];
}

stock Weapon_GetName(weaponid)
{
	static gsWeaponNames[][32] = {
		"Pugni", "Tirapugni", "Golf Club", 
		"Nightstick", "Coltello", "Mazza da Baseball",
		"Pala", "Mazza da Biliardo", "Katana", 
		"Motosega", "Dildo Viola", "Dildo", 
		"Vibratore", "Vibratore Argento", "Fiori", 
		"Mazza", "Granata", "Fumogeno", 
		"Molotov", "Arma Veicolo", "Arma Veicolo", 
		"", "9MM", "9MM Silenziata", 
		"Desert Eagle", "Fucile a pompa", "Fucile a Canne Mozze", 
		"Fucile da Combattimento", "Micro SMG", "MP5", 
		"AK-47", "M4", "TEC-9", 
		"Fucile", "Fucile da Cecchino", "Lanciamissili", 
		"HS Rocket", "Lanciafiamme", "Minigun", 
		"Carica C4", "Detonatore", "Bomboletta Spray", 
		"Estintore", "Fotocamera", "Visore Notturno", 
		"Visore Termico", "Paracadute","Fake Pistol", 
		"Calcio Arma", "Veicolo", "Eliche Elicottero", 
		"Esplosione", "Investito", "Annegamento", 
		"Caduta", "Splat", "Sconosciuto"
	};
	if(weaponid < 0 || weaponid >= sizeof(gsWeaponNames))
		return gsWeaponNames[sizeof(gsWeaponNames)-1];
    return gsWeaponNames[weaponid];
}