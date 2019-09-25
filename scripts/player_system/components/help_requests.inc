
#include <YSI_Coding\y_hooks>

#define MAX_REQUESTS                (100)
#define REQUESTS_PER_PAGE           (15)

static enum _:E_REQUEST_TYPE
{
    REQUEST_TYPE_SUPPORTER = 0,
    REQUEST_TYPE_MODERATOR = 1,

    REQUEST_TYPE_LAST_
};

static 
    RequestCounter[MAX_PLAYERS char],
    RequestTime[MAX_PLAYERS],
    CurrentRequestPage[MAX_PLAYERS char],
    CurrentRequests[MAX_PLAYERS][MAX_REQUESTS],
    SelectedRequest[MAX_PLAYERS],
    RequestNames[REQUEST_TYPE_LAST_][24] = {
        {"Tester"}, {"Moderatori"}
    };

hook OnGameModeInit()
{
    mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `staff_requests` ( \
                        `id` int(11) NOT NULL AUTO_INCREMENT, \
                        `owner_id` tinyint(11) DEFAULT '0', \
                        `type` int(11) NOT NULL DEFAULT '0', \
                        `text` VARCHAR(255) NOT NULL DEFAULT '', \
                        `time` int(11) NOT NULL DEFAULT '0', \
                        PRIMARY KEY (`id`) \
                        ) ENGINE=InnoDB DEFAULT CHARSET=latin1");
    
    mysql_tquery(gMySQL, "DELETE FROM `staff_requests` WHERE 1");
    mysql_tquery(gMySQL, "ALTER TABLE `staff_requests` AUTO_INCREMENT = 1");
    return 1;
}

hook OnPlayerClearData(playerid)
{
    CurrentRequestPage{playerid} = 0;
    RequestCounter{playerid} = 0;
    SelectedRequest[playerid] = 0;
    StaffRequest_Reset(playerid);
    return 1;
}

flags:aiutami(CMD_USER);
CMD:aiutami(playerid, params[])
{
    if(isnull(params) || strlen(params) <= 0 || strlen(params) > 255)
        return SendClientMessage(playerid, COLOR_ERROR, "/aiutami <testo>");
    
    if(RequestTime[playerid] > gettime())
        return SendFormattedMessage(playerid, COLOR_ERROR, "Devi aspettare %d secondi tra una richiesta e l'altra.", RequestTime[playerid] - gettime());

    if(RequestCounter{playerid} >= 3)
        return SendClientMessage(playerid, COLOR_ERROR, "Hai già inviato il massimo di richieste allo staff."), 0;

    StaffRequest_Add(playerid, REQUEST_TYPE_SUPPORTER, params);

    SendMessageToTesters(0, 0xe9de98ff, "[TESTER]: %s (%d) ha effettuato una richiesta: %s", Character_GetOOCName(playerid), playerid, params);
    SendMessageToTesters(0, 0xE9DE98FF, "Digita /ar per accettare la richiesta.");

    SendClientMessage(playerid, -1, "Richiesta inviata ai testers.");
    SendFormattedMessage(playerid, -1, "Al momento ci sono %d richieste prima della tua.", StaffRequest_GetQueue(REQUEST_TYPE_SUPPORTER, gettime()));
    return 1;
}

flags:domanda(CMD_USER);
CMD:domanda(playerid, params[])
{
    if(isnull(params) || strlen(params) <= 0 || strlen(params) > 255)
        return SendClientMessage(playerid, COLOR_ERROR, "(/dom)anda <testo>");
    
    if(RequestTime[playerid] > gettime())
        return SendFormattedMessage(playerid, COLOR_ERROR, "Devi aspettare %d secondi tra una richiesta e l'altra.", RequestTime[playerid] - gettime());

    if(RequestCounter{playerid} >= 3)
        return SendClientMessage(playerid, COLOR_ERROR, "Hai già inviato il massimo di richieste allo staff."), 0;

    StaffRequest_Add(playerid, REQUEST_TYPE_MODERATOR, params);
    
    SendMessageToAdmins(0, 0xe9de98ff, "[ADMIN-REQ]: %s (%d) chiede: %s", Character_GetOOCName(playerid), playerid, params);

    SendClientMessage(playerid, -1, "Hai inviato una richiesta di supporto allo staff. Utilizza /richieste per vederne lo stato nella coda. Ti chiediamo di attendere.");
    SendFormattedMessage(playerid, -1, "Al momento ci sono %d richieste prima della tua.", StaffRequest_GetQueue(REQUEST_TYPE_MODERATOR, gettime()));
    return 1;
}
alias:domanda("dom");

flags:richieste(CMD_USER);
CMD:richieste(playerid, params[])
{   
    Character_ShowMyRequests(playerid, 0);
    return 1;
}

flags:lr(CMD_SUPPORTER);
CMD:lr(playerid, params[])
{
    new type = (Account_GetAdminLevel(playerid) == 1) ? (REQUEST_TYPE_SUPPORTER) : (REQUEST_TYPE_MODERATOR);

    if(!isnull(params) && !strcmp(params, "all", true) && Account_GetAdminLevel(playerid) > 1)
        return StaffRequest_ShowAll(playerid);
    StaffRequest_ShowByType(playerid, type);
    return 1;
}

flags:rr(CMD_SUPPORTER);
CMD:rr(playerid, params[])
{
    new rid, reason[128];
    if(sscanf(params, "ds[128]", rid, reason))
        return SendClientMessage(playerid, COLOR_ERROR, "/rr <requestid> <motivo>");
    
    StaffRequest_Decline(playerid, rid, reason);
    return 1;
}

flags:ar(CMD_SUPPORTER);
CMD:ar(playerid, params[])
{
    new rid;
    if(sscanf(params, "d", rid))
        return SendClientMessage(playerid, COLOR_ERROR, "/ar <requestid>");
    
    StaffRequest_Accept(playerid, rid);
    return 1;
}

static StaffRequest_ShowByType(playerid, type)
{
    if(Account_GetAdminLevel(playerid) <= 0)
        return 0;
    inline OnLoad()
    {
        new rows = cache_num_rows();

        if(rows <= 0)
            return SendClientMessage(playerid, COLOR_ERROR, "Non ci sono richieste da parte di utenti.");

        if(rows > MAX_REQUESTS)
            rows = MAX_REQUESTS;
        new 
            id, owner,
            String:str = @("ID\tGiocatore (ID)\n")
        ;

        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", id);
            cache_get_value_name_int(i, "owner_id", owner);
            
            CurrentRequests[playerid][i] = id;

            str += str_format("%d\t%s (%d)\n", id, Character_GetOOCName(owner), owner);
        }
        Dialog_Show_s(playerid, Dialog_Requests, DIALOG_STYLE_TABLIST_HEADERS, @("Richieste"), str, "Seleziona", "Chiudi");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `staff_requests` WHERE type = '%d' ORDER BY time ASC", type);
    return 1;
}

static StaffRequest_ShowAll(playerid)
{
    if(Account_GetAdminLevel(playerid) <= 0)
        return 0;
    inline OnLoad()
    {
        new rows = cache_num_rows();

        if(rows <= 0)
            return SendClientMessage(playerid, COLOR_ERROR, "Non ci sono richieste da parte di utenti.");

        if(rows > MAX_REQUESTS)
            rows = MAX_REQUESTS;
        new 
            id, owner,
            String:str = @("ID\tGiocatore (ID)\n")
        ;

        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", id);
            cache_get_value_name_int(i, "owner_id", owner);
            
            CurrentRequests[playerid][i] = id;

            str += str_format("%d\t%s (%d)\n", id, Character_GetOOCName(owner), owner);
        }
        Dialog_Show_s(playerid, Dialog_Requests, DIALOG_STYLE_TABLIST_HEADERS, @("Richieste"), str, "Seleziona", "Chiudi");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `staff_requests` ORDER BY time ASC");
    return 1;
}

Dialog:Dialog_Requests(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    SelectedRequest[playerid] = CurrentRequests[playerid][listitem];

    inline OnLoad()
    {
        new id, owner, time, type, text[255],
            year, month, day, hours, minutes, seconds
        ;

        cache_get_value_name_int(0, "id", id);
        cache_get_value_name_int(0, "owner_id", owner);
        cache_get_value_name_int(0, "time", time);
        cache_get_value_name_int(0, "type", type);
        cache_get_value_name(0, "text", text);

        TimestampToDate(time, year, month, day, hours, minutes, seconds, 1);

        new String:str = str_format("Richiesta %s - #%d\n\nOrario di invio: %d:%d\n\nRichieste con precedenza: %d\n\nTesto: %s", RequestNames[type], id, hours, minutes, StaffRequest_GetQueue(type, time), text);
        Dialog_Show_s(playerid, Dialog_SelectedStaffRequest, DIALOG_STYLE_MSGBOX, @("Richiesta"), str, "Opzioni", "Indietro");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `staff_requests` WHERE id = '%d'", SelectedRequest[playerid]);


    return 1;
}

Dialog:Dialog_SelectedStaffRequest(playerid, response, listitem, inputtext[])
{
    if(!response)
        return StaffRequest_ShowAll(playerid);
    Dialog_Show_s(playerid, Dialog_StaffRequestOptions, DIALOG_STYLE_LIST, str_format("Richiesta %d", SelectedRequest[playerid]), @("Accetta\nRifiuta"), "Seleziona", "Indietro");
    return 1;
}

Dialog:Dialog_StaffRequestOptions(playerid, response, listitem, inputtext[])
{
    if(!response)
        return StaffRequest_ShowAll(playerid);
    if(listitem == 0)
        StaffRequest_Accept(playerid, SelectedRequest[playerid]);
    else
        Dialog_Show(playerid, Dialog_StaffReqDeclineReason, DIALOG_STYLE_INPUT, "Rifiuta Richiesta", "Inserisci il motivo del rifiuto.", "Rifiuta", "Indietro");
    return 1;
}

Dialog:Dialog_StaffReqDeclineReason(playerid, response, listitem, inputtext[])
{
    if(!response)
        Dialog_Show_s(playerid, Dialog_StaffRequestOptions, DIALOG_STYLE_LIST, str_format("Richiesta %d", SelectedRequest[playerid]), @("Accetta\nRifiuta"), "Seleziona", "Indietro");
    if(strlen(inputtext) <= 0 || strlen(inputtext) > 250)
        return Dialog_Show(playerid, Dialog_StaffReqDeclineReason, DIALOG_STYLE_INPUT, "Rifiuta Richiesta", "{FF0000}Il motivo inserito è troppo lungo.{FFFFFF}\nInserisci il motivo del rifiuto.", "Rifiuta", "Indietro");
    
    StaffRequest_Decline(playerid, SelectedRequest[playerid], inputtext);
    return 1;
}

static StaffRequest_Accept(playerid, requestid)
{
    inline OnLoad()
    {
        if(!cache_num_rows())
            return SendClientMessage(playerid, COLOR_ERROR, "La richiesta non esiste.");
        
        new id, owner, type;

        cache_get_value_name_int(0, "id", id);
        cache_get_value_name_int(0, "owner_id", owner);
        cache_get_value_name_int(0, "type", type);

        if(Account_GetAdminLevel(playerid) < 2 && type > REQUEST_TYPE_SUPPORTER)
            return SendClientMessage(playerid, COLOR_ERROR, "Non puoi interagire con questa richiesta.");

        if(Account_GetAdminLevel(playerid) == 1)
        {
            SendFormattedMessage(owner, COLOR_GREEN, "%s (%d) ha accettato la tua richiesta.", Character_GetOOCName(playerid), playerid);
            SendMessageToTesters(0, 0xe9de98ff, "TesterCmd: %s (%d) ha accettato la richiesta di %s (%d).", Character_GetOOCName(playerid), playerid, Character_GetOOCName(owner), owner);
        }
        else if(Account_GetAdminLevel(playerid) > 1)
        {
            SendFormattedMessage(owner, COLOR_GREEN, "%s (%d) ha accettato la tua richiesta", Account_GetName(playerid), playerid);
            SendMessageToAdmins(0, 0xFF6347FF, "AdmCmd: %s (%d) ha accettato la richiesta di %s (%d)." , Account_GetName(playerid), playerid, Character_GetOOCName(owner), owner);
        }

        RequestCounter{owner}--;

        SelectedRequest[playerid] = 0;
        mysql_tquery_f(gMySQL, "DELETE FROM `staff_requests` WHERE id = '%d'", requestid);
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `staff_requests` WHERE id = '%d'", requestid);
    return 1;
}

static StaffRequest_Decline(playerid, requestid, const reason[])
{
    inline OnLoad()
    {
        if(!cache_num_rows())
            return SendClientMessage(playerid, COLOR_ERROR, "La richiesta non esiste.");
        
        new id, owner, type, mot[255]
        ;

        cache_get_value_name_int(0, "id", id);
        cache_get_value_name_int(0, "owner_id", owner);
        cache_get_value_name_int(0, "type", type);
        cache_get_value_name(0, "reason", mot);

        if(Account_GetAdminLevel(playerid) < 2 && type > REQUEST_TYPE_SUPPORTER)
            return SendClientMessage(playerid, COLOR_ERROR, "Non puoi interagire con questa richiesta.");

        if(Account_GetAdminLevel(playerid) == 1)
        {
            SendFormattedMessage(owner, COLOR_GREEN, "%s (%d) ha rifiutato la tua richiesta. Motivo: %s", Character_GetOOCName(playerid), playerid, mot);
            SendMessageToTesters(0, COLOR_SUPPORTER, "TesterCmd: %s (%d) ha rifiutato la richiesta di %s (%d). Motivo: %s", Character_GetOOCName(playerid), playerid, Character_GetOOCName(owner), owner, mot);
        }
        else if(Account_GetAdminLevel(playerid) > 1)
        {
            SendFormattedMessage(owner, COLOR_GREEN, "%s (%d) ha rifiutato la tua richiesta. Motivo: %s", Account_GetName(playerid), playerid, mot);
            SendMessageToAdmins(0, 0xFF6347FF, "AdmCmd: %s (%d) ha rifiutato la richiesta di %s (%d). Motivo: %s" , Account_GetName(playerid), playerid, Character_GetOOCName(owner), owner, mot);
        }

        RequestCounter{owner}--;
        SelectedRequest[playerid] = 0;
        mysql_tquery_f(gMySQL, "DELETE FROM `staff_requests` WHERE id = '%d'", requestid);

        StaffRequest_ShowAll(playerid);
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT *, '%e' as reason FROM `staff_requests` WHERE id = '%d'", reason, requestid);
    return 1;
}

static Character_ShowMyRequests(playerid, page)
{
    if(!Character_IsLogged(playerid) || page < 0)
        return 0;
    CurrentRequestPage{playerid} = page;
    
    inline OnLoad()
    {
        new rows = cache_num_rows();

        if(rows <= 0)
            return SendClientMessage(playerid, COLOR_ERROR, "Non hai richieste inviate allo staff attive.");

        new id, type, text[255], time;
        new String:str = @("Richiesta ID\tOrario\tTipo\n");

        new year, month, day, hours, minutes, seconds;
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", id);
            cache_get_value_name_int(i, "type", type);
            cache_get_value_name(i, "text", text);
            cache_get_value_name_int(i, "time", time);

            if(type < 0 || type >= REQUEST_TYPE_LAST_)
                continue;
            TimestampToDate(time, year, month, day, hours, minutes, seconds, 1);
            str += str_format("%d\t%d:%d\t%s\n", id, hours, minutes, RequestNames[type]);
            
            CurrentRequests[playerid][i] = id; 
        }

        Dialog_Show_s(playerid, Dialog_ShowPlayerRequests, DIALOG_STYLE_TABLIST_HEADERS, @("Richieste Staff"), str, "Continua", "Chiudi");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `staff_requests` WHERE owner_id = '%d' LIMIT %d, %d", playerid, page * REQUESTS_PER_PAGE, REQUESTS_PER_PAGE);
    return 1;
}

Dialog:Dialog_ShowPlayerRequests(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    SelectedRequest[playerid] = CurrentRequests[playerid][listitem];
    Dialog_Show_s(playerid, Dialog_RequestView, DIALOG_STYLE_LIST, str_format("Richiesta %d", SelectedRequest[playerid]), @("Visualizza\nAnnulla"), "Continua", "Chiudi");
    return 1;
}

Dialog:Dialog_RequestView(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Character_ShowMyRequests(playerid, CurrentRequestPage{playerid});
    
    if(listitem == 0)
    {
        inline OnLoad()
        {
            new rows = cache_num_rows();

            if(!rows)
                return 0;

            new id, type, text[255], time;

            cache_get_value_name_int(0, "id", id);
            cache_get_value_name_int(0, "type", type);
            cache_get_value_name(0, "text", text);
            cache_get_value_name_int(0, "time", time);

            new String:str = str_format("%s\n\nCi sono ancora %d richieste prima della tua.", text, StaffRequest_GetQueue(type, time));
            Dialog_Show_s(playerid, Dialog_RequestViewText, DIALOG_STYLE_MSGBOX, @("Richieste - Testo"), str, "Continua", "Indietro");
        }
        MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `staff_requests` WHERE id = '%d'", SelectedRequest[playerid]);
    }
    else if(listitem == 1)
    {
        mysql_tquery_f(gMySQL, "DELETE FROM `staff_requests` WHERE id = '%d'", SelectedRequest[playerid]);
        Character_ShowMyRequests(playerid, CurrentRequestPage{playerid});
    }
    return 1;
}

Dialog:Dialog_RequestViewText(playerid, response, listitem, inputtext[])
{
    Character_ShowMyRequests(playerid, CurrentRequestPage{playerid});
    return 1;
}

stock StaffRequest_Add(playerid, type, const text[])
{
    if(!Character_IsLogged(playerid))
        return 0;
    RequestCounter{playerid}++;
    RequestTime[playerid] = gettime() + 20;
    mysql_tquery_f(gMySQL, "INSERT INTO `staff_requests` (owner_id, type, text, time) VALUES('%d', '%d', '%e', '%d')", playerid, type, text, gettime());
    return 1;
}

stock StaffRequest_Reset(playerid)
{
    if(!Character_IsLogged(playerid))
        return 0;
    mysql_tquery_f(gMySQL, "DELETE FROM `staff_requests` WHERE owner_id = '%d'", playerid);
    return 1;
}

static StaffRequest_GetQueue(requestType, timestamp)
{
    new Cache:cache = mysql_query_f(gMySQL, true, "SELECT time FROM `staff_requests` WHERE type = '%d' AND time < '%d'", requestType, timestamp);
    new rows = cache_num_rows();
    cache_delete(cache);
	return rows;
}

