
// ADD POSSIBILITY TO INTERACT WITH PHONEBOOK USERS 

#include <inventory_system\core>
#include <YSI_Coding\y_hooks>

#define POLICE_PHONE_NUMBER         (911)

static enum 
{
    STATE_NOT_READ = 0,
    STATE_READ
}

static
    CallWith[MAX_PLAYERS] = {INVALID_PLAYER_ID, ...},
    CallingNumber[MAX_PLAYERS] = {0, ...},
    UsingNumber[MAX_PLAYERS] = {0, ...},
    IncomingCall[MAX_PLAYERS] = {INVALID_PLAYER_ID, ...},
    SelectedPhone[MAX_PLAYERS char] = {-1, ...},
    SelectedPhoneNumber[MAX_PLAYERS] = {0, ...},
    SelectedContactNumber[MAX_PLAYERS] = {0, ...},
    CallFromNumber[MAX_PLAYERS] = {0, ...},
    CallToNumber[MAX_PLAYERS] = {0, ...}
;

static PlayerEmergencyStatus[MAX_PLAYERS char] = {0, ...};

static 
    SelectedMessage[MAX_PLAYERS],
    List:MessagesList[MAX_PLAYERS]
;

static enum _:E_CONTACT
{
    CONTACT_ID,
    CONTACT_NAME[32]
};
static Map:PhoneBooks; // <phone number, Map:Contact>


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

    mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `phone_call_registry` ( \
						`id` int(11) NOT NULL AUTO_INCREMENT, \
                        `number_owner` int(11) NOT NULL, \
                        `call_to_number` int(11) NOT NULL, \
                        `call_by_number` int(11) NOT NULL, \
                        `call_start` int(11) NOT NULL DEFAULT '0', \
                        `call_end` int(11) NOT NULL DEFAULT '0', \
                        `state` int(2) NOT NULL, \
						PRIMARY KEY (`id`) \
						) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=latin1");
    PhoneBooks = map_new();
    return 1;
}

hook OnPlayerClearData(playerid)
{
    if(Character_IsInCall(playerid))
    {
        Character_StopCall(playerid);
    }
    CallWith[playerid] = INVALID_PLAYER_ID;
    CallingNumber[playerid] = 0;
    UsingNumber[playerid] = 0;
    IncomingCall[playerid] = INVALID_PLAYER_ID;
    SelectedPhone{playerid} = -1;
    SelectedPhoneNumber[playerid] = 0;
    SelectedContactNumber[playerid] = 0;
    PlayerEmergencyStatus{playerid} = 0;
    SelectedMessage[playerid] = 0;
    if(list_valid(MessagesList[playerid]))
        list_delete(MessagesList[playerid]);
    
    for(new i = 0, j = Character_GetInventorySize(playerid); i < j; ++i)
    {
        if(Character_GetSlotItem(playerid, i) != gItem_Phone)
            continue;
        new number = Character_GetSlotExtra(playerid, i, 0);
        UnloadPhoneBook(number);
    }
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
        LoadPhoneBook(number);
        inline OnLoadSMS()
        {
            if(cache_num_rows() > 0)
                SendFormattedMessage(playerid, COLOR_GREEN, "[CELLULARE %d] Hai %d messaggi non letti.", number, cache_num_rows());
        }
        MySQL_TQueryInline(gMySQL, using inline OnLoadSMS, "SELECT * FROM `phone_sms` WHERE number_owner = '%d' AND state = '%d'", number, STATE_NOT_READ);
        
        inline OnLoadCalls()
        {
            if(cache_num_rows() > 0)
                SendFormattedMessage(playerid, COLOR_GREEN, "[CELLULARE %d] Hai %d chiamate perse.", number, cache_num_rows());
        }
        MySQL_TQueryInline(gMySQL, using inline OnLoadCalls, "SELECT * FROM `phone_call_registry` WHERE number_owner = '%d' AND state = '%d'", number, STATE_NOT_READ);
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
                Faction_SendMessage(factionid, "Chiamata da: %s - Numero: %d - Zona: %s", Character_GetRolePlayName(playerid), CallingNumber[playerid], zone);
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
    printf("Number: %d", number);
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

    new slot, norn[9];
    if(sscanf(params, "ds[9]", slot, norn))
        return SendClientMessage(playerid, COLOR_ERROR, "/chiama <slot> <numero/nome rubrica>");

    Character_StartCall(playerid, slot, norn);
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
    new requestedNumber = CallingNumber[incomingFrom];

    if(requestedNumber != Character_GetSlotExtra(playerid, slot))
        return SendClientMessage(playerid, COLOR_ERROR, "Non stai ricevendo chiamate se questo numero.");

    SendFormattedMessage(incomingFrom, COLOR_GREEN, "%s ha risposto alla chiamata.", Character_GetRolePlayName(playerid));
    SendClientMessage(playerid, COLOR_GREEN, "Hai risposto alla chiamata.");

    CallingNumber[incomingFrom] = 0;
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
    new slot, number[9], message[128];
    if(sscanf(params, "ds[9]s[128]", slot, number, message))
        return SendClientMessage(playerid, COLOR_ERROR, "/sms <slot> <numero/nome rubrica> <messaggio>");
    
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
    LoadPhoneBook(number);
    return result;
}

stock Character_GiveAnonymousePhone(playerid)
{
    new number = Server_GeneratePhoneNumber(),
        result = Character_GiveItem(playerid, gItem_Phone, 1, number);
    mysql_tquery_f(gMySQL, "INSERT INTO `phone_data` (created_at, number) VALUES('%d', '%d')", gettime(), number);
    LoadPhoneBook(number);
    return result;
}

static Character_StartCall(playerid, slotid, textNumber[])
{
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un telefono in questo slot."), 0;
    
    new playerNumber = Character_GetSlotExtra(playerid, slotid, 0);
    new number = ParsePhoneNumber(playerNumber, textNumber);

    if(number <= 0 || number > 39999999)
        return SendClientMessage(playerid, COLOR_ERROR, "Il numero inserito non esiste o non è al momento raggiungibile. Riprova più tardi."), 0;

    UsingNumber[playerid] = playerNumber;
    CallingNumber[playerid] = number;

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
    {
        inline OnSearch()
        {
            if(cache_num_rows() > 0)
            {
                SendClientMessage(playerid, COLOR_ERROR, "[Operatore] Il numero inserito non è al momento raggiungibile. Riprova più tardi.");
                AddCallInPhoneRegistry(number, playerNumber, STATE_NOT_READ, 0);
            }
            else
            {
                SendClientMessage(playerid, COLOR_ERROR, "[Operatore] Il numero inserito non esiste.");
                AddCallInPhoneRegistry(0, playerNumber, 0, 0);
            }
        }
        MySQL_TQueryInline(gMySQL, using inline OnSearch, "SELECT * FROM `phone_data` WHERE number = '%d'", number);
        return 0;
    }
    
    if(Character_GetCallPlayer(isValid) != INVALID_PLAYER_ID)
        return SendClientMessage(playerid, COLOR_ERROR, "Il numero chiamato è al momento occupato."), 0;
    
    CallWith[playerid] = isValid;
    
    CallFromNumber[playerid] = CallFromNumber[isValid] = playerNumber;
    CallToNumber[playerid] = CallToNumber[isValid] = number;

    IncomingCall[isValid] = playerid;
    UsingNumber[isValid] = number;

    Character_Do(isValid, "Il cellulare comincia a squillare...");
    
    inline OnLoadSenderNumber()
    {
        if(cache_num_rows() > 0)
        {
            new name[32];
            cache_get_value_name(0, "name", name);
            SendFormattedMessage(isValid, COLOR_GREEN, "Stai ricevendo una chiamata da %s dal numero %d.", name, playerNumber);
        }
        else
        {
            SendFormattedMessage(isValid, COLOR_GREEN, "Stai ricevendo una chiamata dal numero %d.", playerNumber);
        }
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoadSenderNumber, "SELECT name FROM `phonebook` WHERE number_owner = '%d' AND number = '%d'", number, playerNumber);

    inline OnLoadReceiverNumber()
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
    MySQL_TQueryInline(gMySQL, using inline OnLoadReceiverNumber, "SELECT name FROM phonebook WHERE number_owner = '%d' AND number = '%d'", playerNumber, number);
    return 1;
}

stock Character_StopCall(playerid)
{
    if(!Character_IsInCall(playerid))
        return 1;
    printf("To: %d - From: %d", CallToNumber[playerid], CallFromNumber[playerid]);
    AddCallInPhoneRegistry(CallToNumber[playerid], CallFromNumber[playerid], CallFromNumber[playerid] == UsingNumber[playerid] ? STATE_NOT_READ : STATE_READ, gettime());
    
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
            CallingNumber[other] = 0;
            CallFromNumber[other] = CallToNumber[other] = 0;
            RemovePlayerAttachedObject(other, 6);
        }
    }


    CallingNumber[playerid] = 0;
    CallWith[playerid] = IncomingCall[playerid] = INVALID_PLAYER_ID;
    
    CallFromNumber[playerid] = CallToNumber[playerid] = 0;

    PlayerEmergencyStatus{playerid} = 0;

    RemovePlayerAttachedObject(playerid, 6);
    SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
    
    SendClientMessage(playerid, COLOR_GREEN, "Hai chiuso la chiamata.");
    return 1;
}

stock Character_SendSMS(playerid, slot, textNumber[], const msg[])
{

    if(strlen(msg) <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi inviare un messaggio vuoto.");
    
    new senderNumber = Character_GetSlotExtra(playerid, slot, 0);
    new number = ParsePhoneNumber(senderNumber, textNumber);
    
    if(number <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Il numero inserito non è valido."), 0;
       
    mysql_tquery_f(gMySQL, "INSERT INTO `phone_sms` (number_owner, sender_number, addressee_number, message, created_at, state) VALUES('%d', '%d', '%d', '%e', '%d', '%d')", number, senderNumber, number, msg, gettime(), STATE_NOT_READ);
    mysql_tquery_f(gMySQL, "INSERT INTO `phone_sms` (number_owner, sender_number, addressee_number, message, created_at, state) VALUES('%d', '%d', '%d', '%e', '%d', '%d')", senderNumber, senderNumber, number, msg, gettime(), STATE_READ);
    new name[32];
    if(GetPhoneBookNumberName(senderNumber, number, name))
        SendFormattedMessage(playerid, COLOR_GREEN, "SMS inviato al contatto %s. Testo: %s", name, msg);
    else
        SendFormattedMessage(playerid, COLOR_GREEN, "SMS inviato al numero %d. Testo: %s", number, msg);

    new receiver = Character_FindByPhoneNumber(number);

    if(Character_IsLogged(receiver))
    {
        if(GetPhoneBookNumberName(number, senderNumber, name))
        {
            SendFormattedMessage(receiver, COLOR_GREEN, "SMS ricevuto da %s. Usa il tuo cellulare (/cell <slot>) per leggere il messaggio.", name);
        }
        else
        {
            SendFormattedMessage(receiver, COLOR_GREEN, "SMS ricevuto da %d. Usa il tuo cellulare per leggere il messaggio.", senderNumber);
        }
    }
    Character_Me(playerid, "prende il cellulare ed invia un messaggio.");
    return 1;
}

// Use 0 to toNumber to only add history data to byNumber
static AddCallInPhoneRegistry(toNumber, byNumber, callState, endCallTime = 0)
{
    if(toNumber < 0 || toNumber > 39999999 || byNumber < 0 || byNumber > 39999999)
        return 0;
    
    if(toNumber > 0)
    {
        mysql_tquery_f(gMySQL, "INSERT INTO `phone_call_registry` (number_owner, call_by_number, call_to_number, call_start, call_end, state) \
        VALUES('%d', '%d', '%d', '%d', '%d', '%d')", 
        toNumber, byNumber, toNumber, gettime(), endCallTime, callState);
    }

    // MITTENTE
    mysql_tquery_f(gMySQL, "INSERT INTO `phone_call_registry` (number_owner, call_by_number, call_to_number, call_start, call_end, state) \
    VALUES('%d', '%d', '%d', '%d', '%d', '%d')", 
    byNumber, byNumber, toNumber, gettime(), endCallTime, STATE_READ);
    return 1;
}

stock Server_GeneratePhoneNumber()
{
    new number = 30000000 + random(9999999);
    new Cache:cache = mysql_query_f(gMySQL, true, "SELECT id WHERE number = '%d'", number);
    if(cache_num_rows() > 0)
        return Server_GeneratePhoneNumber();
    cache_delete(cache);
    return number;
}

stock Character_IsInCall(playerid)
{
    return CallWith[playerid] != INVALID_PLAYER_ID || CallingNumber[playerid] != 0;
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
    SelectedPhoneNumber[playerid] = Character_GetSlotExtra(playerid, slotid, 0);
    Dialog_Show_s(playerid, Dialog_UseCellPhone, DIALOG_STYLE_LIST, str_format("Cellulare - %d", SelectedPhoneNumber[playerid]), @("Info\nChiama\nSMS\nMessaggi non letti\nMessaggi in entrata\nMessaggi in uscita\nRegistro chiamate perse\nRegistro chiamate in entrata\nRegistro chiamate in uscita\nRubrica\nAggiungi numero in rubrica"), "Seleziona", "Chiudi");
    return 1;
}

Dialog:Dialog_UseCellPhone(playerid, response, listitem, inputtext[])
{
    if(!response || SelectedPhoneNumber[playerid] <= 0)
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
        Dialog_Show_s(playerid, Dialog_PhoneCall, DIALOG_STYLE_INPUT, str_format("Cellulare - %d", SelectedPhoneNumber[playerid]), @("Inserisci il numero di telefono che vuoi chiamare."), "Chiama", "Chiudi");
    }
    else if(listitem == 2)
    {
        Dialog_Show_s(playerid, Dialog_PhoneSMS, DIALOG_STYLE_INPUT, str_format("Cellulare - %d", SelectedPhoneNumber[playerid]), @("Inserisci il numero di telefono seguito dal messaggio che vuoi inviare.\n == Esempio ==\n 444123456 Ciao sono io!\n\n"), "Invia", "Chiudi");
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
        return ShowLostCallRegister(playerid, SelectedPhoneNumber[playerid]);
    }
    else if(listitem == 7)
    {
        return ShowInboundCallRegister(playerid, SelectedPhoneNumber[playerid]);
    }
    else if(listitem == 8)
    {
        return ShowOutboundCallRegister(playerid, SelectedPhoneNumber[playerid]);
    }
    else if(listitem == 9)
    {
        return Character_ShowPhoneBook(playerid, slotid);
    }
    else if(listitem == 10)
    {
        return Dialog_Show(playerid, Dialog_PhoneBookAddNumber, DIALOG_STYLE_INPUT, "Aggiungi numero in rubrica", "Inserisci il numero seguito dal nome che vuoi aggiungere in rubrica.\n== Esempio ==\n44411234 Mario Rossi", "Aggiungi", "Indietro");
    }
    return 1;
}

Dialog:Dialog_PhoneCall(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Character_ShowPhone(playerid, SelectedPhone{playerid});
    Character_StartCall(playerid, SelectedPhone{playerid}, inputtext);
    return 1;
}

Dialog:Dialog_PhoneSMS(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Character_ShowPhone(playerid, SelectedPhone{playerid});

    if(!Character_HasPhoneInSlot(playerid, SelectedPhone{playerid}))
        return 0;
    
    new number[9], msg[128];
    if(sscanf(inputtext, "s[9]s[128]", number, msg))
        return Dialog_Show_s(playerid, Dialog_PhoneSMS, DIALOG_STYLE_INPUT, str_format("Cellulare - %d", SelectedPhoneNumber[playerid]), @("{FF0000}Il formato inserito non è corretto.{FFFFFF}\nInserisci il numero di telefono seguito dal messaggio che vuoi inviare.\n == Esempio ==\n 444123456 Ciao sono io!\n\n"), "Invia", "Chiudi");
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

        mysql_tquery_f(gMySQL, "UPDATE `phone_sms` SET state = '%d' WHERE id = '%d'", STATE_READ, messageid);
        
        SelectedMessage[playerid] = messageid;

        Dialog_Show_s(playerid, Dialog_PhoneMessageOption, DIALOG_STYLE_MSGBOX, @("SMS"), info, "Opzioni", "Indietro");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `phone_sms` WHERE id = '%d'", messageid);
    return 1;
}

Dialog:Dialog_PhoneMessageOption(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Character_ShowPhone(playerid, SelectedPhone{playerid});
        
    if(!Character_HasPhoneInSlot(playerid, SelectedPhone{playerid}))
        return 0;
    
    Dialog_Show(playerid, Dialog_SelectMessageOption, DIALOG_STYLE_LIST, "Opzioni Messaggio", "Cancella", "Seleziona", "Indietro");
    return 1;
}

Dialog:Dialog_SelectMessageOption(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Character_ShowPhone(playerid, SelectedPhone{playerid});
    
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
        return Character_ShowPhone(playerid, SelectedPhone{playerid});
    
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
        
        new String:str = @("Numero/Nome\tInviato il\n");
        new id, addressee, time, year, month, day, hour, minute, second,
            name[32];
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", id);
            cache_get_value_name_int(i, "addressee_number", addressee);
            cache_get_value_name_int(i, "created_at", time);

            TimestampToDate(time, year, month, day, hour, minute, second, 1);

            if(GetPhoneBookNumberName(playerNumber, addressee, name))
                str += str_format("%s\t%d/%d/%d - %d:%d\n", name, day, month, year, hour, minute);
            else
                str += str_format("%d\t%d/%d/%d - %d:%d\n", addressee, day, month, year, hour, minute);
            list_add(MessagesList[playerid], id);
        }

        Dialog_Show_s(playerid, Dialog_ShowPhoneMessage, DIALOG_STYLE_TABLIST_HEADERS, @("Messaggi non letti"), str, "Visualizza", "Indietro");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `phone_sms` WHERE number_owner = '%d' AND sender_number != '%d' AND state = '%d'", playerNumber, playerNumber, STATE_NOT_READ);
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
            if(GetPhoneBookNumberName(playerNumber, sender, name))
                str += str_format("%s\t%d/%d/%d - %d:%d\n", name, day, month, year, hour, minute);
            else
                str += str_format("%d\t%d/%d/%d - %d:%d\n", sender, day, month, year, hour, minute);

            list_add(MessagesList[playerid], id);
        }

        Dialog_Show_s(playerid, Dialog_ShowPhoneMessage, DIALOG_STYLE_TABLIST_HEADERS, @("Messaggi in entrata"), str, "Visualizza", "Indietro");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `phone_sms` WHERE number_owner = '%d' AND sender_number != '%d' AND state = '%d'", playerNumber, playerNumber, STATE_READ);
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
        
            if(GetPhoneBookNumberName(playerNumber, addressee, name))
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

static ShowLostCallRegister(playerid, number)
{
    inline OnLoad()
    {
        new rows = cache_num_rows();

        if(rows <= 0)
            return SendClientMessage(playerid, COLOR_ERROR, "Il registro chiamate è vuoto.");
        
        new String:str = @("Utente\tData\n"), name[32], id, owner, callToNumber, callByNumber, callStart, callEnd,
            year, month, day, hour, minute, second;
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", id);
            cache_get_value_name_int(i, "number_owner", owner);
            cache_get_value_name_int(i, "call_to_number", callToNumber);
            cache_get_value_name_int(i, "call_by_number", callByNumber);
            cache_get_value_name_int(i, "call_start", callStart);
            cache_get_value_name_int(i, "call_end", callEnd);
            
            if(GetPhoneBookNumberName(owner, callByNumber, name))
                str += str_format("%s\t", name);
            else
                str += str_format("%d\t", callByNumber);
            
            TimestampToDate(callStart, year, month, day, hour, minute, second, 0);
            str += str_format("%d/%d/%d - %d:%d\n", day, month, year, hour, minute);
        }
        Dialog_Show_s(playerid, DialogNull, DIALOG_STYLE_TABLIST_HEADERS, @("Registro chiamate"), str, "Seleziona", "Chiudi");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `phone_call_registry` WHERE number_owner = '%d' AND state = '%d'", number, STATE_NOT_READ);
    return 1;
}

static ShowInboundCallRegister(playerid, number)
{
    inline OnLoad()
    {
        new rows = cache_num_rows();

        if(rows <= 0)
            return SendClientMessage(playerid, COLOR_ERROR, "Il registro chiamate è vuoto.");
        
        new String:str = @("Utente\tDurata (m:s)\tData\n"), name[32], id, owner, callToNumber, callByNumber, callStart, callEnd,
            year, month, day, hour, minute, second;
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", id);
            cache_get_value_name_int(i, "number_owner", owner);
            cache_get_value_name_int(i, "call_to_number", callToNumber);
            cache_get_value_name_int(i, "call_by_number", callByNumber);
            cache_get_value_name_int(i, "call_start", callStart);
            cache_get_value_name_int(i, "call_end", callEnd);
            
            if(GetPhoneBookNumberName(owner, callByNumber, name))
                str += str_format("%s\t", name);
            else
                str += str_format("%d\t", callToNumber);

            new time = callEnd - callStart;
            TimestampToDate(time, year, month, day, hour, minute, second, 0);
            str += str_format("%d:%d\t", minute, second);

            TimestampToDate(callStart, year, month, day, hour, minute, second, 0);
            str += str_format("%d/%d/%d - %d:%d\n", day, month, year, hour, minute);
        }
        Dialog_Show_s(playerid, DialogNull, DIALOG_STYLE_TABLIST_HEADERS, @("Registro chiamate in entrata"), str, "Seleziona", "Chiudi");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `phone_call_registry` WHERE number_owner = '%d' AND call_by_number != '%d' AND state = '%d'", number, number, STATE_READ);
    return 1;
}

static ShowOutboundCallRegister(playerid, number)
{
    inline OnLoad()
    {
        new rows = cache_num_rows();

        if(rows <= 0)
            return SendClientMessage(playerid, COLOR_ERROR, "Il registro chiamate è vuoto.");
        
        new String:str = @("Utente\tDurata (m:s)\tData\n"), name[32], id, owner, callToNumber, callByNumber, callStart, callEnd,
            year, month, day, hour, minute, second;
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", id);
            cache_get_value_name_int(i, "number_owner", owner);
            cache_get_value_name_int(i, "call_to_number", callToNumber);
            cache_get_value_name_int(i, "call_by_number", callByNumber);
            cache_get_value_name_int(i, "call_start", callStart);
            cache_get_value_name_int(i, "call_end", callEnd);
            
            if(GetPhoneBookNumberName(owner, callToNumber, name))
                str += str_format("%s\t", name);
            else
                str += str_format("%d\t", callToNumber);

            new time = callEnd - callStart;
            TimestampToDate(time, year, month, day, hour, minute, second, 0);
            str += str_format("%d:%d\t", minute, second);

            TimestampToDate(callStart, year, month, day, hour, minute, second, 0);
            str += str_format("%d/%d/%d - %d:%d\n", day, month, year, hour, minute);
        }
        Dialog_Show_s(playerid, DialogNull, DIALOG_STYLE_TABLIST_HEADERS, @("Registro chiamate in uscita"), str, "Seleziona", "Chiudi");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `phone_call_registry` WHERE number_owner = '%d' AND call_by_number = '%d' AND state = '%d'", number, number, STATE_READ);
    return 1;
}

stock Character_ShowLostCallRegister(playerid, number)
{
    inline OnLoad()
    {
        new rows = cache_num_rows();

        if(rows <= 0)
            return SendClientMessage(playerid, COLOR_ERROR, "Il registro chiamate è vuoto.");
        
        new String:str = @("Utente\tDurata\n"), name[32], id, owner, callToNumber, callByNumber, callStart, callEnd,
            year, month, day, hour, minute, second;
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", id);
            cache_get_value_name_int(i, "number_owner", owner);
            cache_get_value_name_int(i, "call_to_number", callToNumber);
            cache_get_value_name_int(i, "call_by_number", callByNumber);
            cache_get_value_name_int(i, "call_start", callStart);
            cache_get_value_name_int(i, "call_end", callEnd);
            
            if(Character_GetNameFromPhoneBook(owner, callToNumber, name))
                str += str_format("%s\t", name);
            else
                str += str_format("%d\t", callToNumber);
            
            if(callEnd == 0)
                str += @("Nessuna Risposta");
            else
            {
                new time = callEnd - callStart;
                TimestampToDate(time, year, month, day, hour, minute, second, 0);
                str += str_format("%d:%d", minute, second);
            }
            str += @("\n");
        }
        Dialog_Show_s(playerid, DialogNull, DIALOG_STYLE_TABLIST_HEADERS, @("Registro chiamate"), str, "Seleziona", "Chiudi");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `phone_call_registry` WHERE number_owner = '%d' AND state = '%d'", number, STATE_NOT_READ);
    return 1;
}

stock Character_GetEmergencyStatus(playerid)
{
    return PlayerEmergencyStatus{playerid};
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

static ShowContactOptions(playerid, playerNumber, contactNumber)
{
    new name[32];
    GetPhoneBookNumberName(playerNumber, contactNumber, name);
    Dialog_Show_s(playerid, Dialog_PhoneBookContact, DIALOG_STYLE_LIST, str_format("%s - %d", name, contactNumber), @("Chiama\nSMS\nModifica Nome\nCancella"), "Seleziona", "Chiudi");
    return 1;
}

Dialog:Dialog_PhoneBook(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Character_ShowPhone(playerid, SelectedPhone{playerid});
    SelectedContactNumber[playerid] = map_key_at(Map:map_get(PhoneBooks, SelectedPhoneNumber[playerid]), listitem);
    return ShowContactOptions(playerid, SelectedPhoneNumber[playerid], SelectedContactNumber[playerid]);
}

Dialog:Dialog_PhoneBookContact(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Character_ShowPhoneBook(playerid, SelectedPhone{playerid});
    if(listitem == 0)
        return Character_StartCall(playerid, SelectedPhone{playerid}, SelectedContactNumber[playerid]);
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
        return ShowContactOptions(playerid, SelectedPhoneNumber[playerid], SelectedContactNumber[playerid]);
    
    new name[32];
    GetPhoneBookNumberName(SelectedPhoneNumber[playerid], SelectedContactNumber[playerid], name);

    SendFormattedMessage(playerid, COLOR_GREEN, "Hai rimosso il contatto (%s) dalla rubrica.", name);
    
    if(DeletePhoneBookContact(SelectedPhoneNumber[playerid], SelectedContactNumber[playerid]))
    {
        return Character_ShowPhoneBook(playerid, SelectedPhone{playerid});
    }
    return 1;
}

Dialog:Dialog_PhoneBookSMS(playerid, response, listitem, inputtext[])
{
    if(!response)
        return ShowContactOptions(playerid, SelectedPhoneNumber[playerid], SelectedContactNumber[playerid]);
    if(strlen(inputtext) <= 0 || strlen(inputtext) >= 128)
        return Dialog_Show_s(playerid, Dialog_PhoneBookSMS, DIALOG_STYLE_INPUT, str_format("Cellulare - %d", SelectedContactNumber[playerid]), @("{FF0000}Il messaggio inserito è troppo corto o troppo lungo.{FFFFFF}\nInserisci il messaggio che vuoi inviare al contatto.\n\n"), "Invia", "Chiudi");
    Character_SendSMS(playerid, SelectedPhone{playerid}, SelectedPhoneNumber[playerid], inputtext);
    ShowContactOptions(playerid, SelectedPhoneNumber[playerid], SelectedContactNumber[playerid]);
    return 1;
}

Dialog:Dialog_PhoneBookEditName(playerid, response, listitem, inputtext[])
{
    if(!response)
        return ShowContactOptions(playerid, SelectedPhoneNumber[playerid], SelectedContactNumber[playerid]);
    if(strlen(inputtext) <= 0 || strlen(inputtext) >= 31)
        return Dialog_Show_s(playerid, Dialog_PhoneBookSMS, DIALOG_STYLE_INPUT, str_format("Cellulare - %d", SelectedContactNumber[playerid]), @("{FF0000}Il nome inserito è troppo corto o troppo lungo.{FFFFFF}\nInserisci il messaggio che vuoi inviare al contatto.\n\n"), "Invia", "Chiudi");
    if(!SetPhoneBookContactName(SelectedPhoneNumber[playerid], SelectedContactNumber[playerid], inputtext))
        return 0;
    ShowContactOptions(playerid, SelectedPhoneNumber[playerid], SelectedContactNumber[playerid]);
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

static GetPhoneBookContactID(ownerNumber, number)
{
    if(!map_has_key(PhoneBooks, ownerNumber))
        return 0;
    new Map:phonebook = Map:map_get(PhoneBooks, ownerNumber);
    if(!map_has_key(phonebook, number))
        return 0;
    return map_get(phonebook, number, CONTACT_ID);
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
    new Map:phonebook = Map:map_get(PhoneBooks, ownerNumber);
    if(!map_has_key(phonebook, number))
        return false;
    new id = map_get(phonebook, number, CONTACT_ID);
    if(id > 0)
        mysql_tquery_f(gMySQL, "DELETE FROM `phonebook` WHERE id = '%d'", id);
    map_remove_deep(phonebook, number);
    return true;
}

static bool:SetPhoneBookContactName(ownerNumber, number, name[])
{
    if(!map_has_key(PhoneBooks, ownerNumber))
        return false;
    new Map:phonebook = Map:map_get(PhoneBooks, ownerNumber);
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

static ParsePhoneNumber(ownerNumber, value[])
{
    if(IsNumeric(value))
        return strval(value);
    new result = 0;
    
    if(map_has_key(PhoneBooks, ownerNumber))
    {
        new Map:phonebook = Map:map_get(PhoneBooks, ownerNumber);
        new name[32];
        for_map(i : phonebook)
        {
            new number = iter_get_key(i);
            if(GetPhoneBookNumberName(ownerNumber, number, name) && !strcmp(name, value, true))
            {
                result = number;
                break;
            }
        } 
    }
    
    return result;
}

static LoadPhoneBook(phoneNumber)
{
    if(map_has_key(PhoneBooks, phoneNumber))
        return 0;
    inline OnLoad()
    {
        new Map:map = map_new();
        map_set_ordered(map, true);
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