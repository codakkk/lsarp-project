#include <inventory_system\core>
#include <YSI_Coding\y_hooks>

static 
    CallWith[MAX_PLAYERS] = {INVALID_PLAYER_ID, ...},
    CallNumber[MAX_PLAYERS] = {0, ...},
    IncomingCall[MAX_PLAYERS] = {INVALID_PLAYER_ID, ...}
;

hook OnGameModeInit()
{
    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS `phone_data` ( \
						`id` int(11) NOT NULL AUTO_INCREMENT, \
						`character_id` int(11) NOT NULL DEFAULT '0', \
                        `created_at` int(11) NOT NULL DEFAULT '0', \
                        `number` int(11) NOT NULL DEFAULT '0', \
                        `building` int(11) NOT NULL DEFAULT '0', \
						PRIMARY KEY (`id`), \
						CONSTRAINT `phone_characters` FOREIGN KEY (`character_id`) REFERENCES `characters` (`id`) ON DELETE CASCADE \
						) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=latin1", false);
    
    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS `phonebook` ( \
						`number_owner` int(11) NOT NULL, \
						`name` VARCHAR(32) NOT NULL, \
                        `number` int(11) NOT NULL, \
						PRIMARY KEY (`number_owner`), \
						CONSTRAINT `phonebook` FOREIGN KEY (`number_owner`) REFERENCES `phone_data` (`number`) ON DELETE CASCADE \
						) ENGINE=InnoDB DEFAULT CHARSET=latin1", false);
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
            return SendClientMessage(playerid, COLOR_ERROR, "Il cellulare non possiede un numero.");

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

static Character_StartCall(playerid, slotid, number)
{
    if(Character_GetSlotItem(playerid, slotid) != gItem_Phone)
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un telefono in questo slot.");
    
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
        return SendClientMessage(playerid, COLOR_ERROR, "Il numero inserito non esiste o non è al momento raggiungibile. Riprova più tardi.");
    
    if(Character_GetCallPlayer(isValid) != INVALID_PLAYER_ID)
        return SendClientMessage(playerid, COLOR_ERROR, "Il numero chiamato è al momento occupato.");
    
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