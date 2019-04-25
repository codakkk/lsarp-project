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