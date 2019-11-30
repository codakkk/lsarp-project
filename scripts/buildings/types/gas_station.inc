
#include <buildings\core>
#include <faction_system\government\core>
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
    IsDoingGas[playerid] = INVALID_BUILDING;
    Litres{playerid} = 0;
    stop Timer[playerid];
    if(Timer[playerid] != Timer:0)
        stop Timer[playerid];
    Timer[playerid] = Timer:0;
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
    new Building:building;
    if(!Character_TryGetGasStation(playerid, building))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei nei pressi di una pompa di benzina."), 0;
    
    if(Building_IsValid(IsDoingGas[playerid]))
        return SendClientMessage(playerid, COLOR_ERROR, "Stai già facendo rifornimento.");

    new vehicleid = GetPlayerVehicleID(playerid);

    if(!Vehicle_IsValid(vehicleid) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei in un veicolo o non ne sei il guidatore.");

    if(Vehicle_IsEngineOn(vehicleid))
        return SendClientMessage(playerid, COLOR_ERROR, "Il motore del veicolo deve essere spento per poter fare benzina.");

    if(Vehicle_GetFuel(vehicleid) > 100.0 - PETROL_LITRE_DEF)
        return SendClientMessage(playerid, COLOR_ERROR, "Il veicolo ha già il massimo di benzina.");

    new taxedPrice = Tax_CalculateByValue(TAX_TYPE_FUEL, GasStation_GetFuelPrice(building));

    if(Character_GetMoney(playerid) < taxedPrice)
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi per fare rifornimento.");

    //if(GasStation_GetFuelState(building) < PETROL_LITRE_DEF)
        //return SendClientMessage(playerid, COLOR_ERROR, "La stazione di benzina non puo' rifornirti il veicolo poiché ha finito le risorse.");

    SendFormattedMessage(playerid, COLOR_GREEN, "Stai rifornendo il tuo veicolo. Il prezzo al litro è $%d (compreso di accise).", Tax_CalculateByValue(TAX_TYPE_FUEL, GasStation_GetFuelPrice(building)));
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
    SendFormattedMessage(playerid, -1, "Hai finito di fare rifornimento. Hai messo %d litri per $%d.", Litres{playerid}, Tax_CalculateByValue(TAX_TYPE_FUEL, Litres{playerid} * GasStation_GetFuelPrice(IsDoingGas[playerid])));
    IsDoingGas[playerid] = INVALID_BUILDING;
    Litres{playerid} = 0;
    stop Timer[playerid];
    Timer[playerid] = Timer:0;
    return 1;
}

timer GetVehicleLitre[1000](playerid, vehicleid) 
{
    new Float:x, Float:y, Float:z;

    if(!Character_IsDoingGas(playerid) || (Building_GetEnterPos(IsDoingGas[playerid], x, y, z) && GetPlayerDistanceFromPoint(playerid, x, y, z) > GAS_STATION_MAX_RANGE))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Sei uscito dal veicolo o ti sei allontanato dalla pompa di benzina.");
        callcmd::stoprifornimento(playerid, "");
        return 1;
    }
    new Building:building = IsDoingGas[playerid];
    
    /*if(GasStation_GetFuelState(building) <= PETROL_LITRE_DEF)
    {
        SendClientMessage(playerid, COLOR_ERROR, "La stazione di benzina ha finito le proprie risorse.");
        callcmd::stoprifornimento(playerid, "");
        return 1;
    }*/

    new fuelPrice = GasStation_GetFuelPrice(building);
    new taxedPrice = Tax_CalculateByValue(TAX_TYPE_FUEL, fuelPrice);
    
    if(!Character_GiveMoney(playerid, -taxedPrice, "/rifornisci"))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi per continuare il rifornimento.");
        callcmd::stoprifornimento(playerid, "");
        return 1;
    }

    Gov_CollectTax(TAX_TYPE_FUEL, fuelPrice);
    Building_GiveMoney(building, fuelPrice);

    Vehicle_AddFuel(vehicleid, PETROL_LITRE_DEF);
    
    Litres{playerid}++;

    if(Vehicle_GetFuel(vehicleid) >= 100.0)
    {
        callcmd::stoprifornimento(playerid, "");
    }
    return 1;
}

static bool:Character_TryGetGasStation(playerid, &Building:building)
{
    building = Building:0;
    new Float:x, Float:y, Float:z;
    for_building_of<BUILDING_TYPE_GAS_STATION>(b)
    {
        Building_GetEnterPos(b, x, y, z);
        if(GetPlayerDistanceFromPoint(playerid, x, y, z) <= GAS_STATION_MAX_RANGE)
        {
            building = b;
            break;
        }
    }
    return Building_IsValid(building);
}

stock Character_IsDoingGas(playerid)
{
    return IsPlayerInAnyVehicle(playerid) && Building_IsValid(IsDoingGas[playerid]);
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
    new Building:building, E_ELEMENT_TYPE:type;
    Character_GetNearestPickup(playerid, building, type);
    if(type == ELEMENT_TYPE_BUILDING_ENTRANCE && Building_IsValid(building) && Building_GetType(building) == BUILDING_TYPE_GAS_STATION)
        return 0;
    return continue(playerid);
}

hook function Building_ShowTypeMenu(Building:building, playerid)
{
    if(Building_GetType(building) != BUILDING_TYPE_GAS_STATION)
        return continue(building, playerid);
    
    new fuelPrice = GasStation_GetFuelPrice(building);
    new taxedPrice = Tax_CalculateByValue(TAX_TYPE_FUEL, fuelPrice);
 
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
        SendFormattedMessage(playerid, COLOR_ORANGE, "Prezzo finale (compreso di tasse governative): $%s", FormatNumber(taxedPrice));
        break;
    }
    return 1;
}