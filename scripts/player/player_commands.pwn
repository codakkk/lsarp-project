#include <YSI\y_hooks>
#include <player\commands\chat_commands.pwn>
#include <player\commands\premium_commands.pwn>
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

flags:v(CMD_USER);
CMD:v(playerid, params[])
{    
    if(gBuyingVehicle[playerid])
        return 1;
    new text[32], slot, anotherId, anotherParam;
    if(sscanf(params, "s[32]I(-1)U(-1)I(-1)", text, slot, anotherId, anotherParam))
    {
        return SendClientMessage(playerid, COLOR_ERROR, "> /v <list - park - chiudi - apri - vendi - bagagliaio> <slot (facoltativo)>");
    }
    new vehicleid = GetVehicleIDBySlot(playerid, slot);
    if(slot != -1 && vehicleid <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Slot non valido!");
    if(!strcmp(params, "list", true))
    {
        new count = 0, currentVeh = GetPlayerVehicleID(playerid);
        SendClientMessage(playerid, COLOR_GREEN, "|__________[I MIEI VEICOLI]__________|");
        foreach(new v : Vehicles)
        {
            if(VehicleInfo[v][vOwnerID] == PlayerInfo[playerid][pID])
            {   new Float:x, Float:y, Float:z;
                GetVehiclePos(v, x , y, z);
                if(v == currentVeh)
                    SendFormattedMessage(playerid, COLOR_GREEN, "[%d] %s (Attuale)", count, GetVehicleName(v));
                else if(IsPlayerInRangeOfPoint(playerid, 5.0, x, y, z))
                    SendFormattedMessage(playerid, COLOR_GREEN, "[%d] %s (Vicino)", count, GetVehicleName(v));
                else
                    SendFormattedMessage(playerid, COLOR_GREEN, "[%d] %s ", count, GetVehicleName(v));
                count++;
            }
        }
        if(count == 0)
            SendClientMessage(playerid, COLOR_GREEN, "> Non possiedi veicoli!");
        SendClientMessage(playerid, COLOR_GREEN, "|____________________________________|");
        return 1;
    }
    else if(!strcmp(params, "park", true))
    {
        vehicleid = GetPlayerVehicleID(playerid);
        if(vehicleid == 0 || VehicleInfo[vehicleid][vOwnerID] != PlayerInfo[playerid][pID])
            return SendClientMessage(playerid, COLOR_ERROR, "Devi essere su un tuo veicolo per poter utilizzare questo comando!");
        new Float:x, Float:y, Float:z, Float:a;
        GetVehiclePos(vehicleid, x, y, z);
        GetVehicleZAngle(vehicleid, a);
        Vehicle_Park(vehicleid, x, y, z, a);
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai parcheggiato qui la tua %s.", GetVehicleName(vehicleid));
        return 1;
    }
    else if(!strcmp(text, "chiudi", true))
    {
        if(slot != -1)
        {
            new idstr[8];
            valstr(idstr, vehicleid);
            pc_cmd_chiudi(playerid, idstr);
        }
        else
        {
            pc_cmd_chiudi(playerid, NULL);
        }
            
    }
    else if(!strcmp(text, "apri", true))
    {
        if(slot != -1)
        {
            new idstr[8];
            valstr(idstr, vehicleid);
            pc_cmd_apri(playerid, idstr);
        }
        else
        {
            pc_cmd_apri(playerid, NULL);
        }
    }
    else if(!strcmp(text, "bagagliaio", true))
    {
        if(slot != -1)
        {
            new idstr[8];
            valstr(idstr, vehicleid);
            pc_cmd_bagagliaio(playerid, idstr);
        }
        else
        {
            pc_cmd_bagagliaio(playerid, NULL);
        }
    }
    else if(!strcmp(text, "vendi", true)) // misses isinrange checks for vehicle and players.
    {
        if(anotherId == -1)
            return SendClientMessage(playerid, COLOR_ERROR, "/v vendi <slotid> <playerid/partofname> <prezzo>");
        if(!IsPlayerConnected(anotherId) || !gCharacterLogged[anotherId] || anotherId == playerid)
            return SendClientMessage(playerid, COLOR_ERROR, "Giocatore non valido o non connesso!");
        if(anotherParam <= 0)
            return SendClientMessage(playerid, COLOR_ERROR, "Prezzo non valido!");
        if(!IsPlayerInRangeOfPlayer(playerid, anotherId, 6.0))
            return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore!");

        pVehicleSellingTo[playerid] = anotherId;
        pVehicleSeller[anotherId] = playerid;
        pSellingVehicleID[playerid] = pSellingVehicleID[anotherId] = vehicleid;
        pVehicleSellingPrice[playerid] = pVehicleSellingPrice[anotherId] = anotherParam;

        SendFormattedMessage(playerid, COLOR_GREEN, "Hai proposto a %s (%d) il tuo veicolo (%s) per $%d.", Character_GetOOCName(anotherId), anotherId, GetVehicleName(vehicleid), anotherParam);
        SendFormattedMessage(anotherId, COLOR_GREEN, "%s (%d) vuole venderti il suo veicolo (%s) per $%d. Digita /accetta veicolo per accettare.", Character_GetOOCName(playerid), playerid, GetVehicleName(vehicleid), anotherParam);
    }
    else
    {
        return SendClientMessage(playerid, COLOR_ERROR, "> /v <list - park - chiudi - apri - vendi - bagagliaio> <slot (facoltativo)>");
    }
    return 1;
}
alias:v("vehicle", "veicolo");

flags:chiudi(CMD_USER);
CMD:chiudi(playerid, params[])
{
    new vehicleid;
    if(sscanf(params, "d", vehicleid))
    {
        vehicleid = GetClosestVehicle(playerid, 3.5);
    }
    else
    {
        if(!IsPlayerInRangeOfVehicle(playerid, vehicleid, 3.5))
            return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino a questo veicolo!");
    }
    if(vehicleid == 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non ci sono veicoli nelle vicinanze!");
    if(VehicleInfo[vehicleid][vOwnerID] != PlayerInfo[playerid][pID])
        return SendClientMessage(playerid, COLOR_ERROR, "Questo veicolo non è tuo!");
    if(VehicleInfo[vehicleid][vLocked])
        return SendClientMessage(playerid, -1, "Il veicolo è già chiuso!");
    if((IsABike(vehicleid) || IsAMotorBike(vehicleid)) && Vehicle_IsEngineOn(vehicleid))
    {
        return SendClientMessage(playerid, -1, "Prima spegni il motore!");
    }
    VehicleInfo[vehicleid][vLocked] = 1;
    Vehicle_UpdateLockState(vehicleid);
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai chiuso il tuo veicolo (%s).", GetVehicleName(vehicleid));
    if(IsABike(vehicleid) || IsAMotorBike(vehicleid))
    {
        ApplyAnimation(playerid, "BOMBER", "BOM_Plant_Crouch_In", 4.1, 0, 0, 0, 0, 1000, 0);
        Character_AMe(playerid, "prende la catena e l'attacca al veicolo");
    }
    else
    {
        Character_AMe(playerid, "prende le chiavi e chiude il suo veicolo");
    }
    return 1;
}

flags:apri(CMD_USER);
CMD:apri(playerid, params[])
{
    new vehicleid;
    if(sscanf(params, "d", vehicleid))
    {
        vehicleid = GetClosestVehicle(playerid, 3.5);
    }
    else
    {
        new Float:x, Float:y, Float:z;
        GetVehiclePos(vehicleid, x, y, z);
        if(!IsPlayerInRangeOfPoint(playerid, 3.5, x, y, z))
            return SendClientMessage(playerid, COLOR_ERROR, "> Non sei vicino a questo veicolo!");
    }
    if(vehicleid == 0)
        return SendClientMessage(playerid, COLOR_ERROR, "> Non ci sono veicoli nelle vicinanze!");
    if(VehicleInfo[vehicleid][vOwnerID] != PlayerInfo[playerid][pID])
        return SendClientMessage(playerid, COLOR_ERROR, "> Questo veicolo non è tuo!");
    if(!VehicleInfo[vehicleid][vLocked])
        return SendClientMessage(playerid, -1, "> Il veicolo è già aperto!");
    VehicleInfo[vehicleid][vLocked] = 0;
    Vehicle_UpdateLockState(vehicleid);
    SendFormattedMessage(playerid, COLOR_GREEN, "> Hai aperto il tuo veicolo (%s).", GetVehicleName(vehicleid));
    if(IsABike(vehicleid) || IsAMotorBike(vehicleid))
    {
        ApplyAnimation(playerid, "BOMBER", "BOM_Plant_Crouch_In", 4.1, 0, 0, 0, 0, 1000, 0);
        Character_AMe(playerid, "toglie la catena dal suo veicolo");
    }
    else
    {
        Character_AMe(playerid, "prende le chiavi e apre il suo veicolo");
    }
    return 1;
}

flags:bagagliaio(CMD_USER);
CMD:bagagliaio(playerid, params[])
{
    new vehicleid;
    if(sscanf(params, "d", vehicleid))
    {
        vehicleid = GetClosestVehicle(playerid, 3.5);
    }
    else
    {
        if(!IsPlayerInRangeOfVehicle(playerid, vehicleid, 3.5))
            return SendClientMessage(playerid, COLOR_GREEN, "Non sei vicino al veicolo!");
    }
    if(Vehicle_IsTrunkOpened(vehicleid))
        Vehicle_CloseTrunk(vehicleid);
    else
        Vehicle_OpenTrunk(vehicleid);
    return 1;
}

flags:motore(CMD_USER);
CMD:motore(playerid, params[])
{
    new vehicleid = GetPlayerVehicleID(playerid);
    if(vehicleid == 0 || GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo!");
    if(Vehicle_IsEngineOn(vehicleid))
    {
        Vehicle_SetEngineOff(vehicleid);
        Character_AMe(playerid, "gira la chiave e spegne il motore");
    }
    else
    {
        if(VehicleInfo[vehicleid][vOwnerID] == PlayerInfo[playerid][pID])
        {
            if((IsABike(vehicleid) || IsAMotorBike(vehicleid)) && VehicleInfo[vehicleid][vLocked])
            {
                return SendClientMessage(playerid, COLOR_ERROR, "Il veicolo ha la catena!");
            }
            Vehicle_SetEngineOn(vehicleid);
            Character_AMe(playerid, "gira la chiave e accende il motore");
        }
        else
        {
            SendClientMessage(playerid, COLOR_ERROR, "Non hai le chiavi di questo veicolo!");
        }
    }
    return 1;
}

flags:entra(CMD_USER);
CMD:entra(playerid, params[])
{
    new pickupid = pLastPickup[playerid], id, E_ELEMENT_TYPE:type;
    if(pickupid != -1)
    {
        Pickup_GetInfo(pickupid, id, type);
        if(type != ELEMENT_TYPE_BUILDING_ENTRANCE || !Player_Enter(playerid, pickupid, id, type))
            return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrare di un edificio!");
    } else return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrare di un edificio!");
    return 1;
}

flags:esci(CMD_USER);
CMD:esci(playerid, params[])
{
    new pickupid = pLastPickup[playerid], id, E_ELEMENT_TYPE:type;
    if(pickupid != -1)
    {
        Pickup_GetInfo(pickupid, id, type);
        if(type != ELEMENT_TYPE_BUILDING_ENTRANCE || !Player_Exit(playerid, pickupid, id, type))
            return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'uscita di un edificio!");
    }
    else return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'uscita di un edificio!");
    return 1;
}

flags:dai(CMD_USER);
CMD:dai(playerid, params[])
{
    new
        id,
        text[128];
    if(sscanf(params, "us[128]", id, text))
    {
        SendClientMessage(playerid, COLOR_ERROR, "/dai <playerid/partofname> <oggetto>");
        SendClientMessage(playerid, COLOR_ERROR, "Oggetti: chiave");
        return 1;
    }
    
    if(id < 0 || id >= MAX_PLAYERS || !gCharacterLogged[id])
        return SendClientMessage(playerid, COLOR_ERROR, "Giocatore non connesso!");
    
    if(!ProxDetectorS(5.0, playerid, id))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore!");
    
    /*if(!strcmp(text, "chiave", true))
    {
        new vehicleid = GetPlayerVehicleID(playerid);
        if(vehicleid == 0 || VehicleInfo[vehicleid][vOwnerID] != PlayerInfo[playerid][pID]) 
            return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un tuo veicolo.");
        if(pGiveRequest[id] != -1)
            return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore ha già una richiesta attiva.");
        
        SendFormattedMessage(id, COLOR_GREEN, "%s [%d] vuole darti la chiave del suo veicolo (%s).", Character_GetOOCName(playerid), playerid, GetVehicleName(vehicleid));
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai proposto a %s [%d] la chiave del tuo veicolo (%s).", Character_GetOOCName(id), id, GetVehicleName(vehicleid));

        
    }*/
    return 1;
}

flags:accetta(CMD_USER);
CMD:accetta(playerid, params[])
{
    new
        text[128];
    if(sscanf(params, "s[128]", text))
    {
        SendClientMessage(playerid, COLOR_ERROR, "/accetta <oggetto>");
        SendClientMessage(playerid, COLOR_ERROR, "Oggetti: chiave, veicolo");
        return 1;
    }
    if(!strcmp(text, "veicolo", true))
    {
        // I must check if seller disconnected (must clear data too)
        if(! (pVehicleSeller[playerid] != -1 && pVehicleSellingTo[pVehicleSeller[playerid]] == playerid))
            return SendClientMessage(playerid, COLOR_ERROR, "Il venditore si è disconnesso o non esiste!");
        if(AC_GetPlayerMoney(playerid) < pVehicleSellingPrice[playerid])
            return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi!");
        if(!IsPlayerInRangeOfPlayer(playerid, pVehicleSeller[playerid], 10.0))
            return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore!");

        SendFormattedMessage(pVehicleSeller[playerid], COLOR_GREEN, "%s (%d) ha accettato il tuo veicolo per $%d.", Character_GetOOCName(playerid), playerid, pVehicleSellingPrice[playerid]);
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai comprato il veicolo di %s (%d) per $%d.", Character_GetOOCName(pVehicleSeller[playerid]), pVehicleSeller[playerid], pVehicleSellingPrice[playerid]);

        AC_GivePlayerMoney(playerid, -pVehicleSellingPrice[playerid]);
        AC_GivePlayerMoney(pVehicleSeller[playerid], pVehicleSellingPrice[playerid]);

        new 
            vehicleid = pSellingVehicleID[playerid],
            sellerid = pVehicleSeller[playerid];

        new query[256];
        mysql_format(gMySQL, query, sizeof(query), "UPDATE `player_vehicles` SET OwnerID = '%d' WHERE ID = '%d'", PlayerInfo[playerid][pID], VehicleInfo[vehicleid][vID]);
        mysql_tquery(gMySQL, query);

        VehicleInfo[vehicleid][vOwnerID] = PlayerInfo[playerid][pID];
        set(VehicleInfo[vehicleid][vOwnerName], PlayerInfo[playerid][pName]);
        gVehicleDestroyTime[vehicleid] = -1;

        pVehicleSellingPrice[playerid] = pVehicleSellingPrice[sellerid] = 0;
        pSellingVehicleID[playerid] = pSellingVehicleID[sellerid] = 0;
        pVehicleSellingTo[sellerid] = -1;
        pVehicleSeller[playerid] = -1;
        return 1;
    }
    /*if(!strcmp(text, "chiave", true))
    {
        new 
            reqPlayer = GetPVarInt(playerid, "GiveRequest_Player"),
            vehicleKey = GetPVarInt(playerid, "GiveRequest_VehicleKey");
        if(reqPlayer == 0 && vehicleKey == 0)
            return 
        
    }*/
    return 1;
}

flags:getta(CMD_USER);
CMD:getta(playerid, params[])
{
    new
        text[128];
    if(sscanf(params, "s[128]", text))
    {
        SendClientMessage(playerid, COLOR_ERROR, "/getta <oggetto>");
        SendClientMessage(playerid, COLOR_ERROR, "Oggetti: chiave");
        return 1;
    }
    if(!strcmp(text, "chiave", true))
    {
        if(!pVehicleKey[playerid])
            return SendClientMessage(playerid, COLOR_ERROR, "Non hai chiavi da gettare!");
        pVehicleKey[playerid] = 0;
        SendClientMessage(playerid, COLOR_GREEN, "Hai gettato le chiavi!");
    }
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
    SendClientMessage(playerid, -1, "[ALTRO]: /info - /ame - /dom - /b - /me");
    SendClientMessage(playerid, -1, "[VEICOLI]: /motore - /v - /apri - /chiudi");
    SendClientMessage(playerid, -1, "[ALTRO]: /compra - /annulla");
    return 1;
}
alias:aiuto("cmds", "help");

flags:inventario(CMD_USER);
CMD:inventario(playerid, params[])
{
    Character_ShowInventory(playerid, playerid);
    return 1;
}
alias:inventario("inv");

flags:compra(CMD_USER);
CMD:compra(playerid, params[])
{
    if(gBuyingVehicle[playerid])
    {
        return ShowRoom_PlayerConfirmBuy(playerid);
    }
    if(pLastPickup[playerid] == -1)
        return 0;
    new
        eID,
        E_ELEMENT_TYPE:eType,
        Float:x, Float:y, Float:z;

    Pickup_GetInfo(pLastPickup[playerid], eID, eType);
    Pickup_GetPosition(pLastPickup[playerid], x, y, z);
    if(eType == ELEMENT_TYPE_DEALERSHIP && IsPlayerInRangeOfPoint(playerid, 2.5, x, y, z))
    {
        ShowRoom_ShowVehiclesToPlayer(eID, playerid);
    }
    else if(eType == ELEMENT_TYPE_BUILDING_ENTRANCE)
    {
        
    }
    return 1;
}

flags:annulla(CMD_USER);
CMD:annulla(playerid, params[])
{
    if(gBuyingVehicle[playerid])
    {
        return ShowRoom_PlayerCancelBuy(playerid);
    }
    return 1;
}

flags:rimuovi(CMD_USER);
CMD:rimuovi(playerid, params[])
{
    new
        text[128];
    if(sscanf(params, "s[128]", text))
    {
        SendClientMessage(playerid, COLOR_ERROR, "/rimuovi <oggetto>");
        SendClientMessage(playerid, COLOR_ERROR, "Oggetti: zaino");
        return 1;
    }
    if(!strcmp(text, "zaino", true))
    {
        if(!Character_HasBag(playerid))
            return SendClientMessage(playerid, COLOR_ERROR, "Non stai indossando uno zaino!");
        if(Character_GetInventoryUsedSpace(playerid) > PLAYER_INVENTORY_START_SIZE-1)
            return SendClientMessage(playerid, COLOR_ERROR, "Non puoi toglierti lo zaino se hai piu' di 9 oggetti!");
        Character_GiveItem(playerid, pInventoryBag[playerid], 1);
        pInventoryBag[playerid] = 0;
        SendClientMessage(playerid, COLOR_GREEN, "Lo zaino è stato rimesso nel tuo inventario!");
        Character_AMe(playerid, "si toglie lo zaino");
        return 1;
    }
    return 1;
}

stock GetWeaponAmmoItemID(weapon_id)
{
    #pragma unused weapon_id
    return 90;
}

flags:dep(CMD_USER);
CMD:dep(playerid, params[])
{
    new 
        itemid = GetPlayerWeapon(playerid),
        ammos = GetPlayerAmmo(playerid),
        ammoItemId = GetWeaponAmmoItemID(itemid);
    if(itemid != 0)
    {
        if(!Character_HasSpaceForItem(playerid, itemid, 1) || (ammos > 0 && !Character_HasSpaceForItem(playerid, ammoItemId, ammos)))
        {
            return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario!");
        }
        Character_GiveItem(playerid, itemid, 1);
        Character_GiveItem(playerid, ammoItemId, ammos);
        AC_ResetPlayerWeapons(playerid);
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai depositato la tua arma (%s). Munizioni: %d", ServerItem_GetName(itemid), ammos);
    }
    else
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando senza un'arma!");
    }
    return 1;
}

stock GetVehicleIDBySlot(playerid, slotid)
{
    if(slotid == -1)
        return 0;
    new count = 0;
    foreach(new v : Vehicles)
    {
        if(VehicleInfo[v][vOwnerID] == PlayerInfo[playerid][pID])
        {
            if(count == slotid)
                return v;
            count++;
        }
    }
    return 0;
}

stock GetClosestVehicle(playerid, Float:radius)
{
    if(IsPlayerInAnyVehicle(playerid))
        return GetPlayerVehicleID(playerid);
    
    new 
        Float:x, Float:y, Float:z;
    foreach(new i : Vehicles)
    {
        GetVehiclePos(i, x, y, z);
        if(IsPlayerInRangeOfPoint(playerid, radius, x, y, z))
        {
            return i;
        }
    }
    return 0;
}