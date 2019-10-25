
#include <YSI_Coding\y_hooks>

static enum _:E_TELEPORT
{
    ID,
    Name[32],

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
    AllowVehicles,

    EnterPickup,
    Text3D:EnterLabel,

    ExitPickup,
    Text3D:ExitLabel,
};

static Map:TeleportsMap;

hook OnGameModeInit()
{
    TeleportsMap = map_new();

    Teleport_LoadAll();
    return 1;
}

hook OnPlayerPickUpElmPickup(playerid, pickupid, elementId, E_ELEMENT_TYPE:type)
{
    if(type != ELEMENT_TYPE_TELEPORT)
	   return 1;
    new 
		String:str;
    
	if(Account_GetAdminLevel(playerid) > 1 && Player_IsAdminDuty(playerid))
    {
	   str += str_format("~r~Teleport~n~~b~ID: %d~w~~n~", elementId);
    }
    new name[32];
	Teleport_GetName(elementId, name);
	str += str_format("~g~%s~w~~n~", name);
	
	GameTextForPlayerStr(playerid, str, 3000, 3);
    return 1;
}

stock bool:Teleport_AllowVehicles(teleportid, bool:allow)
{
    if(!Teleport_IsValid(teleportid))
        return false;
    mysql_tquery_f(gMySQL, "UPDATE `teleports` SET allow_vehicles = '%d' WHERE id = '%d'", allow ? 1 : 0, Teleport_GetID(teleportid));
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), AllowVehicles, allow);
    return true;
}

stock bool:Teleport_DoesAllowVehicles(teleportid)
{
    if(!Teleport_IsValid(teleportid))
        return false;
    return map_get(TeleportsMap, Teleport_GetID(teleportid), AllowVehicles) > 0 ? true : false;
}

flags:teleportcreate(CMD_MODERATOR);
CMD:teleportcreate(playerid, params[])
{
    new name[32];
    if(sscanf(params, "s[32]", name))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /teleportcreate <name>");
    
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    Teleport_Create(name, x, y, z, GetPlayerInterior(playerid), Character_GetVirtualWorld(playerid));
    SendFormattedMessage(playerid, COLOR_GREEN, "Teleport \"%s\" creato con successo. Usa /teleportedit o /tedit per modificarlo.", name);
    return 1;
}
alias:teleportcreate("tcreate");

flags:gototeleport(CMD_MODERATOR);
CMD:gototeleport(playerid, params[])
{
    if(isnull(params))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /gototeleport <teleportid>");
    new teleportid = strval(params);
    if(!Teleport_IsValid(teleportid))
        return SendClientMessage(playerid, COLOR_ERROR, "Il teleport inserito non è valido.");
    new Float:x, Float:y, Float:z, 
        interior = Teleport_GetEnterInterior(teleportid), 
        world = Teleport_GetEnterWorld(teleportid);
    
    Teleport_GetEnterPosition(teleportid, x, y, z);
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
	    SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /teleportedit <teleportid> <name - enter - exit - faction - player - delete - allowvehicles>");
        return 0;
    }
    
    if(!Teleport_IsValid(teleportid))
        return SendClientMessage(playerid, COLOR_ERROR, "Teleport non valido.");
    if(!strcmp(command, "name", true))
    {
        new newName[32];
        if(sscanf(params, "s[32]", newName))
            return SendFormattedMessage(playerid, COLOR_COMMAND_ERROR, "USO: /teleportedit %d name <name (max 32 chars.)", teleportid);
        
        if(Teleport_SetName(teleportid, newName))
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato il nome del teleport id %d. Nuovo nome: %s", teleportid, newName);
        }
        else
        {
            SendFormattedMessage(playerid, COLOR_ERROR, "Non è stato possibile settare il nome del teleport id %d.", teleportid);
        }
    }
    else if(!strcmp(command, "enter", true))
    {
        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);
        if(Teleport_SetEnterPosition(teleportid, x, y, z, GetPlayerInterior(playerid), Character_GetVirtualWorld(playerid)))
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
        if(Teleport_SetExitPosition(teleportid, x, y, z, GetPlayerInterior(playerid), Character_GetVirtualWorld(playerid)))
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
        {
            SendFormattedMessage(playerid, COLOR_COMMAND_ERROR, "USO: /teleportedit %d faction <factionid>", teleportid);
            SendFormattedMessage(playerid, COLOR_GREY, "Usa %d come factionid per rimuovere il vincolo.", INVALID_FACTION_ID);
            return 0;
        }

        if(factionid != INVALID_FACTION_ID && !Faction_IsValid(factionid))
            return SendClientMessage(playerid, COLOR_ERROR, "La fazione inserita non esiste.");
        
        if(Teleport_SetFaction(teleportid, factionid))
        {
            if(factionid == INVALID_FACTION_ID)
            {
                SendFormattedMessage(playerid, COLOR_GREEN, "Hai rimosso il vincolo di fazione al teleport id %d.", teleportid);
            }
            else
            {
                SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato il teleport id %d accessibile alla fazione id %d.", teleportid, factionid);
            }
        }
        else
            SendFormattedMessage(playerid, COLOR_ERROR, "Non è stato possibile settare la fazione id %d al teleport id %d.", factionid, teleportid);
    }
    else if(!strcmp(command, "player", true))
    {
        new id;
        if(sscanf(params, "k<u>", id))
        {
            SendFormattedMessage(playerid, COLOR_COMMAND_ERROR, "USO: /teleportedit %d player <playerid/partofname/####>", teleportid);
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
    else if(!strcmp(command, "delete", true))
    {   
        if(Teleport_Delete(teleportid))
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai deletato il teleport id %d.", teleportid);
        }
        else
        {
            SendFormattedMessage(playerid, COLOR_ERROR, "Non è stato possibile deletare il teleport id %d.", teleportid);
        }
    }
    else if(!strcmp(command, "allowvehicles", true))
    {
        new allow;
        if(sscanf(params, "d", allow))
            return SendFormattedMessage(playerid, COLOR_COMMAND_ERROR, "USO: /teleportedit %d allowvehicles <1: allow - 0: don't allow>", teleportid);
        
        if(Teleport_AllowVehicles(teleportid, allow ? true : false))
        {
            if(allow)
                SendFormattedMessage(playerid, COLOR_GREEN, "Adesso è possibile entrare nel teleport id %d con i veicoli.", teleportid);
            else
                SendFormattedMessage(playerid, COLOR_GREEN, "Non è più possibile entrare nel teleport id %d con i veicoli.", teleportid);
        }
        else
        {
            SendFormattedMessage(playerid, COLOR_ERROR, "Non è stato possibile modificare l'allow vehicles teleport id %d.", teleportid);
        }
    }
    return 1;
}
alias:teleportedit("tedit");

stock bool:GetTeleportRightPosByPlayer(playerid, teleportid, &Float:finalX, &Float:finalY, &Float:finalZ, &finalInterior, &finalWorld)
{
    static Float:x, Float:y, Float:z, Float:ex, Float:ey, Float:ez;
    new Float:range = IsPlayerInAnyVehicle(playerid) ? 4.0 : 1.5;
    Teleport_GetEnterPosition(teleportid, x, y, z);
    Teleport_GetExitPosition(teleportid, ex, ey, ez);
    if(IsPlayerInRangeOfPoint(playerid, range, x, y, z) && Character_GetVirtualWorld(playerid) == Teleport_GetEnterWorld(teleportid))
    {
        finalX = ex;
        finalY = ey;
        finalZ = ez;
        finalInterior = Teleport_GetExitInterior(teleportid);
        finalWorld = Teleport_GetExitWorld(teleportid);
        return true;
    }
    if(IsPlayerInRangeOfPoint(playerid, range, ex, ey, ez) && Character_GetVirtualWorld(playerid) == Teleport_GetExitWorld(teleportid))
    {
        finalX = x;
        finalY = y;
        finalZ = z;
        finalInterior = Teleport_GetEnterInterior(teleportid);
        finalWorld = Teleport_GetEnterWorld(teleportid);
        return true;
    }
    return false;
}

hook function Character_Enter(playerid)
{
    printf("[Character_Enter]: Teleport");
    
    new Float:finalX, Float:finalY, Float:finalZ, finalInterior, finalWorld;

    new teleport = Character_GetCurrentTeleport(playerid);

    if(Teleport_IsValid(teleport))
    {
        GetTeleportRightPosByPlayer(playerid, teleport, finalX, finalY, finalZ, finalInterior, finalWorld);
    }
    else if(!Teleport_IsValid(teleport) && IsPlayerInAnyVehicle(playerid))
    {
        new tempTeleport = 0;
        for_map(i : TeleportsMap)
        {
            tempTeleport = iter_get_key(i);
            if(GetTeleportRightPosByPlayer(playerid, teleport, finalX, finalY, finalZ, finalInterior, finalWorld))
            {
                teleport = tempTeleport;
                break;
            }
        }
    }

    if(Teleport_IsValid(teleport))
    {
        new vehicleid = GetPlayerVehicleID(playerid);
        if(vehicleid > 0)
        {
            if(GetPlayerVehicleSeat(playerid) != 0)
                return SendClientMessage(playerid, COLOR_ERROR, "Devi essere il guidatore per poter entrare all'interno dell'edificio.");
            
            if(!Teleport_DoesAllowVehicles(teleport))
                return SendClientMessage(playerid, COLOR_ERROR, "Non puoi entrare con un veicolo.");
        }
        
        if(!Player_IsAdminDuty(playerid))
        {
            new tfactionid = Teleport_GetFaction(teleport);
            if(Faction_IsValid(tfactionid) || Teleport_GetCharacter(teleport) != 0)
            {
                if(tfactionid != Character_GetFaction(playerid) && Teleport_GetCharacter(teleport) != Character_GetID(playerid))
                    return SendClientMessage(playerid, COLOR_ERROR, "Non puoi entrare.");
            }
        }

        if(finalX == 0.0 && finalY == 0.0 && finalZ == 0.0)
            return continue(playerid);

        if(vehicleid > 0)
        {
            foreach(new p : Player)
            {
                if(!Character_IsLogged(p) || GetPlayerVehicleID(p) != vehicleid) 
                    continue;
                
                defer SetPlayerInSeatAfterTime(p, GetPlayerVehicleID(p), GetPlayerVehicleSeat(p));
                Character_SetSafePosition(p, finalX, finalY, finalZ, finalInterior, finalWorld);
            }
            
            defer SetVehiclePosDyn(vehicleid, finalX, finalY, finalZ, finalInterior, finalWorld);
        }
        else
        {
            printf("Teleport Set Pos: %f - %f - %f", finalX, finalY, finalZ);
            Character_SetSafePosition(playerid, finalX, finalY, finalZ, finalInterior, finalWorld);
        }
        return 1;
    }

    return continue(playerid);
}

stock Character_GetCurrentTeleport(playerid)
{
    new teleportid = 0, elementid, E_ELEMENT_TYPE:type;
    if(Character_GetNearestPickup(playerid, elementid, type) && type == ELEMENT_TYPE_TELEPORT)
    {
        teleportid = elementid;
    }
    
    return teleportid;
}

stock Teleport_Create(const name[], Float:x, Float:y, Float:z, interior, world)
{
    new data[E_TELEPORT];
    set(data[Name], name);
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
    inline OnInsert()
    {
        data[ID] = cache_insert_id();

        map_add_arr(TeleportsMap, data[ID], data);

        Teleport_CreatePickupAndLabel(data[ID]);
    }
    MySQL_TQueryInline(gMySQL, using inline OnInsert, "INSERT INTO teleports (name, enter_x, enter_y, enter_z, enter_interior, enter_world, exit_x, exit_y, exit_z, exit_interior, exit_world, faction_id, character_id) VALUES('%e', '%f', '%f', '%f', '%d', '%d', '0.0', '0.0', '0.0', '0', '0', '%d', 0)", 
    name, x, y, z, interior, world, INVALID_FACTION_ID);
}

stock bool:Teleport_Delete(teleportid)
{
    if(!Teleport_IsValid(teleportid))
        return false;
    new pickupid, Text3D:text;

    pickupid = map_get(TeleportsMap, Teleport_GetID(teleportid), EnterPickup);
    Pickup_Destroy(pickupid);

    pickupid = map_get(TeleportsMap, Teleport_GetID(teleportid), ExitPickup);
    Pickup_Destroy(pickupid);

    text = Text3D:map_get(TeleportsMap, Teleport_GetID(teleportid), EnterLabel);
    DestroyDynamic3DTextLabel(text);

    text = Text3D:map_get(TeleportsMap, Teleport_GetID(teleportid), ExitLabel);
    DestroyDynamic3DTextLabel(text);

    mysql_tquery_f(gMySQL, "DELETE FROM `teleports` WHERE id = '%d'", Teleport_GetID(teleportid));

    map_remove_deep(TeleportsMap, Teleport_GetID(teleportid));
    return true;       
}

static stock bool:Teleport_CreatePickupAndLabel(teleportid)
{
    if(!Teleport_IsValid(Teleport_GetID(teleportid)))
        return false;
    new Text3D:text, pickupid;

    pickupid = map_get(TeleportsMap, Teleport_GetID(teleportid), EnterPickup);
    if(IsValidDynamicPickup(pickupid))
        Pickup_Destroy(pickupid);

    text = Text3D:map_get(TeleportsMap, Teleport_GetID(teleportid), EnterLabel);
    if(IsValidDynamic3DTextLabel(text))
        DestroyDynamic3DTextLabel(text);

    pickupid = map_get(TeleportsMap, Teleport_GetID(teleportid), ExitPickup);
    if(IsValidDynamicPickup(pickupid))
        Pickup_Destroy(pickupid);

    text = Text3D:map_get(TeleportsMap, Teleport_GetID(teleportid), ExitLabel);
    if(IsValidDynamic3DTextLabel(text))
        DestroyDynamic3DTextLabel(text);
    
    new 
        Float:x, Float:y, Float:z, 
        world = Teleport_GetEnterWorld(teleportid),
        interior = Teleport_GetEnterInterior(teleportid),
        name[32];

    Teleport_GetName(teleportid, name);

    Teleport_GetEnterPosition(teleportid, x, y, z);
    pickupid = Pickup_Create(1239, Teleport_GetID(teleportid), x, y, z, ELEMENT_TYPE_TELEPORT, world, interior);
    text = CreateDynamic3DTextLabel(name, COLOR_GREY, x, y, z + 0.55, 10.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, world, interior, -1, STREAMER_3D_TEXT_LABEL_SD);

    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), EnterPickup, pickupid);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), EnterLabel, text);

    Teleport_GetExitPosition(teleportid, x, y, z);
    world = Teleport_GetExitWorld(teleportid);
    interior = Teleport_GetExitInterior(teleportid);

    pickupid = Pickup_Create(1239, Teleport_GetID(teleportid), x, y, z, ELEMENT_TYPE_TELEPORT, world, interior);
    text = CreateDynamic3DTextLabel(name, COLOR_GREY, x, y, z + 0.55, 10.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, world, interior, -1, STREAMER_3D_TEXT_LABEL_SD);
    
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), ExitPickup, pickupid);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), ExitLabel, text);

    return true;
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
    mysql_tquery_f(gMySQL, "UPDATE teleports SET enter_x = '%f', enter_y = '%f', enter_z = '%f', enter_world = '%d', enter_interior = '%d' WHERE id = '%d'", x, y, z, world, interior, Teleport_GetID(teleportid));
    
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), EnterX, x);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), EnterY, y);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), EnterZ, z);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), EnterInterior, interior);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), EnterWorld, world);
    
    Teleport_CreatePickupAndLabel(Teleport_GetID(teleportid));
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

    mysql_tquery_f(gMySQL, "UPDATE teleports SET exit_x = '%f', exit_y = '%f', exit_z = '%f', exit_world = '%d', exit_interior = '%d' WHERE id = '%d'", x, y, z, world, interior, Teleport_GetID(teleportid));
    
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), ExitX, x);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), ExitY, y);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), ExitZ, z);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), ExitInterior, interior);
    map_set_cell(TeleportsMap, Teleport_GetID(teleportid), ExitWorld, world);

    Teleport_CreatePickupAndLabel(Teleport_GetID(teleportid));
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
    if(!Teleport_IsValid(Teleport_GetID(teleportid)) || (factionid != INVALID_FACTION_ID && !Faction_IsValid(factionid)))
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

stock bool:Teleport_GetName(teleportid, name[])
{
    if(!Teleport_IsValid(teleportid))
        return false;
    new Iter:iter = map_iter_at(TeleportsMap, Teleport_GetID(teleportid));
    iter_get_md_arr(iter, {Name}, name, 32);
    return true;
}

stock bool:Teleport_SetName(teleportid, name[])
{
    if(!Teleport_IsValid(teleportid))
        return false;
    new Iter:iter = Iter:map_iter_at(TeleportsMap, Teleport_GetID(teleportid));
	iter_set_cells(iter, Name, name, 32);

    mysql_tquery_f(gMySQL, "UPDATE teleports SET name = '%e' WHERE id = '%d'", name, Teleport_GetID(teleportid));

    Teleport_CreatePickupAndLabel(Teleport_GetID(teleportid));
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
            cache_get_value_name(i, "name", data[Name]);
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
            
            cache_get_value_name_int(i, "faction_id", data[Faction]);
            cache_get_value_name_int(i, "character_id", data[Character]);
            cache_get_value_name_int(i, "allow_vehicles", data[AllowVehicles]);

            map_add_arr(TeleportsMap, data[ID], data);

            Teleport_CreatePickupAndLabel(data[ID]);
        }

        printf("%d teleports loaded.", rows);
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM teleports");
    return 1;
}