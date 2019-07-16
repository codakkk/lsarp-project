
#include <YSI_Coding\y_hooks>
#define CHOPSHOP_X						(2771.7817)
#define CHOPSHOP_Y						(-1606.4154)
#define CHOPSHOP_Z						(10.9219)
#define PRICE_PER_MECHANICAL_COMPONENT  (15)

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
    new garageid = Character_GetGarage(playerid);
    if(garageid == -1)
        return SendClientMessage(playerid, COLOR_ERROR, "Devi possedere un garage per utilizzare questo comando.");
    if(Character_GetNearBuildingID(playerid) != garageid)
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'interno del tuo garage.");
    if(Character_GetItemAmount(playerid, gItem_MechanicalComponents) < 2)
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza materiali meccanici per utilizzare questo comando.");
    new vehicleid;
    if(sscanf(params, "d", vehicleid))
        return SendClientMessage(playerid, COLOR_ERROR, "/ripara <vehicleid>");
    Character_DecreaseItemAmount(playerid, gItem_MechanicalComponents, 2);
    
    SendClientMessage(playerid, -1, "Hai riparato il veicolo id %d utilizzando 2 materiali.");
    RepairVehicle(vehicleid);
    SetVehicleHealth(vehicleid, 999.0);
    return 1;
}

flags:vernicia(CMD_ALIVE_USER);
CMD:vernicia(playerid, params[])
{
    new garageid = Character_GetGarage(playerid);
    if(garageid == -1)
        return SendClientMessage(playerid, COLOR_ERROR, "Devi possedere un garage per utilizzare questo comando.");
    if(Character_GetNearBuildingID(playerid) != garageid)
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'interno del tuo garage.");
    if(Character_GetItemAmount(playerid, gItem_MechanicalComponents) < 3)
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza materiali meccanici per utilizzare questo comando.");
    new vehicleid, color1, color2;
    if(sscanf(params, "ddd", vehicleid, color1, color2))
        return SendClientMessage(playerid, COLOR_ERROR, "/vernicia <vehicleid> <colore 1> <colore 2>");
    if(color1 < 0 || color1 > 255 || color2 < 0 || color2 > 255)
        return SendClientMessage(playerid, COLOR_ERROR, "Uno o più colori inseriti non sono validi.");
    
    Character_DecreaseItemAmount(playerid, gItem_MechanicalComponents, 3);
    
    SendClientMessage(playerid, -1, "Hai riverniciato il veicolo id %d utilizzando 3 materiali.", vehicleid);
    Vehicle_SetColors(vehicleid, color1, color2);
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

// Returns a garageid (buildingid) if any.
stock Character_GetGarage(playerid)
{
    foreach(new b : Buildings)
    {
        if(Building_GetType(b) != BUILDING_TYPE_GARAGE)
            continue;
        if(Building_GetOwnerID(b) == Character_GetID(playerid))
            return b;
    }
    return -1;
}