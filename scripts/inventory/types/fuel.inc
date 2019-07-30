#include <YSI_Coding\y_hooks>

static Selected[MAX_PLAYERS] = {-1, ...};

hook OnCharacterUseItem(playerid, slotid, itemid)
{
    if(!ServerItem_IsFuel(itemid))
        return 0;
    // Search nearest vehicle?
    //new vehicleid = 0;
    //foreach(new i : Vehicle)
    Selected[playerid] = itemid;
    Dialog_Show(playerid, Dialog_SelectVehicleForFuel, DIALOG_STYLE_INPUT, "ID Veicolo", "Inserisci l'id del veicolo che vuoi rifornire.\nUtilizza /dl per conoscerlo.", "Utilizza", "Chiudi");
    return 1;
}

Dialog:Dialog_SelectVehicleForFuel(playerid, response, listitem, inputtext[])   
{
    if(!response)
        return 0;
    if(IsPlayerInAnyVehicle(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo oggetto dall'interno di un veicolo.");
    new itemid = Selected[playerid];
    if(!ServerItem_IsValid(itemid) || !Character_HasItem(playerid, itemid, 1))
        return 0;
    new vehicleid = strval(inputtext);
    if(!IsValidVehicle(vehicleid))
        return Dialog_Show(playerid, Dialog_SelectVehicleForFuel, DIALOG_STYLE_INPUT, "ID Veicolo", "{FF0000}L'id del veicolo inserito non è valido.{FFFFFF}\nInserisci l'id del veicolo che vuoi rifornire.\nUtilizza /dl per conoscerlo.", "Utilizza", "Chiudi");
    if(!IsPlayerInRangeOfVehicle(playerid, vehicleid, 5.0))
        return Dialog_Show(playerid, Dialog_SelectVehicleForFuel, DIALOG_STYLE_INPUT, "ID Veicolo", "{FF0000}Non sei vicino al veicolo inserito.{FFFFFF}\nInserisci l'id del veicolo che vuoi rifornire.\nUtilizza /dl per conoscerlo.", "Utilizza", "Chiudi");
    SendClientMessage(playerid, -1, "Hai rifornito il tuo veicolo di benzina.");
    
    Vehicle_AddFuel(vehicleid, FuelItem_GetAmount(itemid));

    Character_DecreaseItemAmount(playerid, itemid, 1);
    Player_Info(playerid, "Veicolo rifornito.", true, 2500);

    //Character_AMe(playerid, "riempie di benzina il veicolo.");
    return 1;
}
