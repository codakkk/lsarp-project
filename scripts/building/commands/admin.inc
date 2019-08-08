flags:abuildingcmds(CMD_RCON);
CMD:abuildingcmds(playerid, params[])
{
    SendClientMessage(playerid, -1, "[BUILDINGS]: /abuildingexit - /abuildingentrance - /abuildingname");
    SendClientMessage(playerid, -1, "[BUILDINGS]: /abuildingownable - /abuildingprice - /abuildinglock - /abuildingwelcome");
    SendClientMessage(playerid, -1, "[BUILDINGS]: /abuildingfaction - /abuildingtype");
    return 1;
}

flags:gotobuilding(CMD_ADMIN);
CMD:gotobuilding(playerid, params[])
{
	new id;
	if(sscanf(params, "d", id))
		return SendClientMessage(playerid, COLOR_ERROR, "/gotobuilding <buildingid>");
	if(!Building_IsValid(id))
		return SendClientMessage(playerid, COLOR_ERROR, "L'edificio inserito non è valido.");
	new Float:x, Float:y, Float:z;
	Building_GetEnterPos(id, x, y, z);
	SetPlayerPos(playerid, x, y, z);
	SetPlayerInterior(playerid, Building_GetEnterInterior(id));
	Character_SetVirtualWorld(playerid, Building_GetEnterWorld(id));
	return 1;
}

flags:abcreate(CMD_RCON);
CMD:abcreate(playerid, params[])
{
    
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);

    new id = Building_Create(x, y, z, GetPlayerInterior(playerid), Character_GetVirtualWorld(playerid));
    Log(Account_GetName(playerid), "", "/abcreate", id);
    return 1;
}

flags:abuildingfaction(CMD_RCON);
CMD:abuildingfaction(playerid, params[])
{
	new bid, fid;
	if(sscanf(params, "dd", bid, fid))
	{
		SendClientMessage(playerid, COLOR_ERROR, "/abuildingfaction <buildingid> <factionid>");
		return SendFormattedMessage(playerid, COLOR_ERROR, "Utilizza %d come factionid per rimuoverla.", INVALID_FACTION_ID);
	}
	if(!Building_IsValid(bid))
		return SendClientMessage(playerid, COLOR_ERROR, "L'edificio inserito non è valdio.");
	if(fid != INVALID_FACTION_ID && !Faction_IsValid(fid))
		return SendClientMessage(playerid, COLOR_ERROR, "La fazione inserita non è valida. Usa /afactionslist per una lista.");	
	
	Building_SetFaction(bid, fid);

	if(fid == INVALID_FACTION_ID)
		SendFormattedMessage(playerid, COLOR_GREEN, "L'edificio %d non appartiene più alla fazione ID %d.", bid, fid);
	else
		SendClientMessageStr(playerid, COLOR_GREEN, str_format("Hai settato l'edificio %d appartenente alla fazione %S (%d).", bid, Faction_GetNameStr(fid), fid));
	Log(Account_GetName(playerid), "", "/abuildingfaction", bid);
	return 1;
}

flags:abuildingexit(CMD_RCON);
CMD:abuildingexit(playerid, params[])
{
    new id;
    if(sscanf(params, "i", id))
	   return SendClientMessage(playerid, COLOR_ERROR, "/abuildingexit <buildingid>");
    
    if(!Building_IsValid(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Invalid Building ID");
    
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);

    if(Building_SetInterior(id, x, y, z, GetPlayerInterior(playerid)))
    {
		SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato l'interior del building id %d.", id);
		Log(Account_GetName(playerid), "", "/abuildingexit", id);
    }
    else
    {
	   SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile settare l'interior del building %d.", id);
    }
    return 1;
}

flags:abuildingentrance(CMD_RCON);
CMD:abuildingentrance(playerid, params[])
{
    new id;
    if(sscanf(params, "i", id))
		return SendClientMessage(playerid, COLOR_ERROR, "/abuildingentrance <buildingid>");
    
    if(!Building_IsValid(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Invalid Building ID");
    
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);

    if(Building_SetPosition(id, x, y, z, Character_GetVirtualWorld(playerid), GetPlayerInterior(playerid)))
    {
		SendFormattedMessage(playerid, COLOR_GREEN, "Hai spostato il building %d.", id);
		Log(Account_GetName(playerid), "", "/abuildingentrance", id);
    }
    else
    {
	   SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile spostare il building %d.", id);
    }
    return 1;
}

flags:abuildingname(CMD_RCON);
CMD:abuildingname(playerid, params[])
{
    new id, name[MAX_BUILDING_NAME];
    if(sscanf(params, "is[64]", id, name))
	   return SendClientMessage(playerid, COLOR_ERROR, "/abuildingname <buildingid> <name>");
    
    if(!Building_IsValid(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "Invalid Building ID");
    
    if(strlen(name) > MAX_BUILDING_NAME)
	   return SendClientMessage(playerid, COLOR_ERROR, "Nome troppo lungo.");

	if(Building_SetName(id, name))
	{
		SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato il nome del building %d. Nuovo nome: %s", id, name);
		Log(Account_GetName(playerid), "", "/abuildingname", id);
	}
    else
    {
	   SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile settare il nome del building %d.", id);
    }
    return 1;
}

flags:abuildingownable(CMD_RCON);
CMD:abuildingownable(playerid, params[])
{
    new id, ownable;
    if(sscanf(params, "ii", id, ownable))
	   return SendClientMessage(playerid, COLOR_ERROR, "/abuildingname <buildingid> <ownable: 0:no 1:yes>");
    
    if(!Building_IsValid(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "Invalid Building ID");

    if(Building_SetOwnable(id, ownable))
    {
		if(ownable)
			SendFormattedMessage(playerid, COLOR_GREEN, "Il building %d è ora acquistabile.", id);
		else
			SendFormattedMessage(playerid, COLOR_GREEN, "Il building %d non è più acquistabile.", id);
		Log(Account_GetName(playerid), "", "/abuildingownable", id);
    }
    else
    {
	   SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile eseguire il comando sul building %d.", id);
    }
    return 1;
}

flags:abuildingprice(CMD_RCON);
CMD:abuildingprice(playerid, params[])
{
    new id, price;
    if(sscanf(params, "ii", id, price))
	   return SendClientMessage(playerid, COLOR_ERROR, "/abuildingprice <buildingid> <price>");
    
    if(!Building_IsValid(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "Invalid Building ID");

    if(Building_SetPrice(id, price))
    {
		SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato il prezzo del building %d. Nuovo Prezzo: $%d", id, price);
		Log(Account_GetName(playerid), "", "/abuildingprice", id);
    }
    else
    {
	   SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile eseguire il comando sul building %d.", id);
    }
    return 1;
}

flags:abuildinglock(CMD_RCON);
CMD:abuildinglock(playerid, params[])
{
    new id, lock;
    if(sscanf(params, "ii", id, lock))
	   return SendClientMessage(playerid, COLOR_ERROR, "/abuildinglock <buildingid> <open: 0 - closed: 1>");
    
    if(!Building_IsValid(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "Invalid Building ID");

    Building_SetLocked(id, (lock > 0) ? (1) : (0));

    if(lock)
	   SendFormattedMessage(playerid, COLOR_GREEN, "Hai chiuso il building %d.", id);
    else
	   SendFormattedMessage(playerid, COLOR_GREEN, "Hai aperto il building %d.", id);
    Log(Account_GetName(playerid), "", "/abuildinglock", id);
    return 1;
}

/*flags:abdelete(CMD_RCON);
CMD:abdelete(playerid, params[])
{
    new id;
    if(sscanf(params, "i", id))
	   return SendClientMessage(playerid, COLOR_ERROR, "/abdelete <buildingid>");
    
    if(!Building_IsValid(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "Invalid Building ID");

    if(Building_Delete(id))
    {
	   SendFormattedMessage(playerid, COLOR_GREEN, "Hai rimosso il building id %d definitivamente.", id);
	   Log(Account_GetName(playerid), "", "/abdelete", id);
    }
    else
	   SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile eseguire il comando sul building id %d.", id);
    return 1;
}*/

flags:abuildingwelcome(CMD_RCON);
CMD:abuildingwelcome(playerid, params[])
{
    new id, text[MAX_WELCOME_TEXT_LENGTH];
    if(sscanf(params, "is[128]", id, text) || strlen(text) >= MAX_WELCOME_TEXT_LENGTH)
	   return SendClientMessage(playerid, COLOR_ERROR, "/abuildingwelcome <buildingid> <testo>");
    
    if(!Building_IsValid(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "Invalid Building ID");

	Building_SetWelcomeText(id, text);

	SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato il testo di benvenuto del building %d.", id);
	SendFormattedMessage(playerid, COLOR_GREEN, "Testo: %s", text);

    return 1;
}

flags:abuildingtype(CMD_RCON);
CMD:abuildingtype(playerid, params[])
{
    new id, type;
    if(sscanf(params, "ii", id, type))
	{
		SendClientMessage(playerid, COLOR_ERROR, "/abuildingtype <buildingid> <type>");
		SendClientMessage(playerid, COLOR_ERROR, "0: Store, 1: Paycheck, 2: Garage");
		return 1;
	}
    
    if(!Building_IsValid(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "Invalid Building ID");

	if(!Building_SetType(id, type))
		return SendClientMessage(playerid, COLOR_ERROR, "Tipo di edificio non valido.");

	SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato il tipo del building %d.", id);
	SendFormattedMessage(playerid, COLOR_GREEN, "Nuovo tipo: %d", type);

    return 1;
}