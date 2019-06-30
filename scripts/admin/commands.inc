flags:acmds(CMD_JR_MODERATOR);
CMD:acmds(playerid, params[])
{
	if(AccountInfo[playerid][aAdmin] >= 2)
	{
		SendClientMessage(playerid, -1, "[MODERATORE JR]: /a - /aduty - /goto - /gethere - /jetpack");
		SendClientMessage(playerid, -1, "[MODERATORE JR]: /gotopos - /check - /invcheck - /freeze - /unfreeze");
		SendClientMessage(playerid, -1, "[MODERATORE JR]: /masks - /n - /setweather - /settime - /resetinv");
		SendClientMessage(playerid, -1, "[MODERATORE JR]: /resetdrugstats - /drugstats - /setvw");
		
	}
	if(AccountInfo[playerid][aAdmin] >= 3)
	{
		SendClientMessage(playerid, -1, "[MODERATORE]: /setskin - /sethp - /setvhp - /setarmour - /ajail - /unjail");
		SendClientMessage(playerid, -1, "[MODERATORE]: /vcreate - /vdelete - (/fix)veh - /gotov - /getvhere - /avcolor");
		SendClientMessage(playerid, -1, "[MODERATORE]: /ban - /unban");
	}
	if(AccountInfo[playerid][aAdmin] >= 4)
	{
		SendClientMessage(playerid, -1, "[ADMIN]: /amm - /giveitem - /givevehicleitem - /apark - /vowner - /vsave - /vdespawn");
		SendClientMessage(playerid, -1, "[ADMIN]: /gotobuilding - /banaccount - /unbanaccount - /banofflineaccount - /banip");
	}
	if(AccountInfo[playerid][aAdmin] >= 5)
	{
		//SendClientMessage(playerid, -1, "[DEVELOPER]: /giveitem - /apark");
	}
	if(IsPlayerAdmin(playerid))
	{
		SendClientMessage(playerid, -1, "[RCON]: /setadmin - /givemoney - /payday");
		SendClientMessage(playerid, -1, "[RCON]: /abuildingcmds - /adealershipcmds - /ahousecmds");
	}
    return 1;
}

flags:scmds(CMD_SUPPORTER);
CMD:scmds(playerid, params[])
{
	SendClientMessage(playerid, -1, "[SUPPORTER]: /supp - ");
	return 1;
}

flags:supp(CMD_SUPPORTER);
CMD:supp(playerid, params[])
{
	if(pDisableAdminAlerts[playerid])
		return SendClientMessage(playerid, COLOR_ERROR, "Hai disabilitato la chat supporter.");
	if(isnull(params) || strlen(params) > 128)
		return SendClientMessage(playerid, COLOR_ERROR, "/supp <testo>");
	SendMessageToAdmins(0, COLOR_YELLOW, "[SUPPORTER-CHAT] %s (%s): %s", AccountInfo[playerid][aName], PlayerInfo[playerid][pName], params);
	return 1;
}

flags:a(CMD_JR_MODERATOR);
CMD:a(playerid, params[])
{
	if(pDisableAdminAlerts[playerid])
		return SendClientMessage(playerid, COLOR_ERROR, "Hai disabilitato la chat mod.");
	if(isnull(params) || strlen(params) > 128)
		return SendClientMessage(playerid, COLOR_ERROR, "/a <testo>");
	SendMessageToAdmins(0, 0xE1A400FF, "[MOD-CHAT] %s (%s): %s", AccountInfo[playerid][aName], PlayerInfo[playerid][pName], params);
	return 1;
}

flags:amm(CMD_ADMIN);
CMD:amm(playerid, params[])
{
	if(pDisableAdminAlerts[playerid])
		return SendClientMessage(playerid, COLOR_ERROR, "Hai disabilitato la chat admin.");
	if(isnull(params) || strlen(params) > 128)
		return SendClientMessage(playerid, COLOR_ERROR, "/amm <testo>");
	SendMessageToAdmins(0, 0x876200FF, "[ADMIN-CHAT] %s (%s): %s", AccountInfo[playerid][aName], PlayerInfo[playerid][pName], params);
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
	if(pSupporterDuty[playerid])
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se sei in servizio supporter.");
	if(pAdminDuty[playerid])
	{
		pAdminDuty[playerid] = 0;
		SetPlayerName(playerid, PlayerInfo[playerid][pName]);
		AC_SetPlayerHealth(playerid, 100);
		AC_SetPlayerArmour(playerid, 0);
		SendMessageToAdmins(0, COLOR_ADMINCHAT, "[ADMIN] %s (%d) non è più in servizio.", AccountInfo[playerid][aName], playerid);
		SetPlayerColor(playerid, 0xFFFFFFFF);
		/*if(IsValidDynamic3DTextLabel(pAdminDuty3DText[playerid]))
		{
			DestroyDynamic3DTextLabelEx(pAdminDuty3DText[playerid]);
		}*/
	}    
	else
	{
		if(Character_IsMasked(playerid))
			pc_cmd_sconosciuto(playerid, "");
		
		pAdminDuty[playerid] = 1;
		SetPlayerName(playerid, AccountInfo[playerid][aName]);
		AC_SetPlayerHealth(playerid, 99999.0);
		AC_SetPlayerArmour(playerid, 99999.0);
		//pc_cmd_jetpack(playerid, NULL);
		SendMessageToAdmins(0, COLOR_ADMINCHAT, "[ADMIN] %s (%d) è in servizio.", AccountInfo[playerid][aName], playerid);
		SetPlayerColor(playerid, 0xFF6347FF);
		/*if(IsValidDynamic3DTextLabel(pAdminDuty3DText[playerid]))
		{
			DestroyDynamic3DTextLabelEx(pAdminDuty3DText[playerid]);
			pAdminDuty3DText[playerid] = Text3D:INVALID_3DTEXT_ID;
		}
		pAdminDuty3DText[playerid] = CreateDynamic3DTextLabel("Admin", COLOR_ADMINDUTY, 0, 0, 0.3, 30.0, playerid);*/
	}
	foreach(new i : Player)
	{
		if(!Character_IsLogged(i))
			continue;
		if(Character_IsMasked(i))
		{
			if(pAdminDuty[playerid])
			{
				SetPlayerMarkerForPlayer(playerid, i, 0xFFFF00AA);
				ShowPlayerNameTagForPlayer(playerid, i, true);
			}
			else
				ShowPlayerNameTagForPlayer(playerid, i, false);
		}
		else ShowPlayerNameTagForPlayer(playerid, i, true);
	}
    return 1;
}

flags:setvw(CMD_JR_MODERATOR);
CMD:setvw(playerid, params[])
{
    new id, vw;
    if(sscanf(params, "k<u>d", id, vw))
	   return SendClientMessage(playerid, COLOR_ERROR, "/setvw <playerid/nome> <virtual world>");
	
	Character_SetVirtualWorld(id, vw);

	SendFormattedMessage(id, COLOR_GREEN, "L'admin %s ti ha settato il virtual world a %d.", AccountInfo[playerid][aName], vw);
	SendFormattedMessage(playerid, COLOR_GREEN, "Hai settato il virtual world di %s al valore %d.", Character_GetOOCName(id), vw);
	return 1;
}

flags:setskin(CMD_MODERATOR);
CMD:setskin(playerid, params[])
{
    new id, skid;
    if(sscanf(params, "k<u>d", id, skid))
	   return SendClientMessage(playerid, COLOR_ERROR, "/setskin <playerid/nome> <skin-id>");
    
    if(id < 0 || id >= MAX_PLAYERS || !Character_IsLogged(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "L'utente non è collegato.");
    
	if(skid < 1 || (skid > 311 && skid < 20000) || skid > 30000)
		return SendClientMessage(playerid, COLOR_ERROR, "Inserisci un ID compreso tra 1 e 311. (20000 - 30000 per le custom skin)");
    
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
	if(sscanf(params, "k<u>d", id, lvl))
		return SendClientMessage(playerid, COLOR_ERROR, "/setadmin <playerid/nome> <level (0 - 5)>");

	if(id < 0 || id >= MAX_PLAYERS || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "L'utente non è collegato.");
	if(lvl < 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Livello Admin non valido.");
	if(lvl == 0)
	{
		if(pAdminDuty[id])
			pc_cmd_aduty(id, "");
		SendFormattedMessage(id, COLOR_ERROR, "[ADMIN] %s (%d) ti ha rimosso dallo staff.", AccountInfo[playerid][aName], playerid);
		SendFormattedMessage(playerid, COLOR_ERROR, "Hai rimosso %s (%d) dallo staff.", AccountInfo[id][aName], id);
		SendMessageToAdmins(true, COLOR_ADMIN, "[ADMIN-ALERT]: %s (%d) ha rimosso %s (%d) dallo staff.", AccountInfo[playerid][aName], playerid, AccountInfo[id][aName], id);
	}
	else
	{
		//new ln[] = ;
		SendFormattedMessage(id, COLOR_GREEN, "[ADMIN] %s (%d) ti ha settato %s.", AccountInfo[playerid][aName], playerid, GetAdminLevelName(lvl));
		SendFormattedMessage(playerid, COLOR_GREEN, "%s (%d) è stato settato %s.", AccountInfo[id][aName], id, GetAdminLevelName(lvl));
		SendMessageToAdmins(true, COLOR_ADMIN, "[ADMIN-ALERT]: %s (%d) ha settato %s (%d) %s.", AccountInfo[playerid][aName], playerid, AccountInfo[id][aName], id, GetAdminLevelName(lvl));
	}
	Log(AccountInfo[playerid][aName], AccountInfo[id][aName], "/setadmin", lvl);
	AccountInfo[id][aAdmin] = lvl;
	Account_Save(id);
	return 1;
}

flags:givemoney(CMD_RCON);
CMD:givemoney(playerid, params[])
{
    new id, hp;
    if(sscanf(params, "k<u>d", id, hp))
	   return SendClientMessage(playerid, COLOR_ERROR, "/givemoney <playerid/nome> <ammontare>");
    
    if(id < 0 || id >= MAX_PLAYERS || !Character_IsLogged(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "L'utente non è collegato.");
    
    Character_GiveMoney(id, hp);
    // Send admin alert (?)
    SendMessageToAdmins(1, COLOR_YELLOW, "[ADMIN-ALERT] %s (%s) ha dato $%d a %s (%d).", AccountInfo[playerid][aName], Character_GetOOCName(playerid), hp, Character_GetOOCName(id), id);
    SendFormattedMessage(id, COLOR_GREEN, "[ADMIN] %s (%s) ti ha givato $%d.", AccountInfo[playerid][aName], Character_GetOOCName(playerid), hp);
    Log(AccountInfo[playerid][aName], PlayerInfo[id][pName], "/givemoney", hp);
    Character_Save(id);
    return 1;
}

flags:sethp(CMD_MODERATOR);
CMD:sethp(playerid, params[])
{
    new id, hp;
    if(sscanf(params, "k<u>d", id, hp))
	   return SendClientMessage(playerid, COLOR_ERROR, "/sethp <playerid/nome> <health>");
    
    if(id < 0 || id >= MAX_PLAYERS  || !Character_IsLogged(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "L'utente non è collegato.");
    
    if(hp < 0 || hp > 200.0)
	   return SendClientMessage(playerid, COLOR_ERROR, "Valore HP non valido.");

	if(Character_IsDead(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su un giocatore in sistema morte.");

    AC_SetPlayerHealth(id, hp);
    SendMessageToAdmins(0, COLOR_YELLOW, "[ADMIN-ALERT] %s (%s) ha settato gli HP di %s (%d) a %d.", AccountInfo[playerid][aName], Character_GetOOCName(playerid), Character_GetOOCName(id), id, hp);
    SendFormattedMessage(playerid, COLOR_GREEN, "[ADMIN] %s (%s) ti ha settato gli HP a %d.", AccountInfo[playerid][aName], Character_GetOOCName(playerid), hp);
    Log(AccountInfo[playerid][aName], PlayerInfo[id][pName], "/sethp", hp);
    Character_Save(id);
    return 1;
}
alias:sethp("sethealth");

flags:setarmour(CMD_MODERATOR);
CMD:setarmour(playerid, params[])
{
    new id, hp;
    if(sscanf(params, "k<u>d", id, hp))
	   return SendClientMessage(playerid, COLOR_ERROR, "/setarmour <playerid/nome> <armour>");
    
    if(id < 0 || id >= MAX_PLAYERS  || !Character_IsLogged(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "L'utente non è collegato.");

    if(hp < 0 || hp > 200)
	   return SendClientMessage(playerid, COLOR_ERROR, "Valore Armatura non valido.");

    AC_SetPlayerArmour(id, hp);
    SendMessageToAdmins(0, COLOR_YELLOW, "[ADMIN-ALERT] %s (%s) ha settato l'armatura di %s (%d) a %d.", AccountInfo[playerid][aName], Character_GetOOCName(playerid), Character_GetOOCName(id), id, hp);
    SendFormattedMessage(playerid, COLOR_GREEN, "[ADMIN] %s (%s) ti ha settato l'armatura a %d.", AccountInfo[playerid][aName], Character_GetOOCName(playerid), hp);
    Log(AccountInfo[playerid][aName], PlayerInfo[id][pName], "/setarmour", hp);
    Character_Save(id);
    return 1;
}
alias:setarmour("setap");

flags:vcreate(CMD_MODERATOR);
CMD:vcreate(playerid, params[])
{
	new model, c1, c2;
	if(sscanf(params, "k<vehicle>D(0)D(0)", model, c1, c2))
		return SendClientMessage(playerid, COLOR_ERROR, "/vcreate <modelid> <color1> <color2>");
	if (model < 400 || model > 611) 
		return SendClientMessage(playerid, COLOR_WHITE, "L'id del modello deve andare da 400 a 611.");
	new 
	Float:_x, 
	Float:_y, 
	Float:_z;
	GetPlayerPos(playerid, _x, _y, _z);

	new id = Vehicle_Create(model, _x, _y, _z, 0, c1, c2, 0, 0);
	if(id == -1)
		return SendClientMessage(playerid, COLOR_ERROR, "Non è stato possibile creare il veicolo.");
	
	LinkVehicleToInterior(id, GetPlayerInterior(playerid));
	SetVehicleVirtualWorld(id, Character_GetVirtualWorld(playerid));
	
	Vehicle_SetTemporary(id, true);
	Vehicle_SetFuel(id, 100);
	PutPlayerInVehicle(playerid, id, 0);
	Vehicle_SetEngineOn(id);

	SendFormattedMessage(playerid, COLOR_GREEN, "Veicolo %d creato. Non dimenticare di distruggerlo con /vdelete (a bordo).", id);
	
	Log(AccountInfo[playerid][aName], "", "/vcreate", model);
	return 1;
}

flags:vsave(CMD_ADMIN);
CMD:vsave(playerid, params[])
{
	new vehicleid = GetPlayerVehicleID(playerid);
	if(vehicleid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo.");
	if(!Vehicle_IsTemporary(vehicleid))
		return SendClientMessage(playerid, COLOR_ERROR, "Questo veicolo è già inserito nel database.");
	if(Vehicle_InsertInDatabase(vehicleid))
	{
		SendMessageToAdmins(false, COLOR_ADMIN, "[ADMIN-ALERT]: %s ha aggiunto il veicolo id %d nel database.", AccountInfo[playerid][aName], vehicleid);
	}
	else
	{
		SendClientMessage(playerid, COLOR_ERROR, "Non è stato possibile aggiungere il veicolo nel database.");
	}
	return 1;
}

flags:vfaction(CMD_ADMIN);
CMD:vfaction(playerid, params[])
{
	new vehicleid = GetPlayerVehicleID(playerid);
	if(vehicleid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo.");
	new factionid;
	if(sscanf(params, "d", factionid))
		return SendClientMessage(playerid, COLOR_ERROR, "/vfaction <factionid>");
	if(!Faction_IsValid(factionid))
		return SendClientMessage(playerid, COLOR_ERROR, "Fazione non valida.");
	Vehicle_SetFaction(vehicleid, factionid);
	Vehicle_CancelDespawn(vehicleid);
	Vehicle_Save(vehicleid);
	SendMessageToAdmins(false, COLOR_ADMIN, "[ADMIN-ALERT]: %s ha settato il veicolo id %d nella fazione id %d.", AccountInfo[playerid][aName], vehicleid, factionid);
	Log(AccountInfo[playerid][aName], "", "/vfaction", factionid);
	return 1;
}

flags:vdespawn(CMD_ADMIN);
CMD:vdespawn(playerid, params[])
{
	new vehicleid = GetPlayerVehicleID(playerid);
	if(vehicleid == 0 || !Vehicle_IsValid(vehicleid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo.");
	
	printf("Before Count: %d", Iter_Count(Vehicle));
	Vehicle_Despawn(vehicleid);
	printf("After Count: %d", Iter_Count(Vehicle));

	SendMessageToAdmins(false, COLOR_ADMIN, "[ADMIN-ALERT]: %s ha despawnato il veicolo id %d.", AccountInfo[playerid][aName], vehicleid);
	return 1;
}

flags:jetpack(CMD_SUPPORTER);
CMD:jetpack(playerid, params[])
{
	new id;
	if(sscanf(params, "d", id))
	{
		id = playerid;
	}
	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "L'utente non è collegato.");
	if(GetPlayerSpecialAction(id) == SPECIAL_ACTION_USEJETPACK)
		SetPlayerSpecialAction(id, SPECIAL_ACTION_NONE);
	else
		SetPlayerSpecialAction(id, SPECIAL_ACTION_USEJETPACK);
	gPlayerJetpack[id] = 1;
	return 1;
}

flags:setvhp(CMD_MODERATOR);
CMD:setvhp(playerid, params[])
{
	new id, Float:hp;
	if(sscanf(params, "df", id, hp))
		return SendClientMessage(playerid, COLOR_ERROR, "/setvhp <vehicleid> <health>");

	if(id < 0 || id >= MAX_VEHICLES)
		return SendClientMessage(playerid, COLOR_ERROR, "Il veicolo non esiste.");

	if(hp < 0.0 || hp > 100000.0)
		return SendClientMessage(playerid, COLOR_ERROR, "La salute inserita per il veicolo non è valida.");
	//RepairVehicle(id);
	SetVehicleHealth(id, hp);
	SendMessageToAdmins(0, COLOR_YELLOW, "[ADMIN-ALERT] %s (%s) ha settato gli HP a %f del veicolo %d.", AccountInfo[playerid][aName], Character_GetOOCName(playerid), hp, id);
	Log(AccountInfo[playerid][aName], "", "/setvhp", id);
	foreach(new i : Player)
	{
		if(Vehicle_GetOwnerID(id) == Character_GetID(i))
		{
			Character_Save(i);
			return 1;
		}
	}
	return 1;
}
alias:setvhp("setvhealth");

flags:fixveh(CMD_MODERATOR);
CMD:fixveh(playerid, params[])
{
	new id;
	if(sscanf(params, "d", id))
	{
    	id = GetPlayerVehicleID(playerid);
	}

    if(!IsValidVehicle(id) || id == 0)
	   return SendClientMessage(playerid, COLOR_ERROR, "L'ID inserito non è valido o non sei su un veicolo.");

    SetVehicleHealth(id, 999.0);
	RepairVehicle(id);

    SendMessageToAdmins(0, COLOR_YELLOW, "[ADMIN-ALERT] %s (%d) ha riparato il veicolo id %d.", AccountInfo[playerid][aName], playerid, id);
    Log(AccountInfo[playerid][aName], "", "/fixveh", id);
    return 1;
}
alias:fixveh("fix");

flags:avcolor(CMD_MODERATOR);
CMD:avcolor(playerid, params[])
{
	new id, color1, color2;
	if(sscanf(params, "ddd", id, color1, color2))
		return SendClientMessage(playerid, COLOR_ERROR, "/avcolor <vehicleid> <color1> <color2>");

	if(!IsValidVehicle(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il veicolo inserito non è valido.");

	if(color1 < 0 || color1 > 255)
		return SendClientMessage(playerid, COLOR_ERROR, "Il colore 1 inserito non è valido (0 - 255).");

	if(color2 < 0 || color2 > 255)
		return SendClientMessage(playerid, COLOR_ERROR, "Il colore 2 inserito non è valido (0 - 255).");
	
	ChangeVehicleColor(id, color1, color2);
	
	VehicleInfo[id][vColor1] = color1;
	VehicleInfo[id][vColor2] = color2;

	SendMessageToAdmins(false, COLOR_ADMIN, "[ADMIN-ALERT]: %s ha modificato il colore del veicolo id %d.", AccountInfo[playerid][aName], id);
	return 1;
}

flags:gotovehicle(CMD_MODERATOR);
CMD:gotovehicle(playerid, params[])
{
    new id;
    if(sscanf(params, "d", id))
	   return SendClientMessage(playerid, COLOR_ERROR, "/gotov <vehicleid>");
    
    if(!Vehicle_IsValid(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "Il veicolo non esiste.");

    new Float:x, Float:y, Float:z;
    GetVehiclePos(id, x, y, z);
    SetPlayerPos(playerid, x, y, z);
    SetPlayerInterior(playerid, 0);
    Character_SetVirtualWorld(playerid, GetVehicleVirtualWorld(id)); 
    return 1;
}
alias:gotovehicle("gotov");

flags:getvhere(CMD_MODERATOR);
CMD:getvhere(playerid, params[])
{
    new id;
    if(sscanf(params, "d", id))
	   return SendClientMessage(playerid, COLOR_ERROR, "/getvhere <vehicleid>");
    
    if(id < 0 || id >= MAX_VEHICLES)
	   return SendClientMessage(playerid, COLOR_ERROR, "Il veicolo non esiste.");

    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    SetVehiclePos(id, x, y, z);
	SetVehicleVirtualWorld(id, Character_GetVirtualWorld(playerid));
	LinkVehicleToInterior(id, GetPlayerInterior(playerid));
    return 1;
}

flags:apark(CMD_ADMIN);
CMD:apark(playerid, params[])
{
    new vehicleid = GetPlayerVehicleID(playerid);
    if(vehicleid == 0)
	   return SendClientMessage(playerid, COLOR_ERROR, "Non sei su un veicolo.");
    if(!VehicleInfo[vehicleid][vModel])
	   return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su questo veicolo.");
    new Float:x, Float:y, Float:z, Float:a;
    GetVehiclePos(vehicleid, x, y, z);
    GetVehicleZAngle(vehicleid, a);
    Vehicle_Park(vehicleid, x, y, z, a);
    new pid = -1;
    foreach(new i : Player)
    {
	   if(!Character_IsLogged(i) || PlayerInfo[i][pID] != VehicleInfo[vehicleid][vOwnerID])
		  continue;
	   pid = i;
	   break;
    }
    new fixed[24];
    FixName(PlayerInfo[pid][pName], fixed);
    SendFormattedMessage(playerid, COLOR_GREEN, "[ADMIN] Hai parcheggiato qui il veicolo di %s [%d].", fixed, pid);
    SendFormattedMessage(pid, COLOR_ERROR, "[ADMIN-ALERT]: %s [%d] ha parcheggiato il tuo veicolo altrove. Motivo: %s", AccountInfo[playerid][aName], playerid, params);
    Log(AccountInfo[playerid][aName], "", "/apark", vehicleid);
    return 1;
}

flags:vfuel(CMD_MODERATOR);
CMD:vfuel(playerid, params[])
{
	new fuel = 0, vid;
	if(sscanf(params, "d", fuel))
		return SendClientMessage(playerid, COLOR_ERROR, "/vfuel <fuel>");
	if( (vid = GetPlayerVehicleID(playerid)) == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo.");
	if(fuel < 0 || fuel > 100)
		return SendClientMessage(playerid, COLOR_ERROR, "Valore benzina non valido. (0 - 100).");
	Vehicle_SetFuel(vid, fuel);
	SendMessageToAdmins(false, COLOR_YELLOW, "[ADMIN-ALERT]: %s (%d) ha settato la benzina del veicolo %d. Benzina: %0.2f.", AccountInfo[playerid][aName], playerid, vid, fuel);
	return 1;
}

//flags:payday(CMD_RCON);
flags:payday(CMD_RCON);
CMD:payday(playerid, params[])
{
	foreach(new i : Player)
	{
		if(!Character_IsLogged(i))
			continue;
		Character_PayDay(i);
	}
    return 1;
}

flags:goto(CMD_JR_MODERATOR);
CMD:goto(playerid, params[])
{
    new id;
    if(sscanf(params, "k<u>", id))
	   return SendClientMessage(playerid, COLOR_ERROR, "/goto <playerid/partofname>");
    
    if(id < 0 || id >= MAX_PLAYERS  || !Character_IsLogged(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");

    new 
	   Float:x, Float:y, Float:z,
	   vw = Character_GetVirtualWorld(id), int = GetPlayerInterior(id);
    
    GetPlayerPos(id, x, y, z);
    Streamer_UpdateEx(playerid, x, y, z, vw, int);
    
	Character_SetVirtualWorld(playerid, vw);
    SetPlayerInterior(playerid, int);

    if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
	{
		SetVehiclePos(GetPlayerVehicleID(playerid),x+3,y,z);
		LinkVehicleToInterior(GetPlayerVehicleID(playerid), int);
		SetVehicleVirtualWorld(GetPlayerVehicleID(playerid), vw);
	}
	else
    	SetPlayerPos(playerid, x+2, y, z );
	
    return 1;
}

flags:gethere(CMD_JR_MODERATOR);
CMD:gethere(playerid, params[])
{
    new id;
    if(sscanf(params, "k<u>", id))
	   return SendClientMessage(playerid, COLOR_ERROR, "/gethere <playerid/partofname>");
    
    if(id < 0 || id >= MAX_PLAYERS  || !Character_IsLogged(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");
    new 
	   Float:x, Float:y, Float:z,
	   vw = Character_GetVirtualWorld(playerid), int = GetPlayerInterior(playerid);
    
    GetPlayerPos(playerid, x, y, z);
    
    Streamer_UpdateEx(id, x, y, z, vw, int);
    
	Character_SetVirtualWorld(id, vw);
    SetPlayerInterior(id, int);

    if(GetPlayerState(id) == PLAYER_STATE_DRIVER)
	{
		SetVehiclePos(GetPlayerVehicleID(id),x+3,y,z);
		LinkVehicleToInterior(GetPlayerVehicleID(id), int);
		SetVehicleVirtualWorld(GetPlayerVehicleID(id), vw);
	}
	else
    	SetPlayerPos(id, x+2, y, z );
    return 1;
}

flags:gotopos(CMD_JR_MODERATOR);
CMD:gotopos(playerid, params[])
{
    new Float:x, Float:y, Float:z, int, vw;
    if(sscanf(params, "fffdd", x, y, z, int, vw))
	   return SendClientMessage(playerid, COLOR_ERROR, "/gotopos <x> <y> <z> <int> <vw>");
    SetPlayerInterior(playerid, int);
    Character_SetVirtualWorld(playerid, vw);
    Streamer_UpdateEx(playerid, x, y, z, vw, int);
    SetPlayerPos(playerid, x, y, z);
    return 1;
}

flags:check(CMD_JR_MODERATOR);
CMD:check(playerid, params[])
{
    new id;
    if(sscanf(params, "k<u>", id))
	   return SendClientMessage(playerid, COLOR_ERROR, "/check <playerid/partofname>");
    if(id < 0 || id >= MAX_PLAYERS  || !Character_IsLogged(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");
    Character_ShowStats(id, playerid);
	SendFormattedMessage(playerid, COLOR_GREEN, "Veicoli di %s (ID: %d)", Character_GetOOCName(id), id);
	new count = 0;
	foreach(new v : Vehicle)
    {
		if(Vehicle_GetOwnerID(v) == Character_GetID(id))
		{
			SendFormattedMessage(playerid, COLOR_GREEN, "ID: %d - %s", v, Vehicle_GetNameFromModel(GetVehicleModel(v)));
			count++;
		}
	}
	if(count == 0)
		SendClientMessage(playerid, COLOR_GREEN, "Il giocatore non possiede veicoli.");
	new ip[16];
	GetPlayerIp(playerid, ip, sizeof(ip));
	SendFormattedMessage(playerid, COLOR_GREEN, "L'IP del giocatore è %s", ip);
	SendClientMessage(playerid, COLOR_GREEN, "Per l'inventario, digita /invcheck <playerid/partofname>");
    return 1;
}

flags:invcheck(CMD_JR_MODERATOR);
CMD:invcheck(playerid, params[])
{
    new id;
    if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_ERROR, "/invcheck <playerid/partofname>");
    if(id < 0 || id >= MAX_PLAYERS  || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");
	Inventory_ShowInChat(Character_GetInventory(id), playerid, "Inventario Giocatore");
	// Inventory_ShowInChatStr(Character_GetInventory(id), playerid, Character_GetOOCNameStr(id));
	return 1;
}

flags:giveitem(CMD_ADMIN);
CMD:giveitem(playerid, params[])
{
	new id, itemid, quantity;
	if(sscanf(params, "k<u>k<item>d", id, itemid, quantity))
		return SendClientMessage(playerid, COLOR_ERROR, "/giveitem <playerid/partofname> <itemid/item_name> <quantità>");

	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è connesso.");

	if(itemid == INVALID_ITEM_ID || !ServerItem_IsValid(itemid) || itemid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "L'item inserito non è corretto.");

	if(! (-1000 <= quantity <= 1000))
		return SendClientMessage(playerid, COLOR_ERROR, "La quantità inserita è troppo grande. (Range: |-1000| - |1000| )");

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
			SendFormattedMessage(playerid, COLOR_ERROR, "%s (%d) non ha abbastanza spazio nell'inventario.", Character_GetOOCName(id), id);
		}
    }
    else if(quantity < 0)
    {
		quantity = -quantity;
		Character_DecreaseItemAmount(id, itemid, quantity);
		SendMessageToAdmins(0, COLOR_YELLOW, "[ADMIN-ALERT] %s (%s) ha rimosso %s (Qnt: %d) a %s (%d).", 
		AccountInfo[playerid][aName], Character_GetOOCName(playerid), ServerItem[itemid][sitemName], quantity, Character_GetOOCName(id), id);
	}
	//Inventory_Print(Character_GetInventory(id));
	return 1;
}

flags:resetinv(CMD_JR_MODERATOR);
CMD:resetinv(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_ERROR, "/resetinv <playerid/partofname>");
	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");
	Inventory_Reset(Character_GetInventory(id));
	SendMessageToAdmins(false, COLOR_ADMIN, "[ADMIN-ALERT]: %s ha resettato l'inventario di %s.", AccountInfo[playerid][aName], Character_GetOOCName(id));
	SendFormattedMessage(id, COLOR_ERROR, "[ADMIN]: %s ti ha resettato l'inventario.", AccountInfo[playerid][aName]);
	return 1;
}

flags:giveweapon(CMD_ADMIN);
CMD:giveweapon(playerid, params[])
{
	new id, wid, ammo;
	if(sscanf(params, "k<u>k<weapon>i", id, wid, ammo))
		return SendClientMessage(playerid, COLOR_ERROR, "/giveweapon <playerid/partofname> <weaponid/weapon name> <ammo>");
	if(ammo < 0 || ammo > 1000)
		return SendClientMessage(playerid, COLOR_ERROR, "Valore munizioni errato. (0 - 1000)");
	if(wid < 0)
		return SendClientMessage(playerid, COLOR_ERROR, "ID arma errato.");
	Character_GiveWeapon(id, wid, ammo);
	SendMessageToAdmins(false, COLOR_ADMIN, "[ADMIN-ALERT]: %s ha dato l'arma id %d con %d munizioni a %s.", AccountInfo[playerid][aName], wid, ammo, Character_GetOOCName(id));
	SendFormattedMessage(id, COLOR_GREEN, "[ADMIN]: %s ti ha dato l'arma id %d con %d munizioni.", AccountInfo[playerid][aName], wid, ammo);
	Log(AccountInfo[playerid][aName], Character_GetOOCName(id), "/giveweapon", wid);
	return 1;
}

flags:vowner(CMD_ADMIN);
CMD:vowner(playerid, params[])
{
	new vehicleid = GetPlayerVehicleID(playerid);
	if(vehicleid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo.");
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_ERROR, "/vowner <playerid/partofname>");
	if(!Vehicle_IsOwnable(vehicleid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su questo veicolo.");
	if(Vehicle_IsTemporary(vehicleid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare il comando su questo veicolo. Prima utilizza /vsave.");
	if(!IsPlayerConnected(id)  || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è connesso.");
	if(Character_GetOwnedVehicleCount(id) == MAX_VEHICLES_PER_PLAYER)
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore possiede già il massimo di veicoli personali.");
	Character_AddOwnedVehicle(id, vehicleid);
	Vehicle_Save(vehicleid);
	SendFormattedMessage(id, COLOR_GREEN, "L'admin %s (%d) ti ha settato proprietario del veicolo %d.", AccountInfo[playerid][aName], playerid, vehicleid);
	SendMessageToAdmins(true, COLOR_YELLOW, "[ADMIN ALERT]: %s (%d) ha settato %s (%d) proprietario del veicolo %d.", AccountInfo[playerid][aName], playerid, Character_GetOOCName(id), id, vehicleid);
	return 1;
}

flags:vdelete(CMD_MODERATOR);
CMD:vdelete(playerid, params[])
{
	new vehicleid = GetPlayerVehicleID(playerid);

	if(vehicleid <= 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo.");
	// If Vehicle Faction, don't make it callable

	if(Vehicle_GetID(vehicleid) != 0)
	{
		if(AccountInfo[playerid][aAdmin] < 3)
			return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su questo veicolo.");
		SendMessageToAdmins(true, COLOR_YELLOW, "[ADMIN-ALERT]: %s (%d) ha deletato il veicolo %d di %s.", AccountInfo[playerid][aName], playerid, vehicleid, VehicleInfo[vehicleid][vOwnerName]);
		Vehicle_Delete(vehicleid);
	}
	else
	{
		SendMessageToAdmins(true, COLOR_YELLOW, "[ADMIN-ALERT]: %s (%d) ha deletato il veicolo %d.", AccountInfo[playerid][aName], playerid, vehicleid);
		Vehicle_Destroy(vehicleid);
	}
	return 1;
}

flags:givevehicleitem(CMD_ADMIN);
CMD:givevehicleitem(playerid, params[])
{
    new id, itemid, quantity;
    if(sscanf(params, "dk<item>d", id, itemid, quantity))
	   return SendClientMessage(playerid, COLOR_ERROR, "/givevehicleitem <vehicleid> <itemid/item_name> <quantità>");
    
    if(id < 0 || id >= MAX_VEHICLES)
	   return SendClientMessage(playerid, COLOR_ERROR, "Il veicolo non è valido.");
    
    if(!Vehicle_HasInventory(id))
	   return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su questo veicolo.");

    if(id == INVALID_ITEM_ID || !ServerItem_IsValid(itemid))
	   return SendClientMessage(playerid, COLOR_ERROR, "L'item inserito non è corretto.");
    
    if(quantity > 0)
    {
	   new result = Vehicle_GiveItem(id, itemid, quantity);
	   if(result == INVENTORY_ADD_SUCCESS)
	   {
		  SendMessageToAdmins(0, COLOR_YELLOW, "[ADMIN-ALERT] %s (%s) ha givato %s (Qnt: %d) nel veicolo id %d.", 
		  AccountInfo[playerid][aName], Character_GetOOCName(playerid), ServerItem[itemid][sitemName], quantity, id);
	   }
	   else if(result == INVENTORY_NO_SPACE)
	   {
		  SendFormattedMessage(playerid, COLOR_ERROR, "Il veicolo %d non ha abbastanza spazio nell'inventario.", id);
	   }
    }
    else if(quantity < 0)
    {
	   quantity = -quantity;
	   Vehicle_DecreaseItemAmount(id, itemid, quantity);
	   SendMessageToAdmins(0, COLOR_YELLOW, "[ADMIN-ALERT] %s (%s) ha rimosso %s (Qnt: %d) al veicolo %d.", 
	   AccountInfo[playerid][aName], Character_GetOOCName(playerid), ServerItem[itemid][sitemName], quantity, id);
    }
    //Inventory_Print(Character_GetInventory(id));
    return 1;
}

flags:ajail(CMD_MODERATOR);
CMD:ajail(playerid, params[])
{
	new id, time, reason[64];
	if(sscanf(params, "k<u>ds[64]", id, time, reason))
		return SendClientMessage(playerid, COLOR_ERROR, "/ajail <playerid/partofname> <minuti> <motivo>");
	if(!Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");
	
	if(time < 1)
		return SendClientMessage(playerid, COLOR_ERROR, "Minuti non validi. (1 - +inf)");

	PlayerTextDrawShow(id, pJailTimeText[playerid]);
	
	SetCameraBehindPlayer(id);

	ClearAnimations(id);
	Character_OffDuty(id);

	Character_SetJailTime(id, time);
	Character_SetToJailPos(id);

	SendFormattedMessage(id, COLOR_ERROR, "Sei stato jailato da %s (%d) per %d minuti. Motivo: %s", AccountInfo[playerid][aName], playerid, time, reason);

	SendMessageToAdmins(false, COLOR_ADMIN, "[ADM-CMD] %s (%d) ha jailato %s (%d) per %d minuti. Motivo: %s", AccountInfo[playerid][aName], playerid, Character_GetOOCName(id), id, time, reason);
	Log(Character_GetOOCName(playerid), Character_GetOOCName(id), "/ajail", time);
	return 1;
}

flags:unjail(CMD_MODERATOR);
CMD:unjail(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_ERROR, "/unjail <playerid/partofname>");
	if(!Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");
	if(!Character_IsJailed(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non sta scontando una pena.");
	
	Character_SetJailTimeAsSeconds(id, 2);
	SendFormattedMessage(id, COLOR_ERROR, "Sei stato unjailato da %s", AccountInfo[playerid][aName]);

	SendMessageToAdmins(false, COLOR_ADMIN, "[ADM-CMD] %s (%d) ha unjailato %s (%d).", AccountInfo[playerid][aName], playerid, Character_GetOOCName(id), id);
	Log(Character_GetOOCName(playerid), Character_GetOOCName(id), "/unjail");
	return 1;
}

flags:freeze(CMD_JR_MODERATOR);
CMD:freeze(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_ERROR, "/freeze <playerid/partofname>");
	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");
	Character_SetFreezed(id, true);
	SendClientMessage(id, COLOR_ERROR, "Sei stato freezato da un admin.");
	SendClientMessage(id, COLOR_ERROR, "Hai freezato il giocatore.");
	return 1;
}

flags:unfreeze(CMD_JR_MODERATOR);
CMD:unfreeze(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_ERROR, "/unfreeze <playerid/partofname>");
	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");
	Character_SetFreezed(id, false);
	SendClientMessage(id, COLOR_ERROR, "Sei stato unfreezato da un admin.");
	SendClientMessage(id, COLOR_ERROR, "Hai unfreezato il giocatore.");
	return 1;
}

flags:randomweather(CMD_JR_MODERATOR);
CMD:randomweather(playerid, params[])
{
	Server_SetCurrentWeather(Server_GetNextWeather());
	Server_UpdateWeather();
	return 1;
}

flags:settime(CMD_JR_MODERATOR);
CMD:settime(playerid, params[])
{
	new w = strval(params);
	if(isnull(params) || w < 0 || w > 24)
		return SendClientMessage(playerid, COLOR_ERROR, "/settime <hour (0-24)>");
	SendMessageToAdmins(true, COLOR_ADMIN, "%s ha utilizzato il comando /settime. Orario: %d", AccountInfo[playerid][aName], w);
	SetWorldTime(w);
	return 1;
}

flags:setweather(CMD_JR_MODERATOR);
CMD:setweather(playerid, params[])
{
	if(isnull(params))
		return SendClientMessage(playerid, COLOR_ERROR, "/setweather <weather id>");
	new w = strval(params);
	if(0 <= w <= 20)
	{
		SetWeather(w);
		foreach(new i : Player)
			SetPlayerWeather(i, w);
		SendMessageToAdmins(false, COLOR_ADMIN, "%s (%d) ha cambiato il tempo metereologico. ID: %d.", AccountInfo[playerid][aName], playerid, w);
	}
	else 
		SendClientMessage(playerid, COLOR_ERROR, "ID Errato.");
	return 1;
}

static pAdminAnnounceMessage[MAX_PLAYERS][256];
flags:n(CMD_JR_MODERATOR);
CMD:n(playerid, params[])
{
	if(isnull(params) || strlen(params) > 250)
		return SendClientMessage(playerid, COLOR_ERROR, "/n <testo>");
	set(pAdminAnnounceMessage[playerid], params);
	Dialog_Show(playerid, Dialog_AdminAnnounce, DIALOG_STYLE_MSGBOX, "Annuncio Globale", "Sei sicuro di voler inviare l'annuncio a tutti i giocatori online?\nTesto: %s", "Si", "No", params);
	return 1;
}

Dialog:Dialog_AdminAnnounce(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;
	SendClientMessageToAllStr(0xAFAFAFFF, str_format("Admin %s: %s", AccountInfo[playerid][aName], pAdminAnnounceMessage[playerid]));
	return 1;
}


flags:banip(CMD_ADMIN);
CMD:banip(playerid, params[])
{
	new id, reason[128];
	if(sscanf(params, "k<u>s[128]", id, reason))
		return SendClientMessage(playerid, COLOR_ERROR, "/banip <playerid/partofname> <motivo>");
	if(!IsPlayerConnected(id) || IsPlayerNPC(id) || !Account_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");
	
	SendMessageToAdmins(true, COLOR_ADMIN, "[ADMIN-ALERTS]: %s ha bannato %s per IP. Motivo: %s", AccountInfo[playerid][aName], AccountInfo[id][aName], reason);
	SendFormattedMessage(id, COLOR_ORANGE, "Sei stato bannato da %s. Motivo: %s", AccountInfo[playerid][aName], reason);

	wait_ticks(1);

	BanEx(id, reason);
	return 1;
}

flags:banaccount(CMD_ADMIN);
CMD:banaccount(playerid, params[])
{
	new id, reason[128];
	if(sscanf(params, "k<u>s[128]", id, reason))
		return SendClientMessage(playerid, COLOR_ERROR, "/banaccount <playerid/partofname> <motivo>");
	if(!IsPlayerConnected(id) || IsPlayerNPC(id) || !Account_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");
	
	BanAccount(id, playerid, reason);
	return 1;
}

flags:banofflineaccount(CMD_ADMIN);
CMD:banofflineaccount(playerid, params[])
{
	new name[MAX_PLAYER_NAME];
	if(sscanf(params, "s[24]", name))
		return SendClientMessage(playerid, COLOR_ERROR, "/banofflineaccount <nome>");
	inline FindAccount()
	{
		if(cache_num_rows() > 0)
		{
			new id, e_name[MAX_PLAYER_NAME];
			cache_get_value_index_int(0, 0, id);
			cache_get_value_index(0, 1, e_name);
			mysql_tquery_f(gMySQL, "UPDATE `accounts` SET Banned = '1' WHERE ID = '%d'", id);
			SendFormattedMessage(playerid, COLOR_ORANGE, "L'account \"%s\" è stato bannato.", e_name);
			SendMessageToAdmins(true, COLOR_ORANGE, "[ADMIN-ALERT]: %s ha bannato l'account \"%s\".", AccountInfo[playerid][aName], e_name);
			Log(AccountInfo[playerid][aName], e_name, "/banofflineaccount");
		}
		else
		{
			SendFormattedMessage(playerid, COLOR_ERROR, "L'account non esiste.");
		}
	}
	MySQL_TQueryInline(gMySQL, using inline FindAccount, "SELECT ID, Name FROM `accounts` WHERE LOWER(Name) = LOWER('%e')", name);
	return 1;
}

flags:unbanaccount(CMD_MODERATOR);
CMD:unbanaccount(playerid, params[])
{
	new name[MAX_PLAYER_NAME];
	if(sscanf(params, "s[24]", name))
		return SendClientMessage(playerid, COLOR_ERROR, "/unbanaccount <nome>");
	inline FindAccount()
	{
		if(cache_num_rows() > 0)
		{
			new id, e_name[MAX_PLAYER_NAME];
			cache_get_value_index_int(0, 0, id);
			cache_get_value_index(0, 1, e_name);
			mysql_tquery_f(gMySQL, "UPDATE `accounts` SET Banned = '0', BanExpiry = '0' WHERE ID = '%d'", id);
			SendFormattedMessage(playerid, COLOR_ORANGE, "L'account \"%s\" è stato sbannato.", e_name);
			SendMessageToAdmins(true, COLOR_ORANGE, "[ADMIN-ALERT]: %s ha sbannato l'account \"%s\".", AccountInfo[playerid][aName], e_name);
			Log(AccountInfo[playerid][aName], e_name, "/unbanaccount");
		}
		else
		{
			SendFormattedMessage(playerid, COLOR_ERROR, "L'account non esiste.");
		}
	}
	MySQL_TQueryInline(gMySQL, using inline FindAccount, "SELECT ID, Name FROM `accounts` WHERE LOWER(Name) = LOWER('%e')", name);
	return 1;
}

flags:ban(CMD_MODERATOR);
CMD:ban(playerid, params[])
{
	new id, days, reason[128];
	if(sscanf(params, "k<u>ds[128]", id, days, reason))
		return SendClientMessage(playerid, COLOR_ERROR, "/ban <playerid/partofname> <giorni> <motivo>");
	if(!IsPlayerConnected(id) || IsPlayerNPC(id) || !Account_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");
	if(days < 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Giorni non validi. Usa 0 per bannare permanentemente.");
	BanCharacter(id, playerid, days, reason);
	return 1;
}

flags:unban(CMD_MODERATOR);
CMD:unban(playerid, params[])
{
	new name[MAX_PLAYER_NAME];
	if(sscanf(params, "s[24]", name))
		return SendClientMessage(playerid, COLOR_ERROR, "/unban <nome>");
	inline FindAccount()
	{
		if(cache_num_rows() > 0)
		{
			new id, e_name[MAX_PLAYER_NAME];
			cache_get_value_index_int(0, 0, id);
			cache_get_value_index(0, 1, e_name);
			mysql_tquery_f(gMySQL, "UPDATE `characters` SET Banned = '0', BanExpiry = '0' WHERE ID = '%d'", id);
			SendFormattedMessage(playerid, COLOR_ORANGE, "Il personaggio \"%s\" è stato sbannato.", e_name);
			SendMessageToAdmins(true, COLOR_ORANGE, "[ADMIN-ALERT]: %s ha sbannato il personaggio \"%s\".", AccountInfo[playerid][aName], e_name);
			Log(AccountInfo[playerid][aName], e_name, "/unban");
		}
		else
		{
			SendFormattedMessage(playerid, COLOR_ERROR, "Il personaggio non esiste.");
		}
	}
	MySQL_TQueryInline(gMySQL, using inline FindAccount, "SELECT ID, Name FROM `characters` WHERE LOWER(Name) = LOWER('%e')", name);
	return 1;
}

stock BanCharacter(playerid, adminid, days, reason[])
{
	Character_SetBanned(playerid, 1);

	Character_SetBanExpiry(playerid, (days > 0) ? (gettime() + 60 * 60 * 24 * days) : cellmax );
	if(days > 0)
	{
		SendFormattedMessage(playerid, COLOR_ORANGE, "Sei stato bannato da %s per %d giorni. Motivo: %s", AccountInfo[adminid][aName], days, reason);
		SendMessageToAdmins(true, COLOR_ADMIN, "[ADMIN-ALERT]: %s ha bannato %s per %d giorni. Motivo: %s", AccountInfo[adminid][aName], Character_GetOOCName(playerid), days, reason);
	}
	else
	{
		SendFormattedMessage(playerid, COLOR_ORANGE, "Sei stato bannato da %s permanentemente. Motivo: %s", AccountInfo[adminid][aName], reason);
		SendMessageToAdmins(true, COLOR_ADMIN, "[ADMIN-ALERT]: %s ha bannato %s permanentemente. Motivo: %s", AccountInfo[adminid][aName], Character_GetOOCName(playerid), reason);
	}

	Log(AccountInfo[adminid][aName], AccountInfo[playerid][aName], "BanCharacter", days);

	Character_Save(playerid);
	KickEx(playerid);
	return 1;
}

stock BanAccount(playerid, adminid, reason[])
{
	Account_SetBanned(playerid, 1);
	mysql_tquery_f(gMySQL, "UPDATE `accounts` SET Banned = '1' WHERE ID = '%d'", Account_GetID(playerid));

	SendFormattedMessage(playerid, COLOR_ORANGE, "Il tuo account (%s) è stato bannato da %s. Motivo: %s", AccountInfo[playerid][aName], AccountInfo[adminid][aName], reason);

	SendMessageToAdmins(true, COLOR_ADMIN, "[ADMIN-ALERT]: %s ha bannato l'account di %s (%s). Motivo: %s",
	AccountInfo[adminid][aName], AccountInfo[playerid][aName], Character_GetOOCName(playerid), reason);

	Log(AccountInfo[adminid][aName], AccountInfo[playerid][aName], "BanAccount");

	KickEx(playerid);
	return 1;
}