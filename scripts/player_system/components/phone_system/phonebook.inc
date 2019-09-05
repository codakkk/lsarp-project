
#include <YSI_Coding\y_hooks>

static enum _:E_CONTACT
{
    CONTACT_ID,
    CONTACT_NAME[32]
};

static Map:PhoneBooks; // Map<number, Map<number, E_CONTACT>>

hook OnGameModeInit()
{
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

hook OnCharacterPhoneLoad(playerid, number)
{
    LoadPhoneBook(number);
    return 1;
}

hook OnCharacterPhoneUnload(playerid, number)
{
    UnloadPhoneBook(number);
    return 1;
}

stock Character_ShowPhoneBook(playerid, slotid)
{
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un cellulare in questo slot.");
    
    new playerNumber = Character_GetSlotExtra(playerid, slotid, 0);
    
    new Map:phonebook = Map:map_get(PhoneBooks, playerNumber);
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

    if(HasPhoneBookContact(playerNumber, number))
        return SendClientMessage(playerid, COLOR_ERROR, "Hai già salvato questo contatto. Controlla la rubrica.");

    if(AddPhoneBookContact(playerNumber, number, name))
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai aggiunto il numero %d alla rubrica con il nome %s.", number, name);
    else
        SendClientMessage(playerid, COLOR_ERROR, "Non è stato possibile aggiungere il numero alla rubrica. Errore sconosciuto.");
    return 1;
}

static bool:AddPhoneBookContact(ownerNumber, number, const name[])
{
    if(!map_has_key(PhoneBooks, ownerNumber))
        return false;
    new Map:phonebook = Map:map_get(PhoneBooks, ownerNumber);
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

static bool:DeletePhoneBookContact(ownerNumber, number)
{
    if(!map_has_key(PhoneBooks, ownerNumber))
        return false;
    new Map:phonebook = map_get(PhoneBooks, ownerNumber);
    if(!map_has_key(phonebook, number))
        return false;
    mysql_tquery_f(gMySQL, "DELETE FROM `phonebook` WHERE id = '%d'", map_get(phonebook, number, CONTACT_ID));
    map_remove_deep(phonebook, number);
    return true;
}

static GetPhoneBookNumberName(ownerNumber, number, name[])
{
    if(!map_has_key(PhoneBooks, ownerNumber))
        return 0;
    new Map:phonebook = Map:map_get(PhoneBooks, ownerNumber);
    if(!map_has_key(phonebook, number))
        return 0;
    new Iter:iter = map_iter_at(phonebook, number);
    iter_get_md_arr(iter, {CONTACT_NAME}, name, 32);
    return 1;
}

static bool:HasPhoneBookContact(ownerNumber, number)
{
    if(!map_has_key(PhoneBooks, ownerNumber))
        return false;
    new Map:phonebook = Map:map_get(PhoneBooks, ownerNumber);
    return map_has_key(phonebook, number);
}

static LoadPhoneBook(phoneNumber)
{
    if(map_has_key(PhoneBooks, phoneNumber))
        return 0;
    inline OnLoad()
    {
        new Map:map = map_new();

        new rows = cache_num_rows();

        new data[E_CONTACT], number;
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", data[CONTACT_ID]);
            cache_get_value_name(i, "name", data[CONTACT_NAME]);
            cache_get_value_name_int(i, "number", number);
            map_add_arr(map, number, data);
        }

        map_add(PhoneBooks, phoneNumber, map);

        printf("Loaded PhoneBook for number %d", phoneNumber);
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT id, name, number FROM `phonebook` WHERE number_owner = '%d'", phoneNumber);
    return 1;
}

static UnloadPhoneBook(phoneNumber)
{
    if(!map_has_key(PhoneBooks, phoneNumber))
        return 0;
    map_delete_deep(Map:map_get(PhoneBooks, phoneNumber));
    printf("Unloaded PhoneBook for number %d", phoneNumber);
    return 1;
}

Dialog:Dialog_PhoneBookAddNumber(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Character_ShowPhone(playerid, SelectedPhone{playerid});
    
    if(!Character_HasPhoneInSlot(playerid, SelectedPhone{playerid}))
        return 0;

    new number, name[32];

    if(sscanf(inputtext, "ds[32]", number, name) || number < 0 || number > 39999999)
        return Dialog_Show(playerid, Dialog_PhoneBookAddNumber, DIALOG_STYLE_INPUT, "Aggiungi numero in rubrica", "{FF0000}Il formato inserito non è giusto o il nome è troppo lungo.{FFFFFF}\nInserisci il numero seguito dal nome che vuoi aggiungere in rubrica.\n== Esempio ==\n44411234 Mario Rossi", "Aggiungi", "Indietro");
    
    Character_AddToPhoneBook(playerid, SelectedPhone{playerid}, number, name);
    return 1;
}