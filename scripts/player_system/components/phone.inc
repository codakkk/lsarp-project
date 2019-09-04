
// ADD POSSIBILITY TO INTERACT WITH PHONEBOOK USERS 

#include <inventory_system\core>
#include <YSI_Coding\y_hooks>

#define POLICE_PHONE_NUMBER         (911)

static enum 
{
    SMS_STATE_NOT_READ = 0,
    SMS_STATE_READ
}

static 
    CallWith[MAX_PLAYERS] = {INVALID_PLAYER_ID, ...},
    CallNumber[MAX_PLAYERS] = {0, ...},
    IncomingCall[MAX_PLAYERS] = {INVALID_PLAYER_ID, ...},
    SelectedPhone[MAX_PLAYERS char] = {-1, ...}
;

static PlayerEmergencyStatus[MAX_PLAYERS char] = {0, ...};

static 
    SelectedMessage[MAX_PLAYERS],
    List:MessagesList[MAX_PLAYERS]
;

hook OnGameModeInit()
{
    mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `phone_data` ( \
						`id` int(11) NOT NULL AUTO_INCREMENT, \
						`character_id` int(11) NOT NULL DEFAULT '0', \
                        `created_at` int(11) NOT NULL DEFAULT '0', \
                        `number` int(11) NOT NULL DEFAULT '0', \
                        `building` int(11) NOT NULL DEFAULT '0', \
						PRIMARY KEY (`id`), \
                        KEY (`number`) \
						) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=latin1");
    
    mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `phonebook` ( \
                        `id` int(11) NOT NULL AUTO_INCREMENT, \
						`number_owner` int(11) NOT NULL, \
						`name` VARCHAR(32) NOT NULL, \
                        `number` int(11) NOT NULL, \
                        `created_at` int(11) NOT NULL, \
						PRIMARY KEY (`id`) \
						) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=latin1");
    
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

hook OnPlayerClearData(playerid)
{
    if(Character_IsInCall(playerid))
    {
        Character_StopCall(playerid);
    }
    CallWith[playerid] = INVALID_PLAYER_ID;
    CallNumber[playerid] = 0;
    IncomingCall[playerid] = INVALID_PLAYER_ID;
    SelectedPhone{playerid} = -1;
    PlayerEmergencyStatus{playerid} = 0;
    SelectedMessage[playerid] = 0;
    if(list_valid(MessagesList[playerid]))
        list_delete(MessagesList[playerid]);
    return 1;
}

hook OnCharacterFirstSpawn(playerid)
{
    Character_GivePhone(playerid);
    SendClientMessage(playerid, COLOR_GREEN, "Ti è stato dato un cellulare. Usa /cellcmds per i comandi o (/inv)entario per gestirlo.");
    return 1;
}

hook OnCharacterLateLoad(playerid)
{
    for(new i = 0, j = Character_GetInventorySize(playerid); i < j; ++i)
    {
        if(Character_GetSlotItem(playerid, i) != gItem_Phone)
            continue;
        new number = Character_GetSlotExtra(playerid, i, 0);
        inline OnLoad()
        {
            if(cache_num_rows() > 0)
                SendFormattedMessage(playerid, COLOR_GREEN, "[CELLULARE %d] Hai %d messaggi non letti.", number, cache_num_rows());
        }
        MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `phone_sms` WHERE number_owner = '%d' AND state = '%d'", number, SMS_STATE_NOT_READ);
        
    }
    return 1;
}

hook OnCharacterDeath(playerid, killerid, reason)
{
    if(Character_IsInCall(playerid))
        Character_StopCall(playerid);
    return 1;
}

hook OnCharacterUseItem(playerid, slotid, itemid)
{
    if(itemid != gItem_Phone)
        return 0;
    Character_ShowPhone(playerid, slotid);
    return 1;
}

hook OnCharacterTalk(playerid, text[])
{
    if(!Character_IsInCall(playerid))
        return 0;
    new call = Character_GetCallPlayer(playerid);
    if(Character_IsLogged(call))
    {
        SendFormattedMessage(call, -1, "[Telefono] %s dice: %s", Character_GetRolePlayName(playerid), text);
    }
    else if(Character_GetEmergencyStatus(playerid) == 1)
    {
        if(strfind(text, "polizia", true) != -1)
        {
            PlayerEmergencyStatus{playerid} = 2;
            SendClientMessage(playerid, COLOR_GREY, "[OPERATORE] Descriva l'emergenza in poche parole per il Las Venturas Police Department.");
        }
        else if(strfind(text, "medici", true) != -1)
        {
            PlayerEmergencyStatus{playerid} = 3;
            SendClientMessage(playerid, COLOR_GREY, "[OPERATORE] Descriva l'emergenza in poche parole per il Las Venturas Medical Department.");
        }
        else SendClientMessage(playerid, COLOR_GREY, "[OPERATORE] Ci dica di cosa ha bisogno (polizia o medici).");
    }
    else if(Character_GetEmergencyStatus(playerid) >= 2)
    {
        SendClientMessage(playerid, COLOR_GREY, "[OPERATORE] La richiesta è stata presa in carico.");
        SendFormattedMessage(playerid, COLOR_GREY, "Testo: %s", text);

        new zone[32], Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);
        Get2DZoneName(x, y, z, zone, sizeof(zone));

        new factionid, type = PlayerEmergencyStatus{playerid} == 2 ? FACTION_TYPE_POLICE : FACTION_TYPE_MEDICAL;
        for_map(i : GetFactionsMap())
        {
            factionid = iter_get_key(i);
            if(Faction_GetType(factionid) == type)
            {
                Faction_SendMessage(factionid, "** HQ: Chiamata al centralino");
                Faction_SendMessage(factionid, "Chiamata da: %s - Numero: %d - Zona: %s", Character_GetRolePlayName(playerid), CallNumber[playerid], zone);
                Faction_SendMessage(factionid, "Emergenza: %s", text);
            }
        }

        Character_StopCall(playerid);
    }
    return 1;
}

flags:cellcmds(CMD_USER);
CMD:cellcmds(playerid, params[])
{
    SendClientMessage(playerid, -1, "[TELEFONE]: (/cell)ulare - /chiama - (/ri)spondi - (/ch)iudi - /sms - /rubrica - /cellinfo");    
    return 1;
}

flags:cellulare(CMD_ALIVE_USER);
CMD:cellulare(playerid, params[])
{
    new slotid = strval(params);
    
    if(!Character_ShowPhone(playerid, slotid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un cellulare in questo slot.");
    return 1;
}
alias:cellulare("cell");

flags:cellinfo(CMD_USER);
CMD:cellinfo(playerid, params[])
{
    new slotid;
    if(sscanf(params, "d", slotid))
        return SendClientMessage(playerid, COLOR_ERROR, "/cellinfo <slotid>");
    
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un telefono in questo slot.");
    
    new number = Character_GetSlotExtra(playerid, slotid, 0);

    inline OnLoad()
    {
        if(cache_num_rows() <= 0)
            return SendClientMessage(playerid, COLOR_ERROR, "Il cellulare non ha un numero valido.");

        new time, year, month, day, hour, minute, second;
        cache_get_value_name_int(0, "created_at", time);

        TimestampToDate(time, year, month, day, hour, minute, second, 1);

        SendFormattedMessage(playerid, -1, "[Cellulare] Numero: %d - Acquistato il %d/%d/%d alle %d:%d.", number, day, month, year, hour, minute);
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT created_at FROM `phone_data` WHERE number = '%d'", number);
    //TimestampToDate(, &year, &month, &day, &hour, &minute, &second, HourGMT, MinuteGMT = 0)
    return 1;
}

flags:chiama(CMD_ALIVE_USER);
CMD:chiama(playerid, params[])
{
    if(!Character_HasItem(playerid, gItem_Phone, 1))
        return SendClientMessage(playerid, COLOR_ERROR, "Devi possedere un cellulare per poter utilizzarlo.");
    
    if(Character_GetCallPlayer(playerid) != INVALID_PLAYER_ID)
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se sei già in una chiamata.");

    new slot, number;
    if(sscanf(params, "dd", slot, number))
        return SendClientMessage(playerid, COLOR_ERROR, "/chiama <slot> <numero>");

    Character_StartCall(playerid, slot, number);
    return 1;
}

flags:rispondi(CMD_ALIVE_USER);
CMD:rispondi(playerid, params[])
{
    new slot;
    if(sscanf(params, "d", slot))
        return SendClientMessage(playerid, COLOR_ERROR, "(/ri)spondi <slot>");
    
    if(!Character_HasPhoneInSlot(playerid, slot))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un cellulare in questo slot.");
    
    if(IncomingCall[playerid] == INVALID_PLAYER_ID)
        return SendClientMessage(playerid, COLOR_ERROR, "Non stai ricevendo una chiamata.");
    
    new incomingFrom = IncomingCall[playerid];
    new requestedNumber = CallNumber[incomingFrom];

    if(requestedNumber != Character_GetSlotExtra(playerid, slot))
        return SendClientMessage(playerid, COLOR_ERROR, "Non stai ricevendo chiamate se questo numero.");

    SendFormattedMessage(incomingFrom, COLOR_GREEN, "%s ha risposto alla chiamata.", Character_GetRolePlayName(playerid));
    SendClientMessage(playerid, COLOR_GREEN, "Hai risposto alla chiamata.");

    CallNumber[incomingFrom] = 0;
    IncomingCall[playerid] = INVALID_PLAYER_ID;

    CallWith[playerid] = incomingFrom;
    CallWith[incomingFrom] = playerid;
    
    SetPlayerSpecialAction(playerid, SPECIAL_ACTION_USECELLPHONE);
	SetPlayerAttachedObject(playerid, 6, 330, 6, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 1.000000, 1.000000);

    Character_Me(playerid, "risponde al cellulare.");
    return 1;
}
alias:rispondi("ri");

flags:chiudi(CMD_ALIVE_USER);
CMD:chiudi(playerid, params[])
{
    Character_StopCall(playerid);
    return 1;
}
alias:chiudi("ch");

flags:rubrica(CMD_ALIVE_USER);
CMD:rubrica(playerid, params[])
{
    new slotid;
    if(sscanf(params, "d", slotid))
        return SendClientMessage(playerid, COLOR_ERROR, "/rubrica <slotid>");
    Character_ShowPhoneBook(playerid, slotid);
    return 1;
}

flags:sms(CMD_ALIVE_USER);
CMD:sms(playerid, params[])
{
    new slot, number, message[128];
    if(sscanf(params, "dds[128]", slot, number, message))
        return SendClientMessage(playerid, COLOR_ERROR, "/sms <slot> <numero> <messaggio>");
    
    if(!Character_HasPhoneInSlot(playerid, slot))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un cellulare in questo slot.");
    
    Character_SendSMS(playerid, slot, number, message);    
    return 1;
}

stock Character_GivePhone(playerid)
{
    new number = Server_GeneratePhoneNumber(),
        result = Character_GiveItem(playerid, gItem_Phone, 1, number);
    mysql_tquery_f(gMySQL, "INSERT INTO `phone_data` (character_id, created_at, number) VALUES('%d', '%d', '%d')", Character_GetID(playerid), gettime(), number);
    return result;
}


stock Character_GiveAnonymousePhone(playerid)
{
    new number = Server_GeneratePhoneNumber(),
        result = Character_GiveItem(playerid, gItem_Phone, 1, number);
    mysql_tquery_f(gMySQL, "INSERT INTO `phone_data` (created_at, number) VALUES('%d', '%d')", gettime(), number);
    return result;
}


static Character_StartCall(playerid, slotid, number)
{
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un telefono in questo slot."), 0;
    
    if(number <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Il numero inserito non esiste o non è al momento raggiungibile. Riprova più tardi."), 0;

    CallNumber[playerid] = number;

    Character_AMe(playerid, "prende il cellulare e digita un numero.");

    SetPlayerSpecialAction(playerid,SPECIAL_ACTION_USECELLPHONE);
	SetPlayerAttachedObject(playerid, 6, 330, 6, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 1.000000, 1.000000);

    if(number == POLICE_PHONE_NUMBER)
    {
        SendClientMessage(playerid, COLOR_GREY, "[OPERATORE] Ci dica di cosa ha bisogno (polizia o medici).");
        PlayerEmergencyStatus{playerid} = 1;
        return 1;
    }

    new isValid = Character_FindByPhoneNumber(number);
    
    if(isValid == INVALID_PLAYER_ID)
        return SendClientMessage(playerid, COLOR_ERROR, "Il numero inserito non esiste o non è al momento raggiungibile. Riprova più tardi."), 0;
    
    if(Character_GetCallPlayer(isValid) != INVALID_PLAYER_ID)
        return SendClientMessage(playerid, COLOR_ERROR, "Il numero chiamato è al momento occupato."), 0;
    
    CallWith[playerid] = isValid;
    IncomingCall[isValid] = playerid;

    Character_Do(isValid, "Il cellulare comincia a squillare...");
    
    new playerNumber = Character_GetSlotExtra(playerid, slotid, 0);

    inline OnLoad()
    {
        if(cache_num_rows() > 0)
        {
            new name[32];
            cache_get_value_name(0, "name", name);
            SendFormattedMessage(playerid, COLOR_GREEN, "Stai chiamando %s al numero %d.", name, number);
        }
        else
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Stai chiamando il numero %d.", number);
        }
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT name FROM phonebook WHERE number_owner = '%d' AND number = '%d'", playerNumber, number);
    return 1;
}

stock Character_StopCall(playerid)
{
    if(!Character_IsInCall(playerid))
        return 1;
    
    if(IncomingCall[playerid] != INVALID_PLAYER_ID || CallWith[playerid] != INVALID_PLAYER_ID)
    {
        new other = IncomingCall[playerid] != INVALID_PLAYER_ID ? IncomingCall[playerid] : CallWith[playerid];
        if(Character_IsLogged(other))
        {
            SendFormattedMessage(other, COLOR_GREEN, "%s ha chiuso la chiamata.", Character_GetRolePlayName(playerid));

            if(GetPlayerSpecialAction(other) == SPECIAL_ACTION_USECELLPHONE)
            {
                SetPlayerSpecialAction(other, SPECIAL_ACTION_STOPUSECELLPHONE);            
            }
            CallWith[other] = IncomingCall[other] = INVALID_PLAYER_ID;
            CallNumber[playerid] = 0;
            RemovePlayerAttachedObject(other, 6);
        }
    }

    CallNumber[playerid] = 0;
    CallWith[playerid] = IncomingCall[playerid] = INVALID_PLAYER_ID;
    PlayerEmergencyStatus{playerid} = 0;

    RemovePlayerAttachedObject(playerid, 6);
    SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
    
    SendClientMessage(playerid, COLOR_GREEN, "Hai chiuso la chiamata.");
    return 1;
}

stock Character_ShowPhoneBook(playerid, slotid)
{
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un cellulare in questo slot.");
    
    new playerNumber = Character_GetSlotExtra(playerid, slotid, 0);
    
    inline OnLoad()
    {
        new rows = cache_num_rows();
        if(rows <= 0)
            return SendClientMessage(playerid, COLOR_ERROR, "La rubrica è vuota.");

        new String:str = @("Nome\tNumero\tData Creazione\n");

        new otherNumber, createdAt, name[32],
            year, month, day, hour, minutes, seconds;
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "number", otherNumber);
            cache_get_value_name_int(i, "created_at", createdAt);
            cache_get_value_name(i, "name", name);
            
            TimestampToDate(createdAt, year, month, day, hour, minutes, seconds, 1, 0);

            str += str_format("%s\t%d\t%d/%d/%d\n", name, otherNumber, day, month, year);
            
            Dialog_Show_s(playerid, Dialog_PhoneBook, DIALOG_STYLE_TABLIST_HEADERS, str_format("Cellulare - %d", playerNumber), str, "Invia", "Chiudi");
        }
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM phonebook WHERE number_owner = '%d'", playerNumber);
    return 1;
}

stock Character_SendSMS(playerid, slot, number, const msg[])
{

    if(strlen(msg) <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi inviare un messaggio vuoto.");
    
    new senderNumber = Character_GetSlotExtra(playerid, slot, 0);
    
       
    mysql_tquery_f(gMySQL, "INSERT INTO `phone_sms` (number_owner, sender_number, addressee_number, message, created_at, state) VALUES('%d', '%d', '%d', '%e', '%d', '%d')", number, senderNumber, number, msg, gettime(), SMS_STATE_NOT_READ);
    mysql_tquery_f(gMySQL, "INSERT INTO `phone_sms` (number_owner, sender_number, addressee_number, message, created_at, state) VALUES('%d', '%d', '%d', '%e', '%d', '%d')", senderNumber, senderNumber, number, msg, gettime(), SMS_STATE_READ);

    SendFormattedMessage(playerid, COLOR_GREEN, "SMS inviato al numero %d. Testo: %s", number, msg);

    new receiver = Character_FindByPhoneNumber(number);

    if(Character_IsLogged(receiver))
    {
        inline OnLoadPhoneBook()
        {
            if(cache_num_rows() > 0)
            {
                new name[32];
                cache_get_value_name(0, "name", name);
                SendFormattedMessage(receiver, COLOR_GREEN, "SMS ricevuto da %s. Usa il tuo cellulare per leggere il messaggio.", name);
            }
            else
            {
                SendFormattedMessage(receiver, COLOR_GREEN, "SMS ricevuto da %d. Usa il tuo cellulare per leggere il messaggio.", senderNumber);
            }
        }
        MySQL_TQueryInline(gMySQL, using inline OnLoadPhoneBook, "SELECT name FROM phonebook WHERE number_owner = '%d' AND number = '%d'", senderNumber, number);
    }
    Character_Me(playerid, "prende il cellulare ed invia un messaggio.");
    return 1;
}

stock Server_GeneratePhoneNumber()
{
    new number = 30000000 + random(9999999);
    return number;
}

stock Character_IsInCall(playerid)
{
    return CallWith[playerid] != INVALID_PLAYER_ID || CallNumber[playerid] != 0;
}

stock Character_GetCallPlayer(playerid)
{
    return CallWith[playerid];
}

stock Character_FindByPhoneNumber(number)
{
    new p = INVALID_PLAYER_ID;
    foreach(new i : Player)
    {
        if(!Character_IsLogged(i) || !Inventory_HasPhoneByNumber(Character_GetInventory(i), number))
            continue;
        p = i;
        break;        
    }
    return p;
}

stock bool:Character_HasPhoneInSlot(playerid, slotid)
{
    return Character_IsValidSlot(playerid, slotid) && Character_GetSlotItem(playerid, slotid) == gItem_Phone;
}

stock bool:Inventory_HasPhoneByNumber(Inventory:inventory, number)
{
    new bool:success = false;
    for(new i = 0, j = Inventory_GetSpace(inventory); i < j; ++i)
    {
        if(Inventory_GetSlotItem(inventory, i) != gItem_Phone)
            continue;
        if(Inventory_GetSlotExtra(inventory, i, 0) == number)
        {
            success = true;
            break;
        }
    }
    return success;
}

stock Character_ShowPhone(playerid, slotid)
{
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return 0;
    SelectedPhone{playerid} = slotid;

    new number = Character_GetSlotExtra(playerid, slotid, 0);
    Dialog_Show_s(playerid, Dialog_UseCellPhone, DIALOG_STYLE_LIST, str_format("Cellulare - %d", number), @("Info\nChiama\nSMS\nMessaggi non letti\nMessaggi in entrata\nMessaggi in uscita\nRubrica\nAggiungi numero in rubrica"), "Seleziona", "Chiudi");
    return 1;
}

Dialog:Dialog_UseCellPhone(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    
    new slotid = SelectedPhone{playerid};

    if(!Character_HasPhoneInSlot(playerid, slotid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un cellulare in questo slot.");
    
    if(listitem == 0)
    {
        new val[16];
        valstr(val, SelectedPhone{playerid});
        pc_cmd_cellinfo(playerid, val);
    }
    else if(listitem == 1)
    {
        new number = Character_GetSlotExtra(playerid, slotid, 0);
        Dialog_Show_s(playerid, Dialog_PhoneCall, DIALOG_STYLE_INPUT, str_format("Cellulare - %d", number), @("Inserisci il numero di telefono che vuoi chiamare."), "Chiama", "Chiudi");
    }
    else if(listitem == 2)
    {
        new number = Character_GetSlotExtra(playerid, slotid, 0);
        Dialog_Show_s(playerid, Dialog_PhoneSMS, DIALOG_STYLE_INPUT, str_format("Cellulare - %d", number), @("Inserisci il numero di telefono seguito dal messaggio che vuoi inviare.\n == Esempio ==\n 444123456 Ciao sono io!\n\n"), "Invia", "Chiudi");
    }
    else if(listitem == 3)
    {
        return Character_ShowUnReadPhoneMess(playerid, slotid);
    }
    else if(listitem == 4)
    {
        return Character_ShowInBoundPhoneMess(playerid, slotid);
    }
    else if(listitem == 5)
    {
        return Character_ShowOutBoundPhoneMess(playerid, slotid);
    }
    else if(listitem == 6)
    {
        return Character_ShowPhoneBook(playerid, slotid);
    }
    else if(listitem == 7)
    {
        return Dialog_Show(playerid, Dialog_PhoneBookAddNumber, DIALOG_STYLE_INPUT, "Aggiungi numero in rubrica", "Inserisci il numero seguito dal nome che vuoi aggiungere in rubrica.\n== Esempio ==\n44411234 Mario Rossi", "Aggiungi", "Indietro");
    }
    return 1;
}

Dialog:Dialog_PhoneBookAddNumber(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Character_ShowPhone(playerid, SelectedPhone{playerid});
    
    if(!Character_HasPhoneInSlot(playerid, SelectedPhone{playerid}))
        return 0;

    new number, name[32];

    if(sscanf(inputtext, "ds[32]", number, name))
        return Dialog_Show(playerid, Dialog_PhoneBookAddNumber, DIALOG_STYLE_INPUT, "Aggiungi numero in rubrica", "{FF0000}Il formato inserito non è giusto o il nome è troppo lungo.{FFFFFF}\nInserisci il numero seguito dal nome che vuoi aggiungere in rubrica.\n== Esempio ==\n44411234 Mario Rossi", "Aggiungi", "Indietro");
    
    Character_AddToPhoneBook(playerid, SelectedPhone{playerid}, number, name);
    return 1;
}


Dialog:Dialog_PhoneCall(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    new number = strval(inputtext);
    Character_StartCall(playerid, SelectedPhone{playerid}, number);
    return 1;
}

Dialog:Dialog_PhoneSMS(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;

    if(!Character_HasPhoneInSlot(playerid, SelectedPhone{playerid}))
        return 0;
    
    new playerNumber = Character_GetSlotExtra(playerid, SelectedPhone{playerid}, 0), number, msg[128];
    if(sscanf(inputtext, "ds[128]", number, msg))
        return Dialog_Show_s(playerid, Dialog_PhoneSMS, DIALOG_STYLE_INPUT, str_format("Cellulare - %d", playerNumber), @("{FF0000}Il formato inserito non è corretto.{FFFFFF}\nInserisci il numero di telefono seguito dal messaggio che vuoi inviare.\n == Esempio ==\n 444123456 Ciao sono io!\n\n"), "Invia", "Chiudi");
    Character_SendSMS(playerid, SelectedPhone{playerid}, number, msg);
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

        new String:info;

        if(senderNumber == playerNumber)
        {
            info += str_format("Destinatario: %d\nInviato il: %d/%d/%d alle %d/%d.\n\n", addresseeNumber, day, month, year, hour, minute);
        }
        else
        {
            info += str_format("Mittente: %d\nRicevuto il: %d/%d/%d alle %d/%d.\n\n", senderNumber, day, month, year, hour, minute);
        }
        
        info += str_format("========= CONTENUTO ==========\n\n%s", message);

        mysql_tquery_f(gMySQL, "UPDATE `phone_sms` SET state = '%d' WHERE id = '%d'", SMS_STATE_READ, messageid);
        
        SelectedMessage[playerid] = messageid;

        Dialog_Show_s(playerid, Dialog_PhoneMessageOption, DIALOG_STYLE_MSGBOX, @("SMS"), info, "Opzioni", "Indietro");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `phone_sms` WHERE id = '%d'", messageid);
    return 1;
}

Dialog:Dialog_PhoneMessageOption(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
        
    if(!Character_HasPhoneInSlot(playerid, SelectedPhone{playerid}))
        return 0;
    
    Dialog_Show(playerid, Dialog_SelectMessageOption, DIALOG_STYLE_LIST, "Opzioni Messaggio", "Cancella", "Seleziona", "Indietro");
    return 1;
}

Dialog:Dialog_SelectMessageOption(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    
    if(!Character_HasPhoneInSlot(playerid, SelectedPhone{playerid}))
        return 0;

    new selectedMessage = SelectedMessage[playerid];
    SendClientMessage(playerid, COLOR_GREEN, "Hai cancellato il messaggio con successo.");

    mysql_tquery_f(gMySQL, "DELETE FROM `phone_sms` WHERE id = '%d'", selectedMessage);
    return 1;
}

Dialog:Dialog_ShowPhoneMessage(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    
    if(!Character_HasPhoneInSlot(playerid, SelectedPhone{playerid}))
        return 0;

    new id = list_get(MessagesList[playerid], listitem);
    
    Character_ReadMessage(playerid, SelectedPhone{playerid}, id);
    return 1;
}

stock Character_ShowUnReadPhoneMess(playerid, slotid)
{
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return 0;
    
    inline OnLoad()
    {
        new rows = cache_num_rows();

        if(rows <= 0)
            return SendClientMessage(playerid, COLOR_ERROR, "Non hai messaggi non letti in questo cellulare.");

        if(list_valid(MessagesList[playerid]))
            list_clear(MessagesList[playerid]);
        else
            MessagesList[playerid] = list_new();
        
        new String:str = @("Numero/Nome\tInviato il\n");
        new id, addressee, time, year, month, day, hour, minute, second;
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", id);
            cache_get_value_name_int(i, "addressee_number", addressee);
            cache_get_value_name_int(i, "created_at", time);

            TimestampToDate(time, year, month, day, hour, minute, second, 1);

            str += str_format("%d\t%d/%d/%d - %d:%d\n", addressee, day, month, year, hour, minute);

            list_add(MessagesList[playerid], id);
        }

        Dialog_Show_s(playerid, Dialog_ShowPhoneMessage, DIALOG_STYLE_TABLIST_HEADERS, @("Messaggi non letti"), str, "Visualizza", "Indietro");
    }
    new playerNumber = Character_GetSlotExtra(playerid, slotid, 0);
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `phone_sms` WHERE number_owner = '%d' AND sender_number != '%d' AND state = '%d'", playerNumber, playerNumber, SMS_STATE_NOT_READ);
    return 1;
}

stock Character_ShowInBoundPhoneMess(playerid, slotid)
{
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return 0;
    
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
        new id, sender, time, year, month, day, hour, minute, second;
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", id);
            cache_get_value_name_int(i, "sender_number", sender);
            cache_get_value_name_int(i, "created_at", time);

            TimestampToDate(time, year, month, day, hour, minute, second, 1);

            str += str_format("%d\t%d/%d/%d - %d:%d\n", sender, day, month, year, hour, minute);

            list_add(MessagesList[playerid], id);
        }

        Dialog_Show_s(playerid, Dialog_ShowPhoneMessage, DIALOG_STYLE_TABLIST_HEADERS, @("Messaggi in entrata"), str, "Visualizza", "Indietro");
    }
    new playerNumber = Character_GetSlotExtra(playerid, slotid, 0);
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `phone_sms` WHERE number_owner = '%d' AND sender_number != '%d' AND state = '%d'", playerNumber, playerNumber, SMS_STATE_READ);
    return 1;
}

stock Character_ShowOutBoundPhoneMess(playerid, slotid)
{
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return 0;
    
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
        new id, addressee, time, year, month, day, hour, minute, second;
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", id);
            cache_get_value_name_int(i, "addressee_number", addressee);
            cache_get_value_name_int(i, "created_at", time);

            TimestampToDate(time, year, month, day, hour, minute, second, 1);

            str += str_format("%d\t%d/%d/%d - %d:%d\n", addressee, day, month, year, hour, minute);

            list_add(MessagesList[playerid], id);
        }

        Dialog_Show_s(playerid, Dialog_ShowPhoneMessage, DIALOG_STYLE_TABLIST_HEADERS, @("Messaggi in uscita"), str, "Visualizza", "Indietro");
    }
    new playerNumber = Character_GetSlotExtra(playerid, slotid, 0);
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `phone_sms` WHERE number_owner = '%d' AND sender_number = '%d'", playerNumber, playerNumber);
    return 1;
}

stock Character_AddToPhoneBook(playerid, slotid, number, const name[])
{
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return 0;
    
    new playerNumber = Character_GetSlotExtra(playerid, slotid, 0);
    new query[128];
    mysql_format(gMySQL, query, sizeof(query), "SELECT * FROM `phonebook` WHERE number_owner = '%d'", playerNumber);
    mysql_tquery(gMySQL, query, #OnAddPhoneBookContact, "ddds", playerid, playerNumber, number, name);
    return 1;
}

forward OnAddPhoneBookContact(playerid, playerNumber, number, const name[]);
public OnAddPhoneBookContact(playerid, playerNumber, number, const name[])
{
    if(cache_num_rows() >= 50)
        return SendClientMessage(playerid, COLOR_ERROR, "Hai raggiunto il limite di contatti (50).");
    
    if(strlen(name) > 30)
        return SendClientMessage(playerid, COLOR_ERROR, "Hai superato il numero massimo di caratteri per il contatto.");

    mysql_tquery_f(gMySQL, "INSERT INTO `phonebook` (number_owner, name, number, created_at) VALUES('%d', '%e', '%d', '%d')", playerNumber, name, number, gettime());
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai aggiunto il numero %d alla rubrica con il nome %s.", number, name);
    return 1;
}

stock Character_GetEmergencyStatus(playerid)
{
    return PlayerEmergencyStatus{playerid};
}