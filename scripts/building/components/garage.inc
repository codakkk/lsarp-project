
#include <YSI_Coding\y_hooks>
#define CHOPSHOP_X						(2771.7817)
#define CHOPSHOP_Y						(-1606.4154)
#define CHOPSHOP_Z						(10.9219)
#define PRICE_PER_MECHANICAL_COMPONENT  (15)

static const MATERIALS_PER_REPAIR = 2;
static const MATERIALS_PER_SPRAY = 3;

hook OnCharacterTryBuy(playerid)
{
    new id, E_ELEMENT_TYPE:type;
    if(Character_GetNearestPickup(playerid, id, type) && type == ELEMENT_TYPE_CHOPSHOP)
    {
        Garage_BuyMechanicalComponents(playerid);
    }
    return 1;
}

flags:gcmds(CMD_USER);
CMD:gcmds(playerid, params[])
{
    SendClientMessage(playerid, -1, "[GARAGE]: /ripara - /vernicia");
    SendClientMessage(playerid, -1, "Ricorda che puoi acquistare materiali meccanici ad un qualsiasi Chopshop.");
    return 1;
}
alias:gcmds("gcmd");

flags:ripara(CMD_ALIVE_USER);
CMD:ripara(playerid, params[])
{
    if(!Character_HasGarage(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Devi possedere un garage per utilizzare questo comando.");

    if(!Character_CanUseGarage(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei nei pressi del tuo garage.");

    if(Character_GetItemAmount(playerid, gItem_MechanicalComponents) < MATERIALS_PER_REPAIR)
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza materiali meccanici per utilizzare questo comando.");

    new id, price;
    if(sscanf(params, "k<u>d", id, price))
        return SendClientMessage(playerid, COLOR_ERROR, "/ripara <playerid/partofname/#id> <prezzo>");

    if(id == playerid)
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su te stesso.");

    if(!IsPlayerConnected(id) || !Character_IsLogged(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");

    if(!IsPlayerInRangeOfPlayer(playerid, id, 5.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
    
    new vehicleid = GetPlayerVehicleID(id);
    
    if(vehicleid <= 0 || !IsValidVehicle(vehicleid))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è a bordo di un veicolo.");
    
    if(price > Character_GetMoney(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non ha abbastanza soldi.");

    Character_SetRequest(playerid, id, REQUEST_TYPE_REPAIR, 0, price);

    SendFormattedMessage(id, COLOR_RECEIVE_REQUEST, "%s vuole riparare il tuo veicolo (%s) per $%d.", Character_GetRolePlayName(playerid), Vehicle_GetName(vehicleid), price);
    SendClientMessage(id, COLOR_RECEIVE_REQUEST, "Digita \"/accetta riparazione\" per accettare o \"/rifiuta riparazione\" per rifiutare.");
    
    SendFormattedMessage(playerid, COLOR_SEND_REQUEST, "Hai inviato la richiesta di riparazione a %s per $%d. Usa \"annulla riparazione\" per annullare.", Character_GetRolePlayName(id), price);
    return 1;
}

flags:vernicia(CMD_ALIVE_USER);
CMD:vernicia(playerid, params[])
{
    if(!Character_HasGarage(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Devi possedere un garage per utilizzare questo comando.");

    if(!Character_CanUseGarage(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei nei pressi del tuo garage.");

    if(Character_GetItemAmount(playerid, gItem_MechanicalComponents) < MATERIALS_PER_SPRAY)
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza materiali meccanici per utilizzare questo comando.");
    
    new id, price, color1, color2;
    if(sscanf(params, "dddd", id, price, color1, color2))
        return SendClientMessage(playerid, COLOR_ERROR, "/vernicia <playerid/partofname/#id> <prezzo> <colore 1> <colore 2>");
    
    if(color1 < 0 || color1 > 255 || color2 < 0 || color2 > 255)
        return SendClientMessage(playerid, COLOR_ERROR, "Uno o più colori inseriti non sono validi.");
    
    if(id == playerid)
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su te stesso.");

    if(!IsPlayerConnected(id) || !Character_IsLogged(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");

    if(!IsPlayerInRangeOfPlayer(playerid, id, 5.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
    
    new vehicleid = GetPlayerVehicleID(id);
    
    if(vehicleid <= 0 || !IsValidVehicle(vehicleid))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è a bordo di un veicolo.");
    
    if(price > Character_GetMoney(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non ha abbastanza soldi.");

    Character_SetRequest(playerid, id, REQUEST_TYPE_VEHICLE_SPRAY, color1, price, color2);

    SendFormattedMessage(id, COLOR_RECEIVE_REQUEST, "%s vuole verniciare il tuo veicolo (%s) per $%d.", Character_GetRolePlayName(playerid), Vehicle_GetName(vehicleid), price);
    SendClientMessage(id, COLOR_RECEIVE_REQUEST, "Digita \"/accetta verniciatura\" per accettare o \"/rifiuta verniciatura\" per rifiutare.");
    
    SendFormattedMessage(playerid, COLOR_SEND_REQUEST, "Hai inviato la richiesta di verniciatura a %s per $%d.", Character_GetRolePlayName(id), price);

    return 1;
}

stock Character_AcceptVehicleSpray(playerid)
{
    if(!Request_IsPending(playerid, REQUEST_TYPE_VEHICLE_SPRAY))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai una richiesta di riverniciatura attiva."), 0;
    if(!IsPlayerInRangeOfPlayer(playerid, Request_GetSender(playerid, REQUEST_TYPE_VEHICLE_SPRAY), 5.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore."), 0;
    
    new senderid = Request_GetSender(playerid, REQUEST_TYPE_VEHICLE_SPRAY),
        color1 = Request_GetItem(playerid, REQUEST_TYPE_VEHICLE_SPRAY),
        color2 = Request_GetSlot(playerid, REQUEST_TYPE_VEHICLE_SPRAY),
        price = Request_GetAmount(playerid, REQUEST_TYPE_VEHICLE_SPRAY),
        vehicleid = GetPlayerVehicleID(playerid)
    ;

    if(!Character_CanInteract(senderid))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non puo' riverniciarti il veicolo al momento.");

    Character_GiveMoney(playerid, -price, "spray");
    Character_GiveMoney(senderid, price, "spray");
    Character_DecreaseItemAmount(senderid, gItem_MechanicalComponents, MATERIALS_PER_SPRAY);
    
    SendFormattedMessage(senderid, COLOR_ACCEPT_REQUEST, "%s ha accettato la tua richiesta di verniciatura. Hai guadagnato $%d.", Character_GetRolePlayName(playerid), price);
    SendFormattedMessage(senderid, COLOR_ACCEPT_REQUEST, "La verniciatura ti è costata: %d materiali meccanici.", MATERIALS_PER_SPRAY);
    SendFormattedMessage(playerid, COLOR_ACCEPT_REQUEST, "%s ha verniciato il tuo veicolo (%s) per $%d.", Character_GetRolePlayName(senderid), Vehicle_GetName(vehicleid), price);
    
    Vehicle_SetColors(vehicleid, color1, color2);

    ResetPendingRequest(playerid, REQUEST_TYPE_VEHICLE_SPRAY);
    ResetPendingRequest(senderid, REQUEST_TYPE_VEHICLE_SPRAY);
    return 1;
}

stock Character_AcceptRepair(playerid)
{
    if(!Request_IsPending(playerid, REQUEST_TYPE_REPAIR))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai una richiesta di riparazione attiva."), 0;
    
    if(!IsPlayerInRangeOfPlayer(playerid, Request_GetSender(playerid, REQUEST_TYPE_REPAIR), 5.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore."), 0;


    new senderid = Request_GetSender(playerid, REQUEST_TYPE_REPAIR),
        price = Request_GetAmount(playerid, REQUEST_TYPE_REPAIR),
        vehicleid = GetPlayerVehicleID(playerid)
    ;

    if(!Character_CanInteract(senderid))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non puo' ripararti il veicolo al momento.");

    Character_GiveMoney(playerid, -price, "repair");
    Character_GiveMoney(senderid, price, "repair");

    Character_DecreaseItemAmount(senderid, gItem_MechanicalComponents, MATERIALS_PER_REPAIR);
    
    SendFormattedMessage(senderid, COLOR_ACCEPT_REQUEST, "%s ha accettato la tua richiesta di riparazione. Hai guadagnato $%d.", Character_GetRolePlayName(playerid), price);
    SendFormattedMessage(senderid, COLOR_ACCEPT_REQUEST, "La riparazione ti è costata: %d materiali meccanici.", MATERIALS_PER_REPAIR);

    SendFormattedMessage(playerid, COLOR_ACCEPT_REQUEST, "%s ha riparato il tuo veicolo (%s) per $%d.", Character_GetRolePlayName(senderid), Vehicle_GetName(vehicleid), price);

    RepairVehicle(vehicleid);
    SetVehicleHealth(vehicleid, 999.0);

    ResetPendingRequest(playerid, REQUEST_TYPE_REPAIR);
    ResetPendingRequest(senderid, REQUEST_TYPE_REPAIR);
    return 1;
}

stock Garage_BuyMechanicalComponents(playerid)
{
    if(!IsPlayerInRangeOfPoint(playerid, 5.0, CHOPSHOP_X, CHOPSHOP_Y, CHOPSHOP_Z))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei nei pressi di un Chopshop.");
    return Dialog_Show(playerid, Dialog_BuyMechComponents, DIALOG_STYLE_INPUT, "Acquista", "Inserisci la quantità di materiali meccanici che vuoi acquistare.\nCosto per prezzo: $%d.", "Acquista", "Chiudi", PRICE_PER_MECHANICAL_COMPONENT);
}

Dialog:Dialog_BuyMechComponents(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    if(!IsPlayerInRangeOfPoint(playerid, 5.0, CHOPSHOP_X, CHOPSHOP_Y, CHOPSHOP_Z))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei nei pressi di un Chopshop.");
    new val = strval(inputtext);
    if(val <= 0)
        return Dialog_Show(playerid, Dialog_BuyMechComponents, DIALOG_STYLE_INPUT, "Acquista", "{FF0000}La quantità inserita non è valida.{FFFFFF}\nInserisci la quantità di materiali meccanici che vuoi acquistare.\nCosto per prezzo: $%d.", "Acquista", "Chiudi", PRICE_PER_MECHANICAL_COMPONENT);
    new price = PRICE_PER_MECHANICAL_COMPONENT * val;
    if(price > Character_GetMoney(playerid))
        return Dialog_Show(playerid, Dialog_BuyMechComponents, DIALOG_STYLE_INPUT, "Acquista", "{FF0000}Non hai abbastanza soldi per acquistare %d materiali.{FFFFFF}\nInserisci la quantità di materiali meccanici che vuoi acquistare.\nCosto per prezzo: $%d.", "Acquista", "Chiudi", val, PRICE_PER_MECHANICAL_COMPONENT);
    new result = Character_GiveItem(playerid, gItem_MechanicalComponents, val, 0, true);
    if(result == INVENTORY_NO_SPACE)
        return Dialog_Show(playerid, Dialog_BuyMechComponents, DIALOG_STYLE_INPUT, "Acquista", "{FF0000}Non hai abbastanza spazio per %d materiali.{FFFFFF}\nInserisci la quantità di materiali meccanici che vuoi acquistare.\nCosto per prezzo: $%d.", "Acquista", "Chiudi", val, PRICE_PER_MECHANICAL_COMPONENT);
    Character_GiveMoney(playerid, -price, "chop_shop_mats");
    SendFormattedMessage(playerid, -1, "Hai acquistato %d materiali per $%d.", val, price);
    return 1;
}

stock Character_HasGarage(playerid)
{
    new buildingid = Character_GetBuildingKey(playerid);
    
    if(!Building_IsValid(buildingid))
        return 0;
    
    if(Building_GetType(buildingid) != BUILDING_TYPE_GARAGE)
        return 0;
    
    return 1;
}

stock Character_CanUseGarage(playerid)
{
    if(!Character_HasGarage(playerid))
        return 0;
    
    new garageid = Character_GetBuildingKey(playerid);
    
    if(Character_GetNearBuildingID(playerid) == garageid)
        return 1;
    
    new Float:x, Float:y, Float:z;
    Building_GetEnterPos(garageid, x, y, z);
    if(GetPlayerDistanceFromPoint(playerid, x, y, z) <= 25.0)
        return 1;
    return 0;
}