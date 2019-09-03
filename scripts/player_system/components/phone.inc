#include <inventory_system\core>
#include <YSI_Coding\y_hooks>

static 
    CallWith[MAX_PLAYERS] = {INVALID_PLAYER_ID, ...},
    CallNumber[MAX_PLAYERS] = {0, ...},
    IncomingCall[MAX_PLAYERS] = {INVALID_PLAYER_ID, ...},
    SelectedPhone[MAX_PLAYERS char] = {-1, ...}
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
						CONSTRAINT `phone_characters` FOREIGN KEY (`character_id`) REFERENCES `characters` (`id`) ON DELETE CASCADE \
						) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=latin1");
    
    mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `phonebook` ( \
						`number_owner` int(11) NOT NULL, \
						`name` VARCHAR(32) NOT NULL, \
                        `number` int(11) NOT NULL, \
                        `created_at` int(11) NOT NULL, \
						PRIMARY KEY (`number_owner`), \
						CONSTRAINT `phonebook` FOREIGN KEY (`number_owner`) REFERENCES `phone_data` (`number`) ON DELETE CASCADE \
						) ENGINE=InnoDB DEFAULT CHARSET=latin1");
    
    mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `phone_sms` ( \
						`id` int(11) NOT NULL AUTO_INCREMENT, \
                        `number_owner` int(11) NOT NULL, \
                        `sender_number` int(11) NOT NULL, \
						`addressee_number` INT(11) NOT NULL, \
                        `message` VARCHAR(255) NOT NULL, \
                        `created_at` int(11) NOT NULL, \
						PRIMARY KEY (`id`), \
						CONSTRAINT `phone_sms` FOREIGN KEY (`number_owner`) REFERENCES `phone_data` (`number`) ON DELETE CASCADE \
						) ENGINE=InnoDB DEFAULT CHARSET=latin1");
    return 1;
}

hook OnPlayerClearData(playerid)
{
    if(Character_IsInCall(playerid))
    {
        Character_StopCall(playerid);
        /*
        new other = Character_GetCallPlayer(playerid);
        if(Character_IsLogged(other))
        {
            SendClientMessage(playerid, COLOR_ORANGE, "Il giocatore con cui eri al telefono si è disconnesso.");
            CallWith[other] = INVALID_PLAYER_ID;
        }
        */
    }
    CallWith[playerid] = INVALID_PLAYER_ID;
    CallNumber[playerid] = 0;
    IncomingCall[playerid] = INVALID_PLAYER_ID;
    SelectedPhone{playerid} = -1;
    return 1;
}

hook OnCharacterFirstSpawn(playerid)
{
    new number = Server_GeneratePhoneNumber();
    Character_GiveItem(playerid, gItem_Phone, 1, number);

    mysql_tquery_f(gMySQL, "INSERT INTO `phone_data` (character_id, created_at, number) VALUES('%d', '%d', '%d')", Character_GetID(playerid), gettime(), number);

    SendClientMessage(playerid, COLOR_GREEN, "Ti è stato dato un cellulare. Usa /cellcmds per i comandi o (/inv)entario per gestirlo.");
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
    SelectedPhone{playerid} = slotid;
    new number = Character_GetSlotExtra(playerid, slotid, 0);
    Dialog_Show_s(playerid, Dialog_UseCellPhone, DIALOG_STYLE_LIST, str_format("Cellulare - %d", number), @("Info\nChiama\nSMS\nRubrica"), "Seleziona", "Chiudi");
    return 1;
}

Dialog:Dialog_UseCellPhone(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    
    new slotid = SelectedPhone{playerid};

    if(Character_GetSlotItem(playerid, slotid) != gItem_Phone)
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
        Dialog_Show_s(playerid, Dialog_PhoneSMS, DIALOG_STYLE_INPUT, str_format("Cellulare - %d", number), @("Inserisci il numero di telefono seguito dal messaggio che vuoi inviare.\n == Esempio ==\n- 444123456 Ciao sono io!\n\n"), "Invia", "Chiudi");
    }
    else if(listitem == 3)
    {
        Character_ShowPhoneBook(playerid, slotid);
    }
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
    if(sscanf(params, "ds[128]", number, msg))
        return 1;
    new number = strval(inputtext);
    Character_SendSMS(playerid, SelectedPhone{playerid}, number, const msg[]);
    return 1;
}

flags:cellcmds(CMD_USER);
CMD:cellcmds(playerid, params[])
{
    SendClientMessage(playerid, -1, "[TELEFONE]: /chiama - (/ri)spondi - (/ch)iudi - /sms - /cellinfocellinfo");    
    return 1;
}

flags:cellinfo(CMD_USER);
CMD:cellinfo(playerid, params[])
{
    new slotid;
    if(sscanf(params, "d", slotid))
        return SendClientMessage(playerid, COLOR_ERROR, "/cellinfo <slotid>");
    
    if(!Character_IsValidSlot(playerid, slotid) || Character_GetSlotItem(playerid, slotid) != gItem_Phone)
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
    
    if(!Character_IsValidSlot(playerid, slot) || Character_GetSlotItem(playerid, slot) != gItem_Phone)
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

static Character_StartCall(playerid, slotid, number)
{
    if(Character_GetSlotItem(playerid, slotid) != gItem_Phone)
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un telefono in questo slot."), 0;
    
    if(number <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Il numero inserito non esiste o non è al momento raggiungibile. Riprova più tardi."), 0;

    new isValid = INVALID_PLAYER_ID;
    foreach(new p : Player)
    {
        for(new i = 0, j = Character_GetInventorySize(p); i < j; ++i)
        {
            if(Character_GetSlotExtra(p, i, 0) == number)
            {
                isValid = p;
                break;
            }
        }
        if(isValid != INVALID_PLAYER_ID)
            break;
    }

    if(isValid == INVALID_PLAYER_ID)
        return SendClientMessage(playerid, COLOR_ERROR, "Il numero inserito non esiste o non è al momento raggiungibile. Riprova più tardi."), 0;
    
    if(Character_GetCallPlayer(isValid) != INVALID_PLAYER_ID)
        return SendClientMessage(playerid, COLOR_ERROR, "Il numero chiamato è al momento occupato."), 0;
    
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
    
    CallWith[playerid] = isValid;
    CallNumber[playerid] = number;

    IncomingCall[isValid] = playerid;

    Character_AMe(playerid, "prende il cellulare e digita un numero.");
    Character_Do(isValid, "Il cellulare comincia a squillare...");

    SetPlayerSpecialAction(playerid,SPECIAL_ACTION_USECELLPHONE);
	SetPlayerAttachedObject(playerid, 6, 330, 6, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 1.000000, 1.000000);
    return 1;
}

stock Character_StopCall(playerid)
{
    if(CallWith[playerid] == INVALID_PLAYER_ID && IncomingCall[playerid] == INVALID_PLAYER_ID)
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
        }
    }

    CallWith[playerid] = IncomingCall[playerid] = CallNumber[playerid] = INVALID_PLAYER_ID;

    RemovePlayerAttachedObject(playerid, 6);
    SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
    
    SendClientMessage(playerid, COLOR_GREEN, "Hai chiuso la chiamata.");
    return 1;
}

stock Character_ShowPhoneBook(playerid, slotid)
{
    if(Character_GetSlotItem(playerid, slotid) != gItem_Phone)
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
            
            Dialog_Show_s(playerid, Dialog_PhoneBook, DIALOG_STYLE_LIST, str_format("Cellulare - %d", playerNumber), str, "Invia", "Chiudi");
        }
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM phonebook WHERE number_owner = '%d'", playerNumber);

}

stock Character_SendSMS(playerid, slot, number, const msg[])
{
    if(strlen(msg) <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi inviare un messaggio vuoto.");
    
    return 1;
}

stock Server_GeneratePhoneNumber()
{
    new number = 30000000 + random(9999999);
    return number;
}

stock Character_IsInCall(playerid)
{
    return CallWith[playerid] != INVALID_PLAYER_ID;
}

stock Character_GetCallPlayer(playerid)
{
    return CallWith[playerid];
}