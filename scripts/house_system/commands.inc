
flags:gotohouse(CMD_JR_MODERATOR);
CMD:gotohouse(playerid, params[])
{
	new houseid;
    if(sscanf(params, "d", houseid))
	   return SendClientMessage(playerid, COLOR_ERROR, "/gotohouse <houseid>");
	if(!House_IsValid(houseid) || !House_IsCreated(houseid))
		return SendClientMessage(playerid, COLOR_ERROR, "La casa inserita non esiste.");
	new Float:x, Float:y, Float:z;
	House_GetEnterPosition(houseid, x, y, z);
	SetPlayerPos(playerid, x, y, z);
	SetPlayerInterior(playerid, House_GetEnterInterior(houseid));
	Character_SetVirtualWorld(playerid, House_GetEnterWorld(houseid));
	SendFormattedMessage(playerid, COLOR_GREEN, "Ti sei gotato alla casa id %d", houseid);
	return 1;
}

flags:hcreate(CMD_RCON);
CMD:hcreate(playerid, params[])
{
    new interior;
    if(sscanf(params, "d", interior))
	   return SendClientMessage(playerid, COLOR_ERROR, "/hcreate <interno base>");
    
    if(interior < 0 || interior >= sizeof(allHouseInteriors))
	   return SendClientMessage(playerid, COLOR_ERROR, "Interno base non valido");

	new availableHouseID = -1;
	for(new i = 0, j = list_size(HouseList); i < j; ++i)
	{
		if(House_IsValid(i) && !House_IsCreated(i))
		{
			availableHouseID = i;
		}
	}

	if(availableHouseID == -1)
		return SendClientMessage(playerid, COLOR_ERROR, "Non è possibile creare ulteriori case.");

	House_SetCreated(availableHouseID, 1);
    new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	House_SetEnterPosition(availableHouseID, x, y, z);
	House_SetEnterInterior(availableHouseID, GetPlayerInterior(playerid));
	House_SetEnterWorld(availableHouseID, Character_GetVirtualWorld(playerid));
	House_SetInterior(availableHouseID, interior);
	House_CreateObjects(availableHouseID);
	House_Save(availableHouseID);
    SendFormattedMessage(playerid, COLOR_GREEN, "Casa id %d creata. Interno: %d.", availableHouseID, interior);
	SendMessageToAdmins(true, COLOR_ORANGE, "L'admin %s (%d) ha creato la casa id %d. Interno: %d", AccountInfo[playerid][aName], playerid, availableHouseID, interior);
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

	if(!House_IsCreated(houseid))
		return SendClientMessage(playerid, COLOR_ERROR, "La casa non è stata creata.");

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
	new id, E_ELEMENT_TYPE:type;
	if(Character_GetNearestPickup(playerid, id, type) && (type == ELEMENT_TYPE_HOUSE_ENTRANCE || type == ELEMENT_TYPE_HOUSE_EXIT))
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