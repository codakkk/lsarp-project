Dialog:Dialog_FactionList(playerid, response, listitem, inputtext[])
{
	if(!response || AccountInfo[playerid][aAdmin] < 5)
		return 0;
	return Faction_ShowOptions(listitem, playerid);
}

Dialog:Dialog_FactionActions(playerid, response, listitem, inputtext[])
{
	if(!response || AccountInfo[playerid][aAdmin] < 5)
		return Faction_ShowList(playerid);
	new factionid = pAdminSelectedFaction{playerid};
	switch(listitem)
	{
		case 0: // Modify Name
		{
			return Dialog_Show(playerid, Dialog_ChangeFactionName, DIALOG_STYLE_INPUT, "Modifica Nome", "{FFFFFF}Inserisci il nuovo nome della fazione!\n{00FF00}Lunghezza Minima: 4.\n{00FF00}Lunghezza Massima: 32.", "Cambia", "Annulla");
		}
		case 1: // Modifi Short Name
		{
			return Dialog_Show(playerid, ChangeFactionShortName, DIALOG_STYLE_INPUT, "Modifica Acronimo", "{FFFFFF}Inserisci il nuovo acronimo della fazione!\n{00FF00}Lunghezza Minima: 3.\n{00FF00}Lunghezza Massima: 8.", "Cambia", "Annulla");
		}
		case 2: // Modify Type
		{
			return Dialog_Show(playerid, ChangeFactionType, DIALOG_STYLE_INPUT, "Tipo Fazione", "Inserisci il nuovo tipo per la fazione\nTipo Attuale: %d.\nTipi: 0 - %d", "Modifica", "Annulla", Faction_GetType(factionid), FACTION_TYPE_LAST-1);
		}
		case 3:
		{
			return Faction_ShowRankList(factionid, playerid);
		}
		case 4:
		{
			return Faction_ShowSkinList(factionid, playerid);
		}
		case 5:
		{
			new String:title = str_format("%S", Faction_GetNameStr(factionid));
			return Dialog_Show_s(playerid, Dialog_DeleteFactionConfirm, DIALOG_STYLE_MSGBOX, title, @("{FF0000}Sei sicuro di voler rimuovere la fazione definitivamente?{FFFFFF}"), "Rimuovi", "Annulla");
		}
	}
	return 1;
}

Dialog:Dialog_ChangeFactionName(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Faction_ShowOptions(pAdminSelectedFaction{playerid}, playerid);
	if(strlen(inputtext) < 4 || strlen(inputtext) > 32)
		return Dialog_Show(playerid, Dialog_ChangeFactionName, DIALOG_STYLE_INPUT, "Modifica Nome", "{FF0000}Il nome inserito è troppo lungo o troppo corto!\n{FFFFFF}Inserisci il nuovo nome della fazione!\n{00FF00}Lunghezza Minima: 4.\n{00FF00}Lunghezza Massima: 32.", "Cambia", "Annulla");
	new fid = pAdminSelectedFaction{playerid};
	Faction_SetName(fid, inputtext);
	Faction_Save(fid);
	Faction_ShowOptions(fid, playerid);
	return 1;
}

Dialog:ChangeFactionShortName(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Faction_ShowOptions(pAdminSelectedFaction{playerid}, playerid);
	if(strlen(inputtext) <3 || strlen(inputtext) > 8)
		return Dialog_Show(playerid, ChangeFactionShortName, DIALOG_STYLE_INPUT, "Modifica Acronimo", "{FF0000}Il nome inserito è troppo lungo o troppo corto!\n{FFFFFF}Inserisci il nuovo nome della fazione!\n{00FF00}Lunghezza Minima: 3.\n{00FF00}Lunghezza Massima: 8.", "Cambia", "Annulla");
	new fid = pAdminSelectedFaction{playerid};
	Faction_SetShortName(fid, inputtext);
	Faction_Save(fid);
	Faction_ShowOptions(fid, playerid);
	return 1;
}

Dialog:ChangeFactionType(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Faction_ShowOptions(pAdminSelectedFaction{playerid}, playerid);
	new val = strval(inputtext),
		factionid = pAdminSelectedFaction{playerid};
	if(!IsNumeric(inputtext) || val < 0 || val >= FACTION_TYPE_LAST)
		return Dialog_Show(playerid, ChangeFactionType, DIALOG_STYLE_INPUT, "Tipo Fazione", "{FF0000}Il tipo inserito non è valido!\n{FFFFFF}Inserisci il nuovo tipo per la fazione\nTipo Attuale: %d.\nTipi: 0 - %d", "Modifica", "Annulla", Faction_GetType(factionid), FACTION_TYPE_LAST-1);
	Faction_SetType(factionid, val);
	SendMessageToAdmins(true, COLOR_ADMIN, "[ADM-CMD] %s (%d) ha modificato il tipo della fazione %d. Nuovo tipo: %d", AccountInfo[playerid][aName], playerid, factionid, val);
	return 1;
}


Dialog:Dialog_DeleteFactionConfirm(playerid, response, listitem, inputtext[])
{
	if(AccountInfo[playerid][aAdmin] < 5) return 0;
	if(!response)
		return Faction_ShowList(playerid);
	new factionid = pAdminSelectedFaction{playerid};
	SendMessageToAdmins(true, COLOR_YELLOW, "[ADMIN-ALERT]: %s (%d) ha rimosso la fazione %d (Tipo: %d).", 
	AccountInfo[playerid][aName], playerid, factionid, Faction_GetType(factionid));
	Faction_Reset(factionid);
	return Faction_ShowList(playerid);
}


Dialog:EditFactionRank(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Faction_ShowOptions(pAdminSelectedFaction{playerid}, playerid);
	pAdminSelectedFactionRank{playerid} = listitem;
	new String:info = str_format("%d (%S - $%d)", listitem, Faction_GetRankNameStr(pAdminSelectedFaction{playerid}, listitem), Faction_GetRankSalary(pAdminSelectedFaction{playerid}, listitem));
	Dialog_Show_s(playerid, EditFactionRankActions, DIALOG_STYLE_LIST, info, @("Modifica Nome\nModifica Salario"), "Modifica", "Indietro");
	return 1;
}

Dialog:EditFactionRankActions(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Faction_ShowOptions(pAdminSelectedFaction{playerid}, playerid);
	if(listitem == 0)
		Dialog_Show(playerid, EditFactionRankName, DIALOG_STYLE_INPUT, "Nome Rank", "Inserisci il nome del rank (Max 15 caratteri).", "Modifica", "Indietro");
	else if(listitem == 1)
		Dialog_Show(playerid, EditFactionRankSalary, DIALOG_STYLE_INPUT, "Salario Rank", "Inserisci il salario del rank.", "Modifica", "Indietro");
	return 1;
}

Dialog:EditFactionRankName(playerid, response, listitem, inputtext[])
{
	if(!response) return Faction_ShowOptions(pAdminSelectedFaction{playerid}, playerid);
	if(strlen(inputtext) > 15)
		return Dialog_Show(playerid, EditFactionRankName, DIALOG_STYLE_INPUT, "Nome Rank", "{FF0000}Il nome è troppo lungo!{FFFFFF}\nInserisci il nome del rank (Max 15 caratteri).", "Modifica", "Annulla");
	Faction_SetRankName(pAdminSelectedFaction{playerid}, pAdminSelectedFactionRank{playerid}, inputtext);
	Faction_Save(pAdminSelectedFaction{playerid});
	SendFormattedMessage(playerid, COLOR_GREEN, "Hai cambiato il nome del rank. Nuovo nome: %s", inputtext);
	return 1;
}

Dialog:EditFactionRankSalary(playerid, response, listitem, inputtext[])
{
	if(!response) return Faction_ShowOptions(pAdminSelectedFaction{playerid}, playerid);
	new salary = strval(inputtext);
	if(salary <= 0)
		return Dialog_Show(playerid, EditFactionRankSalary, DIALOG_STYLE_INPUT, "Salario Rank", "{FF0000}Il salario inserito non è valido!{FFFFFF}\nInserisci il salario del rank.", "Modifica", "Annulla");
	Faction_SetRankSalary(pAdminSelectedFaction{playerid}, pAdminSelectedFactionRank{playerid}, salary);
	Faction_Save(pAdminSelectedFaction{playerid});
	SendFormattedMessage(playerid, -1, "Hai cambiato il salario del rank. Nuovo salario: {85bb65}$%d{FFFFFF}", salary);
	return 1;
}


Dialog:Dialog_EditFactionSkin(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Faction_ShowOptions(pAdminSelectedFaction{playerid}, playerid);
	pAdminSelectedFactionSkin{playerid} = listitem;
	new String:title = str_format("Modifica Skin (Corrente: %d)", FactionSkins[pAdminSelectedFaction{playerid}][listitem]);
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
		new String:title = str_format("Modifica Skin (Corrente: %d)", FactionSkins[pAdminSelectedFaction{playerid}][pAdminSelectedFactionSkin{playerid}]);
		Dialog_Show_s(playerid, EditFactionSkinInsert, DIALOG_STYLE_INPUT, title, @("{FF0000}Skin non valida!{FFFFFF}\nInserisci l'ID della skin che vuoi inserire\nN.B: Skin 0 se vuoi rimuoverla."), "Modifica", "Annulla");
		return 1;
	}
	Faction_SetSkin(pAdminSelectedFaction{playerid}, pAdminSelectedFactionSkin{playerid}, v);
	Faction_SaveSkins(pAdminSelectedFaction{playerid});
	return Faction_ShowSkinList(pAdminSelectedFaction{playerid}, playerid);
}