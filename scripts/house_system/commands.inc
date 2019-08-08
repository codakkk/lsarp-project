
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

flags:hcreate(CMD_DEVELOPER);
CMD:hcreate(playerid, params[])
{
    new interior;
    if(sscanf(params, "d", interior))
	   return SendClientMessage(playerid, COLOR_ERROR, "/hcreate <interno base>");
    
    if(interior < 0 || interior >= sizeof(allHouseInteriors))
	   return SendClientMessage(playerid, COLOR_ERROR, "Interno base non valido");


    new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	
	new id = House_Create(x, y, z, GetPlayerInterior(playerid), Character_GetVirtualWorld(playerid), interior);
	
	if(id == 0)
		return 0;
	
    SendFormattedMessage(playerid, COLOR_GREEN, "Casa id %d creata. Interno: %d.", id, interior);
	SendMessageToAdmins(true, COLOR_ORANGE, "L'admin %s (%d) ha creato la casa id %d. Interno: %d", Account_GetName(playerid), playerid, id, interior);
    return 1;
}

flags:hdelete(CMD_DEVELOPER);
CMD:hdelete(playerid, params[])
{
	new houseid = strval(params);
	if(houseid <= 0)
		return SendClientMessage(playerid, COLOR_ERROR, "/hdelete <house id>");
	if(House_Delete(houseid))
	{
		SendMessageToAdmins(true, COLOR_ORANGE, "AdmCmd: %s (%d) ha rimosso la casa id %d.", Account_GetName(playerid), playerid, houseid);
	}
	else
	{
		SendClientMessage(playerid, COLOR_ERROR, "La casa non esiste o non puo' essere cancellata.");
	}
	return 1;
}

flags:hresetowner(CMD_DEVELOPER);
CMD:hresetowner(playerid, params[])
{
	new houseid = strval(params);
	
	if(houseid <= 0)
		return SendClientMessage(playerid, COLOR_ERROR, "/hresetowner <houseid>");

	if(!House_IsValid(houseid))
	   return SendClientMessage(playerid, COLOR_ERROR, "L'id della casa inserita non è valido.");

	if(!House_IsCreated(houseid))
		return SendClientMessage(playerid, COLOR_ERROR, "La casa non è stata creata.");

	if(House_ResetOwner(houseid))
	{
		SendFormattedMessage(playerid, COLOR_YELLOW, "Hai resettato il proprietario della casa id %d.", houseid);
		SendMessageToAdmins(1, COLOR_ORANGE, "AdmCmd: %s (%d) ha resettato il proprietario della casa id %d.", Account_GetName(playerid), playerid, houseid);
	}
	return 1;
}

flags:hedit(CMD_DEVELOPER);
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
    }
    else if(!strcmp(command, "interior", true))
    {
	   if(value == -1)
		  return SendClientMessage(playerid, COLOR_ERROR, "/hedit <houseid> <interior> <interiorid>");
	   if(value < 0 || value >= sizeof(allHouseInteriors))
		  return SendClientMessage(playerid, COLOR_ERROR, "Interior non valido");
	   SendFormattedMessage(playerid, COLOR_GREEN, "Hai cambiato l'interior della casa %d. Nuovo: %d", houseid, value);
	   House_SetInterior(houseid, value);
    }
    else if(!strcmp(command, "price", true))
    {
	   if(value == -1)
		  return SendClientMessage(playerid, COLOR_ERROR, "/hedit <houseid> <price> <price>");
	   if(value <= 0)
		  return SendClientMessage(playerid, COLOR_ERROR, "Il prezzo della casa deve essere maggiore di 0.");
	   SendFormattedMessage(playerid, COLOR_GREEN, "Hai cambiato il prezzo di vendita della casa %d. Nuovo prezzo: %d", houseid, value);
	   House_SetPrice(houseid, value);
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
    }
    else return SendClientMessage(playerid, COLOR_ERROR, "/hedit <houseid> <pos - interior - price - lock>");
    return 1;
}

flags:ahousecmds(CMD_DEVELOPER);
CMD:ahousecmds(playerid, params[])
{
    SendClientMessage(playerid, COLOR_WHITE, "[HOUSES]: /hcreate - /hedit - /resethousefurnitures - /resethouseexterior - /hresetowner");
    return 1;
}

flags:casa(CMD_ALIVE_USER);
CMD:casa(playerid, params[])
{	
	return Character_ShowHouseDialog(playerid);
}
alias:casa("cmenu");

flags:resethousefurnitures(CMD_DEVELOPER);
CMD:resethousefurnitures(playerid, params[])
{
    new houseid;
    if(sscanf(params, "d", houseid))
        return SendClientMessage(playerid, COLOR_ERROR, "/resethouseexterior <houseid>");
    if(!House_IsValid(houseid))
        return SendClientMessage(playerid, COLOR_ERROR, "La casa inserita non è valida.");
    

    if(House_RemoveAllfurnitures(houseid))
        SendFormattedMessage(playerid, COLOR_GREEN, "Mobili rimossi dalla casa id %d", houseid);
    else
        SendFormattedMessage(playerid, COLOR_ERROR, "Non è stato possibile rimuovere i mobili dalla casa id %d", houseid);

    return 1;
}
