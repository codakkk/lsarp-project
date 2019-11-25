
#include <buildings\core>
#include <YSI_Coding\y_hooks>

hook OnPlayerPickUpElmPickup(playerid, pickupid, Building:elementId, E_ELEMENT_TYPE:type)
{
    if(type == ELEMENT_TYPE_BUILDING_ENTRANCE && Building_IsValid(elementId) && Building_GetType(elementId) == BUILDING_TYPE_PAYNSPRAY)
    {
        Player_Info(playerid, "-- ~y~Pay'n'Spray~w~ --~n~Usa /riparaveicolo a bordo di un veicolo per ripararlo", true, .time=2500);
    }
    return 1;
}

flags:riparaveicolo(CMD_ALIVE_USER);
CMD:riparaveicolo(playerid, params[])
{
    if(!IsPlayerInAnyVehicle(playerid) || GetPlayerVehicleSeat(playerid) != 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei il guidatore di un veicolo.");
    
    new Building:building;
    if(!Character_TryGetPaySpray(playerid, building))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei nei pressi di un Pay'n'Spray.");
    
    new remainder = floatround(999.0 - Vehicle_GetHealth(GetPlayerVehicleID(playerid)), floatround_ceil);

    if(remainder < 2)
        return SendClientMessage(playerid, COLOR_ERROR, "Il tuo veicolo non ha bisogno di riparazioni.");

    new basePrice = GetRepairPricePerHP(building),
        taxes = Server_GetTaxOnRepairs(),
        priceWithoutTaxes = basePrice * remainder,
        taxedPrice = GetTaxedRepairPrice(building, remainder);

    yield 1;
    new String:string = str_new("Sei sicuro di voler riparare il tuo veicolo?\n");
    str_append_format(string, "HP Veicolo: %d\n", remainder);
    str_append_format(string, "Prezzo (senza tasse) per HP: $%d\n", basePrice);
    str_append_format(string, "Tasse: %d%%\n", taxes);
    str_append_format(string, "Prezzo finale: $%d\n", taxedPrice);
    
    new response[e_DIALOG_RESPONSE_INFO];
    await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_MSGBOX, @("Ripara veicolo"), string, @("Ripara"), @("Chiudi"));

    if(response[E_DIALOG_RESPONSE_Response])
    {
        if(Character_GetMoney(playerid) < taxedPrice)
            return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi per riparare il veicolo.");
        
        if(!IsPlayerInAnyVehicle(playerid))
            return SendClientMessage(playerid, COLOR_ERROR, "Non sei più sul veicolo da riparare.");
        
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai riparato il tuo veicolo per $%s (Tasse: $%d).", FormatNumber(taxedPrice), taxedPrice - priceWithoutTaxes);
        Vehicle_Repair(GetPlayerVehicleID(playerid));

        Character_GiveMoney(playerid, -taxedPrice, "paynspray");
        Server_GiveGovernamentBank(taxedPrice - priceWithoutTaxes);
    }


    return 1;
}
alias:riparaveicolo("payspray", "paynspray");

static bool:Character_TryGetPaySpray(playerid, &Building:building)
{
    building = Building:0;
    new Float:x, Float:y, Float:z;
    for_building_of<BUILDING_TYPE_PAYNSPRAY>(b)
    {
        Building_GetEnterPos(b, x, y, z);
        if(GetPlayerDistanceFromPoint(playerid, x, y, z) <= 15.0)
        {
            building = b;
            break;
        }
    }
    return Building_IsValid(building);
}

static SetRepairPricePerHP(Building:building, value)
{
    Building_SetIntData(building, "repair_price", value);
}

static GetRepairPricePerHP(Building:building)
{
    new v = 0;
    Building_GetIntData(building, "repair_price", v);
    if(v <= 0)
        v = 2;
    return v;
}

static GetTaxedRepairPrice(Building:building, value)
{
    new basePrice = GetRepairPricePerHP(building) * value;
    return basePrice + floatround(float(basePrice * Server_GetTaxOnRepairs())/100.0, floatround_ceil);
}

hook function Character_Enter(playerid)
{
    new Building:building, E_ELEMENT_TYPE:type;
    Character_GetNearestPickup(playerid, building, type);
    if(type == ELEMENT_TYPE_BUILDING_ENTRANCE && Building_IsValid(building) && Building_GetType(building) == BUILDING_TYPE_PAYNSPRAY)
        return 0;
    return continue(playerid);
}

hook function Building_ShowTypeMenu(Building:building, playerid)
{
    if(Building_GetType(building) != BUILDING_TYPE_PAYNSPRAY)
        return continue(building, playerid);
    
    new basePrice = GetRepairPricePerHP(building);
    new taxedPrice = GetTaxedRepairPrice(building, 1);
 
    new response[e_DIALOG_RESPONSE_INFO];
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_INPUT, @("Modifica prezzo riparazione"), str_format("Inserisci il nuovo prezzo per HP (1HP = $N).\nPrezzo Attuale: $%d\nPrezzo Finale (compreso di tasse): $%d", basePrice, taxedPrice), @("Modifica"), @("Chiudi"));

        if(!response[E_DIALOG_RESPONSE_Response]) break;

        new newPrice = strval(response[E_DIALOG_RESPONSE_InputText]);
        if(newPrice <= 0) 
        {
            SendClientMessage(playerid, COLOR_ERROR, "Il nuovo prezzo inserito non è valido.");
            continue;
        }

        SetRepairPricePerHP(building, newPrice);
        SendClientMessage(playerid, COLOR_GREEN, "Hai modificato il prezzo del costo della riparazione per HP.");
        SendFormattedMessage(playerid, COLOR_ORANGE, "Prezzo: $%s", FormatNumber(newPrice));
        SendFormattedMessage(playerid, COLOR_ORANGE, "Prezzo finale (compreso di tasse governative): $%s", FormatNumber(GetTaxedRepairPrice(building, 1)));
        break;
    }
    return 1;
}