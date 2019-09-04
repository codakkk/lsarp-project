#include <YSI_Coding\y_hooks>

enum _:e_PendingType
{
	REQUEST_TYPE_NONE,
	REQUEST_TYPE_WEAPON,
	REQUEST_TYPE_ITEM,
	REQUEST_TYPE_REPAIR,
	REQUEST_TYPE_VEHICLE_SPRAY,
	REQUEST_TYPE_SEARCH,
	REQUEST_TYPE_BUILDING,
	REQUEST_TYPE_CARE,
	REQUEST_TYPE_FACTION,
	REQUEST_TYPE_VEHICLE,

	// Write request types before REQUEST_TYPE_LAST

	REQUEST_TYPE_LAST
};

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


stock Request_Reset(playerid, type)
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
				SendFormattedMessage(toPlayer, COLOR_ERROR, "La richiesta (%s) di %s è stata annullata poiché si è disconnesso.", Request_GetTypeName(i), Character_GetRolePlayName(playerid));
				Request_Reset(toPlayer, i);
			}
			else if(PendingRequestInfo[fromPlayer][i][Pending] && PendingRequestInfo[fromPlayer][i][ToPlayer] == playerid)
			{
				SendFormattedMessage(fromPlayer, COLOR_ERROR, "La richiesta (%s) inviata a %s è stata annullata poiché si è disconnesso.", Request_GetTypeName(i), Character_GetRolePlayName(playerid));
				Request_Reset(fromPlayer, i);
			}
		}
		Request_Reset(playerid, i);
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
				new toPlayer = Request_GetReceiver(playerid, i),
					byPlayer = Request_GetSender(playerid, i);

				SendFormattedMessage(toPlayer, COLOR_ERROR, "La proposta (%s) di %s è scaduta.", Request_GetTypeName(i), Character_GetRolePlayName(byPlayer));
				Request_Reset(toPlayer, i);
		
				SendFormattedMessage(byPlayer, COLOR_ERROR, "La proposta (%s) inviata a %s è scaduta.", Request_GetTypeName(i), Character_GetRolePlayName(toPlayer));
				Request_Reset(byPlayer, i);
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
	PendingRequestInfo[byPlayer][type][ByPlayer] = byPlayer;

	PendingRequestInfo[toPlayer][type][ByPlayer] = byPlayer;
	PendingRequestInfo[toPlayer][type][ToPlayer] = toPlayer;

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
		return 0;
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

	if(!IsPlayerInRangeOfPlayer(playerid, Request_GetSender(playerid, REQUEST_TYPE_ITEM), 5.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore che ti ha inviato la richiesta.");

	new 
		senderid = Request_GetSender(playerid, REQUEST_TYPE_ITEM),
		amount = Request_GetAmount(playerid, REQUEST_TYPE_ITEM),
		slotid = Request_GetSlot(playerid, REQUEST_TYPE_ITEM);

	new
		itemid = Character_GetSlotItem(senderid, slotid),
		slotAmount = Character_GetItemAmount(playerid, itemid),
		extra = Character_GetSlotExtra(senderid, slotid, 0)
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

	Request_Reset(senderid, REQUEST_TYPE_ITEM);
	Request_Reset(playerid, REQUEST_TYPE_ITEM);
	return 1;
}

stock Request_GetTypeName(type)
{
    new l[16];
    switch(type)
    {
	   case REQUEST_TYPE_WEAPON: l = "Arma";
	   case REQUEST_TYPE_ITEM: l = "Oggetto";
	   case REQUEST_TYPE_REPAIR: l = "Riparazione";
	   case REQUEST_TYPE_VEHICLE_SPRAY: l = "Verniciatura";
	   case REQUEST_TYPE_SEARCH: l = "Perquisizione";
	   case REQUEST_TYPE_BUILDING: l = "Edificio";
	   case REQUEST_TYPE_CARE: l = "Cura";
	   case REQUEST_TYPE_FACTION: l = "Fazione";
	   case REQUEST_TYPE_VEHICLE: l = "Veicolo";
	   default: l = "Sconosciuto";
    }
    return l;
}