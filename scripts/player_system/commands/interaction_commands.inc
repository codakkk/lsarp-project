#include <log_system\core>
#include <YSI_Coding\y_hooks>



// RISOLVERE
flags:entra(CMD_ALIVE_USER);
CMD:entra(playerid, params[])
{
	if(!Character_CanInteract(playerid))
		return 0;
	Character_Enter(playerid);
    return 1;
}
alias:entra("esci");


flags:dai(CMD_ALIVE_USER);
CMD:dai(playerid, params[])
{
    new
        id,
        text[128];
    if(sscanf(params, "k<u>s[128]", id, text))
    {
        SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /dai [playerid/partofname] [oggetto]");
        SendClientMessage(playerid, COLOR_GREY, "Oggetti: arma");
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
	
	Character_GiveMoney(playerid, -amount);
	Character_GiveMoney(id, amount);

	Character_AMe(playerid, "prende dei soldi e li da a %s.", Character_GetRolePlayName(id));
	SendFormattedMessage(id, -1, "{cbccce}%s ti ha dato {8fbc8f}$%s{cbccce}.", Character_GetRolePlayName(playerid), FormatNumber(amount));
	SendFormattedMessage(playerid, -1, "{cbccce}Hai dato {8fbc8f}$%s{cbccce} a %s.", FormatNumber(amount), Character_GetRolePlayName(id));

	if(amount >= 10000)
	{
		SendMessageToAdmins(1, COLOR_ORANGE, "AdmAlert: %s ha pagato $%d a %s.", Character_GetOOCName(playerid), amount, Character_GetOOCName(id));
		LogCharacter(playerid, id, LOG_TYPE_GIVE_MONEY, "/paga", amount);
		LogCharacter(id, playerid, LOG_TYPE_RECEIVE_MONEY, "/paga", amount);
	}
	return 1;
}

flags:accetta(CMD_USER);
CMD:accetta(playerid, params[])
{
	if(!isnull(params))
	{
		if(!Character_IsAlive(playerid))
		{
			if(!strcmp(params, "morte", true))
				return Character_AcceptDeathState(playerid);

			if(strcmp(params, "cure", true))
				return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
		}
	} 
	


	new type = Request_GetTypeFromString(params);

	if(type == REQUEST_TYPE_NONE)
	{
		SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /accetta [oggetto]");
		SendClientMessage(playerid, COLOR_COMMAND_ERROR, "Oggetti: arma, oggetto, morte, cure,  veicolo, (ripa)razione, (vern)iciatura, (perq)uisizione, ...");
		SendClientMessage(playerid, COLOR_COMMAND_ERROR, "... (edi)ficio (biz), (faz)ione, blackjack");
		return 1;
	}

	if(!Character_CanAcceptRequest(playerid, type))
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai questo tipo di richiesta attiva.");

	CallLocalFunction(#OnCharacterTryAccept, "dd", playerid, type);
    return 1;
}

flags:rifiuta(CMD_USER);
CMD:rifiuta(playerid, params[])
{
	new type = Request_GetTypeFromString(params);

	if(type == REQUEST_TYPE_NONE)
	{
		SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /rifiuta <text>");
		SendClientMessage(playerid, COLOR_GREY, "Text: arma, oggetto, morte, (ripa)razione, (vern)iciatura, (perq)uisizione, ...");
		SendClientMessage(playerid, COLOR_GREY, "... (edi)ficio (biz), cure, veicolo, blackjack");
		return 1;
	}
	
	if(!Character_CanAcceptRequest(playerid, type))
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai questo tipo di richiesta attiva.");

	DeclineRequest(playerid, type);
	return 1;
}

flags:annulla(CMD_USER);
CMD:annulla(playerid, params[])
{
	if(Character_IsBuyingVehicle(playerid))
	{
		return Dealership_PlayerCancelBuy(playerid);
	}
	
	new type = Request_GetTypeFromString(params);

	if(type == REQUEST_TYPE_NONE)
	{
		SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /annulla <text>");
		SendClientMessage(playerid, COLOR_GREY, "Text: arma, oggetto, cure, (ripa)razione, (vern)iciatura, (perq)uisizione, ...");
		SendClientMessage(playerid, COLOR_GREY, "... (edi)ficio (biz), centralino");
		return 1;
	}
	
	if(!Request_IsPending(playerid, type) || Request_GetSender(playerid, type) != playerid)
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai inviato questo tipo di richiesta.");

	CancelRequest(playerid, type);
	return 1;
}

static DeclineRequest(playerid, type)
{
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
	new sender = Request_GetSender(playerid, type),
		receiver = Request_GetReceiver(playerid, type);
	
	Request_Reset(sender, type);
	Request_Reset(receiver, type);

	SendFormattedMessage(sender, COLOR_REJECT_REQUEST, "Hai annullato la richiesta (%s) inviata a %s.", Request_GetTypeName(type), Character_GetRolePlayName(receiver));
	SendFormattedMessage(receiver, COLOR_REJECT_REQUEST, "%s ha annullato la proposta (%s).", Character_GetRolePlayName(sender), Request_GetTypeName(type));
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