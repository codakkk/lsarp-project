
#include <YSI_Coding\y_hooks>

static const REQUESTS_PER_PAGE = 15;

static enum _:E_REQUEST_TYPE
{
    REQUEST_TYPE_SUPPORTER = 0,
    REQUEST_TYPE_MODERATOR = 1,
    REQUEST_TYPE_REPORT = 2,

    REQUEST_TYPE_LAST_
};

static const REQUEST_NAME[REQUEST_TYPE_LAST_][24] = {{"Tester"}, {"Mod/Admin"}, {"Report"}};

static 
    List:PlayerRequests[MAX_PLAYERS],
    PlayerRequestsCounter[MAX_PLAYERS char],
    PlayerRequestTime[MAX_PLAYERS],
    CurrentRequestPage[MAX_PLAYERS char],
    CurrentRequestType[MAX_PLAYERS char],
    SelectedRequest[MAX_PLAYERS]
 ;

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

hook OnCharacterClearData(playerid)
{
    if(list_valid(PlayerRequests[playerid]))
        list_delete_deep(PlayerRequests[playerid]);
    PlayerRequests[playerid] = List:0;
    PlayerRequestsCounter{playerid} = 0;
    PlayerRequestTime[playerid] = 0;
    CurrentRequestPage{playerid} = 0;
    CurrentRequestType{playerid} = REQUEST_TYPE_LAST_;
    SelectedRequest[playerid] = 0;
    StaffRequest_Reset(playerid);
    return 1;
}

flags:aiutami(CMD_USER);
CMD:aiutami(playerid, params[])
{
    if(isnull(params) || strlen(params) <= 0 || strlen(params) > 255)
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /aiutami <testo>");
    
    if(PlayerRequestTime[playerid] > gettime())
        return SendFormattedMessage(playerid, COLOR_ERROR, "Devi aspettare %d secondi tra una richiesta e l'altra.", PlayerRequestTime[playerid] - gettime());

    if(PlayerRequestsCounter{playerid} >= 3)
        return SendClientMessage(playerid, COLOR_ERROR, "Hai già inviato il massimo di richieste allo staff."), 0;

    StaffRequest_Add(playerid, REQUEST_TYPE_SUPPORTER, params);

    SendClientMessage(playerid, -1, "Richiesta inviata ai testers.");
    SendFormattedMessage(playerid, -1, "Al momento ci sono %d richieste prima della tua.", StaffRequest_GetQueue(REQUEST_TYPE_SUPPORTER, gettime()));
    return 1;
}

flags:domanda(CMD_USER);
CMD:domanda(playerid, params[])
{
    if(isnull(params) || strlen(params) <= 0 || strlen(params) > 255)
        return SendClientMessage(playerid, COLOR_ERROR, "(/dom)anda <testo>");
    
    if(PlayerRequestTime[playerid] > gettime())
        return SendFormattedMessage(playerid, COLOR_ERROR, "Devi aspettare %d secondi tra una richiesta e l'altra.", PlayerRequestTime[playerid] - gettime());

    if(PlayerRequestsCounter{playerid} >= 3)
        return SendClientMessage(playerid, COLOR_ERROR, "Hai già inviato il massimo di richieste allo staff."), 0;

    StaffRequest_Add(playerid, REQUEST_TYPE_MODERATOR, params);

    SendClientMessage(playerid, -1, "Hai inviato una richiesta di supporto allo staff. Utilizza /richieste per vederne lo stato nella coda. Ti chiediamo di attendere.");
    SendFormattedMessage(playerid, -1, "Al momento ci sono %d richieste prima della tua.", StaffRequest_GetQueue(REQUEST_TYPE_MODERATOR, gettime()));
    return 1;
}
alias:domanda("dom");

flags:richieste(CMD_USER);
CMD:richieste(playerid, params[])
{   
    StaffRequest_Show(playerid, 0, REQUEST_TYPE_LAST_);
    return 1;
}

flags:lr(CMD_TESTER);
CMD:lr(playerid, params[])
{
    new type = (Account_GetAdminLevel(playerid) == 1) ? (REQUEST_TYPE_SUPPORTER) : (REQUEST_TYPE_MODERATOR);
    
    if(Account_GetAdminLevel(playerid) > 1)
    {
        if(!isnull(params) && !strcmp(params, "all", true))
            return StaffRequest_Show(playerid, 0, -1);
        else
            SendClientMessage(playerid, COLOR_ORANGE, "Ricorda che puoi utilizzare /lr all per visualizzare tutti i tipi di richieste.");
    }
    StaffRequest_Show(playerid, 0, type);
    return 1;
}

flags:rr(CMD_TESTER);
CMD:rr(playerid, params[])
{
    new rid, reason[128];
    if(sscanf(params, "ds[128]", rid, reason))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /rr <requestid> <motivo>");
    
    StaffRequest_Decline(playerid, rid, reason);
    return 1;
}

flags:ar(CMD_TESTER);
CMD:ar(playerid, params[])
{
    new rid;
    if(sscanf(params, "d", rid))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /ar <requestid>");
    
    StaffRequest_Accept(playerid, rid);
    return 1;
}

// -1 means all
// REQUEST_TYPE_LAST_ means player's requests
static StaffRequest_Show(playerid, page, typeToShow = -1)
{
    if(typeToShow != REQUEST_TYPE_LAST_ && Account_GetAdminLevel(playerid) <= 0)
        return 0;
    
    CurrentRequestPage{playerid} = page;
    CurrentRequestType{playerid} = typeToShow;
    inline OnLoad()
    {
        new rows = cache_num_rows();

        if(rows <= 0)
        {
            if(typeToShow == REQUEST_TYPE_LAST_)
                SendClientMessage(playerid, COLOR_ERROR, "Non ci sono richieste da parte tua.");
            else
                SendClientMessage(playerid, COLOR_ERROR, "Non ci sono richieste da parte di utenti.");
            return 1;
        }

        if(!list_valid(PlayerRequests[playerid]))
            PlayerRequests[playerid] = list_new();

        list_clear(PlayerRequests[playerid]);

        new String:str;

        if(typeToShow == REQUEST_TYPE_LAST_)
        {
            str = @("ID\tTipo\tOrario\n");
        }
        else
        {
            str = @("ID\tGiocatore (ID)\tTipo\n");
        }
        new  id, owner, type, time;

        if(page > 0)
            str += @("Pagina\tPrecedente\t\n");

        if(rows > REQUESTS_PER_PAGE)
            str += @("Pagina\tSuccessiva\t\n");

        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", id);
            cache_get_value_name_int(i, "owner_id", owner);
            cache_get_value_name_int(i, "type", type);
            cache_get_value_name_int(i, "time", time);


            if(typeToShow == REQUEST_TYPE_LAST_)
            {
                new a, b, c, h, m, s;
                TimestampToDate(time, a, b, c, h, m, s, 0);
                str += str_format("%d\t%s\t%02d:%02d:%02d\n", id, REQUEST_NAME[type], h, m, s);
            }
            else
            {
                str += str_format("%d\t%s (%d)\t%s\n", id, Character_GetOOCName(owner), owner, REQUEST_NAME[type]);
            }

            list_add(PlayerRequests[playerid], id);
        }
        Dialog_Show_s(playerid, Dialog_Requests, DIALOG_STYLE_TABLIST_HEADERS, @("Richieste"), str, "Seleziona", "Chiudi");
    }
    if(typeToShow == -1)
        MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `staff_requests` ORDER BY time ASC LIMIT %d, %d", page * REQUESTS_PER_PAGE, REQUESTS_PER_PAGE + 1);
    else if(typeToShow == REQUEST_TYPE_LAST_)
        MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `staff_requests` WHERE owner_id = '%d' ORDER BY time ASC LIMIT %d, %d", playerid, page * REQUESTS_PER_PAGE, REQUESTS_PER_PAGE+1);
    else
        MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `staff_requests` WHERE type = '%d' ORDER BY time ASC LIMIT %d, %d", typeToShow, page * REQUESTS_PER_PAGE, REQUESTS_PER_PAGE + 1);
    return 1;
}

Dialog:Dialog_Requests(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    
    if(CurrentRequestPage{playerid} > 0 )
    {
        if(listitem == 0)
        {
            return StaffRequest_Show(playerid, CurrentRequestPage{playerid}-1, CurrentRequestType{playerid});
        }
        listitem--;
    }
    if(list_size(PlayerRequests[playerid]) > REQUESTS_PER_PAGE)
    {
        if(listitem == 0)
        {
            return StaffRequest_Show(playerid, CurrentRequestPage{playerid}+1, CurrentRequestType{playerid});
        }
        listitem--;
    }

    SelectedRequest[playerid] = list_get(PlayerRequests[playerid], listitem);

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

        new String:str = str_format("Richiesta %s - #%d\n\nOrario di invio: %02d:%02d\n\nRichieste con precedenza: %d\n\nTesto: %s", REQUEST_NAME[type], id, hours, minutes, StaffRequest_GetQueue(type, time), text);
        if(CurrentRequestType{playerid} == REQUEST_TYPE_LAST_)
        {
            Dialog_Show_s(playerid, Dialog_PlayerRequestView, DIALOG_STYLE_MSGBOX, @("Richiesta"), str, "Annulla", "Indietro");
        }
        else
        {
            Dialog_Show_s(playerid, Dialog_SelectedStaffRequest, DIALOG_STYLE_MSGBOX, @("Richiesta"), str, "Opzioni", "Indietro");
        }
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `staff_requests` WHERE id = '%d'", SelectedRequest[playerid]);
    return 1;
}

Dialog:Dialog_SelectedStaffRequest(playerid, response, listitem, inputtext[])
{
    if(!response)
        return StaffRequest_Show(playerid, CurrentRequestPage{playerid}, CurrentRequestType{playerid});
    Dialog_Show_s(playerid, Dialog_StaffRequestOptions, DIALOG_STYLE_LIST, str_format("Richiesta %d", SelectedRequest[playerid]), @("Accetta\nRifiuta"), "Seleziona", "Indietro");
    return 1;
}

Dialog:Dialog_StaffRequestOptions(playerid, response, listitem, inputtext[])
{
    if(!response)
        return StaffRequest_Show(playerid, CurrentRequestPage{playerid}, CurrentRequestType{playerid});
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

        PlayerRequestsCounter{owner}--;

        SelectedRequest[playerid] = 0;
        mysql_tquery_f(gMySQL, "DELETE FROM `staff_requests` WHERE id = '%d'", requestid);
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `staff_requests` WHERE id = '%d'", requestid);
    return 1;
}

static StaffRequest_Decline(playerid, requestid, const reason[])
{
    new tempReason[255];
    strins(tempReason, reason, 0);
    inline OnLoad()
    {
        if(!cache_num_rows())
            return SendClientMessage(playerid, COLOR_ERROR, "La richiesta non esiste.");
        
        new id, owner, type
        ;

        cache_get_value_name_int(0, "id", id);
        cache_get_value_name_int(0, "owner_id", owner);
        cache_get_value_name_int(0, "type", type);

        if(Account_GetAdminLevel(playerid) < 2 && type > REQUEST_TYPE_SUPPORTER)
            return SendClientMessage(playerid, COLOR_ERROR, "Non puoi interagire con questa richiesta.");

        if(Account_GetAdminLevel(playerid) == 1)
        {
            SendFormattedMessage(owner, COLOR_GREEN, "%s (%d) ha rifiutato la tua richiesta. Motivo: %s", Character_GetOOCName(playerid), playerid, tempReason);
            SendMessageToTesters(0, COLOR_SUPPORTER, "TesterCmd: %s (%d) ha rifiutato la richiesta di %s (%d). Motivo: %s", Character_GetOOCName(playerid), playerid, Character_GetOOCName(owner), owner, tempReason);
        }
        else if(Account_GetAdminLevel(playerid) > 1)
        {
            SendFormattedMessage(owner, COLOR_GREEN, "%s (%d) ha rifiutato la tua richiesta. Motivo: %s", Account_GetName(playerid), playerid, tempReason);
            SendMessageToAdmins(0, 0xFF6347FF, "AdmCmd: %s (%d) ha rifiutato la richiesta di %s (%d). Motivo: %s" , Account_GetName(playerid), playerid, Character_GetOOCName(owner), owner, tempReason);
        }

        PlayerRequestsCounter{owner}--;
        SelectedRequest[playerid] = 0;
        mysql_tquery_f(gMySQL, "DELETE FROM `staff_requests` WHERE id = '%d'", requestid);
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `staff_requests` WHERE id = '%d'", requestid);
    return 1;
}

Dialog:Dialog_PlayerRequestView(playerid, response, listitem, inputtext[])
{
    if(response)
    {
        mysql_tquery_f(gMySQL, "DELETE FROM `staff_requests` WHERE id = '%d'", SelectedRequest[playerid]);
        SendClientMessage(playerid, COLOR_ORANGE, "Hai annullato la tua richiesta.");
        return 1;
    }
    else 
    {
        StaffRequest_Show(playerid, CurrentRequestPage{playerid}, CurrentRequestType{playerid});
    }
    return 1;
}

stock StaffRequest_Add(playerid, type, const text[])
{
    if(!Character_IsLogged(playerid))
        return 0;
    PlayerRequestsCounter{playerid}++;
    PlayerRequestTime[playerid] = gettime() + 20;
    new temp[255];
    strins(temp, text, 0);
    inline OnInsert()
    {
        new id = cache_insert_id();
        if(type == REQUEST_TYPE_SUPPORTER)
        {
            SendMessageToTesters(0, 0xe9de98ff, "[TESTER-REQ %d]: %s (%d) ha effettuato una richiesta: %s", id, Character_GetOOCName(playerid), playerid, temp);
            SendMessageToTesters(0, 0xE9DE98FF, "Digita /ar %d per accettare la richiesta.", id);
        }
        else
        {
            SendMessageToAdmins(0, 0xEA7500FF, "[ADMIN-REQ %d]: %s (%d) chiede: %s", id, Character_GetOOCName(playerid), playerid, temp);
            SendMessageToAdmins(0, 0xEA7500FF, "Digita /ar %d per accettare la richiesta.", id);
        }
    }
    MySQL_TQueryInline(gMySQL, using inline OnInsert, "INSERT INTO `staff_requests` (owner_id, type, text, time) VALUES('%d', '%d', '%e', '%d')", playerid, type, text, gettime());
    
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

