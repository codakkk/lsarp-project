#include <player\commands\chat_commands>
#include <player\commands\premium_commands>
#include <player\commands\inventory_commands>
#include <player\commands\vehicle_commands>
#include <player\commands\interaction_commands>
#include <YSI_Coding\y_hooks>

// CHANGE ME/AME COLOR

hook OnPlayerClearData(playerid)
{
    if(pVehicleSeller[playerid] != -1 && pVehicleSellingTo[pVehicleSeller[playerid]] == playerid)
    {
	   new seller = pVehicleSeller[playerid];
	   SendClientMessage(seller, COLOR_ERROR, "Il giocatore a cui volevi vendere il veicolo si è disconnesso.");
	   pVehicleSellingTo[seller] = -1;
	   pVehicleSellingPrice[seller] = 0;
	   pSellingVehicleID[seller] = 0;
    }
    else if(pVehicleSellingTo[playerid] != -1 && pVehicleSeller[pVehicleSellingTo[playerid]] == playerid)
    {
	   new customer = pVehicleSellingTo[playerid];
	   SendClientMessage(customer, COLOR_ERROR, "Il giocatore che voleva venderti il veicolo si è disconnesso.");
	   pVehicleSeller[customer] = -1;
	   pVehicleSellingPrice[customer] = 0;
	   pSellingVehicleID[customer] = 0;
    }
    pVehicleSellingPrice[playerid] = 0;
    pSellingVehicleID[playerid] = 0;
    pVehicleSellingTo[playerid] = -1;
    pVehicleSeller[playerid] = -1;
    return 1;
}

flags:info(CMD_USER);
CMD:info(playerid, params[])
{
    Character_ShowStats(playerid, playerid);
    return 1;
}
alias:info("stats");

flags:tog(CMD_USER);
CMD:tog(playerid, params[])
{
	if(isnull(params) || strlen(params) > 16)
		return SendClientMessage(playerid, COLOR_ERROR, "/tog <fchat - hud - pm - ooc>");
	if(!strcmp(params, "fchat", true))
	{
		if(Character_GetFaction(playerid) == INVALID_FACTION_ID)
			return SendClientMessage(playerid, COLOR_ERROR, "Non fai parte di una fazione.");
		Character_SetFactionOOCEnabled(playerid, !Character_IsFactionOOCEnabled(playerid));
		if(Character_IsFactionOOCEnabled(playerid))
			SendClientMessage(playerid, COLOR_GREEN, "Hai attivato la chat fazione OOC.");
		else
			SendClientMessage(playerid, COLOR_GREEN, "Hai disattivato la chat fazione OOC.");
	}
	else if(!strcmp(params, "hud", true))
	{
		Account_SetHUDEnabled(playerid, !Account_HasHUDEnabled(playerid));
		if(Account_HasHUDEnabled(playerid))
		{
			TextDrawShowForPlayer(playerid, Clock);
			SetHealthBarVisible(playerid, true);
		}
		else
		{
			TextDrawHideForPlayer(playerid, Clock);
			SetHealthBarVisible(playerid, false);

		}
	}
	else if(!strcmp(params, "pm", true))
	{
		pc_cmd_blockpm(playerid, "all");
	}
	else if(!strcmp(params, "ooc", true))
	{
		pc_cmd_blockb(playerid, "all");
	}
	else
	{
		return SendClientMessage(playerid, COLOR_ERROR, "/tog <fchat - hud - pm - ooc>");
	}
	return 1;
}

flags:dom(CMD_USER);
CMD:dom(playerid, params[])
{
    if(isnull(params))
	   return SendClientMessage(playerid, COLOR_ERROR, "/dom <testo>");
    new seconds = (AccountInfo[playerid][aPremium] > 0) ? 10 : 30;
    if(GetTickCount() - pLastAdminQuestionTime[playerid] < 1000 * seconds)
	   return SendFormattedMessage(playerid, COLOR_ERROR, "Puoi inviare una domanda ogni %d secondi.", seconds);
    if(AccountInfo[playerid][aPremium])
    {
	   SendMessageToAdmins(0, 0xEA7500FF, "(( [PREMIUM] %s (%d) chiede: %s ))", Character_GetOOCName(playerid), playerid, params);
	   SendClientMessage(playerid, -1, "La domanda è stata inviata ai moderatori online. Attendi.");
	   SendClientMessage(playerid, -1, "Essendo un utente Premium, la tua domanda avrà maggiore priorità.");
    }
    else
    {
	   SendMessageToAdmins(0, 0xEA7500FF, "(( %s (%d) chiede: %s ))", Character_GetOOCName(playerid), playerid, params);
	   SendClientMessage(playerid, -1, "La domanda è stata inviata ai moderatori online. Attendi.");
    }
    pLastAdminQuestionTime[playerid] = GetTickCount();
    return 1;
}

flags:afk(CMD_USER);
CMD:afk(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_ERROR, "/afk <playerid/partofname>");
	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Non valido.");
	if(Account_GetAdminLevel(playerid) < 1 && Account_GetAdminLevel(id) > 1)
		SendFormattedMessage(playerid, -1, (Character_IsAFK(id)) ? ("%s è AFK.") : ("%s non è AFK."), Character_GetOOCName(id));
	else
		SendFormattedMessage(playerid, -1, (Character_IsAFK(id)) ? ("%s è AFK da %d minuti (%d secondi).") : ("%s non è AFK."), Character_GetOOCName(id), Character_GetAFKTime(id)/60, Character_GetAFKTime(id));
	return 1;
}

flags:aiuto(CMD_USER);
CMD:aiuto(playerid, params[])
{
    SendClientMessage(playerid, -1, "[GENERALE]: /info - /dom - /compra - /annulla - /sconosciuto");
    SendClientMessage(playerid, -1, "[GENERALE]: /rimuovi - /hotkeys - /arma - /id - /dp - /invmode");
	SendClientMessage(playerid, -1, "[GENERALE]: /afk - /stilechat - /stilelotta - /stilecamminata");
    SendClientMessage(playerid, -1, "[CHAT]: /b - /me - /ame - /low - /melow - /do - /dolow - (/s)hout");
    SendClientMessage(playerid, -1, "[CHAT]: (/w)hisper - (/cw)hisper - /pm - /blockb - /blockpm");
    SendClientMessage(playerid, -1, "[ALTRO]: /animlist - /vcmds - /invcmds - /zpoints - /furniturecmds");
	new factionid = Character_GetFaction(playerid);
	if(factionid != -1)
	{
		if(Faction_GetType(factionid) == FACTION_TYPE_POLICE || 
			Faction_GetType(factionid) == FACTION_TYPE_MEDICAL ||
			Faction_GetType(factionid) == FACTION_TYPE_GOVERNAMENT)
		{
			SendClientMessage(playerid, COLOR_SLATEBLUE, "[FAZIONE]: (/f)azione - /togf - /faiuto");
		}
	}
	if(AccountInfo[playerid][aAdmin] > 0)
		SendClientMessage(playerid, COLOR_GREEN, "[SUPPORTER]: /scmds");
	if(AccountInfo[playerid][aAdmin] > 1)
		SendClientMessage(playerid, COLOR_GREEN, "[ADMIN]: /acmds");
    return 1;
}
alias:aiuto("cmds", "help");

flags:faiuto(CMD_USER);
CMD:faiuto(playerid, params[])
{
	new factionid = Character_GetFaction(playerid);
	if(factionid != -1)
	{
		if(Faction_GetType(factionid) == FACTION_TYPE_POLICE)
		{
			SendClientMessage(playerid, COLOR_SLATEBLUE, "[GUARDIA NAZIONALE]: /ammanetta - /trascina - /arresta");
		}
	}
	return 1;
}

flags:vcmds(CMD_ALIVE_USER);
CMD:vcmds(playerid, params[])
{
	SendClientMessage(playerid, -1, "[VEICOLI]: /motore - (/fin)estrino - /vmenu - /vapri - /vchiudi");
	SendClientMessage(playerid, -1, "[VEICOLI]: /vluci - /vparcheggia - /vpark - /vbagagliaio");
	SendClientMessage(playerid, -1, "[VEICOLI]: (/vdis)assembla - (/vdep)osita");
	return 1;
}

flags:invcmds(CMD_ALIVE_USER);
CMD:invcmds(playerid, params[])
{
	SendClientMessage(playerid, -1, "[INVENTARIO]: (/inv)entario - (/dep)osita - (/dis)assembla - /gettaarma");
	SendClientMessage(playerid, -1, "[INVENTARIO]: /invmode - /usa - /passa - /getta");
	return 1;
}

flags:hotkeys(CMD_ALIVE_USER);
CMD:hotkeys(playerid, params[])
{
	Account_SetHotKeysEnabled(playerid, !Account_HasHotKeysEnabled(playerid));
    if(Account_HasHotKeysEnabled(playerid))
    {
		SendClientMessage(playerid, COLOR_GREEN, "Hai abilitato le HotKeys.");
		SendClientMessage(playerid, -1, "Ora puoi utilizzare anche:");
		//SendClientMessage(playerid, -1, "{00FF00}ENTER/INVIO{FFFFFF}: Entrare/Uscire da un edificio.");
		SendClientMessage(playerid, -1, "{00FF00}Y{FFFFFF}: Entrare/uscire da un edificio e per accendere/spegnere un veicolo.");
		SendClientMessage(playerid, -1, "{00FF00}N{FFFFFF}: Apri/Chiudi veicolo.");
		SendClientMessage(playerid, -1, "{00FF00}2{FFFFFF}: Accendi/Spegni fari.");
		SendClientMessage(playerid, -1, "{00FF00}LALT{FFFFFF}: Gestire la propria casa.");
		SendClientMessage(playerid, COLOR_GREEN, "N.B: I comandi sono relativi al mapping default di GTA.");
		SendClientMessage(playerid, COLOR_GREEN, "Se hai cambiato il mapping default, i tasti potrebbero essere diversi.");
	}
	else
	{
		SendClientMessage(playerid, COLOR_GREEN, "Hai disabilitato le HotKeys. Ora puoi utilizzare soltanto i comandi.");
		SendClientMessage(playerid, COLOR_GREEN, "/entra: entrare/uscire da un edificio.");
		SendClientMessage(playerid, COLOR_GREEN, "/motore: accendere/spegnere un veicolo.");
		SendClientMessage(playerid, COLOR_GREEN, "/vluci: accendere/spegnere li fari del veicolo.");
		SendClientMessage(playerid, COLOR_GREEN, "/raccogli: raccogliere un oggetto da terra.");
	}
	return 1;
}

flags:id(CMD_ALIVE_USER);
CMD:id(playerid, params[])
{
	new id;
	if(isnull(params))
		return SendClientMessage(playerid, COLOR_ERROR, "/id <playerid/partofname>");
	if(IsNumeric(params))
	{
		id = strval(params);
		if(!IsPlayerConnected(id) || !Character_IsLogged(id))
			return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è connesso.");
		SendFormattedMessage(playerid, -1, "%s (%d) - Livello: %d", Character_GetOOCName(id), id, Character_GetLevel(id));
	}
	else
	{
		new c = 0;
		foreach(new i : Player)
		{
			if(!Character_IsLogged(i))
				continue;
			if(strfind(Character_GetOOCName(i), params, true) != -1)
			{
				SendFormattedMessage(playerid, -1, "%s (%d) - Livello: %d", Character_GetOOCName(i), i, Character_GetLevel(i));
				c++;
			}
		}
		if(c == 0)
			SendClientMessage(playerid, COLOR_ERROR, "Giocatore non collegato.");
	}
	return 1;
}

flags:lasciacarcere(CMD_ALIVE_USER);
CMD:lasciacarcere(playerid, params[])
{
	if(!Character_IsJailed(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei in carcere");
	if(Character_GetJailTime(playerid) > 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai ancora scontato la tua pena.");
	new pickupid = Character_GetLastPickup(playerid), id, E_ELEMENT_TYPE:type;
	if(!Pickup_GetInfo(pickupid, id, type) || type != ELEMENT_TYPE_JAIL_EXIT || !IsPlayerInRangeOfPickup(playerid, pickupid, 5.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino all'uscita della prigione.");
	
	Character_SetJailTime(playerid, 0);
	Character_SetICJailed(playerid, 0);
	//Character_Spawn(playerid);
	SetPlayerPos(playerid, 1313.0187, -2063.6877, 57.1440);
	SendClientMessage(playerid, COLOR_GREEN, "Hai lasciato il carcere.");
	return 1;
}

flags:stilelotta(CMD_ALIVE_USER);
CMD:stilelotta(playerid, params[])
{
	new id;
	if(sscanf(params, "d", id) || id < 0 || id > 3)
	{
		// Show Dialog? nah
		return SendClientMessage(playerid, COLOR_ERROR, "/stilelotta <id (0 - 3)>");
	}
	else
	{
		switch(id)
		{
			case 0:
			{
				Character_SetFightingStyle(playerid, FIGHT_STYLE_NORMAL);
			}
			case 1:
			{
				Character_SetFightingStyle(playerid, FIGHT_STYLE_BOXING);
			}
			case 2:
			{
				Character_SetFightingStyle(playerid, FIGHT_STYLE_KUNGFU);
			}
			case 3:
			{
				Character_SetFightingStyle(playerid, FIGHT_STYLE_GRABKICK);
			}
		}
		SendClientMessage(playerid, COLOR_GREEN, "Hai cambiato il tuo stile di combattimento.");
		Character_Save(playerid);
	}
	return 1;
}

flags:cambiapassword(CMD_USER);
CMD:cambiapassword(playerid, params[])
{
	gLoginAttempts{playerid} = 0;
	Dialog_Show(playerid, Dialog_ChangePassword, DIALOG_STYLE_INPUT, "Cambia Password", "Inserisci la tua password attuale.", "Continua", "Chiudi");
	return 1;
}

Dialog:Dialog_ChangePassword(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;

	if(gLoginAttempts{playerid} >= 3)
		return KickEx(playerid);

	inline OnCheck()
	{
		if(cache_num_rows() > 0)
		{
			Dialog_Show(playerid, Dialog_SetNewPassword, DIALOG_STYLE_INPUT, "Nuova password", "Inserisci la nuova password.", "Continua", "Annulla");
		}
		else
		{
			Dialog_Show(playerid, Dialog_ChangePassword, DIALOG_STYLE_INPUT, "Cambia Password", "{FF0000}La password inserita non è valida.\n{FFFFFF}Inserisci la tua password per continuare.\nTentativo: %d/%d", "Continua", "Annulla", gLoginAttempts{playerid}, MAX_LOGIN_ATTEMPTS);
			gLoginAttempts{playerid}++;
		}
	}

	new 
		psw[WHIRLPOOL_LEN];
	WP_Hash(psw, sizeof(psw), inputtext);
	MySQL_TQueryInline(gMySQL, using inline OnCheck, "SELECT * FROM accounts WHERE ID = '%d' AND Password = '%e'", Account_GetID(playerid), psw);
	return 1;
}

Dialog:Dialog_SetNewPassword(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;
	if(!IsValidPassword(inputtext))
		return Dialog_Show(playerid, Dialog_SetNewPassword, DIALOG_STYLE_INPUT, "Nuova password", "La password inserita non è corretta.\nInserisci la nuova password.\nLa password deve contenere almeno:\n- 8 caratteri.\n- Almeno una lettera maiuscola.\n- Almeno un numero.\n- Almeno un carattere speciale.\nLa password non puo':\n- Cominciare o finire con uno spazio\n- Contenere come caratteri speciali: %, \\, ', `, \".", "Continua", "Annulla");
	inline OnUpdate()
	{
		SendClientMessage(playerid, COLOR_GREEN, "Hai cambiato la tua password con successo.");
	}
	new 
		psw[WHIRLPOOL_LEN];
	WP_Hash(psw, sizeof(psw), inputtext);
	MySQL_TQueryInline(gMySQL, using inline OnUpdate, "UPDATE `accounts` SET Password = '%e' WHERE ID = '%d';", psw, Account_GetID(playerid));
	return 1;
}


stock bool:IsValidPassword(const password[])
{
	if(strlen(password) < 8 || strlen(password) > 15 || password[0] == ' ' || password[strlen(password)-1] == ' ')
		return false;
	
	new 
		specialCounter = 0, 
		numberCounter = 0, 
		capitalCounter = 0
	;

	for(new i = 0, j = strlen(password); i < j; ++i)
	{
		// Unallowed Characters
		if(password[i] == '%' || password[i] == '\\' || password[i] == '\'' || password[i] == '`' || password[i] == '\"')
			return false;
		if('0' <= password[i] <= '9')
			numberCounter++;
		else if('A' <= password[i] <= 'Z')
			capitalCounter++;
		else if(password[i] == ' ')
			continue;
		else
			specialCounter++;
	}
	if(specialCounter > 0 && numberCounter > 0 && capitalCounter > 0)
		return true;
	return false;
}