
#include <pickup_system\core>

#include <YSI_Coding\y_hooks>

#define INVALID_LOOT_ZONE 		(0)

static enum e_LZ_ITEM_DATA
{
	DropChance,
	MinQuantity,
	MaxQuantity
};

static enum e_LOOT_ZONE_DATA
{
	ID,
	Float:X,
	Float:Y,
	Float:Z,
	Interior,
	VirtualWorld,
	Map:Items, // <itemid, e_LZ_ITEM_DATA>

	Pickup,
	Text3D:Label,

	Inventory:Inventory,
	Map:ObjectsMap,
	PlayerLooting // Playerid of player looting. INVALID_PLAYER_ID if none.
};

static Map:LootZones; // <lootzoneid, e_LOOT_ZONE_DATA>

static
	CurrentLootZone[MAX_PLAYERS] = {INVALID_LOOT_ZONE, ...},
	SelectedItem[MAX_PLAYERS] = {0, ...}
;

hook OnGameModeInit()
{
	LootZones = map_new();

	mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS loot_zones \
						(`id` INT(11) NOT NULL AUTO_INCREMENT, \
						`x` FLOAT DEFAULT '0.0', \
						`y` FLOAT DEFAULT '0.0', \
						`z` FLOAT DEFAULT '0.0', \
						`interior` INT(11) NULL, \
						`virtual_world` INT(11) NULL, \
						PRIMARY KEY (`id`))");
	
	mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS loot_zone_items \
						(`lootzone_id` INT(11), \
						`item_id` INT(11) DEFAULT '0', \
						`min_quantity` INT(11) DEFAULT '0', \
						`max_quantity` INT(11) DEFAULT '0', \
						`drop_chance` INT(11) DEFAULT '0', \
						PRIMARY KEY (`lootzone_id`,`item_id`), \
                        CONSTRAINT `lootzone_items` FOREIGN KEY (`lootzone_id`) REFERENCES `loot_zones` (`id`) ON DELETE CASCADE \
                        ) ENGINE=InnoDB DEFAULT CHARSET=latin1");
	LootZone_LoadAll();
	return 1;
}

hook OnCharacterClearData(playerid)
{
	if(LootZone_IsLootingBy(Character_GetLootZone(playerid), playerid))
	{
		LootZone_SetLootingPlayer(Character_GetLootZone(playerid), INVALID_PLAYER_ID);
	}
	CurrentLootZone[playerid] = INVALID_LOOT_ZONE;
	SelectedItem[playerid] = 0;
	return 1;
}

task RefreshLootZones[3600000 * 6]() // 3600000 * 6 -> 6 hours
{
	for_map(lootzone : LootZones)
	{
		new lootzoneid = iter_get_key(lootzone);

		LootZone_Refresh(lootzoneid);
	}

	SendMessageToAdmins(1, COLOR_ORANGE, "Server: sono state refreshate tutte le Loot Zones (6 ore).");
}

stock LootZone_Create(Float:x, Float:y, Float:z, interior, virtualWorld)
{
	inline OnInsert()
	{
		new
			data[e_LOOT_ZONE_DATA]
		;

		data[ID] = cache_insert_id();
		data[X] = x;
		data[Y] = y;
		data[Z] = z;
		data[Interior] = interior;
		data[VirtualWorld] = virtualWorld;
		

		map_add_arr(LootZones, data[ID], data);
		
		LootZone_Initialize(data[ID]);
	}

	MySQL_TQueryInline(gMySQL, using inline OnInsert, "INSERT INTO `loot_zones` (x, y, z, interior, virtual_world) VALUES('%f', '%f', '%f', '%d', '%d')", x, y, z, interior, virtualWorld);
}

stock bool:LootZone_Initialize(lootzoneid)
{
	if(!LootZone_IsValid(lootzoneid))
		return false;
	
	if(!Inventory_IsValid(LootZone_GetInventory(lootzoneid)))
		map_set_cell(LootZones, LootZone_GetID(lootzoneid), Inventory, List:Inventory_New(10));
	
	if(!map_valid(LootZone_GetItems(lootzoneid)))
		map_set_cell(LootZones, LootZone_GetID(lootzoneid), Items, map_new());
	
	if(!map_valid(LootZone_GetObjectsMap(lootzoneid)))
		map_set_cell(LootZones, LootZone_GetID(lootzoneid), ObjectsMap, map_new());
	
	new Float:x, Float:y, Float:z, interior = LootZone_GetInterior(lootzoneid), virtualWorld = LootZone_GetVirtualWorld(lootzoneid);
	LootZone_GetPosition(lootzoneid, x, y, z);

	LootZone_SetLootingPlayer(lootzoneid, INVALID_PLAYER_ID);
	
	if(IsValidDynamicPickup(LootZone_GetPickup(lootzoneid)))
		Pickup_Destroy(LootZone_GetPickup(lootzoneid));

	if(IsValidDynamic3DTextLabel(LootZone_GetLabel(lootzoneid)))
		DestroyDynamic3DTextLabel(LootZone_GetLabel(lootzoneid));

	new pickupid = Pickup_Create(1007, LootZone_GetID(lootzoneid), x, y, z, ELEMENT_TYPE_LOOTZONE, virtualWorld, interior);
	map_set_cell(LootZones, LootZone_GetID(lootzoneid), Pickup, pickupid);

	new text[32];
	format(text, sizeof(text), "Loot Zone [%d]", lootzoneid);
	new Text3D:label = CreateDynamic3DTextLabel(text, COLOR_GREY, x, y, z + 0.55, 10.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, virtualWorld, interior);
	map_set_cell(LootZones, LootZone_GetID(lootzoneid), Label, label);

	return true;
}

stock LootZone_Delete(lootzoneid)
{
	if(!LootZone_IsValid(lootzoneid))
		return false;
	
	mysql_tquery_f(gMySQL, "DELETE FROM `loot_zones` WHERE id = '%d'", LootZone_GetID(lootzoneid));

	Inventory_Delete(LootZone_GetInventory(lootzoneid));
	map_delete_deep(LootZone_GetItems(lootzoneid));
	
	LootZone_DestroyObjects(lootzoneid);
	
	if(IsValidDynamic3DTextLabel(LootZone_GetLabel(lootzoneid)))
		DestroyDynamic3DTextLabel(LootZone_GetLabel(lootzoneid));

	map_delete_deep(LootZone_GetObjectsMap(lootzoneid));

	map_remove_deep(LootZones, LootZone_GetID(lootzoneid));
	return true;
}

stock bool:LootZone_Refresh(lootzoneid)
{
	if(!LootZone_IsValid(lootzoneid))
		return false;
	LootZone_SetLootingPlayer(lootzoneid, INVALID_PLAYER_ID);
	LootZone_ClearInventory(lootzoneid);
	LootZone_DestroyObjects(lootzoneid);

	LootZone_Fill(lootzoneid);
	return true;
}

stock bool:LootZone_IsValid(lzid)
{
	return map_has_key(LootZones, lzid);
}

stock LootZone_GetID(lootzoneid)
{
	return map_get(LootZones, lootzoneid, ID);
}

stock bool:LootZone_GetPosition(lootzoneid, &Float:x, &Float:y, &Float:z)
{
	if(!LootZone_IsValid(lootzoneid))
		return false;
	x = Float:map_get(LootZones, LootZone_GetID(lootzoneid), X);
	y = Float:map_get(LootZones, LootZone_GetID(lootzoneid), Y);
	z = Float:map_get(LootZones, LootZone_GetID(lootzoneid), Z);
	return true;
}

stock bool:LootZone_SetPosition(lootzoneid, Float:x, Float:y, Float:z)
{
	if(!LootZone_IsValid(lootzoneid))
		return false;
	map_set_cell(LootZones, LootZone_GetID(lootzoneid), X, x);
	map_set_cell(LootZones, LootZone_GetID(lootzoneid), Y, y);
	map_set_cell(LootZones, LootZone_GetID(lootzoneid), Z, z);

	mysql_tquery_f(gMySQL, "UPDATE `loot_zones` SET x = '%f', y = '%f', z = '%f' WHERE id = '%d'", x, y, z, LootZone_GetID(lootzoneid));
	return true;
}

stock LootZone_GetInterior(lootzoneid)
{
	if(!LootZone_IsValid(lootzoneid))
		return 0;
	return map_get(LootZones, LootZone_GetID(lootzoneid), Interior);
}

stock bool:LootZone_SetInterior(lootzoneid, interior)
{
	if(!LootZone_IsValid(lootzoneid))
		return false;
	map_set_cell(LootZones, LootZone_GetID(lootzoneid), Interior, interior);

	mysql_tquery_f(gMySQL, "UPDATE `loot_zones` SET interior = '%d' WHERE id = '%d'", interior, LootZone_GetID(lootzoneid));
	return true;
}

stock LootZone_GetVirtualWorld(lootzoneid)
{
	if(!LootZone_IsValid(lootzoneid))
		return 0;
	return map_get(LootZones, LootZone_GetID(lootzoneid), VirtualWorld);
}

stock bool:LootZone_SetVirtualWorld(lootzoneid, worldid)
{
	if(!LootZone_IsValid(lootzoneid))
		return false;
	map_set_cell(LootZones, LootZone_GetID(lootzoneid), VirtualWorld, worldid);

	mysql_tquery_f(gMySQL, "UPDATE `loot_zones` SET virtual_world = '%d' WHERE id = '%d'", worldid, LootZone_GetID(lootzoneid));
	return true;
}

stock bool:LootZone_CanDropItem(lootzoneid, itemid)
{
	if(!LootZone_IsValid(lootzoneid))
		return false;
	new Map:items = LootZone_GetItems(lootzoneid);
	return map_has_key(items, itemid);
}

stock bool:LootZone_AddDropItem(lootzoneid, itemid, dropChance = 0, minQuantity = 0, maxQuantity = 0)
{
	if(!LootZone_IsValid(lootzoneid) || LootZone_CanDropItem(lootzoneid, itemid))
		return false;
	new Map:items = LootZone_GetItems(lootzoneid);
	new data[e_LZ_ITEM_DATA];
	data[DropChance] = dropChance;
	data[MinQuantity] = minQuantity;
	data[MaxQuantity] = maxQuantity;
	
	mysql_tquery_f(gMySQL, "INSERT INTO `loot_zone_items` (lootzone_id, item_id, min_quantity, max_quantity, drop_chance) VALUES('%d', '%d', '%d', '%d', '%d')", 
	LootZone_GetID(lootzoneid), itemid, minQuantity, maxQuantity, dropChance);

	return map_add_arr(items, itemid, data);
}

stock bool:LootZone_RemoveDropItem(lootzoneid, itemid)
{
	if(!LootZone_IsValid(lootzoneid) || !LootZone_CanDropItem(lootzoneid, itemid))
		return false;
	mysql_tquery_f(gMySQL, "DELETE FROM `loot_zone_items` WHERE lootzone_id = '%d' AND item_id = '%d'", LootZone_GetID(lootzoneid), itemid);
	return map_remove_deep(LootZone_GetItems(lootzoneid), itemid);
}

stock bool:LootZone_RemoveAllDropItems(lootzoneid)
{
	if(!LootZone_IsValid(lootzoneid))
		return false;
	map_clear(LootZone_GetItems(lootzoneid));
	mysql_tquery_f(gMySQL, "DELETE FROM `loot_zone_items` WHERE lootzone_id = '%d'", LootZone_GetID(lootzoneid));
	return true;
}

stock Map:LootZone_GetItems(lootzoneid)
{
	if(!LootZone_IsValid(lootzoneid))
		return Map:0;
	return Map:map_get(LootZones, LootZone_GetID(lootzoneid), Items);
}

stock Inventory:LootZone_GetInventory(lootzoneid)
{
	if(!LootZone_IsValid(lootzoneid))
		return Inventory:0;
	return Inventory:map_get(LootZones, LootZone_GetID(lootzoneid), Inventory);
}

stock Map:LootZone_GetObjectsMap(lootzoneid)
{
	if(!LootZone_IsValid(lootzoneid))
		return Map:0;
	return Map:map_get(LootZones, LootZone_GetID(lootzoneid), ObjectsMap);
}

stock LootZone_GetLootingPlayer(lootzoneid)
{
	if(!LootZone_IsValid(lootzoneid))
		return INVALID_PLAYER_ID;
	return map_get(LootZones, LootZone_GetID(lootzoneid), PlayerLooting);
}

stock bool:LootZone_SetLootingPlayer(lootzoneid, playerid)
{
	if(!LootZone_IsValid(lootzoneid))
		return false;
	new currentLootingPlayer = LootZone_GetLootingPlayer(lootzoneid);
	if(Character_IsLogged(currentLootingPlayer) && Character_GetLootZone(currentLootingPlayer) == lootzoneid)
	{
		Dialog_Close(currentLootingPlayer);
	}
	map_set_cell(LootZones, LootZone_GetID(lootzoneid), PlayerLooting, playerid);
	return true;
}

stock bool:LootZone_ClearInventory(lootzoneid)
{
	if(!LootZone_IsValid(lootzoneid))
		return false;
	LootZone_SetLootingPlayer(lootzoneid, INVALID_PLAYER_ID);
	Inventory_Reset(LootZone_GetInventory(lootzoneid));
	return true;
}

stock bool:LootZone_DestroyObjects(lootzoneid)
{
	if(!LootZone_IsValid(lootzoneid))
		return false;
	new Map:objects = LootZone_GetObjectsMap(lootzoneid);
	for_map(iter : objects)
	{
		new value = iter_get_value(iter);
		if(IsValidDynamicObject(value))
			DestroyDynamicObject(value);
	}
	map_clear(objects);
	return true;
}

stock LootZone_GetPickup(lootzoneid)
{
	if(!LootZone_IsValid(lootzoneid))
		return 0;
	return map_get(LootZones, LootZone_GetID(lootzoneid), Pickup);
}

stock Text3D:LootZone_GetLabel(lootzoneid)
{
	if(!LootZone_IsValid(lootzoneid))
		return Text3D:INVALID_3DTEXT_ID;
	return Text3D:map_get(LootZones, LootZone_GetID(lootzoneid), Label);
}

// Player Checks
// Current player inspecting loot zone
stock Character_GetLootZone(playerid)
{
	return CurrentLootZone[playerid]; // PlayerInfo[%0][pLootZone]
}

stock Character_SetLootZone(playerid, lootzoneid)
{
	CurrentLootZone[playerid] = lootzoneid;
}

// Checks if loot zone is being looted by playerid
stock bool:LootZone_IsLootingBy(lootzoneid, playerid)
{
	return LootZone_IsValid(lootzoneid) && LootZone_GetLootingPlayer(lootzoneid) == playerid && Character_GetLootZone(playerid) == lootzoneid; // (PlayerInfo[%0][pLootZone] != -1 && LootZoneInfo[PlayerInfo[%0][pLootZone]][lzPlayerID] == %0)
}

stock bool:Character_ShowLootZone(playerid, lootzoneid)
{
	if(!LootZone_IsValid(lootzoneid))
		return false;
	
	new looter = LootZone_GetLootingPlayer(lootzoneid);
	if(Character_IsLogged(looter) && looter != playerid)
	{
		if(LootZone_IsLootingBy(lootzoneid, looter) && !Character_IsNearLootZone(looter, lootzoneid))
		{
			LootZone_SetLootingPlayer(lootzoneid, INVALID_PLAYER_ID);
		}
	}
	else LootZone_SetLootingPlayer(lootzoneid, INVALID_PLAYER_ID);

	if(LootZone_GetLootingPlayer(lootzoneid) != INVALID_PLAYER_ID)
		return SendClientMessage(playerid, COLOR_ERROR, "Qualcuno sta già frugando."), false;

	if(!Character_IsNearLootZone(playerid, lootzoneid))
		return SendClientMessage(playerid, COLOR_ERROR, "Ti sei allontanato dalla loot zone."), false;
	
	new Inventory:inventory = LootZone_GetInventory(lootzoneid);

	if(!Inventory_IsValid(inventory))
		return false;

	if(Inventory_GetUsedSpace(inventory) <= 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non c'è nulla da frugare qui."), false;

	Character_SetLootZone(playerid, lootzoneid);
	LootZone_SetLootingPlayer(lootzoneid, playerid);

	Inventory_Show(inventory, playerid, "Loot Zone", Dialog_LootZone, "Prendi", "Chiudi", true);
	return true;
}

Dialog:Dialog_LootZone(playerid, response, listitem, inputtext[])
{
	new lootzoneid = Character_GetLootZone(playerid);
	if(!response)
	{
		LootZone_SetLootingPlayer(lootzoneid, INVALID_PLAYER_ID);
		Character_SetLootZone(playerid, INVALID_LOOT_ZONE);
		return 0;
	}
	
	if(!Character_IsNearLootZone(playerid, lootzoneid))
		return 0;

	new Inventory:inventory = LootZone_GetInventory(lootzoneid);

	new 
		itemid = Inventory_GetSlotItem(inventory, listitem),
		slotAmount = Inventory_GetSlotAmount(inventory, listitem),
		totalAmount = Inventory_GetItemAmount(inventory, itemid);

	if(!ServerItem_IsValid(itemid) || slotAmount <= 0)
	{
		Character_ShowLootZone(playerid, lootzoneid);
		SendClientMessage(playerid, COLOR_ERROR, "Slot vuoto.");
		return 0;
	}

	SelectedItem[playerid] = itemid;

	if(ServerItem_IsUnique(itemid) || slotAmount == 1)
	{
		if(!Character_HasSpaceForItem(playerid, itemid, slotAmount))
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario.");
			Character_ShowLootZone(playerid, lootzoneid);
			return 0;
		}
		
		Inventory_DecreaseItemAmount(inventory, itemid, slotAmount);
		Character_GiveItem(playerid, itemid, slotAmount);
		LootZone_RemoveUnusedObjects(lootzoneid);
		SendFormattedMessage(playerid, -1, "Hai preso l'oggetto (%s - Q: %d) dalla loot zone.", ServerItem_GetName(itemid), slotAmount);
		Character_ShowLootZone(playerid, lootzoneid);
	}
	else
	{
		new String:str = str_format("Inserisci la quantità dell'oggetto che vuoi prendere.\n- {00FF00}Oggetto{FFFFFF}: %s\n- {00FF00}Quantità disponibile{FFFFFF}: %d", ServerItem_GetName(itemid), totalAmount);
		Dialog_Show_s(playerid, Dialog_LootZoneWithdraw, DIALOG_STYLE_INPUT, @("Loot Zone"), str, "Prendi", "Indietro");
	}
	return 1;
}

Dialog:Dialog_LootZoneWithdraw(playerid, response, listitem, inputtext[])
{
	new lootzoneid = Character_GetLootZone(playerid);
	
	if(!response)
	{
		Character_ShowLootZone(playerid, lootzoneid);
		return 1;
	}

	new 
		Inventory:inventory = LootZone_GetInventory(lootzoneid),
		itemid = SelectedItem[playerid],
		totalAmount = Inventory_GetItemAmount(inventory, itemid)
	;

	if(!ServerItem_IsValid(itemid) || totalAmount <= 0)
	{
		Character_ShowLootZone(playerid, lootzoneid);
		SendClientMessage(playerid, COLOR_ERROR, "Slot vuoto.");
		return 0;
	}

	new val = strval(inputtext);

	if(val <= 0 || val > totalAmount)
	{
		new String:str = str_format("{FF0000}La quantità inserita non è valida.{FFFFFF}\nInserisci la quantità dell'oggetto che vuoi prendere.\n- {00FF00}Oggetto{FFFFFF}: %s\n- {00FF00}Quantità disponibile{FFFFFF}: %d", ServerItem_GetName(itemid), totalAmount);
		Dialog_Show_s(playerid, Dialog_LootZoneWithdraw, DIALOG_STYLE_MSGBOX, @("Loot Zone"), str, "Prendi", "Indietro");
		return 0;
	}

	if(!Character_HasSpaceForItem(playerid, itemid, val))
	{
		new String:str = str_format("{FF0000}Non hai abbastanza spazio nell'inventario.{FFFFFF}\nInserisci la quantità dell'oggetto che vuoi prendere.\n- {00FF00}Oggetto{FFFFFF}: %s\n- {00FF00}Quantità disponibile{FFFFFF}: %d", ServerItem_GetName(itemid), totalAmount);
		Dialog_Show_s(playerid, Dialog_LootZoneWithdraw, DIALOG_STYLE_MSGBOX, @("Loot Zone"), str, "Prendi", "Indietro");
		return 0;
	}
	
	Character_GiveItem(playerid, itemid, val);
	Inventory_DecreaseItemAmount(inventory, itemid, val);

	LootZone_RemoveUnusedObjects(lootzoneid);

	SendFormattedMessage(playerid, -1, "Hai preso l'oggetto (%s - Q: %d) dalla loot zone.", ServerItem_GetName(itemid), val);

	Character_ShowLootZone(playerid, lootzoneid);
	return 1;
}


hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(!Player_HasHotKeysEnabled(playerid))
		return 0;
	
	if(PRESSED(KEY_CROUCH) && Character_IsAlive(playerid) && GetPlayerState(playerid) == PLAYER_STATE_ONFOOT && Character_CanInteract(playerid))
	{
		new id, E_ELEMENT_TYPE:type;
		if(Character_GetNearestPickup(playerid, id, type) && type == ELEMENT_TYPE_LOOTZONE)
		{
			Character_ShowLootZone(playerid, id);
		}
	}
	return 1;
}

stock bool:Character_IsNearLootZone(playerid, lootzoneid)
{
	if(!LootZone_IsValid(lootzoneid))
		return false;
	new Float:x, Float:y, Float:z;
	LootZone_GetPosition(lootzoneid, x, y, z);
	return IsPlayerInRangeOfPoint(playerid, 3.0, x, y, z) > 0;
}

stock bool:LootZone_Fill(lootzoneid)
{
	if(!LootZone_IsValid(lootzoneid))
		return false;
	
	new 
		Inventory:inventory = LootZone_GetInventory(lootzoneid),
		itemid = 0, data[e_LZ_ITEM_DATA],
		Float:x, Float:y, Float:z
	;

	for_map(i : LootZone_GetItems(lootzoneid))
	{
		itemid = iter_get_key(i);
		iter_get_arr(i, data);

		if(!ServerItem_IsValid(itemid))
			continue;
		
		new rand = random(101); // 100

		if(rand >= 100-data[DropChance])
		{
			new qnt = data[MinQuantity] + random(data[MaxQuantity]);

			if(qnt > data[MaxQuantity])
				qnt = data[MaxQuantity];
			
			Inventory_AddItem(inventory, itemid, qnt, 0);

			LootZone_GetPosition(lootzoneid, x, y, z);
			
			new objectid = CreateDynamicObject(ServerItem_GetModelID(itemid), x + math_random_float(-1.0, 1.0) * 0.5, y  + math_random_float(-1.0, 1.0) * 0.5, z, 90.0, 0.0, 0.0, LootZone_GetVirtualWorld(lootzoneid), LootZone_GetInterior(lootzoneid));
			map_add(LootZone_GetObjectsMap(lootzoneid), itemid, objectid);
		}
	}

	return true;
}

static LootZone_RemoveUnusedObjects(lootzoneid)
{
	if(!LootZone_IsValid(lootzoneid))
		return 0;

	for_map(i : LootZone_GetObjectsMap(lootzoneid))
	{
		new itemid = iter_get_key(i);
		if(!Inventory_HasItem(LootZone_GetInventory(lootzoneid), itemid, 1))
		{
			DestroyDynamicObject(iter_get_value(i));
			iter_erase(i);
		}
	}
	return 1;
}

flags:acreatelootzone(CMD_MODERATOR);
CMD:acreatelootzone(playerid, params[])
{
	new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	LootZone_Create(x, y, z - 1.0, GetPlayerInterior(playerid), Character_GetVirtualWorld(playerid));
	SendClientMessage(playerid, -1, "Loot Zone creata con successo. Usa /lzedit per modificarla.");
	return 1;
}
alias:acreatelootzone("lzcreate");

flags:lzedit(CMD_MODERATOR);
CMD:lzedit(playerid, p[])
{
	new lootzoneid, command[16], params[128];
	if(sscanf(p, "ds[16]S()[128]", lootzoneid, command, params))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /lzedit <lootzoneid> <pos - additem - rmitem - resetinv - refresh - items - delete>");	
	
	if(!LootZone_IsValid(lootzoneid))
		return SendClientMessage(playerid, COLOR_ERROR, "Loot Zone id non valido.");
	
	if(!strcmp(command, "pos", true))
	{
		new Float:x, Float:y, Float:z;
		GetPlayerPos(playerid, x, y, z);
		LootZone_SetPosition(lootzoneid, x, y, z - 1.0);
		LootZone_SetInterior(lootzoneid, GetPlayerInterior(playerid));
		LootZone_SetVirtualWorld(lootzoneid, Character_GetVirtualWorld(playerid));
		LootZone_Initialize(lootzoneid);
		LootZone_Refresh(lootzoneid);
		// RECREATE OBJECTS
	}
	else if(!strcmp(command, "goto", true))
	{
		new Float:x, Float:y, Float:z;
		Streamer_UpdateEx(playerid, x, y, z, LootZone_GetVirtualWorld(lootzoneid), LootZone_GetInterior(lootzoneid));
		LootZone_GetPosition(lootzoneid, x, y, z);
		SetPlayerPos(playerid, x, y, z);
		SetPlayerInterior(playerid, LootZone_GetInterior(lootzoneid));
		Character_SetVirtualWorld(playerid, LootZone_GetVirtualWorld(lootzoneid));
		// RECREATE OBJECTS
	}
	else if(!strcmp(command, "additem", true))
	{
		new itemid, minQuantity, maxQuantity, drop;
		if(sscanf(params, "k<item>ddd", itemid, minQuantity, maxQuantity, drop))
		{
			SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /lzedit <lootzoneid> additem <itemid/name> <min quant.> <max. quant.> <drop chance.>");
			SendClientMessage(playerid, COLOR_GREY, "INFO: aggiunge itemid tra gli item droppabili.");
			return 0;
		}
		
		if(drop <= 0 || drop > 100)
			return SendClientMessage(playerid, COLOR_ERROR, "Drop chance range: 1 - 100");

		if(minQuantity < 0)
			return SendClientMessage(playerid, COLOR_ERROR, "La quantità minima non può essere minore di 0.");

		if(LootZone_AddDropItem(lootzoneid, itemid, drop, minQuantity, maxQuantity))
		{
			SendFormattedMessage(playerid, -1, "[LOOTZONE %d] Item aggiunto: %s - Min: %d - Max: %d - Drop Chance: %d.", lootzoneid, ServerItem_GetName(itemid), minQuantity, maxQuantity, drop);
		}
		else
		{
			SendFormattedMessage(playerid, -1, "[LOOTZONE %d] Non è stato possibile aggiungere l'oggetto. Esso risulta già presente tra i droppabili.", lootzoneid);
		}
	}
	else if(!strcmp(command, "rmitem", true))
	{
		if(!strcmp(params, "all", true))
		{

			return 1;
		}
		new itemid;
		if(sscanf(params, "k<item>", itemid))
		{
			SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /lzedit <lootzoneid> rmitem <itemid/name>");
			SendClientMessage(playerid, COLOR_GREY, "INFO: rimuove itemid tra gli item droppabili. Usa 'all' come ultimo parametro per resettarli tutti.");
			return 0;
		}

		if(LootZone_RemoveDropItem(lootzoneid, itemid))
		{
			SendFormattedMessage(playerid, -1, "[LOOTZONE %d] Item rimosso.", lootzoneid);
		}
		else
		{
			SendFormattedMessage(playerid, -1, "[LOOTZONE %d] Non è stato possibile rimuovere l'oggetto. Esso non risulta presente tra i droppabili.", lootzoneid);
		}
	}
	else if(!strcmp(command, "resetinv", true))
	{
		LootZone_SetLootingPlayer(lootzoneid, INVALID_PLAYER_ID);
		LootZone_ClearInventory(lootzoneid);
		SendFormattedMessage(playerid, -1, "[LOOT ZONE %d] Hai resettato l'inventario.", lootzoneid);
	}
	else if(!strcmp(command, "refresh", true))
	{
		LootZone_Refresh(lootzoneid);
		SendFormattedMessage(playerid, -1, "[LOOT ZONE %d] Hai refreshato la loot zone.", lootzoneid);
	}
	else if(!strcmp(command, "items", true))
	{
		new 
			itemid = 0,
			data[e_LZ_ITEM_DATA],
			Map:items = LootZone_GetItems(lootzoneid);
		SendFormattedMessage(playerid, -1, "==================== [LOOT ZONE %d DROPPABLES] ====================", lootzoneid);
		if(map_size(items) <= 0)
			return SendClientMessage(playerid, -1, "Attualmente non ci sono oggetti droppabili in questa loot zone.");
		for_map(i : items)
		{
			itemid = iter_get_key(i);
			iter_get_arr(i, data);

			SendFormattedMessage(playerid, -1, "Item (id): %s (%d) - Min: %d - Max: %d - Drop Chance: %d", ServerItem_GetName(itemid), itemid, data[MinQuantity], data[MaxQuantity], data[DropChance]);
		}
		//SendClientMessage(playerid, -1, "====================================================================");
	}
	else if(!strcmp(command, "delete", true))
	{
		LootZone_Delete(lootzoneid);

		SendFormattedMessage(playerid, COLOR_ORANGE, "[LOOT ZONE %d] Loot Zone rimossa.", lootzoneid);
	}
	else
	{
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /lzedit <lootzoneid> <pos - additem - rmitem - resetinv - refresh - items - delete>");	
	}
	return 1;
}

flags:alzrefreshall(CMD_MODERATOR);
CMD:alzrefreshall(playerid, params[])
{
	for_map(i : LootZones)
	{
		LootZone_Refresh(iter_get_key(i));
	}
	SendMessageToAdmins(1, COLOR_ORANGE, "AdmCmd: %s ha refreshato tutte le Loot Zones.", Account_GetName(playerid));
	return 1;
}

flags:fruga(CMD_ALIVE_USER);
CMD:fruga(playerid, params[])
{
	if(GetPlayerState(playerid) != PLAYER_STATE_ONFOOT || !Character_CanInteract(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");

	new id, E_ELEMENT_TYPE:type;

	if(!Character_GetNearestPickup(playerid, id, type) || type != ELEMENT_TYPE_LOOTZONE) 
		return SendClientMessage(playerid, COLOR_ERROR, "Non c'è nulla da frugare qui.");
		
	Character_ShowLootZone(playerid, id);
	return 1;
}

stock LootZone_LoadAll()
{

	inline OnLoad()
	{
		new rows = cache_num_rows();
		
		new data[e_LOOT_ZONE_DATA];
		
		for(new i = 0; i < rows; ++i)
		{
			cache_get_value_name_int(i, "id", data[ID]);
			cache_get_value_name_float(i, "x", data[X]);
			cache_get_value_name_float(i, "y", data[Y]);
			cache_get_value_name_float(i, "z", data[Z]);
			cache_get_value_name_int(i, "interior", data[Interior]);
			cache_get_value_name_int(i, "virtual_world", data[VirtualWorld]);

			map_add_arr(LootZones, data[ID], data);

			LootZone_Initialize(data[ID]);
		}
		printf("Loaded %d loot zones.", rows);
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `loot_zones`");

	inline OnLoadDroppables()
	{
		new r = cache_num_rows();
		new 
			lootzoneid, itemid, itemData[e_LZ_ITEM_DATA];
		for(new j = 0; j < r; ++j)
		{
			cache_get_value_name_int(j, "lootzone_id", lootzoneid);
			cache_get_value_name_int(j, "item_id", itemid);
			if(!ServerItem_IsValid(itemid))
				continue;
			cache_get_value_name_int(j, "min_quantity", itemData[MinQuantity]);
			cache_get_value_name_int(j, "max_quantity", itemData[MaxQuantity]);
			cache_get_value_name_int(j, "drop_chance", itemData[DropChance]);

			new Map:items = LootZone_GetItems(lootzoneid);
			map_add_arr(items, itemid, itemData);
		}

		printf("Loaded %d droppable items for lootzone id %d.", r, lootzoneid);
	}

	MySQL_TQueryInline(gMySQL, using inline OnLoadDroppables, "SELECT * FROM `loot_zone_items`");
}