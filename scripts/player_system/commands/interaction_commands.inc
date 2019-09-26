#include <YSI_Coding\y_hooks>



// RISOLVERE
flags:entra(CMD_ALIVE_USER);
CMD:entra(playerid, params[])
{
	//new id, E_ELEMENT_TYPE:type;
    //if(Character_GetNearestPickup(playerid, id, type))
	Character_Enter(playerid);
    //else 
		//SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata di un edificio.");
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
    new
        id,
        text[128];
    if(sscanf(params, "k<u>s[128]", id, text))
    {
        SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /dai <playerid/partofname> <oggetto>");
        SendClientMessage(playerid, COLOR_ERROR, "Oggetti: arma");
        return 1;
    }
    
    if(id < 0 || id >= MAX_PLAYERS || !Character_IsLogged(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Giocatore non connesso.");
    
	if(id == playerid)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi usare questo comando su te stesso.");

    if(!IsPlayerInRangeOfPlayer(playerid, id, 3.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");

	if(!Character_IsAlive(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su questo giocatore.");

	if(!strcmp(text, "arma", true))
	{
		if(Request_IsPending(playerid, REQUEST_TYPE_WEAPON))
			return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta d'arma attiva.");
		
		if(Request_IsPending(id, REQUEST_TYPE_WEAPON))
			return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore ha già una richiesta d'arma attiva.");

		new weapon = Character_GetWeapon(playerid);
		new ammo = Character_GetAmmo(playerid);
		
		if(weapon == 0)
			return SendClientMessage(playerid, COLOR_ERROR, "Non hai un'arma in mano.");
		
		if(Character_IsFactionDuty(playerid))
			return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se sei in servizio");

		SendFormattedMessage(playerid, COLOR_SEND_REQUEST, "Hai proposto di dare l'arma (%s) con %d proiettili a %s.", Weapon_GetName(weapon), ammo, Character_GetRolePlayName(id));
		SendClientMessage(playerid, COLOR_SEND_REQUEST, "Digita \"/annulla arma\" per annullare.");

		SendFormattedMessage(id, COLOR_RECEIVE_REQUEST, "%s vuole darti un'arma (%s) con %d proiettili.", Character_GetRolePlayName(playerid), Weapon_GetName(weapon), ammo);
		SendClientMessage(id, COLOR_RECEIVE_REQUEST, "Digita \"/accetta arma\" per accettare o \"/rifiuta arma\" per rifiutare.");

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
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USA: /paga [playerid/partofname] [ammontare]");
	if(id == playerid || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");
	if(amount < 0 || amount > Character_GetMoney(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai tutti questi soldi.");
	if(!IsPlayerInRangeOfPlayer(playerid, id, 2.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
	if(!Character_IsAlive(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su questo giocatore.");
	if(Character_GetLevel(playerid) <= 3)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi ancora utilizzare questo comando.");
	Character_GiveMoney(playerid, -amount);
	Character_GiveMoney(id, amount);
	Character_AMe(playerid, "prende dei soldi e li da a %s.", Character_GetRolePlayName(id));
	SendFormattedMessage(id, -1, "{cbccce}%s ti ha dato {8fbc8f}$%s{cbccce}.", Character_GetRolePlayName(playerid), FormatNumber(amount));
	SendFormattedMessage(playerid, -1, "{cbccce}Hai dato {8fbc8f}$%s{cbccce} a %s.", FormatNumber(amount), Character_GetRolePlayName(id));

	if(amount >= 10000)
		SendMessageToAdmins(1, COLOR_ORANGE, "AdmAlert: %s ha pagato $%d a %s.", Character_GetOOCName(playerid), amount, Character_GetOOCName(id));
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
			if(!Request_IsPending(playerid, REQUEST_TYPE_WEAPON))
				return SendClientMessage(playerid, COLOR_ERROR, "Non hai una richiesta d'arma attiva.");
			if(!IsPlayerInRangeOfPlayer(playerid, Request_GetSender(playerid, REQUEST_TYPE_WEAPON), 5.0))
				return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
			new slotid = Request_GetSlot(playerid, REQUEST_TYPE_WEAPON),
				weaponid = Request_GetItem(playerid, REQUEST_TYPE_WEAPON),
				tempWeapon = 0,
				ammo = 0,
				requestSender = Request_GetSender(playerid, REQUEST_TYPE_WEAPON);
			GetPlayerWeaponData(requestSender, slotid, tempWeapon, ammo);
			if(weaponid != tempWeapon || ammo == 0)
			{
				Request_Reset(playerid, REQUEST_TYPE_WEAPON);
				Request_Reset(requestSender, REQUEST_TYPE_WEAPON);
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
				
				Request_Reset(playerid, REQUEST_TYPE_WEAPON);
				Request_Reset(requestSender, REQUEST_TYPE_WEAPON);
			}
			else if(Character_HasSpaceForItem(playerid, weaponid, ammo))
			{
				Character_RemoveWeapon(requestSender, weaponid);
				Character_GiveItem(playerid, weaponid, 1, ammo);

				SendFormattedMessage(playerid, COLOR_ACCEPT_REQUEST, "Hai accettato l'arma (%s) con %d proiettili da %s.", Weapon_GetName(weaponid), ammo, Character_GetRolePlayName(requestSender));
				SendClientMessage(playerid, COLOR_ACCEPT_REQUEST, "L'arma è stata messa nell'inventario poiché lo slot è occupato.");

				SendFormattedMessage(requestSender, COLOR_ACCEPT_REQUEST, "%s (%d) ha accettato la tua arma.", Character_GetRolePlayName(playerid), playerid);

				Request_Reset(playerid, REQUEST_TYPE_WEAPON);
				Request_Reset(requestSender, REQUEST_TYPE_WEAPON);
			}
			else
				SendClientMessage(playerid, COLOR_ERROR, "Hai già un'arma e non hai abbastanza spazio nell'inventario.");

		}
		else if(!strcmp(params, "morte", true))
		{
			Character_AcceptDeathState(playerid);
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
		else if(!strcmp(params, "fazione", true) || !strcmp(params, "faz", true) || !strcmp(params, "faction", true))
		{
			Character_AcceptFaction(playerid);	
		}
		else if(!strcmp(params, "veicolo", true) || !strcmp(params, "vehicle", true) || !strcmp(params, "veh", true))
		{
			Character_AcceptVehicleRequest(playerid);	
		}
		else if(!strcmp(params, "blackjack", true))
		{
			Character_AcceptBlackJack(playerid);
		}
		//else
			//CallLocalFunction(#OnCharacterAccept, "ds", playerid, params);
	}
	else
	{
		SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /accetta <oggetto>");
		SendClientMessage(playerid, COLOR_GREY, "Oggetti: arma, oggetto, morte, cure,  veicolo, (ripa)razione, (vern)iciatura, (perq)uisizione, ...");
		SendClientMessage(playerid, COLOR_GREY, "... (edi)ficio (biz), (faz)ione, blackjack");
	}
    return 1;
}

flags:rifiuta(CMD_USER);
CMD:rifiuta(playerid, params[])
{
	new type = REQUEST_TYPE_NONE;
	if(!strcmp(params, "morte", true))
		return SendClientMessage(playerid, COLOR_ERROR, "Non decidi tu.");
	else if(!strcmp(params, "arma", true))
	{
		type = REQUEST_TYPE_WEAPON;
	}
	else if(!strcmp(params, "oggetto", true))
	{
		type = REQUEST_TYPE_ITEM;
	}
	else if(!strcmp(params, "riparazione", true) || !strcmp(params, "ripa", true))
	{
		type = REQUEST_TYPE_REPAIR;
	}
	else if(!strcmp(params, "verniciatura", true) || !strcmp(params, "vern", true))
	{
		type = REQUEST_TYPE_VEHICLE_SPRAY;
	}
	else if(!strcmp(params, "perquisizione", true) || !strcmp(params, "perq", true))
	{
		type = REQUEST_TYPE_SEARCH;
	}
	else if(!strcmp(params, "edificio", true) || !strcmp(params, "edi", true) || !strcmp(params, "biz", true))
	{
		type = REQUEST_TYPE_BUILDING;
	}
	else if(!strcmp(params, "cura", true) || !strcmp(params, "cure", true) || !strcmp(params, "heal", true))
	{
		type = REQUEST_TYPE_CARE;
	}
	else if(!strcmp(params, "veicolo", true) || !strcmp(params, "vehicle", true) || !strcmp(params, "veh", true))
	{
		type = REQUEST_TYPE_VEHICLE;
	}
	else if(!strcmp(params, "fazione", true) || !strcmp(params, "faction", true))
	{
		type = REQUEST_TYPE_FACTION;
	}
	else if(!strcmp(params, "blackjack", true))
	{
		type = REQUEST_TYPE_BLACKJACK;
	}
	if(type != REQUEST_TYPE_NONE)
	{
		DeclineRequest(playerid, type);
	}
	else 
	{
		SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /rifiuta <text>");
		SendClientMessage(playerid, COLOR_GREY, "Text: arma, oggetto, morte, (ripa)razione, (vern)iciatura, (perq)uisizione, ...");
		SendClientMessage(playerid, COLOR_GREY, "... (edi)ficio (biz), cure, veicolo, blackjack");
	}
	return 1;
}

static DeclineRequest(playerid, type)
{
	if(!Request_IsPending(playerid, type))
		return SendFormattedMessage(playerid, COLOR_ERROR, "Non hai una richiesta (%s) attiva.", Request_GetTypeName(type));
	
	new sender = Request_GetSender(playerid, type),
		receiver = Request_GetReceiver(playerid, type);
	
	Request_Reset(sender, type);
	Request_Reset(receiver, type);

	SendFormattedMessage(sender, COLOR_REJECT_REQUEST, "%s ha rifiutato la tua richiesta (%s).", Character_GetRolePlayName(receiver), Request_GetTypeName(type));
	SendFormattedMessage(receiver, COLOR_REJECT_REQUEST, "Hai rifiutato la proposta (%s) di %s.", Request_GetTypeName(type), Character_GetRolePlayName(sender));
	return 1;
}

static CancelRequest(playerid, type)
{
	if(!Request_IsPending(playerid, type))
		return SendFormattedMessage(playerid, COLOR_ERROR, "Non hai una richiesta (%s) attiva.", Request_GetTypeName(type));
	
	new sender = Request_GetSender(playerid, type),
		receiver = Request_GetReceiver(playerid, type);
	
	Request_Reset(sender, type);
	Request_Reset(receiver, type);

	SendFormattedMessage(sender, COLOR_REJECT_REQUEST, "Hai annullato la richiesta (%s) inviata a %s.", Request_GetTypeName(type), Character_GetRolePlayName(receiver));
	SendFormattedMessage(receiver, COLOR_REJECT_REQUEST, "%s ha annullato la proposta (%s).", Character_GetRolePlayName(sender), Request_GetTypeName(type));
	return 1;
}

flags:annulla(CMD_USER);
CMD:annulla(playerid, params[])
{
	if(Character_IsBuyingVehicle(playerid))
	{
		return Dealership_PlayerCancelBuy(playerid);
	}
	new type = REQUEST_TYPE_NONE;
	if(!strcmp(params, "morte", true))
		return SendClientMessage(playerid, COLOR_ERROR, "Non decidi tu.");
	else if(!strcmp(params, "arma", true))
	{
		type = REQUEST_TYPE_WEAPON;
	}
	else if(!strcmp(params, "oggetto", true))
	{
		type = REQUEST_TYPE_ITEM;
	}
	else if(!strcmp(params, "riparazione", true) || !strcmp(params, "ripa", true))
	{
		type = REQUEST_TYPE_REPAIR;
	}
	else if(!strcmp(params, "verniciatura", true) || !strcmp(params, "vern", true))
	{
		type = REQUEST_TYPE_VEHICLE_SPRAY;
	}
	else if(!strcmp(params, "perquisizione", true) || !strcmp(params, "perq", true))
	{
		type = REQUEST_TYPE_SEARCH;
	}
	else if(!strcmp(params, "edificio", true) || !strcmp(params, "edi", true) || !strcmp(params, "biz", true))
	{
		type = REQUEST_TYPE_BUILDING;
	}
	else if(!strcmp(params, "cure", true) || !strcmp(params, "cura", true) || !strcmp(params, "heal", true))
	{
		type = REQUEST_TYPE_CARE;
	}
	else if(!strcmp(params, "fazione", true) || !strcmp(params, "faction", true))
	{
		type = REQUEST_TYPE_FACTION;
	}
	else if(!strcmp(params, "veicolo", true) || !strcmp(params, "vehiche", true) || !strcmp(params, "veh", true))
	{
		type = REQUEST_TYPE_VEHICLE;
	}
	if(type != REQUEST_TYPE_NONE)
	{
		CancelRequest(playerid, type);
	}
	else 
	{
		SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /annulla <text>");
		SendClientMessage(playerid, COLOR_GREY, "Text: arma, oggetto, cure, (ripa)razione, (vern)iciatura, (perq)uisizione, ...");
		SendClientMessage(playerid, COLOR_GREY, "... (edi)ficio (biz)");
	}
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
	}
	CallLocalFunction(#OnCharacterTryBuy, "d", playerid);
    return 1;//SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando qui.");
}

flags:rimuovi(CMD_ALIVE_USER);
CMD:rimuovi(playerid, params[])
{
    new
        text[128];
    if(sscanf(params, "s[128]", text))
    {
        SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /rimuovi <oggetto>");
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
        Character_GiveItem(playerid, Character_GetBag(playerid), 1);
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
		SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /arma <slot (0 - 6)>");
		SendClientMessage(playerid, COLOR_ERROR, "0: disarma, 1: armi bianche, 2: pistole, 3: fucili, 4: SMG, 5: Fucili d'assalto, 6: Fucile di precisione");
		return 1;
	}
	if(slotid == 0)
	{
		SetPlayerArmedWeapon(playerid, 0);
		ApplyAnimation(playerid, "CARRY", "crry_prtial", 2.0, 0, 0, 0, 0, 0);
		SetPlayerSpecialAction(playerid, SPECIAL_ACTION_NONE);
		return 1;
	}
	if(slotid < 1 || slotid > 6)
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /arma <slot (0 - 6)>");

	new weaponid, ammo;
    Character_GetWeaponData(playerid, slotid, weaponid, ammo);

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

CMD:myweapons(playerid, params[])
{
	SendClientMessage(playerid, 0xFF6347FF, "Le armi che possiedi sono:");
	return 1;
}