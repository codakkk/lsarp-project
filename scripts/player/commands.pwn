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
    SendClientMessage(playerid, -1, "[GENERALE]: /rimuovi - /hotkeys");
    SendClientMessage(playerid, -1, "[CHAT]: /b - /me - /ame - /low - /melow - /do - /dolow - /shout");
    SendClientMessage(playerid, -1, "[CHAT]: (/w)hisper - (/cw)hisper - /pm - /blockb - /blockpm");
    SendClientMessage(playerid, -1, "[VEICOLI]: /motore - /vmenu - /apri - /chiudi - /parcheggia");
    SendClientMessage(playerid, -1, "[INVENTARIO]: (/inv)entario - (/dep)osita - (/dis)assembla - /gettaarma");
	SendClientMessage(playerid, -1, "[INVENTARIO]: /invmode - /usa");
	if(AccountInfo[playerid][aAdmin] > 0)
		SendClientMessage(playerid, COLOR_GREEN, "[SUPPORTER]: /asupportercmds");
	if(AccountInfo[playerid][aAdmin] > 1)
		SendClientMessage(playerid, COLOR_GREEN, "[ADMIN]: /acmds");
    return 1;
}
alias:aiuto("cmds", "help");

flags:hotkeys(CMD_USER);
CMD:hotkeys(playerid, params[])
{
    Bit_Set(gPlayerBitArray[e_pHotKeys], playerid, !Bit_Get(gPlayerBitArray[e_pHotKeys], playerid));
    if(Bit_Get(gPlayerBitArray[e_pHotKeys], playerid))
    {
        SendClientMessage(playerid, COLOR_GREEN, "Hai abilitato le HotKeys.");
        SendClientMessage(playerid, -1, "Ora puoi utilizzare anche:");
        SendClientMessage(playerid, -1, "{00FF00}ENTER/INVIO: Entrare/Uscire da un edificio.");
        SendClientMessage(playerid, -1, "{00FF00}Y: Accendi/Spegni veicolo.");
        SendClientMessage(playerid, -1, "{00FF00}N: Apri/Chiudi veicolo.");
        SendClientMessage(playerid, -1, "{00FF00}2: Accendi/Spegni fari.");
        SendClientMessage(playerid, COLOR_GREEN, "N.B: I comandi sono mappati di default e non possono essere cambiati.");
    }
    else
    {
        SendClientMessage(playerid, COLOR_GREEN, "Hai disabilitato le HotKeys.");
    }
    return 1;
}