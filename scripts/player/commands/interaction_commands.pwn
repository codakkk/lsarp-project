#include <YSI_Coding\y_hooks>

flags:entra(CMD_USER);
CMD:entra(playerid, params[])
{
    new pickupid = pLastPickup[playerid];
    if(pickupid != -1)
    {
        new id, E_ELEMENT_TYPE:type;
        Pickup_GetInfo(pickupid, id, type);
        Player_Enter(playerid, pickupid, id, type);
    }
    else return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata di un edificio!");
    return 1;
}

flags:esci(CMD_USER);
CMD:esci(playerid, params[])
{
    new pickupid = pLastPickup[playerid];
    if(pickupid != -1)
    {
        new id, E_ELEMENT_TYPE:type;
        Pickup_GetInfo(pickupid, id, type);
        Player_Exit(playerid, pickupid, id, type);
    }
    else return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'uscita di un edificio!");
    return 1;
}


Weapon_


flags:dai(CMD_USER);
CMD:dai(playerid, params[])
{
    new
        id,
        text[128];
    if(sscanf(params, "us[128]", id, text))
    {
        SendClientMessage(playerid, COLOR_ERROR, "/dai <playerid/partofname> <oggetto>");
        SendClientMessage(playerid, COLOR_ERROR, "Oggetti: arma");
        return 1;
    }
    
    if(id < 0 || id >= MAX_PLAYERS || !gCharacterLogged[id])
        return SendClientMessage(playerid, COLOR_ERROR, "Giocatore non connesso!");
    
    if(!ProxDetectorS(3.0, playerid, id))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore!");
    
	if(pPendingRequest[id][rdPending] && GetTickCount() - pPendingRequest[id][rdTime] < 20 * 1000) // 20 seconds
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore ha già una richiesta attiva!");

	if(!strcmp(text, "arma", true))
	{
		new weapon = GetPlayerWeapon(playerid);
		new ammo = GetPlayerAmmo(playerid);
		if(weapon == 0 || ammo <= 0)
			return SendClientMessage(playerid, COLOR_ERROR, "Non hai un'arma in mano!");
		SendFormattedMessage(playerid, COLOR_GREEN, "Hai proposto di dare l'arma (%s) con %d proiettili a %s (%d).", Weapon_GetName(weapon), ammo, Character_GetOOCName(id), id);
		SendFormattedMessage(id, COLOR_GREEN, "%s (%d) vuole darti un'arma (%s) con %d proiettili.", Character_GetOOCName(playerid), playerid, Weapon_GetName(weapon), ammo);
		SendClientMessage(playerid, -1, "Digita '{00FF00}/accetta arma{FFFFFF}' per accettare.");

		pPendingRequest[id][rdPending] = 1;
		pPendingRequest[id][rdByPlayer] = playerid;
		pPendingRequest[id][rdTime] = GetTickCount();
		pPendingRequest[id][rdItem] = weapon;
		pPendingRequest[id][rdAmount] = 1;
		pPendingRequest[id][rdExtra] = ammo;
		pPendingRequest[id][rdType] = PENDING_TYPE_WEAPON;
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
        SendClientMessage(playerid, COLOR_ERROR, "Oggetti: arma, chiave, veicolo");
        return 1;
    }
	if(!strcmp(text, "arma", true))
	{
		if(!pPendingRequest[playerid][rdPending] || pPendingRequest[playerid][rdType] != PENDING_TYPE_WEAPON)
			return SendClientMessage(playerid, color, "Non hai una richiesta d'arma attiva!");
		new weaponid = pPendingRequest[playerid][rdItem],
			ammo = pPendingRequest[playerid][rdExtra],
			requestSender = pPendingRequest[playerid][rdByPlayer];
		if(ACInfo[playerid][pWeapons][Weapon_GetSlot(weaponid)] == 0)
		{
			AC_GivePlayerWeapon(playerid, weaponid, ammo);
			SendFormattedMessage(playerid, COLOR_GREEN, "Hai accettato l'arma (%s) con %d proiettili da %s (%d).", Weapon_GetName(weaponid), ammo, Character_GetOOCName(requestSender), requestSender);
		}
		else if(Character_HasSpaceForItem(playerid, weaponid, ammo))
		{
			Character_GiveItem(playerid, weaponid, 1, ammo);
			SendFormattedMessage(playerid, COLOR_GREEN, "Hai accettato l'arma (%s) con %d proiettili da %s (%d).", Weapon_GetName(weaponid), ammo, Character_GetOOCName(requestSender), requestSender);
			SendClientMessage(playerid, COLOR_GREEN, "L'arma è stata messa nell'inventario poiché lo slot è occupato!");
		}
		else
			SendClientMessage(playerid, COLOR_ERROR, "Hai già un'arma e non hai abbastanza spazio nell'inventario!");

	}
    else if(!strcmp(text, "veicolo", true))
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
	else return SendClientMessage(playerid, COLOR_ERROR, "Oggetti: arma, chiave, veicolo");
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

flags:compra(CMD_USER);
CMD:compra(playerid, params[])
{
    if(gBuyingVehicle[playerid])
    {
        return ShowRoom_PlayerConfirmBuy(playerid);
    }
    if(pLastPickup[playerid] == -1 || !IsPlayerInRangeOfPickup(playerid, pLastPickup[playerid], 3.0))
        return 0;
    new
        eID,
        E_ELEMENT_TYPE:eType,
        Float:x, Float:y, Float:z;

    Pickup_GetInfo(pLastPickup[playerid], eID, eType);
    Pickup_GetPosition(pLastPickup[playerid], x, y, z);

    if(eType == ELEMENT_TYPE_DEALERSHIP)
    {
        ShowRoom_ShowVehiclesToPlayer(eID, playerid);
    }
    else if(eType == ELEMENT_TYPE_BUILDING_ENTRANCE && Building_IsValid(eID))
    {
        return Player_BuyBuilding(playerid, eID);
    }
    else if(eType == ELEMENT_TYPE_HOUSE_ENTRANCE)
    {
        return Player_BuyHouse(playerid, eID);
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