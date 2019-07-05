#include <YSI_Coding\y_hooks>

static stock
	Text3D:pMaskText3D[MAX_PLAYERS];

hook OnPlayerClearData(playerid)
{
	Character_RemoveMask(playerid);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnCharacterInjured(playerid)
{
	Character_RemoveMask(playerid);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnCharacterSpawn(playerid)
{
	//if(Character_IsAlive(playerid))
	//{
	Character_RemoveMask(playerid);
	//}

	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerStreamIn(playerid, forplayerid)
{
	if(Character_IsMasked(playerid))
	{
		if(Account_IsAdminDuty(playerid))
		{
			SetPlayerMarkerForPlayer(forplayerid, playerid, (Account_IsAdminDuty(playerid)) ? 0xFFFF00AA : 0xFFFFFF00);
			ShowPlayerNameTagForPlayer(forplayerid, playerid, true);
		}
		else ShowPlayerNameTagForPlayer(forplayerid, playerid, false);
	}
	else ShowPlayerNameTagForPlayer(forplayerid, playerid, true);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

flags:masks(CMD_JR_MODERATOR);
CMD:masks(playerid, params[])
{
	new c = 0;
	foreach(new i : Player)
	{
		if(Character_IsLogged(i) && Character_IsMasked(playerid))
		{
			SendFormattedMessage(playerid, -1, "%s - %s - %d", Character_GetRolePlayName(i), Character_GetOOCName(i), i);
			c++;
		}
	}	
	if(c == 0)
		SendClientMessage(playerid, COLOR_ERROR, "Non ci sono giocatori mascherati.");
	return 1;
}

flags:mask(CMD_JR_MODERATOR);
CMD:mask(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_ERROR, "/mask <#codice>");
	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è connesso.");
	if(!Character_IsMasked(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è mascherato.");
	SendFormattedMessage(playerid, COLOR_GREEN, "Informazioni Maschera %07d", Character_GetID(id));
	SendFormattedMessage(playerid, COLOR_GREEN, "Nome: %s - ID: %d", Character_GetOOCName(id), id);
	return 1;
}

flags:sconosciuto(CMD_ALIVE_USER);
CMD:sconosciuto(playerid, params[])
{
	if(Account_IsAdminDuty(playerid))	
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
	if(Character_IsMasked(playerid))
	{
		Character_RemoveMask(playerid);
		Player_Info(playerid, "Non stai piu' indossando una maschera.", true);
	}
	else
	{
		Character_SetMasked(playerid, true);

		foreach(new i : Player)
		{
			if(!Character_IsLogged(i))
				continue;
			if(Account_IsAdminDuty(i))
			{
				SetPlayerMarkerForPlayer(i, playerid, 0xFFFF00AA);
				continue;
			}
			ShowPlayerNameTagForPlayer(i, playerid, false);
		}
		/*if(IsValidDynamic3DTextLabel(pMaskText3D[playerid]))
		{
			DestroyDynamic3DTextLabelEx(pMaskText3D[playerid]);
		}
		pMaskText3D[playerid] = CreateDynamic3DTextLabel(Character_GetRolePlayName(playerid), -1, 0, 0, 0.15, 30.0, playerid);*/

		Player_Info(playerid, "Stai indossando una maschera.", true);
		SendClientMessage(playerid, COLOR_GREEN, "Hai attivato lo sconosciuto.");
	}
	return 1;
}

stock Character_RemoveMask(playerid)
{
	Character_SetMasked(playerid, false);

	foreach(new i : Player)
	{
		if(!Character_IsLogged(i))
			continue;
		SetPlayerMarkerForPlayer(i, playerid, COLOR_WHITE);
		ShowPlayerNameTagForPlayer(i, playerid, true);
	}
}