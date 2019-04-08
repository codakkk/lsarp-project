flags:low(CMD_USER);
CMD:low(playerid, params[])
{
    if(isnull(params) || strlen(params) > 128) 
        return SendClientMessage(playerid, COLOR_ERROR, "> /low <testo>");
    new String:string = str_format("%s dice a bassa voce: %s", Character_GetOOCName(playerid), params);
    ProxDetectorStr(playerid, 5.0, string, COLOR_FADE1, COLOR_FADE2, COLOR_FADE3, COLOR_FADE4, COLOR_FADE5);
    return 1;
}

flags:pm(CMD_USER);
CMD:pm(playerid, params[])
{
    new id, s[128];
    if(sscanf(params, "us[128]", id, s) || isnull(s) || strlen(s) > 128)
        return SendClientMessage(playerid, COLOR_ERROR, "> /pm <playerid/partofname> <testo>");
    if(id < 0 || id >= MAX_PLAYERS || !gAccountLogged[id] || !gCharacterLogged[id])
        return SendClientMessage(playerid, COLOR_ERROR, "> Il giocatore non è collegato!");

    if(Iter_Contains(pTogglePM[playerid], id))
        return SendClientMessage(playerid, COLOR_ERROR, "Hai disabilitato i PM verso e da questo giocatore.");

    if(Bit_Get(gPlayerBitArray[e_pTogglePMAll], playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Hai disabilitato i PM verso e da tutti i giocatori!"); 

    if( ((Iter_Contains(pTogglePM[id], playerid) || Bit_Get(gPlayerBitArray[e_pTogglePMAll], id)) && AccountInfo[playerid][aAdmin] < 1))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore ha disabilitato i PM.");
    
    PlayerPlaySound(id, 1085, 0.0, 0.0, 0.0);
    SendFormattedMessage(id, COLOR_YELLOW, "PM da %s (%d): %s", Character_GetOOCName(playerid), playerid, s);
    SendFormattedMessage(playerid, COLOR_YELLOW, "PM a %s (%d): %s.", Character_GetOOCName(id), id, s);
    return 1;
}

/*flags:local(CMD_USER);
CMD:local(playerid, params[])
{
    if(isnull(params) || strlen(params) > 256) 
        return SendClientMessage(playerid, COLOR_ERROR, "> /l(ocal) <testo>");
    new string[256];
    format(string, sizeof(string), "[A bassa voce] %s (%d): %s", Character_GetOOCName(playerid), playerid, params);
    ProxDetector(playerid, 15.0, string, COLOR_FADE1, COLOR_FADE2, COLOR_FADE3, COLOR_FADE4, COLOR_FADE5);
    return 1;
}
alias:local("l");*/

flags:shout(CMD_USER);
CMD:shout(playerid, params[])
{
    if(isnull(params) || strlen(params) > 128) 
        return SendClientMessage(playerid, COLOR_ERROR, "> /s(hout) <testo>");
    new String:string = str_format("%s grida: %s!", Character_GetOOCName(playerid), params);
    ProxDetectorStr(playerid, 40.0, string, COLOR_FADE1, COLOR_FADE2, COLOR_FADE3, COLOR_FADE4, COLOR_FADE5);
    return 1;
}
alias:shout("s");

flags:do(CMD_USER);
CMD:do(playerid, params[])
{
    if(isnull(params) || strlen(params) > 128) 
        return SendClientMessage(playerid, COLOR_ERROR, "> /do <testo>");
    Character_Do(playerid, params);
    return 1;
}

flags:dolow(CMD_USER);
CMD:dolow(playerid, params[])
{
    if(isnull(params) || strlen(params) > 128) 
        return SendClientMessage(playerid, COLOR_ERROR, "> /dolow <testo>");
    Character_DoLow(playerid, params);
    return 1;
}

flags:me(CMD_USER);
CMD:me(playerid, params[])
{
    if(isnull(params) || strlen(params) > 128) 
        return SendClientMessage(playerid, COLOR_ERROR, "> /me <testo>");
    Character_Me(playerid, params);
    return 1;
}

flags:melow(CMD_USER);
CMD:melow(playerid, params[])
{
    if(isnull(params) || strlen(params) > 128) 
        return SendClientMessage(playerid, COLOR_ERROR, "> /melow <testo>");
    Character_MeLow(playerid, params);
    return 1;
}

flags:ame(CMD_USER);
CMD:ame(playerid, params[])
{
    if(isnull(params) || strlen(params) > 128) 
        return SendClientMessage(playerid, COLOR_ERROR, "> /ame <descrizione>");
    Character_AMe(playerid, params);
    return 1;
}

flags:b(CMD_USER);
CMD:b(playerid, params[])
{
    if(isnull(params) || strlen(params) > 128) 
        return SendClientMessage(playerid, COLOR_ERROR, "> /b <testo>");
    new String:string;
    if(pAdminDuty[playerid])
    {
        string = str_format("(( Admin %s [%d]: %s ))", AccountInfo[playerid][aName], playerid, params);
        ProxDetectorStr(playerid, 15.0, string, 0xC7F1FFFF, 0xC7F1FFFF, 0xC7F1FFFF, 0xC7F1FFFF, 0xC7F1FFFF);
    }
    else
    {
        string = str_format("(( %s [%d]: %s ))", Character_GetOOCName(playerid), playerid, params);
        ProxDetectorStr(playerid, 15.0, string, COLOR_FADE1, COLOR_FADE2, COLOR_FADE3, COLOR_FADE4, COLOR_FADE5);
    }
    return 1;
}

flags:whisper(CMD_USER);
CMD:whisper(playerid, params[])
{
    new id, text[128];
    if(sscanf(params, "us[128]", id, text) || strlen(text) > 128)
        return SendClientMessage(playerid, COLOR_ERROR, "> /w(hisper) <playerid/partofname> <testo>");
    if(!IsPlayerConnected(id) || !gCharacterLogged[id])
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato!");
    if(playerid == id)
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su te stesso!");
    if(!ProxDetectorS(3.0, playerid, id))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore!");
    
    SendFormattedMessage(playerid, COLOR_YELLOW, "%s sussurra: %s", Character_GetOOCName(playerid), params);
    SendFormattedMessage(id, COLOR_YELLOW, "%s sussurra: %s", Character_GetOOCName(playerid), params);

    Character_AMe(playerid, "sussurra qualcosa a %s", Character_GetOOCName(id));
    return 1;
}
alias:whisper("w");

flags:cwhisper(CMD_USER);
CMD:cwhisper(playerid, params[])
{
    new text[128];
    if(sscanf(params, "s[128]", text) || strlen(text) > 128)
        return SendClientMessage(playerid, COLOR_ERROR, "> /cw(hisper) <testo>");
    new vehicleid = GetPlayerVehicleID(playerid);
    if(vehicleid <= 0 || vehicleid == INVALID_VEHICLE_ID || IsABike(vehicleid) || IsAMotorBike(vehicleid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei in un veicolo!");
    
    foreach(new p : Player)
    {
        if(!gCharacterLogged[p] || !IsPlayerInVehicle(p, vehicleid))
            continue;
        SendTwoLinesMessage(p, COLOR_YELLOW, "[Veicolo] %s dice a bassa voce: %s", Character_GetOOCName(playerid), params);
    }
    return 1;
}
alias:cwhisper("cw");