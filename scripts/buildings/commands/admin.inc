
#include <buildings\core>
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
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /gotobuilding [buildingid]");
    
    new Building:building = Building_Find(id);

	if(!Building_IsValid(building))
		return SendClientMessage(playerid, COLOR_ERROR, "Il business inserito non è valido.");
    
	new Float:x, Float:y, Float:z;
	Building_GetEnterPos(building, x, y, z);
	SetPlayerPos(playerid, x, y, z);
	SetPlayerInterior(playerid, Building_GetEnterInterior(building));
	Character_SetVirtualWorld(playerid, Building_GetEnterWorld(building));
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
	   return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /abdelete <buildingid>");
    
    new Building:building = Building_Find(id);

    if(!Building_IsValid(building))
	   return SendClientMessage(playerid, COLOR_ERROR, "Invalid Building ID");

    if(Building_Delete(building))
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
    new id, command[16], params[256];
    if(sscanf(p, "ds[16]S()[256]", id, command, params))
    {
	    SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /abedit [buildingid] [pos - interior - price - lock - rmowner - name - type - ..");
        SendClientMessage(playerid, COLOR_COMMAND_ERROR, ".. faction - exit - ownable - welcometext]");
        return 0;
    }
    
    new Building:building = Building_Find(id);

    if(!Building_IsValid(building))
        return SendClientMessage(playerid, COLOR_ERROR, "L'id del business inserito non è valido.");

    if(!strcmp(command, "pos", true))
    {
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai spostato le coordinate d'entrata dell'edificio %d.", id);

        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);

        if(Building_SetPosition(building, x, y, z, Character_GetVirtualWorld(playerid), GetPlayerInterior(playerid)))
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai spostato il building %d.", id);
            Log(Account_GetName(playerid), "", "/abuildingentrance", id);
        }
        else
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile spostare il building %d.", id);
        }
    }
    else if(!strcmp(command, "price", true))
    {
        new price;
	    if(sscanf(params, "d", price))
            return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /abedit <buildingid> <price>");
	    if(Building_SetPrice(building, price))
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato il prezzo del building %d. Nuovo Prezzo: $%d", id, price);
            Log(Account_GetName(playerid), "", "/abedit price", id);
        }
        else
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile eseguire il comando sul building %d.", id);
        }
    }
    else if(!strcmp(command, "rmowner", true))
    {
	   SendFormattedMessage(playerid, COLOR_GREEN, "Hai resettato il proprietario del building %d.", id);
       Building_ResetOwner(building);
       Log(Account_GetName(playerid), "none", "/abedit rmowner", id);
    }
    else if(!strcmp(command, "name", true))
    {
        new name[MAX_BUILDING_NAME];
        if(sscanf(params, "s[64]", name))
            return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /abedit <buildingid> name <name (max 64 characters)>");
        
        if(strlen(name) > MAX_BUILDING_NAME)
    	   return SendClientMessage(playerid, COLOR_ERROR, "Nome troppo lungo.");

        if(Building_SetName(building, name))
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato il nome del building %d. Nuovo nome: %s", id, name);
            Log(Account_GetName(playerid), "", "/abuildingname", id);
        }
        else
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile settare il nome del building %d.", id);
        }
    }
    else if(!strcmp(command, "type", true))
    {
        new type;
        if(sscanf(params, "d", type))
        {
            SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /abedit <buildingid> type <typeid>");
            SendClientMessage(playerid, COLOR_ERROR, "0: Store, 1: Paycheck, 2: Garage, 3: Complex, 4: Banca, 5: Pubblicità, 6: Benzina");
            return 0;
        }

        if(!Building_SetType(building, type))
            return SendClientMessage(playerid, COLOR_ERROR, "Tipo di edificio non valido.");

        SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato il tipo del building %d.", id);
        SendFormattedMessage(playerid, COLOR_GREEN, "Nuovo tipo: %d", type);
    }
    else if(!strcmp(command, "faction", true))
    {
        new factionid;
        if(sscanf(params, "d", factionid))
        {
            SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /abedit <buildingid> faction <factionid>");
            return 0;
        }

        if(factionid == INVALID_FACTION_ID)
        {
            Building_SetFaction(building, factionid);
            SendFormattedMessage(playerid, COLOR_GREEN, "L'edificio %d non appartiene più alla fazione ID %d.", id, factionid);
        }
        else
        {
            if(!Faction_IsValid(factionid))
                return SendClientMessage(playerid, COLOR_ERROR, "La fazione inserita non è valida. Usa /afactionslist per una lista.");	

            SendClientMessageStr(playerid, COLOR_GREEN, str_format("Hai settato l'edificio %d appartenente alla fazione %S (%d).", id, Faction_GetNameStr(factionid), factionid));
            Building_SetFaction(building, factionid);
        }
        Log(Account_GetName(playerid), "", "/abedit faction", id);
    }
    else if(!strcmp(command, "exit", true))
    {
        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);

        if(Building_SetInterior(building, x, y, z, GetPlayerInterior(playerid)))
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato l'interior del building id %d.", id);
            Log(Account_GetName(playerid), "", "/abedit exit", id);
        }
        else
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile settare l'interior del building %d.", id);
        }
    }
    else if(!strcmp(command, "ownable", true))
    {
        new ownable;
        if(sscanf(params, "d", ownable))
            return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /abedit <buildingid> ownable <1: yes - 0: no>");

        if(Building_SetOwnable(building, ownable))
        {
            if(ownable)
                SendFormattedMessage(playerid, COLOR_GREEN, "Il building %d è ora acquistabile.", id);
            else
                SendFormattedMessage(playerid, COLOR_GREEN, "Il building %d non è più acquistabile.", id);
            Log(Account_GetName(playerid), "", "/abedit ownable", id);
        }
        else
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile eseguire il comando sul building %d.", id);
        }
    }
    else if(!strcmp(command, "lock", true))
    {
        new lock;
        if(sscanf(params, "d", lock))
            return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /abedit <buildingid> lock <1: lock - 0: open>");

        Building_SetLocked(building, (lock > 0) ? (1) : (0));

        if(lock)
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai chiuso il building %d.", id);
        else
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai aperto il building %d.", id);
        Log(Account_GetName(playerid), "", "/abedit lock", id);
    }
    else if(!strcmp(command, "welcometext", true))
    {
        new welcomeText[MAX_WELCOME_TEXT_LENGTH];
        if(sscanf(params, "s[128]", welcomeText))
            return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /abedit <buildingid> welcometext <text (max 128 characters)>");

        Building_SetWelcomeText(building, welcomeText);

        SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato il testo di benvenuto del building %d.", id);
        SendFormattedMessage(playerid, COLOR_GREEN, "Testo: %s", welcomeText);
    }
    else if(!strcmp(command, "interior", true))
    {
        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);
        Building_SetInterior(building, x, y, z, GetPlayerInterior(playerid));

        SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato l'interior del building %d alla posizione corrente.", id);
    }
    else 
    {
        SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /abedit <buildingid> <pos - interior - price - lock - rmowner - name - type - ..");
        SendClientMessage(playerid, COLOR_ERROR, ".. faction - exit - ownable - welcometext >");
    }
    return 1;
}