
#include <building_system\core>
#include <YSI_Coding\y_hooks>

#define GAS_STATION_MAX_RANGE       (15.0)
#define PETROL_PRICE_PER_LITRE      (10)
#define PETROL_LITRE_DEF            (5.0) // 1 litre is equals to 10.0 of vehicle fuel.

static 
    Building:IsDoingGas[MAX_PLAYERS] = {INVALID_BUILDING, ...},
    Litres[MAX_PLAYERS char],
    Timer:Timer[MAX_PLAYERS];

hook OnPlayerClearData(playerid)
{
    pc_cmd_stoprifornimento(playerid, "");
    /*stop Timer[playerid];
    Timer[playerid] = Timer:0;
    IsDoingGas[playerid] = INVALID_BUILDING_ID;
    Litres{playerid} = 0;*/
    return 1;
}

hook OnPlayerStateChange(playerid, newstate, oldstate)
{
    if(!Building_IsValid(IsDoingGas[playerid]))
        return 0;
    if(oldstate == PLAYER_STATE_DRIVER && newstate == PLAYER_STATE_ONFOOT)
    {
        pc_cmd_stoprifornimento(playerid, "");
    }
    return 1;
}

flags:rifornisci(CMD_ALIVE_USER);
CMD:rifornisci(playerid, params[])
{
    new Building:building = Character_GetNearGasStation(playerid);
    if(!Building_IsValid(building))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei nei pressi di una pompa di benzina."), 0;
    
    new vehicleid = GetPlayerVehicleID(playerid);

    if(!Vehicle_IsValid(vehicleid) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei in un veicolo o non ne sei il guidatore.");

    if(Vehicle_IsEngineOn(vehicleid))
        return SendClientMessage(playerid, COLOR_ERROR, "Il motore del veicolo deve essere spento per poter fare benzina.");

    if(Vehicle_GetFuel(vehicleid) > 100.0 - PETROL_LITRE_DEF)
        return SendClientMessage(playerid, COLOR_ERROR, "Il veicolo ha già il massimo di benzina.");

    SendFormattedMessage(playerid, COLOR_GREEN, "Stai rifornendo il tuo veicolo. Il prezzo al litro è $%d (compreso di accise).", PETROL_PRICE_PER_LITRE);
    SendClientMessage(playerid, COLOR_GREEN, "Digita \"/stoprifornimento\" per fermarti.");

    IsDoingGas[playerid] = building;

    Timer[playerid] = repeat GetVehicleLitre(playerid, vehicleid);
    return 1;
}

flags:stoprifornimento(CMD_ALIVE_USER);
CMD:stoprifornimento(playerid, params[])
{
    if(!Building_IsValid(IsDoingGas[playerid]))
        return SendClientMessage(playerid, COLOR_ERROR, "Non stai facendo rifornimento.");
    SendFormattedMessage(playerid, -1, "Hai finito di fare rifornimento. Hai messo %d litri per $%d.", Litres{playerid}, Litres{playerid} * PETROL_PRICE_PER_LITRE);
    IsDoingGas[playerid] = INVALID_BUILDING;
    Litres{playerid} = 0;
    stop Timer[playerid];
    Timer[playerid] = Timer:0;
    return 1;
}

timer GetVehicleLitre[1000](playerid, vehicleid) 
{
    if(!Vehicle_IsValid(vehicleid))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Sei uscito dal veicolo ed hai fermato il rifornimento.");
        pc_cmd_stoprifornimento(playerid, "");
        return 1;
    }

    new Float:x, Float:y, Float:z, Building:building = IsDoingGas[playerid];
    Building_GetEnterPos(building, x, y, z);

    if(GetPlayerDistanceFromPoint(playerid, x, y, z) > GAS_STATION_MAX_RANGE)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Ti sei allontanato dalla stazione di benzina ed hai fermato il rifornimento.");
        pc_cmd_stoprifornimento(playerid, "");
        return 1;
    }

    new moneyToGovernament = (PETROL_PRICE_PER_LITRE * Server_GetExcisesOnFuel())/100;
    new moneyToBuilding = PETROL_PRICE_PER_LITRE - moneyToGovernament;

    Server_GiveGovernamentBank(moneyToGovernament);
    Building_GiveMoney(building, moneyToBuilding);

    Vehicle_AddFuel(vehicleid, PETROL_LITRE_DEF);
    
    Litres{playerid}++;

    Character_GiveMoney(playerid, -PETROL_PRICE_PER_LITRE, "/rifornisci");

    if(Vehicle_GetFuel(vehicleid) >= 100.0)
    {
        pc_cmd_stoprifornimento(playerid, "");
    }
    return 1;
}

stock Building:Character_GetNearGasStation(playerid)
{
    new Building:ret = INVALID_BUILDING;
    new Float:x, Float:y, Float:z;
    for_building(building)
    {
        if(Building_GetType(building) != BUILDING_TYPE_GAS_STATION)
            continue;
        Building_GetEnterPos(building, x, y, z);
        if(GetPlayerDistanceFromPoint(playerid, x, y, z) <= GAS_STATION_MAX_RANGE)
        {
            ret = building;
            break;
        }
    }
    return ret;
}

stock Character_IsDoingGas(playerid)
{
    return IsPlayerInAnyVehicle(playerid) && IsDoingGas[playerid] != INVALID_BUILDING;
}

hook function Character_Enter(playerid)
{
    new Building:building, E_ELEMENT_TYPE:type;
    Character_GetNearestPickup(playerid, building, type);
    if(type == ELEMENT_TYPE_BUILDING_ENTRANCE && Building_IsValid(building) && Building_GetType(building) == BUILDING_TYPE_GAS_STATION)
        return 0;
    return continue(playerid);
}