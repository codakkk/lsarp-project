#include <YSI_Coding\y_hooks>

hook OnHouseLoaded(houseid)
{
	House_InitializeInventory(houseid);
	House_LoadInventory(houseid);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

stock Inventory:House_InitializeInventory(houseid)
{
	if(map_has_key(HouseInventory, houseid))
		return Inventory:0;
	new Inventory:inv = Inventory_New(10);
	map_add(HouseInventory, houseid, List:inv);
	//printf("House %d inventory initialized");
	return inv;
}

stock Inventory:House_GetInventory(houseid)
{
	new Inventory:inv;
	if(map_get_safe(HouseInventory, houseid, List:inv))
	{
		return inv;
	}
	return Inventory:0;
}

stock House_SaveInventory(houseid)
{
	Inventory_SaveInDatabase(House_GetInventory(houseid), "house_inventory", "HouseID", House_GetID(houseid));
	return 1;
}

stock House_LoadInventory(houseid)
{
	Inventory_LoadFromDatabase(House_GetInventory(houseid), "house_inventory", "HouseID", House_GetID(houseid));
	return 1;
}