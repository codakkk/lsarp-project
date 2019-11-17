
#include <YSI_Coding\y_hooks>

static enum _:E_CONTACT
{
    CONTACT_ID,
    CONTACT_NAME[32]
};
static Map:PhoneBooks; // <phone number, Map:Contact>

hook OnGameModeInit()
{
    PhoneBooks = map_new();
    mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `phonebook` ( \
                        `id` int(11) NOT NULL AUTO_INCREMENT, \
						`number_owner` int(11) NOT NULL, \
						`name` VARCHAR(32) NOT NULL, \
                        `number` int(11) NOT NULL, \
                        `created_at` int(11) NOT NULL, \
						PRIMARY KEY (`id`) \
						) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=latin1");
    return 1;
}

hook OnCharacterDisconnected(playerid, reason)
{
    for(new i = 0, j = Character_GetInventorySize(playerid); i < j; ++i)
    {
        if(Character_GetSlotItem(playerid, i) != gItem_Phone)
            continue;
        new number = Character_GetSlotExtra(playerid, i, 0);
        UnloadPhoneBook(number);
    }
    return 1;
}

hook OnCharacterLoadPhone(playerid, n)
{
    LoadPhoneBook(n);
    return 1;
}

flags:rubrica(CMD_ALIVE_USER);
CMD:rubrica(playerid, params[])
{
    new slotid;
    if(sscanf(params, "k<slotid>", slotid))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /rubrica <slotid>");
    Character_ShowPhoneBook(playerid, slotid);
    return 1;
}

stock Character_ShowPhoneBook(playerid, slotid)
{
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un cellulare in questo slot.");
    
    new playerNumber = Character_GetSlotExtra(playerid, slotid, 0),
        Map:phonebook = PhoneBook_Get(playerNumber);

    if(map_size(phonebook) <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai registrato nessun contatto nella rubrica di questo cellulare.");

    new response[e_DIALOG_RESPONSE_INFO];
    new contactNumber, name[32];
    new List:temp = list_new();
    yield 1;
    for(;;)
    {
        new String:str = @("Nome\tNumero\n");
        list_clear(temp);
        for_map(i : phonebook)
        {
            contactNumber = iter_get_key(i);
            iter_get_md_arr(i, {CONTACT_NAME}, name, 32);
            str_append_format(str, "%s\t%d\n", name, contactNumber);
            list_add(temp, contactNumber);
        }

        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_TABLIST_HEADERS, str_format("Cellulare - %d", playerNumber), str, @("Continua"), @("Indietro"));
    
        if(!response[E_DIALOG_RESPONSE_Response])
            break;
        
        new listitem = response[E_DIALOG_RESPONSE_Listitem];
        
        contactNumber = list_get(temp, listitem);

        HandlePhoneBookContact(playerid, slotid, playerNumber, contactNumber);
    }

    list_delete(temp);
    return 1;
}

static HandlePhoneBookContact(playerid, slotid, ownerNumber, contactNumber)
{
    new response[e_DIALOG_RESPONSE_INFO];
    yield 1;
    for(;;)
    {
        new name[32];
        PhoneBook_GetContactName(ownerNumber, contactNumber, name);

        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_LIST, str_format("%s - %d", name, contactNumber), @("Chiama\nSMS\nModifica Nome\nCancella"), @("Continua"), @("Indietro"));

        if(!response[E_DIALOG_RESPONSE_Response])
            break;
        
        switch(response[E_DIALOG_RESPONSE_Listitem])
        {
            case 0: HandleCallFromPhoneBook(playerid, slotid, ownerNumber, contactNumber);
            case 1: HandleSendSMSFromPhoneBook(playerid, slotid, ownerNumber, contactNumber);
            case 2: HandleChangeContactNameDialog(playerid, slotid, ownerNumber, contactNumber);
            case 3: DeleteContactDialog(playerid, slotid, ownerNumber, contactNumber);
        }

        break;
    }
}

static HandleCallFromPhoneBook(playerid, slotid, ownerNumber, contactNumber)
{
    #pragma unused ownerNumber
    new number[9];
    valstr(number, contactNumber);
    Character_StartCall(playerid, slotid, number);   
}

static HandleSendSMSFromPhoneBook(playerid, slotid, ownerNumber, contactNumber)
{

    new response[e_DIALOG_RESPONSE_INFO];
    yield 1;
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_INPUT, str_format("Cellulare - %d", ownerNumber), @("Inserisci il messaggio che vuoi inviare al contatto. (Lunghezza Massima: 128 caratteri.\n\n"), @("Invia"), @("Indietro"));
    
        if(!response[E_DIALOG_RESPONSE_Response]) break;
    
        if(strlen(response[E_DIALOG_RESPONSE_InputText]) <= 0 || strlen(response[E_DIALOG_RESPONSE_InputText]) >= 128) continue;

        Character_SendSMS(playerid, slotid, contactNumber, response[E_DIALOG_RESPONSE_InputText]);
        break;
    }
}

static HandleChangeContactNameDialog(playerid, slotid, ownerNumber, contactNumber)
{
    #pragma unused slotid
    new response[e_DIALOG_RESPONSE_INFO];
    yield 1;
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_INPUT, str_format("Cellulare - %d", ownerNumber), @("Inserisci il nuovo nome da assegnare al contatto. (Max 31 caratteri).\n\n"), @("Modifica"), @("Indietro"));
    
        if(!response[E_DIALOG_RESPONSE_Response]) break;
    
        if(strlen(response[E_DIALOG_RESPONSE_InputText]) <= 0 || strlen(response[E_DIALOG_RESPONSE_InputText]) >= 31) continue;
        new res = PhoneBook_SetContactName(ownerNumber, contactNumber, response[E_DIALOG_RESPONSE_InputText]);
        if(!res)
            continue;
        break;
    }
}

static DeleteContactDialog(playerid, slotid, ownerNumber, contactNumber)
{
    #pragma unused slotid
    new response[e_DIALOG_RESPONSE_INFO];
    yield 1;
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_MSGBOX, str_format("Cellulare - %d", ownerNumber), @("Sei sicuro di voler cancellare definitivamente questo numero dalla rubrica?\n\n"), @("{FF0000}Cancella{FFFFFF}"), @("Indietro"));
    
        if(!response[E_DIALOG_RESPONSE_Response]) break;

        new name[32];
        PhoneBook_GetContactName(ownerNumber, contactNumber, name);
        if(PhoneBook_DeleteContact(ownerNumber, contactNumber))
        {
            SendFormattedMessage(playerid, COLOR_YELLOW, "Hai rimosso il contatto (%s) dalla rubrica.", name);
        }
        else SendClientMessage(playerid, COLOR_ERROR, "Non è stato possibile rimuovere il contatto. (Errore interno).");
        break;
    }
    
}

stock Character_AddToPhoneBook(playerid, slotid, number, const name[])
{
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return 0;

    new playerNumber = Character_GetSlotExtra(playerid, slotid, 0);

    if(strlen(name) > 30)
        return SendClientMessage(playerid, COLOR_ERROR, "Hai superato il numero massimo di caratteri per il contatto.");

    if(PhoneBook_HasContact(playerNumber, number))
        return SendClientMessage(playerid, COLOR_ERROR, "Hai già salvato questo numero. Controlla la rubrica.");

    if(PhoneBook_AddContact(playerNumber, number, name))
        SendFormattedMessage(playerid, COLOR_YELLOW, "Hai aggiunto il numero %d alla rubrica con il nome %s.", number, name);
    else
        SendClientMessage(playerid, COLOR_ERROR, "Non è stato possibile aggiungere il numero alla rubrica. Errore sconosciuto.");
    return 1;
}

static LoadPhoneBook(phoneNumber)
{
    if(map_has_key(PhoneBooks, phoneNumber))
        return 0;
    new Map:map = map_new();
    map_set_ordered(map, true);
    map_add(PhoneBooks, phoneNumber, map);
    inline OnLoad()
    {
        new rows = cache_num_rows();

        new data[E_CONTACT], number;
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", data[CONTACT_ID]);
            cache_get_value_name(i, "name", data[CONTACT_NAME]);
            cache_get_value_name_int(i, "number", number);
            map_add_arr(map, number, data);
        }
        //printf("Loaded PhoneBook for number %d", phoneNumber);
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT id, name, number FROM `phonebook` WHERE number_owner = '%d'", phoneNumber);
    return 1;
}

static UnloadPhoneBook(phoneNumber)
{
    if(!map_has_key(PhoneBooks, phoneNumber))
        return 0;
    new Map:map = PhoneBook_Get(phoneNumber);
    if(!map_valid(map))
        return 0;
    map_delete(map);
    map_remove_deep(PhoneBooks, phoneNumber);
    //printf("Unloaded PhoneBook for number %d", phoneNumber);
    return 1;
}

static Map:PhoneBook_Get(number)
{
    if(!bool:map_has_key(PhoneBooks, number))
        return Map:0;
    return Map:map_get(PhoneBooks, number);
}

static stock PhoneBook_GetContactID(ownerNumber, number)
{
    new Map:phonebook = PhoneBook_Get(ownerNumber);
    if(!map_has_key(phonebook, number))
        return 0;
    return map_get(phonebook, number, CONTACT_ID);
}

static bool:PhoneBook_AddContact(ownerNumber, number, const name[])
{
    if(!map_has_key(PhoneBooks, ownerNumber))
        return false;
    new Map:phonebook = PhoneBook_Get(ownerNumber);
    if(map_has_key(phonebook, number))
        return false;
    
    await mysql_aquery_s(gMySQL, str_format("INSERT INTO `phonebook` (number_owner, name, number, created_at) VALUES('%d', '%e', '%d', '%d')", ownerNumber, name, number, gettime()), .parallel=true);
    
    new data[E_CONTACT];
    data[CONTACT_ID] = cache_insert_id();
    set(data[CONTACT_NAME], name);
    map_add_arr(phonebook, number, data);
    return true;
}

static bool:PhoneBook_DeleteContact(ownerNumber, number)
{
    if(!map_has_key(PhoneBooks, ownerNumber))
        return false;
    new Map:phonebook = PhoneBook_Get(ownerNumber);
    if(!map_has_key(phonebook, number))
        return false;
    new id = map_get(phonebook, number, CONTACT_ID);
    if(id > 0)
        mysql_pquery_f(gMySQL, "DELETE FROM `phonebook` WHERE id = '%d'", id);
    map_remove(phonebook, number);
    return true;
}

static bool:PhoneBook_SetContactName(ownerNumber, number, name[])
{
    if(!map_has_key(PhoneBooks, ownerNumber))
        return false;
    new Map:phonebook = PhoneBook_Get(ownerNumber);
    if(!map_has_key(phonebook, number))
        return false;
    
    new id = map_get(phonebook, number, CONTACT_ID);
    
    if(id <= 0)
        return false;

    new 
		Iter:iter = Iter:map_iter_at(phonebook, number)
	;
    iter_set_cells(iter, CONTACT_NAME, name, 32);
    mysql_pquery_f(gMySQL, "UPDATE `phonebook` SET name = '%e' WHERE id = '%d'", name, id);
    return true;
}

stock PhoneBook_GetContactName(ownerNumber, number, name[])
{
    if(!map_has_key(PhoneBooks, ownerNumber))
        return 0;
    new Map:phonebook = PhoneBook_Get(ownerNumber);
    if(!map_has_key(phonebook, number))
        return 0;
    new Iter:iter = map_iter_at(phonebook, number);
    iter_get_md_arr(iter, {CONTACT_NAME}, name, 32);
    return 1;
}

static bool:PhoneBook_HasContact(ownerNumber, number)
{
    if(!map_has_key(PhoneBooks, ownerNumber))
        return false;
    new Map:phonebook = PhoneBook_Get(ownerNumber);
    return map_has_key(phonebook, number);
}

stock PhoneBook_ParseValue(ownerNumber, value[])
{
    if(IsNumeric(value))
        return strval(value);
    new result = 0;
    
    if(map_has_key(PhoneBooks, ownerNumber))
    {
        new Map:phonebook = PhoneBook_Get(ownerNumber);
        new name[32];
        for_map(i : phonebook)
        {
            new number = iter_get_key(i);
            if(PhoneBook_GetContactName(ownerNumber, number, name) && !strcmp(name, value, true))
            {
                result = number;
                break;
            }
        } 
    }
    
    return result;
}
