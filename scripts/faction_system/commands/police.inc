
#include <player_system\core>

#include <YSI_Coding\y_hooks>

// Do we need InspectingBy? This should be remove 

static 
	InspectedBy[MAX_PLAYERS] = {-1, ...},
	InspectingPlayer[MAX_PLAYERS] = {-1, ...};

hook OnPlayerClearData(playerid)
{
	new inspecting = InspectingPlayer[playerid];
	if(inspecting != -1 && InspectedBy[inspecting] == playerid)
	{
		InspectedBy[inspecting] = -1;
	}
	InspectingPlayer[playerid] = InspectedBy[playerid] = -1;
	return 1;
}

hook OnCharacterDeath(playerid, killerid, reason)
{
	if(Character_IsAlive(playerid))
	{
		Character_OffDuty(playerid);
	}
	return 1;
}

flags:manette(CMD_ALIVE_USER);
CMD:manette(playerid, params[])
{
	// MUST CHECK IT BETTER
	if( (!Faction_IsValid(Character_GetFaction(playerid)) || Faction_GetType(Character_GetFaction(playerid)) != FACTION_TYPE_POLICE) && !Player_IsAdminDuty(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei un membro della Guardia Nazionale.");
		
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /manette <playerid/partofname>");
	
	if(!Character_IsLogged(id) || id == playerid || Player_IsAdminDuty(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");

	if(!IsPlayerInRangeOfPlayer(playerid, id, 2.5))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore");
	
	Character_SetCuffed(id, !Character_IsCuffed(id));
	if(Character_IsCuffed(id))
	{
		Character_AMe(playerid, "ammanetta %s.", Character_GetRolePlayName(id));
	}
	else
	{
		Character_AMe(playerid, "toglie le manette a %s.", Character_GetRolePlayName(id));
	}
	return 1;
}

flags:arresta(CMD_POLICE);
CMD:arresta(playerid, params[])
{
	if(!Character_IsFactionDuty(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei in servizio.");
		
	new id, time;
	if(sscanf(params, "k<u>d", id, time))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /arresta <playerid/partofname> <minuti>");

	if(id == playerid || !Character_IsLogged(id) || IsPlayerNPC(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");

	if(Faction_IsValid(Character_GetFaction(id)) && Faction_GetType(Character_GetFaction(id)) == Faction_GetType(Character_GetFaction(playerid)))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi arrestare questo giocatore.");

	if(time < 0 || time > 720)
		return SendClientMessage(playerid, COLOR_ERROR, "Il tempo di arresto va da 0 a 720.");

	if(!IsPlayerInRangeOfPlayer(playerid, id, 4.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");

	if(!IsPlayerNearAnyArrestPos(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei al punto di arresto.");

	if(Character_IsDragging(playerid) && Character_GetDraggedBy(id) == playerid)
	{
		Character_SetDragging(playerid, false);
		Character_StopDragTimer(playerid);
	}

	Character_SetDragged(id, false);
	Character_SetDraggedBy(id, INVALID_PLAYER_ID);
	Character_SetFreezed(id, false);

	Character_SetCuffed(id, false);
	Character_SetJailTime(id, time);
	Character_SetICJailed(id, true);
	
	Character_OffDuty(id);
	
	ClearAnimations(id);

	Character_SetToJailPos(id);

	SendFormattedMessage(id, COLOR_ORANGE, "Sei stato arrestato da %s per %d minuti.", Character_GetOOCName(playerid));

	new rankName[MAX_FACTION_RANK_NAME];
	Faction_GetRankName(Character_GetFaction(playerid), Character_GetRank(playerid), rankName);
	
	new String:message = str_format("(( HQ: %s %s ha arrestato %s per %d minuti. ))", rankName, Character_GetOOCName(playerid), Character_GetOOCName(id), time);
	Faction_SendMessageStr(Character_GetFaction(playerid), message);

	SendFormattedMessage(playerid, COLOR_ORANGE, "** Hai arrestato %s per %d minuti.", Character_GetOOCName(id), time);

	Log(Character_GetOOCName(playerid), Character_GetOOCName(id), "/arresta", time);
	return 1;
}

flags:trascina(CMD_POLICE);
CMD:trascina(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /trascina <playerid/partofname>");

	if(playerid == id || !Character_IsLogged(id) || IsPlayerNPC(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");

	if(!Character_IsAlive(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi trascinare questo giocatore.");

	if(Character_IsJailed(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi trascinare un giocatore incarcerato.");
	
	if(Character_IsDragging(playerid) && Character_IsDragged(id) && Character_GetDraggedBy(id) == playerid)
	{
		Character_StopDragTimer(playerid);
		Character_SetDragged(id, false);
		Character_SetDraggedBy(id, -1);
		Character_SetFreezed(id, false);
		Character_SetDragging(playerid, false);
		SendClientMessage(playerid, COLOR_GREEN, "Hai finito di trascinare il giocatore.");
		return 1;
	}

	if(!IsPlayerInRangeOfPlayer(playerid, id, 3.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");

	if(Character_CanInteract(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è ammanettato o stordito.");
	
	Character_SetDragged(id, true);
	Character_SetDraggedBy(id, playerid);
	Character_SetFreezed(id, true);
	Character_SetDragging(playerid, true);
	Character_StartDragTimer(playerid, id);
	SendFormattedMessage(playerid, COLOR_GREEN, "Stai trascinando %s. Usa di nuovo /trascina %d per smettere.", Character_GetOOCName(id), id);
	Character_CharacterAMe(playerid, id, "trascina");
	return 1;
}

flags:cura(CMD_ALIVE_USER);
CMD:cura(playerid, params[])
{
	if(Request_IsPending(playerid, REQUEST_TYPE_CARE))
		return SendClientMessage(playerid, COLOR_ERROR, "Hai già una richiesta cure attiva.");
	
	new factionid = Character_GetFaction(playerid);
	if(!Player_IsAdminDuty(playerid))
	{
		if(Faction_GetType(factionid) != FACTION_TYPE_POLICE && Faction_GetType(factionid) != FACTION_TYPE_MEDICAL)
			return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando.");
		if(!Character_IsFactionDuty(playerid))
			return SendClientMessage(playerid, COLOR_ERROR, "Non sei in servizio.");
	}
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /cura <playerid/partofname>");
	
	if( (!Player_IsAdminDuty(playerid) && id == playerid) || !IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");
	
	if(!IsPlayerInRangeOfPlayer(playerid, id, 5.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
	
	new Float:health;
	GetPlayerHealth(id, health);

	if(!Character_IsInjured(id) && health >= 100.0)
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non ha bisogno di cure.");
	
	if(Request_IsPending(id, REQUEST_TYPE_CARE))
		return SendClientMessage(playerid, COLOR_ERROR, "Questo giocatore ha già una richiesta di cure.");

	Character_SetRequest(playerid, id, REQUEST_TYPE_CARE);

	SendFormattedMessage(id, COLOR_YELLOW, "%s vuole curarti. Digita \"/accetta cure\" per accettare o \"/rifiuta cure\" per rifiutare.", Character_GetRolePlayName(playerid));
	SendFormattedMessage(playerid, COLOR_YELLOW, "Richiesta di cure inviata a %s. Digita \"/annulla cure\" per annullare.", Character_GetRolePlayName(id));
	return 1;
}

flags:perquisisci(CMD_ALIVE_USER); //flags:perquisisci(CMD_POLICE);
CMD:perquisisci(playerid, params[])
{
	if(!Character_CanInteract(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
	new id;
	if(sscanf(params, "k<u>d", id))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /perquisisci <playerid/partofname/#id>");

	if(id == playerid)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su te stesso.");

	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");

	if(!IsPlayerInRangeOfPlayer(playerid, id, 5.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
	
	if(Request_IsPending(playerid, REQUEST_TYPE_SEARCH))
		return SendClientMessage(playerid, COLOR_ERROR, "Hai già una richiesta di perquisizione attiva.");

	if(Request_IsPending(id, REQUEST_TYPE_SEARCH))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore ha già una richiesta di perquisizione attiva.");
	
	SendFormattedMessage(id, COLOR_RECEIVE_REQUEST, "%s (%d) vuole perquisirti. Digita \"/accetta perquisizione\" per accettare o \"/rifiuta perquisizione\" per rifiutare.", Character_GetRolePlayName(playerid), playerid);
	SendFormattedMessage(playerid, COLOR_SEND_REQUEST, "Richiesta di perquisizione inviata a %s (%d).", Character_GetRolePlayName(id), id);

	Character_SetRequest(playerid, id, REQUEST_TYPE_SEARCH);
	return 1;
}

flags:megafono(CMD_POLICE);
CMD:megafono(playerid, params[])
{
	if(isnull(params) || strlen(params) >= 256)
		return SendClientMessage(playerid, COLOR_ERROR, "(/m)egafono <testo>");
	new String:str = str_format("%s o< %s", Character_GetRolePlayName(playerid), params);
	ProxDetectorStr(playerid, 30.0, str, 0xFFFF00FF, 0xFFFF00FF, 0xFFFF00FF, 0xFFFF00FF, 0xFFFF00FF);	
	return 1;
}
alias:megafono("m");

stock Character_AcceptSearch(playerid)
{
	if(!Request_IsPending(playerid, REQUEST_TYPE_SEARCH))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai una richiesta di perquisizione attiva."), 0;
    if(!IsPlayerInRangeOfPlayer(playerid, Request_GetSender(playerid, REQUEST_TYPE_SEARCH), 5.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore."), 0;
	new senderid = Request_GetSender(playerid, REQUEST_TYPE_SEARCH);

	SendFormattedMessage(senderid, COLOR_ACCEPT_REQUEST, "%s (%d) ha accettato la tua richiesta di perquisizione.", Character_GetRolePlayName(playerid), playerid);
	SendFormattedMessage(playerid, COLOR_ACCEPT_REQUEST, "Hai accettato la richiesta di perquisizione di %s (%d).", Character_GetRolePlayName(senderid), senderid);

	ShowInspectingInventory(senderid, playerid);
	return 1;
}

Dialog:Dialog_InvSearch(playerid, response, listitem, inputtext[])
{
	if(!response || Faction_GetType(Character_GetFaction(playerid)) != FACTION_TYPE_POLICE)
		return 1;
	
	new inspecting = InspectingPlayer[playerid];
	
	if(!IsPlayerInRangeOfPlayer(playerid, inspecting, 5.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
	
	new 
		itemid = Character_GetSlotItem(inspecting, listitem),
		amount = Character_GetSlotAmount(inspecting, listitem)
	;
	
	// Probably amount == 0 is an useless check. Inventory system is robust enough :P
	if(itemid == 0 || amount == 0)
		return ShowInspectingInventory(playerid, inspecting);

	if(!Character_HasSpaceForItem(playerid, itemid, amount))
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario per confiscare questo oggetto.");

	if(Character_DecreaseSlotAmount(inspecting, listitem, amount))
	{
		new rankName[MAX_FACTION_RANK_NAME];
		Faction_GetRankName(Character_GetFaction(playerid), Character_GetRank(playerid), rankName);
		
		SendFormattedMessage(inspecting, COLOR_BLUE, "%s %s ti ha confiscato un oggetto (%s. Quantità: %d).", rankName, Character_GetRolePlayName(playerid), ServerItem_GetName(itemid), amount);
		SendFormattedMessage(playerid, COLOR_BLUE, "Hai confiscato un oggetto (%s. Quantità: %d) a %s.", ServerItem_GetName(itemid), amount, Character_GetRolePlayName(inspecting));
	}
	ShowInspectingInventory(playerid, inspecting);
	return 1;
}

static ShowInspectingInventory(playerid, inspectingPlayer)
{
	if(inspectingPlayer == -1 || !IsPlayerConnected(inspectingPlayer) || !Character_IsLogged(inspectingPlayer))
		return 0;
	InspectingPlayer[playerid] = inspectingPlayer;
	new Inventory:inv = Character_GetInventory(inspectingPlayer);
	new str[128];
	format(str, sizeof(str), "Inventario di %s (%d/%d) - Denaro: $%d",  Character_GetRolePlayName(inspectingPlayer), Inventory_GetUsedSpace(inv), Inventory_GetSpace(inv), Character_GetMoney(inspectingPlayer));
	
	Inventory_Show(inv, playerid, str, Dialog_InvSearch, Faction_GetType(Character_GetFaction(playerid)) == FACTION_TYPE_POLICE ? ("Confisca") : ("Chiudi"), "Chiudi");
	
	Request_Reset(playerid, REQUEST_TYPE_SEARCH);
	Request_Reset(inspectingPlayer, REQUEST_TYPE_SEARCH);
	return 1;
}

stock Character_OffDuty(playerid)
{
	if(!Character_IsFactionDuty(playerid))
		return 1;
	
	Character_SetFactionDuty(playerid, false);
	
	SetPlayerSkin(playerid, Character_GetSkin(playerid));
	
	Character_ResetWeapons(playerid);

	AC_SetPlayerHealth(playerid, 100.0);
	AC_SetPlayerArmour(playerid, 0.0);

	Character_SetSelectingUniform(playerid, false);
	
	new rankName[16];
	Faction_GetRankName(Character_GetFaction(playerid), Character_GetRank(playerid), rankName);

	new String:message = str_format("(( ** HQ: %s %s non è più in servizio. ))", rankName, Character_GetOOCName(playerid));

	Faction_SendMessageStr(Character_GetFaction(playerid), message);

	SetPlayerColor(playerid, -1);

	CallLocalFunction(#OnCharacterDutyChange, "dd", playerid, 0);
	return 1;
}

flags:vsign(CMD_POLICE);
CMD:vsign(playerid, params[])
{
	new vehicleid = GetPlayerVehicleID(playerid);

	if(vehicleid <= 0)return SendClientMessage(playerid, COLOR_ERROR, "Non sei in un veicolo.");

	if(Vehicle_HasSign(vehicleid))
	{
 		Vehicle_RemoveSign(vehicleid);
	}
	else
	{
		new text[32];
		new pos;

		if(sscanf(params, "ds[32]", pos, text))return SendClientMessage(playerid, COLOR_GREY, "/vsign <pos. (0 - 2)> <testo>");

		if(pos < 0 || pos > 2)return SendClientMessage(playerid, COLOR_ERROR, "La posizione è un valore tra 0 e 2.");

		Vehicle_SetSign(vehicleid, pos, text);
	}
	return 1;
}