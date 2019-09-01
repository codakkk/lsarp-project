
#include <building\core>
#include <YSI_Coding\y_hooks>

flags:abuildingcmds(CMD_ADMIN);
CMD:abuildingcmds(playerid, params[])
{
    SendClientMessage(playerid, -1, "[BUILDINGS]: /abcreate - /abedit");
    return 1;
}

flags:gotobuilding(CMD_JR_MODERATOR);
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

flags:abcreate(CMD_ADMIN);
CMD:abcreate(playerid, params[])
{
    
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);

    new id = Building_Create(x, y, z, GetPlayerInterior(playerid), Character_GetVirtualWorld(playerid));
    Log(Account_GetName(playerid), "", "/abcreate", id);
    return 1;
}

flags:abdelete(CMD_DEVELOPER);
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
}


flags:abedit(CMD_ADMIN);
CMD:abedit(playerid, p[])
{
    new buildingid, command[16], params[256];
    if(sscanf(p, "ds[16]S()[256]", buildingid, command, params))
    {
	    SendClientMessage(playerid, COLOR_ERROR, "/abedit <buildingid> <pos - interior - price - lock - rmowner - name - type - ..");
        SendClientMessage(playerid, COLOR_ERROR, ".. faction - exit - ownable - welcometext >");
        return 0;
    }
    
    if(!Building_IsValid(buildingid))
        return SendClientMessage(playerid, COLOR_ERROR, "L'id del building inserito non è valido.");

    if(!strcmp(command, "pos", true))
    {
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai spostato le coordinate d'entrata dell'edificio %d.", buildingid);

        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);

        if(Building_SetPosition(buildingid, x, y, z, Character_GetVirtualWorld(playerid), GetPlayerInterior(playerid)))
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai spostato il building %d.", buildingid);
            Log(Account_GetName(playerid), "", "/abuildingentrance", buildingid);
        }
        else
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile spostare il building %d.", buildingid);
        }
    }
    else if(!strcmp(command, "price", true))
    {
        new price;
	    if(sscanf(params, "d", price))
            return SendClientMessage(playerid, COLOR_ERROR, "/abedit <buildingid> <price>");
	    if(Building_SetPrice(buildingid, price))
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato il prezzo del building %d. Nuovo Prezzo: $%d", buildingid, price);
            Log(Account_GetName(playerid), "", "/abedit price", buildingid);
        }
        else
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile eseguire il comando sul building %d.", buildingid);
        }
    }
    else if(!strcmp(command, "rmowner", true))
    {
	   SendFormattedMessage(playerid, COLOR_GREEN, "Hai resettato il proprietario del building %d.", buildingid);
       Building_ResetOwner(buildingid);
       Log(Account_GetName(playerid), "none", "/abedit rmowner", buildingid);
    }
    else if(!strcmp(command, "name", true))
    {
        new name[MAX_BUILDING_NAME];
        if(sscanf(params, "s[64]", name))
            return SendClientMessage(playerid, COLOR_ERROR, "/abedit <buildingid> name <name (max 64 characters)>");
        
        if(strlen(name) > MAX_BUILDING_NAME)
    	   return SendClientMessage(playerid, COLOR_ERROR, "Nome troppo lungo.");

        if(Building_SetName(buildingid, name))
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato il nome del building %d. Nuovo nome: %s", buildingid, name);
            Log(Account_GetName(playerid), "", "/abuildingname", buildingid);
        }
        else
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile settare il nome del building %d.", buildingid);
        }
    }
    else if(!strcmp(command, "type", true))
    {
        new type;
        if(sscanf(params, "d", type))
        {
            SendClientMessage(playerid, COLOR_ERROR, "/abedit <buildingid> type <typeid>");
            SendClientMessage(playerid, COLOR_ERROR, "0: Store, 1: Paycheck, 2: Garage, 3: Complex");
            return 0;
        }

        if(!Building_SetType(buildingid, type))
            return SendClientMessage(playerid, COLOR_ERROR, "Tipo di edificio non valido.");

        SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato il tipo del building %d.", buildingid);
        SendFormattedMessage(playerid, COLOR_GREEN, "Nuovo tipo: %d", type);
    }
    else if(!strcmp(command, "faction", true))
    {
        new factionid;
        if(sscanf(params, "d", factionid))
        {
            SendClientMessage(playerid, COLOR_ERROR, "/abedit <buildingid> faction <factionid>");
            return 0;
        }

        if(!Faction_IsValid(factionid))
		    return SendClientMessage(playerid, COLOR_ERROR, "La fazione inserita non è valida. Usa /afactionslist per una lista.");	
	
        Building_SetFaction(buildingid, factionid);

        if(factionid == INVALID_FACTION_ID)
            SendFormattedMessage(playerid, COLOR_GREEN, "L'edificio %d non appartiene più alla fazione ID %d.", buildingid, factionid);
        else
            SendClientMessageStr(playerid, COLOR_GREEN, str_format("Hai settato l'edificio %d appartenente alla fazione %S (%d).", buildingid, Faction_GetNameStr(factionid), factionid));
        Log(Account_GetName(playerid), "", "/abedit faction", buildingid);
    }
    else if(!strcmp(command, "exit", true))
    {
        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);

        if(Building_SetInterior(buildingid, x, y, z, GetPlayerInterior(playerid)))
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato l'interior del building id %d.", buildingid);
            Log(Account_GetName(playerid), "", "/abedit exit", buildingid);
        }
        else
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile settare l'interior del building %d.", buildingid);
        }
    }
    else if(!strcmp(command, "ownable", true))
    {
        new ownable;
        if(sscanf(params, "d", ownable))
            return SendClientMessage(playerid, COLOR_ERROR, "/abedit <buildingid> ownable <1: yes - 0: no>");

        if(Building_SetOwnable(buildingid, ownable))
        {
            if(ownable)
                SendFormattedMessage(playerid, COLOR_GREEN, "Il building %d è ora acquistabile.", buildingid);
            else
                SendFormattedMessage(playerid, COLOR_GREEN, "Il building %d non è più acquistabile.", buildingid);
            Log(Account_GetName(playerid), "", "/abedit ownable", buildingid);
        }
        else
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile eseguire il comando sul building %d.", buildingid);
        }
    }
    else if(!strcmp(command, "lock", true))
    {
        new lock;
        if(sscanf(params, "d", lock))
            return SendClientMessage(playerid, COLOR_ERROR, "/abedit <buildingid> lock <1: lock - 0: open>");

        Building_SetLocked(buildingid, (lock > 0) ? (1) : (0));

        if(lock)
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai chiuso il building %d.", buildingid);
        else
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai aperto il building %d.", buildingid);
        Log(Account_GetName(playerid), "", "/abedit lock", buildingid);
    }
    else if(!strcmp(command, "welcometext", true))
    {
        new welcomeText[MAX_WELCOME_TEXT_LENGTH];
        if(sscanf(params, "s[128]", welcomeText))
            return SendClientMessage(playerid, COLOR_ERROR, "/abedit <buildingid> welcometext <text (max 128 characters)>");

        Building_SetWelcomeText(buildingid, welcomeText);

        SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato il testo di benvenuto del building %d.", buildingid);
        SendFormattedMessage(playerid, COLOR_GREEN, "Testo: %s", welcomeText);
    }
    else if(!strcmp(command, "interior", true))
    {
        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);
        Building_SetInterior(buildingid, x, y, z, GetPlayerInterior(playerid));

        SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato l'interior del building %d alla posizione corrente.", buildingid);
    }
    else 
    {
        SendClientMessage(playerid, COLOR_ERROR, "/abedit <buildingid> <pos - interior - price - lock - rmowner - name - type - ..");
        SendClientMessage(playerid, COLOR_ERROR, ".. faction - exit - ownable - welcometext >");
    }
    return 1;
}