
flags:hcreate(CMD_RCON);
CMD:hcreate(playerid, params[])
{
    new interior;
    if(sscanf(params, "d", interior))
	   return SendClientMessage(playerid, COLOR_ERROR, "/hcreate <interno base>");
    
    if(interior < 0 || interior >= sizeof(allHouseInteriors))
	   return SendClientMessage(playerid, COLOR_ERROR, "Interno base non valido");

    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    House_Create(x, y, z, GetPlayerInterior(playerid), Character_GetVirtualWorld(playerid), interior);
    SendFormattedMessage(playerid, COLOR_GREEN, "Casa creata. Interno: %d.", interior);
    return 1;
}

flags:hedit(CMD_RCON);
CMD:hedit(playerid, params[])
{
    new houseid, command[16], value;
    if(sscanf(params, "ds[16]D(-1)", houseid, command, value))
	   return SendClientMessage(playerid, COLOR_ERROR, "/hedit <houseid> <pos - interior - price - lock>");
    
    if(!House_IsValid(houseid))
	   return SendClientMessage(playerid, COLOR_ERROR, "L'id della casa inserita non è valido.");

    if(!strcmp(command, "pos", true))
    {
	   SendFormattedMessage(playerid, COLOR_GREEN, "Hai spostato le coordinate d'entrata della casa %d.", houseid);
	   new Float:x, Float:y, Float:z;
	   GetPlayerPos(playerid, x, y, z);
	   House_SetEnterPosition(houseid, x, y, z);
	   House_SetEnterInterior(houseid, GetPlayerInterior(playerid));
	   House_SetEnterWorld(houseid, Character_GetVirtualWorld(playerid));
	   House_CreateObjects(houseid);
	   House_Save(houseid);
    }
    else if(!strcmp(command, "interior", true))
    {
	   if(value == -1)
		  return SendClientMessage(playerid, COLOR_ERROR, "/hedit <houseid> <interior> <interiorid>");
	   if(value < 0 || value >= sizeof(allHouseInteriors))
		  return SendClientMessage(playerid, COLOR_ERROR, "Interior non valido");
	   SendFormattedMessage(playerid, COLOR_GREEN, "Hai cambiato l'interior della casa %d. Nuovo: %d", houseid, value);
	   House_SetInterior(houseid, value);
	   House_Save(houseid);
    }
    else if(!strcmp(command, "price", true))
    {
	   if(value == -1)
		  return SendClientMessage(playerid, COLOR_ERROR, "/hedit <houseid> <price> <price>");
	   if(value <= 0)
		  return SendClientMessage(playerid, COLOR_ERROR, "Il prezzo della casa deve essere maggiore di 0.");
	   SendFormattedMessage(playerid, COLOR_GREEN, "Hai cambiato il prezzo di vendita della casa %d. Nuovo prezzo: %d", houseid, value);
	   House_SetPrice(houseid, value);
	   House_Save(houseid);
    }
    else if(!strcmp(command, "lock", true))
    {
	   if(value != 0 && value != 1)
		  return SendClientMessage(playerid, COLOR_ERROR, "/hedit <houseid> <lock> <0: open - 1: lock>");
	   new s[8];
	   if(value)
		  s = "Chiusa";
	   else 
		  s = "Aperta";
	   SendFormattedMessage(playerid, COLOR_GREEN, "Hai cambiato lo stato della porta della casa %d. Stato: %s", houseid, s);
	   House_SetLocked(houseid, value);
	   House_Save(houseid);
    }
    else return SendClientMessage(playerid, COLOR_ERROR, "/hedit <houseid> <pos - interior - price - lock>");
    return 1;
}

flags:ahousecmds(CMD_RCON);
CMD:ahousecmds(playerid, params[])
{
    SendClientMessage(playerid, COLOR_WHITE, "[HOUSES]: /hcreate - /hedit");
    return 1;
}

flags:casa(CMD_ALIVE_USER);
CMD:casa(playerid, params[])
{
	new pickupid = Character_GetLastPickup(playerid), id, E_ELEMENT_TYPE:type;
	if(Pickup_GetInfo(pickupid, id, type) && IsPlayerInRangeOfPickup(playerid, pickupid, 2.0) && (type == ELEMENT_TYPE_HOUSE_ENTRANCE || type == ELEMENT_TYPE_HOUSE_EXIT))
	{
		if(House_GetOwnerID(id) == Character_GetID(playerid) || Account_IsAdminDuty(playerid))
		{
			// REMEMBER TO EDIT THE HOTKEY PART IF THIS DIALOG IS EDITED
			Dialog_Show(playerid, Dialog_House, DIALOG_STYLE_LIST, "Casa", "Apri/Chiudi Porta\nInventario\nDeposita Soldi\nRitira Soldi\nVendi\nVendi a Giocatore\nCambia Interior", "Continua", "Chiudi");
		}
		else return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino ad una tua casa.");
	}
	else return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino ad una tua casa.");
	return 1;
}