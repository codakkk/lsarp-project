#include <buildings\core>
#include <YSI_Coding\y_hooks>

CMD:mdc(playerid, params[])
{
    new vehicleid = GetPlayerVehicleID(playerid);
    if(vehicleid <= 0 || !Vehicle_IsValid(vehicleid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo.");
    
    if(Faction_GetType(Vehicle_GetFaction(vehicleid)) != FACTION_TYPE_POLICE)
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo munito del Mobile Data Computer.");

    Character_ShowMDC(playerid);
    return 1;
}

static Character_ShowMDC(playerid)
{
    if(Faction_GetType(Character_GetFaction(playerid)) != FACTION_TYPE_POLICE)
        return 0;
    
    Character_AMe(playerid, "accede al Mobile Data Computer.");

    new response[e_DIALOG_RESPONSE_INFO], String:info = str_new("");
    str_append_format(info, "Ricerca Cittadino\n");
    str_append_format(info, "Ricerca Numero Cellulare\n");
    str_append_format(info, "Ricerca Targa\n");
    //str_append_format(info, "Lista Porto Armi\n");
    str_append_format(info, "Proprietario Biz\n");
    str_append_format(info, "Proprietario Casa\n");
    pawn_guard(info);
    
    yield 1;
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_LIST, @("Mobile Data Computer"), info, @("Procedi"), @("Chiudi"));

        if(!response[E_DIALOG_RESPONSE_Response])
            break;
        
        switch(response[E_DIALOG_RESPONSE_Listitem])
        {
            case 0: HandleCivilianSearchDialog(playerid);
            case 1: HandleSearchPhoneDialog(playerid);
            case 2: HandleSearchPlateDialog(playerid);
            case 3: HandleBizOwnerDialog(playerid);
            case 4: HandleHouseOwnerDialog(playerid);
        }
    }
    return 1;
}

static HandleCivilianSearchDialog(playerid)
{
    new 
        String:error = str_new(""),
        String:info = str_new(""),
        targetid = 0,
        name[MAX_PLAYER_NAME],
        response[e_DIALOG_RESPONSE_INFO]
        ;
    
    pawn_guard(info);
    pawn_guard(error);
    
    yield 1;
    for(;;)
    {
        if(targetid <= 0)
        {
            str_clear(info);
            str_append_format(info, "{FFFFFF}Inserisci il nome del civile da ricercare all'interno del database.\nEsempio: Mario_Rossi");
            await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_INPUT, @("Ricerca Cittadino"), error + @("\n") + info, @("Ricerca"), @("Indietro"));

            str_clear(error);

            if(!response[E_DIALOG_RESPONSE_Response])
                break;

            if(isnull(response[E_DIALOG_RESPONSE_InputText]) || strlen(response[E_DIALOG_RESPONSE_InputText]) <= 0)
            {
                str_append(error, @("{FF0000}Il nome inserito non è valido.{FFFFFF}"));
                continue;
            }

            await mysql_aquery_s(gMySQL, str_format("SELECT id, name FROM `characters` WHERE LOWER(name) = LOWER('%e')", response[E_DIALOG_RESPONSE_InputText]), .parallel=true);

            if(cache_num_rows() <= 0)
            {
                str_append(error, @("{FF0000}Il nome inserito non è valido o non è presente nel database.{FFFFFF}"));
                continue;
            }

            cache_get_value_name_int(0, "id", targetid);
            cache_get_value_name(0, "name", name);
        }

        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_LIST, str_format("%s - Ricerca Cittadino", name), @("Dati del cittadino\nVeicoli del cittadino\nBiz del cittadino\nCase del cittadino\nFedina penale"), @("Seleziona"), @("Indietro"));

        if(!response[E_DIALOG_RESPONSE_Response]) break;

        switch(response[E_DIALOG_RESPONSE_Listitem])
        {
            case 0: HandleCivilianDataDialog(playerid, targetid, name);
            case 1: HandleCivilianVehiclesDialog(playerid, targetid, name);
            case 2: HandleCivilianBizDialog(playerid, targetid, name);
            case 3: HandleCivilianHouseDialog(playerid, targetid, name);
            case 4: MDC_ShowCriminalRecords(playerid, name);
        }
    }
}

static HandleCivilianDataDialog(playerid, targetid, const targetName[])
{
    new String:info = str_new("");
    pawn_guard(info);
    
    new name[MAX_PLAYER_NAME], surname[MAX_PLAYER_NAME], House:rentHouse = INVALID_HOUSE, rentHouseId = 0;

    yield 1;
    await mysql_aquery_s(gMySQL, str_format("SELECT house_rent FROM characters WHERE id = '%d' and banned = 0", targetid), .parallel=true);

    if(cache_num_rows() <= 0)
        return;
    
    cache_get_value_index_int(0, 0, rentHouseId);
    rentHouse = House_Find(rentHouseId);


    sscanf(targetName, "p<_>s[24]s[24]", name, surname);
    
    str_clear(info);
    str_append_format(info, "{FFFFFF}===== SCHEDA DEL CITTADINO =====\n");
    str_append_format(info, "- Nome: %s\n", name);
    str_append_format(info, "- Cognome: %s\n", surname);
    
    SendClientMessage(playerid, -1, "-- Scheda del cittadino --");
    SendClientMessageStr(playerid, -1, str_format("- Nome: %s", name));
    SendClientMessageStr(playerid, -1, str_format("- Cognome: %s", surname));

    if(House_IsValid(rentHouse))
    {
        str_append_format(info, "Affitto: %s\n", House_GetAddress(rentHouse));
        SendClientMessageStr(playerid, -1, str_format("Affitto: %s", House_GetAddress(rentHouse)));
    }
    new response[e_DIALOG_RESPONSE_INFO];
    await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_MSGBOX, @("Scheda"), info, @("Indietro"), @(""));
}

static HandleCivilianVehiclesDialog(playerid, targetid, const targetName[])
{
    yield 1;
    await mysql_aquery_s(gMySQL, str_format("SELECT model, plate FROM vehicles WHERE owner_id = '%d'", targetid), .parallel=true);

    new num = cache_num_rows(), 
        String:string = str_new("-- Veicoli -- \n\n");
    
    SendClientMessageStr(playerid, -1, @("-- Veicoli --"));

    if(num <= 0)
    {
        str_append_format(string, "Il civile non possiede veicoli di proprietà.\n");
        SendClientMessage(playerid, -1, "Il civile non possiede veicoli di proprietà.");
    }
    else
    {
        new modelid, plate[9];
        for(new i = 0; i < num; ++i)
        {
            cache_get_value_name_int(i, "model", modelid);
            cache_get_value_name(i, "plate", plate);

            str_append_format(string, "- %s [%s]\n", Vehicle_GetNameFromModel(modelid), plate);

            SendClientMessageStr(playerid, -1, str_format("- %s [%s]", Vehicle_GetNameFromModel(modelid), plate));
        }
        new response[e_DIALOG_RESPONSE_INFO];
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_MSGBOX, str_format("%s - Veicoli", targetName), string, @("Chiudi"), @(""));
    }
}

static HandleCivilianBizDialog(playerid, targetid, const targetName[])
{
    new String:info = str_new("");
    
    SendClientMessageStr(playerid, -1, str_format("-- Biz di %s", targetName));

    new count = 0;
    str_append_format(info, "\n==== Edifici (Biz) ====\n\n");
    for_building(building)
    {
        if(Building_GetOwnerID(building) != targetid)
            continue;
        
        str_append_format(info, "- {33AA33}%S{FFFFFF}\n", Building_GetAddressStr(building));
        SendClientMessageStr(playerid, -1, str_format("- {33AA33}%S{FFFFFF}", Building_GetAddressStr(building)));
        count++;
    }

    if(count <= 0)
    {
        str_append_format(info, "Il civile non possiede edifici di proprietà.\n");
        SendClientMessage(playerid, -1, "Il civile non possiede edifici di proprietà.");
    }
    new response[e_DIALOG_RESPONSE_INFO];
    await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_MSGBOX, str_format("%s - Case", targetName), info, @("Indietro"), @(""));
}

static HandleCivilianHouseDialog(playerid, targetid, const targetName[])
{
    new String:info = str_new("");
    
    SendClientMessageStr(playerid, -1, str_format("Case di %s", targetName));

    new count = 0;
    for_house(house)
    {
        if(House_GetOwnerID(house) == targetid)
        {
            str_append_format(info, "- {33AA33}%s{FFFFFF}\n", House_GetAddress(house));
            SendClientMessageStr(playerid, -1, str_format("- {33AA33}%s{FFFFFF}\n", House_GetAddress(house)));
            count++;
        }
    }
    
    if(count <= 0)
        str_append_format(info, "Il civile non possiede case di proprietà.");
    
    new response[e_DIALOG_RESPONSE_INFO];
    await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_MSGBOX, str_format("%s - Case", targetName), info, @("Indietro"), @(""));
}

static HandleSearchPhoneDialog(playerid)
{
    new 
        String:info = str_new("{FFFFFF}Inserisci il numero di cellulare che vuoi ricercare."), String:error = str_new(""),
        response[e_DIALOG_RESPONSE_INFO], number = 0;
    
    pawn_guard(info);
    pawn_guard(error);
    
    yield 1;
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_INPUT, @("Ricerca Numero Cellulare"), error + @("\n") + info, @("Cerca"), @("Indietro"));
        
        if(!response[E_DIALOG_RESPONSE_Response])
            break;

        str_clear(error);

        number = strval(response[E_DIALOG_RESPONSE_InputText]);
        if(number <= 0)
        {
            str_append(error, @("{FF0000}Il numero inserito non è del formato giusto.{FFFFFF}"));
            continue;
        }
        
        new String:query = str_format("SELECT phone_data.character_id, phone_data.created_at, characters.name FROM phone_data INNER JOIN `characters` ON phone_data.character_id = characters.id AND phone_data.number = '%d';", number);
        await mysql_aquery_s(gMySQL, query, .parallel=true);

        if(cache_num_rows() <= 0)
        {
            SendClientMessage(playerid, COLOR_ERROR, "Il numero inserito non è stato trovato nel database.");
            str_append(error, @("{FF0000}Il numero inserito non è stato trovato nel database.{FFFFFF}"));
            continue;
        }

        new ownerid, createdAt, name[MAX_PLAYER_NAME];
        cache_get_value_name_int(0, "character_id", ownerid);
        cache_get_value_name_int(0, "created_at", createdAt);
        cache_get_value_name(0, "name", name);


        new year, month, day, hour, minute, second;
        TimestampToDate(createdAt, year, month, day, hour, minute, second);

        new String:res = str_new("");

        SendClientMessageStr(playerid, -1, str_format("-- Numero di cellulare - %d", number));

        if(ownerid > 0)
        {
            str_append_format(res, "Intestatario: %s\n", name);
            SendClientMessageStr(playerid, -1, str_format("Intestatario: %s", name));
        }
        str_append_format(res, "Acquistato il %02d/%02d/%02d alle ore %02d:%02d", day, month, year, hour, minute);

        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_MSGBOX, str_format("Numero di cellulare - %d", number), res, @("Indietro"), @(""));

        break;
    }
}

static HandleSearchPlateDialog(playerid)
{
    new 
        String:info = str_new("{FFFFFF}Inserisci la targa del veicolo da ricercare all'interno del database."), String:error = str_new(""),
        response[e_DIALOG_RESPONSE_INFO];
    
    pawn_guard(info);
    pawn_guard(error);
    
    yield 1;
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_INPUT, @("Ricerca Targa"), error + @("\n") + info, @("Cerca"), @("Indietro"));
        
        if(!response[E_DIALOG_RESPONSE_Response])
            break;

        str_clear(error);

        if(isnull(response[E_DIALOG_RESPONSE_InputText]) || strlen(response[E_DIALOG_RESPONSE_InputText]) <= 0 || strlen(response[E_DIALOG_RESPONSE_InputText]) > 9)
        {
            str_append(error, @("{FF0000}Il numero di targa inserito non è del formato giusto.{FFFFFF}"));
            continue;
        }
        
        new String:query = str_format("SELECT vehicles.owner_id, vehicles.model, characters.name FROM vehicles INNER JOIN `characters` ON vehicles.owner_id = characters.id AND vehicles.plate = '%e';", response[E_DIALOG_RESPONSE_InputText]);
        await mysql_aquery_s(gMySQL, query, .parallel=true);

        if(cache_num_rows() <= 0)
        {
            SendClientMessage(playerid, COLOR_ERROR, "La targa non è stata trovata nel database.");
            str_append(error, @("{FF0000}La targa non è stata trovata nel database.{FFFFFF}"));
            continue;
        }

        new ownerid, modelid, name[MAX_PLAYER_NAME];
        cache_get_value_name_int(0, "owner_id", ownerid);
        cache_get_value_name_int(0, "model", modelid);
        cache_get_value_name(0, "name", name);

        new String:res = str_new("");

        str_append_format(res, "Modello: %s\n", Vehicle_GetNameFromModel(modelid));
        SendClientMessageStr(playerid, -1, str_format("Modello: %s", Vehicle_GetNameFromModel(modelid)));
        if(ownerid > 0)
        {
            str_append_format(res, "Intestatario: %s\n", name);
            SendClientMessageStr(playerid, -1, str_format("Intestatario: %s", name));
        }
        //str_append_format(res, "Acquistato il %02d/%02d/%02d alle ore %02d:%02d", day, month, year, hour, minute);

        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_MSGBOX, str_format("Targa - %s", response[E_DIALOG_RESPONSE_InputText]), res, @("Indietro"), @(""));
        break;
    }
}

/*static HandleWeaponLicenseListDialog(playerid)
{
    #pragma unused playerid
}*/

static HandleBizOwnerDialog(playerid)
{
    new 
        String:info = str_new("{FFFFFF}Inserisci l'id del biz di cui vuoi le informazioni."), String:error = str_new(""),
        response[e_DIALOG_RESPONSE_INFO];
    
    pawn_guard(info);
    pawn_guard(error);
    
    yield 1;
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_INPUT, @("Ricerca Proprietario Biz"), error + @("\n") + info, @("Cerca"), @("Indietro"));
        
        if(!response[E_DIALOG_RESPONSE_Response])
            break;

        str_clear(error);

        new foundid = strval(response[E_DIALOG_RESPONSE_InputText]);
        new Building:found = Building_Find(foundid);
        if(!Building_IsValid(found))
        {
            str_append(error, @("{FF0000}L'edificio inserito non esiste.{FFFFFF}"));
            continue;
        }
        
        new String:res = str_new("");
        pawn_guard(res);

        new name[MAX_BUILDING_NAME];
        Building_GetName(found, name);
        str_append_format(res, "Nome: %s\n", name);
        str_append_format(res, "Indirizzo: %S\n", Building_GetAddressStr(found));
        if(Building_GetOwnerID(found) > 0)
        {
            await mysql_aquery_s(gMySQL, str_format("SELECT name FROM characters WHERE id = '%d'", Building_GetOwnerID(found)), .parallel=true);
            if(cache_num_rows() > 0)
            {
                cache_get_value_name(0, "name", name);
                str_append_format(res, "Proprietario: %s\n", name);
            }
        }
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_MSGBOX, str_format("Biz - %d", foundid), res, @("Indietro"), @(""));
        break;
    }
}
static HandleHouseOwnerDialog(playerid)
{
    new 
        String:info = str_new("{FFFFFF}Inserisci l'id della casa di cui vuoi le informazioni."), String:error = str_new(""),
        response[e_DIALOG_RESPONSE_INFO];
    
    pawn_guard(info);
    pawn_guard(error);
    
    yield 1;
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_INPUT, @("Ricerca Proprietario Casa"), error + @("\n") + info, @("Cerca"), @("Indietro"));
        
        if(!response[E_DIALOG_RESPONSE_Response])
            break;

        str_clear(error);

        new foundid = strval(response[E_DIALOG_RESPONSE_InputText]);
        new House:found = House_Find(foundid);
        if(!House_IsValid(found))
        {
            str_append(error, @("{FF0000}La casa inserita non esiste.{FFFFFF}"));
            continue;
        }
        
        new String:res = str_new("");
        pawn_guard(res);

        str_append_format(res, "Indirizzo: %S\n", House_GetAddressStr(found));
        if(House_GetOwnerID(found) > 0)
        {
            await mysql_aquery_s(gMySQL, str_format("SELECT name FROM characters WHERE id = '%d'", House_GetOwnerID(found)), .parallel=true);
            if(cache_num_rows() > 0)
            {
                new name[MAX_PLAYER_NAME];
                cache_get_value_name(0, "name", name);
                str_append_format(res, "Proprietario: %s\n", name);
            }
        }
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_MSGBOX, str_format("Casa - %d", foundid), res, @("Indietro"), @(""));
        break;
    }
}

stock MDC_ShowCriminalRecordDialogAdd(playerid, targetid)
{
	new response[e_DIALOG_RESPONSE_INFO];
	
	yield 1;
	for(;;)
	{
		await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_INPUT, @("Aggiungi fedina penale"), @("Scrivi un rapporto descrivendo la vicenda accaduta che ha portato all'arresto del criminale."), @("Conferma"), @(""));
		
		if(!response[E_DIALOG_RESPONSE_Response] || isnull(response[E_DIALOG_RESPONSE_InputText]))
			continue;
		
		new String:query = str_format("INSERT INTO `police_arrest_records` (officer_id, target_id, text, created_at) VALUES('%d', '%d', '%e', '%d')", Character_GetID(playerid), Character_GetID(targetid), response[E_DIALOG_RESPONSE_InputText], gettime());
		await mysql_aquery_s(gMySQL, query, .parallel=true);

		new arrestRecordId = cache_insert_id();

		new day, month, year, h, m, s;
		TimestampToDate(gettime(), year, month, day, h, m, s);
		
		yield 1;
		for(;;)
		{
			await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_INPUT, @("Aggiungi reati fedina penale"), str_format("Scrivi i reati commessi da %s in data %d/%d/%d.", Character_GetOOCName(targetid), day, month, year), @("Aggiungi"), @("Invia"));

			if(!response[E_DIALOG_RESPONSE_Response])
			{
				Faction_SendMessageStr(Character_GetFaction(playerid), str_format("** HQ: %s ha aggiornato la fedina penale di %s.", Character_GetOOCName(playerid), Character_GetOOCName(targetid)));
				break;
			}

			if(!isnull(response[E_DIALOG_RESPONSE_InputText]) && strlen(response[E_DIALOG_RESPONSE_InputText]) <= 200)
			{
				mysql_pquery_f(gMySQL, "INSERT INTO `police_crime_records` (arrest_record_id, text) VALUES('%d', '%e')", arrestRecordId, response[E_DIALOG_RESPONSE_InputText]);
			}
		}
		break;
	}

	return 1;
}

flags:fedina(CMD_POLICE);
CMD:fedina(playerid, params[])
{
    new name[MAX_PLAYER_NAME];
    if(sscanf(params, "s[24]", name))
    {
        SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /fedina [nome civile]");
        SendClientMessage(playerid, COLOR_COMMAND_ERROR, "ATTENZIONE: è necessario inserire il nome completo di _.");
        return 1;
    }
    MDC_ShowCriminalRecords(playerid, name);
    return 1;
}   

stock MDC_ShowCriminalRecords(playerid, const name[])
{
    new 
        String:query = str_new(""),
        String:info = str_new(""),
        List:Records = list_new(),
        response[e_DIALOG_RESPONSE_INFO],
        officerName[MAX_PLAYER_NAME],
        civilianName[MAX_PLAYER_NAME];
    
    pawn_guard(query);
    pawn_guard(info);

        
    yield 1;
    for(;;)
    {
        str_clear(query);
        str_append_format(query, "SELECT police_arrest_records.id, police_arrest_records.created_at, characters.name as civilian_name FROM police_arrest_records INNER JOIN `characters` ON police_arrest_records.target_id = characters.id AND LOWER(characters.name) = LOWER('%e')", name);
        await mysql_aquery_s(gMySQL, query, .parallel=true);
        
        new rows = cache_num_rows();
        if(rows <= 0)
        {
            SendClientMessageStr(playerid, COLOR_ERROR, str_format("Non sono stati trovati dati riguardanti il civile inserito (%s).", name));
            break;
        }

        list_clear(Records);
        //list_resize(Records, rows);
        // Name is in each row
        cache_get_value_name(0, "civilian_name", civilianName);

        new time, recordId, year, month, day, h, m, s;
        str_clear(info);
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", recordId);
            cache_get_value_name_int(i, "created_at", time);
            TimestampToDate(time, year, month, day, h, m, s);
            str_append_format(info, "#%d - %d/%d/%d\n", i, day, month, year);

            list_add(Records, recordId);
        }

        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_LIST, str_format("Fedina penale di %s", civilianName), info, @("Visualizza"), @("Chiudi"));

        if(!response[E_DIALOG_RESPONSE_Response])
            break;
        
        new text[201];
        recordId = list_get(Records, response[E_DIALOG_RESPONSE_Listitem]);

        str_clear(query);
        str_append_format(query, "SELECT police_arrest_records.id, police_arrest_records.created_at, police_arrest_records.text, characters.name as officer_name FROM police_arrest_records INNER JOIN `characters` ON police_arrest_records.id = '%d' AND police_arrest_records.officer_id = characters.id", recordId);
        await mysql_aquery_s(gMySQL, query, .parallel = true);

        cache_get_value_name_int(0, "created_at", time);
        cache_get_value_name(0, "officer_name", officerName);
        cache_get_value_name(0, "text", text);

        TimestampToDate(time, year, month, day, h, m, s);

        str_clear(info);
        str_append_format(info, "FEDINA PENALE #%d\n\n", recordId);
        str_append_format(info, "Nome e Cognome: %s\n\n", civilianName);
        str_append_format(info, "Agente: %s\n\n", officerName);
        str_append_format(info, "Data: %d/%d/%d\n\n", day, month, year);
        str_append_format(info, "Rapporto: %s\n\n", text);
        str_append_format(info, "Reati: \n\n");

        str_clear(query);
        str_append_format(query, "SELECT text FROM `police_crime_records` WHERE arrest_record_id = '%d'", recordId);
        await mysql_aquery_s(gMySQL, query, .parallel = true);

        rows = cache_num_rows();
        if(rows <= 0)
            str_append_format(info, "- Non ci sono reati segnalati.");
        else
        {
            for(new i = 0; i < rows; ++i)
            {
                cache_get_value_name(i, "text", text);
                str_append_format(info, "- %s\n", text);
            }
        }

        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_MSGBOX, @("Visualizza crimine"), info, @("Indietro"), @(""));
    }
    list_delete(Records);
}