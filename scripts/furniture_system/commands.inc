
CMD:furniturecmds(playerid, params[])
{
    SendClientMessage(playerid, -1, "[FURNITURE]: (/arreda)mento - /furnitureid (fid)");
    SendClientMessage(playerid, -1, "[FURNITURE]: (/frot)ate - (/fpos)ition - /fname - /fmove");
    SendClientMessage(playerid, -1, "[FURNITURE]: /ftexture - /fdelete");
    //SendClientMessage(playerid, -1, "[FURNITURE]: (/arreda)mento - /listaobject - /textureoggetto");
    //SendClientMessage(playerid, -1, "[FURNITURE]: /piazzaobject - /rinominaobject - /spostaoggetto");
    //SendClientMessage(playerid, -1, "[FURNITURE]: /furnitureid - /idmobili - /fid");
    return 1;
}
alias:furniturecmds("mobilicmds");

flags:arredamento(CMD_ALIVE_USER);
CMD:arredamento(playerid, params[])
{
    if(EditObject_IsEditing(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se stai editando un oggetto.");

    Character_ShowFurnitureDialog(playerid);
    return 1;
}
alias:arredamento("arreda");

flags:furnitureid(CMD_ALIVE_USER);
CMD:furnitureid(playerid, params[])
{
    if(EditObject_IsEditing(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
    
    new bool:isExterior, EntityType:type, entityid;
    new bool:result = Character_GetFurnitureWorld(playerid, isExterior, type, entityid, .onlyAllowed=true, .worldAllowed=true);

    if(!result)
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei nei pressi di una casa arredabile che ti appartiene.");
    
    Character_ToggleFurnitureLabels(playerid, !Furniture_IsShowingLabels(playerid));
    if(Furniture_IsShowingLabels(playerid))
        SendClientMessage(playerid, COLOR_GREEN, "Hai abilitato le informazioni sui mobili.");
    else
        SendClientMessage(playerid, COLOR_GREEN, "Hai disabilitato le informazioni sui mobili.");
    return 1;
}
alias:furnitureid("fid", "idmobili");

flags:frotate(CMD_ALIVE_USER);
CMD:frotate(playerid, params[])
{
    new objectid, Float:rx, Float:ry, Float:rz;
    if(sscanf(params, "dfff", objectid, rx, ry, rz))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: (/frot)ate [objectid] [rot x] [rot y] [rot z]");
    
    new bool:isExterior, EntityType:entityType, entityid;
    new bool:furnable = Character_GetFurnitureWorld(playerid, isExterior, entityType, entityid, .onlyAllowed=true, .worldAllowed=true);

    if(!furnable)
        return SendClientMessage(playerid, COLOR_ERROR, "Non ti trovi nei dintorni di un posto arredabile.");

    if(!IsFurniture(objectid) || !Furniture_IsOfWorld(objectid, entityType, entityid, isExterior))
        return SendClientMessage(playerid, COLOR_ERROR, "L'objectid inserito non è un tuo mobile modificabile.");

    Furniture_SetRotation(objectid, rx, ry, rz);
    Furniture_Update(objectid, playerid);
    return 1;
}   
alias:frotate("frot");

flags:fposition(CMD_ALIVE_USER);
CMD:fposition(playerid, params[])
{
    new objectid, Float:x, Float:y, Float:z;
    if(sscanf(params, "dfff", objectid, x, y, z))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: (/frot)ate [objectid] [x] [y] [z]");
    
    new bool:isExterior, EntityType:entityType, entityid;
    new bool:furnable = Character_GetFurnitureWorld(playerid, isExterior, entityType, entityid, .onlyAllowed=true, .worldAllowed=true);

    if(!furnable)
        return SendClientMessage(playerid, COLOR_ERROR, "Non ti trovi nei dintorni di un posto arredabile.");

    if(!IsFurniture(objectid) || !Furniture_IsOfWorld(objectid, entityType, entityid, isExterior))
        return SendClientMessage(playerid, COLOR_ERROR, "L'objectid inserito non è un tuo mobile modificabile.");

    if(!Furniture_ValidatePos(objectid, x, y, z))
        return SendClientMessage(playerid, COLOR_ERROR, "Il mobile non puo' essere piazzato cosi lontano.");

    Furniture_SetPosition(objectid, x, y, z);
    Furniture_Update(objectid, playerid);
    return 1;
}   
alias:fposition("fpos");

flags:fname(CMD_ALIVE_USER);
CMD:fname(playerid, params[])
{
    new objectid, name[64];
    if(sscanf(params, "ds[64]", objectid, name) || strlen(name) >= 64)
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /fname [objectid] [object name]");
    
    new bool:isExterior, EntityType:entityType, entityid;
    new bool:furnable = Character_GetFurnitureWorld(playerid, isExterior, entityType, entityid, .onlyAllowed=true, .worldAllowed=true);

    if(!furnable)
        return SendClientMessage(playerid, COLOR_ERROR, "Non ti trovi nei dintorni di un posto arredabile.");

    if(!IsFurniture(objectid) || !Furniture_IsOfWorld(objectid, entityType, entityid, isExterior))
        return SendClientMessage(playerid, COLOR_ERROR, "L'objectid inserito non è un tuo mobile modificabile.");

    Furniture_SetName(objectid, name);
    Furniture_Update(objectid, playerid);
    SendClientMessage(playerid, -1, "Il mobile è stato rinominato con successo.");
    return 1;
}   
alias:fname("fnome");

flags:fdelete(CMD_ALIVE_USER);
CMD:fdelete(playerid, params[])
{
    new objectid;
    if(sscanf(params, "d", objectid))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /fdelete [objectid]");
    
    new bool:isExterior, EntityType:entityType, entityid;
    new bool:furnable = Character_GetFurnitureWorld(playerid, isExterior, entityType, entityid, .onlyAllowed=true, .worldAllowed=true);

    if(!furnable)
        return SendClientMessage(playerid, COLOR_ERROR, "Non ti trovi nei dintorni di un posto arredabile.");

    if(!IsFurniture(objectid) || !Furniture_IsOfWorld(objectid, entityType, entityid, isExterior))
        return SendClientMessage(playerid, COLOR_ERROR, "L'objectid inserito non è un tuo mobile modificabile.");

    Furniture_Remove(playerid, objectid);
    return 1;
}   
alias:fdelete("fdel", "frimuovi", "fremove");

flags:fmove(CMD_ALIVE_USER);
CMD:fmove(playerid, params[])
{
    new objectid;
    if(sscanf(params, "d", objectid))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /fmove [objectid]");
    
    new bool:isExterior, EntityType:entityType, entityid;
    new bool:furnable = Character_GetFurnitureWorld(playerid, isExterior, entityType, entityid, .onlyAllowed=true, .worldAllowed=true);

    if(!furnable)
        return SendClientMessage(playerid, COLOR_ERROR, "Non ti trovi nei dintorni di un posto arredabile.");

    if(!IsFurniture(objectid) || !Furniture_IsOfWorld(objectid, entityType, entityid, isExterior))
        return SendClientMessage(playerid, COLOR_ERROR, "L'objectid inserito non è un tuo mobile modificabile.");
        
    EditObject_Start(playerid, EDIT_TYPE_FURNITURE, objectid, 0);
    return 1;
}   
alias:fmove("fmuovi", "fsposta");

flags:ftexture(CMD_ALIVE_USER);
CMD:ftexture(playerid, params[])
{
    new objectid, index, modelid, txd[64], texture[64];
    if(sscanf(params, "ddds[64]s[64]", objectid, index, modelid, txd, texture))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /ftexture [objectid] [index] [modelid] [txd] [texture]");
    
    new bool:isExterior, EntityType:entityType, entityid;
    new bool:furnable = Character_GetFurnitureWorld(playerid, isExterior, entityType, entityid, .onlyAllowed=true, .worldAllowed=true);

    if(!furnable)
        return SendClientMessage(playerid, COLOR_ERROR, "Non ti trovi nei dintorni di un posto arredabile.");

    if(!IsFurniture(objectid) || !Furniture_IsOfWorld(objectid, entityType, entityid, isExterior))
        return SendClientMessage(playerid, COLOR_ERROR, "L'objectid inserito non è un tuo mobile modificabile.");
    
    Furniture_SetTexture(objectid, index, modelid, txd, texture, -1);
    SendClientMessage(playerid, -1, "Hai modificato la texture del mobile.");
    return 1;
}

flags:aplacefurniture(CMD_MODERATOR);
CMD:aplacefurniture(playerid, params[])
{
    if(EditObject_IsEditing(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se stai editando un oggetto.");
    
    new modelid;
    if(sscanf(params, "d", modelid))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /aplacefurniture <modelid>");
    
    new EntityType:entityType = ENTITY_TYPE_NONE, entityid, bool:isExterior;

    if(!Character_GetFurnitureWorld(playerid, isExterior, entityType, entityid, .onlyAllowed=false, .worldAllowed=true))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");

    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);

    new objectid = Furniture_CreateTemporary(modelid, x, y, z, 0.0, 0.0, 0.0, Character_GetVirtualWorld(playerid), GetPlayerInterior(playerid), isExterior, entityType, entityid);

    Furniture_Store(objectid, playerid);
    EditObject_Start(playerid, EDIT_TYPE_FURNITURE, objectid, 0);
    return 1;
}

flags:adeletefurniture(CMD_MODERATOR);
CMD:adeletefurniture(playerid, params[])
{
    new objectid;
    if(sscanf(params, "d", objectid))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /adeletefurniture <objectid>");
    if(Furniture_Delete(objectid))
    {
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai rimosso il mobile id %d.", objectid);
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
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /atextureobject <objectid> <index> <modelid> <txd name> <texture>");
    
    if(!Furniture_IsValid(objectid))
        return SendClientMessage(playerid, COLOR_ERROR, "L'id inserito non è di un mobile."), 0;

    Furniture_SetTexture(objectid, index, modelid, txd, texture, -1);

    SendFormattedMessage(playerid, -1, "Hai settato la texture del mobile id %d con successo.", objectid);
    return 1;
}