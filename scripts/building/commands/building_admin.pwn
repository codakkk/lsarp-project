flags:abuildingcmds(CMD_RCON);
CMD:abuildingcmds(playerid, params[])
{
    SendClientMessage(playerid, -1, "[BUILDINGS]: /abcreate - /abinterior - /abpos - /absetname");
    SendClientMessage(playerid, -1, "[BUILDINGS]: /absetownable - /absetprice - /ablock - /absetwelcome");
    SendClientMessage(playerid, -1, "[BUILDINGS]: /abdelete");
    return 1;
}

flags:abcreate(CMD_RCON);
CMD:abcreate(playerid, params[])
{
    new freeid = Iter_Free(Buildings);
    if(freeid < 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Sono stati creati troppi edifici!");
    
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);

    new id = Building_Create("SET_NAME", x, y, z, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));
    Log(AccountInfo[playerid][aName], "", "/abcreate", id);
    return 1;
}

flags:abinterior(CMD_RCON);
CMD:abinterior(playerid, params[])
{
    new id;
    if(sscanf(params, "i", id))
        return SendClientMessage(playerid, COLOR_ERROR, "/abinterior <buildingid>");
    
    if(!Building_IsValid(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Invalid Building ID");
    
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);

    if(Building_SetInterior(id, x, y, z, GetPlayerInterior(playerid)))
    {
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato l'interior del building id %d.", id);
        Log(AccountInfo[playerid][aName], "", "/abinterior", id);
    }
    else
    {
        SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile settare l'interior del building %d.", id);
    }
    return 1;
}

flags:abpos(CMD_RCON);
CMD:abpos(playerid, params[])
{
    new id;
    if(sscanf(params, "i", id))
        return SendClientMessage(playerid, COLOR_ERROR, "/abpos <buildingid>");
    
    if(!Building_IsValid(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Invalid Building ID");
    
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);

    if(Building_SetPosition(id, x, y, z, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid)))
    {
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai spostato il building %d.", id);
        Log(AccountInfo[playerid][aName], "", "/abpos", id);
    }
    else
    {
        SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile spostare il building %d.", id);
    }
    return 1;
}

flags:absetname(CMD_RCON);
CMD:absetname(playerid, params[])
{
    new id, name[64];
    if(sscanf(params, "is[64]", id, name))
        return SendClientMessage(playerid, COLOR_ERROR, "/absetname <buildingid> <name>");
    
    if(!Building_IsValid(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Invalid Building ID");
    
    if(strlen(name) > MAX_BUILDING_NAME)
        return SendClientMessage(playerid, COLOR_ERROR, "Nome troppo lungo!");

    if(Building_SetName(id, name))
    {
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato il nome del building %d. Nuovo nome: %s", id, name);
        Log(AccountInfo[playerid][aName], "", "/absetname", id);
    }
    else
    {
        SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile settare il nome del building %d.", id);
    }
    return 1;
}

flags:absetownable(CMD_RCON);
CMD:absetownable(playerid, params[])
{
    new id, ownable;
    if(sscanf(params, "ii", id, ownable))
        return SendClientMessage(playerid, COLOR_ERROR, "/absetname <buildingid> <ownable: 0:no 1:yes>");
    
    if(!Building_IsValid(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Invalid Building ID");

    if(Building_SetOwnable(id, ownable))
    {
        if(ownable)
            SendFormattedMessage(playerid, COLOR_GREEN, "Il building %d è ora acquistabile!", id);
        else
            SendFormattedMessage(playerid, COLOR_GREEN, "Il building %d non è più acquistabile!", id);
        Log(AccountInfo[playerid][aName], "", "/absetownable", id);
    }
    else
    {
        SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile eseguire il comando sul building %d.", id);
    }
    return 1;
}

flags:absetprice(CMD_RCON);
CMD:absetprice(playerid, params[])
{
    new id, price;
    if(sscanf(params, "ii", id, price))
        return SendClientMessage(playerid, COLOR_ERROR, "/absetprice <buildingid> <price>");
    
    if(!Building_IsValid(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Invalid Building ID");

    if(Building_SetPrice(id, price))
    {
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato il prezzo del building %d. Nuovo Prezzo: $%d", id, price);
        Log(AccountInfo[playerid][aName], "", "/absetprice", id);
    }
    else
    {
        SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile eseguire il comando sul building %d.", id);
    }
    return 1;
}

flags:ablock(CMD_RCON);
CMD:ablock(playerid, params[])
{
    new id, open;
    if(sscanf(params, "ii", id, open))
        return SendClientMessage(playerid, COLOR_ERROR, "/ablock <buildingid> <open: 0 - closed: 1>");
    
    if(!Building_IsValid(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Invalid Building ID");

    BuildingInfo[id][bLocked] = (open > 0) ? (1) : (0);
    if(open)
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai aperto il building %d.", id);
    else
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai chiuso il building %d.", id);
    Log(AccountInfo[playerid][aName], "", "/ablock", id);
    return 1;
}

flags:abdelete(CMD_RCON);
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
        Log(AccountInfo[playerid][aName], "", "/abdelete", id);
    }
    else
        SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile eseguire il comando sul building id %d.", id);
    return 1;
}

flags:absetwelcome(CMD_RCON);
CMD:absetwelcome(playerid, params[])
{
    new id, text[128];
    if(sscanf(params, "is[128]", id, text) || strlen(text) >= MAX_WELCOME_TEXT_LENGTH)
        return SendClientMessage(playerid, COLOR_ERROR, "/absetwelcome <buildingid> <testo>");
    
    if(!Building_IsValid(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Invalid Building ID");

    if(Building_Delete(id))
    {
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai rimosso il building id %d definitivamente.", id);
        Log(AccountInfo[playerid][aName], "", "/absetwelcome", id);
    }
    else
        SendFormattedMessage(playerid, COLOR_GREEN, "Non è stato possibile eseguire il comando sul building id %d.", id);
    return 1;
}