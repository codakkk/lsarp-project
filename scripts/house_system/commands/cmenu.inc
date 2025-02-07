
#include <YSI_Coding\y_hooks>
//#define extract(%0)\32;=>\32;%0 if(sscanf(%0, "d", ...))

static const RENTERS_PER_PAGE = 10;

flags:casa(CMD_ALIVE_USER);
CMD:casa(playerid, params[])
{	
	Character_ShowHouseDialog(playerid);
	return 1;
}
alias:casa("cmenu");

stock bool:Character_ShowHouseDialog(playerid)
{
    yield 1;
    new response[e_DIALOG_RESPONSE_INFO];
    for(;;)
    {
        new House:house;
        if(!Character_TryGetHouse(playerid, house) || !Character_CanUseHouseMenu(playerid, house, false))
        {
            SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata o all'interno di casa tua.");
            break;
        }
        
        await_arr(response) ShowPlayerAsyncDialog(playerid, DIALOG_STYLE_LIST, "Casa", "Apri/Chiudi Porta\nInventario\nDeposita Soldi\nRitira Soldi\nVendi\nVendi a Giocatore\nCambia Interior\nLista Affittuari\nAttiva/Disattiva Affitto\nPrezzo Affitto\nSposta Uscita", "Continua", "Chiudi");

        if(!response[E_DIALOG_RESPONSE_Response]) break;

        switch(response[E_DIALOG_RESPONSE_Listitem])
        {
            case 0: ToggleHouseLockState(playerid, house);
            case 1: Character_InteractWithInventory(playerid, House_GetInventory(house), true, true);
            case 2: ShowHouseDepositMoneyDialog(playerid, house);
            case 3: ShowHouseWithdrawMoneyDialog(playerid, house);
            case 4: ShowSellHouseDialog(playerid, house);
            case 5: ShowSellHouseToPlayerDialog(playerid, house);
            case 6: ShowChangeHouseInteriorDialog(playerid, house);
            case 7: ShowHouseRentersListDialog(playerid, house, 0);
            case 8: ToggleHouseRent(playerid, house);
            case 9: ShowChangeHouseRentPriceDialog(playerid, house);
            case 10: ShowMoveExitDialog(playerid, house);
        }
    }
}

static ShowMoveExitDialog(playerid, House:house)
{
    if(!Character_CanUseHouseMenu(playerid, house, true))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Solo il proprietario puo' modificare l'uscita della casa.");
        return;
    }
    
    if(Character_GetVirtualWorld(playerid) != House_GetExitWorld(house))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Questa funzionalit� pu� essere utilizzata solo all'interno della casa.");
        return;
    }

    new response[e_DIALOG_RESPONSE_INFO];

    yield 1;
    await_arr(response) ShowPlayerAsyncDialog(playerid, DIALOG_STYLE_MSGBOX, "Sposta Uscita", "Sei sicuro di voler spostare l'uscita della casa alla posizione corrente?", "Sposta", "Indietro");

    if(response[E_DIALOG_RESPONSE_Response])
    {
        SendClientMessage(playerid, -1, "Hai spostato l'uscita della casa alla posizione corrente.");

        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);
        House_SetExitPosition(house, x, y, z);
    }
}

static ToggleHouseLockState(playerid, House:house)
{
    House_SetLocked(house, !House_IsLocked(house));
    SendFormattedMessage(playerid, -1, "{FFFFFF}Hai modificato lo stato della porta della casa. Stato: %s", House_IsLocked(house) ? ("Chiusa") : ("Aperta"));
}

static ShowHouseDepositMoneyDialog(playerid, House:house)
{
    yield 1;
    new response[e_DIALOG_RESPONSE_INFO];
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialog(playerid, DIALOG_STYLE_INPUT, "Deposita soldi", "Inserisci l'ammontare di soldi che vuoi depositare in casa.", "Deposita", "Annulla");

        if(!response[E_DIALOG_RESPONSE_Response]) break;

        new amount = strval(response[E_DIALOG_RESPONSE_InputText]);

        if(amount <= 0 || amount > Character_GetMoney(playerid)) continue;

        Character_GiveMoney(playerid, -amount, "house_deposit");
        House_GiveMoney(house, amount);
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai depositato $%d all'interno della tua casa.", amount);
        SendFormattedMessage(playerid, COLOR_GREEN, "Attuale: $%d", House_GetMoney(house));
        break;
    }
}

static ShowHouseWithdrawMoneyDialog(playerid, House:house)
{
    yield 1;
    new response[e_DIALOG_RESPONSE_INFO];
    for(;;)
    {
        if(!Character_DoesOwnHouse(playerid, house))
        {
            SendClientMessage(playerid, COLOR_ERROR, "Solo il proprietario puo' utilizzare questa funzionalit�.");
            break;
        }

        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_INPUT, @("Ritira soldi"), str_format("Inserisci l'ammontare di soldi che vuoi ritirare dalla casa.\nDisponibile: %d", House_GetMoney(house)), @("Ritira"), @("Annulla"));

        if(!response[E_DIALOG_RESPONSE_Response]) break;

        new amount = strval(response[E_DIALOG_RESPONSE_InputText]);

        if(amount <= 0 || amount > House_GetMoney(house)) continue;

        House_GiveMoney(house, -amount);
        Character_GiveMoney(playerid, amount, "Dialog_HouseWithdraw");
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai ritirato $%d dalla tua casa.", amount);
        SendFormattedMessage(playerid, COLOR_GREEN, "Attuale: $%d", House_GetMoney(house));
        break;
    }
}

static ShowSellHouseDialog(playerid, House:house)
{
    if(!Character_CanUseHouseMenu(playerid, house, true))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Solo il proprietario puo' utilizzare quest'opzione.");
        return;
    }
    yield 1;
    new response[e_DIALOG_RESPONSE_INFO];
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_MSGBOX, @("Vendi Immobile"), str_format("Sei sicuro di voler vendere la tua casa per {00FF00}$%d{FFFFFF}\nUna volta venduta, tutti i mobili verranno rimossi.", House_GetPrice(house)/2), @("Vendi"), @("Annulla"));

        if(!response[E_DIALOG_RESPONSE_Response]) break;

        if(!House_IsValid(house))
            break;

        if(!House_IsOwned(house))
        {
            SendClientMessage(playerid, COLOR_ERROR, "La casa non � acquistata, per tanto non puoi venderla.");
            break;
        }

        if(!Character_DoesOwnHouse(playerid, house))
        {
            SendClientMessage(playerid, COLOR_ERROR, "Solo il proprietario puo' utilizzare questo comando.");
            if(Player_IsAdminDuty(playerid))
                SendClientMessage(playerid, COLOR_ORANGE, "Usa /hedit per resettare il proprietario.");
            break;
        }

        new sellPrice = House_GetPrice(house) / 2;
        Character_GiveMoney(playerid, sellPrice, "Dialog_HouseSell");
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai venduto la tua casa per $%d.", sellPrice);

        //House_RemoveAllfurnitures(house);
        House_ResetOwner(house);
        break;
    }
}

static ShowSellHouseToPlayerDialog(playerid, House:house)
{
    if(!Character_CanUseHouseMenu(playerid, house, true))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Solo il proprietario puo' utilizzare quest'opzione.");
        return;
    }
    yield 1;
    new response[e_DIALOG_RESPONSE_INFO];
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialog(playerid, DIALOG_STYLE_INPUT, "Vendi a giocatore", "Inserisci l'id o il nome del giocatore a cui vuoi vendere le casa seguito dal prezzo.\nEsempio: Peppe_Rossi 10000\n", "Vendi", "Annulla");

        if(!response[E_DIALOG_RESPONSE_Response]) break;

        new id, price;
        if(sscanf(response[E_DIALOG_RESPONSE_InputText], "k<u>d", id, price) || id == playerid) continue;

        if(price < 0 || price > Character_GetMoney(id)) 
        {
            SendFormattedMessage(playerid, COLOR_ERROR, "%s non possiede $%d.", Character_GetRolePlayName(id), price);
            continue;
        }

        if(Request_IsPending(playerid, REQUEST_TYPE_HOUSE) || Request_IsPending(id, REQUEST_TYPE_HOUSE))
        {
            SendClientMessage(playerid, COLOR_ERROR, "Tu od il giocatore avete gi� una richiesta di vendita casa attiva.");
            continue;
        }

        if(!Character_CanOwnHouse(id, house))
        {
            SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non puo' acquistare ulteriori case.");
            continue;
        }

        SendFormattedMessage(id, COLOR_RECEIVE_REQUEST, "%s vuole venderti la sua casa per per $%d.", Character_GetRolePlayName(playerid), price);
        SendClientMessage(id, COLOR_RECEIVE_REQUEST, "Digita \"/accetta casa\" per accettare o \"/rifiuta casa\" per rifiutare.");
    
        SendFormattedMessage(playerid, COLOR_SEND_REQUEST, "Hai inviato la richiesta di vendita a %s per $%d. Usa \"annulla casa\" per annullare.", Character_GetRolePlayName(id), price);
        
        Character_SetRequest(playerid, id, REQUEST_TYPE_HOUSE, house, price);
        break;
    }
}

static ShowChangeHouseInteriorDialog(playerid, House:house)
{
    if(!Character_CanUseHouseMenu(playerid, house, true))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Solo il proprietario puo' utilizzare quest'opzione.");
        return;
    }
    yield 1;
    new response[e_DIALOG_RESPONSE_INFO];
    for(;;)
    {
        new String:string, interiorType = House_GetInteriorType(house);
			
        new List:list = HouseInterior_GetListByType(interiorType);

        if(!list_valid(list) || list_size(list) <= 0)
        {
            SendClientMessage(playerid, COLOR_ERROR, "Non � possibile cambiare l'interno di questa casa.");
            break;
        }

        new count = 0;
        
        for_list(i : list)
        {
            string += str_format("Interno %d\n", count);
            count++;
        }

        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_LIST, str_format("Cambio Interior - %s", GetInteriorTypeName(interiorType)), string, @("Vendi"), @("Annulla"));

        if(!response[E_DIALOG_RESPONSE_Response]) break;

        if(Character_GetVirtualWorld(playerid) == House_GetExitWorld(house))
        {
            SendClientMessage(playerid, COLOR_ERROR, "Devi essere all'esterno della casa per poter cambiare interior.");
            continue;
        }

        if(House_SetInterior(house, House_GetInteriorType(house), response[E_DIALOG_RESPONSE_Listitem]))
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai cambiato l'interno della tua casa. Nuovo interior: %d.", response[E_DIALOG_RESPONSE_Listitem]);
        }
        else
        {
            SendClientMessage(playerid, COLOR_ERROR, "Non � stato possibile settare l'interior della tua casa.");
        }
        break;
    }
}

static ShowHouseRentersListDialog(playerid, House:house, page = 0)
{
    if(!Character_CanUseHouseMenu(playerid, house, true))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Solo il proprietario puo' utilizzare quest'opzione.");
        return;
    }
    new List:PlayerRenters = list_new(),
        selectedPage = page;

    yield 1;
    new String:content = str_new(""), response[e_DIALOG_RESPONSE_INFO];
    pawn_guard(content);
    pawn_guard(PlayerRenters);
    for(;;)
    {
        str_clear(content);
        str_append_format(content, "Nome\tPrezzo\n");

        list_clear(PlayerRenters);

        await mysql_aquery_s(gMySQL, str_format("SELECT id, name, house_rent_price FROM `characters` WHERE house_rent = '%d' LIMIT %d, %d", House_GetID(house), selectedPage * RENTERS_PER_PAGE, RENTERS_PER_PAGE + 1));

        new rows = cache_num_rows();
        if(rows <= 0)
        {
            SendClientMessage(playerid, COLOR_ERROR, "Non ci sono affittuari in questa casa.");
            break;
        }

        new id, name[MAX_PLAYER_NAME], price;

        if(selectedPage > 0) str_append_format(content, "Pagina\tPrecedente\n");
        
        if(rows > RENTERS_PER_PAGE) str_append_format(content, "Pagina\tSuccessiva\n");
        //rows--; // This is because I added 1 to RENTERS_PER_PAGE, just to check if there atleast one entry
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", id);
            cache_get_value_name(i, "name", name);
            cache_get_value_name_int(i, "house_rent_price", price);
            
            list_add(PlayerRenters, id);

            if(i <= RENTERS_PER_PAGE)
                str_append_format(content, "{FFFFFF}%s\t{8FBC8F}$%s{FFFFFF}\n", name, FormatNumber(price));
        }

        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_TABLIST_HEADERS, House_GetAddressStr(house), content, @("Modifica"), @("Indietro"));

        if(!response[E_DIALOG_RESPONSE_Response]) break;

        if(selectedPage > 0)
        {
            if(response[E_DIALOG_RESPONSE_Listitem] == 0)
            {
                selectedPage--;
                continue;
            }
            response[E_DIALOG_RESPONSE_Listitem]--;
        }
        if(list_size(PlayerRenters) > RENTERS_PER_PAGE)
        {
            if(response[E_DIALOG_RESPONSE_Listitem] == 0)
            {
                selectedPage++;
                continue;
            }
            response[E_DIALOG_RESPONSE_Listitem]--;
        }

        new renterid = list_get(PlayerRenters, response[E_DIALOG_RESPONSE_Listitem]);

        for(;;)
        {
            await mysql_aquery_s(gMySQL, str_format("SELECT name, house_rent_price FROM `characters` WHERE id = '%d'", renterid), .parallel = false);

            if(cache_num_rows() <= 0)
                break;
            cache_get_value_name(0, "name", name);
            cache_get_value_name_int(0, "house_rent_price", price);
            await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_LIST, str_format("%s - $%s", name, FormatNumber(price)), @("Caccia"), @("Seleziona"), @("Indietro"));
            
            if(!response[E_DIALOG_RESPONSE_Response]) break;

            HandleHouseRentActionOnPlayer(playerid, house, renterid, response[E_DIALOG_RESPONSE_Listitem]);
            break;
        }
    }

    list_delete_deep(PlayerRenters);
    PlayerRenters = INVALID_LIST;
}

static HandleHouseRentActionOnPlayer(playerid, House:house, playerDatabaseId, listitem)
{
    #pragma unused house
    if(listitem == 0)
    {
        new bool:notified = false;
        foreach(new i : Player)
        {
            if(playerDatabaseId == Character_GetID(i))
            {
                SendClientMessage(playerid, COLOR_ORANGE, "Il proprietario della casa di cui eri affittuario ti ha cacciato.");
                Character_SetRentHouse(i, INVALID_HOUSE);
                Character_SetHouseRentPrice(i, 0);
                notified = true;
                break;
            }
        }
        SendClientMessage(playerid, COLOR_ORANGE, "Hai cacciato il giocatore da casa tua.");
        if(notified == false)
        {
            SendNotification(playerDatabaseId, "Il proprietario della casa di cui eri affittuario ti ha cacciato.");
            mysql_pquery_f(gMySQL, "UPDATE `characters` SET house_rent = '%d', house_rent_price = '0' WHERE id = '%d'", INVALID_HOUSE_ID, playerDatabaseId);
        }
    }
}

static ToggleHouseRent(playerid, House:house)
{
    if(!Character_CanUseHouseMenu(playerid, house, true))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Solo il proprietario puo' utilizzare quest'opzione.");
        return;
    }
    House_SetRentable(house, !House_IsRentable(house));
    if(House_IsRentable(house))
        SendClientMessage(playerid, -1, "Hai abilitato l'affitto alla casa.");
    else
        SendClientMessage(playerid, -1, "Hai disabilitato l'affitto alla casa.");
}

static ShowChangeHouseRentPriceDialog(playerid, House:house)
{
    if(!Character_CanUseHouseMenu(playerid, house, true))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Solo il proprietario puo' utilizzare quest'opzione.");
        return;
    }
    yield 1;
    new response[e_DIALOG_RESPONSE_INFO];
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialog(playerid, DIALOG_STYLE_INPUT, "Modifica prezzo affitto", "Inserisci il prezzo dell'affito.", "Vendi", "Annulla");

        if(!response[E_DIALOG_RESPONSE_Response]) break;

        new v = strval(response[E_DIALOG_RESPONSE_InputText]);
        
        if(v <= 0) continue;
        
        House_SetRentPrice(house, v);
        SendFormattedMessage(playerid, COLOR_ORANGE, "Hai impostato il prezzo dell'affitto di casa tua a $%s.", FormatNumber(v));
        SendClientMessage(playerid, COLOR_ORANGE, "Ricorda che il costo dell'affitto sar� applicato solo agli affittuari futuri.");
        break;
    }
}

hook OnCharacterTryAccept(playerid, type)
{
    if(type != REQUEST_TYPE_HOUSE)
        return 0;
    
    new senderid = Request_GetSender(playerid, REQUEST_TYPE_HOUSE),
        price = Request_GetAmount(playerid, REQUEST_TYPE_HOUSE),
        House:house = House:Request_GetItem(playerid, REQUEST_TYPE_HOUSE)
    ;
    if(!Character_DoesOwnHouse(senderid, house))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non possiede pi� la casa.");
    }
    else
    {
        if(Character_GetMoney(playerid) < price)
            return SendFormattedMessage(playerid, COLOR_ERROR, "Non hai l'ammontare richiesto dal giocatore ($%d).", price);

        if(House_SetOwner(house, playerid))
        {
            Character_GiveMoney(playerid, -price, "buy_house_from_player");
            Character_GiveMoney(senderid, price, "sell_house_to_player");

            SendFormattedMessage(senderid, COLOR_ACCEPT_REQUEST, "%s ha acquistato la tua casa situata all'indirizzo %s per $%d", Character_GetRolePlayName(playerid), House_GetAddress(house), price);
            SendFormattedMessage(playerid, COLOR_ACCEPT_REQUEST, "Hai acquistato la casa di %s situata all'indirizzo %s per $%d", Character_GetRolePlayName(senderid), House_GetAddress(house), price);
        }
    }
    Request_Reset(playerid, REQUEST_TYPE_HOUSE);
    Request_Reset(senderid, REQUEST_TYPE_HOUSE);
    return 1;
}