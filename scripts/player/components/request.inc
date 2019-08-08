#include <YSI_Coding\y_hooks>

static enum e_RequestData
{
	Pending,
	ByPlayer,
	ToPlayer,
	Time,
	Item,
	Amount,
	Extra,
	Type,
	Slot,
};

static 
	PendingRequestInfo[MAX_PLAYERS][e_RequestData]
	;


stock ResetPendingRequest(playerid)
{
	PendingRequestInfo[playerid][Pending] = 0;
	PendingRequestInfo[playerid][ByPlayer] = -1;
	PendingRequestInfo[playerid][ToPlayer] = -1;
	PendingRequestInfo[playerid][Time] = 0;
	PendingRequestInfo[playerid][Item] = 0;
	PendingRequestInfo[playerid][Amount] = 0;
	PendingRequestInfo[playerid][Extra] = 0;
	PendingRequestInfo[playerid][Type] = REQUEST_TYPE_NONE;
	PendingRequestInfo[playerid][Slot] = 0;
}

hook OnPlayerClearData(playerid)
{
	if(Request_IsPending(playerid))
	{
		new toPlayer = PendingRequestInfo[playerid][ToPlayer],
			fromPlayer = PendingRequestInfo[playerid][ByPlayer];
		if(PendingRequestInfo[toPlayer][Pending] && PendingRequestInfo[toPlayer][ByPlayer] == playerid)
		{
			SendFormattedMessage(toPlayer, COLOR_ERROR, "La richiesta di %s è stata annullata poiché si è disconnesso.", Character_GetRolePlayName(playerid));
			ResetPendingRequest(toPlayer);
		}
		else if(PendingRequestInfo[fromPlayer][Pending] && PendingRequestInfo[fromPlayer][ToPlayer] == playerid)
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
		if(GetTickCount() - PendingRequestInfo[playerid][Time] > PENDING_REQUEST_TIME * 60 * 1000) // 60 seconds
		{
			new byPlayer = PendingRequestInfo[playerid][ByPlayer];
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
	PendingRequestInfo[byPlayer][Pending] = PendingRequestInfo[toPlayer][Pending] = 1;

	PendingRequestInfo[byPlayer][ToPlayer] = toPlayer;
	PendingRequestInfo[toPlayer][ByPlayer] = byPlayer;

	PendingRequestInfo[byPlayer][Time] = PendingRequestInfo[toPlayer][Time] = GetTickCount();

	PendingRequestInfo[byPlayer][Item] = PendingRequestInfo[toPlayer][Item] = item;

	PendingRequestInfo[byPlayer][Slot] = PendingRequestInfo[toPlayer][Slot] = slot;

	PendingRequestInfo[byPlayer][Amount] = PendingRequestInfo[toPlayer][Amount] = amount;

	PendingRequestInfo[byPlayer][Type] = PendingRequestInfo[toPlayer][Type] = type;
}

//stock Character_SetPendingRequest(playerid, bool:)

stock Request_IsPending(playerid)
{
	return PendingRequestInfo[playerid][Pending];
}

stock Request_GetItem(playerid)
{
	return PendingRequestInfo[playerid][Item];
}

stock Request_GetAmount(playerid)
{
	return PendingRequestInfo[playerid][Amount];
}

stock Request_GetSlot(playerid)
{
	return PendingRequestInfo[playerid][Slot];
}

stock Request_GetType(playerid)
{
	return PendingRequestInfo[playerid][Type];
}

stock Request_GetSender(playerid)
{
	return PendingRequestInfo[playerid][ByPlayer];
}

stock Request_GetReceiver(playerid)
{
	return PendingRequestInfo[playerid][ToPlayer];
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
		SendFormattedMessage(senderid, COLOR_ACCEPT_REQUEST, "Hai dato l'arma (%s) con %d proiettili a %s.", ServerItem_GetName(itemid), extra, Character_GetRolePlayName(playerid));
		SendFormattedMessage(playerid, COLOR_ACCEPT_REQUEST, "%s ti ha dato un'arma (%s) con %d proiettili.", Character_GetRolePlayName(senderid), ServerItem_GetName(itemid), extra);
	}
	else
	{
		SendFormattedMessage(senderid, COLOR_ACCEPT_REQUEST, "Hai dato l'oggetto (%s, %d) a %s.", ServerItem_GetName(itemid), amount, Character_GetRolePlayName(playerid));
		SendFormattedMessage(playerid, COLOR_ACCEPT_REQUEST, "%s ti ha dato un oggetto (%s, %d).", Character_GetRolePlayName(senderid), ServerItem_GetName(itemid), amount);
	}
	Character_AMe(senderid, "prende degli oggetti e li da a %s.", Character_GetRolePlayName(playerid));

	ResetPendingRequest(senderid);
	ResetPendingRequest(playerid);
	return 1;
}