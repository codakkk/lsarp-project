
#include <YSI_Coding\y_hooks>

#define MAX_GATES       50

static enum E_GATE_INFO
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
    GateInfo[MAX_GATES][E_GATE_INFO], 
    Iterator:Gates<MAX_GATES>,
    bool:GateState[MAX_GATES char] = {false, ...};

hook OnGameModeInit()
{
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

                new Float:tx, Float:ty, Float:tz,
                    Float:trx, Float:try, Float:trz;
                Gate_GetStartPos(gateid, tx, ty, tz);
			    Gate_GetStartRot(gateid, trx, try, trz);
                SetDynamicObjectPos(objectid, tx, ty, tz);
                SetDynamicObjectRot(objectid, trx, try, trz);

                SendFormattedMessage(playerid, -1, "Hai settato la posizione di arrivo per il gateid %d.", gateid);
            }
			Gate_Save(gateid);
            Log(Character_GetOOCName(playerid), "", "Edit Gate", gateid);

			EditObject_End(playerid);
		}
    }
    return 1;
}

flags:cancello(CMD_ALIVE_USER);
CMD:cancello(playerid, params[])
{
    new Float:x, Float:y, Float:z;
    foreach(new gateid : Gates)
    {
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
            Gate_Save(gateid);
            return SendFormattedMessage(playerid, -1, "Hai rimosso il cancello alla fazione precedente (Gate id: %d).", gateid);
        }
        if(!Faction_IsValid(factionid))
            return SendClientMessage(playerid, COLOR_ERROR, "La fazione inserita non esiste.");
        Gate_SetFaction(gateid, factionid);
        Gate_Save(gateid);
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
        SendFormattedMessage(id, -1, "L'admin %s (%d) ti ha settato proprietario del cancello id %d.", AccountInfo[playerid][aName], playerid, gateid);
        Gate_SetOwner(gateid, id);
        Gate_Save(gateid);
    }
    else if(!strcmp(command, "rmowner", true))
    {
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai rimosso il proprietario del gate id %d.", gateid);
        GateInfo[gateid][OwnerID] = 0;
        Gate_Save(gateid);
    }
    else 
        return SendClientMessage(playerid, COLOR_ERROR, "/gedit <gateid> <startpos - endpos - faction - owner>");
    return 1;
}

stock Gate_Create(Float:x, Float:y, Float:z)
{
    new id = Iter_Free(Gates);
    if(id == -1)
        return -1;
    
    Gate_SetFaction(id, INVALID_FACTION_ID);
    Gate_SetOwner(id, -1);
    
    Gate_SetStartPos(id, x, y, z);
    Gate_SetStartRot(id, 0.0, 0.0, 0.0);
    Gate_SetEndPos(id, 0.0, 0.0, 0.0);
    Gate_SetEndRot(id, 0.0, 0.0, 0.0);

    GateState{id} = false;

    inline OnCreate()
    {
        GateInfo[id][ID] = cache_insert_id();
        GateInfo[id][ObjectID] = CreateDynamicObject(19912, x, y, z, 0, 0, 0, -1, -1, -1, STREAMER_OBJECT_SD, STREAMER_OBJECT_DD);
        Streamer_SetIntData(STREAMER_TYPE_OBJECT, GateInfo[id][ObjectID], E_STREAMER_EXTRA_ID, id);
        Iter_Add(Gates, id);
    }
    MySQL_TQueryInline(gMySQL, using inline OnCreate, "INSERT INTO `gates` (model_id, faction_id, owner_id) VALUES(19912, '%d', '%d')", Gate_GetFaction(id), Gate_GetOwner(id));
    return id;
}

stock Gate_LoadAll()
{
    inline OnLoad()
    {
        new rows = cache_num_rows();
        if(rows >= MAX_GATES)
            rows = MAX_GATES-1;
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_index_int(i, 0, GateInfo[i][ID]);

            cache_get_value_index_float(i, 2, GateInfo[i][StartX]);
            cache_get_value_index_float(i, 3, GateInfo[i][StartY]);
            cache_get_value_index_float(i, 4, GateInfo[i][StartZ]);

            cache_get_value_index_float(i, 5, GateInfo[i][StartRotX]);
            cache_get_value_index_float(i, 6, GateInfo[i][StartRotY]);
            cache_get_value_index_float(i, 7, GateInfo[i][StartRotZ]);

            cache_get_value_index_float(i, 8, GateInfo[i][EndX]);
            cache_get_value_index_float(i, 9, GateInfo[i][EndY]);
            cache_get_value_index_float(i, 10, GateInfo[i][EndZ]);

            cache_get_value_index_float(i, 11, GateInfo[i][EndRotX]);
            cache_get_value_index_float(i, 12, GateInfo[i][EndRotY]);
            cache_get_value_index_float(i, 13, GateInfo[i][EndRotZ]);

            cache_get_value_index_int(i, 14, GateInfo[i][FactionID]);

            cache_get_value_index_int(i, 15, GateInfo[i][OwnerID]);

            GateInfo[i][ObjectID] = CreateDynamicObject(19912, GateInfo[i][StartX], GateInfo[i][StartY], GateInfo[i][StartZ], GateInfo[i][StartRotX], GateInfo[i][StartRotY], GateInfo[i][StartRotZ], -1, -1, -1, STREAMER_OBJECT_SD, STREAMER_OBJECT_DD);

            GateState{i} = false;
            Streamer_SetIntData(STREAMER_TYPE_OBJECT, GateInfo[i][ObjectID], E_STREAMER_EXTRA_ID, i);
            Iter_Add(Gates, i);
        }
        printf("Loaded %d gates.", rows);
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `gates` ORDER BY id DESC");
}

stock Gate_Save(gateid)
{
    new Float:x, Float:y, Float:z,
        Float:rx, Float:ry, Float:rz,
        Float:ex, Float:ey, Float:ez,
        Float:erx, Float:ery, Float:erz;
    Gate_GetStartPos(gateid, x, y, z);
    Gate_GetStartRot(gateid, rx, ry, rz);
    Gate_GetEndPos(gateid, ex, ey, ez);
    Gate_GetEndRot(gateid, erx, ery, erz);
    mysql_tquery_f(gMySQL, "UPDATE `gates` SET \
                            start_x = '%f', start_y = '%f', start_z = '%f', \
                            start_rot_x = '%f', start_rot_y = '%f', start_rot_z = '%f', \
                            end_x = '%f', end_y = '%f', end_z = '%f', \
                            end_rot_x = '%f', end_rot_y = '%f', end_rot_z = '%f', \
                            faction_id = '%d', owner_id = '%d' WHERE ID = '%d'",
                            x, y, z,
                            rx, ry, rz,
                            ex, ey, ez,
                            erx, ery, erz,
                            Gate_GetFaction(gateid),
                            Gate_GetOwner(gateid),
                            Gate_GetID(gateid));    
    return 1;
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
    return 0 <= gateid < MAX_GATES;
}

stock Gate_GetID(gateid)
{
    if(!Gate_IsValid(gateid))
        return 0;
    return GateInfo[gateid][ID];
}

stock Gate_SetFaction(gateid, factionid)
{
    if(!Gate_IsValid(gateid))
        return 0;
    GateInfo[gateid][FactionID] = Faction_GetID(factionid);
    return 1;
}

stock Gate_GetFaction(gateid)
{
    if(!Gate_IsValid(gateid))
        return 0;
    return GateInfo[gateid][FactionID];
}

stock Gate_SetOwner(gateid, playerid)
{
    if(!Gate_IsValid(gateid))
        return 0;
    GateInfo[gateid][OwnerID] = Character_GetID(playerid);
    return 1;
}

stock Gate_GetOwner(gateid)
{
    if(!Gate_IsValid(gateid))
        return 0;
    return GateInfo[gateid][OwnerID];
}

stock Gate_SetStartPos(gateid, Float:x, Float:y, Float:z)
{
    if(!Gate_IsValid(gateid))
        return 0;   
    GateInfo[gateid][StartX] = x;
    GateInfo[gateid][StartY] = y;
    GateInfo[gateid][StartZ] = z;

    return 1;
}

stock Gate_GetStartPos(gateid, &Float:x, &Float:y, &Float:z)
{
    if(!Gate_IsValid(gateid))
        return 0;   
    x = GateInfo[gateid][StartX];
    y = GateInfo[gateid][StartY];
    z = GateInfo[gateid][StartZ];

    return 1;
}

stock Gate_SetStartRot(gateid, Float:x, Float:y, Float:z)
{
    if(!Gate_IsValid(gateid))
        return 0;   
    GateInfo[gateid][StartRotX] = x;
    GateInfo[gateid][StartRotY] = y;
    GateInfo[gateid][StartRotZ] = z;

    return 1;
}

stock Gate_GetStartRot(gateid, &Float:x, &Float:y, &Float:z)
{
    if(!Gate_IsValid(gateid))
        return 0;   
    x = GateInfo[gateid][StartRotX];
    y = GateInfo[gateid][StartRotY];
    z = GateInfo[gateid][StartRotZ];

    return 1;
}

stock Gate_SetEndPos(gateid, Float:x, Float:y, Float:z)
{
    if(!Gate_IsValid(gateid))
        return 0;   
    GateInfo[gateid][EndX] = x;
    GateInfo[gateid][EndY] = y;
    GateInfo[gateid][EndZ] = z;

    return 1;
}

stock Gate_GetEndPos(gateid, &Float:x, &Float:y, &Float:z)
{
    if(!Gate_IsValid(gateid))
        return 0;   
    x = GateInfo[gateid][EndX];
    y = GateInfo[gateid][EndY];
    z = GateInfo[gateid][EndZ];

    return 1;
}

stock Gate_SetEndRot(gateid, Float:x, Float:y, Float:z)
{
    if(!Gate_IsValid(gateid))
        return 0;   
    GateInfo[gateid][EndRotX] = x;
    GateInfo[gateid][EndRotY] = y;
    GateInfo[gateid][EndRotZ] = z;

    return 1;
}

stock Gate_GetEndRot(gateid, &Float:x, &Float:y, &Float:z)
{
    if(!Gate_IsValid(gateid))
        return 0;   
    x = GateInfo[gateid][EndRotX];
    y = GateInfo[gateid][EndRotY];
    z = GateInfo[gateid][EndRotZ];

    return 1;
}

stock Gate_GetObjectID(gateid)
{
    if(!Gate_IsValid(gateid))
        return 0;
    return GateInfo[gateid][ObjectID];
}