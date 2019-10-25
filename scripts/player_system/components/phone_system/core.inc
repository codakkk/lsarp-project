#if defined _included_CELLPHONE_SYSTEM
    #endinput
#endif 

#define _included_CELLPHONE_SYSTEM
// ADD POSSIBILITY TO INTERACT WITH PHONEBOOK USERS 

forward OnCharacterLoadPhone(playerid, number);
forward OnPhoneStopCall(number, Inventory:entityInv, entityId, E_ENTITY_TYPE:entityType, slotid);

#include <inventory_system\core>
#include "components\phonebook"
#include "components\registry"
#include "components\sms"
#include <YSI_Coding\y_hooks>

#define POLICE_PHONE_NUMBER         (911)

// FARE NOTIFICA DEI MESSAGGI NON LETTI QUANDO SI ACCENDE IL CELL

static enum 
{
    STATE_NOT_READ = 0,
    STATE_READ
}

static enum 
{
    PHONE_STATE_ON = 0,
    PHONE_STATE_OFF = 1,
}

static
    bool:UsingPhone[MAX_PLAYERS char],
    UsingPhoneSlotID[MAX_PLAYERS],
    SelectedPhone[MAX_PLAYERS char] = {-1, ...},
    SelectedPhoneNumber[MAX_PLAYERS] = {0, ...}
;


static PlayerEmergencyStatus[MAX_PLAYERS char] = {0, ...};

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
    return 1;
}

hook OnCharacterDisconnected(playerid, reason)
{
    if(Character_IsInCall(playerid))
    {
        Character_StopCall(playerid, UsingPhoneSlotID[playerid]);
    }
    return 1;
}

hook OnCharacterClearData(playerid)
{
    UsingPhone{playerid} = false;
    UsingPhoneSlotID[playerid] = -1;
    SelectedPhone{playerid} = -1;
    SelectedPhoneNumber[playerid] = 0;
    PlayerEmergencyStatus{playerid} = 0;
    return 1;
}

hook OnCharacterFirstSpawn(playerid)
{
    Character_GivePhone(playerid);
    SendClientMessage(playerid, COLOR_GREEN, "Ti è stato dato un cellulare. Usa /cellcmds per i comandi o (/inv)entario per gestirlo.");
    return 1;
}

stock LoadPhone(playerid, number)
{
    CallLocalFunction(#OnCharacterLoadPhone, "dd", playerid, number);
    return 1;
}

hook OnCharacterLateLoad(playerid)
{
    for(new i = 0, j = Character_GetInventorySize(playerid); i < j; ++i)
    {
        if(Character_GetSlotItem(playerid, i) != gItem_Phone)
            continue;
        new number = Character_GetSlotExtra(playerid, i, 0);
        LoadPhone(playerid, number);
    }
    return 1;
}

hook OnCharacterGiveItem(playerid, itemid, amount, extra, Map:tempExtra)
{
    if(itemid != gItem_Phone)
        return 0;
    if(extra > 0)
        LoadPhone(playerid, extra);
    return 1;
}

hook OnCharacterDeath(playerid, killerid, reason)
{
    if(Character_IsInCall(playerid))
        Character_StopCall(playerid, UsingPhoneSlotID[playerid]);
    return 1;
}

hook OnCharacterUseItem(playerid, slotid, itemid)
{
    if(itemid != gItem_Phone)
        return 0;
    Character_ShowPhone(playerid, slotid);
    return 1;
}

hook OnCharacterDropItem(playerid, slotid)
{
    if(UsingPhone{playerid} && UsingPhoneSlotID[playerid] == slotid)
    {
        UsingPhone{playerid} = false;
        UsingPhoneSlotID[playerid] = INVALID_PLAYER_ID;
    }
    return 1;
}

timer RingPhoneTimer[10000](number)
{
    static entityId, E_ENTITY_TYPE:entityType, entityPhoneSlotId;
    if(World_FindPhoneByNumber(number, entityType, entityId, entityPhoneSlotId))
    {
        //new Inventory:entityInv = Inventory_Get(entityType, entityId);
        if(entityType == ENTITY_TYPE_PLAYER)
        {
            Character_Do(entityId, "Il cellulare comincia a squillare...");
        }
        else if(entityType == ENTITY_TYPE_DROP)
        {
            new Float:x, Float:y, Float:z;
            Drop_GetPosition(entityId, x, y, z);
            SendRangedMessage(10.0, x, y, z, Drop_GetVirtualWorld(entityId), Drop_GetInterior(entityId), "* (( Un cellulare suona nei dintorni... ))", COLOR_FADE1, COLOR_FADE2, COLOR_FADE3, COLOR_FADE4, COLOR_FADE5);
        }
    }
}

//ptask OnCheckPlayerPhone[500](playerid) 
hook OnPlayerUpdate(playerid)
{
    if(!Character_IsLogged(playerid))
        return 1;
    if(!UsingPhone{playerid})
    {
        new itemid, number;
        for(new i = 0, j = Character_GetInventorySize(playerid); i < j; ++i)
        {
            itemid = Character_GetSlotItem(playerid, i);
            if(itemid != gItem_Phone)
                continue;
            number = Phone_GetCallingNumber(Character_GetInventory(playerid), i);
            if(number != 0)
            {
                UsingPhone{playerid} = true;
                UsingPhoneSlotID[playerid] = i;
                break;
            }
        }
        return 1;
    }
    // Dunno if it's safe
    if(Character_GetSlotItem(playerid, UsingPhoneSlotID[playerid]) != gItem_Phone)
    {
        UsingPhone{playerid} = false;
        UsingPhoneSlotID[playerid] = -1;
        SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
        RemovePlayerAttachedObject(playerid, 6);
    }
    return 1;   
}

hook OnCharacterTalk(playerid, text[])
{
    if(!Character_IsInCall(playerid))
        return 0;
    

    new number = Phone_GetCallingNumber(Character_GetInventory(playerid), UsingPhoneSlotID[playerid]);
    
    new entityId, E_ENTITY_TYPE:entityType, entityPhoneSlotId;
    
    World_FindPhoneByNumber(number, entityType, entityId, entityPhoneSlotId);
    printf("Slot: %d - Number: %d - Entity Type: %d - Entity id: %d", UsingPhoneSlotID[playerid], number, _:entityType, entityId);

    new Inventory:entityInv = Inventory_Get(entityType, entityId);

    new speakerMode = Phone_GetSpeakerPhone(entityInv, entityPhoneSlotId);

    if(entityType == ENTITY_TYPE_DROP)
    {
        if(speakerMode)
        {
            new Float:x, Float:y, Float:z, String:str;
            Drop_GetPosition(entityId, x, y, z);
            
            str = str_format("[Vivavoce] %s dice: %s", Character_GetRolePlayName(playerid), text);
            
            SendRangedMessageStr(5.0, x, y, z, Drop_GetVirtualWorld(entityId), Drop_GetInterior(entityId), str, -1, -1, -1, -1, -1);
        }
    }
    else if(entityType == ENTITY_TYPE_PLAYER && Character_IsLogged(entityId))
    {
        if(speakerMode)
            ProxDetectorStr(entityId, 10.0, str_format("[Vivavoce] %s dice: %s", Character_GetRolePlayName(playerid), text), COLOR_FADE1, COLOR_FADE2, COLOR_FADE3, COLOR_FADE4, COLOR_FADE5, false, true);
        else
            SendFormattedMessage(entityId, 0xFFFF00FF, "[Telefono] %s dice: %s", Character_GetRolePlayName(playerid), text);
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
        
        new playerNumber = Character_GetUsingPhoneNumber(playerid);

        for_map(i : GetFactionsMap())
        {
            factionid = iter_get_key(i);
            if(Faction_GetType(factionid) == type)
            {
                Faction_SendMessage(factionid, "** HQ: Chiamata al centralino");
                Faction_SendMessage(factionid, "Chiamata da: %s - Numero: %d - Zona: %s", Character_GetRolePlayName(playerid), playerNumber, zone);
                Faction_SendMessage(factionid, "Emergenza: %s", text);
            }
        }

        Character_StopCall(playerid, UsingPhoneSlotID[playerid]);
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
    new slotid;
    if(sscanf(params, "k<slotid>", slotid))
        return SendClientMessage(playerid, COLOR_ERROR, "(/cell)ulare <slotid>");
    
    if(!Character_ShowPhone(playerid, slotid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un cellulare in questo slot.");
    return 1;
}
alias:cellulare("cell");

flags:cellinfo(CMD_USER);
CMD:cellinfo(playerid, params[])
{
    new slotid;
    if(sscanf(params, "k<slotid>", slotid))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /cellinfo <slotid>");
    
    Character_ShowPhoneInfo(playerid, slotid);
    //TimestampToDate(, &year, &month, &day, &hour, &minute, &second, HourGMT, MinuteGMT = 0)
    return 1;
}

flags:chiama(CMD_ALIVE_USER);
CMD:chiama(playerid, params[])
{
    if(UsingPhone{playerid})
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se sei al telefono.");

    if(Character_IsInCall(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se sei già in una chiamata.");

    new slot, norn[9];
    if(sscanf(params, "k<slotid>s[9]", slot, norn))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /chiama <slot> <numero/nome rubrica>");

    Character_StartCall(playerid, slot, norn);
    return 1;
}

flags:rispondi(CMD_ALIVE_USER);
CMD:rispondi(playerid, params[])
{
    if(UsingPhone{playerid})
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se sei al telefono.");
    
    new slot;
    if(sscanf(params, "k<slotid>", slot))
        return SendClientMessage(playerid, COLOR_ERROR, "(/ri)spondi <slot>");
    
    if(!Character_HasPhoneInSlot(playerid, slot))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un cellulare in questo slot.");

    if(!Phone_IsBusy(Character_GetInventory(playerid), slot) || Phone_GetIncomingFromNumber(Character_GetInventory(playerid), slot) == 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non stai ricevendo una chiamata su questo cellulare.");
    
    new Inventory:playerInventory = Character_GetInventory(playerid);

    Phone_SetCallingNumber(playerInventory, slot, Phone_GetIncomingFromNumber(playerInventory, slot));
    Phone_SetRinging(playerInventory, slot, false);
    Phone_SetIncomingFromNumber(playerInventory, slot, 0);

    new entityId, E_ENTITY_TYPE:entityType, entityPhoneSlotId;
    World_FindPhoneByNumber(Phone_GetCallingNumber(playerInventory, slot), entityType, entityId, entityPhoneSlotId);

    if(entityType == ENTITY_TYPE_PLAYER)
    {
        SendFormattedMessage(entityId, COLOR_GREEN, "%s ha risposto alla chiamata.", Character_GetRolePlayName(playerid));
    }
    
    UsingPhone{playerid} = true;
    UsingPhoneSlotID[playerid] = slot;

    SendClientMessage(playerid, COLOR_GREEN, "Hai risposto alla chiamata.");
    SendClientMessage(playerid, 0x8C8C8CFF, "[!] Puoi parlare ora usando la chat box.");
    SetPlayerSpecialAction(playerid, SPECIAL_ACTION_USECELLPHONE);
	SetPlayerAttachedObject(playerid, 6, 330, 6, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 1.000000, 1.000000);

    Character_Me(playerid, "risponde al cellulare.");
    return 1;
}
alias:rispondi("ri");

flags:chiudi(CMD_ALIVE_USER);
CMD:chiudi(playerid, params[])
{
    new slot;
    if(sscanf(params, "k<slotid>", slot))
        return SendClientMessage(playerid, COLOR_ERROR, "(/ch)iudi <slotid>");
    Character_StopCall(playerid, slot);
    return 1;
}
alias:chiudi("ch");

flags:vivavoce(CMD_ALIVE_USER);
CMD:vivavoce(playerid, params[])
{
    new slotid;
    if(sscanf(params, "k<slotid>", slotid))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /vivavoce <slotid>");
    
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un cellulare in questo slot.");
    
    new Inventory:inv = Character_GetInventory(playerid);
    Phone_SetSpeakerPhone(inv, slotid, !Phone_GetSpeakerPhone(inv, slotid));

    if(Phone_GetSpeakerPhone(inv, slotid))
        SendClientMessage(playerid, COLOR_GREEN, "Hai abilitato il vivavoce.");
    else
        SendClientMessage(playerid, COLOR_GREEN, "Hai disabilitato il vivavoce.");
    return 1;
}

stock Character_GivePhone(playerid)
{
    new number = Server_GeneratePhoneNumber(),
        result = Character_GiveItem(playerid, gItem_Phone, 1, number);
    mysql_tquery_f(gMySQL, "INSERT INTO `phone_data` (character_id, created_at, number) VALUES('%d', '%d', '%d')", Character_GetID(playerid), gettime(), number);
    CallLocalFunction(#OnCharacterLoadPhone, "dd", playerid, number);
    return result;
}

stock Character_GiveAnonymousePhone(playerid)
{
    new number = Server_GeneratePhoneNumber(),
        result = Character_GiveItem(playerid, gItem_Phone, 1, number);
    mysql_tquery_f(gMySQL, "INSERT INTO `phone_data` (created_at, number) VALUES('%d', '%d')", gettime(), number);
    CallLocalFunction(#OnCharacterLoadPhone, "dd", playerid, number);
    return result;
}

stock Character_StartCall(playerid, slotid, textNumber[])
{
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un telefono in questo slot."), 0;
    
    if(Phone_IsOff(Character_GetInventory(playerid), slotid))
        return SendClientMessage(playerid, COLOR_ERROR, "Hai il telefono spento.");

    if(UsingPhone{playerid})
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se sei già in una chiamata.");

    new Inventory:playerInventory = Character_GetInventory(playerid);
    new playerNumber = Character_GetSlotExtra(playerid, slotid, 0);
    new number = PhoneBook_ParseValue(playerNumber, textNumber);

    if(number <= 0 || number > 39999999)
        return SendClientMessage(playerid, 0x8C8C8CFF, "[!] Il numero inserito non esiste o non è al momento raggiungibile. Riprova più tardi."), 0;

    if(number == playerNumber)
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi chiamare il tuo stesso numero.");

    UsingPhone{playerid} = true;
    UsingPhoneSlotID[playerid] = slotid;

    Phone_SetCallingNumber(playerInventory, slotid, number);
    Phone_SetBusy(playerInventory, slotid, true);

    new callid = Registry_Create(playerNumber, number);
    Phone_SetCallID(playerInventory, slotid, callid);

    Character_AMe(playerid, "prende il cellulare e digita un numero.");

    SetPlayerSpecialAction(playerid,SPECIAL_ACTION_USECELLPHONE);
	SetPlayerAttachedObject(playerid, 6, 330, 6, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 1.000000, 1.000000);

    CallLocalFunction(#OnCharacterStartCall, "dd", playerid, slotid);

    if(number == POLICE_PHONE_NUMBER)
    {
        SendClientMessage(playerid, COLOR_GREY, "[OPERATORE] Ci dica di cosa ha bisogno (polizia o medici).");
        PlayerEmergencyStatus{playerid} = 1;
        return 1;
    }

    new entityId, E_ENTITY_TYPE:entityType, entityPhoneSlotId;
    
    if(!World_FindPhoneByNumber(number, entityType, entityId, entityPhoneSlotId))
    {
        inline OnSearch()
        {
            if(cache_num_rows() > 0)
            {
                SendClientMessage(playerid, COLOR_ERROR, "[Operatore] Il numero inserito non è al momento raggiungibile. Riprova più tardi.");
                Registry_SetReceiverState(callid, CALL_STATE_NOT_READ);
                Registry_Save(callid);
            }
            else
            {
                SendClientMessage(playerid, COLOR_ERROR, "[Operatore] Il numero inserito non esiste.");
            }
            Character_StopCall(playerid, slotid);
        }
        MySQL_TQueryInline(gMySQL, using inline OnSearch, "SELECT * FROM `phone_data` WHERE number = '%d'", number);
        return 0;
    }

    new Inventory:entityInv = Inventory_Get(entityType, entityId);

    new validSlotId = Inventory_GetPhoneSlotByNumber(entityInv, number);

    if(Inventory_GetSlotTempExtra(entityInv, validSlotId, "phone_state") == PHONE_STATE_OFF)
    {        
        Character_StopCall(playerid, slotid);
        return SendClientMessage(playerid, 0x8C8C8CFF, "[!] Il numero chiamato ha il cellulare spento."), 0;
    }


    if(entityType == ENTITY_TYPE_PLAYER)
    {
        if(Phone_IsBusy(entityInv, validSlotId))
        {
            SendFormattedMessage(entityId, COLOR_GREEN, "[Cellulare %d - Slot %d] Qualcuno ha provato a chiamarti ma eri occupato.", number, validSlotId);
        }
        else
        {
            new name[32];
            if(PhoneBook_GetContactName(number, playerNumber, name))
            {
                SendFormattedMessage(entityId, COLOR_GREEN, "[Cellulare %d - Slot %d] Stai ricevendo una chiamata da %s dal numero %d.", number, validSlotId, name, playerNumber);
            }
            else
            {
                SendFormattedMessage(entityId, COLOR_GREEN, "[Cellulare %d - Slot %d] Stai ricevendo una chiamata dal numero %d.", number, validSlotId, playerNumber);
            }

            if(PhoneBook_GetContactName(playerNumber, number, name))
            {
                SendFormattedMessage(playerid, COLOR_GREEN, "Stai chiamando %s al numero %d.", name, number);
            }
            else
            {
                SendFormattedMessage(playerid, COLOR_GREEN, "Stai chiamando il numero %d.", number);
            }
        }
    }

    if(Phone_IsBusy(entityInv, validSlotId))
    {
        SendClientMessage(playerid, 0x8C8C8CFF, "[!] Il numero inserito è attualmente occupato.");
        Character_StopCall(playerid, slotid);
        return 0;
    }

    Phone_SetBusy(entityInv, validSlotId, true);
    Phone_SetIncomingFromNumber(entityInv, validSlotId, playerNumber);
    Phone_SetRinging(entityInv, validSlotId, true);
    return 1;
}

stock Character_StopCall(playerid, slotid)
{
    new Inventory:playerInventory = Character_GetInventory(playerid);
    if(!Phone_IsBusy(playerInventory, slotid))
        return 1;

    new 
        playerNumber = Inventory_GetSlotExtra(playerInventory, slotid, 0),
        callingNumber = Phone_GetCallingNumber(playerInventory, slotid);

    new entityId, E_ENTITY_TYPE:entityType, phoneSlotId;
    if(World_FindPhoneByNumber(callingNumber, entityType, entityId, phoneSlotId))
    {
        new Inventory:entityInv = Inventory_Get(entityType, entityId);
        if(entityType == ENTITY_TYPE_PLAYER)
        {
            if(!Phone_IsOff(entityInv, phoneSlotId))
            {
                new name[32];
                if(PhoneBook_GetContactName(callingNumber, playerNumber, name))
                    SendFormattedMessage(entityId, COLOR_GREEN, "%s ha chiuso la chiamata.", name);
                else
                    SendFormattedMessage(entityId, COLOR_GREEN, "%d ha chiuso la chiamata.", playerNumber);
            }

            if(GetPlayerSpecialAction(entityId) == SPECIAL_ACTION_USECELLPHONE)
            {
                SetPlayerSpecialAction(entityId, SPECIAL_ACTION_STOPUSECELLPHONE);            
            }
            RemovePlayerAttachedObject(entityId, 6);

            UsingPhone{entityId} = false;
            UsingPhoneSlotID{entityId} = false;
        }
        Phone_StopCall(entityInv, phoneSlotId);
    }
    
    new callid = Phone_GetCallID(playerInventory, slotid);
    if(callid > 0)
    {
        Registry_SetReceiverState(callid, CALL_STATE_NOT_READ);
        Registry_Save(callid);
    }

    Phone_StopCall(playerInventory, slotid);

    UsingPhone{playerid} = false;
    PlayerEmergencyStatus{playerid} = 0;

    RemovePlayerAttachedObject(playerid, 6);
    SetPlayerSpecialAction(playerid, SPECIAL_ACTION_STOPUSECELLPHONE);
    ClearAnimations(playerid, 1);
    SendClientMessage(playerid, 0x8C8C8CFF, "[!] La chiamata è terminata.");
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

stock bool:Character_IsInCall(playerid)
{
    return UsingPhone{playerid};
}

stock Phone_StopCall(Inventory:inventory, slotid)
{
    Phone_SetRinging(inventory, slotid, false);
    Phone_SetBusy(inventory, slotid, false);
    Phone_SetCallingNumber(inventory, slotid, 0);
    Phone_SetIncomingFromNumber(inventory, slotid, 0);
    return 1;
}

stock Phone_SetCallingNumber(Inventory:inventory, slotid, number)
{
    return Inventory_SetSlotTempExtra(inventory, slotid, "call_with_number", number);
}

stock Phone_GetCallingNumber(Inventory:inventory, slotid)
{
    return Inventory_GetSlotTempExtra(inventory, slotid, "call_with_number");
}

stock Phone_SetIncomingFromNumber(Inventory:inventory, slotid, number)
{
    return Inventory_SetSlotTempExtra(inventory, slotid, "incoming_from_number", number);
}

stock Phone_GetIncomingFromNumber(Inventory:inventory, slotid)
{
    return Inventory_GetSlotTempExtra(inventory, slotid, "incoming_from_number");
}

stock bool:Phone_IsBusy(Inventory:inventory, slotid)
{
    return Inventory_GetSlotTempExtra(inventory, slotid, "is_busy") == 1;
}

stock bool:Phone_SetBusy(Inventory:inventory, slotid, bool:busy)
{
    return Inventory_SetSlotTempExtra(inventory, slotid, "is_busy", busy ? 1 : 0);
}

stock bool:Phone_SetSpeakerPhone(Inventory:inventory, slotid, bool:speakerPhone)
{
    return Inventory_SetSlotTempExtra(inventory, slotid, "speaker_phone", speakerPhone ? 1 : 0);
}

stock Phone_GetSpeakerPhone(Inventory:inventory, slotid)
{
    return Inventory_GetSlotTempExtra(inventory, slotid, "speaker_phone");
}

stock Phone_SetRinging(Inventory:inventory, slotid, bool:set)
{
    new number = Inventory_GetSlotExtra(inventory, slotid, 0);
    new Timer:timer = Timer:Inventory_GetSlotTempExtra(inventory, slotid, "ring_timer");
    if(timer != Timer:0)
        stop timer;
    timer = Timer:0;
    if(set)
    {
        RingPhoneTimer(number);
        timer = repeat RingPhoneTimer(number);
    }
    Inventory_SetSlotTempExtra(inventory, slotid, "ring_timer", _:timer);
    return 1;
}

stock Character_FindByPhoneNumber(number)
{
    new p = INVALID_PLAYER_ID;
    foreach(new i : Player)
    {
        if(!Character_IsLogged(i) || Inventory_GetPhoneSlotByNumber(Character_GetInventory(i), number) == -1)
            continue;
        p = i;
        break;        
    }
    return p;
}

stock bool:World_FindPhoneByNumber(number, &E_ENTITY_TYPE:entityType, &entityId, &phoneSlotId)
{
    return World_FindItem(gItem_Phone, number, entityType, entityId, phoneSlotId);
}

stock bool:Character_HasPhoneInSlot(playerid, slotid)
{
    return Character_IsValidSlot(playerid, slotid) && Character_GetSlotItem(playerid, slotid) == gItem_Phone;
}

// Returns Inventory's slot id of the phone. -1 if none.
stock Inventory_GetPhoneSlotByNumber(Inventory:inventory, number)
{
    for(new i = 0, j = Inventory_GetSpace(inventory); i < j; ++i)
    {
        if(Inventory_GetSlotItem(inventory, i) != gItem_Phone)
            continue;
        if(Inventory_GetSlotExtra(inventory, i, 0) == number)
        {
            return i;
        }
    }
    return -1;
}

stock Character_ShowPhone(playerid, slotid)
{
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return 0;
    SelectedPhone{playerid} = slotid;
    SelectedPhoneNumber[playerid] = Character_GetSlotExtra(playerid, slotid, 0);
    Dialog_Show_s(playerid, Dialog_UseCellPhone, DIALOG_STYLE_LIST, str_format("Cellulare - %d", SelectedPhoneNumber[playerid]), @("Info\nChiama\nSMS\nMessaggi non letti\nMessaggi in entrata\nMessaggi in uscita\nRegistro chiamate perse\nRegistro chiamate in entrata\nRegistro chiamate in uscita\nRubrica\nAggiungi contatto\nAccendi/Spegni"), "Seleziona", "Chiudi");
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
        Character_ShowPhoneInfo(playerid, slotid);
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
        return Character_ShowLostCalls(playerid, SelectedPhoneNumber[playerid]);
    }
    else if(listitem == 7)
    {
        return Character_ShowInBoundCalls(playerid, SelectedPhoneNumber[playerid]);
    }
    else if(listitem == 8)
    {
        return Character_ShowOutBoundCalls(playerid, SelectedPhoneNumber[playerid]);
    }
    else if(listitem == 9)
    {
        return Character_ShowPhoneBook(playerid, slotid);
    }
    else if(listitem == 10)
    {
        return Dialog_Show(playerid, Dialog_PhoneBookAddNumber, DIALOG_STYLE_INPUT, "Aggiungi numero in rubrica", "Inserisci il numero seguito dal nome che vuoi aggiungere in rubrica.\n== Esempio ==\n44411234 Mario Rossi", "Aggiungi", "Indietro");
    }
    else if(listitem == 11)
    {
        Character_ChangePhoneState(playerid, slotid);
    }
    return 1;
}

stock Character_ChangePhoneState(playerid, slotid)
{
    if(Character_IsInCall(playerid) && Character_GetUsingPhoneNumber(playerid) == Character_GetSlotExtra(playerid, slotid, 0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi spegnere questo telefono mentre stai chiamando."), 0;
    new Inventory:inv = Character_GetInventory(playerid);
    Phone_SetOff(inv, slotid, !Phone_IsOff(inv, slotid));
    if(Phone_IsOff(inv, slotid))
        SendClientMessage(playerid, COLOR_ERROR, "Hai spento il cellulare.");
    else
    {
        SendClientMessage(playerid, COLOR_GREEN, "Hai acceso il cellulare.");
        LoadPhone(playerid, Character_GetSlotExtra(playerid, slotid, 0));
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
    new n = PhoneBook_ParseValue(SelectedPhoneNumber[playerid], number);
    Character_SendSMS(playerid, SelectedPhone{playerid}, n, msg);
    return 1;
}

stock Character_GetEmergencyStatus(playerid)
{
    return PlayerEmergencyStatus{playerid};
}

stock Phone_SetOff(Inventory:inventory, slotid, bool:isOff)
{
    Inventory_SetSlotTempExtra(inventory, slotid, "phone_state", isOff ? PHONE_STATE_OFF : PHONE_STATE_ON);
}

stock Phone_IsOff(Inventory:inventory, slotid)
{
    return Inventory_GetSlotTempExtra(inventory, slotid, "phone_state") == PHONE_STATE_OFF;
}

static stock GetPhoneStatusName(status)
{
    new name[16];
    if(status == PHONE_STATE_ON)
        name = "Acceso";
    else
        name = "Spento";
    return name;
}

stock Character_GetUsingPhoneNumber(playerid)
{
    if(UsingPhoneSlotID[playerid] < 0)
        return 0;
    return Inventory_GetSlotExtra(Character_GetInventory(playerid), UsingPhoneSlotID[playerid], 0);
}

stock Character_GetSelectedPhoneSlot(playerid)
{
    return SelectedPhone{playerid};
}

stock Character_GetSelectedPhone(playerid)
{
    return SelectedPhoneNumber[playerid];
}

stock Character_SetSelectedPhone(playerid, number)
{
    SelectedPhoneNumber[playerid] = number;
}

static Character_ShowPhoneInfo(playerid, slotid)
{
    if(!Character_HasPhoneInSlot(playerid, slotid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un telefono in questo slot."), 0;
    
    new number = Character_GetSlotExtra(playerid, slotid, 0);

    await mysql_aquery_s(gMySQL, str_format("SELECT created_at FROM `phone_data` WHERE number = '%d'", number));

    if(cache_num_rows() <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Il cellulare non ha un numero valido."), 0;

    new time, year, month, day, hour, minute, second;
    cache_get_value_name_int(0, "created_at", time);

    TimestampToDate(time, year, month, day, hour, minute, second, 1);

    SendFormattedMessage(playerid, -1, "[Cellulare] Numero: %d - Acquistato il %d/%d/%d alle %d:%d.", number, day, month, year, hour, minute);
    return 1;
}