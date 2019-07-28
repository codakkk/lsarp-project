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

			pTempSkin[playerid] = GetPlayerSkin(playerid);
			SetPlayerColor(playerid, COLOR_SLATEBLUE);
			//Character_SetSelectingUniform(playerid, true);
			Faction_SendMessageStr(factionid, COLOR_SLATEBLUE, str_format("** HQ: %S %s è entrato in servizio.", Faction_GetRankNameStr(factionid, Character_GetRank(playerid)-1), Character_GetOOCName(playerid)));
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
	
	/*if(Character_IsSelectingUniform(playerid))
	{
		Character_SetSelectingUniform(playerid, false);
		Character_SetFreezed(playerid, false);
		SendFormattedMessage(playerid, COLOR_GREEN, "Hai selezionato la nuova uniforme. ID: %d.", Faction_GetSkin(fid, pSelectedUniformSlot{playerid}));
		return 1;
	}*/

	new uniform_id;
	if(sscanf(params, "d", uniform_id))
	{
		/*Character_SetSelectingUniform(playerid, true);
		Character_SetFreezed(playerid, true);
		pSelectedUniformSlot{playerid} = 0;
		SendClientMessage(playerid, COLOR_GREEN, "Usa Freccia Sx o Freccia Dx per selezionare la skin. Premi LALT (/uniforme) per confermare.");*/
		SendClientMessage(playerid, COLOR_ERROR, "/uniforme <id uniforme>");
		SendClientMessage(playerid, -1, "== {00FF00}Uniformi Disponibili{FFFFFF} ==");
		new String:str;
		for(new i = 0; i < MAX_FACTION_SKINS; ++i)
		{
			new skinid = Faction_GetSkin(fid, i);
			if(skinid != 0)
			{
				str += str_format("{00FF00}%d{FFFFFF}", skinid);
				if(i < MAX_FACTION_SKINS-1)
					str += @(", ");
			}
		}
		SendClientMessageStr(playerid, -1, str);
	}
	else
	{
		if(uniform_id == 0)
			return SendClientMessage(playerid, COLOR_ERROR, "L'uniforme inserita non è valida.");
		for(new i = 0; i < MAX_FACTION_SKINS; ++i)
		{
			if(FactionSkins[fid][i] == uniform_id)
			{
				pTempSkin[playerid] = GetPlayerSkin(playerid);
				SetPlayerSkin(playerid, uniform_id);
				SendFormattedMessage(playerid, COLOR_GREEN, "Hai selezionato la nuova uniforme. ID: %d.", uniform_id);
				return 1;
			}
		}
		SendClientMessage(playerid, COLOR_ERROR, "L'uniforme inserita non è valida.");
	}
	return 1;
}
