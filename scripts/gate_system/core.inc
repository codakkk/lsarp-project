
#include <YSI_Coding\y_hooks>

#define MAX_GATES       50

static enum _:E_GATE_INFO
{
    ID,
    Float:StartX,
    Float:StartY,
    Float:StartZ,
    Float:StartRotX,
    Float:StartRotY,
    Float:StartRotZ,
    Float:EndX,
    Float:EndY,
    Float:EndZ,
    Float:EndRotX,
    Float:EndRotY,
    Float:EndRotZ,
    FactionID,
    OwnerID,
    ObjectID
};

static 
    Map:Gates,
    bool:GateState[MAX_GATES char] = {false, ...};

stock Map:GetGatesMap()
{
    return Gates;
}

hook OnGameModeInit()
{
    Gates = map_new();
    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS gates \
						(`id` INT(11) NOT NULL AUTO_INCREMENT, \
                        `model_id` INT(11) DEFAULT '0', \
						`start_x` FLOAT DEFAULT '0.0', \
                        `start_y` FLOAT DEFAULT '0.0', \
                        `start_z` FLOAT DEFAULT '0.0', \
                        `start_rot_x` FLOAT DEFAULT '0.0', \
                        `start_rot_y` FLOAT DEFAULT '0.0', \
                        `start_rot_z` FLOAT DEFAULT '0.0', \
                        `end_x` FLOAT DEFAULT '0.0', \
                        `end_y` FLOAT DEFAULT '0.0', \
                        `end_z` FLOAT DEFAULT '0.0', \
                        `end_rot_x` FLOAT DEFAULT '0.0', \
                        `end_rot_y` FLOAT DEFAULT '0.0', \
                        `end_rot_z` FLOAT DEFAULT '0.0', \
                        `faction_id` INT(11) DEFAULT '-1', \
                        `owner_id` INT(11) DEFAULT '0', \
						PRIMARY KEY (`id`)) ENGINE=InnoDB DEFAULT CHARSET=latin1;", false);
    Gate_LoadAll();
    return 1;
}

hook OnDynamicObjectMoved(objectid)
{
    new gateid = Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID);
    if(!Gate_IsValid(gateid) || Gate_GetObjectID(gateid) != objectid)
        return 1;
    defer CloseGateTimer(gateid);
    return 1;
}

timer CloseGateTimer[9900 + gateid](gateid)
{
    Gate_Close(gateid);
}

hook OnPlayerEditDynObject(playerid, objectid, response, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz)
{
    new type = EditObject_GetType(playerid);
	if(type == EDIT_TYPE_GATE_START || type == EDIT_TYPE_GATE_END)
	{
        new gateid = EditObject_GetExtraID(playerid);
        if(response == EDIT_RESPONSE_CANCEL)
		{
			new Float:tx, Float:ty, Float:tz,
                Float:trx, Float:try, Float:trz;
            if(type == EDIT_TYPE_GATE_START)
            {
			    Gate_GetStartPos(gateid, tx, ty, tz);
			    Gate_GetStartRot(gateid, trx, try, trz);
            }
            else if(type == EDIT_TYPE_GATE_END)
            {
                Gate_GetEndPos(gateid, tx, ty, tz);
			    Gate_GetEndRot(gateid, trx, try, trz);
            }

			SetDynamicObjectPos(objectid, tx, ty, tz);
			SetDynamicObjectRot(objectid, trx, try, trz);
            SendFormattedMessage(playerid, -1, "Hai annullato le modifiche per il gateid %d.", gateid);
			EditObject_End(playerid);
		}
		else if(response == EDIT_RESPONSE_FINAL)
		{
			if(type == EDIT_TYPE_GATE_START)
            {
			    Gate_SetStartPos(gateid, x, y, z);
                Gate_SetStartRot(gateid, rx, ry, rz);
                SendFormattedMessage(playerid, -1, "Hai settato la posizione di inizio per il gateid %d.", gateid);
            }
            else if(type == EDIT_TYPE_GATE_END)
            {
                Gate_SetEndPos(gateid, x, y, z);
                Gate_SetEndRot(gateid, rx, ry, rz);

                // Reset Pos
                new Float:tx, Float:ty, Float:tz,
                    Float:trx, Float:try, Float:trz;
                Gate_GetStartPos(gateid, tx, ty, tz);
			    Gate_GetStartRot(gateid, trx, try, trz);
                SetDynamicObjectPos(objectid, tx, ty, tz);
                SetDynamicObjectRot(objectid, trx, try, trz);

                SendFormattedMessage(playerid, -1, "Hai settato la posizione di arrivo per il gateid %d.", gateid);
            }
            Log(Character_GetOOCName(playerid), "", "Edit Gate", gateid);

			EditObject_End(playerid);
		}
    }
    return 1;
}

stock Gate_Create(Float:x, Float:y, Float:z)
{
    
    new Cache:result = mysql_query_f(gMySQL, true, "INSERT INTO `gates` (model_id, faction_id, owner_id) VALUES(19912, '%d', '0')", INVALID_FACTION_ID);

    new 
        id = cache_insert_id(),
        data[E_GATE_INFO]
    ;

    data[ID] = id;
    data[StartX] = x;
    data[StartY] = y;
    data[StartZ] = z;
    data[StartRotX] = 0.0;
    data[StartRotY] = 0.0;
    data[StartRotZ] = 0.0;
    data[EndX] = 0.0;
    data[EndY] = 0.0;
    data[EndZ] = 0.0;
    data[EndRotX] = 0.0;
    data[EndRotY] = 0.0;
    data[EndRotZ] = 0.0;
    data[FactionID] = INVALID_FACTION_ID;
    data[OwnerID] = 0;
    
    GateState{id} = false;
    data[ObjectID] = CreateDynamicObject(19912, x, y, z, 0, 0, 0, -1, -1, -1, STREAMER_OBJECT_SD, STREAMER_OBJECT_DD);
    Streamer_SetIntData(STREAMER_TYPE_OBJECT, data[ObjectID], E_STREAMER_EXTRA_ID, id);
    
    map_add_arr(Gates, id, data);

    cache_delete(result);
    return id;
}

stock Gate_Delete(gateid)
{
    if(!Gate_IsValid(gateid))
        return 0;
    new query[64];
    mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `gates` WHERE id = '%d'", gateid);
    mysql_tquery(gMySQL, query);

    DestroyDynamicObject(map_get(Gates, gateid, ObjectID));
    map_remove_deep(Gates, gateid);
    return 1;
}

stock Gate_LoadAll()
{
    inline OnLoad()
    {
        new 
            rows = cache_num_rows(),
            data[E_GATE_INFO];
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_index_int(i, 0, data[ID]);

            cache_get_value_index_float(i, 2, data[StartX]);
            cache_get_value_index_float(i, 3, data[StartY]);
            cache_get_value_index_float(i, 4, data[StartZ]);

            cache_get_value_index_float(i, 5, data[StartRotX]);
            cache_get_value_index_float(i, 6, data[StartRotY]);
            cache_get_value_index_float(i, 7, data[StartRotZ]);

            cache_get_value_index_float(i, 8, data[EndX]);
            cache_get_value_index_float(i, 9, data[EndY]);
            cache_get_value_index_float(i, 10, data[EndZ]);

            cache_get_value_index_float(i, 11, data[EndRotX]);
            cache_get_value_index_float(i, 12, data[EndRotY]);
            cache_get_value_index_float(i, 13, data[EndRotZ]);

            cache_get_value_index_int(i, 14, data[FactionID]);

            cache_get_value_index_int(i, 15, data[OwnerID]);

            data[ObjectID] = CreateDynamicObject(19912, data[StartX], data[StartY], data[StartZ], data[StartRotX], data[StartRotY], data[StartRotZ], -1, -1, -1, STREAMER_OBJECT_SD, STREAMER_OBJECT_DD);

            GateState{i} = false;
            
            Streamer_SetIntData(STREAMER_TYPE_OBJECT, data[ObjectID], E_STREAMER_EXTRA_ID, data[ID]);
            
            map_add_arr(Gates, data[ID], data);
        }
        printf("%d gates loaded.", rows);
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `gates` ORDER BY id DESC");
}

stock Gate_Open(gateid)
{
    if(!Gate_IsValid(gateid) || GateState{gateid} == true)
        return 0;
    new Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz;
    Gate_GetEndPos(gateid, x, y, z);
    Gate_GetEndRot(gateid, rx, ry, rz);
    MoveDynamicObject(Gate_GetObjectID(gateid), x, y, z, 5.0, rx, ry, rz);
    GateState{gateid} = true;
    return 1;
}

stock Gate_IsOpened(gateid)
{
    return GateState{gateid} == true;
}

stock Gate_Close(gateid)
{
    if(!Gate_IsValid(gateid) || GateState{gateid} == false)
        return 0;
    new Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz;
    Gate_GetStartPos(gateid, x, y, z);
    Gate_GetStartRot(gateid, rx, ry, rz);
    MoveDynamicObject(Gate_GetObjectID(gateid), x, y, z, 5.0, rx, ry, rz);
    GateState{gateid} = false;
    return 1;
}

stock Gate_IsClosed(gateid)
{
    return GateState{gateid} == false;
}

/*          ACCESSORS           */

stock Gate_IsValid(gateid)
{
    return map_has_key(Gates, gateid);
}

stock Gate_GetID(gateid)
{
    if(!Gate_IsValid(gateid))
        return 0;
    return map_get(Gates, gateid, ID);
}

stock Gate_SetFaction(gateid, factionid)
{
    if(!Gate_IsValid(gateid))
        return 0;
    map_set_cell(Gates, gateid, FactionID, Faction_GetID(factionid));

    mysql_tquery_f(gMySQL, "UPDATE `gates` SET faction_id = '%d' WHERE id = '%d'", Faction_GetID(factionid), Gate_GetID(gateid));
    return 1;
}

stock Gate_GetFaction(gateid)
{
    if(!Gate_IsValid(gateid))
        return 0;
    return map_get(Gates, gateid, FactionID);
}

stock Gate_SetOwner(gateid, playerid)
{
    if(!Gate_IsValid(gateid))
        return 0;
    map_set_cell(Gates, gateid, OwnerID, Character_GetID(playerid));

    mysql_tquery_f(gMySQL, "UPDATE `gates` SET owner_id = '%d' WHERE id = '%d'", Character_GetID(playerid), Gate_GetID(gateid));
    return 1;
}

stock Gate_GetOwner(gateid)
{
    if(!Gate_IsValid(gateid))
        return 0;
    return map_get(Gates, gateid, OwnerID);
}

stock Gate_ResetOwner(gateid)
{
    if(!Gate_IsValid(gateid))
        return 0;
    map_set_cell(Gates, gateid, OwnerID, 0);

    mysql_tquery_f(gMySQL, "UPDATE `gates` SET owner_id = '0' WHERE id = '%d'", Gate_GetID(gateid));
    return 1;
}

stock Gate_SetStartPos(gateid, Float:x, Float:y, Float:z)
{
    if(!Gate_IsValid(gateid))
        return 0;
    map_set_cell(Gates, gateid, StartX, x);
    map_set_cell(Gates, gateid, StartY, y);
    map_set_cell(Gates, gateid, StartZ, z);

    mysql_tquery_f(gMySQL, "UPDATE `gates` SET start_x = '%f', start_y = '%f', start_z = '%f' WHERE id = '%d'", 
                            x, y, z, Gate_GetID(gateid));
    return 1;
}

stock Gate_GetStartPos(gateid, &Float:x, &Float:y, &Float:z)
{
    if(!Gate_IsValid(gateid))
        return 0;
    x = Float:map_get(Gates, gateid, StartX);
    y = Float:map_get(Gates, gateid, StartY);
    z = Float:map_get(Gates, gateid, StartZ);
    return 1;
}

stock Gate_SetStartRot(gateid, Float:x, Float:y, Float:z)
{
    if(!Gate_IsValid(gateid))
        return 0;
    map_set_cell(Gates, gateid, StartRotX, x);
    map_set_cell(Gates, gateid, StartRotY, y);
    map_set_cell(Gates, gateid, StartRotZ, z);

    mysql_tquery_f(gMySQL, "UPDATE `gates` SET start_rot_x = '%f', start_rot_y = '%f', start_rot_z = '%f' WHERE id = '%d'", 
                            x, y, z, Gate_GetID(gateid));
    return 1;
}

stock Gate_GetStartRot(gateid, &Float:x, &Float:y, &Float:z)
{
    if(!Gate_IsValid(gateid))
        return 0;
    x = Float:map_get(Gates, gateid, StartRotX);
    y = Float:map_get(Gates, gateid, StartRotY);
    z = Float:map_get(Gates, gateid, StartRotZ);
    return 1;
}

stock Gate_SetEndPos(gateid, Float:x, Float:y, Float:z)
{
    if(!Gate_IsValid(gateid))
        return 0;
    map_set_cell(Gates, gateid, EndX, x);
    map_set_cell(Gates, gateid, EndY, y);
    map_set_cell(Gates, gateid, EndZ, z);

    mysql_tquery_f(gMySQL, "UPDATE `gates` SET end_x = '%f', end_y = '%f', end_z = '%f' WHERE id = '%d'", 
                            x, y, z, Gate_GetID(gateid));
    return 1;
}

stock Gate_GetEndPos(gateid, &Float:x, &Float:y, &Float:z)
{
    if(!Gate_IsValid(gateid))
        return 0;
    x = Float:map_get(Gates, gateid, EndX);
    y = Float:map_get(Gates, gateid, EndY);
    z = Float:map_get(Gates, gateid, EndZ);
    return 1;
}

stock Gate_SetEndRot(gateid, Float:x, Float:y, Float:z)
{
    if(!Gate_IsValid(gateid))
        return 0;
    map_set_cell(Gates, gateid, EndRotX, x);
    map_set_cell(Gates, gateid, EndRotY, y);
    map_set_cell(Gates, gateid, EndRotZ, z);

    mysql_tquery_f(gMySQL, "UPDATE `gates` SET end_rot_x = '%f', end_rot_y = '%f', end_rot_z = '%f' WHERE id = '%d'", 
                            x, y, z, Gate_GetID(gateid));
    return 1;
}

stock Gate_GetEndRot(gateid, &Float:x, &Float:y, &Float:z)
{
    if(!Gate_IsValid(gateid))
        return 0;   
    x = Float:map_get(Gates, gateid, EndRotX);
    y = Float:map_get(Gates, gateid, EndRotY);
    z = Float:map_get(Gates, gateid, EndRotZ);

    return 1;
}

stock Gate_GetObjectID(gateid)
{
    if(!Gate_IsValid(gateid))
        return 0;
    return map_get(Gates, gateid, ObjectID);
}

// ======================================================= COMMANDS =====================================================================

flags:cancello(CMD_ALIVE_USER);
CMD:cancello(playerid, params[])
{
    new Float:x, Float:y, Float:z, gateid;
    for_map(i : Gates)
    {
        gateid = iter_get_key(i);
        if(Gate_IsOpened(gateid))
            continue;
        Gate_GetStartPos(gateid, x, y, z);
        if(GetPlayerDistanceFromPoint(playerid, x, y, z) > 15.0)
            continue;
        if(Gate_GetOwner(gateid) == Character_GetID(playerid) || Gate_GetFaction(gateid) == Faction_GetID(Character_GetFaction(playerid)))
        {
            Gate_Open(gateid);
            Player_Info(playerid, "Cancello aperto.~n~Chiusura in ~r~10~w~ secondi.", true, 3500);
        }
    }    
    return 1;
}

flags:gcreate(CMD_ADMIN);
CMD:gcreate(playerid, params[])
{
    if(EditObject_IsEditing(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se stai editando un oggetto.");
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    new gateid = Gate_Create(x, y, z);
    if(gateid != -1)
        SendFormattedMessage(playerid, -1, "Cancello id %d creato con successo. Usa /gedit %d per modificarlo.", gateid, gateid);
    else
        SendClientMessage(playerid, COLOR_ERROR, "Non è stato possibile creare il cancello. Contatta il mast Coda.");
    return 1;
}

flags:gdelete(CMD_ADMIN);
CMD:gdelete(playerid, params[])
{
    new val = strval(params);
    if(val <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "/gdelete <garage id>");
    if(Gate_Delete(val))
	{
		SendMessageToAdmins(true, COLOR_ORANGE, "AdmCmd: %s (%d) ha rimosso il cancello id %d.", Account_GetName(playerid), playerid, val);
	}
	else
	{
		SendClientMessage(playerid, COLOR_ERROR, "Il cancello non esiste o non puo' essere cancellato.");
	}
    return 1;
}

flags:gotogate(CMD_ADMIN);
CMD:gotogate(playerid, params[])
{
    new val = strval(params);
    if(val <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "/gotogate <garage id>");
    
    new Float:x, Float:y, Float:z;
    if(Gate_GetStartPos(val, x, y, z))
        SetPlayerPos(playerid, x, y, z);
    else
        SendClientMessage(playerid, COLOR_ERROR, "Il cancello inserito non esiste.");
    return 1;
}

flags:gedit(CMD_ADMIN);
CMD:gedit(playerid, params[])
{
    new gateid, command[16], otherData[32];
    if(sscanf(params, "ds[16]S(0)[32]", gateid, command, otherData))
        return SendClientMessage(playerid, COLOR_ERROR, "/gedit <gateid> <startpos - endpos - faction - owner - rmowner>");
    
    if(!Gate_IsValid(gateid))
        return SendClientMessage(playerid, COLOR_ERROR, "Il gateid inserito non è valido.");
    
    if(!strcmp(command, "startpos", true))
    {
        EditObject_Start(playerid, EDIT_TYPE_GATE_START, Gate_GetObjectID(gateid), gateid);
    }
    else if(!strcmp(command, "endpos", true))
    {
        EditObject_Start(playerid, EDIT_TYPE_GATE_END, Gate_GetObjectID(gateid), gateid);
    }
    else if(!strcmp(command, "faction", true))
    {
        new factionid;
        if(sscanf(otherData, "d", factionid))
            return SendFormattedMessage(playerid, COLOR_ERROR, "/gedit %d faction <factionid>", gateid);
        
        if(factionid == INVALID_FACTION_ID)
        {
            Gate_SetFaction(gateid, INVALID_FACTION_ID);
            return SendFormattedMessage(playerid, -1, "Hai rimosso il cancello alla fazione precedente (Gate id: %d).", gateid);
        }
        if(!Faction_IsValid(factionid))
            return SendClientMessage(playerid, COLOR_ERROR, "La fazione inserita non esiste.");
        Gate_SetFaction(gateid, factionid);

        SendFormattedMessage(playerid, -1, "Hai settato il gate id %d alla fazione %d.", gateid, factionid);
    }
    else if(!strcmp(command, "owner", true))
    {
        new id;
        if(sscanf(otherData, "k<u>", id))
            return SendFormattedMessage(playerid, COLOR_ERROR, "/gedit %d owner <playerid/partofname/#code>", gateid);
        if(!IsPlayerConnected(id) || !Character_IsLogged(id))
            return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");
        SendFormattedMessage(playerid, -1, "Hai settato %s (%d) proprietario del gate id %d.", Character_GetOOCName(id), id, gateid);
        SendFormattedMessage(id, -1, "L'admin %s (%d) ti ha settato proprietario del cancello id %d.", Account_GetName(playerid), playerid, gateid);
        Gate_SetOwner(gateid, id);
    }
    else if(!strcmp(command, "rmowner", true))
    {
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai rimosso il proprietario del gate id %d.", gateid);
        map_set_cell(Gates, gateid, OwnerID, 0);
    }
    else 
        return SendClientMessage(playerid, COLOR_ERROR, "/gedit <gateid> <startpos - endpos - faction - owner>");
    return 1;
}