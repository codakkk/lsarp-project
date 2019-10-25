
#include <buildings\core>
#include <YSI_Coding\y_hooks>

static const FuelPriceKey[] = "fuel_price";
static const FuelStateKey[] = "fuel_state";

#define GAS_STATION_MAX_RANGE       (15.0)
#define PETROL_LITRE_DEF            (5.0) // 1 litre is equals to 10.0 of vehicle fuel.

static 
    Building:IsDoingGas[MAX_PLAYERS] = {INVALID_BUILDING, ...},
    Litres[MAX_PLAYERS char],
    Timer:Timer[MAX_PLAYERS];

hook OnCharacterClearData(playerid)
{
    callcmd::stoprifornimento(playerid, "");
    /*stop Timer[playerid];
    Timer[playerid] = Timer:0;
    IsDoingGas[playerid] = INVALID_BUILDING_ID;
    Litres{playerid} = 0;*/
    return 1;
}

CMD:cccc(playerid, params[])
{
    printf("Ping: %d", GetPlayerPing(playerid));   
    return 1;
}

hook OnPlayerStateChange(playerid, newstate, oldstate)
{
    if(!Building_IsValid(IsDoingGas[playerid]))
        return 0;
    if(oldstate == PLAYER_STATE_DRIVER && newstate == PLAYER_STATE_ONFOOT)
    {
        callcmd::stoprifornimento(playerid, "");
    }
    return 1;
}

CMD:setbfuel(playerid, params[])
{
    new buildingid, value;
    if(sscanf(params, "dd", buildingid, value))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /setbfuel <buildingid> <fuel quantity>");
    new Building:building = Building_Find(buildingid);
    if(!Building_IsValid(building))
        return SendClientMessage(playerid, COLOR_ERROR, "Il buildingid inserito non esiste.");
    GasStation_SetFuelState(building, float(value));
    return 1;
}

CMD:setfuelprice(playerid, params[])
{
    new buildingid, value;
    if(sscanf(params, "dd", buildingid, value))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /setfuelprice <buildingid> <price>");
    new Building:building = Building_Find(buildingid);
    if(!Building_IsValid(building))
        return SendClientMessage(playerid, COLOR_ERROR, "Il buildingid inserito non esiste.");
    GasStation_SetFuelPrice(building, value);
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

    if(GasStation_GetFuelState(building) < PETROL_LITRE_DEF)
        return SendClientMessage(playerid, COLOR_ERROR, "La stazione di benzina non puo' rifornirti il veicolo poiché ha finito le risorse.");

    SendFormattedMessage(playerid, COLOR_GREEN, "Stai rifornendo il tuo veicolo. Il prezzo al litro è $%d (compreso di accise).", GasStation_GetFuelPrice(building));
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
    SendFormattedMessage(playerid, -1, "Hai finito di fare rifornimento. Hai messo %d litri per $%d.", Litres{playerid}, Litres{playerid} * GasStation_GetFuelPrice(IsDoingGas[playerid]));
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
        callcmd::stoprifornimento(playerid, "");
        return 1;
    }
    new Building:building = IsDoingGas[playerid];
    if(GasStation_GetFuelState(building) <= PETROL_LITRE_DEF)
    {
        SendClientMessage(playerid, COLOR_ERROR, "La stazione di benzina ha finito le proprie risorse.");
        callcmd::stoprifornimento(playerid, "");
        return 1;
    }
    new Float:x, Float:y, Float:z;
    Building_GetEnterPos(building, x, y, z);

    if(GetPlayerDistanceFromPoint(playerid, x, y, z) > GAS_STATION_MAX_RANGE)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Ti sei allontanato dalla stazione di benzina ed hai fermato il rifornimento.");
        callcmd::stoprifornimento(playerid, "");
        return 1;
    }

    new fuelPrice = GasStation_GetFuelPrice(building);
    new taxedPrice = GasStation_GetTaxedFuelPrice(building);
    new moneyToGovernament = taxedPrice - fuelPrice;
    new moneyToBuilding = fuelPrice - moneyToGovernament;

    Server_GiveGovernamentBank(moneyToGovernament);
    Building_GiveMoney(building, moneyToBuilding);

    Vehicle_AddFuel(vehicleid, PETROL_LITRE_DEF);
    
    Litres{playerid}++;

    Character_GiveMoney(playerid, -taxedPrice, "/rifornisci");

    if(Vehicle_GetFuel(vehicleid) >= 100.0)
    {
        callcmd::stoprifornimento(playerid, "");
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

stock bool:GasStation_SetFuelPrice(Building:building, newPrice)
{
    Building_SetIntData(building, FuelPriceKey, newPrice);
    return true;
}

stock GasStation_GetFuelPrice(Building:building)
{
    new v = 0;
    Building_GetIntData(building, FuelPriceKey, v);
    return v;
}

stock GasStation_GetTaxedFuelPrice(Building:building)
{
    new withoutTaxes = GasStation_GetFuelPrice(building);
    return withoutTaxes + floatround(float(withoutTaxes * Server_GetExcisesOnFuel())/100.0, floatround_ceil);
}

stock bool:GasStation_GiveFuelState(Building:building, Float:giveValue)
{
    new Float:f = 0.0;
    Building_GetFloatData(building, FuelStateKey, f);
    Building_SetFloatData(building, FuelStateKey, f + giveValue);
    return true;
}

stock bool:GasStation_SetFuelState(Building:building, Float:newState)
{
    Building_SetFloatData(building, FuelStateKey, newState);
    return true;
}

stock Float:GasStation_GetFuelState(Building:building)
{
    new Float:f = 0.0;
    Building_GetFloatData(building, FuelStateKey, f);
    return f;
}

hook function Character_Enter(playerid)
{
    printf("[Character_Enter]: Gas Station");
    new Building:building, E_ELEMENT_TYPE:type;
    Character_GetNearestPickup(playerid, building, type);
    if(type == ELEMENT_TYPE_BUILDING_ENTRANCE && Building_IsValid(building) && Building_GetType(building) == BUILDING_TYPE_GAS_STATION)
        return 0;
    return continue(playerid);
}

hook function Building_ShowTypeMenu(Building:building, playerid)
{
    if(Building_GetType(building) != BUILDING_TYPE_GAS_STATION)
        return continue(playerid);
    
    new fuelPrice = GasStation_GetFuelPrice(building);
    new taxedPrice = GasStation_GetTaxedFuelPrice(building);
 
    new response[e_DIALOG_RESPONSE_INFO];
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_INPUT, @("Modifica prezzo benzina"), str_format("Inserisci il nuovo prezzo al litro per la benzina.\nPrezzo Attuale: $%d\nPrezzo Finale (compreso di accise): $%d", fuelPrice, taxedPrice), @("Modifica"), @("Chiudi"));

        if(!response[E_DIALOG_RESPONSE_Response]) break;

        new newPrice = strval(response[E_DIALOG_RESPONSE_InputText]);
        if(newPrice <= 0) 
        {
            SendClientMessage(playerid, COLOR_ERROR, "Il nuovo prezzo inserito per la benzina non è valido.");
            continue;
        }

        GasStation_SetFuelPrice(building, newPrice);
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai modificato il prezzo della benzina del tuo business. Nuovo prezzo: $%s.", FormatNumber(newPrice));
        SendFormattedMessage(playerid, COLOR_ORANGE, "Prezzo: $%s", FormatNumber(newPrice));
        SendFormattedMessage(playerid, COLOR_ORANGE, "Prezzo finale (compreso di tasse governative): $%s", FormatNumber(GasStation_GetTaxedFuelPrice(building)));
        break;
    }
    return 1;
}