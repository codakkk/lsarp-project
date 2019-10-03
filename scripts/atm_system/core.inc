
#include <YSI_Coding\y_hooks>

static enum _:E_ATM_DATA
{
    ID,
    Float:X,
    Float:Y,
    Float:Z,
    Float:RotX,
    Float:RotY,
    Float:RotZ,
    
    Interior, 
    World,

    Object,
    Pickup
};
static 
    Map:AtmMap,
    Map:AtmLabelMap[MAX_PLAYERS];

hook OnGameModeInit()
{
    AtmMap = map_new();
    mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `atm` ( \
                        `id` int(11) NOT NULL AUTO_INCREMENT, \
                        `pos_x` FLOAT NOT NULL, \
                        `pos_y` FLOAT NOT NULL, \
                        `pos_z` FLOAT NOT NULL, \
                        `rot_x` FLOAT NOT NULL, \
                        `rot_y` FLOAT NOT NULL, \
                        `rot_z` FLOAT NOT NULL, \
                        `interior` INT(11) NOT NULL, \
                        `world` INT(11) NOT NULL, \
                        PRIMARY KEY (`id`) \
                        ) ENGINE=InnoDB DEFAULT CHARSET=latin1");
    ATM_LoadAll();
    return 1;
}

hook OnCharacterClearData(playerid)
{
    if(map_valid(AtmLabelMap[playerid]))
    {
        Character_DebugLabelsForATM(playerid);
    }
    return 1;
}

static Character_DebugLabelsForATM(playerid)
{
    if(map_valid(AtmLabelMap[playerid]))
    {
        SendClientMessage(playerid, COLOR_GREEN, "Hai disabilitato le label sugli ATM.");
        for_map(i : AtmLabelMap[playerid])
        {
            DestroyDynamic3DTextLabel(Text3D:iter_get_value(i));
        }
        map_delete_deep(AtmLabelMap[playerid]);
    }
    else
    {
        SendClientMessage(playerid, COLOR_GREEN, "Hai abilitato le label sugli ATM.");
        AtmLabelMap[playerid] = map_new();
        new atmid, Text3D:label, text[32], Float:x, Float:y, Float:z;
        for_map(i : AtmMap)
        {
            atmid = iter_get_key(i);
            format(text, sizeof(text), "ATM ID %d", atmid);
            ATM_GetPosition(atmid, x, y, z);
            label = CreateDynamic3DTextLabel(text, COLOR_GREY, x, y, z, 10.0, _, _, _, ATM_GetWorld(atmid), ATM_GetInterior(atmid), playerid, _);
            map_add(AtmLabelMap[playerid], atmid, label);
        }
    }
}

flags:atmcmds(CMD_ADMIN);
CMD:atmcmds(playerid, params[])
{
    SendClientMessage(playerid, -1, "[ATM] /atmcreate - /atmedit - /atmid");
    return 1;
}

flags:atmid(CMD_ADMIN);
CMD:atmid(playerid, params[])
{
    Character_DebugLabelsForATM(playerid);
    return 1;
}

CMD:ritira(playerid, params[])
{
    if(!Character_IsNearPickupType(playerid, ELEMENT_TYPE_ATM) && Building_GetType(Character_GetBuilding(playerid, true)) != BUILDING_TYPE_BANK)
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei nei pressi di un atm o in banca.");
    new money;
    if(sscanf(params, "d", money))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USA: /ritira [ammontare]");

    if(money < 1 || money > Character_GetBank(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai tutti questi soldi in banca.");

    Character_GiveMoney(playerid, money, "atm_withdraw");
    Character_GiveBank(playerid, -money, "atm_withdraw");

    SendFormattedMessage(playerid, COLOR_YELLOW, "Hai prelevato $%d dal tuo conto bancario.", money);

    Log(Character_GetOOCName(playerid), "atm", "/ritira", money);
    return 1;
}

CMD:deposita(playerid, params[])
{
    if(Building_GetType(Character_GetBuilding(playerid, true)) != BUILDING_TYPE_BANK)
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'interno di una banca.");
    new money;
    if(sscanf(params, "d", money))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USA: /deposita [ammontare]");

    if(money < 1 || money > Character_GetMoney(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai tutti questi soldi.");

    Character_GiveMoney(playerid, -money, "atm_withdraw");
    Character_GiveBank(playerid, money, "atm_withdraw");

    SendFormattedMessage(playerid, -1, "Hai depositato $%d nel tuo conto bancario.", money);

    Log(Character_GetOOCName(playerid), "bank", "/deposita", money);
    return 1;
}

CMD:bonifico(playerid, params[])
{
    if(Building_GetType(Character_GetBuilding(playerid, true)) != BUILDING_TYPE_BANK)
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'interno di una banca.");
    new n[24], money;
    if(sscanf(params, "s[24]d", n, money))
    {
        SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USA: /bonifico [nome] [ammontare]");
        SendClientMessage(playerid, COLOR_ERROR, "Ricorda che puoi utilizzare /bonifico gov <ammontare> per inviare un bonifico allo stato.");
        return 1;
    }

    if(money < 1 || money > Character_GetBank(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai tutti questi soldi in banca.");

    if(!strcmp(n, "gov", true) || !strcmp(n, "governo", true) || !strcmp(n, "government", true))
    {
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai inviato un bonifico di $%d al Governo.", money);
        Character_GiveBank(playerid, -money, "bank_bonifico");
        Server_GiveGovernamentBank(money);
        return 1;
    }   

    inline OnLoad()
    {
        if(cache_num_rows() > 0)
        {
            new id, name[24], bank;
            cache_get_value_name_int(0, "id", id);
            cache_get_value_name(0, "name", name);
            cache_get_value_name_int(0, "bank", bank);
            
            new newBank = bank + money, other = Character_IsIDConnected(id);
            new msg[128];
            
            format(msg, sizeof(msg), "Hai ricevuto un bonifico di $%d da %s", money, Character_GetOOCName(playerid));

            if(Character_IsLogged(other))
            {
                SendClientMessage(other, COLOR_GREEN, msg);
                Character_SetBank(other, newBank);
            }
            else
            {
                mysql_tquery_f(gMySQL, "UPDATE `characters` SET bank = '%d' WHERE id = '%d'", newBank, id);
                SendNotification(id, msg);
            }
            SendFormattedMessage(playerid, 0xCBCCCEFF, "Hai effettuato un bonifico a %s di $%s.", name, FormatNumber(money));
            Character_GiveBank(playerid, -money, "bank_bonifico");
            
            Log(Character_GetOOCName(playerid), name, "/bonifico", money);
        }
        else
        {
            SendClientMessage(playerid, COLOR_ERROR, "Il giocatore inserito non esiste.");
        }
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT id, name, bank FROM `characters` WHERE LOWER(name) = LOWER('%e')", n);

    return 1;
}

CMD:bilancio(playerid, params[])
{
    if(!Character_IsNearPickupType(playerid, ELEMENT_TYPE_ATM) && Building_GetType(Character_GetBuilding(playerid, true)) != BUILDING_TYPE_BANK)
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei nei pressi di un atm o in banca.");
    Character_AMe(playerid, "verifica il suo bilancio bancario.");
    SendFormattedMessage(playerid, 0xCBCCCEFF, "Sul tuo conto bancario possiedi $%d.", Character_GetBank(playerid));
    return 1;
}

flags:atmcreate(CMD_ADMIN);
CMD:atmcreate(playerid, params[])
{
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    new id = ATM_Create(x, y, z, GetPlayerInterior(playerid), Character_GetVirtualWorld(playerid));
    SendFormattedMessage(playerid, COLOR_GREEN, "ATM id %d creato. Usa /atmedit %d per modificarlo.", id, id);

    SendMessageToAdmins(1, COLOR_ORANGE, "AdmCmd: %s ha creato l'ATM id %d", Account_GetName(playerid), id);
    return 1;
}

flags:atmedit(CMD_ADMIN);
CMD:atmedit(playerid, params[])
{
    new id, command[16];
    if(sscanf(params, "ds[16]", id, command))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /atmedit <atmid> <pos - delete>");
    
    if(!ATM_IsValid(id))
        return SendClientMessage(playerid, COLOR_ERROR, "L'ATM id inserito non è valido.");

    if(!strcmp(command, "pos", true))
    {
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai spostato l'atm id %d alle posizioni correnti.", id);
        EditObject_Start(playerid, EDIT_TYPE_ATM, ATM_GetObject(id), id);
        //new Float:x, Float:y, Float:z;
        //GetPlayerPos(playerid, x, y, z);
        //ATM_SetPosition(id, x, y, z, GetPlayerInterior(playerid), Character_GetVirtualWorld(playerid));
    }
    else if(!strcmp(command, "delete", true))
    {
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai deletato l'atm id %d.", id);    
        SendMessageToAdmins(1, COLOR_ORANGE, "AdmCmd: %s ha deletato l'ATM id %d", Account_GetName(playerid), id);
        ATM_Delete(id);
    }
    return 1;
}

stock bool:ATM_IsValid(atmid)
{
    return map_has_key(AtmMap, atmid);
}

hook OnPlayerEditDynObject(playerid, objectid, response, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz)
{
	if(EditObject_GetType(playerid) == EDIT_TYPE_ATM)
	{
		new atmid = EditObject_GetExtraID(playerid);
		if(response == EDIT_RESPONSE_CANCEL)
		{
			new Float:tx, Float:ty, Float:tz;
			
			ATM_GetPosition(atmid, tx, ty, tz);
			SetDynamicObjectPos(objectid, tx, ty, tz);

			ATM_GetRotation(atmid, tx, ty, tz);
			SetDynamicObjectRot(objectid, tx, ty, tz);

			EditObject_End(playerid);
		}
		else if(response == EDIT_RESPONSE_FINAL)
		{
			ATM_SetPosition(atmid, x, y, z, ATM_GetInterior(atmid), ATM_GetWorld(atmid));
			ATM_SetRotation(atmid, rx, ry, rz);
    
            Pickup_Destroy(ATM_GetPickup(atmid));
            new pickupid = Pickup_Create(1007, atmid, x, y, z, ELEMENT_TYPE_ATM, ATM_GetWorld(atmid), ATM_GetInterior(atmid));
            map_set_cell(AtmMap, atmid, Pickup, pickupid);

            SendFormattedMessage(playerid, COLOR_GREEN, "Hai spostato l'atm %d.", atmid);
			
			EditObject_End(playerid);
		}
	}
	return 1;
}

stock ATM_GetID(atmid)
{
    if(!ATM_IsValid(atmid))
        return 0;
    return map_get(AtmMap, atmid, ID);
}

stock ATM_Create(Float:x, Float:y, Float:z, interior, world)
{
    new Cache:result = mysql_query_f(gMySQL, true, "INSERT INTO `atm` \
    (pos_x, pos_y, pos_z, rot_x, rot_y, rot_z, interior, world) \
    VALUES('%f', '%f', '%f', '%f', '%f', '%f', '%d', '%d')", x, y, z, 0.0, 0.0, 0.0, interior, world);

    new 
        id = cache_insert_id(),
        data[E_ATM_DATA]
    ;
    data[ID] = id;
    data[X] = x;
    data[Y] = y;
    data[Z] = z;
    data[RotX] = 0.0;
    data[RotY] = 0.0;
    data[RotZ] = 0.0;
    data[Interior] = interior;
    data[World] = world;
    data[Object] = CreateDynamicObject(19324, x, y, z, 0.0, 0.0, 0.0, world, interior, -1);

    map_add_arr(AtmMap, id, data);

    cache_delete(result);
    return id;
}

stock bool:ATM_SetPosition(atmid, Float:x, Float:y, Float:z, interior, world)
{
    if(!ATM_IsValid(atmid))
        return false;
    map_set_cell(AtmMap, atmid, X, x);
    map_set_cell(AtmMap, atmid, Y, y);
    map_set_cell(AtmMap, atmid, Z, z);
    map_set_cell(AtmMap, atmid, Interior, interior);
    map_set_cell(AtmMap, atmid, World, world);

    SetDynamicObjectPos(ATM_GetObject(atmid), x, y, z);

    mysql_tquery_f(gMySQL, "UPDATE `atm` SET pos_x = '%f', pos_y = '%f', pos_z = '%f', interior = '%d', world = '%d' WHERE id = '%d'", x, y, z, interior, world, ATM_GetID(atmid));
    return true;
}

stock bool:ATM_GetPosition(atmid, &Float:x, &Float:y, &Float:z)
{
    if(!ATM_IsValid(atmid))
        return false;
    x = Float:map_get(AtmMap, atmid, X);
    y = Float:map_get(AtmMap, atmid, Y);
    z = Float:map_get(AtmMap, atmid, Z);
    return true;
}

stock bool:ATM_SetRotation(atmid, Float:rx, Float:ry, Float:rz)
{
    if(!ATM_IsValid(atmid))
        return false;
    map_set_cell(AtmMap, atmid, RotX, rx);
    map_set_cell(AtmMap, atmid, RotY, ry);
    map_set_cell(AtmMap, atmid, RotZ, rz);
    
    SetDynamicObjectRot(ATM_GetObject(atmid), rx, ry, rz);
    
    mysql_tquery_f(gMySQL, "UPDATE `atm` SET rot_x = '%f', rot_y = '%f', rot_z = '%f' WHERE id = '%d'", rx, ry, rz, ATM_GetID(atmid));
    return true;
}

stock bool:ATM_GetRotation(atmid, &Float:rx, &Float:ry, &Float:rz)
{
    if(!ATM_IsValid(atmid))
        return false;
    rx = Float:map_get(AtmMap, atmid, RotX);
    ry = Float:map_get(AtmMap, atmid, RotY);
    rz = Float:map_get(AtmMap, atmid, RotZ);
    return true;
}

stock ATM_GetWorld(atmid)
{
    if(!ATM_IsValid(atmid))
        return 0;
    return map_get(AtmMap, atmid, World);
}

stock ATM_GetInterior(atmid)
{
    if(!ATM_IsValid(atmid))
        return 0;
    return map_get(AtmMap, atmid, Interior);
}

stock ATM_GetPickup(atmid)
{
    return map_get(AtmMap, ATM_GetID(atmid), Pickup);
}

stock ATM_GetObject(atmid)
{
    return map_get(AtmMap, ATM_GetID(atmid), Object);
}

stock ATM_Delete(atmid)
{
    mysql_tquery_f(gMySQL, "DELETE FROM `atm` WHERE id = '%d'", ATM_GetID(atmid));
    Pickup_Destroy(ATM_GetPickup(atmid));
    DestroyDynamicObject(ATM_GetObject(atmid));
    map_remove_deep(AtmMap, ATM_GetID(atmid));
    return 1;
}

static ATM_LoadAll()
{
    inline OnLoad()
    {
        new rows = cache_num_rows();
        new data[E_ATM_DATA];
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", data[ID]);

            cache_get_value_name_float(i, "pos_x", data[X]);
            cache_get_value_name_float(i, "pos_y", data[Y]);
            cache_get_value_name_float(i, "pos_z", data[Z]);

            cache_get_value_name_float(i, "rot_x", data[RotX]);
            cache_get_value_name_float(i, "rot_y", data[RotY]);
            cache_get_value_name_float(i, "rot_z", data[RotZ]);

            cache_get_value_name_int(i, "interior", data[Interior]);
            cache_get_value_name_int(i, "world", data[World]);

            data[Object] = CreateDynamicObject(19324, data[X], data[Y], data[Z], data[RotX], data[RotY], data[RotZ], data[World], data[Interior], -1);
            data[Pickup] = Pickup_Create(1007, data[ID], data[X], data[Y], data[Z], ELEMENT_TYPE_ATM, data[World], data[Interior]);
            map_add_arr(AtmMap, data[ID], data);

        }
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `atm`");
    return 1;
}