flags:servizio(CMD_ALIVE_USER);
CMD:servizio(playerid, params[])
{
	new factionid = Character_GetFaction(playerid);
	if(factionid == INVALID_FACTION_ID || Faction_GetType(factionid) != FACTION_TYPE_POLICE)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando.");
	
	if(!Character_IsAlive(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
	
	new buildingid = Character_GetNearBuilding(playerid, true);
	if(buildingid != -1 && Building_GetFaction(buildingid) == factionid)
	{
		if(!Character_IsFactionDuty(playerid))
		{
			Character_SetFactionDuty(playerid, true);
			Character_ResetWeapons(playerid);

			AC_SetPlayerHealth(playerid, 100.0);
			AC_SetPlayerArmour(playerid, Faction_GetType(factionid) == FACTION_TYPE_POLICE ? 100.0 : 0.0);

			SetPlayerColor(playerid, COLOR_SLATEBLUE);
			//Character_SetSelectingUniform(playerid, true);
			new rankName[16];
			Faction_GetRankName(factionid, Character_GetRank(playerid), rankName);
			new String:message = str_format("** HQ: %s %s è entrato in servizio.", rankName, Character_GetOOCName(playerid));
			Faction_SendMessageStr(factionid, COLOR_SLATEBLUE, message);
			SendClientMessage(playerid, COLOR_GREEN, "Utilizza /uniforme <id> per cambiare uniforme.");
			
		} else Character_OffDuty(playerid);
	}
	else
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'interno dell'edificio della tua fazione.");
	return 1;
}

flags:uniforme(CMD_ALIVE_USER);
CMD:uniforme(playerid, params[])
{
	new fid = Character_GetFaction(playerid);
	if(fid == -1)
		return SendClientMessage(playerid, COLOR_ERROR, "Non fai parte di una fazione.");
	if(!Character_IsFactionDuty(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Puoi utilizzare questo comando solo se in servizio.");

	new uniform_id,
		List:factionSkins = Faction_GetSkins(fid);
	if(list_size(factionSkins) <= 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non ci sono uniformi disponibili.");
	if(sscanf(params, "d", uniform_id))
	{
		SendClientMessage(playerid, COLOR_ERROR, "/uniforme <id uniforme>");
		SendClientMessage(playerid, -1, "== {00FF00}Uniformi Disponibili{FFFFFF} ==");
		new String:str;
		for(new i = 0, j = list_size(factionSkins); i < j; ++i)
		{
			new skinid = list_get(factionSkins, i);
			if(skinid != 0)
			{
				str += str_format("{00FF00}%d{FFFFFF}", skinid);
				if(i < j)
					str += @(", ");
			}
		}
		SendClientMessageStr(playerid, -1, str);
	}
	else
	{
		if(list_find(factionSkins, uniform_id) == -1)
			return SendClientMessage(playerid, COLOR_ERROR, "L'uniforme inserita non esiste.");
		SetPlayerSkin(playerid, uniform_id);
		SendFormattedMessage(playerid, COLOR_GREEN, "Hai selezionato la nuova uniforme. ID: %d.", uniform_id);
	}
	return 1;
}
