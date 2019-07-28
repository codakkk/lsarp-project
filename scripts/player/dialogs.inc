
Dialog:Dialog_House(playerid, response, listitem, inputtext[])
{
    if(!response)
	   return 0;
    
	new houseid = Character_GetNearHouseIDMenu(playerid);

	if(!Character_IsNearPickupTypes(playerid, ELEMENT_TYPE_HOUSE_ENTRANCE) && houseid == -1)
	   return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata o all'interno della tua casa.");

	switch(listitem)
	{
		case 0: // Apri/Chiudi Porta
		{
			House_SetLocked(houseid, !House_IsLocked(houseid));
			if(House_IsLocked(houseid))
				SendClientMessage(playerid, COLOR_GREEN, "Hai chiuso la porta della casa.");
			else
				SendClientMessage(playerid, COLOR_GREEN, "Hai aperto la porta della casa.");
			House_Save(houseid);
		}
		case 1: // Show Inv
		{
			House_ShowInventory(houseid, playerid);
		}
		case 2: // Deposita Soldi
		{
			return Dialog_Show(playerid, Dialog_HouseDeposit, DIALOG_STYLE_INPUT, "Deposita soldi", "Inserisci l'ammontare di soldi che vuoi depositare in casa.", "Deposita", "Annulla");
		}
		case 3: // Ritira Soldi
		{
			return Dialog_Show(playerid, Dialog_HouseWithdraw, DIALOG_STYLE_INPUT, "Ritira soldi", "Inserisci l'ammontare di soldi che vuoi ritirare dalla casa.\n{00FF00}Cassa: $%d{FFFFFF}", "Ritira", "Annulla", House_GetMoney(houseid));
		}
		case 4: // Vendi 
		{
			return Dialog_Show(playerid, Dialog_HouseSell, DIALOG_STYLE_MSGBOX, "Vendi", "Sei sicuro di voler vendere la tua casa per {00FF00}$%d{FFFFFF}\nUna volta venduta, tutti i mobili verranno rimossi.", "Vendi", "Annulla", House_GetPrice(houseid)/2);
		}
		case 5: // Vendi a giocatore
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non ancora disponibile.");
			return 1;
		}
		case 6: // Cambia interior
		{
			new String:string, interiorid = House_GetInteriorID(houseid);
			for(new i = 0, sz = sizeof(allHouseInteriors); i < sz; ++i)
			{
				if(allHouseInteriors[interiorid][iType] != allHouseInteriors[i][iType])
				string += @("{FF0000}");
				string += str_format("Interno %d", i);
				string += @("{FFFFFF}\n");
			}
			return Dialog_Show_s(playerid, Dialog_HouseInterior, DIALOG_STYLE_LIST, @("Cambio Interior"), string, "Vendi", "Annulla");
		}
	}
    return 1;
}

Dialog:Dialog_HouseDeposit(playerid, response, listitem, inputtext[])
{
    if(!response)
	   return 0;
    new houseid = Character_GetNearHouseIDMenu(playerid);
	if(houseid == -1)
		return 0;
    new amount = strval(inputtext);
    if(amount <= 0 || amount > Character_GetMoney(playerid))
	   return Dialog_Show(playerid, Dialog_HouseDeposit, DIALOG_STYLE_INPUT, "Deposita soldi", "{FF0000}Ammontare non valido!\n{FFFFFF}Inserisci l'ammontare di soldi che vuoi depositare in casa.", "Deposita", "Annulla");
    Character_GiveMoney(playerid, -amount, "house_deposit");
	House_GiveMoney(houseid, amount);
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai depositato $%d all'interno della tua casa.", amount);
    SendFormattedMessage(playerid, COLOR_GREEN, "Attuale: $%d", House_GetMoney(houseid));
	House_Save(houseid);
    return 1;
}

Dialog:Dialog_HouseWithdraw(playerid, response, listitem, inputtext[])
{
    if(!response)
	   return 0;
	new houseid = Character_GetNearHouseIDMenu(playerid);
	if(houseid == -1)
		return 0;
    new amount = strval(inputtext);
    if(amount <= 0 || amount > House_GetMoney(houseid))
	   return Dialog_Show(playerid, Dialog_HouseWithdraw, DIALOG_STYLE_INPUT, "Ritira soldi", "{FF0000}Ammontare non valido!\n{FFFFFF}Inserisci l'ammontare di soldi che vuoi ritirare dalla casa.\n{00FF00}Cassa: $%d{FFFFFF}", "Ritira", "Annulla", House_GetMoney(houseid));
    House_GiveMoney(houseid, -amount);
    Character_GiveMoney(playerid, amount, "Dialog_HouseWithdraw");
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai ritirato $%d dalla tua casa.", amount);
    SendFormattedMessage(playerid, COLOR_GREEN, "Attuale: $%d", House_GetMoney(houseid));
	House_Save(houseid);
    return 1;
}

Dialog:Dialog_HouseSell(playerid, response, listitem, inputtext[])
{
    if(!response)
	   return Character_ShowHouseDialog(playerid);
	new houseid = Character_GetNearHouseIDMenu(playerid);
	printf("House ID: %d", houseid);
	if(houseid == -1)
		return 0;
    new sellPrice = House_GetPrice(houseid) / 2;
    Character_GiveMoney(playerid, sellPrice, "Dialog_HouseSell");
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai venduto la tua casa per $%d.", sellPrice);

	House_RemoveAllfurnitures(houseid);
    House_ResetOwner(houseid);
    House_Save(houseid);
    return 1;
}

Dialog:Dialog_HouseInterior(playerid, response, listitem, inputtext[])
{
    if(!response)
	   return 0;
	new houseid = Character_GetNearHouseIDMenu(playerid);
	if(houseid == -1)
		return 0;
	if(!IsPlayerInRangeOfHouseEntrance(playerid, houseid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata della tua casa.");
	if(allHouseInteriors[House_GetInteriorID(houseid)][iType] != allHouseInteriors[listitem][iType])
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi selezionare questo interno per questa casa.");
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai cambiato l'interno della tua casa. Nuovo interno: %d.", listitem);
    House_SetInterior(houseid, listitem);
    House_Save(houseid);
    return 1;
}
