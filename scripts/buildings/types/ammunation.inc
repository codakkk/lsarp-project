
#include <buildings\core>
#include <YSI_Coding\y_hooks>


hook OnGameModeLateInit()
{
    DefineSellableItem(BUILDING_TYPE_AMMUNATION, "9MM", WEAPON_COLT45, 8000, .amount=1);
    DefineSellableItem(BUILDING_TYPE_AMMUNATION, "Desert Eagle", WEAPON_DEAGLE, 12500, .amount=1);
    DefineSellableItem(BUILDING_TYPE_AMMUNATION, "Shotgun", WEAPON_SHOTGUN, 30000, .amount=1);
    DefineSellableItem(BUILDING_TYPE_AMMUNATION, "Country Rifle", WEAPON_RIFLE, 100000, .amount=1);
    DefineSellableItem(BUILDING_TYPE_AMMUNATION, "Munizioni leggere", gItem_LightAmmo, 4, .amount=1);
    DefineSellableItem(BUILDING_TYPE_AMMUNATION, "Munizioni a pallettoni", gItem_BuckShotAmmo, 10, .amount=1);
    DefineSellableItem(BUILDING_TYPE_AMMUNATION, "Munizioni fucile", gItem_RifleAmmo, 20, .amount=1);
    return 1;
}

hook OnCharacterTryBuy(playerid)
{
    new Building:building;
    if(!Character_TryGetBuilding(playerid, building) || !Character_IsInsideBuilding(playerid, building) || Building_GetType(building) != BUILDING_TYPE_AMMUNATION)
        return 0;
    if(!Character_HasWeaponLicense(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Necessiti di un porto d'armi per acquistare in questo negozio.");
    Building_ShowSellingItems(building, playerid, @("Ammu-Nation"));
    return 1;
}