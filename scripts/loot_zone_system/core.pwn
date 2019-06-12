#include <YSI_Coding\y_hooks>


#define LootZone_IsValid(%0) (!(%0 < 0 || %0 >= MAX_LOOT_ZONES || !LootZoneInfo[%0][lzID]))
#define LootZone_IsEditing(%0) (LootZoneInfo[%0][lzPlayerEditing] != -1 && Player_GetEditingLootZone(LootZoneInfo[%0][lzPlayerEditing]) == %0)
#define LootZone_GetPlayerEditing(%0) LootZoneInfo[%0][lzPlayerEditing]
#define LootZone_RemovePlayerEditing(%0) LootZoneInfo[%0][lzPlayerEditing] = -1


// Player Checks
#define Player_GetLootZone(%0) 					PlayerInfo[%0][pLootZone]
#define Player_StopEditingLootZone(%0)			DeletePVar(%0, "LootZoneSelectedLootZoneID")
#define Player_GetEditingLootZone(%0) 			GetPVarInt(%0, "LootZoneSelectedLootZoneID")			
#define Player_IsEditingAnyLootZone(%0) 		(LootZone_GetPlayerEditing(Player_GetEditingLootZone(%0)) == %0)
#define Player_IsLooting(%0)					(PlayerInfo[%0][pLootZone] != -1 && LootZoneInfo[PlayerInfo[%0][pLootZone]][lzPlayerID] == %0)

stock Player_SetEditingLootZone(playerid, zone_id) SetPVarInt(playerid, "LootZoneSelectedLootZoneID", zone_id);

#define Player_GetLastLootZoneAdminListItemID(%0) 	GetPVarInt(playerid, "Player_AdminLootZoneListLastID")

stock LootZone_SetPlayerEditing(zone_id, playerid) LootZoneInfo[zone_id][lzPlayerEditing] = playerid;

stock LootZone_IsPlayerLootingThis(zone_id, playerid)
{
	return LootZone_IsValid(zone_id) && PlayerInfo[playerid][pLootZone] == zone_id && LootZoneInfo[zone_id][lzPlayerID] == playerid;
}

hook OnPlayerClearData(playerid)
{
	if(Player_IsEditingAnyLootZone(playerid))
	{
		LootZone_RemovePlayerEditing(Player_GetEditingLootZone(playerid));
		//SendMessageToAdmins(false, COLOR_ADMIN, "[ADM-CMD] %d hook disconnected. (TESTING)", playerid);
	}
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(PRESSED(KEY_WALK) && Character_IsAlive(playerid))
	{
		if((new pickupid = Character_GetLastPickup(playerid)) != -1 && GetPlayerState(playerid) == PLAYER_STATE_ONFOOT)
		{
			new id, E_ELEMENT_TYPE:type;
			if(Pickup_GetInfo(pickupid, id, type) && type == ELEMENT_TYPE_LOOTZONE && IsPlayerInRangeOfLootZone(playerid, id, 2.0))
			{
				LootZone_ShowItemsToPlayer(playerid, id);
			}
		}
	}
	return 1;
}

// Called every second (by SecondCheck) when a player is inspecting (listing) a loot zone.
hook GlobalPlayerSecondTimer(playerid)
{
	new zone_id = Player_GetLootZone(playerid);
	if(zone_id != -1 && !IsPlayerInRangeOfLootZone(playerid, zone_id) || Character_IsFreezed(playerid))
	{
		Player_ClearLootZoneVars(playerid);
		Dialog_Close(playerid);
	}
}

stock LootZone_StartEdit(zone_id, playerid)
{
	if(!LootZone_IsValid(zone_id))
		return 0;

	if(LootZone_IsEditing(zone_id))
	{
		new playerEditing = LootZone_GetPlayerEditing(zone_id);
		if(playerEditing != playerid)
		{
			if(Dialog_Opened(playerEditing)) // If editing player has dialog opened, returns 0.
				return 0;
			else // else, we can edit this zone.
				Player_StopEditingLootZone(playerEditing);
		}
	}

	if(Player_IsEditingAnyLootZone(playerid))
	{
		LootZone_EndEdit(Player_GetEditingLootZone(playerid), playerid);
	}

	Player_SetEditingLootZone(playerid, zone_id);
	LootZone_SetPlayerEditing(zone_id, playerid);

	new 
		string[64];
	format(string, sizeof(string), "Modifica Loot Zone [%d]", zone_id);
	Dialog_Show(playerid, Dialog_LootZoneEdit, DIALOG_STYLE_LIST, string, "Aggiungi Item\nModifica Item\nSposta Qui\nGo To\nRimuovi Zona", "Seleziona", "Indietro");
	return 1;
}

stock LootZone_EndEdit(zone_id, playerid)
{
	if(!LootZone_IsEditing(zone_id) || Player_GetEditingLootZone(playerid) != zone_id)
	{
		return 0; // playerid isn't editing zone_id
	}
	Player_StopEditingLootZone(playerid);
	LootZone_RemovePlayerEditing(zone_id);
	Dialog_Close(playerid); // Safeness
	return 1;
}

stock LootZone_AdminShowMenu(playerid, last_lz_id = 0)
{
	if(last_lz_id < 0)
		return LootZone_AdminShowMenu(playerid, 0);

	new
		string[1024],
		count = 0,
		last_lz_tmp = 0,
		extra[32];

	for(new i = last_lz_id; i < MAX_LOOT_ZONES; ++i)
	{
		if(!LootZone_IsValid(i))
			continue;
		
		extra = "";
		
		format(extra, sizeof(extra), LootZone_IsEditing(i) ? "{FF0000}Modificando{FFFFFF}" : "{00FF00}Modificabile{FFFFFF}");
			
		if(IsPlayerInRangeOfLootZone(playerid, i))
			format(string, sizeof(string), "%sLoot Zone %d (Vicino) [%s]\n", string, i, extra);
		else
			format(string, sizeof(string), "%sLoot Zone %d [%s]\n", string, i, extra);
		
		LootZoneListItem[playerid][count] = i;
		count++;

		last_lz_tmp = i;

		if(count == MAX_LOOT_ZONES_PER_PAGE)
			break;		
	}

	if(count == 0)
		return 0;

	SetPVarInt(playerid, "Player_AdminLootZoneListCount", count);
	SetPVarInt(playerid, "Player_AdminLootZoneListLastID", last_lz_tmp);

	Dialog_Show(playerid, Dialog_LootZoneList, DIALOG_STYLE_LIST, "Gestisci Loot Zones", "Avanti -->\n<-- Indietro\nModifica da ID\n%s", "Seleziona", "Chiudi", string);
	return 1;
}

stock LootZone_ShowItemsToPlayer(playerid, zone_id)
{
	if(!LootZone_IsValid(zone_id) || !IsPlayerInRangeOfLootZone(playerid, zone_id) ||
		GetPlayerVirtualWorld(playerid) != LootZoneInfo[zone_id][lzVirtualWorld] ||
		GetPlayerInterior(playerid) != LootZoneInfo[zone_id][lzInterior])
		return 0;
	
	if(Character_IsFreezed(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando adesso.");

	if(LootZoneInfo[zone_id][lzPlayerID] != -1 && LootZoneInfo[zone_id][lzPlayerID] != playerid)
	{
		return SendClientMessage(playerid, COLOR_ERROR, Qualcuno sta già frugrando qui.");
	}

	new
		string[1024],
		item_id,
		extra_str[8],
		count = 0
		;

	for(new i = 0; i < MAX_ITEMS_PER_LOOT; ++i)
	{
		if(!LootZoneInfo[zone_id][lzCurrentLootItems][i])
			continue;
		item_id = LootZoneInfo[zone_id][lzCurrentLootItems][i];

		extra_str = "-";
		//if(Inventory_IsFood(item_id)) format(extra_str, sizeof(extra_str), "%d%%", LootZoneInfo[zone_id][lzCurrentLootExtra][i]);
		format(string, sizeof(string), "%s%s\t%d\t%s\n", 
			string, ServerItem_GetName(item_id), LootZoneInfo[zone_id][lzCurrentLootAmount][i], extra_str);
		
		LootZoneListItem[playerid][count] = i;
		count++;
	}
	if(!count)
	{
		if(LootZone_IsPlayerLootingThis(zone_id, playerid))
			SendClientMessage(playerid, COLOR_ERROR, Non c'è più nulla da frugare qui.");
		else
			SendClientMessage(playerid, COLOR_ERROR, Non c'è nulla da frugare qui.");
		Player_ClearLootZoneVars(playerid);
		return 1;
	}
	
	PlayerInfo[playerid][pLootZone] = zone_id;
	LootZoneInfo[zone_id][lzPlayerID] = playerid;
	
	Dialog_Show(playerid, Dialog_ShowItemsToPlayer, DIALOG_STYLE_TABLIST_HEADERS, "Loot Zone", "Oggetto\tQuantità\tValore\n%s", "Raccogli", "Chiudi", string);
	return 1;
}

stock LootZone_CreateObjects(zone_id, Float:_x, Float:_y, Float:_z, _interior, _virtual_world)
{
	if(IsValidDynamic3DTextLabel(LootZoneInfo[zone_id][lz3DText]))
		DestroyDynamic3DTextLabelEx(LootZoneInfo[zone_id][lz3DText]);
	
	if(IsValidDynamicObject(LootZoneInfo[zone_id][lzObject]))
		DestroyDynamicObject(LootZoneInfo[zone_id][lzObject]);

	if(IsValidDynamicPickup(LootZoneInfo[zone_id][lzPickup]))
		DestroyDynamicPickup(LootZoneInfo[zone_id][lzPickup]);

	new label[32];
	format(label, sizeof(label), "Loot Zone [%d]", zone_id);
	LootZoneInfo[zone_id][lz3DText] = CreateDynamic3DTextLabel(label, COLOR_GREY, _x, _y, _z + 0.55, 10.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, _virtual_world, _interior);
	LootZoneInfo[zone_id][lzObject] = CreateDynamicObject(3005, _x, _y, _z - 0.9, 0.0, 0.0, 0.0, _virtual_world, _interior, _, _, _);
	LootZoneInfo[zone_id][lzPickup] = Pickup_Create(1007, zone_id, _x, _y, _z, ELEMENT_TYPE_LOOTZONE, _virtual_world, _interior);
}

stock LootZone_InitializeInventory(zoneid)
{
	new Inventory:inventory = Inventory_New(10);
	list_add(LootZoneInventoryList, List:inventory);
	return 1;
}

stock LootZone_UnloadInventory(zoneid)
{
	return list_remove_deep(LootZoneInventoryList, zoneid);
}

stock LootZone_Create(Float:_x, Float:_y, Float:_z, _interior, _virtual_world)
{
	inline OnInsert()
	{
		new
			empty_items_string[128],
			data[E_LOOT_ZONE];
		data[lzID] = cache_insert_id();
		data[lzX] = _x;
		data[lzY] = _y;
		data[lzZ] = _z;
		data[lzInterior] = _interior;
		data[lzVirtualWorld] = _virtual_world;
		data[lzPlayerID] = -1;

		for(new i = 0; i < MAX_ITEMS_PER_LOOT; ++i)
		{
			data[lzLootItem][i] = 0;
			data[lzLootAmount][i] = 0;
			format(empty_items_string, sizeof(empty_items_string), "%s%s", empty_items_string, (i == MAX_ITEMS_PER_LOOT-1) ? "0" : "0|");
		}

		new idx = list_add_arr(LootZoneList, data);
		
		LootZone_CreateObjects(idx, _x, _y, _z, _interior, _virtual_world);
		LootZone_RemovePlayerEditing(idx);
	}

	MySQL_TQueryInline(gMySQL, using inline OnInsert, "INSERT INTO `loot_zones` (lzX, lzY, lzZ, lz_interior, lz_virtual_world, lz_possible_items, lz_possible_items_amount, lz_items_rarity) VALUES('%f', '%f', '%f', '%d', '%d', '%s', '%s', '%s')", 
															_x, _y, _z, _interior, _virtual_world, empty_items_string, empty_items_string, empty_items_string);
	return idx;
}

stock LootZone_SetPosition(zone_id, Float:_x, Float:_y, Float:_z, _interior = -1, _virtual_world = -1)
{
	if(!LootZone_IsValid(zone_id))
		return;
	LootZoneInfo[zone_id][lzX] = _x;
	LootZoneInfo[zone_id][lzY] = _y;
	LootZoneInfo[zone_id][lzZ] = _z;
	LootZoneInfo[zone_id][lzInterior] = _interior;
	LootZoneInfo[zone_id][lzVirtualWorld] = _virtual_world;

	new label[32];
	format(label, sizeof(label), "Loot Zone [%d]", zone_id);

	LootZone_CreateObjects(zone_id, _x, _y, _z, _interior, _virtual_world);

	LootZone_Save(zone_id);
}

stock LootZone_Delete(zoneid)
{
	if(!LootZone_IsValid(zoneid))
		return 0;
	LootZoneInfo[zoneid][lzID] = 0;
	LootZoneInfo[zoneid][lzX] = 0.0;
	LootZoneInfo[zoneid][lzY] = 0.0;
	LootZoneInfo[zoneid][lzZ] = 0.0;
	LootZoneInfo[zoneid][lzInterior] = LootZoneInfo[zoneid][lzVirtualWorld] = 0;

	for(new i = 0; i < MAX_ITEMS_PER_LOOT; ++i)
	{
		LootZoneInfo[zoneid][lzLootItem][i] = 0;
		LootZoneInfo[zoneid][lzLootAmount][i] = 0;
		LootZoneInfo[zoneid][lzLootRarity][i] = 0;
	}

	if(LootZone_IsEditing(zoneid))
	{
		LootZone_EndEdit(zoneid, LootZone_GetPlayerEditing(zoneid));
	}

	if(LootZoneInfo[zoneid][lzPlayerID] != -1)
	{
		Player_ClearLootZoneVars(LootZoneInfo[zoneid][lzPlayerID]);
	}

	LootZone_RemovePlayerEditing(zoneid);
	
	DestroyDynamicObject(LootZoneInfo[zoneid][lzObject]);
	DestroyDynamicPickup(LootZoneInfo[zoneid][lzPickup]);
	DestroyDynamic3DTextLabelEx(LootZoneInfo[zoneid][lz3DText]);

	Iter_Remove(LootZoneIterator, zoneid);

	new 
		query[128];

	format(query, sizeof(query), "DELETE FROM `loot_zones` WHERE id = '%d'", LootZoneInfo[zoneid][lzID]);
	mysql_tquery(gMySQL, query);
	return 1;
}

stock LootZone_FreeItemSlot(zone_id)
{
	if(!LootZone_IsValid(zone_id))
		return -1;
	for(new i = 0; i < MAX_ITEMS_PER_LOOT; ++i)
	{
		if(!LootZoneInfo[zone_id][lzCurrentLootItems][i])
			return i;
	}
	return -1;
}

stock LootZone_AddItem(zone_id, item_id, amount = 1, extra = 0)
{
	if(!LootZone_IsValid(zone_id))
		return -1;

	//new type = InventoryObjects[item_id][invType];
	//new item_slot = LootZone_HasItem(zone_id, item_id);

	for(new i = 0; i < amount; ++i)
	{
		new free_slot = LootZone_FreeItemSlot(zone_id);

		if(free_slot == -1)
			break;

		LootZoneInfo[zone_id][lzCurrentLootItems][free_slot] = item_id;
		LootZoneInfo[zone_id][lzCurrentLootAmount][free_slot] = 1;
		LootZoneInfo[zone_id][lzCurrentLootExtra][free_slot] = extra;
	}
	/*if(item_slot != -1 && type != ITEM_DRUG && !Inventory_IsUnique(item_id))
		LootZoneInfo[zone_id][lzCurrentLootAmount][item_slot] += amount;
	else
	{
		item_slot = LootZone_FreeItemSlot(zone_id);
		if(item_slot != -1)
		{
			LootZoneInfo[zone_id][lzCurrentLootItems][item_slot] = item_id;
			LootZoneInfo[zone_id][lzCurrentLootAmount][item_slot] = amount;
			LootZoneInfo[zone_id][lzCurrentLootExtra][item_slot] = extra;			
		}
	}*/
	LootZone_Save(zone_id);
	return 1;
}

stock LootZone_HasItem(zone_id, item_id)
{
	if(!LootZone_IsValid(zone_id))
		return -1;
	for(new i = 0; i < MAX_ITEMS_PER_LOOT; ++i)
	{
		if(LootZoneInfo[zone_id][lzCurrentLootItems][i] == item_id)
			return i;
	}
	return -1;
}

stock LootZone_LoadAll()
{
	inline OnLoad()
	{
		new 
			count = 0,
			tempString[1024],
			sscanfFormat[32];

		cache_get_row_count(count);

		// Called here to avoid calling format in loop everytime.
		format(sscanfFormat, sizeof(sscanfFormat), "p<|>a<i>[%d]", MAX_ITEMS_PER_LOOT);

		for(new i = 0; i < count; ++i)
		{
			if(i >= MAX_LOOT_ZONES)
				break;
			cache_get_value_index_int(i, 0, LootZoneInfo[i][lzID]);
			cache_get_value_index_float(i, 1, LootZoneInfo[i][lzX]);
			cache_get_value_index_float(i, 2, LootZoneInfo[i][lzY]);
			cache_get_value_index_float(i, 3, LootZoneInfo[i][lzZ]);

			cache_get_value_index_int(i, 4, LootZoneInfo[i][lzInterior]);
			cache_get_value_index_int(i, 5, LootZoneInfo[i][lzVirtualWorld]);

			cache_get_value_index(i, 6, tempString);
			sscanf(tempString, sscanfFormat, LootZoneInfo[i][lzLootItem]);

			cache_get_value_index(i, 7, tempString);
			sscanf(tempString, sscanfFormat, LootZoneInfo[i][lzLootAmount]);

			cache_get_value_index(i, 8, tempString);
			sscanf(tempString, sscanfFormat, LootZoneInfo[i][lzLootRarity]);

			LootZoneInfo[i][lzPlayerID] = -1;

			LootZone_RemovePlayerEditing(i);
			
			LootZone_CreateObjects(i, LootZoneInfo[i][lzX], LootZoneInfo[i][lzY], LootZoneInfo[i][lzZ], LootZoneInfo[i][lzInterior], LootZoneInfo[i][lzVirtualWorld]);

			Iter_Add(LootZoneIterator, i);
		}

		//LootZone_RefreshAll();
	}
	mysql_tquery_inline(gMySQL, "SELECT * FROM loot_zones ORDER BY id ASC", using inline OnLoad, ""); // DESC?
}

stock LootZone_Save(idx)
{
	if(!LootZone_IsValid(idx))
		return;
	new 
		query[1024],
		possible_items_s[256],
		possible_items_amount_s[256],
		items_rarity_s[256],
		suffix[2] = "";

	for(new i = 0; i < MAX_ITEMS_PER_LOOT; ++i)
	{
		suffix = (i == MAX_ITEMS_PER_LOOT-1) ? "" : "|";
		format(possible_items_s, sizeof(possible_items_s), "%s%d%s", possible_items_s, LootZoneInfo[idx][lzLootItem][i], suffix);
		format(possible_items_amount_s, sizeof(possible_items_amount_s), "%s%d%s", possible_items_amount_s, LootZoneInfo[idx][lzLootAmount][i], suffix);
		format(items_rarity_s, sizeof(items_rarity_s), "%s%d%s", items_rarity_s, LootZoneInfo[idx][lzLootRarity][i], suffix);
	}

	format(query, sizeof(query), "UPDATE loot_zones SET lzX = '%f', lzY = '%f', lzZ = '%f', lz_interior = '%d', lz_virtual_world = '%d', lz_possible_items = '%s', lz_possible_items_amount = '%s', lz_items_rarity = '%s' \
		WHERE id = '%d'",
		LootZoneInfo[idx][lzX], LootZoneInfo[idx][lzY], LootZoneInfo[idx][lzZ],
		LootZoneInfo[idx][lzInterior], LootZoneInfo[idx][lzVirtualWorld],
		possible_items_s, possible_items_amount_s, items_rarity_s, 
		LootZoneInfo[idx][lzID]);
	mysql_tquery(gMySQL, query);
}

stock LootZone_Refresh(zone_id)
{
	if(!LootZone_IsValid(zone_id))
		return 0;
	// Maybe close players dialogs?
	foreach(new p : Player)
	{
		if(Player_GetLootZone(p)  != -1)
		{
			Player_ClearLootZoneVars(p);
			Dialog_Close(p);
		}
	}
	// First reset current loot.
	for(new i = 0; i < MAX_ITEMS_PER_LOOT; ++i)
	{
		LootZoneInfo[zone_id][lzCurrentLootItems][i] = 0;
		LootZoneInfo[zone_id][lzCurrentLootAmount][i] = 0;
		LootZoneInfo[zone_id][lzCurrentLootExtra][i] = 0;

		if(!LootZoneInfo[zone_id][lzLootItem][i] || LootZoneInfo[zone_id][lzLootAmount][i] == 0 || LootZoneInfo[zone_id][lzLootRarity][i] == 0)
			continue;

		new rand = random(101); // 100

		if(rand >= 100-LootZoneInfo[zone_id][lzLootRarity][i])
		{
			new qnt = 1 + random(LootZoneInfo[zone_id][lzLootAmount][i]);
			LootZone_AddItem(zone_id, LootZoneInfo[zone_id][lzLootItem][i], qnt, 100);
		}
	}

	// Decide which items to spawn, randomly
	return 1;
}

stock LootZone_RefreshAll()
{
	//lzCurrLootItems[MAX_ITEMS_PER_LOOT],
	//lzLootAmount[MAX_ITEMS_PER_LOOT],
	//lzCurrLootAmount[MAX_ITEMS_PER_LOOT],
	foreach(new zone_id : LootZoneIterator)
	{
		LootZone_Refresh(zone_id);
	}

	//SendAdminAlert(true, COLOR_ORANGE, "** [SERVER]: Tutte le Loot Zone sono state refreshate.");
}

stock IsPlayerInRangeOfLootZone(playerid, zone_id, Float:range = 2.5)
{
	return LootZone_IsValid(zone_id) && IsPlayerInRangeOfPoint(playerid, range, LootZoneInfo[zone_id][lzX], LootZoneInfo[zone_id][lzY], LootZoneInfo[zone_id][lzZ]);
}

stock Player_ClearLootZoneVars(playerid)
{
	for(new i = 0; i < sizeof(LootZoneListItem[]); ++i)
	{
		LootZoneListItem[playerid][i] = -1;
	}
	new
		zone_id = Player_GetLootZone(playerid);
	if(zone_id != -1)
	{
		LootZoneInfo[zone_id][lzPlayerID] = -1;
		PlayerInfo[playerid][pLootZone] = -1;
		Dialog_Close(playerid);
	}
}