
#include <YSI_Coding\y_hooks>

static enum _:E_TELEPORT
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

    EnterPickup,
    ExitPickup
};

static Map:TeleportsMap;

hook OnGameModeInit()
{
    TeleportsMap = map_new();
    return 1;
}

flags:teleportcreate(CMD_MODERATOR);
CMD:teleportcreate(playerid, params[])
{
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    Teleport_Create(x, y, z, GetPlayerVirtualWorld(playerid), Character_GetVirtualWorld(playerid));
    SendClientMessage(playerid, COLOR_GREEN, "Teleport creato con successo. Usa /teleportedit o /tedit per modificarlo.");
    return 1;
}
alias:teleportcreate("tcreate");

flags:gototeleport(CMD_MODERATOR);
CMD:gototeleport(playerid, params[])
{
    if(isnull(params))
        return SendClientMessage(playerid, COLOR_ERROR, "/gototeleport <teleportid>");
    new teleportid = strval(params);
    if(!Teleport_IsValid(teleportid))
        return SendClientMessage(playerid, COLOR_ERROR, "Il teleport inserito non è valido.");
    new Float:x, Float:y, Float:z, interior = Teleport_GetEnterInterior(teleportid), world = Teleport_GetEnterWorld(teleportid);

    Streamer_UpdateEx(playerid, x, y, z, world, interior);

    SetPlayerPos(playerid, x, y, z);
    SetPlayerInterior(playerid, interior);
    Character_SetVirtualWorld(playerid, world);
    return 1;
}

flags:teleportedit(CMD_MODERATOR);
CMD:teleportedit(playerid, p[])
{
    new teleportid, command[16], params[256];
    if(sscanf(p, "ds[16]S()[256]", teleportid, command, params))
    {
	    SendClientMessage(playerid, COLOR_ERROR, "/teleportedit <teleportid> <enter - exit - faction - player>");
        return 0;
    }
    
    if(!Teleport_IsValid(teleportid))
        return SendClientMessage(playerid, COLOR_ERROR, "Teleport non valido.");

    if(!strcmp(command, "enter", true))
    {
        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);
        if(Teleport_SetEnterPosition(teleportid, x, y, z, GetPlayerInterior(playerid), Character_GetVirtualWorld(teleportid)))
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato la posizione del teleport id %d.", teleportid);
        }
        else
        {
            SendFormattedMessage(playerid, COLOR_ERROR, "Non è stato possibile settare la posizione del teleport id %d.", teleportid);
        }
    }
    else if(!strcmp(command, "exit", true))
    {
        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);
        if(Teleport_SetExitPosition(teleportid, x, y, z, GetPlayerInterior(playerid), Character_GetVirtualWorld(teleportid)))
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato la posizione d'uscita del teleport id %d.", teleportid);
        }
        else
        {
            SendFormattedMessage(playerid, COLOR_ERROR, "Non è stato possibile settare la posizione d'uscita del teleport id %d.", teleportid);
        }
    }
    else if(!strcmp(command, "faction", true))
    {
        new factionid;
        if(sscanf(params, "d", factionid))
            return SendFormattedMessage(playerid, COLOR_ERROR, "/teleportedit %d faction <factionid>", teleportid);

        if(!Faction_IsValid(factionid))
            return SendClientMessage(playerid, COLOR_ERROR, "La fazione inserita non esiste.");
        
        if(Teleport_SetFaction(teleportid, factionid))
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato il teleport id %d accessibile alla fazione id %d.", teleportid, factionid);
        else
            SendFormattedMessage(playerid, COLOR_ERROR, "Non è stato possibile settare la fazione id %d al teleport id %d.", factionid, teleportid);
    }
    else if(!strcmp(command, "player", true))
    {
        new id;
        if(sscanf(params, "k<u>", id))
        {
            SendFormattedMessage(playerid, COLOR_ERROR, "/teleportedit %d player <playerid/partofname/####>", teleportid);
            SendClientMessage(playerid, COLOR_GREY, "Usa -1 come playerid per rimuovere il vincolo.");
            return 0;
        }

        if(!Character_IsLogged(id) && id != -1)
            return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");

        if(Teleport_SetCharacter(teleportid, id))
        {
            if(id == -1)
                SendFormattedMessage(playerid, COLOR_GREEN, "Hai rimosso il vincolo giocatore al teleport id %d", teleportid);
            else
                SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato il teleport id %d accessibile al giocatore %s.", teleportid, Character_GetOOCName(id));
        }
        else
            SendFormattedMessage(playerid, COLOR_ERROR, "Non è stato possibile settare il giocatore al teleport id %d.", teleportid);
    }
    return 1;
}
alias:teleportedit("tedit");

hook function Character_Enter(playerid)
{
    new elementid, E_ELEMENT_TYPE:type;
    Character_GetNearestPickup(playerid, elementid, type);
    
    new Float:range = IsPlayerInAnyVehicle(playerid) ? 7.0 : 1.5;

    if(type != ELEMENT_TYPE_TELEPORT)
    {
        new teleportid;
        new Float:x, Float:y, Float:z;
        for_map(i : TeleportsMap)
        {
            teleportid = iter_get_key(i);
            Teleport_GetEnterPosition(teleportid, x, y, z);
            if(IsPlayerInRangeOfPoint(playerid, range, x, y, z))
            {
                elementid = teleportid;
                break;
            }
        }
    }

    if(Teleport_IsValid(elementid))
    {
        new vehicleid = GetPlayerVehicleID(playerid);
        if(vehicleid > 0 && GetPlayerVehicleSeat(playerid) != 0)
            return SendClientMessage(playerid, COLOR_ERROR, "Devi essere il guidatore per poter entrare all'interno dell'edificio.");
        
        new Float:x, Float:y, Float:z;
        Teleport_GetEnterPosition(elementid, x, y, z);

        new interiorid = Teleport_GetExitInterior(elementid);
        new worldid = Teleport_GetExitWorld(elementid);
        
        if(vehicleid > 0)
        {
            LinkVehicleToInterior(vehicleid, interiorid);
            SetVehicleVirtualWorld(vehicleid, worldid);
            SetVehiclePos(vehicleid, x, y, z);
            foreach(new p : Player)
            {
                if(p == playerid || GetPlayerVehicleID(p) != vehicleid) 
                    continue;
                
                Character_SetVirtualWorld(p, worldid);
                SetPlayerInterior(p, interiorid);
            }
        }
        else
        {
            Character_SetSafePosition(playerid, x, y, z, interiorid, worldid);
        }
        SetPlayerInterior(playerid, interiorid);
        Character_SetVirtualWorld(playerid, worldid);
    }

    return continue(playerid);
}

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

        data[EnterPickup] = Pickup_Create(1007, data[ID], x, y, z, ELEMENT_TYPE_TELEPORT, world, interior);
        data[ExitPickup] = Pickup_Create(1007, data[ID], data[ExitX], data[ExitY], data[ExitZ], ELEMENT_TYPE_TELEPORT, data[ExitWorld], data[ExitInterior]);

        map_add_arr(TeleportsMap, data[ID], data);
    }
    MySQL_TQueryInline(gMySQL, using inline OnInsert, "INSERT INTO teleports (enter_x, enter_y, enter_z, enter_interior, enter_world, exit_x, exit_y, exit_z, exit_interior, exit_world, faction_id, character_id) VALUES('%f', '%f', '%f', '%d', '%d', '0.0', '0.0', '0.0', '0', '0', '%d', 0)", 
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
    if(!IsValidDynamicPickup(map_get(TeleportsMap, teleportid, EnterPickup)))
        return false;
    
    mysql_tquery_f(gMySQL, "UPDATE teleports SET enter_x = '%f', enter_y = '%f', enter_z = '%f', enter_world = '%d', enter_interior = '%d' WHERE id = '%d'", x, y, z, world, interior, Teleport_GetID(teleportid));
    new pickupid = Pickup_Create(1239, Teleport_GetID(teleportid), x, y, z, ELEMENT_TYPE_TELEPORT, world, interior);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), EnterX, x);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), EnterY, z);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), EnterZ, z);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), EnterInterior, interior);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), EnterWorld, world);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), EnterPickup, pickupid);
    return true;
}

stock bool:Teleport_GetEnterPosition(teleportid, &Float:x, &Float:y, &Float:z)
{
    if(!Teleport_IsValid(Teleport_GetID(teleportid)))
        return false;
    x = Float:map_get(TeleportsMap, Teleport_GetID(teleportid), EnterX);
    y = Float:map_get(TeleportsMap, Teleport_GetID(teleportid), EnterY);
    z = Float:map_get(TeleportsMap, Teleport_GetID(teleportid), EnterZ);
    return true;
}

stock Teleport_GetEnterInterior(teleportid)
{
    return map_get(TeleportsMap, Teleport_GetID(teleportid), EnterInterior);
}

stock Teleport_GetEnterWorld(teleportid)
{
    return map_get(TeleportsMap, Teleport_GetID(teleportid), EnterWorld);
}

stock bool:Teleport_SetExitPosition(teleportid, Float:x, Float:y, Float:z, interior, world)
{
    if(!Teleport_IsValid(teleportid))
        return false;
    
    if(!IsValidDynamicPickup(map_get(TeleportsMap, teleportid, ExitPickup)))
        return false;
    
    mysql_tquery_f(gMySQL, "UPDATE teleports SET exit_x = '%f', exit_y = '%f', exit_z = '%f', exit_world = '%d', exit_interior = '%d' WHERE id = '%d'", x, y, z, world, interior, Teleport_GetID(teleportid));
    
    new pickupid = Pickup_Create(1007, Teleport_GetID(teleportid), x, y, z, ELEMENT_TYPE_TELEPORT, world, interior);
    
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), ExitX, x);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), ExitY, z);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), ExitZ, z);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), ExitInterior, interior);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), ExitWorld, world);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), ExitPickup, pickupid);
    return true;
}

stock bool:Teleport_GetExitPosition(teleportid, &Float:x, &Float:y, &Float:z)
{
    if(!Teleport_IsValid(Teleport_GetID(teleportid)))
        return false;
    x = Float:map_get(TeleportsMap, Teleport_GetID(teleportid), ExitX);
    y = Float:map_get(TeleportsMap, Teleport_GetID(teleportid), ExitY);
    z = Float:map_get(TeleportsMap, Teleport_GetID(teleportid), ExitZ);
    return true;
}

stock Teleport_GetExitInterior(teleportid)
{
    return map_get(TeleportsMap, Teleport_GetID(teleportid), ExitInterior);
}

stock Teleport_GetExitWorld(teleportid)
{
    return map_get(TeleportsMap, Teleport_GetID(teleportid), ExitWorld);
}

stock bool:Teleport_SetFaction(teleportid, factionid)
{
    if(!Teleport_IsValid(Teleport_GetID(teleportid)) || !Faction_IsValid(factionid))
        return false;
    mysql_tquery_f(gMySQL, "UPDATE teleports SET faction_id = '%d' WHERE id = '%d'", factionid, Teleport_GetID(teleportid));
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), Faction, factionid);
    return true;
}

stock Teleport_GetFaction(teleportid)
{
    if(!Teleport_IsValid(Teleport_GetID(teleportid)))
        return INVALID_FACTION_ID;
    new factionid = map_get(TeleportsMap, Teleport_GetID(teleportid), Faction);
    if(!Faction_IsValid(factionid))
        return INVALID_FACTION_ID;
    return factionid;
}

stock bool:Teleport_SetCharacter(teleportid, playerid)
{
    if(!Teleport_IsValid(Teleport_GetID(teleportid)))
        return false;
    new id = 0;
    if(playerid != -1 && Character_IsLogged(playerid))
        id = Character_GetID(playerid);
    mysql_tquery_f(gMySQL, "UPDATE teleports SET character_id = '%d' WHERE id = '%d'", id, Teleport_GetID(teleportid));
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), Character, id);
    return true;
}

stock Teleport_GetCharacter(teleportid)
{
    if(!Teleport_IsValid(Teleport_GetID(teleportid)))
        return 0;
    return map_get(TeleportsMap, Teleport_GetID(teleportid), Character);
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

            data[EnterPickup] = Pickup_Create(1007, data[ID], data[EnterX], data[EnterY], data[EnterZ], ELEMENT_TYPE_TELEPORT, data[EnterWorld], data[EnterInterior]);
            data[ExitPickup] = Pickup_Create(1007, data[ID], data[ExitX], data[ExitY], data[ExitZ], ELEMENT_TYPE_TELEPORT, data[ExitWorld], data[ExitInterior]);

            map_add_arr(TeleportsMap, data[ID], data);
        }

        printf("%d teleports loaded.");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM teleports");
    return 1;
}