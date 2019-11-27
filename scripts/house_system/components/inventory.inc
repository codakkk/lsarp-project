#include <YSI_Coding\y_hooks>

hook OnGameModeInit()
{
	mysql_pquery(gMySQL, "CREATE TABLE IF NOT EXISTS `house_inventory` ( \
							`house_id` int(11) NOT NULL, \
							`slot_id` tinyint(4) NOT NULL, \
							`item` int(11) NOT NULL, \
							`amount` int(11) NOT NULL, \
							`extra` int(11) NOT NULL, \
							PRIMARY KEY (`house_id`,`slot_id`), \
							CONSTRAINT `house_inventory` FOREIGN KEY (`house_id`) REFERENCES `houses` (`id`) ON DELETE CASCADE \
							) ENGINE=InnoDB DEFAULT CHARSET=latin1");

	return 1;
}

hook OnHouseLoaded(House:house)
{
	House_InitializeInventory(house);
	House_LoadInventory(house);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

stock Inventory:House_InitializeInventory(House:house)
{
	new Inventory:inv = Inventory_New(ENTITY_TYPE_HOUSE, House_GetID(house), 20);
	return inv;
}

stock Inventory:House_GetInventory(House:house)
{
	return Inventory_Get(ENTITY_TYPE_HOUSE, House_GetID(house));
}

stock House_SaveInventory(House:house)
{
	Inventory_SaveInDatabase(House_GetInventory(house), "house_inventory", "house_id", House_GetID(house));
	return 1;
}

stock House_LoadInventory(House:house)
{
	Inventory_LoadFromDatabase(House_GetInventory(house), "house_inventory", "house_id", House_GetID(house));
	return 1;
}

