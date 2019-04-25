#include <YSI_Coding\y_hooks>

flags:vmenu(CMD_USER);
CMD:vmenu(playerid, params[])
{
    if(gBuyingVehicle[playerid])
        return 1;
    new 
        current_vehicle = GetPlayerVehicleID(playerid),
        string[1024],
        count = 0;
    memset(pVehicleListItem[playerid], 0);
    foreach(new v : Vehicles)
    {
        if(VehicleInfo[v][vOwnerID] != PlayerInfo[playerid][pID])
            continue;
        new Float:x, Float:y, Float:z;
        GetVehiclePos(v, x , y, z);
        if(v == current_vehicle)
        {
            format(string, sizeof(string), "%s%s (Attuale)\n", string, GetVehicleName(v));
        }
        else if(IsPlayerInRangeOfPoint(playerid, 5.0, x, y, z))
        {
            format(string, sizeof(string), "%s%s (Vicino)\n", string, GetVehicleName(v));
        }
        else
        {
            format(string, sizeof(string), "%s%s\n", string, GetVehicleName(v));
        }
        pVehicleListItem[playerid][count] = v;
        count++;
    }
    if(count == 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non possiedi veicoli!");
    Dialog_Show(playerid, Dialog_VehicleList, DIALOG_STYLE_LIST, "I miei veicoli", string, "Avanti", "Chiudi");
    return 1;
}

flags:vparcheggia(CMD_USER);
CMD:vparcheggia(playerid, params[])
{
	new vehicleid = GetPlayerVehicleID(playerid);
	if(vehicleid <= 0 || Vehicle_GetOwnerID(vehicleid) != Character_GetID(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un tuo veicolo!");

	new 
		Float:x, Float:y, Float:z, Float:a;
	GetVehiclePos(vehicleid, x, y, z);
	GetVehicleZAngle(vehicleid, a);
	Vehicle_Park(vehicleid, x, y, z, a);
	SendFormattedMessage(playerid, COLOR_GREEN, "Hai parcheggiato qui la tua %s.", GetVehicleName(vehicleid));
	return 1;
}
alias:vparcheggia("vpark");

flags:vchiudi(CMD_USER);
CMD:vchiudi(playerid, params[])
{
    new vehicleid;
    if(sscanf(params, "d", vehicleid))
    {
        vehicleid = GetClosestVehicle(playerid, 3.5);
    }
    else
    {
        if(!IsPlayerInRangeOfVehicle(playerid, vehicleid, 3.5))
            return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino a questo veicolo!");
    }
    if(vehicleid == 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non ci sono veicoli nelle vicinanze!");
    if(VehicleInfo[vehicleid][vOwnerID] != PlayerInfo[playerid][pID])
        return SendClientMessage(playerid, COLOR_ERROR, "Questo veicolo non è tuo!");
    if(VehicleInfo[vehicleid][vLocked])
        return SendClientMessage(playerid, -1, "Il veicolo è già chiuso!");
    if((IsABike(vehicleid) || IsAMotorBike(vehicleid)) && Vehicle_IsEngineOn(vehicleid))
    {
        return SendClientMessage(playerid, -1, "Prima spegni il motore!");
    }
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai chiuso il tuo veicolo (%s).", GetVehicleName(vehicleid));
    if(IsABike(vehicleid) || IsAMotorBike(vehicleid))
    {
        //ApplyAnimation(playerid, "BOMBER", "BOM_Plant_Crouch_In", 4.1, 0, 0, 0, 0, 1000, 0);
        Character_AMe(playerid, "prende la catena e l'attacca al veicolo");
    }
    else
    {
        Character_AMe(playerid, "prende le chiavi e chiude il suo veicolo");
    }
    Vehicle_Lock(vehicleid);
    return 1;
}

flags:vapri(CMD_USER);
CMD:vapri(playerid, params[])
{
    new vehicleid;
    if(sscanf(params, "d", vehicleid))
    {
        vehicleid = GetClosestVehicle(playerid, 3.5);
    }
    else
    {
        new Float:x, Float:y, Float:z;
        GetVehiclePos(vehicleid, x, y, z);
        if(!IsPlayerInRangeOfPoint(playerid, 3.5, x, y, z))
            return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino a questo veicolo!");
    }
    if(vehicleid == 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non ci sono veicoli nelle vicinanze!");
    if(VehicleInfo[vehicleid][vOwnerID] != PlayerInfo[playerid][pID])
        return SendClientMessage(playerid, COLOR_ERROR, "Questo veicolo non è tuo!");
    if(!VehicleInfo[vehicleid][vLocked])
        return SendClientMessage(playerid, -1, "Il veicolo è già aperto!");
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai aperto il tuo veicolo (%s).", GetVehicleName(vehicleid));
    if(IsABike(vehicleid) || IsAMotorBike(vehicleid))
    {
        ApplyAnimation(playerid, "BOMBER", "BOM_Plant_Crouch_In", 4.1, 0, 0, 0, 0, 1000, 0);
        Character_AMe(playerid, "toglie la catena dal suo veicolo");
    }
    else
    {
        Character_AMe(playerid, "prende le chiavi e apre il suo veicolo");
    }
    Vehicle_UnLock(vehicleid);
    return 1;
}

flags:vbagagliaio(CMD_USER);
CMD:vbagagliaio(playerid, params[])
{
    new vehicleid;
    if(sscanf(params, "d", vehicleid))
    {
        vehicleid = GetClosestVehicle(playerid, 3.5);
    }
    else
    {
        if(!IsPlayerInRangeOfVehicle(playerid, vehicleid, 3.5))
            return SendClientMessage(playerid, COLOR_GREEN, "Non sei vicino al veicolo!");
    }
    /*if(Vehicle_IsTrunkOpened(vehicleid))
        Vehicle_CloseTrunk(vehicleid);
    else
        Vehicle_OpenTrunk(vehicleid);*/
    Vehicle_ShowInventory(vehicleid, playerid);
    return 1;
}
alias:vbagagliaio("vtrunk");

flags:motore(CMD_USER);
CMD:motore(playerid, params[])
{
    new vehicleid = GetPlayerVehicleID(playerid);
    if(vehicleid == 0 || GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo!");
    if(Vehicle_IsEngineOn(vehicleid))
    {
        Vehicle_SetEngineOff(vehicleid);
        Character_AMe(playerid, "gira la chiave e spegne il motore");
    }
    else
    {
		if(Vehicle_GetFuel(vehicleid) < 0.5)
			return SendClientMessage(playerid, COLOR_ERROR, "Non c'è benzina nel veicolo!");
		if(Vehicle_GetHealth(vehicleid) <= 350.0)
			return SendClientMessage(playerid, COLOR_ERROR, "Il motore del veicolo non funziona!");
        if(VehicleInfo[vehicleid][vOwnerID] == PlayerInfo[playerid][pID])
        {
            if((IsABike(vehicleid) || IsAMotorBike(vehicleid)) && VehicleInfo[vehicleid][vLocked])
            {
                return SendClientMessage(playerid, COLOR_ERROR, "Il veicolo ha la catena!");
            }
            Character_AMe(playerid, "inserisce la chiave e la gira");
			Character_SetFreezed(playerid, true);
			defer TurnOnVehicleEngine(playerid);
        }
        else
        {
            SendClientMessage(playerid, COLOR_ERROR, "Non hai le chiavi di questo veicolo!");
        }
    }
    return 1;
}

timer TurnOnVehicleEngine[500](playerid)
{
	new vehicleid = GetPlayerVehicleID(playerid);
	if(vehicleid > 0)
	{
		new Float:vh = Vehicle_GetHealth(vehicleid), vr = 0;
		if(vh < 300.0) vr = 2;
		else if(vh < 400) vr = 3;
		else if(vh < 500) vr = 4;
		else if(vh < 600) vr = 5;
		else if(vh < 700) vr = 6;
		else if(vh < 800) vr = 7;
		else if(vh < 900) vr = 8;
		else vr = 9;
		vr *= 2;
		new r = random(vr);
		if(r == 0)
		{
			Character_Do(playerid, "Il motore non si accende");
			Vehicle_SetEngineOff(vehicleid);
		}
		else
		{
			Character_Do(playerid, "Il motore si accende");
			Vehicle_SetEngineOn(vehicleid);
		}
	}
	Character_SetFreezed(playerid, false);
	Character_SetTurningOnEngine(playerid, false);
}

Dialog:Dialog_VehicleList(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    new 
        vehicle_id = pVehicleListItem[playerid][listitem];
    if(vehicle_id == 0 || VehicleInfo[vehicle_id][vID] == 0 || VehicleInfo[vehicle_id][vOwnerID] != PlayerInfo[playerid][pID])
        return 0;
    pSelectedVehicleListItem[playerid] = vehicle_id;
    new string[64];
    format(string, sizeof(string), "%s", GetVehicleName(vehicle_id));
    Dialog_Show(playerid, Dialog_VehicleAction, DIALOG_STYLE_LIST, string, "Apri/Chiudi\nParcheggia\nBagagliaio\nVendi a giocatore\nVendi\nInfo", "Avanti", "Annulla");
    return 1;
}

Dialog:Dialog_VehicleAction(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    new vehicle_id = pSelectedVehicleListItem[playerid];
    switch(listitem)
    {
        case 0: // Open/Close
        {
            if(!IsPlayerInRangeOfVehicle(playerid, vehicle_id, 5.0))
                return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al veicolo!");
            if(Vehicle_IsLocked(vehicle_id))
            {
                if(IsABike(vehicle_id) || IsAMotorBike(vehicle_id))
                {
                    Character_AMe(playerid, "toglie la catena dal suo veicolo");
                }
                else
                {
                    Character_AMe(playerid, "prende le chiavi e apre il suo veicolo");
                }
                SendClientMessage(playerid, COLOR_GREEN, "Hai aperto il tuo veicolo!");
                Vehicle_UnLock(vehicle_id);
            }
            else
            {
                if(IsABike(vehicle_id) || IsAMotorBike(vehicle_id))
                {
                    Character_AMe(playerid, "prende la catena e l'attacca al veicolo");
                }
                else
                {
                    Character_AMe(playerid, "prende le chiavi e chiude il suo veicolo");
                }
                SendClientMessage(playerid, COLOR_GREEN, "Hai chiuso il tuo veicolo!");
                Vehicle_UnLock(vehicle_id);
            }
            
        }
        case 1: // Park
        {
            if(GetPlayerVehicleID(playerid) != vehicle_id)
                return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo del veicolo!");
            new 
                Float:x, Float:y, Float:z, Float:a;
            GetVehiclePos(vehicle_id, x, y, z);
            GetVehicleZAngle(vehicle_id, a);
            Vehicle_Park(vehicle_id, x, y, z, a);
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai parcheggiato qui la tua %s.", GetVehicleName(vehicle_id));
        }
        case 2: // Trunk
        {
            if(!IsPlayerInRangeOfVehicle(playerid, vehicle_id, 5.0))
                return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al veicolo!");
            Vehicle_ShowInventory(vehicle_id, playerid);
        }
        case 3: // Sell to player
        {
            if(!IsPlayerInRangeOfVehicle(playerid, vehicle_id, 5.0))
                return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al veicolo!");
            Dialog_Show(playerid, Dialog_SellToPlayer, DIALOG_STYLE_INPUT, "Vendi a giocatore", "Inserisci l'ID o il nome del giocatore\n a cui vuoi vendere il veicolo\nseguito dal prezzo.\nEsempio: {00FF00}Mario Rossi {FF0000}10000.", "Vendi", "Annulla");
        }
        case 4: // Sell
        {
            if(!IsPlayerInRangeOfVehicle(playerid, vehicle_id, 5.0))
                return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al veicolo!");
        }
		case 5: // Info
		{
			new Float:health;
			GetVehicleHealth(vehicle_id, health);
			SendFormattedMessage(playerid, -1, "ID Veicolo: {00FF00}%d{FFFFFF} - HP: {00FF00}%.2f{FFFFFF}", vehicle_id, health);
		}
    }
    return 1;
}

Dialog:Dialog_SellToPlayer(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    new otherPlayer, price;
    if(sscanf(inputtext, "ud", otherPlayer, price))
        return Dialog_Show(playerid, Dialog_SellToPlayer, DIALOG_STYLE_INPUT, "Vendi a giocatore", "{FF0000}I dati inseriti non sono validi!\n{FFFFFF}Inserisci l'ID o il nome del giocatore\n a cui vuoi vendere il veicolo\nseguito dal prezzo.\nEsempio: {00FF00}Mario Rossi {FF0000}10000.", "Vendi", "Annulla");
    
    if(!IsPlayerConnected(otherPlayer) || !Character_IsLogged(otherPlayer) || otherPlayer == playerid)
        return Dialog_Show(playerid, Dialog_SellToPlayer, DIALOG_STYLE_INPUT, "{FFFFFF}Vendi a giocatore", "{FF0000}Il giocatore non è connesso!\n{FFFFFF}Inserisci l'ID o il nome del giocatore\n a cui vuoi vendere il veicolo\nseguito dal prezzo.\nEsempio: {00FF00}Mario Rossi {FF0000}10000.", "Vendi", "Annulla");
    
    if(price < 0)
        return Dialog_Show(playerid, Dialog_SellToPlayer, DIALOG_STYLE_INPUT, "{FFFFFF}Vendi a giocatore", "{FF0000}Il prezzo inserito non è valido!\nInserisci l'ID o il nome del giocatore\n a cui vuoi vendere il veicolo\nseguito dal prezzo.\nEsempio: {00FF00}Mario Rossi {FF0000}10000.", "Vendi", "Annulla");
    
    if(Character_GetMoney(otherPlayer) < price)
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non ha abbastanza soldi!");
    
    if(!IsPlayerInRangeOfPlayer(playerid, otherPlayer, 6.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore!");
    
    new vehicle_id = pSelectedVehicleListItem[playerid];
    pVehicleSellingTo[playerid] = otherPlayer;
    pVehicleSeller[otherPlayer] = playerid;
    pSellingVehicleID[playerid] = pSellingVehicleID[otherPlayer] = vehicle_id;
    pVehicleSellingPrice[playerid] = pVehicleSellingPrice[otherPlayer] = price;

    SendFormattedMessage(playerid, COLOR_GREEN, "Hai proposto a %s (%d) il tuo veicolo (%s) per $%d.", Character_GetOOCName(otherPlayer), otherPlayer, GetVehicleName(vehicle_id), price);
    SendFormattedMessage(otherPlayer, COLOR_GREEN, "%s (%d) vuole venderti il suo veicolo (%s) per $%d. Digita /accetta veicolo per accettare.", Character_GetOOCName(playerid), playerid, GetVehicleName(vehicle_id), price);
    return 1;
}