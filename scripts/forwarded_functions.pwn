// just forward here functions that gives warning: function with tag result used before definition, forcing reparse.
// Those are probably stocks.
forward Inventory:Character_GetInventory(playerid);
forward bool:Character_SetBag(playerid, bagid);

forward Inventory:Vehicle_GetInventory(vehicleid);