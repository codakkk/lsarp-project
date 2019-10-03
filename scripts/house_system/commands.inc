
flags:gotohouse(CMD_JR_MODERATOR);
CMD:gotohouse(playerid, params[])
{
	new houseid;
    if(sscanf(params, "d", houseid))
	   return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /gotohouse <houseid>");
	new House:house = House_Find(houseid);
	if(!House_IsValid(house))
		return SendClientMessage(playerid, COLOR_ERROR, "La casa inserita non esiste.");
	new Float:x, Float:y, Float:z;
	House_GetEnterPosition(house, x, y, z);
	SetPlayerPos(playerid, x, y, z);
	SetPlayerInterior(playerid, House_GetEnterInterior(house));
	Character_SetVirtualWorld(playerid, House_GetEnterWorld(house));
	SendFormattedMessage(playerid, COLOR_GREEN, "Ti sei gotato alla casa id %d", House_GetID(house));
	return 1;
}

flags:hinttypes(CMD_JR_MODERATOR);
CMD:hinttypes(playerid, params[])
{
	new type;
	if(sscanf(params, "d", type) || !HouseInterior_IsValidType(type))
	{
		SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /hinttypes <tipo interno>");
		SendClientMessage(playerid, COLOR_GREY, "1: Casa molto piccola - 2: Casa piccola - 3: Casa media - 4: Casa grande");
		SendClientMessage(playerid, COLOR_GREY, "5: Casa molto grande - 6: Roulotte molto piccola - 7: Roulotte piccola");
		SendClientMessage(playerid, COLOR_GREY, "8: Roulotte media - 9: Roulotte grande");
		return 1;
	}
	new 
		count = 0,
		List:interiors = HouseInterior_GetListByType(type);
	
	SendFormattedMessage(playerid, COLOR_GREEN, "INTERIOR ID DISPONIBILI PER: %s", GetInteriorTypeName(type));
	for_list(i : interiors)
	{
		SendFormattedMessage(playerid, COLOR_GREEN, "Interior %d", count);
		++count;
	}
	return 1;
}

flags:hcreate(CMD_JR_MODERATOR);
CMD:hcreate(playerid, params[])
{
    new type;
    if(sscanf(params, "d", type))
	{
		SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /hcreate <tipo interno>");
		SendClientMessage(playerid, COLOR_GREY, "Usa /hinttypes per una lista.");
		return 1;
	}
    
    if(!HouseInterior_IsValidType(type))
	   return SendClientMessage(playerid, COLOR_ERROR, "Tipo interno non valido.");


    new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	
	new id = House_Create(x, y, z, GetPlayerInterior(playerid), Character_GetVirtualWorld(playerid), type);
	
	if(id <= 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non è stato possibile creare la casa. Contatta Coda.");
	

    SendFormattedMessage(playerid, COLOR_GREEN, "Casa id %d creata. Tipo Interno: %s - ID Interno: %d.", id, GetInteriorTypeName(type), 0);
	SendMessageToAdmins(true, COLOR_ORANGE, "AmdCmd: %s (%d) ha creato la casa id %d. Tipo Interno: %s - ID Interno: %d", Account_GetName(playerid), playerid, id, GetInteriorTypeName(type), 0);
    return 1;
}

flags:hdelete(CMD_JR_MODERATOR);
CMD:hdelete(playerid, params[])
{
	new houseid = strval(params);
	if(houseid <= 0)
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /hdelete <house id>");
	
	new House:house = House_Find(houseid);
	if(!House_IsValid(house))
		return SendClientMessage(playerid, COLOR_ERROR, "La casa inserita non esiste.");

	if(House_Delete(house))
	{
		SendMessageToAdmins(true, COLOR_ORANGE, "AdmCmd: %s (%d) ha rimosso la casa id %d.", Account_GetName(playerid), playerid, houseid);
	}
	else
	{
		SendClientMessage(playerid, COLOR_ERROR, "La casa non esiste o non puo' essere cancellata.");
	}
	return 1;
}

flags:hresetowner(CMD_JR_MODERATOR);
CMD:hresetowner(playerid, params[])
{
	new houseid = strval(params);
	
	if(houseid <= 0)
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /hresetowner <houseid>");

	new House:house = House_Find(houseid);
	if(!House_IsValid(house))
	   return SendClientMessage(playerid, COLOR_ERROR, "L'id della casa inserita non è valido.");


	if(House_ResetOwner(house))
	{
		SendFormattedMessage(playerid, COLOR_YELLOW, "Hai resettato il proprietario della casa id %d.", houseid);
		SendMessageToAdmins(1, COLOR_ORANGE, "AdmCmd: %s (%d) ha resettato il proprietario della casa id %d.", Account_GetName(playerid), playerid, houseid);
	}
	return 1;
}

flags:hedit(CMD_JR_MODERATOR);
CMD:hedit(playerid, params[])
{
    new houseid, command[16], value, value2;
    if(sscanf(params, "ds[16]D(-1)D(-1)", houseid, command, value, value2))
	   return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /hedit <houseid> <pos - interior - price - lock>");
    
	new House:house = House_Find(houseid);
	if(!House_IsValid(house))
	   return SendClientMessage(playerid, COLOR_ERROR, "L'id della casa inserita non è valido.");

    if(!strcmp(command, "pos", true))
    {
	   SendFormattedMessage(playerid, COLOR_GREEN, "Hai spostato le coordinate d'entrata della casa %d.", houseid);
	   new Float:x, Float:y, Float:z;
	   GetPlayerPos(playerid, x, y, z);
	   House_SetEnterPosition(house, x, y, z);
	   House_SetEnterInterior(house, GetPlayerInterior(playerid));
	   House_SetEnterWorld(house, Character_GetVirtualWorld(playerid));
	   House_CreateObjects(house);
    }
    else if(!strcmp(command, "interior", true))
    {
	   	if(value == -1 || value2 == -1)
		{
			SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /hedit <houseid> <interior type> <interior id>");
			SendClientMessage(playerid, COLOR_GREY, "Usa /hinttypes per una lista");
			return 0;
		}
	   	if(!HouseInterior_IsValidType(value))
			return SendClientMessage(playerid, COLOR_ERROR, "Tipo interior non valido");
		
	   	if(House_SetInterior(house, value, value2))
		{
	   		SendFormattedMessage(playerid, COLOR_GREEN, "Hai cambiato l'interior della casa %d. Tipo: %s - ID: %d", houseid, GetInteriorTypeName(value), value2);
		}
		else
			SendClientMessage(playerid, COLOR_ERROR, "Non è stato possibile settare l'interior.");
    }
    else if(!strcmp(command, "price", true))
    {
	   if(value == -1)
		  return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /hedit <houseid> <price> <price>");
	   if(value <= 0)
		  return SendClientMessage(playerid, COLOR_ERROR, "Il prezzo della casa deve essere maggiore di 0.");
	   SendFormattedMessage(playerid, COLOR_GREEN, "Hai cambiato il prezzo di vendita della casa %d. Nuovo prezzo: %d", houseid, value);
	   House_SetPrice(house, value);
    }
    else if(!strcmp(command, "lock", true))
    {
	   if(value != 0 && value != 1)
		  return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /hedit <houseid> <lock> <0: open - 1: lock>");
	   new s[8];
	   if(value)
		  s = "Chiusa";
	   else 
		  s = "Aperta";
	   SendFormattedMessage(playerid, COLOR_GREEN, "Hai cambiato lo stato della porta della casa %d. Stato: %s", houseid, s);
	   House_SetLocked(house, value?true:false);
    }
    else return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /hedit <houseid> <pos - interior - price - lock>");
    return 1;
}

flags:ahousecmds(CMD_JR_MODERATOR);
CMD:ahousecmds(playerid, params[])
{
    SendClientMessage(playerid, COLOR_WHITE, "[HOUSES]: /hcreate - /hedit - /resethousefurnitures - /resethouseexterior - /hresetowner");
    return 1;
}

CMD:housecmds(playerid, params[])
{
	SendClientMessage(playerid, -1, "[Casa] /casa ");
	return 1;
}

flags:casa(CMD_ALIVE_USER);
CMD:casa(playerid, params[])
{	
	return Character_ShowHouseDialog(playerid);
}
alias:casa("cmenu");

flags:bussa(CMD_ALIVE_USER);
CMD:bussa(playerid, params[])
{
	new House:house = Character_GetHouse(playerid, false);
	new Building:building = Character_GetBuilding(playerid, false);

	if(!House_IsValid(house) && !Building_IsValid(building))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata di un biz o di una casa.");

	new world = House_IsValid(house) ? House_GetExitWorld(house) : Building_GetExitWorld(building);

	foreach(new p : Player)
	{
		if(Character_GetVirtualWorld(p) == world)
			SendClientMessage(p, COLOR_PURPLE, "** Qualcuno sta bussando alla porta di casa. **");
	}
	Character_AMe(playerid, "bussa alla porta.");
	return 1;
}

flags:dshout(CMD_ALIVE_USER);
CMD:dshout(playerid, params[])
{
	if(isnull(params) || strlen(params) <= 0)
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: (/ds)hout [testo]");
	
	new House:house = Character_GetHouse(playerid, false);
	new Building:building = Character_GetBuilding(playerid, false);

	if(House_IsValid(house) || Building_IsValid(building))
	{
		new world = House_IsValid(house) ? House_GetExitWorld(house) : Building_GetExitWorld(building);
		foreach(new p : Player)
		{
			if(Character_GetVirtualWorld(p) == world)
				SendFormattedMessage(p, -1, "[Porta] %s grida: %s!", Character_GetRolePlayName(playerid), params);
		}
		pc_cmd_shout(playerid, params);
	}
	else
	{
		house = Character_GetHouse(playerid, true);
		building = Character_GetBuilding(playerid, true);

		if(!House_IsValid(house) && !Building_IsValid(building))
			return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata o all'uscita di un biz o di una casa.");
		
		new Float:x, Float:y, Float:z, world, interior;
		if(House_IsValid(house))
		{
			House_GetEnterPosition(house, x, y, z);
			world = House_GetEnterWorld(house);
			interior = House_GetEnterInterior(house);
		}
		else
		{
			Building_GetEnterPos(building, x, y, z);
			world = Building_GetEnterWorld(building);
			interior = Building_GetEnterInterior(building);
		}
		pc_cmd_shout(playerid, params);
		SendRangedMessageStr(20.0, x, y, z, world, interior, str_format("[Porta] %s grida: %s", Character_GetRolePlayName(playerid), params), COLOR_FADE1, COLOR_FADE2, COLOR_FADE3, COLOR_FADE4, COLOR_FADE5, false);
	}
	return 1;
}
alias:dshout("ds");

flags:ddo(CMD_ALIVE_USER);
CMD:ddo(playerid, params[])
{
	if(isnull(params) || strlen(params) <= 0)
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /ddo [testo]");
	
	new House:house = Character_GetHouse(playerid, false);
	new Building:building = Character_GetBuilding(playerid, false);

	if(House_IsValid(house) || Building_IsValid(building))
	{
		new world = House_IsValid(house) ? House_GetExitWorld(house) : Building_GetExitWorld(building);
		foreach(new p : Player)
		{
			if(Character_GetVirtualWorld(p) == world)
				SendFormattedMessage(p, -1, "* [Porta] %s (( %s ))", params, Character_GetRolePlayName(playerid));
		}
		pc_cmd_do(playerid, params);
	}
	else
	{
		house = Character_GetHouse(playerid, true);
		building = Character_GetBuilding(playerid, true);

		if(!House_IsValid(house) && !Building_IsValid(building))
			return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata o all'uscita di un biz o di una casa.");
		
		new Float:x, Float:y, Float:z, world, interior;
		if(House_IsValid(house))
		{
			House_GetEnterPosition(house, x, y, z);
			world = House_GetEnterWorld(house);
			interior = House_GetEnterInterior(house);
		}
		else
		{
			Building_GetEnterPos(building, x, y, z);
			world = Building_GetEnterWorld(building);
			interior = Building_GetEnterInterior(building);
		}
		
		pc_cmd_do(playerid, params);

		SendRangedMessageStr(20.0, x, y, z, world, interior, str_format("* [Porta] %s (( %s ))", params, Character_GetRolePlayName(playerid)), COLOR_INTERACTION, COLOR_INTERACTION, COLOR_INTERACTION, COLOR_INTERACTION, COLOR_INTERACTION, false);
	}
	return 1;
}

flags:resethousefurnitures(CMD_JR_MODERATOR);
CMD:resethousefurnitures(playerid, params[])
{
    new houseid;
    if(sscanf(params, "d", houseid))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /resethouseexterior <houseid>");
    new House:house = House_Find(houseid);
	if(!House_IsValid(house))
        return SendClientMessage(playerid, COLOR_ERROR, "La casa inserita non è valida.");
    

    if(House_RemoveAllfurnitures(house))
        SendFormattedMessage(playerid, COLOR_GREEN, "Mobili rimossi dalla casa id %d", House_GetID(house));
    else
        SendFormattedMessage(playerid, COLOR_ERROR, "Non è stato possibile rimuovere i mobili dalla casa id %d", House_GetID(house));

    return 1;
}
