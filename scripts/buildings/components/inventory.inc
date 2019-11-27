
#include <YSI_Coding\y_hooks>

hook OnGameModeInit()
{
    mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `building_inventory` ( \
                        `building_id` int(11) NOT NULL, \
                        `slot_id` tinyint(4) NOT NULL, \
                        `item` int(11) NOT NULL, \
                        `amount` int(11) NOT NULL, \
                        `extra` int(11) NOT NULL, \
                        PRIMARY KEY (`building_id`,`slot_id`), \
                        CONSTRAINT `building_inventory` FOREIGN KEY (`building_id`) REFERENCES `buildings` (`id`) ON DELETE CASCADE \
                        ) ENGINE=InnoDB DEFAULT CHARSET=latin1");
	return 1;
}

hook OnBuildingLoaded(Building:building)
{
	Building_InitializeInventory(building);
	Building_LoadInventory(building);
	return 1;
}

static Inventory:Building_InitializeInventory(Building:building)
{
	new Inventory:inv = Inventory_New(ENTITY_TYPE_BUILDING, Building_GetID(building), 30);
	return inv;
}

stock Inventory:Building_GetInventory(Building:building)
{
	return Inventory_Get(ENTITY_TYPE_BUILDING, Building_GetID(building));
}

stock Building_SaveInventory(Building:building)
{
	Inventory_SaveInDatabase(Building_GetInventory(building), "building_inventory", "building_id", Building_GetID(building));
	return 1;
}

static Building_LoadInventory(Building:building)
{
    Inventory_LoadFromDatabase(Building_GetInventory(building), "building_inventory", "building_id", Building_GetID(building));
    return 1;
}