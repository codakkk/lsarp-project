
new 
    gShowRoomListItemList[MAX_PLAYERS][MAX_SHOWROOMS],
    gAdminSelectedShowRoom[MAX_PLAYERS],
    gShowRoomItemList[MAX_PLAYERS][MAX_VEHICLES_IN_SHOWROOM]
;


stock ShowRoom_ShowInternalVehicles(showroom_id, playerid)
{
    if(AccountInfo[playerid][aAdmin] < 2) // Safeness
        return 0;
    new
        string[1024],
        count = 0;
    
    for(new i = 0; i < MAX_VEHICLES_IN_SHOWROOM; ++i)
    {
        if(!ShowRoomInfo[showroom_id][srModels][i] || !ShowRoomInfo[showroom_id][srPrices][i])
            continue;
        new 
            modelid = ShowRoomInfo[showroom_id][srModels][i],
            price = ShowRoomInfo[showroom_id][srPrices][i]
        ;
        format(string, sizeof(string), "%s%s\t$%d\n", string, GetVehicleNameFromModel(modelid), price);
        gShowRoomItemList[playerid][count] = i;
        count++;
    }
    if(count == 0)
        return Dialog_Show(playerid, Dialog_EditShowRoom, DIALOG_STYLE_MSGBOX, "Errore", "Questa concessionaria non ha veicoli!", "Ok", "");
    SetPVarInt(playerid, "ShowRoomEdit_Phase", 0);
    return Dialog_Show(playerid, Dialog_ShowRoomEditVeh, DIALOG_STYLE_TABLIST_HEADERS, "ShowRoom Edit", "Modello\tPrezzo\n%s", "Modifica", "Indietro", string);
}

stock ShowRoom_ShowAllShowroomsList(playerid, params[]="")
{
    if(AccountInfo[playerid][aAdmin] < 2) // Safeness
        return 0;
    if(Iter_Count(ShowRooms) == 0)
        return SendClientMessage(playerid, COLOR_ERROR, "> Non esistono concessionarie. Usa /acreateshowroom per crearne una!");
    new id = -1;
    if(sscanf(params, "d", id))
    {
        new 
            temp[256], 
            locName[32],
            count = 0
            ;
        
        foreach(new i : ShowRooms)
        {
            Get2DZoneName(ShowRoomInfo[i][srX], ShowRoomInfo[i][srY], ShowRoomInfo[i][srZ], locName, sizeof(locName));
            format(temp, sizeof(temp), "%s%d\t%s\t%s\n", temp, i, ShowRoomInfo[i][srName], locName);
            gShowRoomListItemList[playerid][count] = i;
            count++;
        }
        return Dialog_Show(playerid, Dialog_AShowRoomList, DIALOG_STYLE_TABLIST_HEADERS, "ShowRoom Edit", "ID\tName\tLocation\n%s", "Edit", "Close", temp);
    }
    else
    {
        if(!ShowRoomInfo[id][srID])
            return SendClientMessage(playerid, COLOR_ERROR, "> Non esiste una concessionaria con questo ID!");
        gAdminSelectedShowRoom[playerid] = id;
        new string[64];
        format(string, sizeof(string), "%s [%d]", ShowRoomInfo[id][srName], id);
        return Dialog_Show(playerid, Dialog_EditShowRoom, DIALOG_STYLE_LIST, string, "Lista Veicoli\nModifica Nome\nAggiungi Veicolo\nSposta (Posizione Corrente)\nPosizione Veicolo\n{FF0000}Cancella (Definitivamente)", "Avanti", "Chiudi");
    }
}

flags:acreateshowroom(CMD_DEVELOPER);
CMD:acreateshowroom(playerid, params[])
{
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    new id = ShowRoom_Create(x, y, z);
    SendFormattedMessage(playerid, COLOR_GREEN, "> Concessionaria [%d] creata!", id);
    SendClientMessage(playerid, COLOR_GREEN, "> Ricorda di settare tutte le impostazioni giuste!");
    SendClientMessage(playerid, COLOR_GREEN, "> Utilizza /ashowroommenu <id> per cominciare a modificare questa concessionaria.");
    return 1;
}

flags:ashowroommenu(CMD_DEVELOPER);
CMD:ashowroommenu(playerid, params[])
{
    ShowRoom_ShowAllShowroomsList(playerid, params);
    return 1;
}

Dialog:Dialog_AShowRoomList(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
        ShowRoom_ClearEditVars(playerid);
        return 1;
    }
    gAdminSelectedShowRoom[playerid] = gShowRoomListItemList[playerid][listitem];
    new string[64];
    format(string, sizeof(string), "%s [%d]", ShowRoomInfo[gAdminSelectedShowRoom[playerid]][srName], gAdminSelectedShowRoom[playerid]);
    Dialog_Show(playerid, Dialog_EditShowRoom, DIALOG_STYLE_LIST, string, "Lista Veicoli\nModifica Nome\nAggiungi Veicolo\nSposta (Posizione Corrente\nPosizione Veicolo\n{FF0000}Cancella (Definitivamente)", "Avanti", "Chiudi");
    return 1;
}

Dialog:Dialog_EditShowRoom(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
        ShowRoom_ClearEditVars(playerid);
        return 1;
    }
    switch(listitem)
    {
        case 0: // Vehicle list
        {
            return ShowRoom_ShowInternalVehicles(gAdminSelectedShowRoom[playerid], playerid);
        }
        case 1: // Edit Name
        {
           return Dialog_Show(playerid, Dialog_ShowRoomEditName, DIALOG_STYLE_INPUT, "ShowRoom Edit Name", "Inserisci il nome della concessionaria.\nLimite Caratteri: 22.", "Modifica", "Indietro");
        }
        case 2: // Add Vehicle
        {
            return Dialog_Show(playerid, Dialog_ShowRoomAddVeh, DIALOG_STYLE_INPUT, "Aggiungi Veicolo", "Inserisci il modello che vuoi inserire seguito dal prezzo!\nEsempio: 522 50000", "Aggiungi", "Indietro");
        }
        case 3: // Move ShowRoom
        {
            new Float:x, Float:y, Float:z;
            GetPlayerPos(playerid, x, y, z);
            ShowRoom_SetPosition(gAdminSelectedShowRoom[playerid], x, y, z);
            SendFormattedMessage(playerid, COLOR_GREEN, "> La concessionaria (%d) è stata spostata con successo!", gAdminSelectedShowRoom[playerid]);
            return 1;
        }
        case 4:
        {
            if(!IsPlayerInAnyVehicle(playerid))
                return SendClientMessage(playerid, COLOR_ERROR, "Devi essere a bordo di un veicolo!");
            new Float:x, Float:y, Float:z, Float:a;
            GetVehiclePos(GetPlayerVehicleID(playerid), x, y, z);
            GetVehicleZAngle(GetPlayerVehicleID(playerid), a);
            ShowRoom_SetVehicleSpawnPos(gAdminSelectedShowRoom[playerid], x, y, z, a);
            return 1;
        }
        case 5: // Delete (def)
        {
            Dialog_Show(playerid, Dialog_ShowRoomDelete, DIALOG_STYLE_MSGBOX, "ShowRoom Delete", "Sei sicuro di voler rimuovere questa concessionaria\ndefinitivamente?\nInfo: %s [%d]", "Elimina", "Annulla", ShowRoomInfo[gAdminSelectedShowRoom[playerid]][srName], gAdminSelectedShowRoom[playerid]);
            return 1;
        }
    }
    return 1;
}

Dialog:Dialog_ShowRoomDelete(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
        return Dialog_Show(playerid, Dialog_EditShowRoom, DIALOG_STYLE_LIST, "ShowRoom Edit", "Lista Veicoli\nAggiungi Veicolo\nSposta (Posizione Corrente\nCancella (Definitivamente)", "Avanti", "Chiudi");
    }
    if(ShowRoom_Delete(gAdminSelectedShowRoom[playerid]))
    {
        SendFormattedMessage(playerid, COLOR_ERROR, "> Concessionaria \"%s\" [%d] eliminata con successo.", ShowRoomInfo[gAdminSelectedShowRoom[playerid]][srName], gAdminSelectedShowRoom[playerid]);
    }
    else
    {
        SendClientMessage(playerid, COLOR_ERROR, "> C'è stato un errore durante l'eliminazione della Concessionaria.");
    }
    
    return 1;
}


Dialog:Dialog_ShowRoomEditName(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
        return ShowRoom_ShowInternalVehicles(gAdminSelectedShowRoom[playerid], playerid);
    }
    if(strlen(inputtext) > 22)
        return Dialog_Show(playerid, Dialog_EditShowRoom, DIALOG_STYLE_INPUT, "ShowRoom Edit Name", "Il nome inserito è troppo lungo!\nInserisci il nome della concessionaria.\nLimite Caratteri: 22.", "Modifica", "Indietro");
    if(ShowRoom_SetName(gAdminSelectedShowRoom[playerid], inputtext))
    {
        SendClientMessage(playerid, COLOR_GREEN, "> Hai cambiato il nome della concessionaria!");
        SendFormattedMessage(playerid, COLOR_GREEN, "> Nuovo nome: %s", inputtext);
    }
    else
    {
        SendClientMessage(playerid, COLOR_ERROR, "> Il cambio del nome non è andato a buon fine!");
    }
    return Dialog_Show(playerid, Dialog_EditShowRoom, DIALOG_STYLE_LIST, "ShowRoom Edit", "Lista Veicoli\nAggiungi Veicolo\nSposta (Posizione Corrente\nCancella (Definitivamente)", "Avanti", "Chiudi");
}


Dialog:Dialog_ShowRoomEditVeh(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
        return Dialog_Show(playerid, Dialog_EditShowRoom, DIALOG_STYLE_LIST, "ShowRoom Edit", "Lista Veicoli\nAggiungi Veicolo\nSposta (Posizione Corrente\nCancella (Definitivamente)", "Avanti", "Chiudi");
    }
    new 
        phase = GetPVarInt(playerid, "ShowRoomEdit_Phase"),
        showroomid = gAdminSelectedShowRoom[playerid],
        modelid = ShowRoomInfo[showroomid][srModels][gShowRoomItemList[playerid][listitem]];
    switch(phase)
    {
        case 0:
        {
            new 
                string[32]
            ;
            SetPVarInt(playerid, "ShowRoomEdit_SelectedSlotID", gShowRoomItemList[playerid][listitem]);
            format(string, sizeof(string), "%s (%d)", GetVehicleNameFromModel(modelid), modelid);
            SetPVarInt(playerid, "ShowRoomEdit_Phase", 1);
            Dialog_Show(playerid, Dialog_ShowRoomEditVeh, DIALOG_STYLE_LIST, string, "Rimuovi Veicolo\nModifica Prezzo", "Continua", "Annulla");
            return 1;
        }
        case 1:
        {
            if(listitem == 0)
            {
                new slotid = GetPVarInt(playerid, "ShowRoomEdit_SelectedSlotID");
                Dialog_Show(playerid, Dialog_ShowRoomDelVeh, DIALOG_STYLE_MSGBOX, "Concessionaria", "Sei sicuro di voler rimuovere questa %s dalla concessionaria?", "Rimuovi", "Annulla", GetVehicleNameFromModel(ShowRoomInfo[showroomid][srModels][slotid]));
            }
            else if(listitem == 1)
            {
                new slotid = GetPVarInt(playerid, "ShowRoomEdit_SelectedSlotID");
                Dialog_Show(playerid, Dialog_ShowRoomVehPrice, DIALOG_STYLE_INPUT, "Modifica Prezzo", "Inserisci il nuovo prezzo per la %s.\nPrezzo Attuale: %d", "Continua", "Annulla", GetVehicleNameFromModel(ShowRoomInfo[showroomid][srModels][slotid]), ShowRoomInfo[showroomid][srPrices][slotid]);
            }
            return 1;
        }
    }

    return 1;
}

Dialog:Dialog_ShowRoomDelVeh(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
        new 
            modelid = ShowRoomInfo[gAdminSelectedShowRoom[playerid]][srModels][GetPVarInt(playerid, "ShowRoomEdit_SelectedSlotID")],
            string[32];
        format(string, sizeof(string), "%s (%d)", GetVehicleNameFromModel(modelid), modelid);
        SetPVarInt(playerid, "ShowRoomEdit_Phase", 1);
        Dialog_Show(playerid, Dialog_ShowRoomEditVeh, DIALOG_STYLE_LIST, string, "Rimuovi Veicolo\nModifica Prezzo", "Continua", "Annulla");
        return 1;
    }
    new 
        showroomid = gAdminSelectedShowRoom[playerid],
        slotid = GetPVarInt(playerid, "ShowRoomEdit_SelectedSlotID");
    SendFormattedMessage(playerid, COLOR_GREEN, "> %s [%d]: Veicolo (%s) rimosso.", ShowRoomInfo[showroomid][srName], showroomid, GetVehicleNameFromModel(ShowRoomInfo[showroomid][srModels][slotid]));
    ShowRoom_DeleteVehicle(showroomid, slotid);
    DeletePVar(playerid, "ShowRoomEdit_SelectedSlotID");
    return 1;
}


Dialog:Dialog_ShowRoomVehPrice(playerid, response, listitem, inputtext[])
{
    new 
        showroomid = gAdminSelectedShowRoom[playerid],
        val = strval(inputtext), 
        slotid = GetPVarInt(playerid, "ShowRoomEdit_SelectedSlotID");
    if(val <= 0)
        return Dialog_Show(playerid, Dialog_ShowRoomEditVeh, DIALOG_STYLE_INPUT, "Modifica Prezzo", "{FF0000}Il prezzo non è valido!\n{FFFFFF}Inserisci il nuovo prezzo per la %s.\nPrezzo Attuale: %d", "Continua", "Annulla", GetVehicleNameFromModel(ShowRoomInfo[showroomid][srModels][slotid]), ShowRoomInfo[showroomid][srPrices][slotid]);
    ShowRoomInfo[showroomid][srPrices][slotid] = val;
    SendFormattedMessage(playerid, COLOR_GREEN, "> %s [%d]: Nuovo prezzo per %s. $%d.", ShowRoomInfo[showroomid][srName], showroomid, GetVehicleNameFromModel(ShowRoomInfo[showroomid][srModels][slotid]), val);
    DeletePVar(playerid, "ShowRoomEdit_SelectedSlotID");
    return 1;
}


Dialog:Dialog_ShowRoomAddVeh(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
        return Dialog_Show(playerid, Dialog_EditShowRoom, DIALOG_STYLE_LIST, "ShowRoom Edit", "Lista Veicoli\nAggiungi Veicolo\nSposta (Posizione Corrente\nCancella (Definitivamente)", "Avanti", "Chiudi");
    }
    new 
        modelid, price;
    if(isnull(inputtext) || sscanf(inputtext, "dd", modelid, price))
    {
        return Dialog_Show(playerid, Dialog_ShowRoomAddVeh, DIALOG_STYLE_INPUT, "Aggiungi Veicolo", "{FF0000}La stringa che hai inserito non è corretta!\n{FFFFFF}Inserisci il modello che vuoi inserire seguito dal prezzo!\nEsempio: 522 50000", "Aggiungi", "Indietro");
    }
    ShowRoom_AddVehicle(gAdminSelectedShowRoom[playerid], modelid, price);
    SendFormattedMessage(playerid, COLOR_GREEN, "> Hai aggiunto %s (%d) per $%d alla concessionaria (%d).", GetVehicleNameFromModel(modelid), modelid, price, gAdminSelectedShowRoom[playerid]);
    return 1;
}

stock ShowRoom_ClearEditVars(playerid)
{
    for(new i = 0; i < MAX_SHOWROOMS; ++i)
    {
        gShowRoomListItemList[playerid][i] = -1;
    }
    gAdminSelectedShowRoom[playerid] = -1;
    for(new i = 0; i < MAX_VEHICLES_IN_SHOWROOM; ++i)
    {
        gShowRoomItemList[playerid][i] = -1;
    }
    DeletePVar(playerid, "ShowRoomEdit_Phase");
    DeletePVar(playerid, "ShowRoomEdit_SelectedSlotID");
}