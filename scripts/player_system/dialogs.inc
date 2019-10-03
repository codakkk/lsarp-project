
Dialog:Dialog_House(playerid, response, listitem, inputtext[])
{
    if(!response)
	   return 0;
    
	new House:house = Character_GetHouseMenu(playerid);

	if(!House_IsValid(house))
	   return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata o all'interno della tua casa.");

	switch(listitem)
	{
		case 0: // Apri/Chiudi Porta
		{
			House_SetLocked(house, !House_IsLocked(house));
			if(House_IsLocked(house))
				SendClientMessage(playerid, COLOR_GREEN, "Hai chiuso la porta della casa.");
			else
				SendClientMessage(playerid, COLOR_GREEN, "Hai aperto la porta della casa.");
		}
		case 1: // Show Inv
		{
			House_ShowInventory(house, playerid);
		}
		case 2: // Deposita Soldi
		{
			return Dialog_Show(playerid, Dialog_HouseDeposit, DIALOG_STYLE_INPUT, "Deposita soldi", "Inserisci l'ammontare di soldi che vuoi depositare in casa.", "Deposita", "Annulla");
		}
		case 3: // Ritira Soldi
		{
			return Dialog_Show(playerid, Dialog_HouseWithdraw, DIALOG_STYLE_INPUT, "Ritira soldi", "Inserisci l'ammontare di soldi che vuoi ritirare dalla casa.\n{00FF00}Cassa: $%d{FFFFFF}", "Ritira", "Annulla", House_GetMoney(house));
		}
		case 4: // Vendi 
		{
			return Dialog_Show(playerid, Dialog_HouseSell, DIALOG_STYLE_MSGBOX, "Vendi", "Sei sicuro di voler vendere la tua casa per {00FF00}$%d{FFFFFF}\nUna volta venduta, tutti i mobili verranno rimossi.", "Vendi", "Annulla", House_GetPrice(house)/2);
		}
		case 5: // Vendi a giocatore
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non ancora disponibile.");
			return 1;
		}
		case 6: // Cambia interior
		{
			if(Character_GetHouseID(playerid) > 0)
				return SendClientMessage(playerid, COLOR_ERROR, "Devi essere all'esterno della casa per poter cambiare interior.");
			
			new String:string, interiorType = House_GetInteriorType(house);
			
			new List:list = HouseInterior_GetListByType(interiorType);

			if(!list_valid(list) || list_size(list) <= 0)
				return SendClientMessage(playerid, COLOR_ERROR, "Non è possibile cambiare l'interno di questa casa.");

			new count = 0;
			
			for_list(i : list)
			{
				string += str_format("Interno %d\n", count);
				count++;
			}
			return Dialog_Show_s(playerid, Dialog_HouseInterior, DIALOG_STYLE_LIST, str_format("Cambio Interior - %s", GetInteriorTypeName(interiorType)), string, "Vendi", "Annulla");
		}
		case 7: // Affittuari
		{
			House_ShowRentersList(house, playerid);
		}
		case 8:
		{
			House_SetRentable(house, !House_IsRentable(house));
			if(House_IsRentable(house))
				SendFormattedMessage(playerid, COLOR_ORANGE, "Hai messo la tua casa in affitto per $%s.", FormatNumber(House_GetRentPrice(house)));
			else
				SendClientMessage(playerid, COLOR_ORANGE, "La tua casa non è più in affitto.");
		}
		case 9:
		{
			Dialog_Show(playerid, Dialog_HouseSetRentPrice, DIALOG_STYLE_INPUT, "Affitto", "Inserisci il prezzo dell'affito.", "Imposta", "Indietro");
		}
	}
    return 1;
}

Dialog:Dialog_HouseSetRentPrice(playerid, response, listitem, inputtext[])
{
	new House:house = Character_GetHouseMenu(playerid);
	if(!House_IsValid(house))
		return 0;
	if(!response)
		return Character_ShowHouseDialog(playerid);
	
	new v = strval(inputtext);
	if(v < 0)
		return Dialog_Show(playerid, Dialog_HouseSetRentPrice, DIALOG_STYLE_INPUT, "Affitto", "{FF0000}Il prezzo dell'affitto non è valido.\nInserisci il prezzo dell'affito.", "Imposta", "Indietro");
	House_SetRentPrice(house, v);
	SendFormattedMessage(playerid, COLOR_ORANGE, "Hai impostato il prezzo dell'affitto di casa tua a $%s.", FormatNumber(v));
	return 1;
}


Dialog:Dialog_HouseDeposit(playerid, response, listitem, inputtext[])
{
    if(!response)
	   return 0;
    new House:house = Character_GetHouseMenu(playerid);
	if(!House_IsValid(house))
		return 0;
    new amount = strval(inputtext);
    if(amount <= 0 || amount > Character_GetMoney(playerid))
	   return Dialog_Show(playerid, Dialog_HouseDeposit, DIALOG_STYLE_INPUT, "Deposita soldi", "{FF0000}Ammontare non valido!\n{FFFFFF}Inserisci l'ammontare di soldi che vuoi depositare in casa.", "Deposita", "Annulla");
    Character_GiveMoney(playerid, -amount, "house_deposit");
	House_GiveMoney(house, amount);
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai depositato $%d all'interno della tua casa.", amount);
    SendFormattedMessage(playerid, COLOR_GREEN, "Attuale: $%d", House_GetMoney(house));
    return 1;
}

Dialog:Dialog_HouseWithdraw(playerid, response, listitem, inputtext[])
{
    if(!response)
	   return 0;
    new House:house = Character_GetHouseMenu(playerid);
	if(!House_IsValid(house))
		return 0;
    new amount = strval(inputtext);
    if(amount <= 0 || amount > House_GetMoney(house))
	   return Dialog_Show(playerid, Dialog_HouseWithdraw, DIALOG_STYLE_INPUT, "Ritira soldi", "{FF0000}Ammontare non valido!\n{FFFFFF}Inserisci l'ammontare di soldi che vuoi ritirare dalla casa.\n{00FF00}Cassa: $%d{FFFFFF}", "Ritira", "Annulla", House_GetMoney(house));
    House_GiveMoney(house, -amount);
    Character_GiveMoney(playerid, amount, "Dialog_HouseWithdraw");
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai ritirato $%d dalla tua casa.", amount);
    SendFormattedMessage(playerid, COLOR_GREEN, "Attuale: $%d", House_GetMoney(house));
    return 1;
}

Dialog:Dialog_HouseSell(playerid, response, listitem, inputtext[])
{
    if(!response)
	   return Character_ShowHouseDialog(playerid);
    new House:house = Character_GetHouseMenu(playerid);
	if(!House_IsValid(house))
		return 0;
    new sellPrice = House_GetPrice(house) / 2;
    Character_GiveMoney(playerid, sellPrice, "Dialog_HouseSell");
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai venduto la tua casa per $%d.", sellPrice);

	House_RemoveAllfurnitures(house);
    House_ResetOwner(house);
    return 1;
}

Dialog:Dialog_HouseInterior(playerid, response, listitem, inputtext[])
{
    new House:house = Character_GetHouseMenu(playerid);
	if(!response || !House_IsValid(house))
		return 0;
	

    if(House_SetInterior(house, House_GetInteriorType(house), listitem))
	{
    	SendFormattedMessage(playerid, COLOR_GREEN, "Hai cambiato l'interno della tua casa. Nuovo interior: %d.", listitem);
	}
	else
	{
		SendClientMessage(playerid, COLOR_ERROR, "Non è stato possibile settare l'interior della tua casa.");
	}
    return 1;
}
