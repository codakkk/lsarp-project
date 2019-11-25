
#include <buildings\core>

#include <YSI_Coding\y_hooks>

hook OnGameModeLateInit()
{
    DefineSellableItem(BUILDING_TYPE_STORE, "Mazza da baseball", WEAPON_BAT, 300, .amount=1, .extra=1);
    DefineSellableItem(BUILDING_TYPE_STORE, "Spray can", WEAPON_SPRAYCAN, 400, .amount=1, .extra=1000);
    DefineSellableItem(BUILDING_TYPE_STORE, "Fotocamera", WEAPON_CAMERA, 500, .amount=1, .extra=100);
    DefineSellableItem(BUILDING_TYPE_STORE, "Cellulare", gItem_Phone, 500, .amount=1);
    DefineSellableItem(BUILDING_TYPE_STORE, "Cellulare (usa e getta)", gItem_Phone, 500, .amount=1, .extra=1);
    DefineSellableItem(BUILDING_TYPE_STORE, "Tanica piccola", gItem_SmallFuelTank, 150, .amount=1);
    DefineSellableItem(BUILDING_TYPE_STORE, "Tanica media", gItem_MediumFuelTank, 250, .amount=1);
    DefineSellableItem(BUILDING_TYPE_STORE, "Tanica grande", gItem_BigFuelTank, 700, .amount=1);
    DefineSellableItem(BUILDING_TYPE_STORE, "Vestiti", gItem_Dress, 10, .amount=1);
    DefineSellableItem(BUILDING_TYPE_STORE, "Radio", gItem_Radio, 500, .amount=1);
    DefineSellableItem(BUILDING_TYPE_STORE, "Sigarette", gItem_Cigarette, 50, .amount=10, .extra=1);
    return 1;
}

hook OnPlayerEnterBuilding(playerid, Building:building)
{
    if(Building_GetType(building) == BUILDING_TYPE_STORE)
    {
        SendClientMessage(playerid, COLOR_ORANGE, "Questo edificio ha degli oggetti in vendita. Usa \"/compra\" per visualizzarne la lista.");
    }
    return 1;
}

hook OnCharacterTryBuy(playerid)
{
    new Building:building;
    if(!Character_TryGetBuilding(playerid, building) || !Character_IsInsideBuilding(playerid, building) || Building_GetType(building) != BUILDING_TYPE_STORE)
        return 0;
    Building_ShowSellingItems(building, playerid, @("24/7"));
    return 1;
}
