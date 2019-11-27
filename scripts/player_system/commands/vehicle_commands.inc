#include <YSI_Coding\y_hooks>

static 
	pVehicleListItem[MAX_PLAYERS][MAX_VEHICLES_PER_PLAYER],
	pSelectedVehicleListItem[MAX_PLAYERS]
;


/*flags:vmenu(CMD_ALIVE_USER);
CMD:vmenu(playerid, params[])
{
    if(Character_IsBuyingVehicle(playerid))
        return 1;
    new 
        current_vehicle = GetPlayerVehicleID(playerid),
        string[1024],
        count = 0;
    memset(pVehicleListItem[playerid], 0);
    foreach(new v : Vehicle)
    {
        if(Vehicle_GetOwnerID(v) != Character_GetID(playerid))
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
}*/

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
	Character_AMe(playerid, w[window] ? ("chiude il finestrino.") : ("apre il finestrino."));
	return 1;
}
alias:finestrino("fin");

flags:vluci(CMD_ALIVE_USER);
CMD:vluci(playerid, params[])
{
	return callcmd::vehicle(playerid, "luci");
}

flags:vparcheggia(CMD_ALIVE_USER);
CMD:vparcheggia(playerid, params[])
{
	return callcmd::vehicle(playerid, "parcheggia");
}
alias:vparcheggia("vpark");

flags:vsicura(CMD_ALIVE_USER);
CMD:vsicura(playerid, params[])
{
    return callcmd::vehicle(playerid, "sicura");
}


flags:vbagagliaio(CMD_ALIVE_USER);
CMD:vbagagliaio(playerid, params[])
{
	return callcmd::vehicle(playerid, "bagagliaio");
}
alias:vbagagliaio("vtrunk");

flags:vinventario(CMD_ALIVE_USER);
CMD:vinventario(playerid, params[])
{
	return callcmd::vehicle(playerid, "inventario");
}
alias:vinventario("vinv");

flags:vdeposita(CMD_ALIVE_USER);
CMD:vdeposita(playerid, params[])
{
	if(Request_IsPending(playerid, REQUEST_TYPE_ITEM))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta attiva.");
	
	if(Character_IsFactionDuty(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se sei in servizio");
	
	new vehicleid;
	if(sscanf(params, "d", vehicleid))
	{
		vehicleid = Character_GetClosestVehicle(playerid, 3.5);
	}
	else
	{
		if(!Vehicle_IsValid(vehicleid))
			return SendClientMessage(playerid, COLOR_ERROR, "Veicolo non valido.");

		if(!IsPlayerInRangeOfVehicle(playerid, vehicleid, 3.5) || Character_GetVirtualWorld(playerid) != GetVehicleVirtualWorld(vehicleid))
			return SendClientMessage(playerid, COLOR_GREEN, "Non sei vicino al veicolo.");
	}

	if(!Vehicle_IsOwner(vehicleid, playerid, false))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei il proprietario di questo veicolo.");

	new itemid = Character_GetWeapon(playerid);
	if(itemid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando senza un'arma.");
	
	new ammo = Character_GetAmmo(playerid);
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
	Character_RemoveWeapon(playerid, itemid);
	return 1;
}
alias:vdeposita("vdep");

flags:vdisassembla(CMD_ALIVE_USER);
CMD:vdisassembla(playerid, params[])
{
	if(Request_IsPending(playerid, REQUEST_TYPE_WEAPON))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta attiva.");
	new 
		slotid, itemid, ammo;
	new vehicleid = Character_GetClosestVehicle(playerid, 3.0);
	if(vehicleid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino ad un veicolo.");
	if(!Vehicle_IsOwner(vehicleid, playerid, false))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei il proprietario di questo veicolo.");
	if(sscanf(params, "k<slotid>", slotid))
	{
		if(Character_IsFactionDuty(playerid))
			return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se sei in servizio");
		itemid = Character_GetWeapon(playerid);
		ammo = Character_GetAmmo(playerid);
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
		Character_RemoveWeapon(playerid, itemid);
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
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /vusa <slotid>");
	return 1;
}*/

flags:motore(CMD_ALIVE_USER);
CMD:motore(playerid, params[])
{
    return callcmd::vehicle(playerid, "motore");
}


Dialog:Dialog_VehicleList(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    new 
        vehicle_id = pVehicleListItem[playerid][listitem];
    if(vehicle_id == 0 || Vehicle_GetID(vehicle_id) == 0 || Vehicle_GetOwnerID(vehicle_id) != Character_GetID(playerid))
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
    new vehicleid = pSelectedVehicleListItem[playerid];
    switch(listitem)
    {
        case 0: // Open/Close
        {
            new dst[24];
			format(dst, sizeof(dst), "sicura %d", dst);
			callcmd::vsicura(playerid, dst);
        }
        case 1: // Park
        {
            return callcmd::vparcheggia(playerid, "");
        }
        case 2: // Trunk
        {
            if(!IsPlayerInRangeOfVehicle(playerid, vehicleid, 5.0))
                return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al veicolo.");
            Character_InteractWithInventory(playerid, Vehicle_GetInventory(vehicleid), true, true);
        }
        case 3: // Sell to player
        {
            if(!IsPlayerInRangeOfVehicle(playerid, vehicleid, 5.0))
                return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al veicolo.");
        }
        case 4: // Sell
        {
            if(!IsPlayerInRangeOfVehicle(playerid, vehicleid, 5.0))
                return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al veicolo.");
        }
		case 5: // Info
		{
			new Float:health;
			GetVehicleHealth(vehicleid, health);
			SendFormattedMessage(playerid, -1, "ID Veicolo: {00FF00}%d{FFFFFF} - HP: {00FF00}%.2f{FFFFFF}", vehicleid, health);
		}
    }
    return 1;
}
