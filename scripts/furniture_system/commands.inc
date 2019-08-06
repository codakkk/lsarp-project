
CMD:furniturecmds(playerid, params[])
{
    SendClientMessage(playerid, -1, "[FURNITURE]: (/arreda)mento - /showid - /listaobject - /textureoggetto");
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
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei in una tua casa.");
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
    
    if(!Character_CanFurnish(playerid, true))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei in una tua casa.");

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
    
    if(!Character_CanFurnish(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei in una tua casa.");
    
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
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei in una tua casa.");
    
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
    
    if(!Character_CanFurnish(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei in una tua casa.");

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
    
    if(!Character_CanFurnish(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei in una tua casa.");

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
    
    if(!Character_CanFurnish(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei in una tua casa.");

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

flags:aplaceobject(CMD_MODERATOR);
CMD:aplaceobject(playerid, params[])
{
    
    return 1;
}