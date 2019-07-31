#include <YSI_Coding\y_hooks>

static 
    Map:Houses;

stock Map:GetHousesMap()
{
    return Houses;
}

hook OnGameModeInit()
{
    Houses = map_new();
    return 1;
}

hook OnCharacterLoad(playerid)
{
	new houseid = Character_GetHouseKey(playerid);
	if(houseid > 0)
	{
		if(!House_IsValid(houseid))
		{
			SendClientMessage(playerid, COLOR_ERROR, "La casa di cui eri affittuario non esiste più.");
			Character_SetHouseKey(playerid, 0);
            Character_Save(playerid);
		}
	}
	return 1;
}


stock House_Create(Float:x, Float:y, Float:z, int, world, interiorid)
{
    new Cache:result = mysql_query_f(gMySQL, true, "INSERT INTO `houses` \
    (OwnerID, OwnerName, EnterX, EnterY, EnterZ, EnterInterior, EnterWorld, Price, Locked) \
    VALUES(0, '', %f, %f, %f, %d, %d, 0, 1)", x, y, z, int, world);

    new 
        id = cache_insert_id(),
        data[E_HOUSE_DATA]
    ;
        
    data[hID] = id;
    data[hOwnerID] = 0;
    data[hOwnerName] = "";
    data[hPrice] = 0;

    data[hEnterX] = x;
    data[hEnterY] = y;
    data[hEnterZ] = z;
    data[hEnterInterior] = int;
    data[hEnterWorld] = world;

    data[hExitX] = 0.0;
    data[hExitY] = 0.0;
    data[hExitZ] = 0.0;
    data[hExitInterior] = 0;

    data[hLocked] = 1;

    map_add_arr(Houses, id, data);

    House_SetInterior(id, interiorid);
    // House_CreateObjects(id); // Already called by House_SetInterior

    cache_delete(result);
    return id;
}

stock House_Delete(houseid)
{
    if(!House_IsValid(houseid))
        return 0;

    new query[64];
    mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `houses` WHERE ID = '%d'", houseid);
    mysql_tquery(gMySQL, query);

    House_DestroyObjects(houseid);
    map_remove_deep(Houses, houseid);
    return 1;
}

stock House_CreateObjects(houseid)
{
    if(!House_IsValid(houseid))
	   return 0;

    House_DestroyObjects(houseid);
    new 
	   Float:x, Float:y, Float:z,
	   Float:ex, Float:ey, Float:ez,
	   interior = House_GetEnterInterior(houseid),
	   einterior = House_GetExitInterior(houseid),
	   enterWorld = House_GetEnterWorld(houseid),
       exitWorld = House_GetExitWorld(houseid);
    
    House_GetEnterPosition(houseid, x, y, z);
    House_GetExitPosition(houseid, ex, ey, ez);

    new zone[32];

    Get2DZoneName(x, y, z, zone, sizeof(zone));

    new 
	   enterPickupid = Pickup_Create(1273, houseid, x, y, z, ELEMENT_TYPE_HOUSE_ENTRANCE, enterWorld, interior),
	   exitPickupid = Pickup_Create(1007, houseid, ex, ey, ez, ELEMENT_TYPE_HOUSE_EXIT, exitWorld, einterior),
	   Text3D:label = CreateDynamic3DTextLabel(zone, 0xFFFFFF, x, y, z + 0.55, 15.0, .worldid = enterWorld, .interiorid = interior, .testlos = 1);
    
    map_set_cell(Houses, houseid, hEnterPickup, enterPickupid);
    map_set_cell(Houses, houseid, hExitPickup, exitPickupid);
    map_set_cell_safe(Houses, houseid, h3DText, _:label);
    return 1;
}

stock House_DestroyObjects(houseid)
{
    if(!House_IsValid(houseid))
	   return 0;
    new 
	   enterPickupid = House_GetEnterPickup(houseid),
	   exitPickupid = House_GetExitPickup(houseid),
	   Text3D:label = House_Get3DText(houseid);
    
    if(IsValidDynamicPickup(enterPickupid))
	   Pickup_Destroy(enterPickupid);
    if(IsValidDynamicPickup(exitPickupid))
	   Pickup_Destroy(exitPickupid);
    if(IsValidDynamic3DTextLabel(label))
	   DestroyDynamic3DTextLabelEx(label);
    return 1;
}

stock House_IsValid(houseid)
{
    return map_has_key(Houses, houseid);
}

stock House_IsCreated(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, hCreated);
}

stock House_SetCreated(houseid, created)
{
    if(!House_IsValid(houseid))
        return 0;
    map_set_cell(Houses, houseid, hCreated, created);
    return 1;
}

stock House_GetData(houseid, data[E_HOUSE_DATA])
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get_arr(Houses, houseid, data);
}

stock House_GetID(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, hID);
}

stock House_SetOwner(houseid, playerid)
{
    if(!House_IsValid(houseid) || !IsPlayerConnected(playerid) || !Character_IsLogged(playerid) || Character_HasHouseKey(playerid))
	   return 0;
    map_set_cell(Houses, houseid, hOwnerID, PlayerInfo[playerid][pID]);
    new Iter:it = map_iter(Houses, houseid);
    iter_set_cells(it, hOwnerName, Character_GetOOCName(playerid));
    return 1;
}

stock House_ResetOwner(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    map_set_cell(Houses, houseid, hOwnerID, 0);
    new Iter:it = map_iter(Houses, houseid);
    iter_set_cells(it, hOwnerName, "");
    return 1;
}

stock House_GetOwnerID(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, hOwnerID);
}

stock House_IsOwned(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return House_GetOwnerID(houseid) > 0;
}

stock bool:House_GetOwnerName(houseid, name[MAX_PLAYER_NAME])
{
    if(!House_IsValid(houseid))
        return false;
    new Iter:it = map_iter(Houses, houseid);
    iter_get_md_arr(it, {hOwnerName}, name);
    return true;
}

stock String:House_GetOwnerNameStr(houseid)
{
    if(!House_IsValid(houseid))
	   return STRING_NULL;
    new name[MAX_PLAYER_NAME];
    House_GetOwnerName(houseid, name);
    return str_new(name);
}

stock House_SetEnterPosition(houseid, Float:x, Float:y, Float:z)
{
    if(!House_IsValid(houseid))
        return 0;
    map_set_cell(Houses, houseid, hEnterX, x);
    map_set_cell(Houses, houseid, hEnterY, y);
    map_set_cell(Houses, houseid, hEnterZ, z);
    return 1;
}

stock House_GetEnterPosition(houseid, &Float:X, &Float:Y, &Float:Z)
{
    if(!House_IsValid(houseid))
        return 0;
    X = Float:map_get(Houses, houseid, hEnterX);
    Y = Float:map_get(Houses, houseid, hEnterY);
    Z = Float:map_get(Houses, houseid, hEnterZ);
    return 1;
}

stock House_SetEnterInterior(houseid, interior)
{
    if(!House_IsValid(houseid))
        return 0;
    map_set_cell(Houses, houseid, hEnterInterior, interior);
    return 1;
}

stock House_GetEnterInterior(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, hEnterInterior);
}

stock House_SetEnterWorld(houseid, world)
{
    if(!House_IsValid(houseid))
        return 0;
    map_set_cell(Houses, houseid, hEnterWorld, world);
    return 1;
}

stock House_GetEnterWorld(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, hEnterWorld);
}

stock House_GetExitWorld(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, hExitWorld);
}

stock House_SetExitPosition(houseid, Float:x, Float:y, Float:z)
{
    if(!House_IsValid(houseid))
        return 0;
    map_set_cell(Houses, houseid, hExitX, x);
    map_set_cell(Houses, houseid, hExitY, y);
    map_set_cell(Houses, houseid, hExitZ, z);
    return 1;
}

stock House_GetExitPosition(houseid, &Float:X, &Float:Y, &Float:Z)
{
    if(!House_IsValid(houseid))
        return 0;
    X = Float:map_get(Houses, houseid, hExitX);
    Y = Float:map_get(Houses, houseid, hExitY);
    Z = Float:map_get(Houses, houseid, hExitZ);
    return 1;
}

stock House_SetExitInterior(houseid, interior)
{
    if(!House_IsValid(houseid))
        return 0;
    map_set_cell(Houses, houseid, hExitInterior, interior);
    return 1;
}

stock House_GetExitInterior(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, hExitInterior);
}

stock House_SetInterior(houseid, interiorid)
{
    if(!House_IsValid(houseid))
	   return 0;
    map_set_cell(Houses, houseid, hInterior, interiorid);
    House_SetExitPosition(houseid, allHouseInteriors[interiorid][iHouseX], allHouseInteriors[interiorid][iHouseY], allHouseInteriors[interiorid][iHouseZ]);
    House_SetExitInterior(houseid, allHouseInteriors[interiorid][iHouseInt]);
    House_CreateObjects(houseid);
    House_Save(houseid);
    return 1;
}

stock House_GetInteriorID(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, hInterior);
}

stock House_GetEnterPickup(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, hEnterPickup);
}

stock House_GetExitPickup(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, hExitPickup);
}

stock House_SetPrice(houseid, price)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_set_cell(Houses, houseid, hPrice, price);
}

stock House_GetPrice(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, hPrice);
}

stock House_SetLocked(houseid, lock)
{
    if(!House_IsValid(houseid))
        return 0;
    map_set_cell(Houses, houseid, hLocked, lock);
    return 1;
}

stock House_IsLocked(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, hLocked);
}

stock House_GetMoney(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, hMoney);
}

stock House_GiveMoney(houseid, money)
{
    if(!House_IsValid(houseid))
        return 0;
    map_set_cell(Houses, houseid, hMoney, House_GetMoney(houseid) + money);
    return 1;
}

stock House_LoadAll()
{
    inline OnLoad()
    {
	   printf("Loading houses...");
	   new count = cache_num_rows(), data[E_HOUSE_DATA];
	   for(new i = 0; i < count; ++i)
	   {
			cache_get_value_index_int(i, 0, data[hID]);
			cache_get_value_index_int(i, 1, data[hOwnerID]);
			cache_get_value_index(i, 2, data[hOwnerName]);

			cache_get_value_index_float(i, 3, data[hEnterX]);
			cache_get_value_index_float(i, 4, data[hEnterY]);
			cache_get_value_index_float(i, 5, data[hEnterZ]);
			cache_get_value_index_int(i, 6, data[hEnterInterior]);
			cache_get_value_index_int(i, 7, data[hEnterWorld]);

			cache_get_value_index_float(i, 8, data[hExitX]);
			cache_get_value_index_float(i, 9, data[hExitY]);
			cache_get_value_index_float(i, 10, data[hExitZ]);
			cache_get_value_index_int(i, 11, data[hExitInterior]);
            //cache_get_value_index_int(i, 12, data[hExitWorld]);

			cache_get_value_index_int(i, 13, data[hPrice]);
			cache_get_value_index_int(i, 14, data[hLocked]);
			cache_get_value_index_int(i, 15, data[hMoney]);
			cache_get_value_index_int(i, 16, data[hInterior]);

            cache_get_value_index_int(i, 17, data[hCreated]);

            data[hExitWorld] = HOUSE_START_WORLD + i;

			map_add_arr(Houses, data[hID], data);
			House_CreateObjects(i);
			
			CallLocalFunction(#OnHouseLoaded, "d", i);
	   }
	   printf("%d houses loaded.", count);
    }
    MySQL_PQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `houses` ORDER BY ID");
}

stock House_Save(houseid)
{
    if(!House_IsValid(houseid) || !House_IsCreated(houseid))
	   return 0;
    inline OnSave()
    {
	   //printf("House %d saved successfully.", houseid);
    }
    new 
        Float:x, Float:y, Float:z, Float:ex, Float:ey, Float:ez,
        name[MAX_PLAYER_NAME];
    
    House_GetEnterPosition(houseid, x, y, z);
    House_GetExitPosition(houseid, ex, ey, ez);
    House_GetOwnerName(houseid, name);

    MySQL_TQueryInline(gMySQL, using inline OnSave, "UPDATE `houses` SET \
    OwnerID = '%d', OwnerName = '%e', \
    EnterX = '%f', EnterY = '%f', EnterZ = '%f', EnterInterior = '%d', EnterWorld = '%d', \
    ExitX = '%f', ExitY = '%f', ExitZ = '%f', ExitInterior = '%d', \
    Price = '%d', Locked = '%d', Money = '%d', Interior = '%d', Created = '%d' \
    WHERE ID = '%d'", 
    House_GetOwnerID(houseid), name, 
    x, y, z, House_GetEnterInterior(houseid), House_GetEnterWorld(houseid),
    ex, ey, ez, House_GetExitInterior(houseid),
    House_GetPrice(houseid), House_IsLocked(houseid), House_GetMoney(houseid), House_GetInteriorID(houseid),
    House_IsCreated(houseid),
	House_GetID(houseid));
    return 1;
}

stock House_ShowInventory(houseid, playerid)
{
    if(!House_IsValid(houseid))
        return 0;
    #pragma unused houseid, playerid
    new Inventory:inventory = House_GetInventory(houseid);
	Inventory_ShowStr(inventory, playerid, str_format("Inventario Casa (%d/%d)", Inventory_GetUsedSpace(inventory), Inventory_GetSpace(inventory)), Dialog_HouseInventory, "Avanti", "Chiudi");
	return 1;
}

Dialog:Dialog_HouseInventory(playerid, response, listitem, inputtext[])
{
	
	return 1;
}


stock IsPlayerInRangeOfHouseEntrance(playerid, houseid, Float:range = 2.5)
{
	if(!House_IsValid(houseid))
		return 0;
	new pickupid = House_GetEnterPickup(houseid);
	return IsPlayerInRangeOfPickup(playerid, pickupid, range);
}

stock IsPlayerInRangeOfHouseExit(playerid, houseid, Float:range = 2.5)
{
	if(!House_IsValid(houseid))
		return 0;
	new pickupid = House_GetExitPickup(houseid);
	return IsPlayerInRangeOfPickup(playerid, pickupid, range);
}

stock Text3D:House_Get3DText(houseid)
{
    return Text3D:map_get(Houses, houseid, h3DText);
}