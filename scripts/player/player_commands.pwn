#include <YSI\y_hooks>
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
    pTogglePMAll[playerid] = 0;
    pToggleOOCAll[playerid] = 0;

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
    if(GetTickCount() - pLastAdminQuestionTime[playerid] < 1000 * 30)
        return SendClientMessage(playerid, COLOR_ERROR, "Puoi inviare una domanda ogni 30 secondi!");
    SendMessageToAdmins(0, COLOR_ERROR, "(( [DOMANDA] %s [%d]: %s ))", Character_GetOOCName(playerid), playerid, params);
    SendClientMessage(playerid, -1, "La domanda è stata inviata agli amministratori online. Attendi.");
    pLastAdminQuestionTime[playerid] = GetTickCount();
    return 1;
}

flags:aiuto(CMD_USER);
CMD:aiuto(playerid, params[])
{
    SendClientMessage(playerid, -1, "[GENERALE]: /info - /ame - /dom - /b - /me - /compra - /annulla");
    SendClientMessage(playerid, -1, "[GENERALE]: /rimuovi - ");
    SendClientMessage(playerid, -1, "[VEICOLI]: /motore - /vmenu - /apri - /chiudi - /parcheggia");
    SendClientMessage(playerid, -1, "[INVENTARIO]: (/inv)entario - /dep");
    return 1;
}
alias:aiuto("cmds", "help");

stock GetWeaponAmmoItemID(weapon_id)
{
    #pragma unused weapon_id
    return 90;
}

