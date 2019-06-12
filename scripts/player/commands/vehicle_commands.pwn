#include <YSI_Coding\y_hooks>

static 
	pVehicleListItem[MAX_PLAYERS][MAX_VEHICLES_PER_PLAYER],
	pSelectedVehicleListItem[MAX_PLAYERS]
;


flags:vmenu(CMD_ALIVE_USER);
CMD:vmenu(playerid, params[])
{
    if(gBuyingVehicle[playerid])
        return 1;
    new 
        current_vehicle = GetPlayerVehicleID(playerid),
        string[1024],
        count = 0;
    memset(pVehicleListItem[playerid], 0);
    foreach(new v : Vehicle)
    {
        if(VehicleInfo[v][vOwnerID] != PlayerInfo[playerid][pID])
            continue;
        new Float:x, Float:y, Float:z;
        GetVehiclePos(v, x , y, z);
        if(v == current_vehicle)
        {
            format(string, sizeof(string), "%s%s (Attuale)\n", string, Vehicle_GetName(v));
        }
        else if(IsPlayerInRangeOfPoint(playerid, 5.0, x, y, z))
        {
            format(string, sizeof(string), "%s%s (Vicino)\n", string, Vehicle_GetName(v));
        }
        else
        {
            format(string, sizeof(string), "%s%s\n", string, Vehicle_GetName(v));
        }
        pVehicleListItem[playerid][count] = v;
        count++;
    }
    if(count == 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non possiedi veicoli.");
    Dialog_Show(playerid, Dialog_VehicleList, DIALOG_STYLE_LIST, "I miei veicoli", string, "Avanti", "Chiudi");
    return 1;
}

flags:finestrino(CMD_ALIVE_USER);
CMD:finestrino(playerid, params[])
{
	new vehicleid = GetPlayerVehicleID(playerid);
	if(vehicleid <= 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei in un veicolo.");
	if(IsABike(vehicleid) || IsAMotorBike(vehicleid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su questa tipologia di veicolo.");
	
	new w[4];
	new window = GetPlayerVehicleSeat(playerid);
	
	GetVehicleParamsCarWindows(vehicleid, w[0], w[1], w[2], w[3]);

	if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
	{
		if(strlen(params) > 0)
			window = strval(params);
		else
			return SendClientMessage(playerid, COLOR_ERROR, "(/fin)estrino <0 - 3>");
	} 

	if(window < 0 || window > 3)return
	    SendClientMessage(playerid, COLOR_ERROR, "Finestrino inesistente. (0 - 3)");
	w[window] = !w[window];
	SetVehicleParamsCarWindows(vehicleid, w[0], w[1], w[2], w[3]);
	Character_AMe(playerid, w[window] ? ("chiude il finestrino") : ("apre il finestrino"));
	return 1;
}
alias:finestrino("fin");

flags:vluci(CMD_ALIVE_USER);
CMD:vluci(playerid, params[])
{
	new vehicleid = GetPlayerVehicleID(playerid);
	if(vehicleid > 0)
	{
		if(!Vehicle_IsEngineOn(vehicleid))
			return SendClientMessage(playerid, COLOR_ERROR, "Il veicolo è spento.");
		Vehicle_SetLightState(vehicleid, !Vehicle_IsLightOn(vehicleid));
	}
	else return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo.");
	return 1;
}

flags:vparcheggia(CMD_ALIVE_USER);
CMD:vparcheggia(playerid, params[])
{
	new vehicleid = GetPlayerVehicleID(playerid);
	if(vehicleid <= 0 || Vehicle_IsOwner(vehicleid, playerid, true))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un tuo veicolo.");

	new 
		Float:x, Float:y, Float:z, Float:a;
	GetVehiclePos(vehicleid, x, y, z);
	GetVehicleZAngle(vehicleid, a);
	Vehicle_Park(vehicleid, x, y, z, a);
	SendFormattedMessage(playerid, COLOR_GREEN, "Hai parcheggiato qui la tua %s.", Vehicle_GetName(vehicleid));
	return 1;
}
alias:vparcheggia("vpark");

flags:vchiudi(CMD_ALIVE_USER);
CMD:vchiudi(playerid, params[])
{
    new vehicleid;
    if(sscanf(params, "d", vehicleid))
    {
        vehicleid = Character_GetClosestVehicle(playerid, 3.5);
    }
    else
    {
        if(!IsPlayerInRangeOfVehicle(playerid, vehicleid, 3.5))
            return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino a questo veicolo.");
    }
    if(vehicleid == 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non ci sono veicoli nelle vicinanze.");
    /*if((IsABike(vehicleid) || IsAMotorBike(vehicleid)) && Vehicle_IsEngineOn(vehicleid))
    {
        return SendClientMessage(playerid, -1, "Prima spegni il motore.");
    }*/
	if(Vehicle_IsOwner(vehicleid, playerid, false))
	{
		if(Vehicle_IsLocked(vehicleid))
			return SendClientMessage(playerid, -1, "Il veicolo è già chiuso.");
		/*if(IsABike(vehicleid) || IsAMotorBike(vehicleid))
		{
			//ApplyAnimation(playerid, "BOMBER", "BOM_Plant_Crouch_In", 4.1, 0, 0, 0, 0, 1000, 0);
			Character_AMe(playerid, "prende la catena e l'attacca al veicolo");
		}
		else
		{
			Character_AMe(playerid, "prende le chiavi e chiude il suo veicolo");
		}*/
		Character_AMe(playerid, "prende le chiavi e chiude il suo veicolo");
		Vehicle_Lock(vehicleid);
		return SendFormattedMessage(playerid, COLOR_GREEN, "Hai chiuso il tuo veicolo (%s).", Vehicle_GetName(vehicleid));
	}
	else return SendClientMessage(playerid, COLOR_ERROR, "Questo veicolo non è tuo.");
}

flags:vapri(CMD_ALIVE_USER);
CMD:vapri(playerid, params[])
{
	new vehicleid;
	if(sscanf(params, "d", vehicleid))
	{
		vehicleid = Character_GetClosestVehicle(playerid, 3.5);
	}
	else
	{
		new Float:x, Float:y, Float:z;
		GetVehiclePos(vehicleid, x, y, z);
		if(!IsPlayerInRangeOfPoint(playerid, 3.5, x, y, z))
			return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino a questo veicolo.");
	}
	if(vehicleid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non ci sono veicoli nelle vicinanze.");
	
	if(Vehicle_IsOwner(vehicleid, playerid, false))
	{
		if(!Vehicle_IsLocked(vehicleid))
			return SendClientMessage(playerid, -1, "Il veicolo è già aperto.");
		
		/*if(IsABike(vehicleid) || IsAMotorBike(vehicleid))
		{
			//ApplyAnimation(playerid, "BOMBER", "BOM_Plant_Crouch_In", 4.1, 0, 0, 0, 0, 1000, 0);
			Character_AMe(playerid, "toglie la catena dal suo veicolo");
		}
		else
		{
			Character_AMe(playerid, "prende le chiavi e apre il suo veicolo");
		}*/
		Character_AMe(playerid, "prende le chiavi e apre il suo veicolo.");
		Vehicle_UnLock(vehicleid);

		return SendFormattedMessage(playerid, COLOR_GREEN, "Hai aperto il tuo veicolo (%s).", Vehicle_GetName(vehicleid));
	}
	return SendClientMessage(playerid, COLOR_ERROR, "Questo veicolo non è tuo.");
}

flags:vbagagliaio(CMD_ALIVE_USER);
CMD:vbagagliaio(playerid, params[])
{
	new vehicleid;
	if(sscanf(params, "d", vehicleid))
	{
		vehicleid = Character_GetClosestVehicle(playerid, 3.5);
	}
	else
	{
		if(!IsPlayerInRangeOfVehicle(playerid, vehicleid, 3.5))
			return SendClientMessage(playerid, COLOR_GREEN, "Non sei vicino al veicolo.");
	}
	if(vehicleid == 0 || !Vehicle_IsValid(vehicleid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino ad un veicolo.");
	if(!Vehicle_IsOwner(vehicleid, playerid, false))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei il proprietario di questo veicolo.");
	/*if(Vehicle_IsTrunkOpened(vehicleid))
		Vehicle_CloseTrunk(vehicleid);
	else
		Vehicle_OpenTrunk(vehicleid);*/
	Vehicle_ShowInventory(vehicleid, playerid);
	return 1;
}
alias:vbagagliaio("vtrunk", "vinventario", "vinv");

flags:vdeposita(CMD_ALIVE_USER);
CMD:vdeposita(playerid, params[])
{
	if(Request_IsPending(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta attiva.");
	
	new vehicleid;
	if(sscanf(params, "d", vehicleid))
	{
		vehicleid = Character_GetClosestVehicle(playerid, 3.5);
	}
	else
	{
		if(!Vehicle_IsValid(vehicleid))
			return SendClientMessage(playerid, COLOR_ERROR, "Veicolo non valido.");

		if(!IsPlayerInRangeOfVehicle(playerid, vehicleid, 3.5))
			return SendClientMessage(playerid, COLOR_GREEN, "Non sei vicino al veicolo.");
	}

	if(!Vehicle_IsOwner(vehicleid, playerid, false))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei il proprietario di questo veicolo.");

	new itemid = AC_GetPlayerWeapon(playerid);
	if(itemid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando senza un'arma.");
	
	new ammo = AC_GetPlayerAmmo(playerid);
	if(Weapon_IsGrenade(itemid))
	{
		if(!Vehicle_HasSpaceForItem(vehicleid, itemid, ammo))
			return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nel veicolo.");
		Character_GiveItem(playerid, itemid, ammo);
		Vehicle_GiveItem(vehicleid, itemid, ammo);
		Player_InfoStr(playerid, str_format("Hai depositato ~g~%d ~y~%s~w~~n~nel veicolo.", ammo, Weapon_GetName(itemid)), true);
	}
	else
	{
		if(!Vehicle_HasSpaceForItem(vehicleid, itemid, 1))
			return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nel veicolo.");
		Vehicle_GiveItem(vehicleid, itemid, 1, ammo);
		Player_InfoStr(playerid, str_format("Hai depositato ~g~%s~w~~n~nel veicolo~n~con %d munizioni.", Weapon_GetName(itemid), ammo), true);
	}
	Character_AMe(playerid, "deposita qualcosa all'interno del veicolo.");
	AC_RemovePlayerWeapon(playerid, itemid);
	return 1;
}
alias:vdeposita("vdep");

flags:vdisassembla(CMD_ALIVE_USER);
CMD:vdisassembla(playerid, params[])
{
	if(Request_IsPending(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta attiva.");
	new 
		slotid, itemid, ammo;
	new vehicleid = Character_GetClosestVehicle(playerid, 3.0);
	if(vehicleid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino ad un veicolo.");
	if(!Vehicle_IsOwner(vehicleid, playerid, false))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei il proprietario di questo veicolo.");
	if(sscanf(params, "d", slotid))
	{
		itemid = AC_GetPlayerWeapon(playerid);
		ammo = AC_GetPlayerAmmo(playerid);
		if(itemid == 0 || ammo == 0)
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando senza un'arma.");
			return SendClientMessage(playerid, COLOR_ERROR, "Altrimenti usa (/vdis)assembla <slotid> per disassemblare un'arma che hai nell'inventario.");
		}
		if(!Weapon_CanBeDisassembled(itemid))
			return SendClientMessage(playerid, COLOR_ERROR, "Non puoi disassemblare quest'arma.");
		new data[10], amounts[10];
		data[0] = itemid;
		data[1] = Weapon_GetAmmoType(itemid);
		amounts[0] = 1;
		amounts[1] = ammo;
		if(!Inventory_HasSpaceForItems(Vehicle_GetInventory(vehicleid), data, amounts))
			return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario.");
		AC_RemovePlayerWeapon(playerid, itemid);
	}
	else
	{	
		if(!Character_IsValidSlot(playerid, slotid))
			return SendClientMessage(playerid, COLOR_ERROR, "Slot non valida.");
		itemid = Character_GetSlotItem(playerid, slotid);
		ammo = Character_GetSlotExtra(playerid, slotid);
		if(ServerItem_GetType(itemid) != ITEM_TYPE_WEAPON)
			return SendClientMessage(playerid, COLOR_ERROR, "L'oggetto selezionato non è un'arma.");
		if(!Weapon_CanBeDisassembled(itemid))
			return SendClientMessage(playerid, COLOR_ERROR, "Non puoi disassemblare quest'arma.");
		if(ammo == 0)
			return SendClientMessage(playerid, COLOR_ERROR, "L'arma selezionata è già disassemblata.");
		new data[10], amounts[10];
		data[0] = itemid;
		data[1] = Weapon_GetAmmoType(itemid);
		amounts[0] = 1;
		amounts[1] = ammo;
		if(!Inventory_HasSpaceForItems(Vehicle_GetInventory(vehicleid), data, amounts))
			return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario.");
		Character_DecreaseSlotAmount(playerid, slotid, 1);
	}
	Vehicle_GiveItem(vehicleid, itemid, 1);
	Vehicle_GiveItem(vehicleid, Weapon_GetAmmoType(itemid), ammo);
	SendFormattedMessage(playerid, COLOR_GREEN, "Hai disassemblato la tua arma (%s) all'interno del veicolo. Munizioni: %d", ServerItem_GetName(itemid), ammo);
	Character_AMe(playerid, "disassembla un'arma e la deposita nel veicolo.");
	return 1;
}
alias:vdisassembla("vdis");

/*flags:vusa(CMD_ALIVE_USER);
CMD:vusa(playerid, params[])
{
	new slotid;
	if(sscanf(params, "d", slotid))
		return SendClientMessage(playerid, COLOR_ERROR, "/vusa <slotid>");
	return 1;
}*/

flags:motore(CMD_ALIVE_USER);
CMD:motore(playerid, params[])
{
    new vehicleid = GetPlayerVehicleID(playerid);
    if(vehicleid == 0 || GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo.");
    if(Vehicle_IsEngineOn(vehicleid))
    {
        Vehicle_SetEngineOff(vehicleid);
		Vehicle_SetLightState(vehicleid, false);
        Character_AMe(playerid, "gira la chiave e spegne il motore.");
    }
    else
    {
		if(Vehicle_GetFuel(vehicleid) < 0.5)
			return SendClientMessage(playerid, COLOR_ERROR, "Non c'è benzina nel veicolo.");
		if(Vehicle_GetHealth(vehicleid) <= 350.0)
			return SendClientMessage(playerid, COLOR_ERROR, "Il motore del veicolo non funziona.");
        if(Vehicle_IsOwner(vehicleid, playerid, false))
        {
            Character_AMe(playerid, "inserisce la chiave e la gira.");
			Character_Do(playerid, "Il motore si accende.");
			Vehicle_SetEngineOn(vehicleid);
			//Character_SetFreezed(playerid, true);
			//defer TurnOnVehicleEngine(playerid);
        }
        else
        {
            SendClientMessage(playerid, COLOR_ERROR, "Non hai le chiavi di questo veicolo.");
        }
    }
    return 1;
}

/*timer TurnOnVehicleEngine[500](playerid)
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
}*/

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
    format(string, sizeof(string), "%s", Vehicle_GetName(vehicle_id));
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
                return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al veicolo.");
            if(Vehicle_IsLocked(vehicle_id))
            {
                /*if(IsABike(vehicle_id) || IsAMotorBike(vehicle_id))
                {
                    Character_AMe(playerid, "toglie la catena dal suo veicolo");
                }
                else
                {
                    Character_AMe(playerid, "prende le chiavi e apre il suo veicolo");
                }
                SendClientMessage(playerid, COLOR_GREEN, "Hai aperto il tuo veicolo.");
                Vehicle_UnLock(vehicle_id);*/
				new dst[8];
				valstr(dst, vehicle_id);
				pc_cmd_vapri(playerid, dst);
            }
            else
            {
                /*if(IsABike(vehicle_id) || IsAMotorBike(vehicle_id))
                {
                    Character_AMe(playerid, "prende la catena e l'attacca al veicolo");
                }
                else
                {
                    Character_AMe(playerid, "prende le chiavi e chiude il suo veicolo");
                }
                SendClientMessage(playerid, COLOR_GREEN, "Hai chiuso il tuo veicolo.");
                Vehicle_UnLock(vehicle_id);*/
				new dst[8];
				valstr(dst, vehicle_id);
				pc_cmd_vchiudi(playerid, dst);
            }
            
        }
        case 1: // Park
        {
            if(GetPlayerVehicleID(playerid) != vehicle_id)
                return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo del veicolo.");
			if(Character_GetDeathState(playerid) > 0)
				return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
            new 
                Float:x, Float:y, Float:z, Float:a;
            GetVehiclePos(vehicle_id, x, y, z);
            GetVehicleZAngle(vehicle_id, a);
            Vehicle_Park(vehicle_id, x, y, z, a);
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai parcheggiato qui la tua %s.", Vehicle_GetName(vehicle_id));
        }
        case 2: // Trunk
        {
            if(!IsPlayerInRangeOfVehicle(playerid, vehicle_id, 5.0))
                return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al veicolo.");
            Vehicle_ShowInventory(vehicle_id, playerid);
        }
        case 3: // Sell to player
        {
            if(!IsPlayerInRangeOfVehicle(playerid, vehicle_id, 5.0))
                return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al veicolo.");
            Dialog_Show(playerid, Dialog_SellToPlayer, DIALOG_STYLE_INPUT, "Vendi a giocatore", "Inserisci l'ID o il nome del giocatore\n a cui vuoi vendere il veicolo\nseguito dal prezzo.\nEsempio: {00FF00}Mario Rossi {FF0000}10000.", "Vendi", "Annulla");
        }
        case 4: // Sell
        {
            if(!IsPlayerInRangeOfVehicle(playerid, vehicle_id, 5.0))
                return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al veicolo.");
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
        return Dialog_Show(playerid, Dialog_SellToPlayer, DIALOG_STYLE_INPUT, "{FFFFFF}Vendi a giocatore", "{FF0000}Il prezzo inserito non è valido!\n{FFFFFF}Inserisci l'ID o il nome del giocatore\n a cui vuoi vendere il veicolo\nseguito dal prezzo.\nEsempio: {00FF00}Mario Rossi {FF0000}10000.", "Vendi", "Annulla");
    
    if(!IsPlayerInRangeOfPlayer(playerid, otherPlayer, 6.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
    
	if(Character_GetDeathState(otherPlayer) > 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi vendere il veicolo a questo giocatore ora.");

	if(Character_GetMoney(otherPlayer) < price)
		return Dialog_Show(playerid, Dialog_SellToPlayer, DIALOG_STYLE_INPUT, "{FFFFFF}Vendi a giocatore", "{FF0000}Il giocatore non ha abbastanza soldi!\n{FFFFFF}Inserisci l'ID o il nome del giocatore\n a cui vuoi vendere il veicolo\nseguito dal prezzo.\nEsempio: {00FF00}Mario Rossi {FF0000}10000.", "Vendi", "Annulla");
    
    
    new vehicle_id = pSelectedVehicleListItem[playerid];
    pVehicleSellingTo[playerid] = otherPlayer;
    pVehicleSeller[otherPlayer] = playerid;
    pSellingVehicleID[playerid] = pSellingVehicleID[otherPlayer] = vehicle_id;
    pVehicleSellingPrice[playerid] = pVehicleSellingPrice[otherPlayer] = price;

    SendFormattedMessage(playerid, COLOR_GREEN, "Hai proposto a %s il tuo veicolo (%s) per $%d.", Character_GetOOCName(otherPlayer), Vehicle_GetName(vehicle_id), price);
    SendFormattedMessage(otherPlayer, COLOR_GREEN, "%s vuole venderti il suo veicolo (%s) per $%d. Digita /accetta veicolo per accettare.", Character_GetOOCName(playerid), Vehicle_GetName(vehicle_id), price);
    return 1;
}