
#include <YSI_Coding\y_hooks>

static enum _:E_GPS_LOCATION
{
    ID,
    Name[32],
    Float:X,
    Float:Y,
    Float:Z
};

static List:Locations;
static CurrentLocationCheckpoint[MAX_PLAYERS] = {-1, ...};

hook OnGameModeInit()
{
    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS `gps_locations` ( \
                    `id` int(11) NOT NULL AUTO_INCREMENT, \
                    `name` VARCHAR(32) NOT NULL, \
                    `pos_x` FLOAT NOT NULL, \
                    `pos_y` FLOAT NOT NULL, \
                    `pos_z` FLOAT NOT NULL, \
                    PRIMARY KEY (`id`) \
                    ) ENGINE=InnoDB DEFAULT CHARSET=latin1");
    Locations = list_new();

    GPS_LoadAll();
    return 1;
}

hook OnCharacterClearData(playerid)
{
    HideGPS(playerid);
    return 1;
}

hook OnPlayerEnterDynamicCP(playerid, checkpointid)
{
    if(CurrentLocationCheckpoint[playerid] == checkpointid && IsValidDynamicCP(CurrentLocationCheckpoint[playerid]))
    {
        SendClientMessage(playerid, COLOR_ORANGE, "Hai raggiunto il luogo segnato.");
        HideGPS(playerid);    
    }
    return 1;
}

flags:agpscmds(CMD_ADMIN);
CMD:agpscmds(playerid, params[])
{
    SendClientMessage(playerid, -1, "[GPS/LUOGHI] /gpscreate /gpsedit");
    return 1;
}

flags:gpscreate(CMD_ADMIN);
CMD:gpscreate(playerid, params[])
{
    if(isnull(params) || strlen(params) < 3)
        return SendClientMessage(playerid, COLOR_ORANGE, "USO: /gpscreate <nome (min 3 caratteri)>");
    
    new data[E_GPS_LOCATION];
    GetPlayerPos(playerid, data[X], data[Y], data[Z]);
    
    await mysql_aquery_s(gMySQL, str_format("INSERT INTO `gps_locations` (name, pos_x, pos_y, pos_z) VALUES('%e', '%f', '%f', '%f')", params, data[X], data[Y], data[Z]));
    
    data[ID] = cache_insert_id();
    format(data[Name], 32, params);
    list_add_arr(Locations, data);

    SendFormattedMessage(playerid, COLOR_ORANGE, "Location GPS id %d creata con successo alla posizione corrente.", data[ID]);
    SendMessageToAdmins(0, COLOR_ADMIN, "AdmCmd: %s ha creato un nuovo luogo chiamato %s.", Account_GetName(playerid), params);
    return 1;
}

flags:gpsedit(CMD_ADMIN);
CMD:gpsedit(playerid, params[])
{
    if(list_size(Locations) <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non ci sono luoghi definiti.");
    yield 1;
    new 
        String:content = str_new(""), 
        response[e_DIALOG_RESPONSE_INFO],
        data[E_GPS_LOCATION];
    pawn_guard(content);
    for(;;)
    {
        str_clear(content);

        for_list(i : Locations)
        {
            iter_get_arr(i, data);
            str_append_format(content, "%s\n", data[Name]);
        }

        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_LIST, @("Edit GPS"), content, @("Seleziona"), @("Indietro"));

        if(!response[E_DIALOG_RESPONSE_Response]) break;

        new listitem = response[E_DIALOG_RESPONSE_Listitem];
        if(listitem < 0 || listitem >= list_size(Locations))
            continue;
        
        list_get_arr(Locations, listitem, data);
        
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_LIST, str_format("%s", data[Name]), @("Modifica Nome\nCambia Posizione (Posizione Corrente)\nCancella"), @("Seleziona"), @("Indietro"));

        if(!response[E_DIALOG_RESPONSE_Response]) continue;

        switch(response[E_DIALOG_RESPONSE_Listitem])
        {
            case 0: ShowChangeGPSNameDialog(playerid, data[ID], data);
            case 1:
            {
                GetPlayerPos(playerid, data[X], data[Y], data[Z]);
                mysql_pquery_s(gMySQL, str_format("UPDATE `gps_locations` SET pos_x = '%f', pos_y = '%f', pos_z = '%f' WHERE id = '%d'", data[X], data[Y], data[Z], data[ID]));
                SendFormattedMessage(playerid, COLOR_ORANGE, "Hai modificato la posizione del luogo id %d.", data[ID]);
                SendMessageToAdmins(0, COLOR_ADMIN, "AdmCmd: %s ha modificato le coordinate del luogo %s (ID %d).", Account_GetName(playerid), data[Name], data[ID]);
            }
            case 2: ShowDeleteGPSDialog(playerid, listitem, data);
        }
        list_set_arr(Locations, listitem, data);
        break;
    }

    return 1;
}

static ShowChangeGPSNameDialog(playerid, listitem, data[E_GPS_LOCATION])
{
    #pragma unused listitem
    yield 1;
    new response[e_DIALOG_RESPONSE_INFO];
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_INPUT, str_format("Nome Corrente: %s", data[Name]), @("Inserisci il nuovo nome che vuoi dare alla Location del GPS."), @("Seleziona"), @("Indietro"));

        if(!response[E_DIALOG_RESPONSE_Response])
            break;

        if(isnull(response[E_DIALOG_RESPONSE_InputText]) || strlen(response[E_DIALOG_RESPONSE_InputText]) >= 32)
            continue;

        SendMessageToAdmins(0, COLOR_ADMIN, "AdmCmd: %s ha modificato il nome del luogo %s. Nuovo nome: %s.", Account_GetName(playerid), data[Name], response[E_DIALOG_RESPONSE_InputText]);
        
        format(data[Name], 32, response[E_DIALOG_RESPONSE_InputText]);
        mysql_pquery_s(gMySQL, str_format("UPDATE `gps_locations` SET name = '%e' WHERE id = '%d'", data[Name], data[ID]));
        SendFormattedMessage(playerid, COLOR_ORANGE, "Hai modificato il nome della location del GPS id %d. Nuovo Nome: %s", data[ID], response[E_DIALOG_RESPONSE_InputText]);
        break;
    }
}

static ShowDeleteGPSDialog(playerid, listitem, data[E_GPS_LOCATION])
{
    yield 1;
    new response[e_DIALOG_RESPONSE_INFO];
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_MSGBOX, str_format("%s", data[Name]), @("Sei sicuro di voler cancellare definitivamente questo luogo?"), @("Cancella"), @("Indietro"));
        
        if(!response[E_DIALOG_RESPONSE_Response]) break;
        
        SendMessageToAdmins(0, COLOR_ADMIN, "AdmCmd: %s ha cancellato il luogo %s (ID %d).", Account_GetName(playerid), data[Name], data[ID]);

        SendFormattedMessage(playerid, COLOR_ORANGE, "Il luogo id %d è stato cancellato con successo.", data[ID]);
        mysql_pquery_s(gMySQL, str_format("DELETE FROM `gps_locations` WHERE id = '%d'", data[ID]));
        list_remove(Locations, listitem);
        break;
    }
}

CMD:gps(playerid, params[])
{
    if(YHash(params) == _I<off>)
    {
        HideGPS(playerid);
        SendClientMessage(playerid, -1, "Hai rimosso il checkpoint del GPS.");
    }
    else
    {
        ShowGPS(playerid);
    }
    return 1;
}
alias:gps("luoghi");

static ShowGPS(playerid)
{
    if(list_size(Locations) <= 0)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Non ci sono luoghi definiti.");
        return;
    }
    new 
        String:content = str_new(""), 
        response[e_DIALOG_RESPONSE_INFO],
        data[E_GPS_LOCATION];

    pawn_guard(content);
    yield 1;
    for(;;)
    {
        str_clear(content);

        for_list(i : Locations)
        {
            iter_get_arr(i, data);
            str_append_format(content, "%s\n", data[Name]);
        }

        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_LIST, @("GPS"), content, @("Seleziona"), @("Indietro"));

        if(!response[E_DIALOG_RESPONSE_Response]) break;

        new listitem = response[E_DIALOG_RESPONSE_Listitem];
        if(listitem < 0 || listitem >= list_size(Locations))
            continue;
        
        list_get_arr(Locations, listitem, data);
        
        HideGPS(playerid);

        CurrentLocationCheckpoint[playerid] = CreateDynamicCP(data[X], data[Y], data[Z], 2.5, 0, 0, playerid, 99999.0);
        SendFormattedMessage(playerid, COLOR_ORANGE, "Il luogo (%s) è segnato sulla mappa da un checkpoint rosso. Usa \"/luoghi off\" per disabilitarlo.", data[Name]);
        break;
    }
}

static HideGPS(playerid)
{
    if(IsValidDynamicCP(CurrentLocationCheckpoint[playerid]))
        DestroyDynamicCP(CurrentLocationCheckpoint[playerid]);
    CurrentLocationCheckpoint[playerid] = -1;
}

static GPS_LoadAll()
{
    await mysql_aquery_s(gMySQL, @("SELECT * FROM `gps_locations`"));
    
    new rows = cache_num_rows();
    new data[E_GPS_LOCATION];

    for(new i = 0; i < rows; ++i)
    {
        cache_get_value_name_int(i, "id", data[ID]);
        cache_get_value_name(i, "name", data[Name]);
        cache_get_value_name_float(i, "pos_x", data[X]);
        cache_get_value_name_float(i, "pos_y", data[Y]);
        cache_get_value_name_float(i, "pos_z", data[Z]);

        list_add_arr(Locations, data);
    }
}