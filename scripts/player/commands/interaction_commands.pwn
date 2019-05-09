#include <YSI_Coding\y_hooks>

flags:ritirastipendio(CMD_ALIVE_USER);
CMD:ritirastipendio(playerid, params[])
{
	new pickupid = Character_GetLastPickup(playerid), id, E_ELEMENT_TYPE:eType;
	if(!Pickup_GetInfo(pickupid, id, eType) || eType != ELEMENT_TYPE_PAYCHECK)
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei al punto esatto per ritirare lo stipendio!");
	new paycheck = Character_GetPayCheck(playerid);
	if(paycheck <= 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai uno stipendio da ritirare!");
	
	Character_GiveMoney(playerid, paycheck, "paycheck");
	Character_SetPayCheck(playerid, 0);
	Character_Save(playerid);
	SendFormattedMessage(playerid, -1, "Hai ritirato il tuo stipendio! Ammontare: {85bb65}$%d{FFFFFF}", paycheck);
	return 1;
}

flags:entra(CMD_ALIVE_USER);
CMD:entra(playerid, params[])
{
	new pickupid = Character_GetLastPickup(playerid), id, E_ELEMENT_TYPE:type;
    if(Pickup_GetInfo(pickupid, id, type))
    	Character_Enter(playerid, pickupid, id, type);
    else 
		SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata di un edificio!");
    return 1;
}

flags:esci(CMD_ALIVE_USER);
CMD:esci(playerid, params[])
{
	new pickupid = Character_GetLastPickup(playerid), id, E_ELEMENT_TYPE:type;
    if(Pickup_GetInfo(pickupid, id, type))
    	Character_Exit(playerid, pickupid, id, type);
    else
		SendClientMessage(playerid, COLOR_ERROR, "Non sei all'uscita di un edificio!");
    return 1;
}

flags:dai(CMD_ALIVE_USER);
CMD:dai(playerid, params[])
{
	if(PendingRequestInfo[playerid][rdPending])
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta attiva!");
    new
        id,
        text[128];
    if(sscanf(params, "k<u>s[128]", id, text))
    {
        SendClientMessage(playerid, COLOR_ERROR, "/dai <playerid/partofname> <oggetto>");
        SendClientMessage(playerid, COLOR_ERROR, "Oggetti: arma");
        return 1;
    }
    
    if(id < 0 || id >= MAX_PLAYERS || !Character_IsLogged(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Giocatore non connesso!");
    
	if(id == playerid)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi usare questo comando su te stesso!");

    if(!IsPlayerInRangeOfPlayer(playerid, id, 3.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore!");
    
	if(PendingRequestInfo[id][rdPending])
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore ha già una richiesta attiva!");
	
	if(!Character_IsAlive(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su questo giocatore.");

	if(!strcmp(text, "arma", true))
	{
		new weapon = GetPlayerWeapon(playerid);
		new ammo = GetPlayerAmmo(playerid);
		if(weapon == 0 || ammo <= 0)
			return SendClientMessage(playerid, COLOR_ERROR, "Non hai un'arma in mano!");
		SendFormattedMessage(playerid, COLOR_GREEN, "Hai proposto di dare l'arma (%s) con %d proiettili a %s.", Weapon_GetName(weapon), ammo, Character_GetRolePlayName(id));
		SendClientMessage(playerid, -1, "Digita '{FF0000}/annulla arma{FFFFFF}'' per annullare.");
		SendFormattedMessage(id, COLOR_GREEN, "%s vuole darti un'arma (%s) con %d proiettili.", Character_GetRolePlayName(playerid), Weapon_GetName(weapon), ammo);
		SendClientMessage(id, -1, "Digita '{00FF00}/accetta arma{FFFFFF}' per accettare.");

		PendingRequestInfo[playerid][rdPending] = 1;
		PendingRequestInfo[playerid][rdToPlayer] = id;
		PendingRequestInfo[playerid][rdTime] = GetTickCount();
		PendingRequestInfo[playerid][rdItem] = weapon;
		PendingRequestInfo[playerid][rdSlot] = Weapon_GetSlot(weapon);
		PendingRequestInfo[playerid][rdAmount] = 1;
		PendingRequestInfo[playerid][rdType] = PENDING_TYPE_WEAPON;
		
		PendingRequestInfo[id][rdPending] = 1;
		PendingRequestInfo[id][rdByPlayer] = playerid;
		PendingRequestInfo[id][rdTime] = GetTickCount();
		PendingRequestInfo[id][rdItem] = weapon;
		PendingRequestInfo[id][rdSlot] = Weapon_GetSlot(weapon);
		PendingRequestInfo[id][rdAmount] = 1;
		PendingRequestInfo[id][rdType] = PENDING_TYPE_WEAPON;
	}
	else
	{
		SendClientMessage(playerid, COLOR_ERROR, "Oggetti: arma");
	}
    /*if(!strcmp(text, "chiave", true))
    {
        new vehicleid = GetPlayerVehicleID(playerid);
        if(vehicleid == 0 || VehicleInfo[vehicleid][vOwnerID] != PlayerInfo[playerid][pID]) 
            return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un tuo veicolo.");
        if(pGiveRequest[id] != -1)
            return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore ha già una richiesta attiva.");
        
        SendFormattedMessage(id, COLOR_GREEN, "%s [%d] vuole darti la chiave del suo veicolo (%s).", Character_GetOOCName(playerid), playerid, Vehicle_GetName(vehicleid));
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai proposto a %s [%d] la chiave del tuo veicolo (%s).", Character_GetOOCName(id), id, Vehicle_GetName(vehicleid));

        
    }*/
    return 1;
}

flags:paga(CMD_ALIVE_USER);
CMD:paga(playerid, params[])
{
	new id, amount;
	if(sscanf(params, "k<u>d", id, amount))
		return SendClientMessage(playerid, COLOR_ERROR, "/paga <playerid/partofname> <ammontare>");
	if(id == playerid || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");
	if(amount < 0 || amount > Character_GetMoney(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai tutti questi soldi!");
	if(!IsPlayerInRangeOfPlayer(playerid, id, 2.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore!");
	if(!Character_IsAlive(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su questo giocatore.");
	Character_GiveMoney(playerid, -amount);
	Character_GiveMoney(id, amount);
	Character_AMe(playerid, "prende dei soldi e li da a %s", Character_GetRolePlayName(id));
	SendFormattedMessage(id, COLOR_GREEN, "%s ti ha dato $%d.", Character_GetRolePlayName(playerid), amount);
	SendFormattedMessage(playerid, COLOR_GREEN, "Hai dato $%d a %s.", amount, Character_GetRolePlayName(id));
	return 1;
}

flags:accetta(CMD_USER);
CMD:accetta(playerid, params[])
{
    if(isnull(params) || strlen(params) > 30)
    {
        SendClientMessage(playerid, COLOR_ERROR, "/accetta <oggetto>");
        SendClientMessage(playerid, COLOR_ERROR, "Oggetti: arma, oggetto, morte, veicolo");
        return 1;
    }
	if( strcmp(params, "morte", true) && strcmp(params, "cure", true) && !Character_IsAlive(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
	if(!strcmp(params, "arma", true))
	{
		if(!PendingRequestInfo[playerid][rdPending] || PendingRequestInfo[playerid][rdType] != PENDING_TYPE_WEAPON)
			return SendClientMessage(playerid, COLOR_ERROR, "Non hai una richiesta d'arma attiva.");
		if(!IsPlayerInRangeOfPlayer(playerid, PendingRequestInfo[playerid][rdByPlayer], 5.0))
			return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore!");
		new slotid = PendingRequestInfo[playerid][rdSlot],
			weaponid = PendingRequestInfo[playerid][rdItem],
			tempWeapon = 0,
			ammo = 0,
			requestSender = PendingRequestInfo[playerid][rdByPlayer];
		GetPlayerWeaponData(requestSender, slotid, tempWeapon, ammo);
		if(weaponid != tempWeapon || ammo == 0)
		{
			ResetPendingRequest(playerid);
			ResetPendingRequest(requestSender);
			SendClientMessage(playerid, COLOR_ERROR, "Non possiedi più l'arma. La richiesta è stata annullata.");
			return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non possiede più l'arma, pertanto non puoi più accettare.");	
		}
		if(ACInfo[playerid][acWeapons][Weapon_GetSlot(weaponid)] == 0)
		{
			AC_RemovePlayerWeapon(requestSender, weaponid);
			AC_GivePlayerWeapon(playerid, weaponid, ammo);
			SendFormattedMessage(playerid, COLOR_GREEN, "Hai accettato l'arma (%s) con %d proiettili da %s.", Weapon_GetName(weaponid), ammo, Character_GetOOCName(requestSender));
			
			ResetPendingRequest(playerid);
			ResetPendingRequest(requestSender);
		}
		else if(Character_HasSpaceForItem(playerid, weaponid, ammo))
		{
			AC_RemovePlayerWeapon(requestSender, weaponid);
			Character_GiveItem(playerid, weaponid, 1, ammo);
			SendFormattedMessage(playerid, COLOR_GREEN, "Hai accettato l'arma (%s) con %d proiettili da %s.", Weapon_GetName(weaponid), ammo, Character_GetOOCName(requestSender));
			SendClientMessage(playerid, COLOR_GREEN, "L'arma è stata messa nell'inventario poiché lo slot è occupato.");

			ResetPendingRequest(playerid);
			ResetPendingRequest(requestSender);
		}
		else
			SendClientMessage(playerid, COLOR_ERROR, "Hai già un'arma e non hai abbastanza spazio nell'inventario.");

	}
	else if(!strcmp(params, "morte", true))
	{
		Character_AcceptDeathState(playerid);
	}
    else if(!strcmp(params, "veicolo", true))
    {
        // I must check if seller disconnected (must clear data too)
        if(! (pVehicleSeller[playerid] != -1 && pVehicleSellingTo[pVehicleSeller[playerid]] == playerid))
            return SendClientMessage(playerid, COLOR_ERROR, "Il venditore si è disconnesso o non esiste!");
        if(Character_GetMoney(playerid) < pVehicleSellingPrice[playerid])
            return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi!");
        if(!IsPlayerInRangeOfPlayer(playerid, pVehicleSeller[playerid], 10.0))
            return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore!");

        SendFormattedMessage(pVehicleSeller[playerid], COLOR_GREEN, "%s ha accettato il tuo veicolo per $%d.", Character_GetOOCName(playerid), pVehicleSellingPrice[playerid]);
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai comprato il veicolo di %s per $%d.", Character_GetOOCName(pVehicleSeller[playerid]), pVehicleSellingPrice[playerid]);

        Character_GiveMoney(playerid, -pVehicleSellingPrice[playerid]);
        Character_GiveMoney(pVehicleSeller[playerid], pVehicleSellingPrice[playerid]);

        new 
            vehicleid = pSellingVehicleID[playerid],
            sellerid = pVehicleSeller[playerid];

        new query[256];
        mysql_format(gMySQL, query, sizeof(query), "UPDATE `vehicles` SET OwnerID = '%d' WHERE ID = '%d'", PlayerInfo[playerid][pID], VehicleInfo[vehicleid][vID]);
        mysql_tquery(gMySQL, query);

        VehicleInfo[vehicleid][vOwnerID] = PlayerInfo[playerid][pID];
        set(VehicleInfo[vehicleid][vOwnerName], PlayerInfo[playerid][pName]);

        pVehicleSellingPrice[playerid] = pVehicleSellingPrice[sellerid] = 0;
        pSellingVehicleID[playerid] = pSellingVehicleID[sellerid] = 0;
        pVehicleSellingTo[sellerid] = -1;
        pVehicleSeller[playerid] = -1;
        return 1;
    }
	else if(!strcmp(params, "cure", true))
    {
		Character_AcceptCare(playerid);
		//Log(Character_GetOOCName(playerid), Character_GetOOCName(senderid), "/accetta cure", senderid);
    }
	else if(!strcmp(params, "oggetto", true))
	{
		Character_AcceptItemRequest(playerid);
	}
	else
		return SendClientMessage(playerid, COLOR_ERROR, "Oggetti: arma, oggetto, morte, veicolo");
    return 1;
}

CMD:annulla(playerid, params[])
{
	if(gBuyingVehicle[playerid])
    {
        return Dealership_PlayerCancelBuy(playerid);
    }
	if(!PendingRequestInfo[playerid][rdPending])
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai una richiesta attiva!");
	new toPlayer = PendingRequestInfo[playerid][rdToPlayer];
	if(PendingRequestInfo[toPlayer][rdPending] && PendingRequestInfo[toPlayer][rdByPlayer] == playerid)
	{
		SendFormattedMessage(toPlayer, COLOR_ERROR, "%s ha annullato la richiesta.", Character_GetOOCName(playerid));
		ResetPendingRequest(toPlayer);
	}
	SendClientMessage(playerid, COLOR_GREEN, "Hai annullato la richiesta attiva!");
	ResetPendingRequest(playerid);
	return 1;
}

flags:compra(CMD_ALIVE_USER);
CMD:compra(playerid, params[])
{
	if(gBuyingVehicle[playerid])
	{
		return Dealership_PlayerConfirmBuy(playerid);
	}
	
	new
		pickupid = Character_GetLastPickup(playerid),
		eID,
		E_ELEMENT_TYPE:eType;
	
	if(Pickup_GetInfo(pickupid, eID, eType) && IsPlayerInRangeOfPickup(playerid, pickupid, 3.0))
	{
		if(eType == ELEMENT_TYPE_DEALERSHIP)
		{
			return Dealership_ShowVehiclesToPlayer(eID, playerid);
		}
		else if(eType == ELEMENT_TYPE_BUILDING_ENTRANCE && Building_IsValid(eID))
		{
			return Character_BuyBuilding(playerid, eID);
		}
		else if(eType == ELEMENT_TYPE_HOUSE_ENTRANCE && House_IsValid(eID))
		{
			return Character_BuyHouse(playerid, eID);
		}
	}
    return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando qui!");
}

flags:rimuovi(CMD_ALIVE_USER);
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
        new Inventory:playerInv = Character_GetInventory(playerid);
        if(!Character_HasBag(playerid))
            return SendClientMessage(playerid, COLOR_ERROR, "Non stai indossando uno zaino!");
        if(Inventory_GetUsedSpace(playerInv) > PLAYER_INVENTORY_START_SIZE-1)
            return SendClientMessage(playerid, COLOR_ERROR, "Non puoi toglierti lo zaino se hai piu' di 9 oggetti!");
        Character_GiveItem(playerid, pInventoryBag[playerid], 1);
        Character_SetBag(playerid, 0);
        SendClientMessage(playerid, COLOR_GREEN, "Lo zaino è stato rimesso nel tuo inventario!");
        Character_AMe(playerid, "si toglie lo zaino");
        return 1;
    }
    return 1;
}

CMD:arma(playerid, params[])
{
	if(!IsPlayerInAnyVehicle(playerid) || GetPlayerState(playerid) != PLAYER_STATE_PASSENGER)
		return SendClientMessage(playerid, COLOR_ERROR, "Devi essere passeggero di un veicolo per utilizzare questo comando!");
	new slotid;
	if(sscanf(params, "d", slotid))
	{
		SendClientMessage(playerid, COLOR_ERROR, "/arma <slot (0 - 6)>");
		SendClientMessage(playerid, COLOR_ERROR, "0: disarma, 1: armi bianche, 2: pistole, 3: fucili, 4: SMG, 5: Fucili d'assalto, 6: Fucile di precisione");
		return 1;
	}
	if(slotid == 0)
		return SetPlayerArmedWeapon(playerid, 0);
	if(slotid < 1 || slotid > 6)
		return SendClientMessage(playerid, COLOR_ERROR, "/arma <slot (0 - 6)>");

	new weaponid, ammo;

    GetPlayerWeaponData(playerid, slotid, weaponid, ammo);

	switch(weaponid)
	{
		case 22, 25, 28 .. 33:
 		{
			SetPlayerArmedWeapon(playerid, weaponid);
		}
		default: SetPlayerArmedWeapon(playerid, 0);
	}
	return 1;
}