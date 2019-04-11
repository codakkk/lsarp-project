new gWeaponNames[][32] = {
    "Pugni", "Tirapugni",
    "Golf Club", "Nightstick", "Coltello", "Mazza da Baseball",
    "Pala", "Mazza da Biliardo", "Katana", "Motosega",
    "Dildo Viola", "Dildo", "Vibratore", "Vibratore Argento",
    "Fiori", "Mazza", "Granata", "Allucinogini", "Molotov",
    "INVALID", "INVALID", "INVALID",
    "9MM", "9MM Silenziata", "D. Eagle", "Fucile a pompa",
    "Fucile a Canne Mozze", "Fucile da Combattimento", 
    "Micro SMG", "MP5", "AK-47", "M4", "TEC-9", "Fucile",
    "Fucile da Cecchino", "Lanciamissili", "HS Rocket", 
    "Lanciafiamme", "Minigun", "Carica C4", "Detonatore",
    "Bomboletta Spray", "Estintore", "Fotocamera", 
    "Visore Notturno", "Visore Termico", "Paracadute"
};

stock Weapon_GetName(weaponid)
{
    return gWeaponNames[weaponid];
}

stock RemovePlayerWeapon(playerid, weaponid)
{
	new plyWeapons[12] = 0;
	new plyAmmo[12] = 0;
	for(new slot = 0; slot != 12; slot++)
	{
		new wep, ammo;
		GetPlayerWeaponData(playerid, slot, wep, ammo);

		if(wep != weaponid && ammo != 0)
		{
			GetPlayerWeaponData(playerid, slot, plyWeapons[slot], plyAmmo[slot]);
		}
		else if(wep == weaponid)
		{
			ACInfo[playerid][acWeapons][slot] = 0; // AntiCheat
			ACInfo[playerid][acAmmo][slot] = 0; // AntiCheat
		}
	}

	AC_ResetPlayerWeapons(playerid);
	for(new slot = 0; slot != 12; slot++)
	{
	    if(plyAmmo[slot] != 0)
	    {
			AC_GivePlayerWeapon(playerid, plyWeapons[slot], plyAmmo[slot]);
		}
	}
	return 1;
}

stock RemovePlayerWeaponBySlot(playerid, slotid)
{
    new w, a;
    GetPlayerWeaponData(playerid, slotid, w, a);
    RemovePlayerWeapon(playerid, w);
    return 1;
}