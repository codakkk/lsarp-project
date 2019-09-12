

#if defined _included_HOUSE
    #endinput
#endif 

#define _included_HOUSE 

#include <house_system\components\inventory>
#include <house_system\components\interiors>

#include <house_system\commands>

#include <YSI_Coding\y_hooks>

#define INVALID_HOUSE_ID	(-1)
#define HOUSE_START_WORLD 	(1000)

static enum _:E_HOUSE_DATA
{
    ID,

	Created,

    OwnerID,
    OwnerName[MAX_PLAYER_NAME],
    Price,
    Locked,
	Money,

    Float:EnterX,
    Float:EnterY,
    Float:EnterZ,
    EnterInterior,
    EnterWorld,

    Float:ExitX,
    Float:ExitY,
    Float:ExitZ,
    ExitInterior,
    ExitWorld,
	
    InteriorType,
	InteriorID,

    EnterPickup,
    ExitPickup,
    Text3D:h3DText,
};

static 
    Map:Houses;

stock Map:GetHousesMap()
{
    return Houses;
}

hook OnGameModeInit()
{
    Houses = map_new();
    House_LoadAll();
    return 1;
}

hook function Character_Enter(playerid)
{
    if(IsPlayerInAnyVehicle(playerid))
        return continue(playerid);
    
    printf("House Enter");
    new id, E_ELEMENT_TYPE:type;
    if(Character_GetNearestPickup(playerid, id, type))
    {
        if(type == ELEMENT_TYPE_HOUSE_ENTRANCE)
        {
            if(House_IsLocked(id))
               return GameTextForPlayer(playerid, "~r~Porta Chiusa", 5000, 1);
            
            new 
                Float:x = 0.0, 
                Float:y = 0.0, 
                Float:z = 0.0,
                interiorId = House_GetExitInterior(id),
                world = House_GetExitWorld(id);
            
            House_GetExitPosition(id, x, y, z);

            Streamer_UpdateEx(playerid, x, y, z, world, interiorId);

            SetPlayerInterior(playerid, interiorId);
            Character_SetVirtualWorld(playerid, world);
            
            // Should I use Fading?
            Character_SetSafePosition(playerid, x, y, z, interiorId, world);
            
            Character_AMe(playerid, "apre la porta ed entra.");
            return 1;
        }
    }
    return continue(playerid);
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
		}
	}
	return 1;
}

hook OnCharacterChangeName(playerid, newCharacterName[])
{
	for_map(i : Houses)
	{
        new houseid = iter_get_key(i);
		if(House_GetOwnerID(houseid) == Character_GetID(playerid))
		{
			new Iter:it = map_iter_at(Houses, houseid);
            iter_set_cells(it, OwnerName, newCharacterName, MAX_PLAYER_NAME);
		}
	}
	return 1;
}


stock House_Create(Float:x, Float:y, Float:z, int, world, interiorType)
{
    new Cache:result = mysql_query_f(gMySQL, true, "INSERT INTO `houses` \
    (owner_id, enter_x, enter_y, enter_z, enter_interior, enter_world, price, locked) \
    VALUES(0, %f, %f, %f, %d, %d, 0, 1)", x, y, z, int, world);

    new 
        id = cache_insert_id(),
        data[E_HOUSE_DATA]
    ;
    // we
    data[Created] = 1;
    data[ID] = id;
    data[OwnerID] = 0;
    data[OwnerName] = "";
    data[Price] = 0;

    data[EnterX] = x;
    data[EnterY] = y;
    data[EnterZ] = z;
    data[EnterInterior] = int;
    data[EnterWorld] = world;

    data[ExitX] = 0.0;
    data[ExitY] = 0.0;
    data[ExitZ] = 0.0;
    data[ExitInterior] = 0;

    data[ExitWorld] = HOUSE_START_WORLD + id;

    data[Locked] = 1;

    map_add_arr(Houses, id, data);

    House_SetInterior(id, interiorType, 0);
    House_CreateObjects(id); // Already called by House_SetInterior
    CallLocalFunction(#OnHouseLoaded, "d", data[ID]);
    cache_delete(result);
    return id;
}

stock House_Delete(houseid)
{
    if(!House_IsValid(houseid))
        return 0;

    new query[64];
    mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `houses` WHERE id = '%d'", House_GetID(houseid));
    mysql_tquery(gMySQL, query);

    House_DestroyObjects(houseid);
    map_remove_deep(Houses, House_GetID(houseid));
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

    if(enterWorld != 0)
    {
        format(zone, sizeof(zone), "Casa");
    }
    else
        Get2DZoneName(x, y, z, zone, sizeof(zone));

    new 
	   enterPickupid = Pickup_Create(1273, houseid, x, y, z, ELEMENT_TYPE_HOUSE_ENTRANCE, enterWorld, interior),
	   exitPickupid = Pickup_Create(1007, houseid, ex, ey, ez, ELEMENT_TYPE_HOUSE_EXIT, exitWorld, einterior),
	   Text3D:label = CreateDynamic3DTextLabel(zone, 0xFFFFFF, x, y, z + 0.55, 15.0, .worldid = enterWorld, .interiorid = interior, .testlos = 1);
    
    map_set_cell(Houses, houseid, EnterPickup, enterPickupid);
    map_set_cell(Houses, houseid, ExitPickup, exitPickupid);
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
    return map_get(Houses, houseid, Created);
}

stock House_SetCreated(houseid, created)
{
    if(!House_IsValid(houseid))
        return 0;
    map_set_cell(Houses, houseid, Created, created);

    mysql_tquery_f(gMySQL, "UPDATE `houses` SET created = '%d' WHERE id = '%d'", created, House_GetID(houseid));
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
    return map_get(Houses, houseid, ID);
}

stock House_SetOwner(houseid, playerid)
{
    if(!House_IsValid(houseid) || !IsPlayerConnected(playerid) || !Character_IsLogged(playerid) || Character_HasHouseKey(playerid))
	   return 0;
    map_set_cell(Houses, houseid, OwnerID, Character_GetID(playerid));
    new Iter:it = map_iter_at(Houses, houseid);
    iter_set_cells(it, OwnerName, Character_GetOOCName(playerid));

    mysql_tquery_f(gMySQL, "UPDATE `houses` SET owner_id = '%d' WHERE id = '%d'", Character_GetID(playerid), House_GetID(houseid));
    return 1;
}

stock House_ResetOwner(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    map_set_cell(Houses, houseid, OwnerID, 0);
    new Iter:it = map_iter_at(Houses, houseid);
    iter_set_cells(it, OwnerName, "");

    mysql_tquery_f(gMySQL, "UPDATE `houses` SET owner_id = '0' WHERE id = '%d'", House_GetID(houseid));
    
    return 1;
}

stock House_GetOwnerID(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, OwnerID);
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
    new Iter:it = map_iter_at(Houses, houseid);
    iter_get_md_arr(it, {OwnerName}, name);
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
    map_set_cell(Houses, houseid, EnterX, x);
    map_set_cell(Houses, houseid, EnterY, y);
    map_set_cell(Houses, houseid, EnterZ, z);

    mysql_tquery_f(gMySQL, "UPDATE `houses` SET enter_x = '%f', enter_y = '%f', enter_z = '%f' WHERE id = '%d'", x, y, z, House_GetID(houseid));
    return 1;
}

stock House_GetEnterPosition(houseid, &Float:X, &Float:Y, &Float:Z)
{
    if(!House_IsValid(houseid))
        return 0;
    X = Float:map_get(Houses, houseid, EnterX);
    Y = Float:map_get(Houses, houseid, EnterY);
    Z = Float:map_get(Houses, houseid, EnterZ);
    return 1;
}

stock House_SetEnterInterior(houseid, interior)
{
    if(!House_IsValid(houseid))
        return 0;
    map_set_cell(Houses, houseid, EnterInterior, interior);

    mysql_tquery_f(gMySQL, "UPDATE `houses` SET enter_interior = '%d' WHERE id = '%d'", interior, House_GetID(houseid));
    return 1;
}

stock House_GetEnterInterior(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, EnterInterior);
}

stock House_SetEnterWorld(houseid, world)
{
    if(!House_IsValid(houseid))
        return 0;
    map_set_cell(Houses, houseid, EnterWorld, world);

    mysql_tquery_f(gMySQL, "UPDATE `houses` SET enter_world = '%d' WHERE id = '%d'", world, House_GetID(houseid));
    return 1;
}

stock House_GetEnterWorld(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, EnterWorld);
}

stock House_GetExitWorld(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, ExitWorld);
}

stock House_SetExitPosition(houseid, Float:x, Float:y, Float:z)
{
    if(!House_IsValid(houseid))
        return 0;
    map_set_cell(Houses, houseid, ExitX, x);
    map_set_cell(Houses, houseid, ExitY, y);
    map_set_cell(Houses, houseid, ExitZ, z);

    mysql_tquery_f(gMySQL, "UPDATE `houses` SET exit_x = '%f', exit_y = '%f', exit_z = '%f' WHERE id = '%d'", x, y, z, House_GetID(houseid));
    return 1;
}

stock House_GetExitPosition(houseid, &Float:X, &Float:Y, &Float:Z)
{
    if(!House_IsValid(houseid))
        return 0;
    X = Float:map_get(Houses, houseid, ExitX);
    Y = Float:map_get(Houses, houseid, ExitY);
    Z = Float:map_get(Houses, houseid, ExitZ);
    return 1;
}

stock House_SetExitInterior(houseid, interior)
{
    if(!House_IsValid(houseid))
        return 0;
    map_set_cell(Houses, houseid, ExitInterior, interior);

    mysql_tquery_f(gMySQL, "UPDATE `houses` SET exit_interior = '%d' WHERE id = '%d'", interior, House_GetID(houseid));
    return 1;
}

stock House_GetExitInterior(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, ExitInterior);
}

stock House_SetInterior(houseid, interiorType, interiorid)
{
    if(!House_IsValid(houseid) || !HouseInterior_IsValidType(interiorType))
	   return 0;

    new data[E_INTERIORS_HOUSE];
    if(!HouseInterior_Get(interiorType, interiorid, data))
        return 0;

    map_set_cell(Houses, houseid, InteriorType, interiorType);
    map_set_cell(Houses, houseid, InteriorID, interiorid);

    if(House_GetID(houseid) > 0)
        mysql_tquery_f(gMySQL, "UPDATE `houses` SET interior_id = '%d', interior_type = '%d' WHERE id = '%d'", interiorid, interiorType, House_GetID(houseid));
    
    House_SetExitPosition(houseid, data[iHouseX], data[iHouseY], data[iHouseZ]);
    House_SetExitInterior(houseid, data[iHouseInt]);
    House_CreateObjects(houseid);
    return 1;
}

stock House_GetInteriorType(houseid)
{
    if(!House_IsValid(houseid))
        return HOUSE_BUGGED;
    return map_get(Houses, houseid, InteriorType);
}


stock House_GetInteriorID(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, InteriorID);
}

stock bool:House_IsRoulotte(houseid)
{
    if(!House_IsValid(houseid))
        return false;
    new type = House_GetInteriorType(houseid);
    return (type == ROULOTTE_VERY_SMALL || type == ROULOTTE_SMALL || type == ROULOTTE_MEDIUM || type == ROULOTTE_BIG);
}

stock House_GetEnterPickup(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, EnterPickup);
}

stock House_GetExitPickup(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, ExitPickup);
}

stock House_SetPrice(houseid, price)
{
    if(!House_IsValid(houseid))
        return 0;
    map_set_cell(Houses, houseid, Price, price);
    mysql_tquery_f(gMySQL, "UPDATE `houses` SET price = '%d' WHERE id = '%d'", price, House_GetID(houseid));
    return 1;
}

stock House_GetPrice(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, Price);
}

stock House_SetLocked(houseid, lock)
{
    if(!House_IsValid(houseid))
        return 0;
    map_set_cell(Houses, houseid, Locked, lock);
    mysql_tquery_f(gMySQL, "UPDATE `houses` SET locked = '%d' WHERE id = '%d'", lock, House_GetID(houseid));
    return 1;
}

stock House_IsLocked(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, Locked);
}

stock House_GetMoney(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    return map_get(Houses, houseid, Money);
}

stock House_GiveMoney(houseid, money)
{
    if(!House_IsValid(houseid))
        return 0;
    House_SetMoney(houseid, House_GetMoney(houseid) + money);
    return 1;
}

stock House_SetMoney(houseid, amount)
{
    if(!House_IsValid(houseid))
        return 0;
    map_set_cell(Houses, houseid, Money, amount);
    mysql_tquery_f(gMySQL, "UPDATE `houses` SET money = '%d' WHERE id = '%d'", amount, House_GetID(houseid));
    return 1;
}

stock House_GetAddress(houseid)
{
    new zone[32], Float:x, Float:y, Float:z;
    House_GetEnterPosition(houseid, x, y, z);
    Get2DZoneName(x, y, z, zone, sizeof(zone));
    return zone;
}

stock House_LoadAll()
{
    inline OnLoad()
    {
        new count = cache_num_rows(), data[E_HOUSE_DATA];
        for(new i = 0; i < count; ++i)
        {
            cache_get_value_name_int(i, "id", data[ID]);
            cache_get_value_name_int(i, "owner_id", data[OwnerID]);

            cache_get_value_name_float(i, "enter_x", data[EnterX]);
            cache_get_value_name_float(i, "enter_y", data[EnterY]);
            cache_get_value_name_float(i, "enter_z", data[EnterZ]);
            cache_get_value_name_int(i, "enter_interior", data[EnterInterior]);
            cache_get_value_name_int(i, "enter_world", data[EnterWorld]);

            cache_get_value_name_float(i, "exit_x", data[ExitX]);
            cache_get_value_name_float(i, "exit_y", data[ExitY]);
            cache_get_value_name_float(i, "exit_z", data[ExitZ]);
            cache_get_value_name_int(i, "exit_interior", data[ExitInterior]);
            //cache_get_value_name_int(i, 12, data[ExitWorld]);

            cache_get_value_name_int(i, "price", data[Price]);
            cache_get_value_name_int(i, "locked", data[Locked]);
            cache_get_value_name_int(i, "money", data[Money]);

            cache_get_value_name_int(i, "interior_id", data[InteriorID]);
            cache_get_value_name_int(i, "interior_type", data[InteriorType]);

            cache_get_value_name_int(i, "created", data[Created]);

            data[ExitWorld] = HOUSE_START_WORLD + data[ID];
            data[Created] = 1;

            printf("House id: %d", data[ID]);
            map_add_arr(Houses, data[ID], data);
            House_CreateObjects(data[ID]);
            
            CallLocalFunction(#OnHouseLoaded, "d", data[ID]);
        }
        printf("%d houses loaded.", count);

        for_map(i : Houses)
        {
            new houseid = iter_get_key(i);
            if(House_GetOwnerID(houseid) > 0)
            {
                new Cache:cache = mysql_query_f(gMySQL, true, "SELECT name FROM `characters` WHERE id = '%d'", House_GetID(houseid));
                if(cache_num_rows() > 0)
                {
                    new name[MAX_PLAYER_NAME];
                    cache_get_value_name(0, "name", name);
                    iter_set_cells(i, OwnerName, name);
                }
                cache_delete(cache);
            }

        }
    }
    MySQL_PQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `houses` ORDER BY id");
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