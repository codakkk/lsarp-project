#include <building_system\core>
#include <YSI_Coding\y_hooks>

static
    TargetID[MAX_PLAYERS];

hook OnCharacterClearData(playerid)
{
    ClearMDCData(playerid);
    return 1;
}

CMD:mdc(playerid, params[])
{
    new vehicleid = GetPlayerVehicleID(playerid);
    if(vehicleid <= 0 || !Vehicle_IsValid(vehicleid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo.");
    
    if(Faction_GetType(Vehicle_GetFaction(vehicleid)) != FACTION_TYPE_POLICE)
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo munito del Mobile Data Computer.");

    if(Character_ShowMDC(playerid, true))
    {
        Character_AMe(playerid, "accede al Mobile Data Computer.");
    }
    return 1;
}

static Character_ShowMDC(playerid, bool:clearData = false)
{
    if(Faction_GetType(Character_GetFaction(playerid)) != FACTION_TYPE_POLICE)
        return 0;
    if(clearData)
        ClearMDCData(playerid);
    //\nRicerca\tTarga\nRicerca\tNumero\nAPB\tInserisci\nAPB\tLista\nPorto d'armi\tGestione\nProprietario\tBiz\nProprietario\tCasa
    Dialog_Show(playerid, Dialog_MDC, DIALOG_STYLE_TABLIST_HEADERS, "Mobile Data Computer", "Azione\tTipo\nRicerca\tCittadino", "Procedi", "Annulla");
    return 1;
}

Dialog:Dialog_MDC(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    
    switch(listitem)
    {
        case 0: return Dialog_Show(playerid, Dialog_MDCSearch, DIALOG_STYLE_INPUT, "Ricerca cittadino", "Inserisci i dati del cittadino che vuoi cercare.\nFormati validi:\n- Nome_Cognome\n- ID Giocatore", "Ricerca", "Chiudi");
    }
    return 1;
}

Dialog:Dialog_MDCSearch(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Character_ShowMDC(playerid);
    
    static const rowsNeeded[] = "id, name";

    inline OnSearch()
    {
        if(cache_num_rows() <= 0)
            return Dialog_Show(playerid, Dialog_MDCSearch, DIALOG_STYLE_INPUT, "Ricerca cittadino", "{FF0000}Non è stato trovato nessun cittadino.{FFFFFF}\nInserisci i dati del cittadino che vuoi cercare.\nFormati validi:\n- Nome_Cognome\n- ID Giocatore", "Ricerca", "Chiudi");
        new name[MAX_PLAYER_NAME];
        cache_get_value_index_int(0, 0, TargetID[playerid]);
        cache_get_value_index(0, 1, name);

        Dialog_Show(playerid, Dialog_MDCSearchCharacter, DIALOG_STYLE_LIST, name, "Dati del cittadino\nVeicoli proprietari", "Continua", "Chiudi");
    }
    if(IsNumeric(inputtext))
    {
        new val = strval(inputtext);
        
        if(!Character_IsLogged(val))
            return Dialog_Show(playerid, Dialog_MDCSearch, DIALOG_STYLE_INPUT, "Ricerca cittadino", "{FF0000}L'ID inserito non è valido.{FFFFFF}\nInserisci i dati del cittadino che vuoi cercare.\nFormato: Nome_Cognome", "Ricerca", "Chiudi");

        MySQL_TQueryInline(gMySQL, using inline OnSearch, "SELECT %e FROM characters WHERE id = '%d'", rowsNeeded, Character_GetID(val));
    }
    else
    {
        MySQL_TQueryInline(gMySQL, using inline OnSearch, "SELECT %e FROM characters WHERE LOWER(name) = LOWER('%e')", rowsNeeded, inputtext);
    }

    return 1;
}

Dialog:Dialog_MDCSearchCharacter(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Character_ShowMDC(playerid, true);
    
    if(TargetID[playerid] == 0)
        return 0;
    
    if(listitem == 0)
    {
        inline OnSearch()
        {
            if(cache_num_rows() <= 0)
                return SendClientMessage(playerid, COLOR_ERROR, "Il cittadino non è più presente nel database.");
            
            new id, address[64], tempName[MAX_PLAYER_NAME], name[MAX_PLAYER_NAME], surname[MAX_PLAYER_NAME], hid = 0, bid = 0;

            cache_get_value_index_int(0, 0, id);
            cache_get_value_index(0, 1, tempName);
            cache_get_value_index_int(0, 2, hid);
            cache_get_value_index_int(0, 3, bid);

            sscanf(tempName, "p<_>s[24]s[24]", name, surname);

            new String:end = str_format("{FFFFFF}==== SCHEDA DEL CITTADINO ====\n- Nome: %s\n- Cognome: %s\n\n==== Case ====\n\n", name, surname);

            if(House_IsValid(hid))
            {
                House_GetAddress(hid, address);
                end += str_format("- %d (%s)\n", hid, address);
            }

            for_map(i : GetHousesMap())
            {
                new houseid = iter_get_key(i);
                if(House_GetOwnerID(houseid) == id && hid != houseid)
                {
                    House_GetAddress(houseid, address);
                    end += str_format("{33AA33}%s{FFFFFF}\n", address);
                }
            }
            
            end += @("\n==== Edifici (Biz) ====\n\n");
            for_building(building)
            {
                if(Building_GetOwnerID(building) == id)
                {
                    address = Building_GetAddress(building);
                    end += str_format("{33AA33}%s{FFFFFF}\n", address);
                }
            } 

            end += @("\n");

            Dialog_Show_s(playerid, DialogNull, DIALOG_STYLE_MSGBOX, @("Scheda"), end, "Chiudi", "");
        }
        MySQL_TQueryInline(gMySQL, using inline OnSearch, "SELECT id, name, house_key, building_key FROM characters WHERE id = '%d' and banned = 0", TargetID[playerid]);
    }
    else if(listitem == 1)
    {
        inline OnSearch()
        {
            new rows = cache_num_rows();
            if(rows <= 0)
                return SendClientMessage(playerid, COLOR_ERROR, "Questo cittadino non possiede veicoli.");

            new model, plate[9], String:end = @("Modello\tTarga\tUltima Localiz.\n");

            for(new i = 0; i < rows; ++i)
            {
                cache_get_value_index_int(i, 0, model);
                cache_get_value_index(i, 1, plate);
                end += str_format("%s\t%s\tSconosciuta", Vehicle_GetNameFromModel(model), plate);
            }

            Dialog_Show_s(playerid, DialogNull, DIALOG_STYLE_TABLIST_HEADERS, @("Veicoli Cittadino"), end, "Chiudi", "");
        }
        MySQL_TQueryInline(gMySQL, using inline OnSearch, "SELECT model, plate FROM `vehicles` WHERE owner_id = '%d'", TargetID[playerid]);
    }
    return 1;
}

static ClearMDCData(playerid)
{
    TargetID[playerid] = 0;
}