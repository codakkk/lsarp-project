
#if defined _included_governament
    #endinput
#endif 

#define _included_governament

#define TAXES_MAX_VALUE 40

enum _:TaxType
{
    TAX_TYPE_VAT = 0,
    TAX_TYPE_TOBIN = 1,
    TAX_TYPE_BIZ = 2,
    TAX_TYPE_VEHICLE = 3,
    TAX_TYPE_FUEL = 4,
    TAX_TYPE_SALARY = 5
}

enum GovMoneyReason
{
    MONEY_REASON_NONE = -1, 
    MONEY_REASON_VAT = 0,
    MONEY_REASON_TOBIN = 1,
    MONEY_REASON_BIZ = 2,
    MONEY_REASON_VEHICLE = 3,
    MONEY_REASON_FUEL = 4,
    MONEY_REASON_SALARY = 5,
    MONEY_REASON_FINE = 6,
    MONEY_REASON_WITHDRAW = 7,
    MONEY_REASON_DONATION = 8,
    MONEY_REASON_DEPOSIT = 9,
    MONEY_REASON_LICENSE = 10
};

static ReasonTypeName[GovMoneyReason][32] = 
{
    "V.A.T", "Tobin Tax", "Tassa business", "Tassa veicolo", "Accise benzina", "Tassa stipendio", "Multe", "Ritiro", "Donazioni", "Deposito", "Licenze"
};

static enum _:E_TAX_TYPE_DATA
{
    Type,
    Name[32],
    CurrentValue,
    GovMoneyReason:Reason
};

static Map:Taxes;

#include <YSI_Coding\y_hooks>

hook OnGameModeInit()
{
    Taxes = map_new();
    map_set_ordered(Taxes, true);
    DefineTaxType(TAX_TYPE_VAT, "Value Added Tax", MONEY_REASON_VAT);
    DefineTaxType(TAX_TYPE_TOBIN, "Tobin Tax", MONEY_REASON_TOBIN);
    DefineTaxType(TAX_TYPE_BIZ, "Tassa Business", MONEY_REASON_BIZ);
    DefineTaxType(TAX_TYPE_VEHICLE, "Tassa Veicolo", MONEY_REASON_VEHICLE);
    DefineTaxType(TAX_TYPE_FUEL, "Accise benzina", MONEY_REASON_FUEL);
    DefineTaxType(TAX_TYPE_SALARY, "Tassa stipendio", MONEY_REASON_SALARY);

    mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `taxes` (`type` int(11) NOT NULL, `current_value` int(11) NOT NULL DEFAULT '0', PRIMARY KEY (`type`)) ENGINE=MyISAM DEFAULT CHARSET=latin1");
    mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `gov_money_records` (`id` int(11) AUTO_INCREMENT, \
    `money` int(11) NOT NULL, \
    `type` int(11) NOT NULL, \
    `character_id` int(11) NOT NULL, \
    `reason` VARCHAR(128) NOT NULL, \
    `created_at` int(11) NOT NULL, \
    PRIMARY KEY (`id`)) ENGINE=MyISAM DEFAULT CHARSET=latin1");

    LoadTaxes();
    return 1;
}

static DefineTaxType(tax_type, const taxName[], GovMoneyReason:reason)
{
    new data[E_TAX_TYPE_DATA];
    data[Type] = tax_type;
    set(data[Name], taxName);
    data[CurrentValue] = 0;
    data[Reason] = reason;
    map_add_arr(Taxes, tax_type, data, 32);

    mysql_pquery_f(gMySQL, "INSERT IGNORE INTO `taxes` SET type = '%d', current_value = '0'", tax_type);
}

stock Tax_GetValue(taxType)
{
    if(!map_has_key(Taxes, taxType))
        return 0;
    return map_get(Taxes, taxType, CurrentValue);
}

stock bool:Tax_SetValue(taxType, newValue)
{
    if(!map_has_key(Taxes, taxType))
        return false;
    mysql_pquery_f(gMySQL, "UPDATE `taxes` SET current_value = '%d' WHERE type = '%d'", newValue, taxType);
    if(taxType == TAX_TYPE_VAT)
    {
        RefreshHousesLabel();
    }
    return bool:map_set_cell(Taxes, taxType, CurrentValue, newValue);
}

stock Tax_CalculateByValue(taxType, baseValue)
{
    return baseValue + Tax_CalculateOnlyTax(taxType, baseValue);
}

stock Tax_CalculateOnlyTax(taxType, baseValue)
{
    if(baseValue <= 0)
        return 0;
    return floatround(float(baseValue * Tax_GetValue(taxType))/100.0, floatround_ceil);
}

stock GovMoneyReason:Tax_GetReason(taxType)
{
    if(!map_has_key(Taxes, taxType))
        return MONEY_REASON_NONE;
    return GovMoneyReason:map_get(Taxes, taxType, Reason);
}

stock bool:Gov_CollectTax(taxType, basePrice)
{
    if(basePrice <= 0)
        return false;
    Gov_GiveMoney(Tax_GetReason(taxType), Tax_CalculateOnlyTax(taxType, basePrice));
    // GiveMoney = taxed - basePrice
    return true;
}

// characterid and reason are used by /ritiragov
stock bool:Gov_GiveMoney(GovMoneyReason:reason, amount, characterid=0, const textReason[] = "")
{
    new factionid = Faction_FindOneByType(FACTION_TYPE_GOVERNMENT);
    Faction_GiveMoney(factionid, amount);

    mysql_pquery_f(gMySQL, "INSERT INTO `gov_money_records` (money, type, character_id, reason, created_at) VALUES('%d', '%d', '%d', '%e', '%d')", amount, _:reason, characterid, textReason, gettime());
    return true;
}

stock Gov_GetMoney()
{
    new factionid = Faction_FindOneByType(FACTION_TYPE_GOVERNMENT);
    return Faction_GetMoney(factionid);
}

stock ShowTaxesInfoDialog(playerid)
{
    new String:info = str_new("");

    new response[e_DIALOG_RESPONSE_INFO],
        data[E_TAX_TYPE_DATA];
    
    for_map(i : Taxes)
    {
        iter_get_arr(i, data);
        str_append_format(info, "- %s: %d%%\n", data[Name], data[CurrentValue]);
    }
    str_append_format(info, "\n\n* Le tasse possono variare a discrezione del Governo attuale.");
    await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_MSGBOX, @("Las Venturas - Tasse"), info, @("Chiudi"), @(""));
}

stock Tax_ShowEditDialog(playerid)
{
    new String:info = str_new("");

    pawn_guard(info);

    new response[e_DIALOG_RESPONSE_INFO],
        data[E_TAX_TYPE_DATA];

    yield 1;
    for(;;)
    {
        str_clear(info);
        str_append_format(info, "Nome\tValore Attuale (%%)\n");
        str_append_format(info, "Statistiche\t--\n");
        for_map(i : Taxes)
        {
            iter_get_arr(i, data);
            str_append_format(info, "%s\t%d%%\n", data[Name], data[CurrentValue]);
        }

        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_TABLIST_HEADERS, @("Tasse - Las Venturas"), info, @("Modifica"), @("Chiudi"));
    
        if(!response[E_DIALOG_RESPONSE_Response])
            break;
        
        new listitem = response[E_DIALOG_RESPONSE_Listitem];

        if(listitem == 0)
        {
            Tax_ShowStats(playerid);
            continue;
        }

        listitem--;

        new type = map_key_at(Taxes, listitem);

        if(type < 0 || type >= TaxType)
            continue;
        
        map_get_arr(Taxes, type, data);

        for(;;)
        {
            str_clear(info);
            str_append_format(info, "Inserisci il valore (percentuale) che vuoi impostare alla tassa.\nNome: %s\nValore Attuale: %d%%", data[Name], data[CurrentValue]);
            
            await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_INPUT, @(data[Name]), info, @("Imposta"), @("Indietro"));
    
            if(!response[E_DIALOG_RESPONSE_Response])
                break;

            new value = strval(response[E_DIALOG_RESPONSE_InputText]);

            if(!(0 <= value <= TAXES_MAX_VALUE))
            {
                SendClientMessageStr(playerid, COLOR_ERROR, str_format("Il valore della tassa deve essere compreso tra 0 e %d.", TAXES_MAX_VALUE));
                continue;
            }

            SendClientMessageStr(playerid, -1, str_format("Hai impostato il nuovo valore della tassa \"%s\". Valore precedente: %d%%. Nuovo valore: %d%%", data[Name], data[CurrentValue], value));
            Tax_SetValue(type, value);
            break;
        }
    
    }
    return 1;
}

stock Tax_ShowStats(playerid)
{
    new response[e_DIALOG_RESPONSE_INFO];
    yield 1;
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_LIST, @("Guadagni per periodo"), @("Guadagni ultimi sette giorni\nGuadagni ultimi 14 giorni\nGuadagni ultimi 21 giorni\nGuadagni ultimo mese\nIntero periodo"), @("Seleziona"), @("Indietro"));

        if(!response[E_DIALOG_RESPONSE_Response])
            break;
        
        switch(response[E_DIALOG_RESPONSE_Listitem])
        {
            case 0: ShowSevenDaysStatsDialog(playerid);
        }
    }
}

static ShowSevenDaysStatsDialog(playerid)
{
    await mysql_aquery_s(gMySQL, str_format("SELECT type, SUM(money) as value FROM gov_money_records WHERE 1 GROUP BY (type)"), .parallel=true);
    
    new rows = cache_num_rows();
    if(rows <= 0)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Non ci sono statistiche per questo periodo.");
        return;
    }

    new String:info = str_new("\n\n");
    new type, value;

    for(new i = 0; i < rows; ++i)
    {
        cache_get_value_name_int(i, "type", type);
        cache_get_value_name_int(i, "value", value);
        str_append_format(info, "%s: $%d\n", ReasonTypeName[GovMoneyReason:type], value);
    }
    str_append_format(info, "\n\n");

    new response[e_DIALOG_RESPONSE_INFO];
    await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_MSGBOX, @("Guadagni ultimi 7 giorni"), info, @("Chiudi"), @(""));
}

static LoadTaxes()
{
    await mysql_aquery_s(gMySQL, str_format("SELECT type, current_value FROM taxes"), .parallel=true);
    
    new rows = cache_num_rows(), data[E_TAX_TYPE_DATA];
    for(new i = 0; i < rows; ++i)
    {
        cache_get_value_name_int(i, "type", data[Type]);
        if(!map_has_key(Taxes, data[Type]))
        {
            printf("Trying to load an undefined tax type.");
            continue;
        }
        map_get_arr(Taxes, data[Type], data);
        cache_get_value_name_int(i, "current_value", data[CurrentValue]);
        map_set_arr(Taxes, data[Type], data);
        printf("Loaded tax: %s - Value: %d", data[Name], data[CurrentValue]);
    }
}

flags:gov(CMD_GOVERNMENT);
CMD:gov(playerid, params[])
{
	if(Character_GetRank(playerid) > 3) 
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai il grado adatto per utilizzare questo comando.");

	if(isnull(params) || strlen(params) > 256)return
		SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /gov [testo]");

	foreach(new j : Player)
	{
		if(!Character_IsLogged(j))continue;
		SendFormattedMessage(j, COLOR_BLUE, "[Annuncio dal Governo] %s dice: %s", Character_GetOOCName(playerid), params);
	}

	return 1;
}

flags:infotasse(CMD_ALIVE_USER);
CMD:infotasse(playerid, params[])
{
	ShowTaxesInfoDialog(playerid);	
	return 1;
}

flags:tasse(CMD_GOVERNMENT);
CMD:tasse(playerid, params[])
{
	if(Character_GetRank(playerid) > 2) 
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai il grado adatto per utilizzare questo comando.");
	
	Tax_ShowEditDialog(playerid);
	return 1;
}

flags:ritiragov(CMD_GOVERNMENT);
CMD:ritiragov(playerid, params[])
{
    if(Character_GetRank(playerid) > 1) 
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai il grado adatto per utilizzare questo comando.");
    new amount, reason[128];
    if(sscanf(params, "ds[128]", amount, reason) || amount <= 0 || strlen(reason) >= 128)
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /ritiragov [ammontare] [motivo (max 128 caratteri)]");
    
    if(amount > Gov_GetMoney())
        return SendClientMessage(playerid, COLOR_ERROR, "Non ci sono tutti questi soldi nel conto bancario del governo.");
    
    Gov_GiveMoney(MONEY_REASON_WITHDRAW, -amount, Character_GetID(playerid), reason);
    new rankName[MAX_FACTION_RANK_NAME];
    Faction_GetRankName(Character_GetFaction(playerid), Character_GetRank(playerid), rankName);

    Faction_SendMessageToTypeStr(FACTION_TYPE_GOVERNMENT, str_format("** HQ: [%s] %s ha ritirato $%s dal conto del governo. Motivo: %s", rankName, Character_GetOOCName(playerid), FormatNumber(amount), reason));
    SendMessageToAdmins(1, COLOR_ADMIN, "AdmInfo: %s (%d) ha ritirato $%s dal conto del governo. Motivo: %s", Character_GetOOCName(playerid), playerid, FormatNumber(amount), reason);
    return 1;
}

flags:depositagov(CMD_GOVERNMENT);
CMD:depositagov(playerid, params[])
{
    if(Character_GetRank(playerid) > 1) 
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai il grado adatto per utilizzare questo comando.");
    new amount, reason[128];
    if(sscanf(params, "ds[128]", amount, reason) || amount <= 0 || strlen(reason) >= 128)
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /depositagov [ammontare] [motivo (max 128 caratteri)]");
    
    if(!Character_GiveMoney(playerid, -amount, "gov_deposita"))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai l'ammontare inserito.");
    
    Gov_GiveMoney(MONEY_REASON_DEPOSIT, amount, Character_GetID(playerid), reason);

    new rankName[MAX_FACTION_RANK_NAME];
    Faction_GetRankName(Character_GetFaction(playerid), Character_GetRank(playerid), rankName);
    Faction_SendMessageToTypeStr(FACTION_TYPE_GOVERNMENT, str_format("** HQ: [%s] %s ha depositato $%s nel conto del governo. Motivo: %s", rankName, Character_GetOOCName(playerid), FormatNumber(amount), reason));
    SendMessageToAdmins(1, COLOR_ADMIN, "AdmInfo: %s (%d) ha depositato $%s nel conto del governo. Motivo: %s", Character_GetOOCName(playerid), playerid, FormatNumber(amount), reason);
    return 1;
}

flags:bilanciogov(CMD_GOVERNMENT);
CMD:bilanciogov(playerid, params[])
{
    if(Character_GetRank(playerid) > 1) 
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai il grado adatto per utilizzare questo comando.");
    SendClientMessageStr(playerid, -1, str_format("Bilancio del governo: $%s", FormatNumber(Gov_GetMoney())));
    return 1;
}

flags:sequestracasa(CMD_GOVERNMENT);
CMD:sequestracasa(playerid, params[])
{
    if(Character_GetRank(playerid) > 4) 
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai il grado adatto per utilizzare questo comando.");
    
    new houseid, reason[128];
    if(sscanf(params, "ds[128]", houseid, reason) || strlen(reason) >= 128)
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /sequestracasa [casa id] [motivo (max 128 caratteri)]");
    
    new House:house = House_Find(houseid);

    if(!House_IsValid(house))
        return SendClientMessage(playerid, COLOR_ERROR, "La casa inserita non è valida.");
    
    return 1;
}