#include <YSI_Coding\y_hooks>

enum 
{
    CALL_STATE_NOT_READ,
    CALL_STATE_READ
} 

enum _:E_REGISTRY_ROW
{
    E_REGISTRY_CALLER_NUMBER,
    E_REGISTRY_CALLER_STATE,
    E_REGISTRY_RECEIVER_NUMBER,
    E_REGISTRY_RECEIVER_STATE,
    E_REGISTRY_START_TIME,
    E_REGISTRY_END_TIME,
};

static Map:RegistryMap; // <progressiveId, E_REGISTRY_ROW

static 
    List:Registry[MAX_PLAYERS],
    SelectedRow[MAX_PLAYERS];

hook OnGameModeInit()
{
    RegistryMap = map_new();
    mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `phone_call_registry` ( \
						`id` int(11) NOT NULL AUTO_INCREMENT, \
                        `number_owner` int(11) NOT NULL, \
                        `call_to_number` int(11) NOT NULL DEFAULT '0', \
                        `call_by_number` int(11) NOT NULL DEFAULT '0', \
                        `call_start` int(11) NOT NULL DEFAULT '0', \
                        `call_end` int(11) NOT NULL DEFAULT '0', \
                        `state` int(2) NOT NULL DEFAULT '0', \
						PRIMARY KEY (`id`) \
						) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=latin1");
    return 1;
}

hook OnPlayerClearData(playerid)
{
    if(list_valid(Registry[playerid]))
        list_delete_deep(Registry[playerid]);
    SelectedRow[playerid] = 0;
    return 1;
}

stock Character_ShowLostCalls(playerid, number)
{
    inline OnLoad()
    {
        new rows = cache_num_rows();

        if(rows <= 0)
            return SendClientMessage(playerid, COLOR_ERROR, "Il registro chiamate è vuoto.");
        
        new String:str = @("Utente\tData\n"), name[32], id, owner, callToNumber, callByNumber, callStart, callEnd,
            year, month, day, hour, minute, second;
        
        if(list_valid(Registry[playerid]))
            list_clear(Registry[playerid]);
        else
            Registry[playerid] = list_new();
        
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", id);
            cache_get_value_name_int(i, "number_owner", owner);
            cache_get_value_name_int(i, "call_to_number", callToNumber);
            cache_get_value_name_int(i, "call_by_number", callByNumber);
            cache_get_value_name_int(i, "call_start", callStart);
            cache_get_value_name_int(i, "call_end", callEnd);
            
            if(PhoneBook_GetContactName(owner, callByNumber, name))
                str += str_format("%s\t", name);
            else
                str += str_format("%d\t", callByNumber);
            
            TimestampToDate(callStart, year, month, day, hour, minute, second, 0);
            str += str_format("%d/%d/%d - %d:%d\n", day, month, year, hour, minute);

            list_add(Registry[playerid], id);
        }
        Dialog_Show_s(playerid, Dialog_LostPhoneCallRegister, DIALOG_STYLE_TABLIST_HEADERS, @("Registro chiamate"), str, "Elimina", "Indietro");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `phone_call_registry` WHERE number_owner = '%d' AND state = '%d'", number, CALL_STATE_NOT_READ);
    return 1;
}

stock Character_ShowInBoundCalls(playerid, number)
{
    inline OnLoad()
    {
        new rows = cache_num_rows();

        if(rows <= 0)
            return SendClientMessage(playerid, COLOR_ERROR, "Il registro chiamate è vuoto.");
        
        if(list_valid(Registry[playerid]))
            list_clear(Registry[playerid]);
        else
            Registry[playerid] = list_new();
        
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
            
            if(PhoneBook_GetContactName(owner, callByNumber, name))
                str += str_format("%s\t", name);
            else
                str += str_format("%d\t", callToNumber);

            new time = callEnd - callStart;
            TimestampToDate(time, year, month, day, hour, minute, second, 0);
            str += str_format("%d:%d\t", minute, second);

            TimestampToDate(callStart, year, month, day, hour, minute, second, 0);
            str += str_format("%d/%d/%d - %d:%d\n", day, month, year, hour, minute);

            list_add(Registry[playerid], id);
        }
        Dialog_Show_s(playerid, Dialog_InPhoneCallRegister, DIALOG_STYLE_TABLIST_HEADERS, @("Registro chiamate in entrata"), str, "Elimina", "Chiudi");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `phone_call_registry` WHERE number_owner = '%d' AND call_by_number != '%d' AND state = '%d'", number, number, CALL_STATE_READ);
    return 1;
}

stock Character_ShowOutBoundCalls(playerid, number)
{
    inline OnLoad()
    {
        new rows = cache_num_rows();

        if(rows <= 0)
            return SendClientMessage(playerid, COLOR_ERROR, "Il registro chiamate è vuoto.");
        
        if(list_valid(Registry[playerid]))
            list_clear(Registry[playerid]);
        else
            Registry[playerid] = list_new();
        
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
            
            if(PhoneBook_GetContactName(owner, callToNumber, name))
                str += str_format("%s\t", name);
            else
                str += str_format("%d\t", callToNumber);

            new time = callEnd - callStart;
            TimestampToDate(time, year, month, day, hour, minute, second, 0);
            str += str_format("%d:%d\t", minute, second);

            TimestampToDate(callStart, year, month, day, hour, minute, second, 0);
            str += str_format("%d/%d/%d - %d:%d\n", day, month, year, hour, minute);

            list_add(Registry[playerid], id);
        }
        Dialog_Show_s(playerid, Dialog_OutPhoneCallRegister, DIALOG_STYLE_TABLIST_HEADERS, @("Registro chiamate in uscita"), str, "Elimina", "Chiudi");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `phone_call_registry` WHERE number_owner = '%d' AND call_by_number = '%d' AND state = '%d'", number, number, CALL_STATE_READ);
    return 1;
}

stock bool:Registry_SetReceiverState(callid, st)
{
    if(!map_has_key(RegistryMap, callid))
        return false;
    map_set_cell(RegistryMap, callid, E_REGISTRY_RECEIVER_STATE, st);
    return true;
}

stock Registry_Create(callByNumber, callToNumber)
{
    static progressiveId = 0;

    new data[E_REGISTRY_ROW];
    data[E_REGISTRY_CALLER_NUMBER] = callByNumber;
    data[E_REGISTRY_CALLER_STATE] = CALL_STATE_READ;
    data[E_REGISTRY_RECEIVER_NUMBER] = callToNumber;
    data[E_REGISTRY_START_TIME] = gettime();

    progressiveId++;
    map_add_arr(RegistryMap, progressiveId, data);
    return progressiveId;
}

stock bool:Registry_Save(regid)
{
    if(!map_has_key(RegistryMap, regid))
        return false;

    new data[E_REGISTRY_ROW];
    map_get_arr(RegistryMap, regid, data);

    data[E_REGISTRY_END_TIME] = gettime();

    mysql_tquery_f(gMySQL, "INSERT INTO `phone_call_registry` (number_owner, call_to_number, call_by_number, call_start, call_end, state) \
    VALUES('%d', '%d', '%d', '%d', '%d', '%d')", data[E_REGISTRY_CALLER_NUMBER], data[E_REGISTRY_RECEIVER_NUMBER], data[E_REGISTRY_CALLER_NUMBER], data[E_REGISTRY_START_TIME], data[E_REGISTRY_END_TIME], CALL_STATE_READ);

    mysql_tquery_f(gMySQL, "INSERT INTO `phone_call_registry` (number_owner, call_to_number, call_by_number, call_start, call_end, state) \
    VALUES('%d', '%d', '%d', '%d', '%d', '%d')", data[E_REGISTRY_RECEIVER_NUMBER], data[E_REGISTRY_RECEIVER_NUMBER], data[E_REGISTRY_CALLER_NUMBER], data[E_REGISTRY_START_TIME], data[E_REGISTRY_END_TIME], data[E_REGISTRY_RECEIVER_STATE]);

    map_remove(RegistryMap, regid);
    return true;
}

stock Phone_SetCallID(Inventory:inventory, slotid, id)
{
    return Inventory_SetSlotTempExtra(inventory, slotid, "call_id", id);
}

stock Phone_GetCallID(Inventory:inventory, slotid)
{
    return Inventory_GetSlotTempExtra(inventory, slotid, "call_id");
}

Dialog:Dialog_LostPhoneCallRegister(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Character_ShowPhone(playerid, Character_GetSelectedPhoneSlot(playerid));
    SelectedRow[playerid] = list_get(Registry[playerid], listitem);
    
    SendClientMessage(playerid, COLOR_GREEN, "Hai eliminato l'elemento del registro.");

    mysql_tquery_f(gMySQL, "DELETE FROM `phone_call_registry` WHERE id = '%d'", SelectedRow[playerid]);
    Character_ShowLostCalls(playerid, Character_GetSelectedPhone(playerid));
    return 1;
}

Dialog:Dialog_InPhoneCallRegister(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Character_ShowPhone(playerid, Character_GetSelectedPhoneSlot(playerid));
    SelectedRow[playerid] = list_get(Registry[playerid], listitem);
    
    SendClientMessage(playerid, COLOR_GREEN, "Hai eliminato l'elemento del registro.");

    mysql_tquery_f(gMySQL, "DELETE FROM `phone_call_registry` WHERE id = '%d'", SelectedRow[playerid]);

    Character_ShowInBoundCalls(playerid, Character_GetSelectedPhone(playerid));
    return 1;
}

Dialog:Dialog_OutPhoneCallRegister(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Character_ShowPhone(playerid, Character_GetSelectedPhoneSlot(playerid));
    SelectedRow[playerid] = list_get(Registry[playerid], listitem);
    
    SendClientMessage(playerid, COLOR_GREEN, "Hai eliminato l'elemento del registro.");

    mysql_tquery_f(gMySQL, "DELETE FROM `phone_call_registry` WHERE id = '%d'", SelectedRow[playerid]);
    Character_ShowOutBoundCalls(playerid, Character_GetSelectedPhone(playerid));
    return 1;
}