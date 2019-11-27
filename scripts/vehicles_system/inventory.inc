#include <YSI_Coding\y_hooks>

static 
	CurrentVehicleInventory[MAX_PLAYERS], // Current Opened Vehicle Inventory Dialog
	SelectedListItem[MAX_PLAYERS]
;

hook OnCharacterClearData(playerid)
{
    CurrentVehicleInventory[playerid] = 0;
	SelectedListItem[playerid] = -1;
    return 1;
}

stock Inventory:Vehicle_InitializeInventory(vehicleid)
{
    new modelid = Vehicle_GetModel(vehicleid);
    new size = 10;
    /*switch(modelid)
    {
        case 481, 509, 522, 468: size = 1;
        case 521, 581, 461, 462, 463, 471, 403, 422, 443, 478, 514, 515, 543, 578: size = 2;
        case 586: size = 4;
        case 473, 541, 500, 535, 415, 477, 562, 560, 603, 411, 451: size = 5;
        case 458: size = 12;
        case 493, 418, 446, 452: size = 15;
        case 483, 579, 489: size = 20;
        case 440, 459: size = 25;
        case 413: size = 30;
        case 414, 453: size = 35;
        case 482: size = 40;
        case 454, 408: size = 50;
        case 456, 499: size = 100;
        case 455, 498: size = 150;
        case 435, 450, 591: size = 200;
    }*/
    switch(modelid)
    {
        case 481, 509, 522, 468: size = 1;
        case 521, 581, 461, 462, 463, 471, 403, 422, 443, 478, 514, 515, 543, 578: size = 2;
        case 586: size = 4;
        case 473, 541, 500, 535, 415, 477, 562, 560, 603, 411, 451: size = 5;
        case 458: size = 12;
        case 493, 418, 446, 452: size = 15;
        case 483, 579, 489: size = 20;
        case 440, 459: size = 25;
        case 413: size = 30;
        case 414, 453: size = 35;
        case 482: size = 40;
        case 454, 408, 456, 499, 455, 498, 435, 450, 591: size = 50;
    }
    new Inventory:inv = Inventory_New(ENTITY_TYPE_VEHICLE, vehicleid, size);
    return inv;
}

stock Vehicle_UnloadInventory(vehicleid)
{
	Inventory_Delete(Vehicle_GetInventory(vehicleid));
    return 1;
}

stock Inventory:Vehicle_GetInventory(vehicleid)
{
	return Inventory_Get(ENTITY_TYPE_VEHICLE, vehicleid);
}

// Checks if slotid is in the inventory boundaries (0 <= slotid <= Inventory_GetSpace(inventory))
stock Vehicle_IsValidSlot(vehicleid, slotid)
{
	return Inventory_IsValidSlot(Vehicle_GetInventory(vehicleid), slotid);
}

stock bool:Vehicle_HasInventory(vehicleid)
{
	new Inventory:inv = Vehicle_GetInventory(vehicleid);
    return Inventory_IsValid(inv);
}

hook OnVehicleUnloaded(vehicleid)
{
    Vehicle_UnloadInventory(vehicleid);
    return 1;
}

hook OnVehicleLoaded(vehicleid)
{
    Vehicle_InitializeInventory(vehicleid);
    Vehicle_LoadInventory(vehicleid);
    return 1;
}

stock Vehicle_GiveItem(vehicleid, itemid, amount = 1, extra = 0)
{
    new result = Inventory_AddItem(Vehicle_GetInventory(vehicleid), itemid, amount, extra);
    Vehicle_SaveInventory(vehicleid);
    return result;
}

stock Vehicle_DecreaseItemAmount(vehicleid, itemid, amount = 1)
{
    new Inventory:inventory = Vehicle_GetInventory(vehicleid);
    
    new result = Inventory_DecreaseItemAmount(inventory, itemid, amount);

    Vehicle_SaveInventory(vehicleid);
    return result;
}

stock Vehicle_HasSpaceForItem(vehicleid, itemid, amount)
{
    return Inventory_HasSpaceForItem(Vehicle_GetInventory(vehicleid), itemid, amount);
}

stock Vehicle_SaveInventory(vehicleid)
{
	if(!Vehicle_GetID(vehicleid))
		return 0;
	Inventory_SaveInDatabase(Vehicle_GetInventory(vehicleid), "vehicle_inventory", "vehicle_id", Vehicle_GetID(vehicleid));
	return 1;
}

stock Vehicle_LoadInventory(vehicleid)
{
	if(!Vehicle_GetID(vehicleid))
		return 0;
	Inventory_LoadFromDatabase(Vehicle_GetInventory(vehicleid), "vehicle_inventory", "vehicle_id", Vehicle_GetID(vehicleid));
	return 1;
}