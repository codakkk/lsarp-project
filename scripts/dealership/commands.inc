
flags:acreateshowroom(CMD_DEVELOPER);
CMD:acreateshowroom(playerid, params[])
{
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    new id = Dealership_Create(x, y, z, GetPlayerVirtualWorld(playerid));
    SendFormattedMessage(playerid, COLOR_GREEN, "Concessionaria [%d] creata.", id);
    SendClientMessage(playerid, COLOR_GREEN, "Ricorda di settare tutte le impostazioni giuste.");
    SendClientMessage(playerid, COLOR_GREEN, "Utilizza /ashowroommenu <id> per cominciare a modificare questa concessionaria.");
    return 1;
}

flags:ashowroommenu(CMD_DEVELOPER);
CMD:ashowroommenu(playerid, params[])
{
    Dealership_ShowList(playerid, params);
    return 1;
}

Dialog:Dialog_AShowRoomList(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
	   Dealership_ClearEditVars(playerid);
	   return 1;
    }
    gAdminSelectedDealership[playerid] = gDealershipListItemList[playerid][listitem];
    new string[64];
    format(string, sizeof(string), "%s [%d]", DealershipInfo[gAdminSelectedDealership[playerid]][dsName], gAdminSelectedDealership[playerid]);
    Dialog_Show(playerid, Dialog_EditShowRoom, DIALOG_STYLE_LIST, string, "Lista Veicoli\nModifica Nome\nAggiungi Veicolo\nSposta (Posizione Corrente\nPosizione Veicolo\n{FF0000}Cancella (Definitivamente)", "Avanti", "Chiudi");
    return 1;
}

Dialog:Dialog_EditShowRoom(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
	   Dealership_ClearEditVars(playerid);
	   return 1;
    }
    switch(listitem)
    {
	   case 0: // Vehicle list
	   {
		  return Dealership_ShowInternalVehicles(gAdminSelectedDealership[playerid], playerid);
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
		  Dealership_SetPosition(gAdminSelectedDealership[playerid], x, y, z);
		  SendFormattedMessage(playerid, COLOR_GREEN, "a concessionaria (%d) è stata spostata con successo.", gAdminSelectedDealership[playerid]);
		  return 1;
	   }
	   case 4:
	   {
		  if(!IsPlayerInAnyVehicle(playerid))
			 return SendClientMessage(playerid, COLOR_ERROR, "Devi essere a bordo di un veicolo.");
		  new Float:x, Float:y, Float:z, Float:a;
		  GetVehiclePos(GetPlayerVehicleID(playerid), x, y, z);
		  GetVehicleZAngle(GetPlayerVehicleID(playerid), a);
		  Dealership_SetVehicleSpawnPos(gAdminSelectedDealership[playerid], x, y, z, a);
		  return 1;
	   }
	   case 5: // Delete (def)
	   {
		  Dialog_Show(playerid, Dialog_ShowRoomDelete, DIALOG_STYLE_MSGBOX, "ShowRoom Delete", "Sei sicuro di voler rimuovere questa concessionaria\ndefinitivamente?\nInfo: %s [%d]", "Elimina", "Annulla", DealershipInfo[gAdminSelectedDealership[playerid]][dsName], gAdminSelectedDealership[playerid]);
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
    if(Dealership_Delete(gAdminSelectedDealership[playerid]))
    {
	   SendFormattedMessage(playerid, COLOR_ERROR, "Concessionaria \"%s\" [%d] eliminata con successo.", DealershipInfo[gAdminSelectedDealership[playerid]][dsName], gAdminSelectedDealership[playerid]);
    }
    else
    {
	   SendClientMessage(playerid, COLOR_ERROR, "C'è stato un errore durante l'eliminazione della Concessionaria.");
    }
    
    return 1;
}


Dialog:Dialog_ShowRoomEditName(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
	   return Dealership_ShowInternalVehicles(gAdminSelectedDealership[playerid], playerid);
    }
    if(strlen(inputtext) > 22)
	   return Dialog_Show(playerid, Dialog_EditShowRoom, DIALOG_STYLE_INPUT, "ShowRoom Edit Name", "Il nome inserito è troppo lungo!\nInserisci il nome della concessionaria.\nLimite Caratteri: 22.", "Modifica", "Indietro");
    if(Dealership_SetName(gAdminSelectedDealership[playerid], inputtext))
    {
	   SendClientMessage(playerid, COLOR_GREEN, "Hai cambiato il nome della concessionaria.");
	   SendFormattedMessage(playerid, COLOR_GREEN, "Nuovo nome: %s", inputtext);
    }
    else
    {
	   SendClientMessage(playerid, COLOR_ERROR, "Il cambio del nome non è andato a buon fine.");
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
	   dealershipid = gAdminSelectedDealership[playerid],
	   modelid = DealershipInfo[dealershipid][dsModels][gDealershipItemList[playerid][listitem]];
    switch(phase)
    {
	   case 0:
	   {
		  new 
			 string[32]
		  ;
		  SetPVarInt(playerid, "ShowRoomEdit_SelectedSlotID", gDealershipItemList[playerid][listitem]);
		  format(string, sizeof(string), "%s (%d)", Vehicle_GetNameFromModel(modelid), modelid);
		  SetPVarInt(playerid, "ShowRoomEdit_Phase", 1);
		  Dialog_Show(playerid, Dialog_ShowRoomEditVeh, DIALOG_STYLE_LIST, string, "Rimuovi Veicolo\nModifica Prezzo", "Continua", "Annulla");
		  return 1;
	   }
	   case 1:
	   {
		  if(listitem == 0)
		  {
			 new slotid = GetPVarInt(playerid, "ShowRoomEdit_SelectedSlotID");
			 Dialog_Show(playerid, Dialog_ShowRoomDelVeh, DIALOG_STYLE_MSGBOX, "Concessionaria", "Sei sicuro di voler rimuovere questa %s dalla concessionaria?", "Rimuovi", "Annulla", Vehicle_GetNameFromModel(DealershipInfo[dealershipid][dsModels][slotid]));
		  }
		  else if(listitem == 1)
		  {
			 new slotid = GetPVarInt(playerid, "ShowRoomEdit_SelectedSlotID");
			 Dialog_Show(playerid, Dialog_ShowRoomVehPrice, DIALOG_STYLE_INPUT, "Modifica Prezzo", "Inserisci il nuovo prezzo per la %s.\nPrezzo Attuale: %d", "Continua", "Annulla", Vehicle_GetNameFromModel(DealershipInfo[dealershipid][dsModels][slotid]), DealershipInfo[dealershipid][dsPrices][slotid]);
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
		  modelid = DealershipInfo[gAdminSelectedDealership[playerid]][dsModels][GetPVarInt(playerid, "ShowRoomEdit_SelectedSlotID")],
		  string[32];
	   format(string, sizeof(string), "%s (%d)", Vehicle_GetNameFromModel(modelid), modelid);
	   SetPVarInt(playerid, "ShowRoomEdit_Phase", 1);
	   Dialog_Show(playerid, Dialog_ShowRoomEditVeh, DIALOG_STYLE_LIST, string, "Rimuovi Veicolo\nModifica Prezzo", "Continua", "Annulla");
	   return 1;
    }
    new 
	   dealershipid = gAdminSelectedDealership[playerid],
	   slotid = GetPVarInt(playerid, "ShowRoomEdit_SelectedSlotID");
    SendFormattedMessage(playerid, COLOR_GREEN, "%s [%d]: Veicolo (%s) rimosso.", DealershipInfo[dealershipid][dsName], dealershipid, Vehicle_GetNameFromModel(DealershipInfo[dealershipid][dsModels][slotid]));
    Dealership_DeleteVehicle(dealershipid, slotid);
    DeletePVar(playerid, "ShowRoomEdit_SelectedSlotID");
    return 1;
}


Dialog:Dialog_ShowRoomVehPrice(playerid, response, listitem, inputtext[])
{
    new 
	   dealershipid = gAdminSelectedDealership[playerid],
	   val = strval(inputtext), 
	   slotid = GetPVarInt(playerid, "ShowRoomEdit_SelectedSlotID");
    if(val <= 0)
	   return Dialog_Show(playerid, Dialog_ShowRoomEditVeh, DIALOG_STYLE_INPUT, "Modifica Prezzo", "{FF0000}Il prezzo non è valido!\n{FFFFFF}Inserisci il nuovo prezzo per la %s.\nPrezzo Attuale: %d", "Continua", "Annulla", Vehicle_GetNameFromModel(DealershipInfo[dealershipid][dsModels][slotid]), DealershipInfo[dealershipid][dsPrices][slotid]);
    DealershipInfo[dealershipid][dsPrices][slotid] = val;
    SendFormattedMessage(playerid, COLOR_GREEN, "%s [%d]: Nuovo prezzo per %s. $%d.", DealershipInfo[dealershipid][dsName], dealershipid, Vehicle_GetNameFromModel(DealershipInfo[dealershipid][dsModels][slotid]), val);
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
    Dealership_AddVehicle(gAdminSelectedDealership[playerid], modelid, price);
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai aggiunto %s (%d) per $%d alla concessionaria (%d).", Vehicle_GetNameFromModel(modelid), modelid, price, gAdminSelectedDealership[playerid]);
    return 1;
}

stock Dealership_ClearEditVars(playerid)
{
    for(new i = 0; i < MAX_DEALERSHIPS; ++i)
    {
	   gDealershipListItemList[playerid][i] = -1;
    }
    gAdminSelectedDealership[playerid] = -1;
    for(new i = 0; i < MAX_VEHICLES_IN_SHOWROOM; ++i)
    {
	   gDealershipItemList[playerid][i] = -1;
    }
    DeletePVar(playerid, "ShowRoomEdit_Phase");
    DeletePVar(playerid, "ShowRoomEdit_SelectedSlotID");
}