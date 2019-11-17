#include <player_system\commands\chat_commands>
#include <player_system\commands\premium_commands>
#include <player_system\commands\inventory_commands>
#include <player_system\commands\vehicle_commands>
#include <player_system\commands\interaction_commands>
#include <YSI_Coding\y_hooks>

static stock
	LastAdminQuestionTime[MAX_PLAYERS],
	LoginAttempts[MAX_PLAYERS char];

hook OnCharacterClearData(playerid)
{
	LastAdminQuestionTime[playerid] = 0;
	return 1;
}

hook OnPlayerUpdate(playerid)
{
	if(!Player_HasHUDEnabled(playerid))
	{
		SetHealthBarVisible(playerid, false);
	}
	return 1;
}

// CHANGE ME/AME COLOR



flags:info(CMD_USER);
CMD:info(playerid, params[])
{
    Character_ShowStats(playerid, playerid);
    return 1;
}
alias:info("stats");

flags:time(CMD_ALIVE_USER);
CMD:time(playerid, params[])
{
	new h, m, s;
	gettime(h, m, s);
	new String:str = str_format("~g~~w~%02d:%02d:%02d", h, m, s);
	GameTextForPlayerStr(playerid, str, 2000, 1);
	Character_AMe(playerid, "controlla l'orario.");
	return 1;
}
alias:time("orario");


flags:tog(CMD_USER);
CMD:tog(playerid, params[])
{
	if(!isnull(params))
	{

		if(!strcmp(params, "fchat", true))
		{
			if(Character_GetFaction(playerid) == INVALID_FACTION_ID)
				return SendClientMessage(playerid, COLOR_ERROR, "Non fai parte di una fazione.");
			Character_SetFactionOOCEnabled(playerid, !Character_IsFactionOOCEnabled(playerid));
			if(Character_IsFactionOOCEnabled(playerid))
				SendClientMessage(playerid, COLOR_GREEN, "Hai attivato la chat fazione OOC.");
			else
				SendClientMessage(playerid, COLOR_GREEN, "Hai disattivato la chat fazione OOC.");
			return 1;
		}
		else if(!strcmp(params, "hud", true))
		{
			Player_SetHUDEnabled(playerid, !Player_HasHUDEnabled(playerid));
			if(Player_HasHUDEnabled(playerid))
			{
				SetHealthBarVisible(playerid, true);
				if(IsPlayerInAnyVehicle(playerid) && GetPlayerVehicleSeat(playerid) == 0 && !IsABike(GetPlayerVehicleID(playerid)))
					TextDraw_ShowFuel(playerid, true);
			}
			else
			{
				SetHealthBarVisible(playerid, false);
				TextDraw_ShowFuel(playerid, false);
			}
			return 1;
		}
		else if(!strcmp(params, "pm", true))
		{
			callcmd::blockpm(playerid, "all");
			return 1;
		}
		else if(!strcmp(params, "ooc", true))
		{
			callcmd::blockb(playerid, "all");
			return 1;
		}
		else if(!strcmp(params, "cheat", true))
		{
			Player_SetCheatAlertsEnabled(playerid, !Player_HasCheatAlertsEnabled(playerid));
			if(Player_HasCheatAlertsEnabled(playerid))
				SendClientMessage(playerid, COLOR_GREEN, "Hai abilitato gli avvisi cheat.");
			else
				SendClientMessage(playerid, COLOR_GREEN, "Hai disattivato gli avvisi cheat.");
			return 1;
		}
	}

	SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /tog <fchat - hud - pm - ooc>");
	if(Account_GetAdminLevel(playerid) > 1)
		SendClientMessage(playerid, COLOR_GREY, "Admin: cheat");
	return 1;
}

/*flags:domanda(CMD_USER);
CMD:domanda(playerid, params[])
{
    if(isnull(params))
	   return SendClientMessage(playerid, COLOR_ERROR, "(/dom)anda <testo>");
    new seconds = (Account_GetPremiumLevel(playerid) > 0) ? 10 : 30;
    if(GetTickCount() - LastAdminQuestionTime[playerid] < 1000 * seconds)
	   return SendFormattedMessage(playerid, COLOR_ERROR, "Puoi inviare una domanda ogni %d secondi.", seconds);
    if(Account_GetPremiumLevel(playerid) > 0)
    {
	   SendMessageToAdmins(0, COLOR_ORANGE, "(( [PREMIUM] %s (%d) chiede: %s ))", Character_GetOOCName(playerid), playerid, params);
	   SendClientMessage(playerid, -1, "La domanda è stata inviata ai moderatori online. Attendi.");
	   SendClientMessage(playerid, -1, "Essendo un utente Premium, la tua domanda avrà maggiore priorità.");
    }
    else
    {
	   SendMessageToAdmins(0, COLOR_ORANGE, "(( %s (%d) chiede: %s ))", Character_GetOOCName(playerid), playerid, params);
	   SendClientMessage(playerid, -1, "La domanda è stata inviata ai moderatori online. Attendi.");
    }
    LastAdminQuestionTime[playerid] = GetTickCount();
    return 1;
}
alias:domanda("dom");*/

flags:report(CMD_USER);
CMD:report(playerid, params[])
{
	new seconds = (Account_GetPremiumLevel(playerid) > 0) ? 10 : 30;
    if(GetTickCount() - LastAdminQuestionTime[playerid] < 1000 * seconds)
	   return SendFormattedMessage(playerid, COLOR_ERROR, "Puoi inviare una domanda/report ogni %d secondi.", seconds);

	new id, reason[128];
	if(sscanf(params, "k<u>s[128]", id, reason))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /report <playerid/partofname/#####> <motivo>");
	
	if(!Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");
	
	if(playerid == id)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi reportare te stesso.");
	
	SendMessageToAdmins(0, COLOR_ORANGE, "%s (%d) ha reportato %s (%d). Motivo: %s", Character_GetOOCName(playerid), playerid, Character_GetOOCName(id), id, reason);
	SendFormattedMessage(playerid, -1, "Hai reportato %s (%d). Motivo: %s", Character_GetOOCName(id), id, reason);

	LastAdminQuestionTime[playerid] = GetTickCount();
	return 1;
}

flags:afk(CMD_USER);
CMD:afk(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /afk [playerid/partofname]");
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
	SendClientMessage(playerid, 0xCCE6E6FF, "_______________www.lv-rp.it_______________");
	SendClientMessage(playerid, 0xCCE6E6FF, "[ACCOUNT] /stats /inv /oggetto /hotkeys /lostitems /zpoints");
    SendClientMessage(playerid, 0xCCE6E6FF, "[GENERALE]: /luoghi /paga /time /compra /id /cellinfo /chiama /bussa /blindfold /sa /animlist");
    SendClientMessage(playerid, 0xCCE6E6FF, "[GENERALE]: (/pub)blicita /documenti /report /stilechat /stilelotta /stilecamminata /multe");
	SendClientMessage(playerid, 0xCCE6E6FF, "[CHAT]: (/w)hisper (/s)hout /me /b(OOC) /do /cw /low /pm(OOC) /r /melow /ame /dolow");
	SendClientMessage(playerid, 0xCCE6E6FF, "[BANCA]: /bilancio /ritira /deposita /bonifico /ritirastipendio");
	SendClientMessage(playerid, 0xCCE6E6FF, "[HELP]: /cellcmds /casacmds (/v)ehicle /furniturecmds /invcmds /fcmds /vcmds");
	SendClientMessage(playerid, 0xCCE6E6FF, "[HELP]: /radiocmds /businesscmds (/dom)anda /aiutami /aiuto");
	SendClientMessage(playerid, 0xCCE6E6FF, "[TOG/BLOCK]: /tog /tog (f)(hud)(pm)(ooc) /tog all /noads /blockb /blockpm /clearchat");
	if(Account_GetAdminLevel(playerid) > 0)
		SendClientMessage(playerid, COLOR_GREEN, "[TESTER]: /tcmds");
	if(Account_GetAdminLevel(playerid) > 1)
		SendClientMessage(playerid, COLOR_GREEN, "[ADMIN]: /acmds");
	SendClientMessage(playerid, 0xCCE6E6FF, "__________________________________________");
    return 1;
}
alias:aiuto("cmds", "help", "comandi");

flags:chatcmds(CMD_USER);
CMD:chatcmds(playerid, params[])
{
    SendClientMessage(playerid, -1, "[CHAT]: /b - /me - /ame - /low - /melow - /do - /dolow - (/s)hout");
    SendClientMessage(playerid, -1, "[CHAT]: (/w)hisper - (/cw)hisper - /pm - /blockb - /blockpm - /clearchat");
	return 1;
}

flags:vcmds(CMD_ALIVE_USER);
CMD:vcmds(playerid, params[])
{
	SendClientMessage(playerid, -1, "[VEICOLI]: (/v)eicolo - /motore - (/fin)estrino - /vsicura");
	SendClientMessage(playerid, -1, "[VEICOLI]: /vluci - /vparcheggia - /vpark - /vbagagliaio");
	SendClientMessage(playerid, -1, "[VEICOLI]: (/vinv)entario - (/vdis)assembla - (/vdep)osita");
	return 1;
}

flags:invcmds(CMD_ALIVE_USER);
CMD:invcmds(playerid, params[])
{
	SendClientMessage(playerid, -1, "[INVENTARIO]: (/inv)entario - (/dep)ositaarma - (/dis)assembla - /gettaarma");
	SendClientMessage(playerid, -1, "[INVENTARIO]: /invmode - /usa - /passa - /getta - /depositadenaro");
	return 1;
}

flags:hotkeys(CMD_ALIVE_USER);
CMD:hotkeys(playerid, params[])
{
	Player_SetHotKeysEnabled(playerid, !Player_HasHotKeysEnabled(playerid));
    if(Player_HasHotKeysEnabled(playerid))
    {
		SendClientMessage(playerid, COLOR_GREEN, "Hai abilitato le HotKeys.");
		SendClientMessage(playerid, -1, "Ora puoi utilizzare anche:");
		//SendClientMessage(playerid, -1, "{00FF00}ENTER/INVIO{FFFFFF}: Entrare/Uscire da un edificio.");
		SendClientMessage(playerid, -1, "{00FF00}Y{FFFFFF}: entra/esci da un edificio oppure accendere/spegnere un veicolo.");
		SendClientMessage(playerid, -1, "{00FF00}N{FFFFFF}: apri/chiudi un veicolo.");
		SendClientMessage(playerid, -1, "{00FF00}2{FFFFFF}: accendi/spegni fari.");
		SendClientMessage(playerid, -1, "{00FF00}C{FFFFFF}: raccogli un oggetto da terra.");
		SendClientMessage(playerid, COLOR_GREEN, "N.B: I comandi sono relativi al mapping default di GTA.");
		SendClientMessage(playerid, COLOR_GREEN, "Se hai cambiato il mapping default, i tasti potrebbero essere diversi.");
	}
	else
	{
		SendClientMessage(playerid, COLOR_GREEN, "Hai disabilitato le HotKeys. Ora puoi utilizzare soltanto i comandi.");
		SendClientMessage(playerid, -1, "{00FF00}/entra{FFFFFF}: entra/usci da un edificio.");
		SendClientMessage(playerid, -1, "{00FF00}/motore{FFFFFF}: accende/spegne un veicolo.");
		SendClientMessage(playerid, -1, "{00FF00}/vluci{FFFFFF}: accende/spegne i fari di un veicolo.");
		SendClientMessage(playerid, -1, "{00FF00}/raccogli{FFFFFF}: raccogli un oggetto da terra.");
	}
	return 1;
}

flags:id(CMD_ALIVE_USER);
CMD:id(playerid, params[])
{
	new id;
	if(isnull(params))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /id [playerid/partofname]");
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
			SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è connesso.");
	}
	return 1;
}

flags:lasciacarcere(CMD_ALIVE_USER);
CMD:lasciacarcere(playerid, params[])
{
	if(!Character_IsICJailed(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei in carcere.");
	if(Character_GetJailTime(playerid) > 1)
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai ancora scontato la tua pena.");
	
	//if(!Character_IsNearPickupType(playerid, ELEMENT_TYPE_JAIL_EXIT))
		//return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino all'uscita della prigione.");
	
	Character_SetJailTime(playerid, 0);
	Character_SetICJailed(playerid, false);

	SetPlayerPos(playerid, 957.2038, 2401.2576, 10.8203);
	Character_SetVirtualWorld(playerid, 0);
	SetPlayerInterior(playerid, 0);
	SendClientMessage(playerid, -1, "Hai lasciato il carcere.");
	return 1;
}

flags:stilelotta(CMD_ALIVE_USER);
CMD:stilelotta(playerid, params[])
{
	new id;
	if(sscanf(params, "d", id) || id < 0 || id > 3)
	{
		// Show Dialog? nah
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /stilelotta <id (0 - 3)>");
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
	}
	return 1;
}

flags:onduty(CMD_USER);
CMD:onduty(playerid, params[])
{
	new count = 0, factionid = 0;
	foreach(new i : Player)
	{
		factionid = Character_GetFaction(i);
		if(!Character_IsLogged(i) || !Faction_IsValid(factionid) || Faction_GetType(factionid) != FACTION_TYPE_POLICE)
			continue;
		count++;
	}
	if(count == 0)
		SendClientMessage(playerid, COLOR_ERROR, "Non ci sono membri della polizia in servizio.");
	else 
		SendFormattedMessage(playerid, COLOR_ERROR, "Attualmente ci sono %d membri in servizio del Las Venturas Metropolitan Police Department.", count);
	return 1;
}


/*flags:cambiapassword(CMD_USER);
CMD:cambiapassword(playerid, params[])
{
	LoginAttempts{playerid} = 0;
	Dialog_Show(playerid, Dialog_ChangePassword, DIALOG_STYLE_INPUT, "Cambia Password", "Inserisci la tua password attuale.", "Continua", "Chiudi");
	return 1;
}
alias:cambiapassword("cambiapass");

Dialog:Dialog_ChangePassword(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;

	if(LoginAttempts{playerid} >= 3)
		return KickEx(playerid);

	inline OnCheck()
	{
		if(cache_num_rows() > 0)
		{
			Dialog_Show(playerid, Dialog_SetNewPassword, DIALOG_STYLE_INPUT, "Nuova password", "Inserisci la nuova password.", "Continua", "Annulla");
		}
		else
		{
			Dialog_Show(playerid, Dialog_ChangePassword, DIALOG_STYLE_INPUT, "Cambia Password", "{FF0000}La password inserita non è valida.\n{FFFFFF}Inserisci la tua password per continuare.\nTentativo: %d/%d", "Continua", "Annulla", LoginAttempts{playerid}, 3);
			LoginAttempts{playerid}++;
		}
	}

	new 
		psw[WHIRLPOOL_LEN];
	WP_Hash(psw, sizeof(psw), inputtext);
	MySQL_TQueryInline(gMySQL, using inline OnCheck, "SELECT * FROM accounts WHERE id = '%d' AND password = '%e'", Account_GetID(playerid), psw);
	return 1;
}

Dialog:Dialog_SetNewPassword(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;
	if(!IsValidPassword(inputtext))
		return Dialog_Show(playerid, Dialog_SetNewPassword, DIALOG_STYLE_INPUT, "Nuova password", "La password inserita non è corretta.\nInserisci la nuova password.\nLa password deve contenere almeno:\n- 8 caratteri.\n- Almeno una lettera maiuscola.\n- Almeno un numero.\n- Almeno un carattere speciale.\nLa password non puo':\n- Cominciare o finire con uno spazio\n- Contenere come caratteri speciali: %%, \\, ', `, \".", "Continua", "Annulla");
	inline OnUpdate()
	{
		SendClientMessage(playerid, COLOR_GREEN, "Hai cambiato la tua password con successo.");
	}
	new 
		psw[WHIRLPOOL_LEN];
	WP_Hash(psw, sizeof(psw), inputtext);
	MySQL_TQueryInline(gMySQL, using inline OnUpdate, "UPDATE `accounts` SET password = '%e' WHERE id = '%d';", psw, Account_GetID(playerid));
	return 1;
}*/


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