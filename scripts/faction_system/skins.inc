
#include <YSI_Coding\y_hooks>

static 
    Map:FactionSkinsMap
;

hook OnGameModeInit()
{
    FactionSkinsMap = map_new();
	mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS faction_skins \
						(`faction_id` INT(11) NOT NULL, \
						`skin_id` INT(11) NOT NULL DEFAULT '0', \
						PRIMARY KEY (`faction_id`, `skin_id`), \
						CONSTRAINT `faction_skins` FOREIGN KEY (`faction_id`) REFERENCES `factions` (`id`) ON DELETE CASCADE) ENGINE=InnoDB DEFAULT CHARSET=latin1;");
    return 1;
}

hook OnFactionLoaded(factionid)
{
    Faction_LoadSkins(factionid);
    return 1;
}

hook OnFactionCreated(factionid)
{
    map_add(FactionSkinsMap, factionid, List:list_new());
    return 1;
}

hook OnFactionDeleted(factionid)
{
	map_remove_deep(FactionSkinsMap, factionid);
	return 1;
}

flags:afactionskin(CMD_RCON);
CMD:afactionskin(playerid, params[])
{
	new factionid, command[32], skinid;
	if(sscanf(params, "ds[32]D(-1)", factionid, command, skinid))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /fskin <factionid> <(a)dd/(r)emove/(l)ist>");
	
	if(!Faction_IsValid(factionid))
		return SendClientMessage(playerid, COLOR_ERROR, "La fazione inserita non esiste.");

	if(!strcmp(command, "add", true) || !strcmp(command, "a", true))
	{
		if(skinid <= -1)
			return SendFormattedMessage(playerid, COLOR_COMMAND_ERROR, "USO: /fskin %d add <skinid>", factionid);
		if(Faction_AddSkin(factionid, skinid))
			SendFormattedMessage(playerid, COLOR_GREEN, "Hai aggiunto la skin id %d alla fazione %d.", skinid, factionid);
		else
			SendClientMessage(playerid, COLOR_ERROR, "La skinid inserita è già utilizzata.");

	}
	else if(!strcmp(command, "remove", true) || !strcmp(command, "r", true))
	{
		if(skinid <= -1)
			return SendFormattedMessage(playerid, COLOR_COMMAND_ERROR, "USO: /fskin %d remove <skinid>", factionid);
		
		if(Faction_RemoveSkin(factionid, skinid))
			SendFormattedMessage(playerid, COLOR_GREEN, "Hai rimosso la skin id %d dalla fazione %d.", skinid, factionid);
		else
			SendFormattedMessage(playerid, COLOR_ERROR, "La fazione id %d non utilizza la skin id %d.", factionid, skinid);
	}
	else if(!strcmp(command, "list", true) || !strcmp(command, "l", true))
	{
		new 
			List:list = Faction_GetSkins(factionid),
			String:message = @("");
		for(new i = 0, j = list_size(list); i < j; ++i)
		{
			message += str_format("%d", list_get(list, i));
			if(i < j)
				message += @(", ");
		}
		SendClientMessageStr(playerid, -1, message);
	}

	return 1;
}
// Returns a PawnPlus List with all skins.
stock List:Faction_GetSkins(factionid)
{
    if(!Faction_IsValid(factionid))
        return List:0;
    return List:map_get(FactionSkinsMap, factionid);
}

stock Faction_GetSkin(factionid, index)
{
	if(!Faction_IsValid(factionid))
		return 0;
    new List:temp = Faction_GetSkins(factionid);
    if(!(0 <= index < list_size(temp)))
        return 0;
	return list_get(temp, index);
}

// Sets a faction skin. Pass skinid = 0 to remove it.
stock Faction_SetSkin(factionid, index, skinid)
{
	if(!Faction_IsValid(factionid))
		return 0;
    
    new List:temp = Faction_GetSkins(factionid);
    
    if(!(0 <= index < list_size(temp)))
        return 0;
    
    if(skinid == 0)
    {
        mysql_tquery_f(gMySQL, "DELETE FROM `faction_skins` WHERE skin_id = '%d' AND faction_id = '%d'", list_get(temp, index), Faction_GetID(factionid));    
        list_remove_deep(temp, index);
    }
    else
    {
        mysql_tquery_f(gMySQL, "UPDATE `faction_skins` SET skin_id = '%d' WHERE faction_id = '%d' AND skin_id = '%d'", skinid, Faction_GetID(factionid), list_get(temp, index));
        list_set(temp, index, skinid);
    }
    
	return 1;
}

stock Faction_RemoveSkin(factionid, skinid)
{
	if(!Faction_IsValid(factionid))
		return 0;
	new List:list = Faction_GetSkins(factionid), 
		index = list_find(list, skinid);
	if(index == -1)
		return 0;
	mysql_tquery_f(gMySQL, "DELETE FROM `faction_skins` WHERE skin_id = '%d' AND faction_id = '%d'", skinid, Faction_GetID(factionid));    
	list_remove_deep(list, index);
	return 1;
}

stock Faction_AddSkin(factionid, skinid)
{
	if(!Faction_IsValid(factionid))
		return 0;

	new List:skins = Faction_GetSkins(factionid);

	if(list_find(skins, skinid) != -1)
		return 0;
	
	list_add(skins, skinid);

	mysql_tquery_f(gMySQL, "INSERT INTO `faction_skins` (faction_id, skin_id) VALUES('%d', '%d')", factionid, skinid);
	return 1;
}

stock Faction_LoadSkins(factionid)
{
	inline OnLoad()
	{
		new 
            rows = cache_num_rows(),
            List:skins = list_new(),
            skinid = 0
        ;
		for(new x = 0; x < rows; ++x)
		{
            cache_get_value_index_int(x, 1, skinid);
            if(skinid > 0 && list_find(skins, skinid) == -1)
                list_add(skins, skinid);
		}
        if(!map_add(FactionSkinsMap, factionid, skins))
            printf("Unable to add FactionSkinsMap to map");
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `faction_skins` WHERE faction_id = '%d' ORDER BY skin_id", Faction_GetID(factionid));
}

stock Faction_ShowSkinList(factionid, playerid)
{
	if(!Faction_IsValid(factionid) || Account_GetAdminLevel(playerid) < 5)
		return 0;
	new 
		String:content = @("ID\tSkin ID\n"),
		List:factionSkinsList = Faction_GetSkins(factionid);
	
	for(new i = 0, j = list_size(factionSkinsList); i < j; ++i)
	{
		content += str_format("%d\t%d\n", i, list_get(factionSkinsList, i));
	}
	Dialog_Show_s(playerid, Dialog_EditFactionSkin, DIALOG_STYLE_TABLIST_HEADERS, @("Skins"), content, "Modifica", "Annulla");
	return 1;
}

Dialog:Dialog_EditFactionSkin(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Faction_ShowOptions(pAdminSelectedFaction{playerid}, playerid);
	pAdminSelectedFactionSkin{playerid} = listitem;
	new String:title = str_format("Modifica Skin (Corrente: %d)", Faction_GetSkin(pAdminSelectedFaction{playerid}, listitem));
	Dialog_Show_s(playerid, EditFactionSkinInsert, DIALOG_STYLE_INPUT, title, @("Inserisci l'ID della skin che vuoi inserire\nN.B: Skin 0 se vuoi rimuoverla."), "Modifica", "Annulla");
	return 1;
}

Dialog:EditFactionSkinInsert(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Faction_ShowSkinList(pAdminSelectedFaction{playerid}, playerid);
	new v = strval(inputtext);
	if(v < 0)
	{
		new String:title = str_format("Modifica Skin (Corrente: %d)", Faction_GetSkin(pAdminSelectedFaction{playerid}, pAdminSelectedFactionSkin{playerid}));
		Dialog_Show_s(playerid, EditFactionSkinInsert, DIALOG_STYLE_INPUT, title, @("{FF0000}Skin non valida!{FFFFFF}\nInserisci l'ID della skin che vuoi inserire\nN.B: Skin 0 se vuoi rimuoverla."), "Modifica", "Annulla");
		return 1;
	}
	if(!Faction_AddSkin(pAdminSelectedFaction{playerid}, v))
	{
		new String:title = str_format("Modifica Skin (Corrente: %d)", Faction_GetSkin(pAdminSelectedFaction{playerid}, pAdminSelectedFactionSkin{playerid}));
		Dialog_Show_s(playerid, EditFactionSkinInsert, DIALOG_STYLE_INPUT, title, @("{FF0000}Skin non valida!{FFFFFF}\nInserisci l'ID della skin che vuoi inserire\nN.B: Skin 0 se vuoi rimuoverla."), "Modifica", "Annulla");
	}
	Faction_SetSkin(pAdminSelectedFaction{playerid}, pAdminSelectedFactionSkin{playerid}, v);
	return Faction_ShowSkinList(pAdminSelectedFaction{playerid}, playerid);
}