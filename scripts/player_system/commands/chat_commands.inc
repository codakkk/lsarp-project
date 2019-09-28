
flags:clearchat(CMD_USER);
CMD:clearchat(playerid, params[])
{
	for(new i = 0; i < 50; ++i)
		SendClientMessage(playerid, -1, "");
	SendClientMessage(playerid, -1, "La chat è stata pulita.");
	return 1;
}

flags:low(CMD_USER);
CMD:low(playerid, params[])
{
	if(Character_IsDead(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
    if(isnull(params) || strlen(params) > 128) 
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /low <testo>");
    new String:string = str_format("%s dice a bassa voce: %s", Character_GetRolePlayName(playerid), params);
    ProxDetectorStr(playerid, 5.0, string, 0x9d9d9dFF, 0x9d9d9dFF, 0x9d9d9dFF, 0x9d9d9dFF, 0x9d9d9dFF);
    return 1;
}

flags:pm(CMD_USER);
CMD:pm(playerid, params[])
{
	/*if(Account_GetAdminLevel(playerid) < 2 && Account_GetPremiumLevel(playerid) < 3)
	{
		new seconds = 5;
		if(GetTickCount() - pLastPMTime[playerid] < 1000 * seconds)
			return SendFormattedMessage(playerid, COLOR_ERROR, "Puoi inviare un PM ogni %d secondi.", seconds);
	}*/
    new id, s[128];
    if(sscanf(params, "k<u>s[128]", id, s) || isnull(s) || strlen(s) > 128)
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /pm [playerid/partofname] <testo>");
    if(id < 0 || id >= MAX_PLAYERS  || !Character_IsLogged(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");
	if(playerid == id)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi inviare un PM a te stesso.");
    if(Iter_Contains(pTogglePM[playerid], id))
        return SendClientMessage(playerid, COLOR_ERROR, "Hai disabilitato i PM verso e da questo giocatore.");

    if(!Account_HasPMAllEnabled(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Hai disabilitato i PM verso e da tutti i giocatori."); 

	if(Account_GetAdminLevel(playerid) < 1)
	{
		if(Iter_Contains(pTogglePM[id], playerid) || !Account_HasPMAllEnabled(id))
			return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore ha disabilitato i PM.");
	}
    
    PlayerPlaySound(id, 1085, 0.0, 0.0, 0.0);
    if(Player_IsAdminDuty(playerid))
    {
        SendTwoLinesMessage(id, COLOR_RECEIVEPM, "** PM da {FF6347}%s{eccd2d} (%d): %s", Account_GetName(playerid), playerid, s);
        if(Player_IsAdminDuty(id))
        {
            SendTwoLinesMessage(playerid, COLOR_SENDPM, "PM a {FF6347}%s{eee854} (%d): %s", Account_GetName(id), id, s);
        }
        else
        {
            SendTwoLinesMessage(playerid, COLOR_SENDPM, "PM a {FF6347}%s{eee854} (%d): %s", Character_GetOOCName(id), id, s);
        }
    }
    else
    {
        if(Account_IsSupporterDuty(playerid))
        {
            SendTwoLinesMessage(id, COLOR_RECEIVEPM, "PM da {e9de98}%s{eccd2d} (%d): %s", Character_GetOOCName(playerid), playerid, s);
            SendTwoLinesMessage(playerid, COLOR_SENDPM, "PM a {e9de98}%s{eee854} (%d): %s", Character_GetOOCName(id), id, s);
        }
        else
        {
            SendTwoLinesMessage(id, COLOR_RECEIVEPM, "PM da %s (%d): %s", Character_GetOOCName(playerid), playerid, s);
            SendTwoLinesMessage(playerid, COLOR_SENDPM, "PM a %s (%d): %s", Character_GetOOCName(id), id, s);
        }
    }
	
	if(Character_IsAFK(id))
	{
		Player_Info(playerid, "Il giocatore a cui hai inviato il ~y~PM~w~ risulta ~r~AFK~w~.");
	}
    return 1;
}

flags:shout(CMD_ALIVE_USER);
CMD:shout(playerid, params[])
{
    if(isnull(params) || strlen(params) > 128) 
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /s(hout) <testo>");
    new String:string = str_format("%s grida: %s", Character_GetRolePlayName(playerid), params);
    ProxDetectorStr(playerid, 40.0, string, COLOR_FADE1, COLOR_FADE2, COLOR_FADE3, COLOR_FADE4, COLOR_FADE5);
    return 1;
}
alias:shout("s");

flags:do(CMD_USER);
CMD:do(playerid, params[])
{
	//if(Character_IsDead(playerid))
		//return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
    if(isnull(params) || strlen(params) > 128) 
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /do <testo>");
    Character_Do(playerid, params);
    return 1;
}

flags:dolow(CMD_USER);
CMD:dolow(playerid, params[])
{
	//if(Character_IsDead(playerid))
		//return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
    if(isnull(params) || strlen(params) > 128) 
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /dolow <testo>");
    Character_DoLow(playerid, params);
    return 1;
}

flags:me(CMD_USER);
CMD:me(playerid, params[])
{
	if(Character_IsDead(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
    if(isnull(params) || strlen(params) > 128) 
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /me <testo>");
	if(Character_IsInjured(playerid))
		return pc_cmd_melow(playerid, params);
    Character_Me(playerid, params);
    return 1;
}

flags:melow(CMD_USER);
CMD:melow(playerid, params[])
{
	if(Character_IsDead(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
    if(isnull(params) || strlen(params) > 128) 
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /melow <testo>");
    Character_MeLow(playerid, params);
    return 1;
}

flags:ame(CMD_USER);
CMD:ame(playerid, params[])
{
	if(Character_IsDead(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
    if(isnull(params) || strlen(params) > 128) 
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /ame <descrizione>");
    Character_AMe(playerid, params);
    return 1;
}

flags:b(CMD_USER);
CMD:b(playerid, params[])
{
	if(!Account_HasOOCEnabled(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Hai disabilitato la chat OOC. Usa \"/blockb all\" per riattivarla.");
    if(isnull(params) || strlen(params) > 128) 
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /b <testo>");
    new String:string;
    if(Player_IsAdminDuty(playerid))
    {
        string = str_format("{FFFFFF}(( {FF6347}%s{FFFFFF} [%d]: %s ))", Account_GetName(playerid), playerid, params);
        //ProxDetectorStr(playerid, 15.0, string, 0xC7F1FFFF, 0xC7F1FFFF, 0xC7F1FFFF, 0xC7F1FFFF, 0xC7F1FFFF, true);
		ProxDetectorStr(playerid, 15.0, string, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, true);
    }
    else
    {
        string = str_format("(( %s [%d]: %s ))", Character_GetOOCName(playerid), playerid, params);
        ProxDetectorStr(playerid, 15.0, string, 0xB8BAC6FF, 0xB8BAC6FF, 0xB8BAC6FF, 0xB8BAC6FF, 0xB8BAC6FF, true);
    }
    return 1;
}

flags:whisper(CMD_USER);
CMD:whisper(playerid, params[])
{
	if(Character_IsDead(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
    new id, text[128];
    if(sscanf(params, "k<u>s[128]", id, text) || strlen(text) > 128)
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /w(hisper) [playerid/partofname] <testo>");
    if(!IsPlayerConnected(id) || !Character_IsLogged(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");
    if(playerid == id)
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su te stesso.");
    if(!ProxDetectorS(3.0, playerid, id))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
    
    SendTwoLinesMessage(playerid, COLOR_YELLOW, "%s sussurra: %s", Character_GetRolePlayName(playerid), text);
    SendTwoLinesMessage(id, COLOR_YELLOW, "%s sussurra: %s", Character_GetRolePlayName(playerid), text);

	Character_CharacterAMe(playerid, id, "sussurra qualcosa a");
    // Character_AMe(playerid, "sussurra qualcosa a %s", Character_GetRolePlayName(id));
    return 1;
}
alias:whisper("w", "sussurra");

flags:cwhisper(CMD_USER);
CMD:cwhisper(playerid, params[])
{
	if(Character_IsDead(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
    new text[128];
    if(sscanf(params, "s[128]", text) || strlen(text) > 128)
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /cw(hisper) <testo>");
    new vehicleid = GetPlayerVehicleID(playerid);
    if(vehicleid <= 0 || vehicleid == INVALID_VEHICLE_ID || IsABike(vehicleid) || IsAMotorBike(vehicleid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei in un veicolo.");
    
    foreach(new p : Player)
    {
        if(!Character_IsLogged(p) || !IsPlayerInVehicle(p, vehicleid))
            continue;
        SendTwoLinesMessage(p, COLOR_YELLOW, "[Veicolo] %s dice: %s", Character_GetRolePlayName(playerid), params);
    }
    return 1;
}
alias:cwhisper("cw");