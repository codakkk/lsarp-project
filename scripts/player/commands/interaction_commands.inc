#include <YSI_Coding\y_hooks>



// RISOLVERE
flags:entra(CMD_ALIVE_USER);
CMD:entra(playerid, params[])
{
	new id, E_ELEMENT_TYPE:type;
    if(Character_GetNearestPickup(playerid, id, type))
    	Character_Enter(playerid, id, type);
    else 
		SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata di un edificio.");
    return 1;
}

flags:esci(CMD_ALIVE_USER);
CMD:esci(playerid, params[])
{
	new id, E_ELEMENT_TYPE:type;
    if(Character_GetNearestPickup(playerid, id, type))
    	Character_Exit(playerid, id, type);
    else
		SendClientMessage(playerid, COLOR_ERROR, "Non sei all'uscita di un edificio.");
    return 1;
}

flags:dai(CMD_ALIVE_USER);
CMD:dai(playerid, params[])
{
	if(Request_IsPending(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta attiva.");
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
        return SendClientMessage(playerid, COLOR_ERROR, "Giocatore non connesso.");
    
	if(id == playerid)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi usare questo comando su te stesso.");

    if(!IsPlayerInRangeOfPlayer(playerid, id, 3.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
    
	if(Request_IsPending(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore ha già una richiesta attiva.");
	
	if(!Character_IsAlive(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su questo giocatore.");

	if(!strcmp(text, "arma", true))
	{
		new weapon = Character_GetWeapon(playerid);
		new ammo = Character_GetAmmo(playerid);
		if(weapon == 0 || ammo <= 0)
			return SendClientMessage(playerid, COLOR_ERROR, "Non hai un'arma in mano.");
		
		if(Character_IsFactionDuty(playerid))
			return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se sei in servizio");

		SendFormattedMessage(playerid, COLOR_SEND_REQUEST, "Hai proposto di dare l'arma (%s) con %d proiettili a %s.", Weapon_GetName(weapon), ammo, Character_GetRolePlayName(id));
		SendClientMessage(playerid, COLOR_SEND_REQUEST, "Digita '{FF0000}/annulla{FFFFFF} per annullare.");
		SendFormattedMessage(id, COLOR_RECEIVE_REQUEST, "%s vuole darti un'arma (%s) con %d proiettili.", Character_GetRolePlayName(playerid), Weapon_GetName(weapon), ammo);
		SendClientMessage(id, COLOR_RECEIVE_REQUEST, "Digita '{00FF00}/accetta arma{FFFFFF}' per accettare.");

		Character_SetRequest(playerid, id, REQUEST_TYPE_WEAPON, weapon, 1, Weapon_GetSlot(weapon));
	}
	else
	{
		SendClientMessage(playerid, COLOR_ERROR, "Oggetti: arma");
	}
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
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai tutti questi soldi.");
	if(!IsPlayerInRangeOfPlayer(playerid, id, 2.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
	if(!Character_IsAlive(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su questo giocatore.");
	Character_GiveMoney(playerid, -amount);
	Character_GiveMoney(id, amount);
	Character_AMe(playerid, "prende dei soldi e li da a %s.", Character_GetRolePlayName(id));
	SendFormattedMessage(id, COLOR_GREEN, "%s ti ha dato $%d.", Character_GetRolePlayName(playerid), amount);
	SendFormattedMessage(playerid, COLOR_GREEN, "Hai dato $%d a %s.", amount, Character_GetRolePlayName(id));
	return 1;
}

flags:accetta(CMD_USER);
CMD:accetta(playerid, params[])
{
	if(!isnull(params) && strlen(params) <= 20)
	{
		if( strcmp(params, "morte", true) && strcmp(params, "cure", true) && !Character_IsAlive(playerid))
			return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
		if(!strcmp(params, "arma", true))
		{
			if(!Request_IsPending(playerid) || Request_GetType(playerid) != REQUEST_TYPE_WEAPON)
				return SendClientMessage(playerid, COLOR_ERROR, "Non hai una richiesta d'arma attiva.");
			if(!IsPlayerInRangeOfPlayer(playerid, Request_GetSender(playerid), 5.0))
				return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
			new slotid = Request_GetSlot(playerid),
				weaponid = Request_GetItem(playerid),
				tempWeapon = 0,
				ammo = 0,
				requestSender = Request_GetSender(playerid);
			GetPlayerWeaponData(requestSender, slotid, tempWeapon, ammo);
			if(weaponid != tempWeapon || ammo == 0)
			{
				ResetPendingRequest(playerid);
				ResetPendingRequest(requestSender);
				SendClientMessage(playerid, COLOR_ERROR, "Non possiedi più l'arma. La richiesta è stata annullata.");
				return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non possiede più l'arma, pertanto non puoi più accettare.");	
			}
			new w, a;
			Character_GetWeaponData(playerid, Weapon_GetSlot(weaponid), w, a);
			if(w == 0 || a == 0)
			{
				Character_RemoveWeapon(requestSender, weaponid);
				Character_GiveWeapon(playerid, weaponid, ammo);
				
				SendFormattedMessage(playerid, COLOR_ACCEPT_REQUEST, "Hai accettato l'arma (%s) con %d proiettili da %s.", Weapon_GetName(weaponid), ammo, Character_GetRolePlayName(requestSender));
				SendFormattedMessage(requestSender, COLOR_ACCEPT_REQUEST, "%s (%d) ha accettato la tua arma.", Character_GetRolePlayName(playerid), playerid);
				
				ResetPendingRequest(playerid);
				ResetPendingRequest(requestSender);
			}
			else if(Character_HasSpaceForItem(playerid, weaponid, ammo))
			{
				Character_RemoveWeapon(requestSender, weaponid);
				Character_GiveItem(playerid, weaponid, 1, ammo);

				SendFormattedMessage(playerid, COLOR_ACCEPT_REQUEST, "Hai accettato l'arma (%s) con %d proiettili da %s.", Weapon_GetName(weaponid), ammo, Character_GetRolePlayName(requestSender));
				SendClientMessage(playerid, COLOR_ACCEPT_REQUEST, "L'arma è stata messa nell'inventario poiché lo slot è occupato.");

				SendFormattedMessage(requestSender, COLOR_ACCEPT_REQUEST, "%s (%d) ha accettato la tua arma.", Character_GetRolePlayName(playerid), playerid);

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
				return SendClientMessage(playerid, COLOR_ERROR, "Il venditore si è disconnesso o non esiste.");
			if(Character_GetMoney(playerid) < pVehicleSellingPrice[playerid])
				return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi.");
			if(!IsPlayerInRangeOfPlayer(playerid, pVehicleSeller[playerid], 10.0))
				return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");

			SendFormattedMessage(pVehicleSeller[playerid], COLOR_GREEN, "%s ha accettato il tuo veicolo per $%d.", Character_GetOOCName(playerid), pVehicleSellingPrice[playerid]);
			SendFormattedMessage(playerid, COLOR_GREEN, "Hai comprato il veicolo di %s per $%d.", Character_GetOOCName(pVehicleSeller[playerid]), pVehicleSellingPrice[playerid]);

			Character_GiveMoney(playerid, -pVehicleSellingPrice[playerid]);
			Character_GiveMoney(pVehicleSeller[playerid], pVehicleSellingPrice[playerid]);

			new 
				vehicleid = pSellingVehicleID[playerid],
				sellerid = pVehicleSeller[playerid];

			new query[256];
			mysql_format(gMySQL, query, sizeof(query), "UPDATE `vehicles` SET owner_id = '%d' WHERE id = '%d'", Character_GetID(playerid), Vehicle_GetID(vehicleid));
			mysql_tquery(gMySQL, query);

			Vehicle_SetOwner(vehicleid, playerid);

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
		else if(!strcmp(params, "riparazione", true))
		{
			Character_AcceptRepair(playerid);
		}
		else if(!strcmp(params, "verniciatura", true) || !strcmp(params, "vern", true))
		{
			Character_AcceptVehicleSpray(playerid);
		}
		else if(!strcmp(params, "perquisizione", true) || !strcmp(params, "perq", true))
		{
			Character_AcceptSearch(playerid);
		}
		else if(!strcmp(params, "edificio", true) || !strcmp(params, "edi", true) || !strcmp(params, "biz", true))
		{
			Character_AcceptBuilding(playerid);
		}
	}
	else
	{
		SendClientMessage(playerid, COLOR_ERROR, "/accetta <oggetto>");
		SendClientMessage(playerid, COLOR_ERROR, "Oggetti: arma, oggetto, morte, veicolo, (ripa)razione, (vern)iciatura, (perq)uisizione, ...");
		SendClientMessage(playerid, COLOR_ERROR, "... edificio (biz)");
	}
    return 1;
}

flags:annulla(CMD_USER);
CMD:annulla(playerid, params[])
{
	if(Character_IsBuyingVehicle(playerid))
    {
        return Dealership_PlayerCancelBuy(playerid);
    }
	if(!Request_IsPending(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai una richiesta attiva.");

	new 
		toPlayer = Request_GetReceiver(playerid), 
		byPlayer = Request_GetSender(playerid)
	;
	if(toPlayer == playerid)
	{
		SendFormattedMessage(byPlayer, COLOR_REJECT_REQUEST, "%s ha rifiutato la tua richiesta (%s).", Character_GetRolePlayName(playerid), GetRequestTypeName(Request_GetType(playerid)));
		SendFormattedMessage(playerid, COLOR_REJECT_REQUEST, "Hai rifiutato la richiesta (%s) di %s.", Character_GetRolePlayName(byPlayer), GetRequestTypeName(Request_GetType(playerid)));
		ResetPendingRequest(byPlayer);
	}
	else if(byPlayer == playerid)
	{
		SendFormattedMessage(toPlayer, COLOR_REJECT_REQUEST, "%s ha annullato la richiesta (%s).", Character_GetRolePlayName(playerid), GetRequestTypeName(Request_GetType(playerid)));
		SendFormattedMessage(playerid, COLOR_REJECT_REQUEST, "Hai annullato la richiesta (%s) inviata a %s.", Character_GetRolePlayName(toPlayer), GetRequestTypeName(Request_GetType(playerid)));
		ResetPendingRequest(toPlayer);
	}
	ResetPendingRequest(playerid);
	return 1;
}

flags:compra(CMD_ALIVE_USER);
CMD:compra(playerid, params[])
{
	if(Character_IsBuyingVehicle(playerid))
	{
		return Dealership_PlayerConfirmBuy(playerid);
	}
	
	new
		eID,
		E_ELEMENT_TYPE:eType;
	
	if(Character_GetNearestPickup(playerid, eID, eType))
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
		else if(eType == ELEMENT_TYPE_CHOPSHOP)
		{
			return Garage_BuyMechanicalComponents(playerid);
		}
	}
    return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando qui.");
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
            return SendClientMessage(playerid, COLOR_ERROR, "Non stai indossando uno zaino.");
        if(Inventory_GetUsedSpace(playerInv) > PLAYER_INVENTORY_START_SIZE-1)
            return SendClientMessage(playerid, COLOR_ERROR, "Non puoi toglierti lo zaino se hai piu' di 9 oggetti.");
        Character_GiveItem(playerid, pInventoryBag[playerid], 1);
        Character_SetBag(playerid, 0);
        SendClientMessage(playerid, COLOR_GREEN, "Lo zaino è stato rimesso nel tuo inventario.");
        Character_AMe(playerid, "si toglie lo zaino.");
        return 1;
    }
	else
		SendClientMessage(playerid, COLOR_ERROR, "Oggetti: zaino");
    return 1;
}

CMD:arma(playerid, params[])
{
	if(!IsPlayerInAnyVehicle(playerid) || GetPlayerState(playerid) != PLAYER_STATE_PASSENGER)
		return SendClientMessage(playerid, COLOR_ERROR, "Devi essere passeggero di un veicolo per utilizzare questo comando.");
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