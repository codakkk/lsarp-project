
flags:a(CMD_JR_MODERATOR);
CMD:a(playerid, params[])
{
    if(pDisableAdminAlerts[playerid])
        return SendClientMessage(playerid, COLOR_ERROR, "Hai disabilitato i messaggi admin.");
    if(isnull(params) || strlen(params) > 128)
        return SendClientMessage(playerid, COLOR_ERROR, "/a <testo>");
    SendMessageToAdmins(0, COLOR_YELLOW, "[ADMIN-CHAT] %s (%s): %s", AccountInfo[playerid][aName], PlayerInfo[playerid][pName], params);
    return 1;
}

flags:disablealerts(CMD_JR_MODERATOR);
CMD:disablealerts(playerid, params[])
{
    pDisableAdminAlerts[playerid] = !pDisableAdminAlerts[playerid];
    if(pDisableAdminAlerts[playerid])
        SendClientMessage(playerid, COLOR_YELLOW, "Hai disabilitato i messaggi admin.");
    else
        SendClientMessage(playerid, COLOR_YELLOW, "Hai abilitato i messaggi admin.");
    return 1;
}

flags:aduty(CMD_JR_MODERATOR);
CMD:aduty(playerid, params[])
{
    if(pAdminDuty[playerid])
    {
        pAdminDuty[playerid] = 0;
        SetPlayerName(playerid, PlayerInfo[playerid][pName]);
        AC_SetPlayerHealth(playerid, 100);
        AC_SetPlayerArmour(playerid, 0);
        SendMessageToAdmins(0, COLOR_ADMINCHAT, "[ADMIN] %s (%d) non è più in servizio.", AccountInfo[playerid][aName], playerid);
        if(IsValidDynamic3DTextLabel(pAdminDuty3DText[playerid]))
        {
            DestroyDynamic3DTextLabel(pAdminDuty3DText[playerid]);
            pAdminDuty3DText[playerid] = Text3D:INVALID_3DTEXT_ID;
        }        
    }    
    else
    {
        pAdminDuty[playerid] = 1;
        SetPlayerName(playerid, AccountInfo[playerid][aName]);
        AC_SetPlayerHealth(playerid, 9999);
        AC_SetPlayerArmour(playerid, 9999);
        //pc_cmd_jetpack(playerid, NULL);
        SendMessageToAdmins(0, COLOR_ADMINCHAT, "[ADMIN] %s (%d) è in servizio.", AccountInfo[playerid][aName], playerid);
        if(IsValidDynamic3DTextLabel(pAdminDuty3DText[playerid]))
        {
            DestroyDynamic3DTextLabel(pAdminDuty3DText[playerid]);
            pAdminDuty3DText[playerid] = Text3D:INVALID_3DTEXT_ID;
        }
        pAdminDuty3DText[playerid] = CreateDynamic3DTextLabel("Admin", COLOR_ADMINDUTY, 0, 0, 0.5, 30.0, playerid);
    }
    return 1;
}

flags:setskin(CMD_JR_MODERATOR);
CMD:setskin(playerid, params[])
{
    new id, skid;
    if(sscanf(params, "ud", id, skid))
        return SendClientMessage(playerid, COLOR_ERROR, "> /setskin <playerid/nome> <skin-id>");
    
    if(id < 0 || id >= MAX_PLAYERS || !gAccountLogged[id] || !gCharacterLogged[id])
        return SendClientMessage(playerid, COLOR_ERROR, "> L'utente non è collegato!");
    
    PlayerInfo[id][pSkin] = skid;
    SetPlayerSkin(id, skid);
    // Send admin alert (?)
    // Log (?)
    Character_Save(id);
    return 1;
}

flags:setadmin(CMD_RCON);
CMD:setadmin(playerid, params[])
{
    new id, lvl;
    if(sscanf(params, "ud", id, lvl))
        return SendClientMessage(playerid, COLOR_ERROR, "> /setadmin <playerid/nome> <level (0 - 5)>");

    if(id < 0 || id >= MAX_PLAYERS || !gAccountLogged[id] || !gCharacterLogged[id])
        return SendClientMessage(playerid, COLOR_ERROR, "> L'utente non è collegato!");
    if(lvl < 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Livello Admin non valido!");
    if(lvl == 0)
    {
        SendFormattedMessage(id, COLOR_ERROR, "> [ADMIN] %s (%d) ti ha rimosso dallo staff.", AccountInfo[playerid][aName], playerid);
        SendFormattedMessage(playerid, COLOR_ERROR, "> Hai rimosso %s (%d) dallo staff.", AccountInfo[id][aName], id);
    }
    else
    {
        //new ln[] = ;
        SendFormattedMessage(id, COLOR_GREEN, "> [ADMIN] %s (%d) ti ha settato %s.", AccountInfo[playerid][aName], playerid, GetAdminLevelName(lvl));
        SendFormattedMessage(playerid, COLOR_GREEN, "> %s (%d) è stato settato %s.", AccountInfo[id][aName], id, GetAdminLevelName(lvl));
    }
    Log(AccountInfo[playerid][aName], AccountInfo[id][aName], "/setadmin", lvl);
    AccountInfo[id][aAdmin] = lvl;
    SaveAccountData(id);
    return 1;
}

flags:givemoney(CMD_RCON);
CMD:givemoney(playerid, params[])
{
    new id, hp;
    if(sscanf(params, "ud", id, hp))
        return SendClientMessage(playerid, COLOR_ERROR, "> /givemoney <playerid/nome> <ammontare>");
    
    if(id < 0 || id >= MAX_PLAYERS || !gAccountLogged[id] || !gCharacterLogged[id])
        return SendClientMessage(playerid, COLOR_ERROR, "> L'utente non è collegato!");
    
    AC_GivePlayerMoney(id, hp);
    // Send admin alert (?)
    SendMessageToAdmins(1, COLOR_YELLOW, "[ADMIN-ALERT] %s (%s) ha dato $%d a %s (%d).", AccountInfo[playerid][aName], Character_GetOOCName(playerid), hp, Character_GetOOCName(id), id);
    SendFormattedMessage(id, COLOR_GREEN, "[ADMIN] %s (%s) ti ha givato $%d.", AccountInfo[playerid][aName], Character_GetOOCName(playerid), hp);
    Log(AccountInfo[playerid][aName], PlayerInfo[id][pName], "/givemoney", hp);
    Character_Save(id);
    return 1;
}

flags:sethp(CMD_JR_MODERATOR);
CMD:sethp(playerid, params[])
{
    new id, hp;
    if(sscanf(params, "ud", id, hp))
        return SendClientMessage(playerid, COLOR_ERROR, "> /sethp <playerid/nome> <health>");
    
    if(id < 0 || id >= MAX_PLAYERS || !gAccountLogged[id] || !gCharacterLogged[id])
        return SendClientMessage(playerid, COLOR_ERROR, "> L'utente non è collegato!");
    
    if(hp <= -INFINITY || hp >= INFINITY)
        return SendClientMessage(playerid, COLOR_ERROR, "Valore HP non valido!");

    AC_SetPlayerHealth(id, hp);
    SendMessageToAdmins(0, COLOR_YELLOW, "[ADMIN-ALERT] %s (%s) ha settato gli HP di %s (%d) a %d.", AccountInfo[playerid][aName], Character_GetOOCName(playerid), Character_GetOOCName(id), id, hp);
    SendFormattedMessage(playerid, COLOR_GREEN, "[ADMIN] %s (%s) ti ha settato gli HP a %d.", AccountInfo[playerid][aName], Character_GetOOCName(playerid), hp);
    Log(AccountInfo[playerid][aName], PlayerInfo[id][pName], "/sethp", hp);
    Character_Save(id);
    return 1;
}
alias:sethp("sethealth");

flags:setarmour(CMD_JR_MODERATOR);
CMD:setarmour(playerid, params[])
{
    new id, hp;
    if(sscanf(params, "ud", id, hp))
        return SendClientMessage(playerid, COLOR_ERROR, "> /setarmour <playerid/nome> <armour>");
    
    if(id < 0 || id >= MAX_PLAYERS || !gAccountLogged[id] || !gCharacterLogged[id])
        return SendClientMessage(playerid, COLOR_ERROR, "> L'utente non è collegato!");

    if(hp <= -INFINITY || hp >= INFINITY)
        return SendClientMessage(playerid, COLOR_ERROR, "Valore Armatura non valido!");

    AC_SetPlayerArmour(id, hp);
    SendMessageToAdmins(0, COLOR_YELLOW, "[ADMIN-ALERT] %s (%s) ha settato l'armatura di %s (%d) a %d.", AccountInfo[playerid][aName], Character_GetOOCName(playerid), Character_GetOOCName(id), id, hp);
    SendFormattedMessage(playerid, COLOR_GREEN, "[ADMIN] %s (%s) ti ha settato l'armatura a %d.", AccountInfo[playerid][aName], Character_GetOOCName(playerid), hp);
    Log(AccountInfo[playerid][aName], PlayerInfo[id][pName], "/setarmour", hp);
    Character_Save(id);
    return 1;
}
alias:setarmour("setap");

flags:veh(CMD_JR_MODERATOR);
CMD:veh(playerid, params[])
{
    new veh;
    if(sscanf(params, "d", veh))
        return SendClientMessage(playerid, COLOR_ERROR, "> /veh <modelid>");
    if(gAdminVehicle[playerid] != 0)
        DestroyVehicle(gAdminVehicle[playerid]);
    new 
        Float:_x, 
        Float:_y, 
        Float:_z;
    GetPlayerPos(playerid, _x, _y, _z);

    gAdminVehicle[playerid] = CreateVehicle(veh, _x, _y, _z, 0, 0, 3, 0, 0);
    PutPlayerInVehicle(playerid, gAdminVehicle[playerid], 0);
    Vehicle_SetEngineOn(gAdminVehicle[playerid]);
    Log(AccountInfo[playerid][aName], "", "/veh", veh);
    return 1;
}

flags:jetpack(CMD_JR_MODERATOR);
CMD:jetpack(playerid, params[])
{
    new id;
    if(sscanf(params, "d", id))
    {
        SetPlayerSpecialAction(playerid, SPECIAL_ACTION_USEJETPACK);
    }
    else
    {
        if(id < 0 || id >= MAX_PLAYERS || !gAccountLogged[id] || !gCharacterLogged[id])
            return SendClientMessage(playerid, COLOR_ERROR, "> L'utente non è collegato!");
        SetPlayerSpecialAction(id, SPECIAL_ACTION_USEJETPACK);
    }
    gPlayerJetpack[playerid] = 1;
    return 1;
}

flags:setvhp(CMD_JR_MODERATOR);
CMD:setvhp(playerid, params[])
{
    new id, Float:hp;
    if(sscanf(params, "df", id, hp))
        return SendClientMessage(playerid, COLOR_ERROR, "> /setvhp <playerid/nome> <health>");
    
    if(id < 0 || id >= MAX_VEHICLES)
        return SendClientMessage(playerid, COLOR_ERROR, "> Il veicolo non esiste!");

    SetVehicleHealth(id, hp);
    SendMessageToAdmins(0, COLOR_YELLOW, "[ADMIN-ALERT] %s (%s) ha settato gli HP a %f del veicolo %d.", AccountInfo[playerid][aName], Character_GetOOCName(playerid), hp, id);
    Log(AccountInfo[playerid][aName], "", "/setvhp", id);
    foreach(new i : Player)
    {
        if(VehicleInfo[id][vOwnerID] == PlayerInfo[i][pID])
        {
            Character_Save(i);
            return 1;
        }
    }
    return 1;
}
alias:setvhp("setvhealth");

flags:gotov(CMD_JR_MODERATOR);
CMD:gotov(playerid, params[])
{
    new id;
    if(sscanf(params, "d", id))
        return SendClientMessage(playerid, COLOR_ERROR, "> /gotov <vehicleid>");
    
    if(id < 0 || id >= MAX_VEHICLES)
        return SendClientMessage(playerid, COLOR_ERROR, "> Il veicolo non esiste!");

    new Float:x, Float:y, Float:z;
    GetVehiclePos(id, x, y, z);
    SetPlayerPos(playerid, x, y, z);
    return 1;
}

flags:getvhere(CMD_JR_MODERATOR);
CMD:getvhere(playerid, params[])
{
    new id;
    if(sscanf(params, "d", id))
        return SendClientMessage(playerid, COLOR_ERROR, "> /getvhere <vehicleid>");
    
    if(id < 0 || id >= MAX_VEHICLES)
        return SendClientMessage(playerid, COLOR_ERROR, "> Il veicolo non esiste!");

    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    SetVehiclePos(id, x, y, z);
    return 1;
}

flags:apark(CMD_JR_MODERATOR);
CMD:apark(playerid, params[])
{
    new vehicleid = GetPlayerVehicleID(playerid);
    if(vehicleid == 0)
        return SendClientMessage(playerid, COLOR_ERROR, "> Non sei su un veicolo!");
    if(!VehicleInfo[vehicleid][vModel])
        return SendClientMessage(playerid, COLOR_ERROR, "> Non puoi utilizzare questo comando su questo veicolo!");
    new Float:x, Float:y, Float:z, Float:a;
    GetVehiclePos(vehicleid, x, y, z);
    GetVehicleZAngle(vehicleid, a);
    Vehicle_Park(vehicleid, x, y, z, a);
    new pid = -1;
    foreach(new i : Player)
    {
        if(!gAccountLogged[i] || !gCharacterLogged[i] || PlayerInfo[i][pID] != VehicleInfo[vehicleid][vOwnerID])
            continue;
        pid = i;
        break;
    }
    new fixed[24];
    FixName(PlayerInfo[pid][pName], fixed);
    SendFormattedMessage(playerid, COLOR_GREEN, "> [ADMIN] Hai parcheggiato qui il veicolo di %s [%d].", fixed, pid);
    SendFormattedMessage(pid, COLOR_ERROR, "> [ADMIN-ALERT]: %s [%d] ha parcheggiato il tuo veicolo altrove. Motivo: %s", AccountInfo[playerid][aName], playerid, params);
    Log(AccountInfo[playerid][aName], "", "/apark", vehicleid);
    return 1;
}

//flags:payday(CMD_RCON);
flags:payday(CMD_JR_MODERATOR);
CMD:payday(playerid, params[])
{
    foreach(new i : Player)
    {
        Character_PayDay(i);
    }
    return 1;
}

flags:goto(CMD_JR_MODERATOR);
CMD:goto(playerid, params[])
{
    new id;
    if(sscanf(params, "u", id))
        return SendClientMessage(playerid, COLOR_ERROR, "> /goto <playerid/partofname>");
    
    if(id < 0 || id >= MAX_PLAYERS || !gAccountLogged[id] || !gCharacterLogged[id])
        return SendClientMessage(playerid, COLOR_ERROR, "> Il giocatore non è collegato!");

    new 
        Float:x, Float:y, Float:z,
        vw = GetPlayerVirtualWorld(id), int = GetPlayerInterior(id);
    
    GetPlayerPos(id, x, y, z);
    
    Streamer_UpdateEx(playerid, x, y, z, vw, int);
    SetPlayerPos(playerid, x, y, z);
    SetPlayerVirtualWorld(playerid, vw);
    SetPlayerInterior(playerid, int);
    return 1;
}

flags:gethere(CMD_JR_MODERATOR);
CMD:gethere(playerid, params[])
{
    new id;
    if(sscanf(params, "u", id))
        return SendClientMessage(playerid, COLOR_ERROR, "> /gethere <playerid/partofname>");
    
    if(id < 0 || id >= MAX_PLAYERS || !gAccountLogged[id] || !gCharacterLogged[id])
        return SendClientMessage(playerid, COLOR_ERROR, "> Il giocatore non è collegato!");
    new 
        Float:x, Float:y, Float:z,
        vw = GetPlayerVirtualWorld(playerid), int = GetPlayerInterior(playerid);
    
    GetPlayerPos(playerid, x, y, z);
    
    Streamer_UpdateEx(id, x, y, z, vw, int);
    SetPlayerPos(id, x, y, z);
    SetPlayerVirtualWorld(id, vw);
    SetPlayerInterior(id, int);
    return 1;
}

flags:gotopos(CMD_JR_MODERATOR);
CMD:gotopos(playerid, params[])
{
    new Float:x, Float:y, Float:z, int, vw;
    if(sscanf(params, "fffdd", x, y, z, int, vw))
        return SendClientMessage(playerid, COLOR_ERROR, "> /gotopos <x> <y> <z> <int> <vw>");
    SetPlayerInterior(playerid, int);
    SetPlayerVirtualWorld(playerid, vw);
    Streamer_UpdateEx(playerid, x, y, z, vw, int);
    SetPlayerPos(playerid, x, y, z);
    return 1;
}

flags:check(CMD_JR_MODERATOR);
CMD:check(playerid, params[])
{
    new id;
    if(sscanf(params, "u", id))
        return SendClientMessage(playerid, COLOR_ERROR, "> /check <playerid/partofname>");
    if(id < 0 || id >= MAX_PLAYERS || !gAccountLogged[id] || !gCharacterLogged[id])
        return SendClientMessage(playerid, COLOR_ERROR, "> Il giocatore non è collegato!");
    Character_ShowStats(id, playerid);
    return 1;
}

flags:pm(CMD_JR_MODERATOR);
CMD:pm(playerid, params[])
{
    new id, s[128];
    if(sscanf(params, "us[128]", id, s) || isnull(s) || strlen(s) > 128)
        return SendClientMessage(playerid, COLOR_ERROR, "> /pm <playerid/partofname> <testo>");
    if(id < 0 || id >= MAX_PLAYERS || !gAccountLogged[id] || !gCharacterLogged[id])
        return SendClientMessage(playerid, COLOR_ERROR, "> Il giocatore non è collegato!");

    SendFormattedMessage(id, COLOR_YELLOW, "PM da %s (%d): %s", Character_GetOOCName(playerid), playerid, s);
    SendFormattedMessage(playerid, COLOR_YELLOW, "PM a %s (%d) inviato.", Character_GetOOCName(id), id);
    SendFormattedMessage(playerid, COLOR_YELLOW, "Testo: %s", s);
    return 1;
}

flags:giveitem(CMD_JR_MODERATOR);
CMD:giveitem(playerid, params[])
{
    new id, itemid, quantity;
    if(sscanf(params, "uk<item>d", id, itemid, quantity))
        return SendClientMessage(playerid, COLOR_ERROR, "/giveitem <playerid/partofname> <itemid/item_name> <quantità>");
    
    if(!IsPlayerConnected(id) || !gCharacterLogged[id])
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è connesso!");
    
    if(id == INVALID_ITEM_ID || !ServerItem_IsValid(itemid))
        return SendClientMessage(playerid, COLOR_ERROR, "L'item inserito non è corretto!");
    
    if(quantity > 0)
    {
        new result = Character_GiveItem(id, itemid, quantity);
        if(result == INVENTORY_ADD_SUCCESS)
        {
            SendMessageToAdmins(0, COLOR_YELLOW, "[ADMIN-ALERT] %s (%s) ha givato %s (Qnt: %d) a %s (%d).", 
            AccountInfo[playerid][aName], Character_GetOOCName(playerid), ServerItem[itemid][sitemName], quantity, Character_GetOOCName(id), id);
        }
        else if(result == INVENTORY_NO_SPACE)
        {
            SendFormattedMessage(playerid, COLOR_ERROR, "%s (%d) non ha abbastanza spazio nell'inventario!", Character_GetOOCName(id), id);
        }
    }
    else if(quantity < 0)
    {
        quantity = -quantity;
        Character_DecreaseItemAmount(id, itemid, quantity);
        SendMessageToAdmins(0, COLOR_YELLOW, "[ADMIN-ALERT] %s (%s) ha rimosso %s (Qnt: %d) a %s (%d).", 
        AccountInfo[playerid][aName], Character_GetOOCName(playerid), ServerItem[itemid][sitemName], quantity, Character_GetOOCName(id), id);
    }
    return 1;
}

flags:giveweapon(CMD_ADMIN);
CMD:giveweapon(playerid, params[])
{
    new id, wid, ammo;
    if(sscanf(params, "uk<weapon>i", id, wid, ammo))
        return SendClientMessage(playerid, COLOR_ERROR, "/giveweapon <playerid/partofname> <weaponid/weapon name> <ammo>");
    GivePlayerWeapon(playerid, wid, ammo);
    
    return 1;
}

flags:acmds(CMD_JR_MODERATOR);
CMD:acmds(playerid, params[])
{
    SendClientMessage(playerid, -1, "/a - /aduty - /setskin - /setadmin - /givemoney");
    SendClientMessage(playerid, -1, "/sethp - /setarmour - /veh - /jetpack - /setvhp");
    SendClientMessage(playerid, -1, "/gotov - /getvhere - /apark - /payday - /goto");
    SendClientMessage(playerid, -1, "/gethere - /gotopos - /check - /pm - /giveitem");
    SendClientMessage(playerid, -1, "/acmds");
    return 1;
}

SSCANF:item(string[])
{
    // probably an ID
    if('0' <= string[0] <= '9')
    {
        new ret = strval(string);
        if(0 <= ret <= MAX_ITEMS_IN_SERVER)
        {
            return ret;
        }
    }
    else
    {
        foreach(new item : ServerItems)
        {
            if(!strcmp(ServerItem[item][sitemName], string, true) || strfind(ServerItem[item][sitemName], string, true) > -1)
                return item;
        }
    }
    return INVALID_ITEM_ID;
}