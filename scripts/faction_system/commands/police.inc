#include <YSI_Coding\y_hooks>

ptask OnPlayerUniformSelectUpdate[1000](playerid)
{
	if(!Character_IsLogged(playerid) || !Character_IsSelectingUniform(playerid))
		return 1;
    new keys, updown, leftright;
    GetPlayerKeys(playerid, keys, updown, leftright);
	if(leftright == KEY_LEFT)
	{
		pSelectedUniformSlot{playerid}--;
	}
	return 1;
}

flags:ammanetta(CMD_POLICE);
CMD:ammanetta(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_ERROR, "/ammanetta <playerid/partofname>");
	
	if(!Character_IsLogged(id) || id == playerid)
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
		return SendClientMessage(playerid, COLOR_ERROR, "/arresta <playerid/partofname> <minuti>");
	if(id == playerid || !Character_IsLogged(id) || IsPlayerNPC(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");
	if(Character_GetFaction(id) != INVALID_FACTION_ID && Faction_GetType(Character_GetFaction(id)) == Faction_GetType(Character_GetFaction(playerid)))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi arrestare questo giocatore.");
	if(time < 0 || time > 720)
		return SendClientMessage(playerid, COLOR_ERROR, "Il tempo di arresto va da 0 a 720.");
	if(!IsPlayerInRangeOfPlayer(playerid, id, 4.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
	if(!IsPlayerNearAnyArrestPos(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei al punto di arresto.");

	Character_SetJailTime(id, time);
	Character_SetICJailed(id, true);

	Character_SetToJailPos(id);

	SendFormattedMessage(id, -1, "Sei stato arrestato da %s per {FF0000}%d{FFFFFF} minuti.", Character_GetOOCName(playerid));

	new rankName[16];
	Faction_GetRankName(Character_GetFaction(playerid), Character_GetRank(playerid), rankName);

	new String:message = str_format("(( HQ: %s %s ha arrestato %s per %d minuti. ))", rankName, Character_GetOOCName(playerid), Character_GetOOCName(id), time);

	foreach(new i : Player)
	{
		if(Character_GetFaction(i) != INVALID_FACTION_ID && Character_GetFaction(i) == Character_GetFaction(playerid) && Character_IsFactionDuty(i))
		{
			SendClientMessageStr(i, COLOR_BLUE, message);
		}
	}
	Log(Character_GetOOCName(playerid), Character_GetOOCName(id), "/arresta", time);
	return 1;
}

flags:trascina(CMD_POLICE);
CMD:trascina(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_ERROR, "/trascina <playerid/partofname>");
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

	if(!Character_CanInteract(playerid))
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
	new factionid = Character_GetFaction(playerid);
	if(!Account_IsAdminDuty(playerid))
	{
		if(factionid == INVALID_FACTION_ID || (Faction_GetType(factionid) != FACTION_TYPE_POLICE && Faction_GetType(factionid) != FACTION_TYPE_MEDICAL && !Account_IsAdminDuty(playerid)))
			return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando.");
		if(!Character_IsFactionDuty(playerid))
			return SendClientMessage(playerid, COLOR_ERROR, "Non sei in servizio.");
	}
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_ERROR, "/cura <playerid/partofname>");
	if( (!Account_IsAdminDuty(playerid) && id == playerid) || !IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");
	if(!IsPlayerInRangeOfPlayer(playerid, id, 5.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
	if(!Character_IsInjured(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non ha bisogno di cure.");
	if(Character_GetCarePlayer(id) != INVALID_PLAYER_ID)
		return SendClientMessage(playerid, COLOR_ERROR, "Questo giocatore ha già una richiesta di cure.");
	if(GetTickCount() - Character_GetCareTicks(playerid) < 1000*15)
		return SendClientMessage(playerid, COLOR_ERROR, "Puoi inviare una richiesta di cure ogni quindici secondi.");
	
	Character_SetCarePlayer(id, playerid);
	Character_SetCareTick(playerid, GetTickCount());

	SendFormattedMessage(id, COLOR_YELLOW, "%s vuole curarti. Digita \"/accetta cure\" per accettare.", Character_GetRolePlayName(playerid));
	SendFormattedMessage(playerid, COLOR_YELLOW, "Richiesta di cure inviata a %s.", Character_GetRolePlayName(id));
	return 1;
}

flags:perquisisci(CMD_ALIVE_USER); //flags:perquisisci(CMD_POLICE);
CMD:perquisisci(playerid, params[])
{
	if(!Character_CanInteract(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
	new id;
	if(sscanf(params, "k<u>d", id))
		return SendClientMessage(playerid, COLOR_ERROR, "/perquisisci <playerid/partofname/#id>");

	if(id == playerid)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su te stesso.");

	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");

	if(!IsPlayerInRangeOfPlayer(playerid, id, 5.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
	
	SendFormattedMessage(id, COLOR_RECEIVE_REQUEST, "%s (%d) vuole perquisirti. Digita \"/accetta perquisizione\" per accettare o \"/annulla\" per rifiutare.", Character_GetRolePlayName(playerid), playerid);
	SendFormattedMessage(playerid, COLOR_SEND_REQUEST, "Richiesta di perquisizione inviata a %s (%d).", Character_GetRolePlayName(id), id);

	Character_SetRequest(playerid, id, REQUEST_TYPE_SEARCH);
	return 1;
}

stock Character_AcceptSearch(playerid)
{
	if(!Request_IsPending(playerid) || Request_GetType(playerid) != REQUEST_TYPE_SEARCH)
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai una richiesta di perquisizione attiva."), 0;
    if(!IsPlayerInRangeOfPlayer(playerid, Request_GetSender(playerid), 5.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore."), 0;
	new senderid = Request_GetSender(playerid);

	SendFormattedMessage(senderid, COLOR_ACCEPT_REQUEST, "%s (%d) ha accettato la tua richiesta di perquisizione.", Character_GetRolePlayName(playerid), playerid);
	SendFormattedMessage(playerid, COLOR_ACCEPT_REQUEST, "Hai accettato la richiesta di perquisizione di %s (%d).", Character_GetRolePlayName(senderid), senderid);

	new str[64], Inventory:inv = Character_GetInventory(playerid);
	format(str, sizeof(str), "Inventario di %s (%d/%d)",  Character_GetRolePlayName(playerid), Inventory_GetUsedSpace(inv), Inventory_GetSpace(inv));
	Inventory_ShowInChat(inv, senderid, str);
	SendFormattedMessage(senderid, -1, "Denaro: $%d", Character_GetMoney(playerid));
	return 1;
}

stock Character_OffDuty(playerid)
{
	if(!Character_IsFactionDuty(playerid))
		return 1;
	
	Character_SetFactionDuty(playerid, false);
	
	if(pTempSkin[playerid] != 0)
		SetPlayerSkin(playerid, pTempSkin[playerid]);
	
	Character_ResetWeapons(playerid);

	AC_SetPlayerHealth(playerid, 100.0);
	AC_SetPlayerArmour(playerid, 0.0);

	Character_SetSelectingUniform(playerid, false);

	pTempSkin[playerid] = 0;
	
	new rankName[16];
	Faction_GetRankName(Character_GetFaction(playerid), Character_GetRank(playerid), rankName);

	new String:message = str_format("(( ** HQ: %s %s non è più in servizio. ))", rankName, Character_GetOOCName(playerid));

	Faction_SendMessageStr(Character_GetFaction(playerid), COLOR_SLATEBLUE, message);

	SetPlayerColor(playerid, -1);

	CallLocalFunction(#OnCharacterDutyChange, "dd", playerid, 0);
	return 1;
}