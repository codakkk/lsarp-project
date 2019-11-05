
#include <YSI_Coding\y_hooks>

static Building:EmployeeAt[MAX_PLAYERS] = {INVALID_BUILDING, ...};

hook OnCharacterClearData(playerid)
{
    EmployeeAt[playerid] = INVALID_BUILDING;
    return 1;
}

hook OnCharacterLoad(playerid)
{
    await mysql_aquery_s(gMySQL, str_format("SELECT employee_at FROM `characters` WHERE id = '%d'", Character_GetID(playerid)));
    
    new buildingid = 0;
    cache_get_value_name_int(0, "employee_at", buildingid);

    new Building:temp = Building_Find(buildingid);

    if(Building_IsValid(temp))
        EmployeeAt[playerid] = temp;
    return 1;
}

flags:assumi(CMD_ALIVE_USER);
CMD:assumi(playerid, params[])
{
    new id, buildingid;
    if(sscanf(params, "k<u>d", id, buildingid))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /assumi [playerid/partofname] [businessid]");
    new Building:building = Building_Find(buildingid);

    if(!Building_IsValid(building) || !Character_DoesOwnBuilding(playerid, building))
        return SendClientMessage(playerid, COLOR_ERROR, "Il business inserito non è tuo.");

    if(playerid == id || !Character_IsLogged(id))
        return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido o giocatore non connesso.");
    
    if(!Character_IsEmployee(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore è già dipendente di un altro business.");
    
    if(!IsPlayerInRangeOfPlayer(playerid, id, 3.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");

    Character_SetRequest(playerid, id, REQUEST_TYPE_EMPLOYEE, building);

    new name[MAX_BUILDING_NAME];
    Building_GetName(building, name);

    SendFormattedMessage(id, COLOR_RECEIVE_REQUEST, "%s vuole assumerti nel suo edificio \"%s\".", Character_GetRolePlayName(playerid), name);
    SendClientMessage(id, COLOR_RECEIVE_REQUEST, "Digita \"accetta lavoro\" per accettare o \"rifiuta lavoro\" per rifiutare.");
    SendFormattedMessage(playerid, COLOR_SEND_REQUEST, "Hai proposto a %s di diventare un tuo dipendente presso il tuo edificio \"%s\".", Character_GetRolePlayName(id), name);
    SendClientMessage(playerid, COLOR_SEND_REQUEST, "Digita \"annulla lavoro\" per annullare la richiesta.");
    return 1;
}

flags:licenziati(CMD_ALIVE_USER);
CMD:licenziati(playerid, params[])
{
    if(!Character_IsEmployee(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei dipendente di nessun business.");
    
    
    new name[MAX_BUILDING_NAME];
    Building_GetName(Character_GetEmployeeAt(playerid), name);

    SendFormattedMessage(playerid, COLOR_ORANGE, "Adesso non fai più parte del business \"%s\".", name);
    
    Character_SetEmployeeAt(playerid, INVALID_BUILDING);
    return 1;
}

hook OnCharacterTryAccept(playerid, type)
{
    if(type != REQUEST_TYPE_EMPLOYEE)
        return 0;
	
	new 
        senderid = Request_GetSender(playerid, REQUEST_TYPE_EMPLOYEE),
        Building:building = Building:Request_GetItem(playerid, REQUEST_TYPE_EMPLOYEE);

    if(!IsPlayerInRangeOfPlayer(playerid, senderid, 3.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore."), 0;
    
    if(!Building_IsValid(building) || !Character_DoesOwnBuilding(playerid, building))
    {
        Request_Reset(playerid, REQUEST_TYPE_EMPLOYEE);
        Request_Reset(senderid, REQUEST_TYPE_EMPLOYEE);
        return SendClientMessage(playerid, COLOR_ERROR, "Il business non è più del giocatore che ti ha inviato la richiesta.");
    }

    new name[MAX_BUILDING_NAME];
    Building_GetName(building, name);

    SendFormattedMessage(senderid, COLOR_ACCEPT_REQUEST, "%s ha accettato la tua richiesta di lavoro presso il tuo edificio \"%s\".", Character_GetRolePlayName(playerid), name);
    SendFormattedMessage(playerid, COLOR_ACCEPT_REQUEST, "Hai accettato il lavoro di %s presso il suo edificio \"%s\".", Character_GetRolePlayName(senderid), name);

    Character_SetEmployeeAt(playerid, building);

    Request_Reset(playerid, REQUEST_TYPE_EMPLOYEE);
    Request_Reset(senderid, REQUEST_TYPE_EMPLOYEE);
    return 1;
}

stock bool:Character_IsEmployee(playerid)
{
    new Building:b = Character_GetEmployeeAt(playerid);
    return b != INVALID_BUILDING && Building_IsValid(b);
}

stock Building:Character_GetEmployeeAt(playerid)
{
    return EmployeeAt[playerid];
}

stock Character_SetEmployeeAt(playerid, Building:building)
{
    EmployeeAt[playerid] = building;
    new buildingid = 0;
    if(building != INVALID_BUILDING && Building_IsValid(building))
        buildingid = Building_GetID(building);
    mysql_pquery_f(gMySQL, "UPDATE `characters` SET employee_at = '%d' WHERE id = '%d'", buildingid, Character_GetID(playerid));
}