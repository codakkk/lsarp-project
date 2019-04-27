#include <YSI_Coding\y_hooks>
#include <player\commands\chat_commands.pwn>
#include <player\commands\premium_commands.pwn>
#include <player\commands\inventory_commands.pwn>
#include <player\commands\vehicle_commands.pwn>
#include <player\commands\interaction_commands.pwn>

// CHANGE ME/AME COLOR

hook OnPlayerClearData(playerid)
{
    printf("player_commands.pwn/OnPlayerClearData");
    if(pVehicleSeller[playerid] != -1 && pVehicleSellingTo[pVehicleSeller[playerid]] == playerid)
    {
	   new seller = pVehicleSeller[playerid];
	   SendClientMessage(seller, COLOR_ERROR, "Il giocatore a cui volevi vendere il veicolo si è disconnesso!");
	   pVehicleSellingTo[seller] = -1;
	   pVehicleSellingPrice[seller] = 0;
	   pSellingVehicleID[seller] = 0;
    }
    else if(pVehicleSellingTo[playerid] != -1 && pVehicleSeller[pVehicleSellingTo[playerid]] == playerid)
    {
	   new customer = pVehicleSellingTo[playerid];
	   SendClientMessage(customer, COLOR_ERROR, "Il giocatore che voleva venderti il veicolo si è disconnesso!");
	   pVehicleSeller[customer] = -1;
	   pVehicleSellingPrice[customer] = 0;
	   pSellingVehicleID[customer] = 0;
    }
    pVehicleSellingPrice[playerid] = 0;
    pSellingVehicleID[playerid] = 0;
    pVehicleSellingTo[playerid] = -1;
    pVehicleSeller[playerid] = -1;

    // before everything, make sure to reset ToggleOOC and PM from players to this playerid
    foreach(new i : Player)
    {
	   if(Iter_Contains(pTogglePM[i], playerid))
		  Iter_Remove(pTogglePM[i], playerid);
	   if(Iter_Contains(pToggleOOC[i], playerid))
		  Iter_Remove(pToggleOOC[i], playerid);
    }
    Iter_Clear(pTogglePM[playerid]);
    Iter_Clear(pToggleOOC[playerid]);
    Bit_Set(gPlayerBitArray[e_pTogglePMAll], playerid, false);
    Bit_Set(gPlayerBitArray[e_pToggleOOCAll], playerid, false);

    return 1;
}

flags:info(CMD_USER);
CMD:info(playerid, params[])
{
    Character_ShowStats(playerid, playerid);
    return 1;
}
alias:info("stats");

flags:dom(CMD_USER);
CMD:dom(playerid, params[])
{
    if(isnull(params))
	   return SendClientMessage(playerid, COLOR_ERROR, "/dom <testo>");
    new seconds = (AccountInfo[playerid][aPremium] > 0) ? 15 : 30;
    if(GetTickCount() - pLastAdminQuestionTime[playerid] < 1000 * seconds)
	   return SendFormattedMessage(playerid, COLOR_ERROR, "Puoi inviare una domanda ogni %d secondi!", seconds);
    if(AccountInfo[playerid][aPremium])
    {
	   SendMessageToAdmins(0, COLOR_GREEN, "(( [PREMIUM] %s (%d) chiede: %s ))", Character_GetOOCName(playerid), playerid, params);
	   SendClientMessage(playerid, -1, "La domanda è stata inviata agli amministratori online. Attendi.");
	   SendClientMessage(playerid, -1, "Essendo un utente Premium, la tua domanda avrà maggiore priorità.");
    }
    else
    {
	   SendMessageToAdmins(0, COLOR_ERROR, "(( %s (%d) chiede: %s ))", Character_GetOOCName(playerid), playerid, params);
	   SendClientMessage(playerid, -1, "La domanda è stata inviata agli amministratori online. Attendi.");
    }
    pLastAdminQuestionTime[playerid] = GetTickCount();
    return 1;
}

flags:aiuto(CMD_USER);
CMD:aiuto(playerid, params[])
{
    SendClientMessage(playerid, -1, "[GENERALE]: /info - /dom - /compra - /annulla");
    SendClientMessage(playerid, -1, "[GENERALE]: /rimuovi - /hotkeys - /arma - /id - /stilecombattimento");
    SendClientMessage(playerid, -1, "[CHAT]: /b - /me - /ame - /low - /melow - /do - /dolow - (/s)hout");
    SendClientMessage(playerid, -1, "[CHAT]: (/w)hisper - (/cw)hisper - /pm - /blockb - /blockpm");
    SendClientMessage(playerid, -1, "[ALTRO]: /animlist - /vehcmds - /invcmds");
	if(Character_GetFaction(playerid) != -1)
	{
		SendClientMessage(playerid, COLOR_SLATEBLUE, "[FAZIONE]: (/f)azione - /togf");
	}
	if(AccountInfo[playerid][aAdmin] > 0)
		SendClientMessage(playerid, COLOR_GREEN, "[SUPPORTER]: /asupportercmds");
	if(AccountInfo[playerid][aAdmin] > 1)
		SendClientMessage(playerid, COLOR_GREEN, "[ADMIN]: /acmds");
    return 1;
}
alias:aiuto("cmds", "help");

flags:vehcmds(CMD_USER);
CMD:vehcmds(playerid, params[])
{
	SendClientMessage(playerid, -1, "[VEICOLI]: /motore - /vmenu - /vapri - /vchiudi - /vparcheggia - /vpark");
	return 1;
}

flags:invcmds(CMD_USER);
CMD:invcmds(playerid, params[])
{
	SendClientMessage(playerid, -1, "[INVENTARIO]: (/inv)entario - (/dep)osita - (/dis)assembla - /gettaarma");
	SendClientMessage(playerid, -1, "[INVENTARIO]: /invmode - /usa - /passa - /getta");
	return 1;
}

flags:hotkeys(CMD_USER);
CMD:hotkeys(playerid, params[])
{
    Bit_Set(gPlayerBitArray[e_pHotKeys], playerid, !Bit_Get(gPlayerBitArray[e_pHotKeys], playerid));
    if(Bit_Get(gPlayerBitArray[e_pHotKeys], playerid))
    {
		SendClientMessage(playerid, COLOR_GREEN, "Hai abilitato le HotKeys.");
		SendClientMessage(playerid, -1, "Ora puoi utilizzare anche:");
		SendClientMessage(playerid, -1, "{00FF00}ENTER/INVIO{FFFFFF}: Entrare/Uscire da un edificio.");
		SendClientMessage(playerid, -1, "{00FF00}Y{FFFFFF}: Accendi/Spegni veicolo.");
		SendClientMessage(playerid, -1, "{00FF00}N{FFFFFF}: Apri/Chiudi veicolo.");
		SendClientMessage(playerid, -1, "{00FF00}2{FFFFFF}: Accendi/Spegni fari.");
		SendClientMessage(playerid, -1, "{00FF00}LALT{FFFFFF}: Gestire la propria casa.");
		SendClientMessage(playerid, COLOR_GREEN, "N.B: I comandi sono relativi al mapping default di GTA.");
		SendClientMessage(playerid, COLOR_GREEN, "Se hai cambiato il mapping default, i tasti potrebbero essere diversi.");
	}
	else
	{
		SendClientMessage(playerid, COLOR_GREEN, "Hai disabilitato le HotKeys.");
	}
	return 1;
}

flags:id(CMD_USER);
CMD:id(playerid, params[])
{
	new id;
	if(sscanf(params, "u", id))
		return SendClientMessage(playerid, COLOR_ERROR, "/id <playerid/partofname>");
	if(!Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è connesso!");
	SendFormattedMessage(playerid, COLOR_GREEN, "%s (%d) - Livello: %d.", Character_GetOOCName(id), id, Character_GetLevel(id));
	return 1;
}

flags:lasciacarcere(CMD_USER);
CMD:lasciacarcere(playerid, params[])
{
	if(!Character_IsJailed(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei in carcere");
	if(Character_GetJailTime(playerid) > 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai ancora scontato la tua pena.");
	new pickupid = Character_GetLastPickup(playerid), id, E_ELEMENT_TYPE:type;
	if(!Pickup_GetInfo(pickupid, id, type) || type != ELEMENT_TYPE_JAIL_EXIT || !IsPlayerInRangeOfPickup(playerid, pickupid, 5.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino all'uscita della prigione!");
	
	Character_SetJailTime(playerid, 0);
	Character_SetICJailed(playerid, 0);
	//Character_Spawn(playerid);
	SetPlayerPos(playerid, 1313.0187, -2063.6877, 57.1440);
	SendClientMessage(playerid, COLOR_GREEN, "Hai lasciato il carcere.");
	return 1;
}

flags:stilecombattimento(CMD_USER);
CMD:stilecombattimento(playerid, params[])
{
	new id;
	if(sscanf(params, "d", id) || id < 0 || id > 4)
	{
		// Show Dialog? nah
		return SendClientMessage(playerid, COLOR_ERROR, "/stilecombattimento <id (0 - 4)");
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
				Character_SetFightingStyle(playerid, FIGHT_STYLE_KNEEHEAD);
			}
			case 4:
			{
				Character_SetFightingStyle(playerid, FIGHT_STYLE_GRABKICK);
			}
		}
		SendClientMessage(playerid, COLOR_GREEN, "Hai cambiato il tuo stile di combattimento.");
	}
	return 1;
}