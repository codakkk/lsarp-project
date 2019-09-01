
CMD:furniturecmds(playerid, params[])
{
    SendClientMessage(playerid, -1, "[FURNITURE]: (/arreda)mento - /listaobject - /textureoggetto");
    SendClientMessage(playerid, -1, "[FURNITURE]: /piazzaobject - /rinominaobject - /spostaoggetto");
    SendClientMessage(playerid, -1, "[FURNITURE]: /furnitureid - /idmobili - /fid");
    return 1;
}

flags:arredamento(CMD_ALIVE_USER);
CMD:arredamento(playerid, params[])
{
    if(EditObject_IsEditing(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se stai editando un oggetto.");
    
    new houseid = Character_CanFurnish(playerid, true);

    if(!House_IsValid(houseid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei nei pressi di una casa arredabile.");
    
    Character_SetFurnitureLinkedTo(playerid, House_GetExitWorld(houseid));
    Dialog_Show(playerid, Dialog_Furniture, DIALOG_STYLE_LIST, "Arredamento", "Mobili piazzati\nPiazza", "Avanti", "Chiudi");
    return 1;
}
alias:arredamento("arreda");

flags:furnitureid(CMD_ALIVE_USER);
CMD:furnitureid(playerid, params[])
{
    if(EditObject_IsEditing(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
    
    if(!House_IsValid(Character_CanFurnish(playerid, true)) && !Player_IsAdminDuty(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei nei pressi di una casa arredabile.");

    if(Furniture_IsShowingInfo(playerid))
    {
        Furniture_DeleteLabels(playerid);
        SendClientMessage(playerid, COLOR_GREEN, "Hai disabilitato le informazioni sui mobili.");
    }
    else
    {
        Furniture_CreateLabels(playerid);
        SendClientMessage(playerid, COLOR_GREEN, "Hai abilitato le informazioni sui mobili.");
    }
    return 1;
}
alias:furnitureid("fid", "idmobili");

flags:listaobject(CMD_ALIVE_USER);
CMD:listaobject(playerid, params[])
{
    if(EditObject_IsEditing(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se stai editando un oggetto.");
    
    if(!House_IsValid(Character_CanFurnish(playerid)))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei nei pressi di una casa arredabile.");
    
    Character_ShowFurnitures(playerid, 0);
    return 1;
}

flags:piazzaobject(CMD_ALIVE_USER);
CMD:piazzaobject(playerid, params[])
{
    if(EditObject_IsEditing(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se stai editando un oggetto.");
    
    new houseid = Character_CanFurnish(playerid, true);

    if(!House_IsValid(houseid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei nei pressi di una casa arredabile.");
    
    Character_SetFurnitureLinkedTo(playerid, House_GetExitWorld(houseid));
    Furniture_ShowCategoryList(playerid);
    return 1;
}

flags:rinominaobject(CMD_ALIVE_USER);
CMD:rinominaobject(playerid, params[])
{
    if(EditObject_IsEditing(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se stai editando un oggetto.");
    new id, name[64];
    
    if(sscanf(params, "ds[64]", id, name) || strlen(name) < 3)
        return SendClientMessage(playerid, COLOR_ERROR, "/rinominaobject <localid> <nome (min 3 caratteri - max 64 caratteri)>");
    
    new houseid = Character_CanFurnish(playerid, true);

    if(!House_IsValid(houseid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei nei pressi di una casa arredabile.");

    new objectid = Furniture_GetFurnitureObjectID(Character_GetVirtualWorld(playerid), id);

    if(Furniture_Rename(Character_GetVirtualWorld(playerid), objectid, name))
    {
        return SendFormattedMessage(playerid, COLOR_GREEN, "Hai rinominato l'oggetto. Nuovo nome: %s", name);
    }
    return SendClientMessage(playerid, COLOR_ERROR, "Non è stato possibile rinominare l'oggetto. ID invalido.");
}

flags:spostaoggetto(CMD_ALIVE_USER);
CMD:spostaoggetto(playerid, params[])
{
    if(EditObject_IsEditing(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
    
    new id;
    if(sscanf(params, "d", id))
        return SendClientMessage(playerid, COLOR_ERROR, "/spostaoggetto <local id>");
    
    new houseid = Character_CanFurnish(playerid, true);

    if(!House_IsValid(houseid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei nei pressi di una casa arredabile.");

    new 
        world = Character_GetVirtualWorld(playerid),
        objectid = Furniture_GetFurnitureObjectID(world, id);

    if(objectid == -1)
        return SendClientMessage(playerid, COLOR_ERROR, "L'id inserito non è valido.");

    new Float:x, Float:y, Float:z;
    GetDynamicObjectPos(objectid, x, y, z);

    if(GetPlayerDistanceFromPoint(playerid, x, y, z) > 25.0)
        return SendClientMessage(playerid, COLOR_ERROR, "Sei troppo distante dall'oggetto.");
    
    SendClientMessage(playerid, COLOR_WHITE, "Premi {FF6347}ESC{FFFFFF} per annullare e {FF6347}SPAZIO{FFFFFF} per spostare la visuale. Clicca sul {FF6347}FLOPPY{FFFFFF} per salvare.");
    EditObject_Start(playerid, EDIT_TYPE_FURNITURE, objectid, 0);
    return 1;
}

flags:textureoggetto(CMD_ALIVE_USER);
CMD:textureoggetto(playerid, params[])
{
    if(EditObject_IsEditing(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
    
    new id, index, modelid, txd[64], texture[64];
    if(sscanf(params, "ddds[64]s[64]", id, index, modelid, txd, texture))
        return SendClientMessage(playerid, COLOR_ERROR, "/textureoggetto <local id> <index> <modelid> <txd> <texture>");
    
    new houseid = Character_CanFurnish(playerid, true);

    if(!House_IsValid(houseid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei nei pressi di una casa arredabile.");

    new 
        world = Character_GetVirtualWorld(playerid),
        objectid = Furniture_GetFurnitureObjectID(world, id);

    if(objectid == -1)
        return SendClientMessage(playerid, COLOR_ERROR, "L'id inserito non è valido.");

    new Float:x, Float:y, Float:z;
    GetDynamicObjectPos(objectid, x, y, z);

    if(GetPlayerDistanceFromPoint(playerid, x, y, z) > 25.0)
        return SendClientMessage(playerid, COLOR_ERROR, "Sei troppo distante dall'oggetto.");
    
    Furniture_SetTexture(objectid, index, modelid, txd, texture, -1);
    return 1;
}

flags:aplacefurniture(CMD_MODERATOR);
CMD:aplacefurniture(playerid, params[])
{
    if(EditObject_IsEditing(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se stai editando un oggetto.");
    
    new modelid;
    if(sscanf(params, "d", modelid))
        return SendClientMessage(playerid, COLOR_ERROR, "/aplacefurniture <modelid>");
    
    new worldid = Character_GetVirtualWorld(playerid), Float:x, Float:y, Float:z;

    GetPlayerPos(playerid, x, y, z);

    new objectid = Furniture_CreateTemporary(modelid, x, y, z, 0.0, 0.0, 0.0, worldid, GetPlayerInterior(playerid), worldid);
    Furniture_Store(objectid, modelid, playerid, worldid);

    EditObject_Start(playerid, EDIT_TYPE_FURNITURE, objectid, 0);
    return 1;
}

flags:adeletefurniture(CMD_MODERATOR);
CMD:adeletefurniture(playerid, params[])
{
    new objectid;
    if(sscanf(params, "d", objectid))
        return SendClientMessage(playerid, COLOR_ERROR, "/adeletefurniture <objectid>");
    if(Furniture_RemoveByObjectID(objectid))
    {
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai rimosso il mobile id %d.", objectid);
        Furniture_UpdateInfoList(playerid);
    }
    else
    {
        SendClientMessage(playerid, COLOR_ERROR, "L'id inserito non è di un mobile.");
    }
    return 1;
}

flags:atextureobject(CMD_MODERATOR);
CMD:atextureobject(playerid, params[])
{
    if(EditObject_IsEditing(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se stai editando un oggetto.");
    
    new objectid, index, modelid, txd[32], texture[32];
    if(sscanf(params, "ddds[32]s[32]", objectid, index, modelid, txd, texture))
        return SendClientMessage(playerid, COLOR_ERROR, "/atextureobject <objectid> <index> <modelid> <txd name> <texture>");
    
    if(!Furniture_IsValid(objectid))
        return SendClientMessage(playerid, COLOR_ERROR, "L'id inserito non è di un mobile."), 0;

    Furniture_SetTexture(objectid, index, modelid, txd, texture, -1);

    SendFormattedMessage(playerid, -1, "Hai settato la texture del mobile id %d con successo.", objectid);
    return 1;
}