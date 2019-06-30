#include <YSI_Coding\y_hooks>

static enum e_RequestData
{
	rdPending,
	rdByPlayer,
	rdToPlayer,
	rdTime,
	rdItem,
	rdAmount,
	rdExtra,
	rdType,
	rdSlot,
};

static 
	PendingRequestInfo[MAX_PLAYERS][e_RequestData]
	;


stock ResetPendingRequest(playerid)
{
	PendingRequestInfo[playerid][rdPending] = 0;
	PendingRequestInfo[playerid][rdByPlayer] = -1;
	PendingRequestInfo[playerid][rdToPlayer] = -1;
	PendingRequestInfo[playerid][rdTime] = 0;
	PendingRequestInfo[playerid][rdItem] = 0;
	PendingRequestInfo[playerid][rdAmount] = 0;
	PendingRequestInfo[playerid][rdExtra] = 0;
	PendingRequestInfo[playerid][rdType] = REQUEST_TYPE_NONE;
	PendingRequestInfo[playerid][rdSlot] = 0;
}

hook OnPlayerClearData(playerid)
{
	if(Request_IsPending(playerid))
	{
		new toPlayer = PendingRequestInfo[playerid][rdToPlayer],
			fromPlayer = PendingRequestInfo[playerid][rdByPlayer];
		if(PendingRequestInfo[toPlayer][rdPending] && PendingRequestInfo[toPlayer][rdByPlayer] == playerid)
		{
			SendFormattedMessage(toPlayer, COLOR_ERROR, "La richiesta di %s è stata annullata poiché si è disconnesso.", Character_GetRolePlayName(playerid));
			ResetPendingRequest(toPlayer);
		}
		else if(PendingRequestInfo[fromPlayer][rdPending] && PendingRequestInfo[fromPlayer][rdToPlayer] == playerid)
		{
			SendFormattedMessage(fromPlayer, COLOR_ERROR, "La richiesta inviata a %s è stata annullata poiché si è disconnesso.", Character_GetRolePlayName(playerid));
			ResetPendingRequest(fromPlayer);
		}
	}
	ResetPendingRequest(playerid);
    return 1;
}

ptask OnRequestUpdateTimer[1000](playerid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	if(Request_IsPending(playerid))
	{
		if(GetTickCount() - PendingRequestInfo[playerid][rdTime] > PENDING_REQUEST_TIME * 60 * 1000) // 60 seconds
		{
			new byPlayer = PendingRequestInfo[playerid][rdByPlayer];
			SendFormattedMessage(playerid, COLOR_ERROR, "La richiesta da %s è scaduta.", Character_GetRolePlayName(byPlayer), byPlayer);
			ResetPendingRequest(playerid);
	
			SendFormattedMessage(byPlayer, COLOR_ERROR, "La richiesta inviata a %s è scaduta.", Character_GetRolePlayName(playerid), playerid);
			ResetPendingRequest(byPlayer);
		}
	}
	return 1;
}

stock Character_SetRequest(byPlayer, toPlayer, type, item = 0, amount = 0, slot = -1)
{
	PendingRequestInfo[byPlayer][rdPending] = PendingRequestInfo[toPlayer][rdPending] = 1;

	PendingRequestInfo[byPlayer][rdToPlayer] = toPlayer;
	PendingRequestInfo[toPlayer][rdByPlayer] = byPlayer;

	PendingRequestInfo[byPlayer][rdTime] = PendingRequestInfo[toPlayer][rdTime] = GetTickCount();

	PendingRequestInfo[byPlayer][rdItem] = PendingRequestInfo[toPlayer][rdItem] = item;

	PendingRequestInfo[byPlayer][rdSlot] = PendingRequestInfo[toPlayer][rdSlot] = slot;

	PendingRequestInfo[byPlayer][rdAmount] = PendingRequestInfo[toPlayer][rdAmount] = amount;

	PendingRequestInfo[byPlayer][rdType] = PendingRequestInfo[toPlayer][rdType] = type;
}

//stock Character_SetPendingRequest(playerid, bool:)

stock Request_IsPending(playerid)
{
	return PendingRequestInfo[playerid][rdPending];
}

stock Request_GetItem(playerid)
{
	return PendingRequestInfo[playerid][rdItem];
}

stock Request_GetAmount(playerid)
{
	return PendingRequestInfo[playerid][rdAmount];
}

stock Request_GetSlot(playerid)
{
	return PendingRequestInfo[playerid][rdSlot];
}

stock Request_GetType(playerid)
{
	return PendingRequestInfo[playerid][rdType];
}

stock Request_GetSender(playerid)
{
	return PendingRequestInfo[playerid][rdByPlayer];
}

stock Request_GetReceiver(playerid)
{
	return PendingRequestInfo[playerid][rdToPlayer];
}

stock Character_AcceptItemRequest(playerid)
{
	if(!Request_IsPending(playerid) || Request_GetType(playerid) != REQUEST_TYPE_ITEM)
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai una richiesta attiva.");

	if(!IsPlayerInRangeOfPlayer(playerid, Request_GetSender(playerid), 5.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore che ti ha inviato la richiesta.");

	new 
		senderid = Request_GetSender(playerid),
		amount = Request_GetAmount(playerid),
		slotid = Request_GetSlot(playerid);

	new
		itemid = Character_GetSlotItem(senderid, slotid),
		slotAmount = Inventory_GetItemAmount(Character_GetInventory(senderid), itemid),
		extra = Character_GetSlotExtra(senderid, slotid)
	;

	if(!Character_HasItem(senderid, itemid, amount))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non possiede più l'oggetto.");

	if(slotAmount < 1 || amount > slotAmount)
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non possiede la quantità necessaria.");
	
	if(!Character_HasSpaceForItem(playerid, itemid, amount))
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario.");

	Character_DecreaseItemAmount(senderid, itemid, amount);
	Character_GiveItem(playerid, itemid, amount, extra);

	if(ServerItem_GetType(itemid) == ITEM_TYPE_WEAPON)
	{
		SendFormattedMessage(senderid, COLOR_GREEN, "Hai dato l'arma (%s) con %d proiettili a %s.", ServerItem_GetName(itemid), extra, Character_GetRolePlayName(playerid));
		SendFormattedMessage(playerid, COLOR_GREEN, "%s ti ha dato un'arma (%s) con %d proiettili.", Character_GetRolePlayName(senderid), ServerItem_GetName(itemid), extra);
	}
	else
	{
		SendFormattedMessage(senderid, COLOR_GREEN, "Hai dato l'oggetto (%s, %d) a %s.", ServerItem_GetName(itemid), amount, Character_GetRolePlayName(playerid));
		SendFormattedMessage(playerid, COLOR_GREEN, "%s ti ha dato un oggetto (%s, %d).", Character_GetRolePlayName(senderid), ServerItem_GetName(itemid), amount);
	}
	Character_AMe(senderid, "prende degli oggetti e li da a %s.", Character_GetRolePlayName(playerid));

	ResetPendingRequest(senderid);
	ResetPendingRequest(playerid);
	return 1;
}