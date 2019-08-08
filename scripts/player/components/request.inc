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

static // Should I use PawnPlus MAp or List? mh
	PendingRequestInfo[MAX_PLAYERS][REQUEST_TYPE_LAST][e_RequestData]
	;


stock ResetPendingRequest(playerid, type)
{
	PendingRequestInfo[playerid][type][Pending] = 0;
	PendingRequestInfo[playerid][type][ByPlayer] = -1;
	PendingRequestInfo[playerid][type][ToPlayer] = -1;
	PendingRequestInfo[playerid][type][Time] = 0;
	PendingRequestInfo[playerid][type][Item] = 0;
	PendingRequestInfo[playerid][type][Amount] = 0;
	PendingRequestInfo[playerid][type][Extra] = 0;
	PendingRequestInfo[playerid][type][Type] = REQUEST_TYPE_NONE;
	PendingRequestInfo[playerid][type][Slot] = 0;
}

hook OnPlayerClearData(playerid)
{
	for(new i = 0; i < REQUEST_TYPE_LAST; ++i)
	{
		if(Request_IsPending(playerid, i))
		{
			new toPlayer = PendingRequestInfo[playerid][i][ToPlayer],
				fromPlayer = PendingRequestInfo[playerid][i][ByPlayer];
			if(PendingRequestInfo[toPlayer][i][Pending] && PendingRequestInfo[toPlayer][i][ByPlayer] == playerid)
			{
				SendFormattedMessage(toPlayer, COLOR_ERROR, "La richiesta (%s) di %s è stata annullata poiché si è disconnesso.", GetRequestTypeName(i), Character_GetRolePlayName(playerid));
				ResetPendingRequest(toPlayer);
			}
			else if(PendingRequestInfo[fromPlayer][i][Pending] && PendingRequestInfo[fromPlayer][i][ToPlayer] == playerid)
			{
				SendFormattedMessage(fromPlayer, COLOR_ERROR, "La richiesta (%s) inviata a %s è stata annullata poiché si è disconnesso.", GetRequestTypeName(i), Character_GetRolePlayName(playerid));
				ResetPendingRequest(fromPlayer);
			}
		}
		ResetPendingRequest(playerid, i);
	}
    return 1;
}

ptask OnRequestUpdateTimer[1000](playerid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	for(new i = 0; i < REQUEST_TYPE_LAST; ++i)
	{
		if(Request_IsPending(playerid, i))
		{
			if(GetTickCount() - PendingRequestInfo[playerid][i][Time] > PENDING_REQUEST_TIME * 60 * 1000) // 60 seconds
			{
				new byPlayer = PendingRequestInfo[playerid][i][ByPlayer];
				SendFormattedMessage(playerid, COLOR_ERROR, "La richiesta (%s) da %s è scaduta.", GetRequestTypeName(i), Character_GetRolePlayName(byPlayer), byPlayer);
				ResetPendingRequest(playerid, i);
		
				SendFormattedMessage(byPlayer, COLOR_ERROR, "La richiesta (%s) inviata a %s è scaduta.", GetRequestTypeName(i), Character_GetRolePlayName(playerid), playerid);
				ResetPendingRequest(byPlayer, i);
			}
		}
	}
	return 1;
}

stock Character_SetRequest(byPlayer, toPlayer, type, item = 0, amount = 0, slot = -1)
{
	if(! (REQUEST_TYPE_NONE < type < REQUEST_TYPE_LAST))
		return 1;
	
	PendingRequestInfo[byPlayer][type][Pending] = PendingRequestInfo[toPlayer][type][Pending] = 1;

	PendingRequestInfo[byPlayer][type][ToPlayer] = toPlayer;
	PendingRequestInfo[toPlayer][type][ByPlayer] = byPlayer;

	PendingRequestInfo[byPlayer][type][Time] = PendingRequestInfo[toPlayer][type][Time] = GetTickCount();

	PendingRequestInfo[byPlayer][type][Item] = PendingRequestInfo[toPlayer][type][Item] = item;

	PendingRequestInfo[byPlayer][type][Slot] = PendingRequestInfo[toPlayer][type][Slot] = slot;

	PendingRequestInfo[byPlayer][type][Amount] = PendingRequestInfo[toPlayer][type][Amount] = amount;

	PendingRequestInfo[byPlayer][type][Type] = PendingRequestInfo[toPlayer][type][Type] = type;
	return 1;
}

//stock Character_SetPendingRequest(playerid, bool:)

stock Request_IsPending(playerid, type)
{
	if(! (REQUEST_TYPE_NONE < type < REQUEST_TYPE_LAST))
		return 1;
	return PendingRequestInfo[playerid][type][Pending];
}

stock Request_GetItem(playerid, reqType)
{
	return PendingRequestInfo[playerid][reqType][Item];
}

stock Request_GetAmount(playerid, reqType)
{
	return PendingRequestInfo[playerid][reqType][Amount];
}

stock Request_GetSlot(playerid, reqType)
{
	return PendingRequestInfo[playerid][reqType][Slot];
}

stock Request_GetType(playerid, reqType)
{
	return PendingRequestInfo[playerid][reqType][Type];
}

stock Request_GetSender(playerid, reqType)
{
	return PendingRequestInfo[playerid][reqType][ByPlayer];
}

stock Request_GetReceiver(playerid, reqType)
{
	return PendingRequestInfo[playerid][reqType][ToPlayer];
}

stock Character_AcceptItemRequest(playerid)
{
	if(!Request_IsPending(playerid, REQUEST_TYPE_ITEM))
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