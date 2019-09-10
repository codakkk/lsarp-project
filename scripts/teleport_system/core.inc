
#include <YSI_Coding\y_hooks>

static enum E_TELEPORT
{
    ID,
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
    Faction,
    Character,

    Pickup,
    ExitPickup
};

static Map:TeleportsMap;

stock Teleport_Create(Float:x, Float:y, Float:z, interior, world)
{
    inline OnInsert()
    {
        new data[E_TELEPORT];
        data[ID] = cache_insert_id();
        data[EnterX] = x;
        data[EnterY] = y;
        data[EnterZ] = z;
        data[EnterInterior] = interior;
        data[EnterWorld] = world;
        data[ExitX] = 0.0;
        data[ExitY] = 0.0;
        data[ExitZ] = 0.0;
        data[ExitInterior] = 0;
        data[ExitWorld] = 0;

        data[Faction] = INVALID_FACTION_ID;
        data[Character] = 0;

        data[Pickup] = Pickup_Create(1007, data[ID], x, y, z, ELEMENT_TYPE_TELEPORT, world, interior);
        data[ExitPickup] = Pickup_Create(1007, data[ID], data[ExitX], data[ExitY], data[ExitZ], ELEMENT_TYPE_TELEPORT, data[ExitWorld], data[ExitInterior]);

        map_add_arr(TeleportsMap, data[ID], data);
    }
    MySQL_TQueryInline(gMySQL, using inline OnInsert, "INSERT INTO teleports (enter_x, enter_y, enter_z, enter_interior, enter_world, exit_x, exit_y, exit_z, exit_interior, exit_world, faction, character) VALUES('%f', '%f', '%f', '%d', '%d', '0.0', '0.0', '0.0', '0', '0', '%d', 0)", 
    x, y, z, interior, world, INVALID_FACTION_ID);
}

stock bool:Teleport_IsValid(id)
{
    return map_has_key(TeleportsMap, id);
}

stock Teleport_GetID(teleportid)
{
    if(!Teleport_IsValid(teleportid))
        return 0;
    return map_get(TeleportsMap, teleportid, ID);
}

stock bool:Teleport_SetEnterPosition(teleportid, Float:x, Float:y, Float:z, interior, world)
{
    if(!Teleport_IsValid(teleportid))
        return false;
    if(!IsValidDynamicPickup(map_get(TeleportsMap, teleportid, Pickup)))
        return false;
    
    mysql_tquery_f(gMySQL, "UPDATE teleports SET enter_x = '%f', enter_y = '%f', enter_z = '%f', enter_world = '%d', enter_interior = '%d' WHERE id = '%d'", x, y, z, world, interior, Teleport_GetID(teleportid));
    new pickupid = Pickup_Create(1007, Teleport_GetID(teleportid), x, y, z, ELEMENT_TYPE_TELEPORT, world, interior);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), EnterX, x);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), EnterY, z);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), EnterZ, z);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), EnterInterior, interior);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), EnterWorld, world);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), Pickup, pickupid);
    return true;
}

stock Teleport_LoadAll()
{
    inline OnLoad()
    {
        new rows = cache_num_rows();
        new data[E_TELEPORT];
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", data[ID]);
            cache_get_value_name_float(i, "enter_x", data[EnterX]);
            cache_get_value_name_float(i, "enter_y", data[EnterY]);
            cache_get_value_name_float(i, "enter_z", data[EnterZ]);
            cache_get_value_name_int(i, "enter_interior", data[EnterInterior]);
            cache_get_value_name_int(i, "enter_world", data[EnterWorld]);
            cache_get_value_name_float(i, "exit_x", data[ExitX]);
            cache_get_value_name_float(i, "exit_y", data[ExitY]);
            cache_get_value_name_float(i, "exit_z", data[ExitZ]);
            cache_get_value_name_int(i, "exit_interior", data[ExitInterior]);
            cache_get_value_name_int(i, "exit_world", data[ExitWorld]);

            map_add_arr(TeleportsMap, data[ID], data);
        }

        printf("%d teleports loaded.");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM teleports");
    return 1;
}