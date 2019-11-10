#include <YSI_Coding\y_hooks>

flags:fcmds(CMD_USER);
CMD:fcmds(playerid, params[])
{
	SendClientMessage(playerid, COLOR_SLATEBLUE, "/flascia - /servizio - /uniforme - /fcaccia - /finvita - /frank - /fmembri");
	new factionid = Character_GetFaction(playerid);
	if(Faction_IsValid(factionid))
	{
		if(Faction_GetType(factionid) == FACTION_TYPE_POLICE)
		{
			SendClientMessage(playerid, COLOR_SLATEBLUE, "/manette - /trascina - /arresta - (/m)egafono - /vsign");
			SendClientMessage(playerid, COLOR_SLATEBLUE, "/perquisisci - /servizio - (/d)ipartimento - /traccia");
			SendClientMessage(playerid, COLOR_SLATEBLUE, "/ritiralicenza - /daiportodarmi - /beanbag - /taser - /mdc");
		}
		else if(Faction_GetType(factionid) == FACTION_TYPE_GOVERNMENT)
		{
			SendClientMessage(playerid, COLOR_SLATEBLUE, "/gov");
		}
	}
	return 1;
}

flags:servizio(CMD_ALIVE_USER);
CMD:servizio(playerid, params[])
{
	if(Player_IsAdminDuty(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando in admin duty.");
		
	new factionid = Character_GetFaction(playerid);
	if(!Faction_IsValid(factionid) || (Faction_GetType(factionid) != FACTION_TYPE_POLICE && Faction_GetType(factionid) != FACTION_TYPE_MEDICAL))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando.");
	
	if(!Character_IsAlive(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
	
	new Building:building = Character_GetBuilding(playerid, true);
	if(Building_IsValid(building) && Building_GetFaction(building) == factionid)
	{
		if(!Character_IsFactionDuty(playerid))
		{
			Character_SetMasked(playerid, false);
			Character_SetFactionDuty(playerid, true);
			Character_ResetWeapons(playerid);

			Character_SetHealth(playerid, 100.0);

			SetPlayerColor(playerid, COLOR_SLATEBLUE);
			//Character_SetSelectingUniform(playerid, true);
			new rankName[16];
			Faction_GetRankName(factionid, Character_GetRank(playerid), rankName);
			new String:message = str_format("(( ** HQ: %s %s è entrato in servizio. ))", rankName, Character_GetOOCName(playerid));
			Faction_SendMessageStr(factionid, message);
			SendClientMessage(playerid, COLOR_GREEN, "Utilizza /uniforme <id> per cambiare uniforme.");
			
			Faction_GiveWeapons(playerid);

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
	if(!Faction_IsValid(fid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non fai parte di una fazione.");
	
	if(!Character_IsFactionDuty(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Puoi utilizzare questo comando solo se in servizio.");

	new uniform_id,
		List:factionSkins = Faction_GetSkins(fid);

	if(list_size(factionSkins) <= 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non ci sono uniformi disponibili.");
	if(sscanf(params, "d", uniform_id))
	{
		SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /uniforme <id uniforme>");
		SendClientMessage(playerid, -1, "== {00FF00}Uniformi Disponibili{FFFFFF} ==");
		new String:str;
		for(new i = 0, j = list_size(factionSkins); i < j; ++i)
		{
			new skinid = list_get(factionSkins, i);
			if(skinid != 0)
			{
				SendFormattedMessage(playerid, COLOR_GREEN, "Skin: %d", skinid);
			}
			/*if(skinid != 0)
			{
				str += str_format("{00FF00}%d{FFFFFF}", skinid);
				if(i < j)
					str += @(", ");
			}*/
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

flags:fcaccia(CMD_USER);
CMD:fcaccia(playerid, params[])
{
	if(!Faction_IsValid(Character_GetFaction(playerid)) || Character_GetRank(playerid) > 3)
		return 0;
	
	new name[24];
	if(sscanf(params, "s[24]", name))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /fcaccia <nome>");
	
	inline OnQuery()
	{
		new rows = cache_num_rows();

		if(rows <= 0)
			return SendClientMessage(playerid, COLOR_ERROR, "Il nome inserito non è valido.");
		
		new id, tmpName[MAX_PLAYER_NAME];

		cache_get_value_name_int(0, "id", id);
		cache_get_value_name(0, "name", tmpName);

		if(Character_GetID(playerid) == id)
			return SendClientMessage(playerid, COLOR_ERROR, "Il nome inserito non è valido.");

		foreach(new i : Player)
		{
			if(i == playerid || Character_GetID(i) != id)
				continue;
			
			new fname[MAX_FACTION_NAME];
			Faction_GetName(Character_GetFaction(playerid), fname);

			SendFormattedMessage(i, COLOR_ORANGE, "%s ti ha cacciato dalla fazione %s.", Character_GetOOCName(playerid), fname);

			Character_OffDuty(i);
			
			Character_SetFaction(i, INVALID_FACTION_ID);
			Character_SetRank(i, 0);

			break;
		}

		SendFormattedMessage(playerid, COLOR_ORANGE, "Hai cacciato %s dalla tua fazione.", tmpName);
	}

	MySQL_TQueryInline(gMySQL, using inline OnQuery, "SELECT id, name FROM `characters` WHERE name = '%e' AND faction = '%d'", name, Character_GetFaction(playerid));
	return 1;
}

flags:frank(CMD_USER);
CMD:frank(playerid, params[])
{
	if(!Faction_IsValid(Character_GetFaction(playerid)) || Character_GetRank(playerid) > 3)
		return 0;
	
	new id, rank;
	if(sscanf(params, "k<u>d", id, rank))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /frank <playerid/partofname/#####> <rank>");
	
	if(!Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");
	
	if(Character_GetFaction(playerid) != Character_GetFaction(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non fa parte della tua fazione.");
	
	if(Character_GetRank(playerid) >= Character_GetRank(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi settare il rank di questo membro.");

	if(!Faction_HasRank(Character_GetFaction(playerid), rank))
		return SendClientMessage(playerid, COLOR_ERROR, "Il rank inserito non è valido.");
	
	if(Character_GetRank(playerid) >= rank)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi settare questo giocatore un rank maggiore o uguale al tuo.");

	new rname[MAX_FACTION_RANK_NAME];
	Faction_GetRankName(Character_GetFaction(playerid), rank, rname);

	SendFormattedMessage(id, COLOR_ORANGE, "%s ti ha settato rank %d (%s).", Character_GetRolePlayName(playerid), rank, rname);
	SendFormattedMessage(playerid, COLOR_ORANGE, "Hai settato %s rank %d (%s).", Character_GetRolePlayName(id), rank, rname);

	Character_SetRank(id, rank);
	return 1;
}

flags:flascia(CMD_USER);
CMD:flascia(playerid, params[])
{
	if(!Faction_IsValid(Character_GetFaction(playerid)))
		return SendClientMessage(playerid, COLOR_ERROR, "Non fai parte di una fazione.");
	
	new fname[MAX_FACTION_NAME];
	Faction_GetName(Character_GetFaction(playerid), fname);
	
	Character_OffDuty(playerid);
	Character_SetFaction(playerid, INVALID_FACTION_ID);
	Character_SetRank(playerid, 0);

	SendFormattedMessage(playerid, COLOR_ORANGE, "Hai lasciato la fazione %s.", fname);
	return 1;
}

flags:finvita(CMD_USER);
CMD:finvita(playerid, params[])
{
	if(!Faction_IsValid(Character_GetFaction(playerid)) || Character_GetRank(playerid) > 3)
		return 0;
	
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /finvita <playerid/partofname/#####>");
	
	if(!Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");
	
	if(Faction_IsValid(Character_GetFaction(id)))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore fa già parte di una fazione.");
	
	Character_SetRequest(playerid, id, REQUEST_TYPE_FACTION, Character_GetFaction(playerid));

	new fname[MAX_FACTION_NAME];
	Faction_GetName(Character_GetFaction(playerid), fname);

	SendFormattedMessage(id, COLOR_RECEIVE_REQUEST, "%s ti ha invitato nella sua fazione (%s).", Character_GetRolePlayName(playerid), fname);
    SendClientMessage(id, COLOR_RECEIVE_REQUEST, "Digita \"/accetta fazione\" per accettare o \"/rifiuta fazione\" per rifiutare.");
    
    SendFormattedMessage(playerid, COLOR_SEND_REQUEST, "Hai inviato l'invito di fazione a %s.", Character_GetRolePlayName(id));
	SendClientMessage(playerid, COLOR_SEND_REQUEST, "Digita \"/annulla fazione\" per annullare.");
	return 1;
}

hook OnCharacterTryAccept(playerid, type)
{
    if(type != REQUEST_TYPE_FACTION)
		return 0;
	
	if(Faction_IsValid(Character_GetFaction(playerid)))
		return SendClientMessage(playerid, COLOR_ERROR, "Fai già parte di una fazione.");
	
	new senderid = Request_GetSender(playerid, REQUEST_TYPE_FACTION);

	if(!Faction_IsValid(Character_GetFaction(senderid)))
		return SendClientMessage(playerid, COLOR_ERROR, "La fazione non esiste più o il giocatore che ti ha invitato non ne fa piu' parte.");
	
	new factionid = Character_GetFaction(senderid);

	Character_SetFaction(playerid, factionid);
	Character_SetRank(playerid, Faction_GetLastValidRank(factionid));

	new fname[MAX_FACTION_NAME];
	Faction_GetName(factionid, fname);

	SendFormattedMessage(playerid, COLOR_ORANGE, "Hai accettato l'invito di %s. Adesso fai parte della fazione %s.", Character_GetRolePlayName(senderid), fname);
	SendFormattedMessage(senderid, COLOR_ORANGE, "%s ha accettato il tuo invito in fazione.", Character_GetRolePlayName(playerid));
	return 1;
}

static CurrentPage[MAX_PLAYERS char] = {0, ...};

flags:fmembri(CMD_USER);
CMD:fmembri(playerid, params[])
{
	if(!Faction_IsValid(Character_GetFaction(playerid)) || Character_GetRank(playerid) > 3)
		return 0;
	
	ShowMembersPage(playerid, 0);
	return 1;
}

static ShowMembersPage(playerid, page)
{
	if(page < 0)
		return 0;
	
	static const MEMBERS_PER_PAGE = 15;

	CurrentPage{playerid} = page;
	inline OnLoad()
	{
		new 
			String:string = @("Nome\tStato\n"),
			bool:isOnline,
			id,
			name[MAX_PLAYER_NAME], 
			rows = cache_num_rows();
		
		if(rows <= 0)
			return SendClientMessage(playerid, COLOR_ERROR, "Non c'è più nulla da mostrare.");

		string += @(">> Pagina\tSuccessiva\n");
		string += @("<< Pagina\tPrecedente\n");
		for(new i = 0; i < rows; ++i)
		{
			cache_get_value_name_int(i, "id", id);
			cache_get_value_name(i, "name", name);
			
			isOnline = false;

			foreach(new p : Player)
			{
				if(Character_GetID(p) == id)
				{
					isOnline = true;
					break;
				}
			}

			string += str_format("%s\t%s\n", name, isOnline ? ("{00FF00}Online{FFFFFF}") : ("{FF0000}Offline{FFFFFF}"));
		}
		Dialog_Show_s(playerid, DialogFactionMembers, DIALOG_STYLE_TABLIST_HEADERS, str_format("Lista Membri (Pagina %d)", page+1), string, "Seleziona", "Chiudi");
	}

	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT id, name FROM `characters` WHERE faction = '%d' LIMIT %d, %d", Character_GetFaction(playerid), page * MEMBERS_PER_PAGE, MEMBERS_PER_PAGE);
	return 1;
}

Dialog:DialogFactionMembers(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;
	if(listitem == 0)
	{
		ShowMembersPage(playerid, CurrentPage{playerid}+1);
	}
	else if(listitem == 1)
	{
		ShowMembersPage(playerid, CurrentPage{playerid}-1);
	}
	return 1;
}

static Faction_GiveWeapons(playerid)
{
	if(Faction_GetType(Character_GetFaction(playerid)) == FACTION_TYPE_POLICE)
	{
		Character_GiveWeapon(playerid, WEAPON_NITESTICK, 1);
		Character_GiveWeapon(playerid, WEAPON_SPRAYCAN, 1000);
		Character_GiveWeapon(playerid, WEAPON_DEAGLE, 100);
		Character_SetArmour(playerid, 100.0);
	}
}