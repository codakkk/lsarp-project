flags:biz(CMD_ALIVE_USER);
CMD:biz(playerid, params[])
{
	return Character_ShowBizDialog(playerid);
}
alias:biz("bmenu");

static bool:Character_ShowBizDialog(playerid)
{
    yield 1;
    new response[e_DIALOG_RESPONSE_INFO];
    for(;;)
    {
        new Building:building;
        if(!Character_TryGetBuilding(playerid, building) || !Character_CanUseBuildingMenu(playerid, building))
        {
            SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata o all'interno di un tuo biz.");
            break;
        }
        
        await_arr(response) ShowPlayerAsyncDialog(playerid, DIALOG_STYLE_LIST, "Edificio", "Apri/Chiudi Porta\nInventario\nDeposita Soldi\nRitira Soldi\nVendi\nVendi a Giocatore\nGestisci Negozio\nPrezzo entrata", "Continua", "Chiudi");

        if(!response[E_DIALOG_RESPONSE_Response]) break;

        switch(response[E_DIALOG_RESPONSE_Listitem])
        {
            case 0: ToggleBuildingLockState(playerid, building);
            case 1: Character_InteractWithInventory(playerid, Building_GetInventory(building), true, true);
            case 2: ShowBuildingDepositMoneyDialog(playerid, building);
            case 3: ShowBuildingWithdrawMoneyDialog(playerid, building);
            case 4: ShowSellBuildingDialog(playerid, building);
            case 5: ShowSellBuildingToPlayerDialog(playerid, building);
            case 6: Building_ShowTypeMenu(building, playerid);
            case 7: ShowBuildingEntrancePrice(playerid, building);
        }

        if(!Character_CanUseBuildingMenu(playerid, building))
            break;
    }
    return true;
}

static ToggleBuildingLockState(playerid, Building:building)
{
    Building_SetLocked(building, !Building_IsLocked(building));
    if(Building_IsLocked(building))
        SendClientMessage(playerid, COLOR_GREEN, "Hai chiuso l'edificio.");
    else
        SendClientMessage(playerid, COLOR_GREEN, "Hai aperto l'edificio.");
}

static ShowBuildingEntrancePrice(playerid, Building:building)
{
    yield 1;
    new response[e_DIALOG_RESPONSE_INFO];
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_INPUT, @("Prezzo d'entrata"), @("Inserisci il prezzo d'entrata che vuoi settare per l'edificio.\nUsa 0 come prezzo per rimuoverlo."), @("Imposta"), @("Indietro"));

        if(!response[E_DIALOG_RESPONSE_Response]) break;

        new value = strval(response[E_DIALOG_RESPONSE_InputText]);

        if(value < 0 || value >= cellmax)
        {
            SendClientMessage(playerid, COLOR_ERROR, "L'ammontare inserito non è valido.");
            continue;
        }
        Building_SetEntrancePrice(building, value);
	    SendFormattedMessage(playerid, -1, "Hai settato il prezzo d'entrata dell'edificio a $%s.", FormatNumber(value));
        break;
    }
}

static ShowBuildingDepositMoneyDialog(playerid,  Building:building)
{
    yield 1;
    new response[e_DIALOG_RESPONSE_INFO];
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_INPUT, @("Deposita Denaro"), str_format("Inserisci l'ammontare di denaro che vuoi depositare all'interno del tuo edificio.\nAttualmente ci sono $%d nella cassa dell'edificio.", Building_GetMoney(building)), @("Deposita"), @("Indietro"));

        if(!response[E_DIALOG_RESPONSE_Response]) break;

        new value = strval(response[E_DIALOG_RESPONSE_InputText]);

        if(value <= 0 || value > Character_GetMoney(playerid))
        {
            SendClientMessage(playerid, COLOR_ERROR, "L'ammontare inserito non è valido.");
            continue;
        }

        Building_GiveMoney(building, value);
        Character_GiveMoney(playerid, -value, "building_deposit");

	    SendFormattedMessage(playerid, -1, "Hai depositato $%d all'interno dell'edificio. Nella cassa dell'edificio ci sono $%d.", value, Building_GetMoney(building));
        
        break;
    }
}

static ShowBuildingWithdrawMoneyDialog(playerid,  Building:building)
{
    yield 1;
    new response[e_DIALOG_RESPONSE_INFO];
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_INPUT, @("Ritira Denaro"), str_format("Inserisci l'ammontare di denaro che vuoi ritirare dall'interno del tuo edificio.\nAttualmente ci sono $%d nella cassa dell'edificio.", Building_GetMoney(building)), @("Ritira"), @("Indietro"));

        if(!response[E_DIALOG_RESPONSE_Response]) break;

        new value = strval(response[E_DIALOG_RESPONSE_InputText]);

        if(value <= 0 || value > Building_GetMoney(building))
        {
            SendClientMessage(playerid, COLOR_ERROR, "L'ammontare inserito non è valido.");
            continue;
        }

        Building_GiveMoney(building, -value);
	    Character_GiveMoney(playerid, value, "building_withdraw");

	    SendFormattedMessage(playerid, -1, "Hai ritirato $%d dall'interno dell'edificio. Nella cassa dell'edificio ci sono $%d.", value, Building_GetMoney(building));
        
        break;
    }
}

static ShowSellBuildingDialog(playerid, Building:building)
{
    if(!Character_CanUseBuildingMenu(playerid, building, true))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Solo il proprietario puo' utilizzare quest'opzione.");
        return;
    }
    yield 1;
    new response[e_DIALOG_RESPONSE_INFO];
    new name[MAX_BUILDING_NAME], price = Building_GetPrice(building)/2;
    for(;;)
    {
        Building_GetName(building, name);
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_MSGBOX, @("Vendita Edificio"), str_format("Sei sicuro di voler vendere questo edificio (%s) per {85BB65}$%d{FFFFFF}?", name, price), @("Vendi"), @("Annulla"));

        if(!response[E_DIALOG_RESPONSE_Response]) break;
        
        if(Building_GetOwnerID(building) <= 0)
        {
            SendClientMessage(playerid, COLOR_ERROR, "Il biz non è acquistato, per tanto non puoi venderlo.");
            break;
        }

        if(!Character_DoesOwnBuilding(playerid, building))
        {
            SendClientMessage(playerid, COLOR_ERROR, "Solo il proprietario puo' utilizzare questo comando.");
            if(Player_IsAdminDuty(playerid))
                SendClientMessage(playerid, COLOR_ORANGE, "Usa /abedit per resettare il proprietario.");
            break;
        }

	    Building_ResetOwner(building);
        Character_GiveMoney(playerid, price, "building_sell");
        SendFormattedMessage(playerid, -1, "Hai venduto il tuo business (%s) per $%d.", name, price);
        break;
    }
}

static ShowSellBuildingToPlayerDialog(playerid, Building:building)
{
    if(!Character_CanUseBuildingMenu(playerid, building, true))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Solo il proprietario puo' utilizzare quest'opzione.");
        return;
    }

    yield 1;
    new response[e_DIALOG_RESPONSE_INFO];
    new name[MAX_BUILDING_NAME];
    for(;;)
    {
        Building_GetName(building, name);
        await_arr(response) ShowPlayerAsyncDialog(playerid, DIALOG_STYLE_INPUT, "Vendita Edificio", "Inserisci l'id, il nome o il codice (#XXXXX) del giocatore a cui vuoi vendere l'edificio, seguito dal prezzo.\nEsempi:\n> 1 50010\n> Pinco_Pallino 50000\n> #0001 50000", "Vendi", "Annulla");

        if(!response[E_DIALOG_RESPONSE_Response]) break;

        if(Building_GetOwnerID(building) <= 0)
        {
            SendClientMessage(playerid, COLOR_ERROR, "Il biz non è acquistato, per tanto non puoi venderlo.");
            break;
        }

        if(!Character_DoesOwnBuilding(playerid, building))
        {
            SendClientMessage(playerid, COLOR_ERROR, "Solo il proprietario puo' utilizzare questo comando.");
            if(Player_IsAdminDuty(playerid))
                SendClientMessage(playerid, COLOR_ORANGE, "Usa /abedit per resettare il proprietario.");
            break;
        }

        new id, money;
        if(sscanf(response[E_DIALOG_RESPONSE_InputText], "k<u>d", id, money))
        {
            SendClientMessage(playerid, COLOR_ERROR, "Il formato inserito non è valido.");
            continue;
        }    

        if(!Character_IsLogged(id) || id == playerid)
        {
            SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è valido o non è collegato.");
            continue;
        }

        if(!IsPlayerInRangeOfPlayer(playerid, id, 8.0))
		{
            SendClientMessage(playerid, COLOR_ERROR, "Sei troppo distante dal giocatore.");
            continue;
        }

        if(money < 0 || money > Character_GetMoney(id))
        {
            SendClientMessage(playerid, COLOR_ERROR, "Il prezzo inserito non è valido o il giocatore non possiede tale somma.");
            continue;
        }

        if(!Character_CanOwnBuilding(id, building))
		{
            SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non puo' acquistare ulteriori business.");
            continue;
        }

        Character_SetRequest(playerid, id, REQUEST_TYPE_BUILDING, building, money);
        SendFormattedMessage(id, COLOR_RECEIVE_REQUEST, "%s vuole venderti il suo edificio situato presso %s per $%d.", Character_GetRolePlayName(playerid), Building_GetAddress(building), money);
        SendClientMessage(id, COLOR_RECEIVE_REQUEST, "Digita \"accetta edificio\" per accettare o \"rifiuta edificio\" per rifiutare.");
        SendFormattedMessage(playerid, COLOR_SEND_REQUEST, "Hai proposto il tuo edificio a %s per $%d.", Character_GetRolePlayName(id), money);
        SendClientMessage(playerid, COLOR_SEND_REQUEST, "Digita \"annulla edificio\" per annullare la richiesta.");
        break;
    }
}

stock Building_ShowTypeMenu(Building:building, playerid)
{
	#pragma unused building
	SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare quest'opzione su questo tipo di biz.");
	return 1;
}