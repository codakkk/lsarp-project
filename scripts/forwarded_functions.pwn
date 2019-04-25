// just forward here functions that gives warning: function with tag result used before definition, forcing reparse.
// Those are probably stocks.
forward Inventory:Character_GetInventory(playerid);
forward bool:Character_SetBag(playerid, bagid);

forward Inventory:Vehicle_GetInventory(vehicleid);

forward ITEM_TYPE:ServerItem_GetType(itemid);
forward List:Inventory_HasItemBySlots(Inventory:inventory, itemid);
forward Float:Vehicle_GetFuel(vehicleid);
forward Inventory:Vehicle_InitializeInventory(vehicleid);
forward String:Building_GetWelcomeTextStr(buildingid);
forward String:House_GetOwnerNameStr(houseid);
forward bool:Weapon_RequireAmmo(weaponid);
forward String:Faction_GetNameStr(factionid);
forward String:Faction_GetRankNameStr(factionid, rank);
