
#include <YSI_Coding\y_hooks>

hook function ShowJobCommands(playerid)
{
    if(Character_GetJobType(playerid) == JOB_TYPE_MECHANIC)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "[MECCANICO] /ripara /vernicia /rimorchia");
    }
    return continue(playerid);
}

hook function EnterJobDuty(playerid)
{
    #pragma unused playerid
    printf("mechanic");
    return 0;
}

flags:ripara(CMD_ALIVE_USER);
CMD:ripara(playerid, params[])
{
    if(Character_GetJobType(playerid) != JOB_TYPE_MECHANIC || !Character_IsJobDuty(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei un meccanico in servizio.");

    new id, price;
    if(sscanf(params, "k<u>d", id, price))
    {
        SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /ripara <playerid/partofname/#id> <prezzo>");
        return SendClientMessage(playerid, COLOR_GREY, "Utilizza il tuo id/nome per riparare il tuo veicolo.");
    }

    if(!IsPlayerConnected(id) || !Character_IsLogged(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");

    if(!IsPlayerInRangeOfPlayer(playerid, id, 5.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
    
    new vehicleid = GetPlayerVehicleID(id);
    
    if(vehicleid <= 0 || !IsValidVehicle(vehicleid))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è a bordo di un veicolo.");
    
    if(price > Character_GetMoney(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non ha abbastanza soldi.");

    if(id == playerid)
    {
        vehicleid = GetPlayerVehicleID(playerid);
        
        if(!Vehicle_IsValid(vehicleid))
            return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo.");

        SendClientMessage(playerid, COLOR_GREEN, "Hai riparato il tuo veicolo.");
        Vehicle_Repair(vehicleid);
    }
    else
    {
        Character_SetRequest(playerid, id, REQUEST_TYPE_REPAIR, 0, price);

        SendFormattedMessage(id, COLOR_RECEIVE_REQUEST, "%s vuole riparare il tuo veicolo (%s) per $%d.", Character_GetRolePlayName(playerid), Vehicle_GetName(vehicleid), price);
        SendClientMessage(id, COLOR_RECEIVE_REQUEST, "Digita \"/accetta riparazione\" per accettare o \"/rifiuta riparazione\" per rifiutare.");
        
        SendFormattedMessage(playerid, COLOR_SEND_REQUEST, "Hai inviato la richiesta di riparazione a %s per $%d. Usa \"annulla riparazione\" per annullare.", Character_GetRolePlayName(id), price);
    }
    return 1;
}

flags:vernicia(CMD_ALIVE_USER);
CMD:vernicia(playerid, params[])
{
    if(Character_GetJobType(playerid) != JOB_TYPE_MECHANIC || !Character_IsJobDuty(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei un meccanico in servizio.");
    
    new id, price, color1, color2;
    if(sscanf(params, "dddd", id, price, color1, color2))
    {
        SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /vernicia <playerid/partofname/#id> <prezzo> <colore 1> <colore 2>");
        return SendClientMessage(playerid, COLOR_GREY, "Utilizza il tuo id/nome per riverniciare il tuo veicolo.");
    }
    
    if(color1 < 0 || color1 > 255 || color2 < 0 || color2 > 255)
        return SendClientMessage(playerid, COLOR_ERROR, "Uno o più colori inseriti non sono validi.");

    if(!IsPlayerConnected(id) || !Character_IsLogged(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");

    if(!IsPlayerInRangeOfPlayer(playerid, id, 5.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
    
    new vehicleid = GetPlayerVehicleID(id);
    
    if(vehicleid <= 0 || !IsValidVehicle(vehicleid))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è a bordo di un veicolo.");
    
    if(price > Character_GetMoney(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non ha abbastanza soldi.");

    if(id == playerid)
    {
        vehicleid = GetPlayerVehicleID(playerid);
        
        if(!Vehicle_IsValid(vehicleid))
            return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo.");
        
        //Character_DecreaseItemAmount(playerid, gItem_MechanicalComponents, MATERIALS_PER_SPRAY);
        SendClientMessage(playerid, COLOR_GREEN, "Hai riverniciato il tuo veicolo.");

        Vehicle_SetColors(vehicleid, color1, color2);
    }
    else
    {
        Character_SetRequest(playerid, id, REQUEST_TYPE_VEHICLE_SPRAY, color1, price, color2);

        SendFormattedMessage(id, COLOR_RECEIVE_REQUEST, "%s vuole verniciare il tuo veicolo (%s) per $%d.", Character_GetRolePlayName(playerid), Vehicle_GetName(vehicleid), price);
        SendClientMessage(id, COLOR_RECEIVE_REQUEST, "Digita \"/accetta verniciatura\" per accettare o \"/rifiuta verniciatura\" per rifiutare.");
        
        SendFormattedMessage(playerid, COLOR_SEND_REQUEST, "Hai inviato la richiesta di verniciatura a %s per $%d.", Character_GetRolePlayName(id), price);
    }
    return 1;
}

CMD:rimorchia(playerid, params[])
{
    if(Faction_GetType(Character_GetFaction(playerid)) != FACTION_TYPE_POLICE && (Character_GetJobType(playerid) != JOB_TYPE_MECHANIC || !Character_IsJobDuty(playerid)))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei un meccanico in servizio.");
    
	new vehicleid = GetPlayerVehicleID(playerid),
		Float:x, Float:y, Float:z,
		Float:vx, Float:vy, Float:vz;

	if(!Character_IsInTowTruck(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un Tow Truck.");

	if(IsTrailerAttachedToVehicle(vehicleid))
        return DetachTrailerFromVehicle(vehicleid);

	GetPlayerPos(playerid, x, y, z);

	foreach(new i : Vehicle)
	{
		if(!Vehicle_IsValid(i))continue;

		GetVehiclePos(i, vx, vy, vz);

		if((floatabs(x - vx) < 7.0) && (floatabs(y - vy) < 7.0) && (floatabs(z - vz) < 7.0) && i != vehicleid)
		{
			AttachTrailerToVehicle(i, vehicleid); 
            break;
		}
	}
	return 1;
}

hook OnCharacterTryAccept(playerid, type)
{
    if(type == REQUEST_TYPE_VEHICLE_SPRAY)
	{
        AcceptVehicleSpray(playerid);
    }
    else if(type == REQUEST_TYPE_REPAIR)
    {
        AcceptVehicleRepair(playerid);
    }
    return 1;
}

static AcceptVehicleRepair(playerid)
{   
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

    SendFormattedMessage(senderid, COLOR_ACCEPT_REQUEST, "%s ha accettato la tua richiesta di riparazione. Hai guadagnato $%d.", Character_GetRolePlayName(playerid), price);
    //SendFormattedMessage(senderid, COLOR_ACCEPT_REQUEST, "La riparazione ti è costata: %d materiali meccanici.", MATERIALS_PER_REPAIR);

    SendFormattedMessage(playerid, COLOR_ACCEPT_REQUEST, "%s ha riparato il tuo veicolo (%s) per $%d.", Character_GetRolePlayName(senderid), Vehicle_GetName(vehicleid), price);

    Vehicle_Repair(vehicleid);

    Request_Reset(playerid, REQUEST_TYPE_REPAIR);
    Request_Reset(senderid, REQUEST_TYPE_REPAIR);
    return 1;
}

static AcceptVehicleSpray(playerid)
{
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
    
    SendFormattedMessage(senderid, COLOR_ACCEPT_REQUEST, "%s ha accettato la tua richiesta di verniciatura. Hai guadagnato $%d.", Character_GetRolePlayName(playerid), price);
    //SendFormattedMessage(senderid, COLOR_ACCEPT_REQUEST, "La verniciatura ti è costata: %d materiali meccanici.", MATERIALS_PER_SPRAY);
    SendFormattedMessage(playerid, COLOR_ACCEPT_REQUEST, "%s ha verniciato il tuo veicolo (%s) per $%d.", Character_GetRolePlayName(senderid), Vehicle_GetName(vehicleid), price);
    
    Vehicle_SetColors(vehicleid, color1, color2);

    Request_Reset(playerid, REQUEST_TYPE_VEHICLE_SPRAY);
    Request_Reset(senderid, REQUEST_TYPE_VEHICLE_SPRAY);
    return 1;
}

static Character_IsInTowTruck(playerid)
{
    new vehicleid = GetPlayerVehicleID(playerid);
    return Vehicle_IsValid(vehicleid) && Vehicle_GetModel(vehicleid) == 525;
}