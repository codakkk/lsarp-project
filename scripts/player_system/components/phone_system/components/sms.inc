#include <YSI_Coding\y_hooks>

enum 
{
    SMS_STATE_NONE = -1,
    SMS_STATE_NOT_READ = 0,
    SMS_STATE_READ
}

hook OnGameModeInit()
{
    mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `phone_sms` ( \
						`id` int(11) NOT NULL AUTO_INCREMENT, \
                        `number_owner` int(11) NOT NULL, \
                        `sender_number` int(11) NOT NULL, \
						`addressee_number` INT(11) NOT NULL, \
                        `message` VARCHAR(255) NOT NULL, \
                        `created_at` int(11) NOT NULL, \
                        `state` int(2) NOT NULL, \
						PRIMARY KEY (`id`) \
						) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=latin1");
    return 1;
}

flags:sms(CMD_ALIVE_USER);
CMD:sms(playerid, params[])
{
    new slot, number[9], message[128];
    if(sscanf(params, "k<slotid>s[9]s[128]", slot, number, message))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /sms <slot> <numero/nome rubrica> <messaggio>");
    
    if(!Character_HasPhoneInSlot(playerid, slot))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un cellulare in questo slot.");
    new n = PhoneBook_ParseValue(Character_GetSlotExtra(playerid, slot, 0), number);
    Character_SendSMS(playerid, slot, n, message);    
    return 1;
}

hook OnCharacterLoadPhone(playerid, number)
{
    inline OnLoadSMS()
    {
        if(cache_num_rows() > 0)
            SendFormattedMessage(playerid, COLOR_GREEN, "[CELLULARE %d] Hai %d messaggi non letti.", number, cache_num_rows());
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoadSMS, "SELECT * FROM `phone_sms` WHERE number_owner = '%d' AND state = '%d'", number, SMS_STATE_NOT_READ);
    
    inline OnLoadCalls()
    {
        if(cache_num_rows() > 0)
            SendFormattedMessage(playerid, COLOR_GREEN, "[CELLULARE %d] Hai %d chiamate perse.", number, cache_num_rows());
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoadCalls, "SELECT * FROM `phone_call_registry` WHERE number_owner = '%d' AND state = '%d'", number, SMS_STATE_NOT_READ);
    return 1;
}

stock Character_SendSMS(playerid, slot, number, const msg[])
{
    if(Phone_IsOff(Character_GetInventory(playerid), slot))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi inviare un SMS con il cellulare spento.");
    
    if(strlen(msg) <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi inviare un messaggio vuoto.");
    
    new senderNumber = Character_GetSlotExtra(playerid, slot, 0);
    
    if(number <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Il numero inserito non è valido."), 0;
    
    if(number == senderNumber)
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi inviare un SMS a te stesso."), 0;

    mysql_pquery_f(gMySQL, "INSERT INTO `phone_sms` (number_owner, sender_number, addressee_number, message, created_at, state) VALUES('%d', '%d', '%d', '%e', '%d', '%d')", number, senderNumber, number, msg, gettime(), SMS_STATE_NOT_READ);
    mysql_pquery_f(gMySQL, "INSERT INTO `phone_sms` (number_owner, sender_number, addressee_number, message, created_at, state) VALUES('%d', '%d', '%d', '%e', '%d', '%d')", senderNumber, senderNumber, number, msg, gettime(), SMS_STATE_READ);
    
    new name[32];
    if(PhoneBook_GetContactName(senderNumber, number, name))
        SendFormattedMessage(playerid, COLOR_YELLOW, "SMS inviato al contatto %s. Testo: %s", name, msg);
    else
        SendFormattedMessage(playerid, COLOR_YELLOW, "SMS inviato al numero %d. Testo: %s", number, msg);

    new entityId, EntityType:entityType, phoneSlotId;
    if(World_FindPhoneByNumber(number, entityType, entityId, phoneSlotId))
    {
        new Inventory:entityInv = Inventory_Get(entityType, entityId);
        if(!Phone_IsOff(entityInv, phoneSlotId))
        {
            if(entityType == ENTITY_TYPE_PLAYER)
            {
                if(PhoneBook_GetContactName(number, senderNumber, name))
                {
                    SendFormattedMessage(entityId, COLOR_GREEN, "SMS ricevuto da %s. Usa il tuo cellulare (/cell <slot>) per leggere il messaggio.", name);
                }
                else
                {
                    SendFormattedMessage(entityId, COLOR_GREEN, "SMS ricevuto da %d. Usa il tuo cellulare per leggere il messaggio.", senderNumber);
                }
            }
            else if(entityType == ENTITY_TYPE_DROP)
            {
                new Float:x, Float:y, Float:z;
                Drop_GetPosition(entityId, x, y, z);
                SendRangedMessage(10.0, x, y, z, Drop_GetVirtualWorld(entityId), Drop_GetInterior(entityId), "(( * Si sente lo squillo di un SMS ))", COLOR_FADE1, COLOR_FADE2, COLOR_FADE3, COLOR_FADE4, COLOR_FADE5);
            }
        }
    }
    Character_Me(playerid, "prende il cellulare ed invia un messaggio.");
    return 1;
}

stock Character_ShowRegistry(playerid, slotid, bool:sentByPlayer, smsState = SMS_STATE_NONE)
{
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return;
    
    new playerNumber = Character_GetSlotExtra(playerid, slotid, 0);
    new List:temp = list_new(),
        String:string = str_new(""),
        String:condition = str_new("");
    
    pawn_guard(string);
    pawn_guard(condition);
    
    str_append_format(condition, "number_owner = '%d' ", playerNumber, smsState);
    str_append_format(condition, sentByPlayer ? ("AND sender_number = '%d' ") : ("AND sender_number != '%d' "), playerNumber);
    
    if(smsState != SMS_STATE_NONE)
        str_append_format(condition, "AND state = '%d'", smsState);
    
    new response[e_DIALOG_RESPONSE_INFO];
    
    new
        id, sender, addressee, owner, time, year, month, day, hour, minute, second,
        name[32], rows = 0;
    
    yield 1;
    
    for(;;)
    {
        if(!Character_HasPhoneInSlot(playerid, slotid))
            break;

        await mysql_aquery_s(gMySQL, str_format("SELECT * FROM `phone_sms` WHERE %S", condition));
        rows = cache_num_rows();

        if(rows <= 0)
        {
            SendClientMessage(playerid, COLOR_ERROR, "Non hai messaggi in questo registro.");
            break;
        }
        
        str_clear(string);
        str_append_format(string, "Numero/Nome\tInviato il\n");

        list_clear(temp);

        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", id);
            cache_get_value_name_int(i, "owner_number", owner);
            cache_get_value_name_int(i, "sender_number", sender);
            cache_get_value_name_int(i, "addressee_number", addressee);
            cache_get_value_name_int(i, "created_at", time);

            TimestampToDate(time, year, month, day, hour, minute, second, 1);
            
            new show = sentByPlayer ? addressee : sender;
            printf("Show: %d", show);
            PhoneBook_GetContactName(playerNumber, show, name);

            str_append_format(string, "%s\t%d/%d/%d - %d:%d\n", name, day, month, year, hour, minute);
            
            
            list_add(temp, id);
        }

        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_TABLIST_HEADERS, @("Messaggi non letti"), string, @("Visualizza"), @("Indietro"));

        if(!response[E_DIALOG_RESPONSE_Response]) break;
        
        Character_ShowMessage(playerid, playerNumber, list_get(temp, response[E_DIALOG_RESPONSE_Listitem]));
    }

    list_delete(temp);
}

static Character_ShowMessage(playerid, playerNumber, smsid)
{
    new response[e_DIALOG_RESPONSE_INFO];
    yield 1;
    new message[128], senderNumber, addresseeNumber, createdAt, messState,
        year, month, day, hour, minute, second,
        String:info = str_new(""), name[32];
    pawn_guard(info);
    for(;;)
    {
        str_clear(info);
        await mysql_aquery_s(gMySQL, str_format("SELECT * FROM `phone_sms` WHERE id = '%d'", smsid));
        
        if(cache_num_rows() <= 0)
            break;
        
        cache_get_value_name(0, "message", message);
        cache_get_value_name_int(0, "sender_number", senderNumber);
        cache_get_value_name_int(0, "addressee_number", addresseeNumber);
        cache_get_value_name_int(0, "created_at", createdAt);
        cache_get_value_name_int(0, "state", messState);

        TimestampToDate(createdAt, year, month, day, hour, minute, second, 2);

        if(senderNumber == playerNumber)
        {
            if(PhoneBook_GetContactName(playerNumber, addresseeNumber, name))
                str_append_format(info, "Destinatario: %s\nInviato il: %d/%d/%d alle %d/%d.\n\n", name, day, month, year, hour, minute);
            else
                str_append_format(info, "Destinatario: %d\nInviato il: %d/%d/%d alle %d/%d.\n\n", addresseeNumber, day, month, year, hour, minute);
        }
        else
        {
            if(PhoneBook_GetContactName(playerNumber, senderNumber, name))
                str_append_format(info, "Mittente: %s\nRicevuto il: %d/%d/%d alle %d/%d.\n\n", name, day, month, year, hour, minute);
            else
                str_append_format(info, "Mittente: %d\nRicevuto il: %d/%d/%d alle %d/%d.\n\n", senderNumber, day, month, year, hour, minute);
        }
        
        str_append_format(info, "========= CONTENUTO ==========\n\n%s", message);

        await mysql_aquery_s(gMySQL, str_format("UPDATE `phone_sms` SET state = '%d' WHERE id = '%d'", SMS_STATE_READ, smsid), .parallel=true);

        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_MSGBOX, @("SMS"), info, @("Opzioni"), @("Indietro"));

        if(!response[E_DIALOG_RESPONSE_Response]) break;

        HandleSMSDialog(playerid, smsid);
    }
}

static HandleSMSDialog(playerid, smsid)
{
    new response[e_DIALOG_RESPONSE_INFO];
    yield 1;
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_LIST, @("SMS"), @("Elimina"), @("Seleziona"), @("Indietro"));

        if(!response[E_DIALOG_RESPONSE_Response]) break;

        await mysql_aquery_s(gMySQL, str_format("DELETE FROM `phone_sms` WHERE id = '%d'", smsid), .parallel=true);

        break;
    }
}