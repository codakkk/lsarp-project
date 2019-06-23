Dialog:Dialog_LootZoneList(playerid, response, listitem, inputtext[])
{
	if(!response)
	{
		Player_ClearLootZoneVars(playerid);
		return 1;
	}

	new 
		last_id = GetPVarInt(playerid, "Player_AdminLootZoneListLastID"),
		current_count = GetPVarInt(playerid, "Player_AdminLootZoneListCount")
		;
	
	#pragma unused current_count

	if(listitem == 0) // Avanti -->
	{
		return LootZone_AdminShowMenu(playerid, last_id);
	}
	else if(listitem == 1) // <-- Indietro
	{
		return LootZone_AdminShowMenu(playerid, last_id - MAX_LOOT_ZONES_PER_PAGE);
	}
	else if(listitem == 2) // Modifica by ID
	{
		return Dialog_Show(playerid, Dialog_LootZoneListAdmID, DIALOG_STYLE_INPUT, "Edit Loot Zone By ID", "Inserisci l'ID della Loot Zone che vuoi modificare.\nN.B: l'ID è tra [N] nel nome delle loot zone.", "Conferma", "Annulla");
	}

	listitem -= 3;

	new zone_id = LootZoneListItem[playerid][listitem];

	if(!LootZone_StartEdit(zone_id, playerid))
	{
		return LootZone_AdminShowMenu(playerid, last_id);
	}
	return 1;
}

Dialog:Dialog_LootZoneListAdmID(playerid, response, listitem, inputtext[])
{
	if(!response)
	{
		return LootZone_AdminShowMenu(playerid, GetPVarInt(playerid, "Player_AdminLootZoneListLastID"));
	}
	if(!IsNumeric(inputtext))
	{
		return Dialog_Show(playerid, Dialog_LootZoneListAdmID, DIALOG_STYLE_INPUT, "Loot Zone By ID", "Loot Zone ID non valido.\nInserisci l'ID della Loot Zone che vuoi modificare.", "Conferma", "Annulla");
	}
	new zone_id = strval(inputtext);
	if(!LootZone_StartEdit(zone_id, playerid))
	{
		return Dialog_Show(playerid, Dialog_LootZoneListAdmID, DIALOG_STYLE_INPUT, "Loot Zone By ID", "Loot Zone ID non valida oppure qualcuno la sta gia' modificando!\nInserisci l'ID della Loot Zone che vuoi modificare.", "Conferma", "Annulla");
	}
	return 1;
}

Dialog:Dialog_LootZoneEdit(playerid, response, listitem, inputtext[])
{
	new 
		zone_id = Player_GetEditingLootZone(playerid);
	
	if(!response)
	{
		LootZone_EndEdit(zone_id, playerid);
		return LootZone_AdminShowMenu(playerid, Player_GetLastLootZoneAdminListItemID(playerid));
	}
	switch(listitem)
	{
		case 0: // Add Item
		{
			new
				string[64];
			format(string, sizeof(string), "Modifica Loot Zone [%d]", zone_id);
			Dialog_Show(playerid, Dialog_LootZoneAddItem, DIALOG_STYLE_INPUT, string, "Inserisci l'ID dell'item che vuoi inserire.\nPer una lista completa, usa: {00FF00}/aitemslist{FFFFFF}.\nN.B: /aitemslist chiudera' il dialog corrente.", "Aggiungi", "Indietro");
		}
		case 1: // Modify Loot
		{
			new 
				title[64],
				content[256],
				idx = 0;

			format(title, sizeof(title), "Modifica Loot Zone [%d]", zone_id);

			for(new i = 0; i < MAX_ITEMS_PER_LOOT; ++i)
			{
				new item_id = LootZoneInfo[zone_id][lzLootItem][i];
				if(!item_id)
					continue;
				format(content, sizeof(content), "%s%s\t%d\t%d\n", content, ServerItem_GetName(item_id), LootZoneInfo[zone_id][lzLootAmount][i], LootZoneInfo[zone_id][lzLootRarity][i]);
				LootZoneListItem[playerid][idx] = i;
				idx++;
			}
			if(idx == 0)
			{
				SendClientMessage(playerid, COLOR_ERROR, "[Loot Zone] > Non ci sono items da modificare in questa Loot Zone.");
				new string[64];
				format(string, sizeof(string), "Modifica Loot Zone [%d]", LootZoneListItem[playerid][listitem]);
				Dialog_Show(playerid, Dialog_LootZoneEdit, DIALOG_STYLE_LIST, string, "Aggiungi Item\nModifica Item ID\nSposta Qui\nRimuovi Zona", "Seleziona", "Indietro");
			}
			else
				Dialog_Show(playerid, Dialog_LootZoneIListEdit, DIALOG_STYLE_TABLIST_HEADERS, title, "Item\tQuantita'(Random)\tDrop Chance(%%)\n%s", "Seleziona", "Indietro", content);
		}
		case 2: // Move
		{
			new
				Float:x,
				Float:y,
				Float:z;
			GetPlayerPos(playerid, x, y, z);
			LootZone_SetPosition(zone_id, x, y, z, GetPlayerInterior(playerid), Character_GetVirtualWorld(playerid));
			SendFormattedMessage(playerid, -1, [LOOT ZONE %d] Posizione modificata.", zone_id);
			new s[4];
			valstr(s, zone_id);
			//Log(playerid, "Loot Zone Position", s);
			LootZone_StartEdit(zone_id, playerid);
		}
		case 3: // Go-TO
		{
			SetPlayerPos(playerid, LootZoneInfo[zone_id][lzX], LootZoneInfo[zone_id][lzY], LootZoneInfo[zone_id][lzZ]);
			SetPlayerInterior(playerid, LootZoneInfo[zone_id][lzInterior]);
			Character_SetVirtualWorld(playerid, LootZoneInfo[zone_id][lzVirtualWorld]);
			LootZone_EndEdit(zone_id, playerid);
		}
		case 4: // Delete
		{
			Dialog_Show(playerid, Dialog_LootZoneDelete, DIALOG_STYLE_MSGBOX, "Loot Zone", "Sei sicuro di voler eliminare definitivamente\nquesta Loot Zone?", "Rimuovi", "Annulla");
		}
	}
	return 1;
}

Dialog:Dialog_LootZoneAddItem(playerid, response, listitem, inputtext[])
{
	new 
		zone_id = Player_GetEditingLootZone(playerid);
	if(!response)
	{
		return LootZone_StartEdit(zone_id, playerid);
	}
	new
		val = strval(inputtext);
	if(!IsNumeric(inputtext) || !ServerItem_IsValid(val) || val == 0)
	{
		return Dialog_Show(playerid, Dialog_LootZoneAddItem, DIALOG_STYLE_INPUT, "Item Edit", "{FF0000}L'ID non e' valido!{FFFFFF}\nInserisci l'ID dell'item.\nUsa /aitemlist per una lista completa\ndi tutti gli items.", "Aggiungi", "Annulla");
	}

	new free_space = -1; //LootZone_FreeItemSlot(zone_id); // LootZone_FreeItemSlot searches for free items slot in CurrentLootItems. Here we need LootItems

	for(new i = 0; i < MAX_ITEMS_PER_LOOT; ++i)
	{
		if(!LootZoneInfo[zone_id][lzLootItem][i])
		{
			free_space = i;
			break;
		}
	}

	if(free_space == -1)
	{
		SendFormattedMessage(playerid, COLOR_GREEN, "[LOOT ZONE %d]: Non e' stato possibile aggiungere l'item alla Loot Zone. Limite raggiunto.", zone_id, ServerItem_GetName(val), val);
		return LootZone_StartEdit(zone_id, playerid);
	}

	SendFormattedMessage(playerid, COLOR_GREEN, "[LOOT ZONE %d]: %s (%d) e' stato aggiunto con successo tra gli items droppabili.", zone_id, ServerItem_GetName(val), val);

	LootZoneInfo[zone_id][lzLootItem][free_space] = val;
	LootZoneInfo[zone_id][lzLootAmount][free_space] = 1;
	LootZoneInfo[zone_id][lzLootRarity][free_space] = 100;
	LootZone_Save(zone_id);

	LootZone_StartEdit(zone_id, playerid);
	//Log(playerid, "Loot Zone Add item", zone_id, ServerItem_GetName(val), val);
	return 1;
}

Dialog:Dialog_LootZoneDelete(playerid, response, listitem, inputtext[])
{
	new 
		zone_id = Player_GetEditingLootZone(playerid);
	if(!response)
	{
		return LootZone_StartEdit(zone_id, playerid);
	}
	LootZone_Delete(zone_id);
	
	LootZone_EndEdit(zone_id, playerid);
	LootZone_AdminShowMenu(playerid, Player_GetLastLootZoneAdminListItemID(playerid));
	SendFormattedMessage(playerid, -1, [LOOT ZONE %d] Loot Zone rimossa con successo.", zone_id);
	//Log(playerid, "Loot Zone Delete", zone_id);
	return 1;
}

Dialog:Dialog_LootZoneIListEdit(playerid, response, listitem, inputtext[])
{
	if(!response)
	{
		return LootZone_AdminShowMenu(playerid, Player_GetLastLootZoneAdminListItemID(playerid));
	}
	SetPVarInt(playerid, "LootZoneSelectedItemID", LootZoneListItem[playerid][listitem]);
	Dialog_Show(playerid, Dialog_LootZoneEditItem, DIALOG_STYLE_LIST, "Loot Zone Item Edit", "Modifica ID\nModifica Quantita'\nDrop Chance(%%)\n{FF0000}Elimina", "Seleziona", "Indietro");
	return 1;
}

Dialog:Dialog_LootZoneEditItem(playerid, response, listitem, inputtext[])
{
	if(!response)
	{
		return 1;	
	}
	new 
		zone_id = Player_GetEditingLootZone(playerid),
		loot_zone_item_slot = GetPVarInt(playerid, "LootZoneSelectedItemID"),
		item_id = LootZoneInfo[zone_id][lzLootItem][loot_zone_item_slot];
	switch(listitem)
	{
		case 0: // Item ID Edit
		{
			Dialog_Show(playerid, Dialog_LootZoneSetItemID, DIALOG_STYLE_INPUT, "Item Edit", "Inserisci l'ID del nuovo item.\nUsa /aitemlist per una lista completa\ndi tutti gli items.", "Modifica", "Annulla");
		}
		case 1: // Amount Edit
		{
			new 
				string[64];
			format(string, sizeof(string), "%s | Item Amount", ServerItem_GetName(item_id));
			Dialog_Show(playerid, Dialog_LZSetItemAmount, DIALOG_STYLE_INPUT, string, "Inserisci l'ammontare massimo droppabile per questo item.", "Modifica", "Indietro");
		}
		case 2: // Drop Chance
		{
			//SendClientMessage(playerid, COLOR_ERROR, Funzionalita' ancora non disponibile.");
			new 
				string[64];
			format(string, sizeof(string), "%s | Item Drop Chance", ServerItem_GetName(item_id));
			Dialog_Show(playerid, Dialog_LZSetItemRarity, DIALOG_STYLE_INPUT, string, "Inserisci la drop chance per questo oggetto.\nOggetto: %s\nDrop Chance Corrente: %d", "Modifica", "Indietro", 
						ServerItem_GetName(item_id), LootZoneInfo[zone_id][lzLootRarity][loot_zone_item_slot]);
		}
		case 3: // Remove Item
		{
			new
				string[64];
			format(string, sizeof(string), "Sei sicuro di voler eliminare definitivamente\nquesto item (%s [%d]) dalla Loot Zone?", ServerItem_GetName(item_id), item_id);
			Dialog_Show(playerid, Dialog_LZoneItemRemove, DIALOG_STYLE_MSGBOX, "Loot Zone", string, "Rimuovi", "Indietro");
		}
	}
	return 1;
}

Dialog:Dialog_LootZoneSetItemID(playerid, response, listitem, inputtext[])
{
	new 
		zone_id = Player_GetEditingLootZone(playerid),
		item_list_id = GetPVarInt(playerid, "LootZoneSelectedItemID");
	
	if(!response)
	{
		return LootZone_StartEdit(zone_id, playerid);
	}
	new
		val = strval(inputtext);
	if(!IsNumeric(inputtext) || !ServerItem_IsValid(val) || val == 0)
	{
		return Dialog_Show(playerid, Dialog_LootZoneSetItemID, DIALOG_STYLE_INPUT, "Item Edit", "{FF0000}L'ID non e' valido!{FFFFFF}\nInserisci l'ID dell'item.\nUsa /aitemlist per una lista completa\ndi tutti gli items.", "Modifica", "Annulla");
	}


	SendFormattedMessage(playerid, COLOR_GREEN, "[LOOT ZONE %d]: Hai cambiato con successo l'item della Loot Zone. Nuovo Item: %s (%d)", zone_id, ServerItem_GetName(val), val);

	LootZoneInfo[zone_id][lzLootItem][item_list_id] = val;
	LootZoneInfo[zone_id][lzLootAmount][item_list_id] = 1;
	LootZoneInfo[zone_id][lzLootRarity][item_list_id] = 100;
	LootZone_Save(zone_id);

	// Log(playerid, "Loot Zone Set Item ID", zone_id, ServerItem_GetName(val), val);
	return 1;
}

Dialog:Dialog_LZSetItemAmount(playerid, response, listitem, inputtext[])
{
	new
		val = strval(inputtext),
		zone_id = Player_GetEditingLootZone(playerid),
		loot_zone_item_slot = GetPVarInt(playerid, "LootZoneSelectedItemID"),
		item_id = LootZoneInfo[zone_id][lzLootItem][loot_zone_item_slot];
	if(!response)
	{
		return LootZone_StartEdit(zone_id, playerid);
	}
	if(!IsNumeric(inputtext) || val <= 0)
	{
		new 
			string[64];
		format(string, sizeof(string), "%s | Item Amount", ServerItem_GetName(item_id));
		return Dialog_Show(playerid, Dialog_LZSetItemAmount, DIALOG_STYLE_INPUT, string, "{FF0000}Ammontare non valido!\n{FFFFFF}Inserisci l'ammontare massimo droppabile per questo item.", "Modifica", "Annulla");
	}

	LootZoneInfo[zone_id][lzLootAmount][loot_zone_item_slot] = val;
	SendFormattedMessage(playerid, COLOR_GREEN, "[LOOT ZONE %d]: Hai cambiato l'ammontare dell'item. Nuovo: %d", zone_id, val);
	LootZone_Save(zone_id);
	Dialog_Show(playerid, Dialog_LootZoneEditItem, DIALOG_STYLE_LIST, "Loot Zone Item Edit", "Modifica ID\nModifica Quantita'\nDrop Chance(%%)\n{FF0000}Elimina", "Seleziona", "Indietro");
	return 1;
}

Dialog:Dialog_LZSetItemRarity(playerid, response, listitem, inputtext[])
{
	if(!response)
	{
		return Dialog_Show(playerid, Dialog_LootZoneEditItem, DIALOG_STYLE_LIST, "Loot Zone Item Edit", "Modifica ID\nModifica Quantita'\nDrop Chance(%%)\n{FF0000}Elimina", "Seleziona", "Indietro");
	}

	new
		value = strval(inputtext),
		zone_id = Player_GetEditingLootZone(playerid),
		loot_zone_item_slot = GetPVarInt(playerid, "LootZoneSelectedItemID"),
		item_id = LootZoneInfo[zone_id][lzLootItem][loot_zone_item_slot];

	if(!IsNumeric(inputtext) || value < 0 || value > 100)
	{
		new 
			string[64];
		format(string, sizeof(string), "%s | Item Drop Chance", ServerItem_GetName(item_id));
		return Dialog_Show(playerid, Dialog_LZSetItemRarity, DIALOG_STYLE_INPUT, string, "{FF0000}Drop Chance non valida! (0 - 100)\n{FFFFFF}Inserisci la drop chance per questo oggetto.\n{00FF00}Oggetto: %s\n{0000FF}Drop Chance Corrente: %d", "Modifica", "Indietro", 
				ServerItem_GetName(item_id), LootZoneInfo[zone_id][lzLootRarity][loot_zone_item_slot]);
	}

	
	SendFormattedMessage(playerid, COLOR_GREEN, [LOOT ZONE %d | Item: %s (%d)]: Hai modificato la drop chance dell'oggetto da %d a %d", 
						zone_id, ServerItem_GetName(item_id), item_id, LootZoneInfo[zone_id][lzLootRarity][loot_zone_item_slot], value);
	LootZoneInfo[zone_id][lzLootRarity][loot_zone_item_slot] = value;
	LootZone_Save(zone_id);
	Dialog_Show(playerid, Dialog_LootZoneEditItem, DIALOG_STYLE_LIST, "Loot Zone Item Edit", "Modifica ID\nModifica Quantita'\nDrop Chance(%%)\n{FF0000}Elimina", "Seleziona", "Indietro");
	//Log(playerid, "Loot Zone Set Drop Chance", zone_id, ServerItem_GetName(item_id), value);
	return 1;
}

Dialog:Dialog_LZoneItemRemove(playerid, response, listitem, inputtext[])
{
	if(!response)
	{
		return Dialog_Show(playerid, Dialog_LootZoneEditItem, DIALOG_STYLE_LIST, "Loot Zone Item Edit", "Modifica ID\nModifica Quantita'\nDrop Chance(%%)\n{FF0000}Elimina", "Seleziona", "Indietro");
	}
	//Remove item from lootzone
	new
		zone_id = Player_GetEditingLootZone(playerid),
		item_list = GetPVarInt(playerid, "LootZoneSelectedItemID");

	if(!LootZoneInfo[zone_id][lzLootItem][item_list])
	{
		return 1;
	}

	SendFormattedMessage(playerid, COLOR_GREEN, "[LOOT ZONE %d]: %s rimosso con successo dalla Loot Zone.", zone_id, ServerItem_GetName(LootZoneInfo[zone_id][lzLootItem][item_list]));
	
	LootZoneInfo[zone_id][lzLootItem][item_list] = 0;
	LootZoneInfo[zone_id][lzLootAmount][item_list] = 0;
	LootZoneInfo[zone_id][lzLootRarity][item_list] = 0;
	
	LootZone_Save(zone_id);

	LootZone_StartEdit(zone_id, playerid);
	
	//Log(playerid, "Loot Zone Remove Item", zone_id, InventoryObjects[LootZoneInfo[zone_id][lzLootItem][item_list]][invName]);
	return 1;
}

Dialog:Dialog_LootZoneRemove(playerid, response, listitem, inputtext[])
{
	if(response)
	{
		new
			zone_id = Player_GetEditingLootZone(playerid);
		if(LootZone_Delete(zone_id))
		{
			SendFormattedMessage(playerid, COLOR_GREEN, "[LOOT ZONE %d]: Loot Zone {FF0000}rimossa{00FF00} con successo.", zone_id);
			LootZone_EndEdit(zone_id, playerid);
			return LootZone_AdminShowMenu(playerid, Player_GetLastLootZoneAdminListItemID(playerid));
		}
		else
		{
			SendFormattedMessage(playerid, COLOR_GREEN, "[LOOT ZONE %d]: Non e' stato possibile rimuovere questa Loot Zone. Contatta lo sviluppatore.", zone_id);
			LootZone_EndEdit(zone_id, playerid);
		}
	}
	return 1;
}

Dialog:Dialog_ShowItemsToPlayer(playerid, response, listitem, inputtext[])
{
	if(!response)
	{
		Player_ClearLootZoneVars(playerid);
		return 1;
	}
	new
		list_id = LootZoneListItem[playerid][listitem],
		zone_id = Player_GetLootZone(playerid),
		item_id = LootZoneInfo[zone_id][lzCurrentLootItems][list_id],
		extra 	= LootZoneInfo[zone_id][lzCurrentLootExtra][list_id],
		amount 	= LootZoneInfo[zone_id][lzCurrentLootAmount][list_id]
		;
	if(!LootZone_IsValid(zone_id))
		return 1;
	
	if(!IsPlayerInRangeOfLootZone(playerid, zone_id))
	{
		Player_ClearLootZoneVars(playerid);
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino alla zona.");
	}

	/*if(Inventory_FreeID(playerid) == -1)
	{
		SendClientMessage(playerid, COLOR_ERROR, Non hai spazio nell'inventario.");
		// Player_ClearLootZoneVars(playerid); // Probably unnecessary, because ShowItemsToPlayer already checks if the same player is requesting the list.
		return LootZone_ShowItemsToPlayer(playerid, zone_id);
	}*/

	if(ServerItem_IsUnique(item_id) || amount == 1)
	{
		// Inventory_AddItem(playerid, Inventory_FreeID(playerid), ServerItem_GetName(item_id), 1, extra);
		SendFormattedMessage(playerid, -1, Hai raccolto %s", ServerItem_GetName(item_id));
		LootZoneInfo[zone_id][lzCurrentLootAmount][list_id] -= 1;
		if(LootZoneInfo[zone_id][lzCurrentLootAmount][list_id] <= 0)
		{
			LootZoneInfo[zone_id][lzCurrentLootItems][list_id] = 0;
			LootZoneInfo[zone_id][lzCurrentLootAmount][list_id] = 0;
			LootZoneInfo[zone_id][lzCurrentLootExtra][list_id] = 0;
		}

		// Log(playerid, "Loot Zone Looting", zone_id, ServerItem_GetName(item_id), 1);

		Player_ClearLootZoneVars(playerid);
		return LootZone_ShowItemsToPlayer(playerid, zone_id);
	}
	else 
	{
		SetPVarInt(playerid, "LootZone_PlayerItemListItem", list_id);
		return Dialog_Show(playerid, Dialog_LootZoneWithdraw, DIALOG_STYLE_INPUT, "Quantita'", "Inserisci la quantita' da raccogliere.", "Raccogli", "Indietro");
	}
}

Dialog:Dialog_LootZoneWithdraw(playerid, response, listitem, inputtext[])
{
	new 
		list_id = GetPVarInt(playerid, "LootZone_PlayerItemListItem"),
		zone_id = Player_GetLootZone(playerid),
		item_id = LootZoneInfo[zone_id][lzCurrentLootItems][list_id],
		extra 	= LootZoneInfo[zone_id][lzCurrentLootExtra][list_id],
		amount 	= LootZoneInfo[zone_id][lzCurrentLootAmount][list_id],
		val = strval(inputtext)
		;
	
	if(!LootZone_IsValid(zone_id))
		return 1;
	
	if(!response)
		return LootZone_ShowItemsToPlayer(playerid, zone_id);
	
	if(!IsPlayerInRangeOfLootZone(playerid, zone_id))
	{
		Player_ClearLootZoneVars(playerid);
		return SendClientMessage(playerid, COLOR_ERROR, Non sei vicino alla zona.");
	}

	if(!IsNumeric(inputtext) || val > amount || val <= 0)
	{
		new string[20];
		format(string, sizeof(string), "Quantita' [%d]", amount);
		return Dialog_Show(playerid, Dialog_LootZoneWithdraw, DIALOG_STYLE_INPUT, string, "{FF0000}Quantita' non valida!\n{FFFFFF}Inserisci la quantita' che vuoi raccogliere", "Raccogli", "Indietro");
	}

	// Inventory_AddItem(playerid, Inventory_FreeID(playerid), ServerItem_GetName(item_id), val, extra);

	SendFormattedMessage(playerid, COLOR_GREEN, Hai raccolto %s. Ammontare: %d", ServerItem_GetName(item_id), val);

	LootZoneInfo[zone_id][lzCurrentLootAmount][list_id] -= val;
	if(LootZoneInfo[zone_id][lzCurrentLootAmount][list_id] <= 0)
	{
		LootZoneInfo[zone_id][lzCurrentLootItems][list_id] = 0;
		LootZoneInfo[zone_id][lzCurrentLootAmount][list_id] = 0;
		LootZoneInfo[zone_id][lzCurrentLootExtra][list_id] = 0;
	}

	// Log(playerid, "Loot Zone Looting", zone_id, ServerItem_GetName(item_id), val);

	Player_ClearLootZoneVars(playerid);
	return LootZone_ShowItemsToPlayer(playerid, zone_id);
}