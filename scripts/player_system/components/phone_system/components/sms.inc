#include <YSI_Coding\y_hooks>

static enum 
{
    SMS_STATE_NOT_READ = 0,
    SMS_STATE_READ
}

static 
    SelectedMessage[MAX_PLAYERS],
    List:MessagesList[MAX_PLAYERS]
;

hook OnCharacterClearData(playerid)
{
    SelectedMessage[playerid] = 0;
    if(list_valid(MessagesList[playerid]))
        list_delete(MessagesList[playerid]);
    return 1;
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

stock Character_ShowUnReadPhoneMess(playerid, slotid)
{
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return 0;
    
    new playerNumber = Character_GetSlotExtra(playerid, slotid, 0);
    await mysql_aquery_s(gMySQL, str_format("SELECT * FROM `phone_sms` WHERE number_owner = '%d' AND sender_number != '%d' AND state = '%d'", playerNumber, playerNumber, SMS_STATE_NOT_READ));
    new rows = cache_num_rows();

    if(rows <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai messaggi non letti in questo cellulare.");

    if(list_valid(MessagesList[playerid]))
        list_clear(MessagesList[playerid]);
    else
        MessagesList[playerid] = list_new();
    
    new String:str = @("Numero/Nome\tInviato il\n");
    new id, sender, time, year, month, day, hour, minute, second,
        name[32];
    for(new i = 0; i < rows; ++i)
    {
        cache_get_value_name_int(i, "id", id);
        cache_get_value_name_int(i, "sender_number", sender);
        cache_get_value_name_int(i, "created_at", time);

        TimestampToDate(time, year, month, day, hour, minute, second, 1);
        
        if(PhoneBook_GetContactName(playerNumber, sender, name))
            str += str_format("%s\t%d/%d/%d - %d:%d\n", name, day, month, year, hour, minute);
        else
            str += str_format("%d\t%d/%d/%d - %d:%d\n", sender, day, month, year, hour, minute);
        list_add(MessagesList[playerid], id);
    }

    Dialog_Show_s(playerid, Dialog_ShowPhoneMessage, DIALOG_STYLE_TABLIST_HEADERS, @("Messaggi non letti"), str, "Visualizza", "Indietro");
    return 1;
}

stock Character_ShowInBoundPhoneMess(playerid, slotid)
{
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return 0;
    
    new playerNumber = Character_GetSlotExtra(playerid, slotid, 0);

    inline OnLoad()
    {
        new rows = cache_num_rows();

        if(rows <= 0)
            return SendClientMessage(playerid, COLOR_ERROR, "Non hai messaggi non letti in questo cellulare.");

        if(list_valid(MessagesList[playerid]))
            list_clear(MessagesList[playerid]);
        else
            MessagesList[playerid] = list_new();
        
        new String:str = @("Mittente\tInviato il\n");
        new id, sender, time, year, month, day, hour, minute, second, name[32];
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", id);
            cache_get_value_name_int(i, "sender_number", sender);
            cache_get_value_name_int(i, "created_at", time);

            TimestampToDate(time, year, month, day, hour, minute, second, 1);
            if(PhoneBook_GetContactName(playerNumber, sender, name))
                str += str_format("%s\t%d/%d/%d - %d:%d\n", name, day, month, year, hour, minute);
            else
                str += str_format("%d\t%d/%d/%d - %d:%d\n", sender, day, month, year, hour, minute);

            list_add(MessagesList[playerid], id);
        }

        Dialog_Show_s(playerid, Dialog_ShowPhoneMessage, DIALOG_STYLE_TABLIST_HEADERS, @("Messaggi in entrata"), str, "Visualizza", "Indietro");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `phone_sms` WHERE number_owner = '%d' AND sender_number != '%d' AND state = '%d'", playerNumber, playerNumber, SMS_STATE_READ);
    return 1;
}

stock Character_ShowOutBoundPhoneMess(playerid, slotid)
{
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return 0;

    new playerNumber = Character_GetSlotExtra(playerid, slotid, 0);
    
    inline OnLoad()
    {
        new rows = cache_num_rows();

        if(rows <= 0)
            return SendClientMessage(playerid, COLOR_ERROR, "Non hai messaggi non letti in questo cellulare.");

        if(list_valid(MessagesList[playerid]))
            list_clear(MessagesList[playerid]);
        else
            MessagesList[playerid] = list_new();

        new String:str = @("Destinatario\tInviato il\n");
        new id, addressee, time, year, month, day, hour, minute, second, name[32];
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", id);
            cache_get_value_name_int(i, "addressee_number", addressee);
            cache_get_value_name_int(i, "created_at", time);

            TimestampToDate(time, year, month, day, hour, minute, second, 1);
        
            if(PhoneBook_GetContactName(playerNumber, addressee, name))
                str += str_format("%s\t%d/%d/%d - %d:%d\n", name, day, month, year, hour, minute);
            else
                str += str_format("%d\t%d/%d/%d - %d:%d\n", addressee, day, month, year, hour, minute);

            list_add(MessagesList[playerid], id);
        }

        Dialog_Show_s(playerid, Dialog_ShowPhoneMessage, DIALOG_STYLE_TABLIST_HEADERS, @("Messaggi in uscita"), str, "Visualizza", "Indietro");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `phone_sms` WHERE number_owner = '%d' AND sender_number = '%d'", playerNumber, playerNumber);
    return 1;
}

stock Character_ReadMessage(playerid, slotid, messageid)
{
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return 0;

    if(messageid <= 0)
        return 0;
    
    new playerNumber = Character_GetSlotExtra(playerid, slotid, 0);

    inline OnLoad()
    {
        new message[128], senderNumber, addresseeNumber, createdAt, messState;
        
        cache_get_value_name(0, "message", message);
        cache_get_value_name_int(0, "sender_number", senderNumber);
        cache_get_value_name_int(0, "addressee_number", addresseeNumber);
        cache_get_value_name_int(0, "created_at", createdAt);
        cache_get_value_name_int(0, "state", messState);

        new year, month, day, hour, minute, second;

        TimestampToDate(createdAt, year, month, day, hour, minute, second, 2);

        new String:info, name[32];

        if(senderNumber == playerNumber)
        {
            if(PhoneBook_GetContactName(playerNumber, addresseeNumber, name))
                info += str_format("Destinatario: %s\nInviato il: %d/%d/%d alle %d/%d.\n\n", name, day, month, year, hour, minute);
            else
                info += str_format("Destinatario: %d\nInviato il: %d/%d/%d alle %d/%d.\n\n", addresseeNumber, day, month, year, hour, minute);
        }
        else
        {
            if(PhoneBook_GetContactName(playerNumber, senderNumber, name))
                info += str_format("Mittente: %s\nRicevuto il: %d/%d/%d alle %d/%d.\n\n", name, day, month, year, hour, minute);
            else
                info += str_format("Mittente: %d\nRicevuto il: %d/%d/%d alle %d/%d.\n\n", senderNumber, day, month, year, hour, minute);
        }
        
        info += str_format("========= CONTENUTO ==========\n\n%s", message);

        mysql_pquery_f(gMySQL, "UPDATE `phone_sms` SET state = '%d' WHERE id = '%d'", SMS_STATE_READ, messageid);
        
        SelectedMessage[playerid] = messageid;

        Dialog_Show_s(playerid, Dialog_PhoneMessageOption, DIALOG_STYLE_MSGBOX, @("SMS"), info, "Opzioni", "Indietro");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `phone_sms` WHERE id = '%d'", messageid);
    return 1;
}
/*
Dialog:Dialog_PhoneMessageOption(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Character_ShowPhone(playerid, Character_GetSelectedPhoneSlot(playerid));
        
    if(!Character_HasPhoneInSlot(playerid, Character_GetSelectedPhoneSlot(playerid)))
        return 0;
    
    Dialog_Show(playerid, Dialog_SelectMessageOption, DIALOG_STYLE_LIST, "Opzioni Messaggio", "Cancella", "Seleziona", "Indietro");
    return 1;
}

Dialog:Dialog_SelectMessageOption(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Character_ShowPhone(playerid, Character_GetSelectedPhoneSlot(playerid));
    
    if(!Character_HasPhoneInSlot(playerid, Character_GetSelectedPhoneSlot(playerid)))
        return 0;

    new selectedMessage = SelectedMessage[playerid];
    SendClientMessage(playerid, COLOR_GREEN, "Hai cancellato il messaggio con successo.");

    mysql_pquery_f(gMySQL, "DELETE FROM `phone_sms` WHERE id = '%d'", selectedMessage);
    return 1;
}

Dialog:Dialog_ShowPhoneMessage(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Character_ShowPhone(playerid, Character_GetSelectedPhoneSlot(playerid));
    
    if(!Character_HasPhoneInSlot(playerid, Character_GetSelectedPhoneSlot(playerid)))
        return 0;

    new id = list_get(MessagesList[playerid], listitem);
    
    Character_ReadMessage(playerid, Character_GetSelectedPhoneSlot(playerid), id);
    return 1;
}*/