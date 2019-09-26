
#include <YSI_Coding\y_hooks>

static enum _:E_CONTACT
{
    CONTACT_ID,
    CONTACT_NAME[32]
};
static Map:PhoneBooks; // <phone number, Map:Contact>

static SelectedContactNumber[MAX_PLAYERS];

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

hook OnPlayerClearData(playerid)
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
    if(sscanf(params, "d", slotid))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /rubrica <slotid>");
    Character_ShowPhoneBook(playerid, slotid);
    return 1;
}

stock Character_ShowPhoneBook(playerid, slotid)
{
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un cellulare in questo slot.");
    
    new playerNumber = Character_GetSlotExtra(playerid, slotid, 0);
    Character_SetSelectedPhone(playerid, playerNumber);
    new Map:phonebook = PhoneBook_Get(playerNumber);
    new number, name[32];

    new String:str = @("Nome\tNumero\n");
    for_map(i : phonebook)
    {
        number = iter_get_key(i);
        iter_get_md_arr(i, {CONTACT_NAME}, name, 32);
        
        str += str_format("%s\t%d\n", name, number);
    }
    Dialog_Show_s(playerid, Dialog_PhoneBook, DIALOG_STYLE_TABLIST_HEADERS, str_format("Cellulare - %d", playerNumber), str, "Invia", "Chiudi");
    return 1;
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

        

        printf("Loaded PhoneBook for number %d", phoneNumber);
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT id, name, number FROM `phonebook` WHERE number_owner = '%d'", phoneNumber);
    return 1;
}

static UnloadPhoneBook(phoneNumber)
{
    if(!map_has_key(PhoneBooks, phoneNumber))
        return 0;
    map_delete_deep(PhoneBook_Get(phoneNumber));
    printf("Unloaded PhoneBook for number %d", phoneNumber);
    return 1;
}

static ShowContactOptions(playerid, playerNumber, contactNumber)
{
    new name[32];
    PhoneBook_GetContactName(playerNumber, contactNumber, name);
    Dialog_Show_s(playerid, Dialog_PhoneBookContact, DIALOG_STYLE_LIST, str_format("%s - %d", name, contactNumber), @("Chiama\nSMS\nModifica Nome\nCancella"), "Seleziona", "Chiudi");
    return 1;
}

static Map:PhoneBook_Get(number)
{
    if(!map_has_key(PhoneBooks, number))
        LoadPhoneBook(number);
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
    
    inline OnInsert()
    {
        map_set_cell(phonebook, number, CONTACT_ID, cache_insert_id());    
    }
    
    new data[E_CONTACT];
    set(data[CONTACT_NAME], name);
    map_add_arr(phonebook, number, data);

    MySQL_TQueryInline(gMySQL, using inline OnInsert, "INSERT INTO `phonebook` (number_owner, name, number, created_at) VALUES('%d', '%e', '%d', '%d')", ownerNumber, name, number, gettime());
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
        mysql_tquery_f(gMySQL, "DELETE FROM `phonebook` WHERE id = '%d'", id);
    map_remove_deep(phonebook, number);
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
    mysql_tquery_f(gMySQL, "UPDATE `phonebook` SET name = '%e' WHERE id = '%d'", name, id);
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

Dialog:Dialog_PhoneBook(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Character_ShowPhone(playerid, Character_GetSelectedPhoneSlot(playerid));
    
    SelectedContactNumber[playerid] = map_key_at(PhoneBook_Get(Character_GetSelectedPhone(playerid)), listitem);
    printf("%d - %d", map_size(PhoneBook_Get(Character_GetSelectedPhone(playerid))), SelectedContactNumber[playerid]);
    return ShowContactOptions(playerid, Character_GetSelectedPhone(playerid), SelectedContactNumber[playerid]);
}

Dialog:Dialog_PhoneBookContact(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Character_ShowPhoneBook(playerid, Character_GetSelectedPhoneSlot(playerid));
    if(listitem == 0)
    {
        new number[9];
        valstr(number, SelectedContactNumber[playerid]);
        return Character_StartCall(playerid, Character_GetSelectedPhoneSlot(playerid), number);   
    }
    else if(listitem == 1)
        return Dialog_Show_s(playerid, Dialog_PhoneBookSMS, DIALOG_STYLE_INPUT, str_format("Cellulare - %d", SelectedContactNumber[playerid]), @("Inserisci il messaggio che vuoi inviare al contatto.\n\n"), "Invia", "Chiudi");
    else if(listitem == 2)
        return Dialog_Show_s(playerid, Dialog_PhoneBookEditName, DIALOG_STYLE_INPUT, str_format("Cellulare - %d - Modifica Nome", SelectedContactNumber[playerid]), @("Inserisci il nuovo nome da assegnare al contatto. Max. 32 caratteri\n\n"), "Modifica", "Chiudi");
    else if(listitem == 3)
        return Dialog_Show_s(playerid, Dialog_PhoneBookDeleteContact, DIALOG_STYLE_MSGBOX, str_format("Cellulare - %d - Cancella", SelectedContactNumber[playerid]), @("Sei sicuro di voler cancellare definitivamente questo numero dalla rubrica?\n\n"), "Cancella", "Chiudi");
    return 1;
}

Dialog:Dialog_PhoneBookDeleteContact(playerid, response, listitem, inputtext[])
{
    if(!response)
        return ShowContactOptions(playerid, Character_GetSelectedPhone(playerid), SelectedContactNumber[playerid]);
    
    new name[32];
    PhoneBook_GetContactName(Character_GetSelectedPhone(playerid), SelectedContactNumber[playerid], name);

    SendFormattedMessage(playerid, COLOR_YELLOW, "Hai rimosso il contatto (%s) dalla rubrica.", name);
    
    if(PhoneBook_DeleteContact(Character_GetSelectedPhone(playerid), SelectedContactNumber[playerid]))
    {
        return Character_ShowPhoneBook(playerid, Character_GetSelectedPhoneSlot(playerid));
    }
    return 1;
}

Dialog:Dialog_PhoneBookSMS(playerid, response, listitem, inputtext[])
{
    if(!response)
        return ShowContactOptions(playerid, Character_GetSelectedPhone(playerid), SelectedContactNumber[playerid]);
    if(strlen(inputtext) <= 0 || strlen(inputtext) >= 128)
        return Dialog_Show_s(playerid, Dialog_PhoneBookSMS, DIALOG_STYLE_INPUT, str_format("Cellulare - %d", SelectedContactNumber[playerid]), @("{FF0000}Il messaggio inserito è troppo corto o troppo lungo.{FFFFFF}\nInserisci il messaggio che vuoi inviare al contatto.\n\n"), "Invia", "Chiudi");
    Character_SendSMS(playerid, Character_GetSelectedPhoneSlot(playerid), SelectedContactNumber[playerid], inputtext);
    ShowContactOptions(playerid, Character_GetSelectedPhone(playerid), SelectedContactNumber[playerid]);
    return 1;
}

Dialog:Dialog_PhoneBookEditName(playerid, response, listitem, inputtext[])
{
    if(!response)
        return ShowContactOptions(playerid, Character_GetSelectedPhone(playerid), SelectedContactNumber[playerid]);
    if(strlen(inputtext) <= 0 || strlen(inputtext) >= 31)
        return Dialog_Show_s(playerid, Dialog_PhoneBookSMS, DIALOG_STYLE_INPUT, str_format("Cellulare - %d", SelectedContactNumber[playerid]), @("{FF0000}Il nome inserito è troppo corto o troppo lungo.{FFFFFF}\nInserisci il messaggio che vuoi inviare al contatto.\n\n"), "Invia", "Chiudi");
    if(!PhoneBook_SetContactName(Character_GetSelectedPhone(playerid), SelectedContactNumber[playerid], inputtext))
        return 0;
    ShowContactOptions(playerid, Character_GetSelectedPhone(playerid), SelectedContactNumber[playerid]);
    return 1;
}

Dialog:Dialog_PhoneBookAddNumber(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Character_ShowPhone(playerid, Character_GetSelectedPhoneSlot(playerid));
    
    if(!Character_HasPhoneInSlot(playerid, Character_GetSelectedPhoneSlot(playerid)))
        return 0;

    new number, name[32];

    if(sscanf(inputtext, "ds[32]", number, name) || number < 0 || number > 39999999)
        return Dialog_Show(playerid, Dialog_PhoneBookAddNumber, DIALOG_STYLE_INPUT, "Aggiungi numero in rubrica", "{FF0000}Il formato inserito non è giusto o il nome è troppo lungo.{FFFFFF}\nInserisci il numero seguito dal nome che vuoi aggiungere in rubrica.\n== Esempio ==\n44411234 Mario Rossi", "Aggiungi", "Indietro");
    
    Character_AddToPhoneBook(playerid, Character_GetSelectedPhoneSlot(playerid), number, name);
    return 1;
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
