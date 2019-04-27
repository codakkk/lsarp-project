#include <YSI_Coding\y_hooks>

// #define Character_%0(%1, Inventory_%0(Character_GetInventory(%1), // It is a convenient way to call Inventory_Func with player inventory

stock bool:Character_InitializeInventory(playerid)
{
	//if(!Character_IsLogged(playerid))
		//return false;
	new Inventory:inventory = Inventory_New(PLAYER_INVENTORY_START_SIZE);
	map_add(PlayerInventory, playerid, List:inventory);
	printf("Inventory Initialized");
	return true;
}

stock bool:Character_UnloadInventory(playerid)
{
	if(!Character_IsLogged(playerid))
		return false;
	map_remove_deep(PlayerInventory, playerid);
	return true;
}

hook OnCharacterSaveData(playerid)
{
	Character_SaveInventory(playerid);
	return 1;
}

hook OnPlayerCharacterLoad(playerid)
{
	printf("player_inventory.pwn/OnPlayerCharacterLoad");
	Character_InitializeInventory(playerid);
	Character_LoadInventory(playerid);
	return 1;
}

hook OnPlayerClearData(playerid)
{
	Character_ResetInventory(playerid);
	Character_UnloadInventory(playerid);
	printf("player_inventory.pwn/OnPlayerClearData");
	return 1;
}


hook OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
	// printf("inventory.pwn/OnPlayerWeaponShot. State: %d", GetPlayerWeaponState(playerid));
	new ammo = GetPlayerAmmo(playerid);
	if(( (GetPlayerWeaponState(playerid) == WEAPONSTATE_LAST_BULLET || IsPlayerInAnyVehicle(playerid)) && AC_AntiWeaponCheck(playerid, weaponid, ammo) && ammo <= 1))
	{
		if(Character_HasSpaceForItem(playerid, weaponid, 1))
		{
			SendClientMessage(playerid, COLOR_GREEN, "Proiettili esauriti. L'arma è stata rimessa nell'inventario.");
			Character_GiveItem(playerid, weaponid, 1, 0, false);
			AC_RemovePlayerWeapon(playerid, weaponid);
		}
		else
		{
			SendClientMessage(playerid, COLOR_ERROR, "Proiettili esauriti. Hai l'inventario pieno, quindi l'arma è stata gettata a terra.");
			new Float:x, Float:y, Float:z;
			if(IsPlayerInAnyVehicle(playerid))
				GetVehiclePos(GetPlayerVehicleID(playerid), x, y, z);
			else 
			 	GetPlayerPos(playerid, x, y, z);
			Drop_Create(x, y, z - 0.9, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), weaponid, 1, 0, Character_GetOOCNameStr(playerid));
			AC_RemovePlayerWeapon(playerid, weaponid);
		}
		if(PendingRequestInfo[playerid][rdPending] && PendingRequestInfo[playerid][rdType] == PENDING_TYPE_WEAPON && PendingRequestInfo[playerid][rdSlot] == Weapon_GetSlot(weaponid) && PendingRequestInfo[playerid][rdItem] == weaponid)
		{
			ResetPendingRequest(playerid);
			ResetPendingRequest(PendingRequestInfo[playerid][rdToPlayer]);
			SendClientMessage(playerid, COLOR_ERROR, "La richiesta è stata annullata poiché non hai più l'arma!");
			SendClientMessage(PendingRequestInfo[playerid][rdToPlayer], COLOR_ERROR, "La richiesta ricevuta dal giocatore è stata annullata poiché non possiede più l'arma!");
		}
	}
	return 1;
}

stock Inventory:Character_GetInventory(playerid)
{
	new Inventory:inv;
	if(!map_has_key(PlayerInventory, playerid))
	{
		Character_InitializeInventory(playerid);
	}
	map_get_safe(PlayerInventory, playerid, List:inv);
	return inv;
}

// Returns success or not.
// if greater than 0, returns the difference.
stock Character_GiveItem(playerid, item_id, amount = 1, extra = 0, bool:callback = false)
{
	#pragma unused callback
	
	new Inventory:playerInventory = Character_GetInventory(playerid);
	
	new result = Inventory_AddItem(playerInventory, item_id, amount, extra);
	Character_SaveInventory(playerid);
	return result;
}

stock Character_DecreaseSlotAmount(playerid, slotid, amount = 1)
{
	new Inventory:playerInventory = Character_GetInventory(playerid);
	
	new result = Inventory_DecreaseSlotAmount(playerInventory, slotid, amount);

	//if(result)
		//CallLocalFunction("OnPlayerInventoryItemDecrease", "iii", playerid, itemid, amount);

	Character_SaveInventory(playerid);
	return result;
}

stock Character_DecreaseItemAmount(playerid, itemid, amount = 1)
{
	new Inventory:playerInventory = Character_GetInventory(playerid);
	
	new result = Inventory_DecreaseItemAmount(playerInventory, itemid, amount);

	if(result == INVENTORY_DECREASE_SUCCESS)
		CallLocalFunction("OnPlayerInventoryItemDecrease", "iii", playerid, itemid, amount);

	Character_SaveInventory(playerid);
	return result;
}

stock Character_ResizeInventory(playerid)
{
	new Inventory:inv = Character_GetInventory(playerid);
	Inventory_Resize(inv, Character_GetInventorySize(playerid));
}

stock Character_GetInventorySize(playerid)
{
	return PLAYER_INVENTORY_START_SIZE + ServerItem_GetBagSize(Character_GetBag(playerid));
}

stock bool:Character_IsSlotUsed(playerid, slotid)
{
	return Inventory_IsSlotUsed(Character_GetInventory(playerid), slotid);
}

// Checks if slotid is in the inventory boundaries (0 <= slotid <= Inventory_GetSpace(inventory))
stock Character_IsValidSlot(playerid, slotid)
{
	return Inventory_IsValidSlot(Character_GetInventory(playerid), slotid);
}

stock Character_GetSlotExtra(playerid, slotid)
{
	return Inventory_GetSlotExtra(Character_GetInventory(playerid), slotid);
}

stock Character_ShowInventory(playerid, targetid)
{
	if(!Character_IsLogged(playerid))
		return 0;

	new Inventory:playerInventory = Character_GetInventory(playerid);

	new 
		String:string = Inventory_ParseForDialog(playerInventory),
		String:title = str_format("Inventario (%d/%d)", Inventory_GetUsedSpace(playerInventory), Inventory_GetSpace(playerInventory));
	Dialog_Show_s(targetid, Dialog_InventoryItemList, DIALOG_STYLE_TABLIST_HEADERS, title, string, "Avanti", "Chiudi");
	return 1;
}

stock Character_UseInventoryItem(playerid, slotid)
{
	new 
		itemid = Character_GetSlotItem(playerid, slotid),
		amount = Character_GetSlotAmount(playerid, slotid),
		extra = Character_GetSlotExtra(playerid, slotid);
	if(ServerItem_IsBag(itemid))
    {
		// If character has bag
		if(Character_HasBag(playerid))
		{
			SendClientMessage(playerid, COLOR_ERROR, "Stai già indossando uno zaino! Usa '/rimuovi zaino' per rimuoverlo.");
			return 0;
		}
		Character_AMe(playerid, "indossa lo zaino");
		//SendFormattedMessage(playerid, COLOR_GREEN, "Stai indossando '%s'. Usa '/rimuovi zaino' per rimuoverlo.", ServerItem_GetName(item_id));
		Character_SetBag(playerid, itemid);
		Player_InfoStr(playerid, str_format("Stai indossando: ~g~%s", ServerItem_GetName(itemid)), false);
		Character_DecreaseSlotAmount(playerid, slotid, 1);
	}
	else if(ServerItem_IsWeapon(itemid))
	{
		new 
			weaponSlot = Weapon_GetSlot(itemid), 
			weapon, 
			ammo;
		GetPlayerWeaponData(playerid, weaponSlot, weapon, ammo);
		if(Character_HasWeaponInSlot(playerid, weaponSlot) && !Weapon_IsGrenade(weapon))
		{
			SendClientMessage(playerid, COLOR_ERROR, "Hai già un'arma equipaggiata in questa slot.");	
			return 0;
		}
		if(Weapon_RequireAmmo(itemid))
		{
			if(extra > 0)
			{
				AC_GivePlayerWeapon(playerid, itemid, extra);
				Player_InfoStr(playerid, str_format("Hai equipaggiato: ~g~%s~w~~n~con ~g~%d~w~ proiettili.", Weapon_GetName(itemid), extra), true);
			}
			else
			{
				if(Character_HasItem(playerid, Weapon_GetAmmoType(itemid), 1) == -1)
					SendClientMessage(playerid, COLOR_ERROR, "Non hai le munizioni necessarie.");
				else
				{
					SetPVarInt(playerid, "InventorySelect_WeaponItem", itemid);
					new ammos = Inventory_GetItemAmount(Character_GetInventory(playerid), Weapon_GetAmmoType(itemid));
					Dialog_Show(playerid, Dialog_InvSelectAmmo, DIALOG_STYLE_INPUT, "Inserisci le munizioni", "Immetti la quantità di munizioni che vuoi inserire nell'arma.\nQuantità: {00FF00}%d{FFFFFF}.", "Usa", "Annulla", ammos);
				}
			}
		}
		else
		{
			AC_GivePlayerWeapon(playerid, itemid, amount);
			Player_InfoStr(playerid, str_format("Hai equipaggiato: ~g~%s~w~", Weapon_GetName(itemid)), true);
			Character_DecreaseSlotAmount(playerid, slotid, 1);
			Trigger_OnPlayerInvItemUse(playerid, itemid, 1, ITEM_TYPE_WEAPON);
		}
	}
    else if(ServerItem_IsAmmo(itemid))
    {
		new currentWeapon = GetPlayerWeapon(playerid);
		if(currentWeapon == 0)
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non hai armi equipaggiate!");
			return 0;
		} 
		if(!Weapon_RequireAmmo(currentWeapon) || Weapon_GetAmmoType(currentWeapon) != itemid)
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare queste munizioni su quest'arma.");
			return 0;
		}
		new ammos = Inventory_GetItemAmount(Character_GetInventory(playerid), itemid);
		SetPVarInt(playerid, "InventorySelect_CurrentWeaponItem", currentWeapon);
		Dialog_Show(playerid, Dialog_InvSelectAddAmmo, DIALOG_STYLE_INPUT, "Inserisci le munizioni", "Immetti la quantità di munizioni che vuoi inserire nell'arma.\nQuantità: {00FF00}%d{FFFFFF}.", "Usa", "Annulla", ammos);
    }
	return 1;
}

Dialog:Dialog_InventoryItemList(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;
	new
		itemid = Character_GetSlotItem(playerid, listitem)
		;
	if(itemid == 0)
	{
		return Dialog_Show(playerid, Dialog_InvEmptyItemAction, DIALOG_STYLE_LIST, "Slot Libero", "Deposita arma\nDisassembla arma", "Avanti", "Indietro");
	}
	new 
		extra = Character_GetSlotExtra(playerid, listitem),
		String:title,
		String:functions = @("Usa\nGetta\nDai a un giocatore");
	if(ServerItem_GetType(itemid) == ITEM_TYPE_WEAPON && extra > 0)
	{
		title = str_format("%s (Munizioni: %d)", ServerItem_GetName(itemid), extra);
		functions += @("\nDisassembla");
	}
	else
	{
		title = str_format("%s (Quantità: %d)", ServerItem_GetName(itemid), Character_GetSlotAmount(playerid, listitem));
	}
	Dialog_Show_s(playerid, Dialog_InvItemAction, DIALOG_STYLE_LIST, title, functions, "Continua", "Annulla");
	pSelectedListItem[playerid] = listitem;
	return 1;
}

Dialog:Dialog_InvEmptyItemAction(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Character_ShowInventory(playerid, playerid);
	if(listitem == 0) // /dep
	{
		pc_cmd_deposita(playerid, "");
	}
	else if(listitem == 1) // Dis
	{
		pc_cmd_disassembla(playerid, "");
	}
	Character_ShowInventory(playerid, playerid);
	return 1;
}


Dialog:Dialog_InvItemAction(playerid, response, listitem, inputtext[])
{
	if(!response)
	{
		return Character_ShowInventory(playerid, playerid);
	}
	new 
		slotid = pSelectedListItem[playerid],
		itemid = Character_GetSlotItem(playerid, slotid),
		extra = Character_GetSlotExtra(playerid, slotid);
	if(itemid == 0) // Safeness
		return 0;
	switch(listitem)
	{
		case 0:
		{
			Character_UseInventoryItem(playerid, slotid);
			return 1;
		}
		case 1: 
		{
			if(IsPlayerInAnyVehicle(playerid))
				return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando all'interno di un veicolo!");
			new String:title,
				String:content, 
				style
			;
			if(ServerItem_IsUnique(itemid))
			{
				if(ServerItem_GetType(itemid) == ITEM_TYPE_WEAPON && extra > 0)
					title = str_format("%s (Munizioni: %d)", ServerItem_GetName(itemid), extra);
				else
					title = str_format("%s", ServerItem_GetName(itemid));
				content = str_format("Sei sicuro di voler gettare {00FF00}%s{FFFFFF}?", ServerItem_GetName(itemid));
				style = DIALOG_STYLE_MSGBOX;
			}
			else
			{
				title = str_format("%s (Quantità: %d)", ServerItem_GetName(itemid), Character_GetSlotAmount(playerid, slotid));
				content = str_format("Inserisci la quantità di %s che vuoi gettare (1 di default).", ServerItem_GetName(itemid));
				style = DIALOG_STYLE_INPUT;
			}
			Dialog_Show_s(playerid, Dialog_InvItemDrop, style, title, content, "Getta", "Annulla");
			return 1;
		}
		case 2:
		{
			new String:title,
				String:content;
			if(ServerItem_IsUnique(itemid))
			{
				if(ServerItem_GetType(itemid) == ITEM_TYPE_WEAPON && extra > 0)
					title = str_format("%s (Munizioni: %d)", ServerItem_GetName(itemid), extra);
				else
					title = str_format("%s", ServerItem_GetName(itemid));
				content = @("Inserisci il nome o l'id del giocatore\na cui vuoi dare l'oggetto.\n{00FF00}Esempio:{FFFFFF} Mario Rossi");
			}
			else
			{
				title = str_format("%s (Quantità: %d)", ServerItem_GetName(itemid), Character_GetSlotAmount(playerid, slotid));
				content = @("Inserisci il nome o l'id del giocatore\nseguito dalla quantità.\n{00FF00}Esempio:{FFFFFF} Mario Rossi 5\n{00FF00}Oppure:{FFFFFF} Mario Rossi per dare solo 1.");
			}
			Dialog_Show_s(playerid, Dialog_InvItemGivePlayer, DIALOG_STYLE_INPUT, title, content, "Continua", "Annulla");
			return 1;
		}
		case 3:
		{
			if(ServerItem_GetType(itemid) == ITEM_TYPE_WEAPON && extra > 0)
			{
				new dest[8];
				valstr(dest, slotid);
				pc_cmd_disassembla(playerid, dest);
				return 1;
			}
		}
	}
	return 1;
}



Dialog:Dialog_InvItemDrop(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Character_ShowInventory(playerid, playerid);
	new amount = 0, 
		slotid = pSelectedListItem[playerid],
		itemid = Character_GetSlotItem(playerid, slotid),
		slotAmount = Character_GetSlotAmount(playerid, slotid);
	sscanf(inputtext, "D(1)", amount);
	if(amount <= 0 || amount > slotAmount)
	{
		new String:title = str_format("%s (Quantità: %d)", ServerItem_GetName(itemid), Character_GetSlotAmount(playerid, slotid));
		new String:content, style;
		if(ServerItem_IsUnique(itemid))
		{
		  content = str_format("Sei sicuro di voler gettare %s?", ServerItem_GetName(itemid));
		  style = DIALOG_STYLE_MSGBOX;
		}
		else
		{
		  content = str_format("{FF0000}La quantità inserita non è valida!\n{FFFFFF}Inserisci la quantità di %s che vuoi gettare (1 di default).", ServerItem_GetName(itemid));
		  style = DIALOG_STYLE_INPUT;
		}
		Dialog_Show_s(playerid, Dialog_InvItemDrop, style, title, content, "Getta", "Annulla");
		return 1;
	}
	return Character_DropItem(playerid, slotid, amount);
}

Dialog:Dialog_InvItemGivePlayer(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Character_ShowInventory(playerid, playerid);
	new 
		id, amount,
		slotid = pSelectedListItem[playerid],
		itemid = Character_GetSlotItem(playerid, slotid),
		slotAmount = Character_GetSlotAmount(playerid, slotid);
	if(itemid == 0) // Safeness
		return 0;
	if(sscanf(inputtext, "uD(1)", id, amount))
	{
		new String:title = str_format("%s (Quantità: %d)", ServerItem_GetName(itemid), Character_GetSlotAmount(playerid, listitem));
		Dialog_Show_s(playerid, Dialog_InvItemGivePlayer, DIALOG_STYLE_INPUT, title, @("{FF0000}Il giocatore non è connesso.\n{FFFFFF}Inserisci il nome o l'id del giocatore\nseguito dalla quantità.\nEsempio: Mario Rossi 5\nOppure: Mario Rossi per dare solo 1."), "Continua", "Annulla");
		return 1;
	}
	if(amount < 1 || amount > slotAmount)
	{
		new String:title = str_format("%s (Quantità: %d)", ServerItem_GetName(itemid), Character_GetSlotAmount(playerid, listitem));
		Dialog_Show_s(playerid, Dialog_InvItemGivePlayer, DIALOG_STYLE_INPUT, title, @("{FF0000}La quantità inserita non è valida.\n{FFFFFF}Inserisci il nome o l'id del giocatore\nseguito dalla quantità.\nEsempio: Mario Rossi 5\nOppure: Mario Rossi per dare solo 1."), "Continua", "Annulla");
		return 1;
	}
	if(!Character_GiveItemToPlayer(playerid, id, slotid, amount))
	{
		new String:title = str_format("%s (Quantità: %d)", ServerItem_GetName(itemid), Character_GetSlotAmount(playerid, listitem));
		Dialog_Show_s(playerid, Dialog_InvItemGivePlayer, DIALOG_STYLE_INPUT, title, @("{FF0000}Il giocatore non ha abbastanza spazio nell'inventario.\n{FFFFFF}Inserisci il nome o l'id del giocatore\nseguito dalla quantità.\nEsempio: Mario Rossi 5\nOppure: Mario Rossi per dare solo 1."), "Continua", "Annulla");
		return 1;
	}
	return 1;
}


stock Character_GetSlotItem(playerid, slotid)
{
	new Inventory:playerInv = Character_GetInventory(playerid);
	return Inventory_GetSlotItem(playerInv, slotid);
}

stock Character_GetSlotAmount(playerid, slotid)
{
	new Inventory:playerInv = Character_GetInventory(playerid);
	return Inventory_GetSlotAmount(playerInv, slotid);
}


stock Character_HasItem(playerid, itemid, min = 1) return Inventory_HasItem(Character_GetInventory(playerid), itemid, min);

stock Character_HasSpaceForItem(playerid, itemid, amount) return Inventory_HasSpaceForItem(Character_GetInventory(playerid), itemid, amount);

stock Character_HasBag(playerid) return pInventoryBag[playerid] != 0 && ServerItem_IsBag(pInventoryBag[playerid]);

stock Character_GetBag(playerid) return pInventoryBag[playerid];

stock bool:Character_SetBag(playerid, bagid)
{
	if(bagid != 0 && !ServerItem_IsBag(bagid))
		return false;
	pInventoryBag[playerid] = bagid;
	Character_ResizeInventory(playerid);
	return true;
}

stock Character_ResetInventory(playerid)
{
	pInventoryBag[playerid] = 0;
	return 1;
}

stock Character_SaveInventory(playerid)
{
	if(!Character_IsLogged(playerid))
		return 0;

	new
		Inventory:inventory = Character_GetInventory(playerid),
		query[1024],
		tempItems[128],
		tempAmounts[128],
		tempExtras[128]
		;
	
	if(Inventory_ParseForSave(inventory, tempItems, tempAmounts, tempExtras))
	{
		mysql_format(gMySQL, query, sizeof(query), "INSERT INTO `character_inventory` \
		(CharacterID, Items, ItemsAmount, ItemsExtraData, EquippedBag) VALUES('%d', '%e', '%e', '%e', '%d') ON DUPLICATE KEY UPDATE \
		Items = VALUES(Items), ItemsAmount = VALUES(ItemsAmount), ItemsExtraData = VALUES(ItemsExtraData), EquippedBag = VALUES(EquippedBag)",
		PlayerInfo[playerid][pID], tempItems, tempAmounts, tempExtras, pInventoryBag[playerid]
		);
		mysql_tquery(gMySQL, query);
	}
	return 1;
}

stock Character_LoadInventory(playerid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	inline OnLoad()
	{
		//Character_ResetInventory(playerid); // Probably useless.
		if(cache_num_rows() > 0)
		{
			new 
				temp[256], 
				tempItems[MAX_ITEMS_PER_PLAYER],
				tempAmounts[MAX_ITEMS_PER_PLAYER],
				tempExtraData[MAX_ITEMS_PER_PLAYER],
				sscanf_format[16];

			format(sscanf_format, sizeof(sscanf_format), "p<|>a<i>[%d]", MAX_ITEMS_PER_PLAYER);

			// Load Items Ids
			cache_get_value_index(0, 0, temp, sizeof(temp));
			sscanf(temp, sscanf_format, tempItems);
			// Load Items Amounts
			cache_get_value_index(0, 1, temp, sizeof(temp));
			sscanf(temp, sscanf_format, tempAmounts);

			cache_get_value_index(0, 2, temp, sizeof(temp));
			sscanf(temp, sscanf_format, tempExtraData);
			
			cache_get_value_index_int(0, 3, pInventoryBag[playerid]);

			Character_ResizeInventory(playerid);

			new Inventory:inventory = Character_GetInventory(playerid);
			for(new i = 0; i < Character_GetInventorySize(playerid); ++i)
			{
					if(!ServerItem_IsValid(tempItems[i]))
						continue;
					Inventory_SetItem(inventory, i, tempItems[i], tempAmounts[i], tempExtraData[i]);
					//Character_GiveItem(playerid, tempItems[i], tempAmounts[i], tempExtraData[i]);
			}

			if(Character_HasBag(playerid))
			{
				SendFormattedMessage(playerid, COLOR_GREEN, "Stai utilizzando: %s.", ServerItem_GetName(Character_GetBag(playerid)));
			}
		}
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT Items, ItemsAmount, ItemsExtraData, EquippedBag FROM `character_inventory` WHERE CharacterID = '%d'", PlayerInfo[playerid][pID]);
	return 1;
}

// Gives the item in the 'slotid' to 'giveTo' player.
stock Character_GiveItemToPlayer(playerid, giveTo, slotid, amount)
{
	if(!IsPlayerConnected(giveTo) || !Character_IsLogged(giveTo))
		return INVALID_PLAYER_ID;
	new itemid = Character_GetSlotItem(playerid, slotid),
		slotAmount = Character_GetSlotAmount(playerid, slotid),
		extra = Character_GetSlotExtra(playerid, slotid)
	;
	if(slotAmount < 1 || amount > slotAmount || !Character_HasSpaceForItem(giveTo, itemid, amount))
		return 0;
	Character_DecreaseSlotAmount(playerid, slotid, amount);
	Character_GiveItem(giveTo, itemid, amount, extra);
	if(ServerItem_GetType(itemid) == ITEM_TYPE_WEAPON)
	{
		SendFormattedMessage(playerid, COLOR_GREEN, "Hai dato l'arma (%s) con %d proiettili a %s (%d).", ServerItem_GetName(itemid), extra, Character_GetOOCName(giveTo), giveTo);
		SendFormattedMessage(giveTo, COLOR_GREEN, "%s (%d) ti ha dato un'arma (%s) con %d proiettili.", Character_GetOOCName(playerid), playerid, ServerItem_GetName(itemid), extra);
	}
	else
	{
		SendFormattedMessage(playerid, COLOR_GREEN, "Hai dato l'oggetto (%s, %d) a %s (%d).", ServerItem_GetName(itemid), amount, Character_GetOOCName(giveTo), giveTo);
		SendFormattedMessage(giveTo, COLOR_GREEN, "%s (%d) ti ha dato un oggetto (%s, %d).", Character_GetOOCName(playerid), playerid, ServerItem_GetName(itemid), amount);
	}
	Character_AMe(playerid, "prende degli oggetti e li da a %s", Character_GetOOCName(giveTo));
	return 1;
}

stock Character_DropItem(playerid, slotid, amount)
{
	if(PendingRequestInfo[playerid][rdPending])
		return 0;
	new 
		itemid = Character_GetSlotItem(playerid, slotid),
		slotAmount = Character_GetSlotAmount(playerid, slotid),
		extra = Character_GetSlotExtra(playerid, slotid);
	if(itemid == 0 || slotAmount == 0)
		return 0;
	if(amount < 1 || amount > slotAmount)
		return 0;
	new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	GetXYInFrontOfPlayer(playerid, x, y, 1.5);
	Drop_Create(x, y, z - 0.9, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), itemid, amount, extra, Character_GetOOCNameStr(playerid));
	
	ApplyAnimation(playerid, "GRENADE", "WEAPON_throwu", 3.0, 0, 0, 0, 0, 0);

	Character_DecreaseSlotAmount(playerid, slotid, amount);
	SendFormattedMessage(playerid, COLOR_GREEN, "Hai gettato: %s (%d)", ServerItem_GetName(itemid), amount);
	return 1;
}