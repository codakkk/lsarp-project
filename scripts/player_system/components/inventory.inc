
#include <player_system\core>

#include <YSI_Coding\y_hooks>

static 
	Map:PlayerInventory, 
	pInventoryBag[MAX_PLAYERS],
	SelectedListItem[MAX_PLAYERS]
;
forward OnCharacterUseItem(playerid, slotid, itemid);
forward OnCharacterDropItem(playerid, slotid);
forward OnCharacterGiveItem(playerid, itemid, amount, extra, Map:tempExtra);

// #define Character_%0(%1, Inventory_%0(Character_GetInventory(%1), // It is a convenient way to call Inventory_Func with player inventory

hook OnGameModeInit()
{
	PlayerInventory = map_new();
	return 1;
}

stock bool:Character_InitializeInventory(playerid)
{
	//if(!Character_IsLogged(playerid))
		//return false;
	if(map_has_key(PlayerInventory, playerid))
	{
		map_remove_deep(PlayerInventory, playerid);
	}
	new Inventory:inventory = Inventory_New(PLAYER_INVENTORY_START_SIZE);
	map_add(PlayerInventory, playerid, List:inventory);
	return true;
}

stock bool:Character_UnloadInventory(playerid)
{
	if(!Character_IsLogged(playerid))
		return false;
	map_remove_deep(PlayerInventory, playerid);
	return true;
}

hook OnCharacterLoad(playerid)
{
	#if defined LSARP_DEBUG
		printf("player_inventory.pwn/OnCharacterLoad");
	#endif
	Character_InitializeInventory(playerid);
	Character_LoadInventory(playerid);
	return 1;
}

hook OnPlayerClearData(playerid)
{
	#if defined LSARP_DEBUG
		printf("player_inventory.pwn/OnPlayerClearData");
	#endif
	SelectedListItem[playerid] = -1;
	Character_ResetInventory(playerid);
	Character_UnloadInventory(playerid);
	return 1;
}


hook OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
	if(Character_IsFactionDuty(playerid))
		return 1;
	
	new ammo;
	Character_GetWeaponData(playerid, Weapon_GetSlot(weaponid), weaponid, ammo);

	if(Character_HasWeapon(playerid, weaponid) && ammo <= 1)
	{
		if(Character_HasSpaceForItem(playerid, weaponid, 1))
		{
			SendClientMessage(playerid, COLOR_GREEN, "Proiettili esauriti. L'arma è stata rimessa nell'inventario.");
			//Character_AMe(playerid, "ripone l'arma nell'inventario.");
			Character_GiveItem(playerid, weaponid, 1, 0, false);
			Character_RemoveWeapon(playerid, weaponid);
		}
		else
		{
			SendClientMessage(playerid, COLOR_ERROR, "Proiettili esauriti. Hai l'inventario pieno, quindi l'arma è stata gettata a terra.");
			Character_AMe(playerid, "getta un'arma a terra.");
			new Float:x, Float:y, Float:z;
			if(IsPlayerInAnyVehicle(playerid))
				GetVehiclePos(GetPlayerVehicleID(playerid), x, y, z);
			else 
			 	GetPlayerPos(playerid, x, y, z);
			new result = Drop_Create(Character_GetOOCNameStr(playerid), x, y, z - 0.9, Character_GetVirtualWorld(playerid), GetPlayerInterior(playerid), weaponid, 1, 0);
			if(result != -1)
			{
				Character_RemoveWeapon(playerid, weaponid);
			}
		}
		if(Request_IsPending(playerid, REQUEST_TYPE_WEAPON) && Request_GetSlot(playerid, REQUEST_TYPE_WEAPON) == Weapon_GetSlot(weaponid) && Request_GetItem(playerid, REQUEST_TYPE_WEAPON) == weaponid)
		{
			Request_Reset(playerid, REQUEST_TYPE_WEAPON);
			Request_Reset(Request_GetReceiver(playerid, REQUEST_TYPE_WEAPON), REQUEST_TYPE_WEAPON);
			SendClientMessage(playerid, COLOR_ERROR, "La richiesta è stata annullata poiché non possiedi più l'arma.");
			SendClientMessage(Request_GetReceiver(playerid, REQUEST_TYPE_WEAPON), COLOR_ERROR, "La richiesta ricevuta dal giocatore è stata annullata poiché non possiede più l'arma.");
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
stock Character_GiveItem(playerid, item_id, amount = 1, extra = 0, bool:callback = false, Map:tempExtra = Map:0)
{
	#pragma unused callback
	
	new Inventory:playerInventory = Character_GetInventory(playerid);
	
	new result = Inventory_AddItem(playerInventory, item_id, amount, extra, tempExtra);

	CallLocalFunction(#OnCharacterGiveItem, "ddddd", playerid, item_id, amount, extra, _:tempExtra);


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

stock Character_GetSlotExtra(playerid, slotid, index = 0)
{
	return Inventory_GetSlotExtra(Character_GetInventory(playerid), slotid, index);
}

stock Character_SetSlotExtra(playerid, slotid, index, value)
{
	Inventory_SetSlotExtra(Character_GetInventory(playerid), slotid, index, value);
	return 1;
}

stock Character_ShowInventory(playerid, targetid)
{
	if(!Character_IsLogged(playerid))
		return 0;

	new Inventory:playerInventory = Character_GetInventory(playerid);
	Inventory_ShowStr(playerInventory, targetid, str_format("Inventario Giocatore (%d/%d)", Inventory_GetUsedSpace(playerInventory), Inventory_GetSpace(playerInventory)), Dialog_InventoryItemList, "Avanti", "Chiudi");
	return 1;
}

stock Character_UseInventoryItem(playerid, slotid)
{
	CallLocalFunction(#OnCharacterUseItem, "ddd", playerid, slotid, Character_GetSlotItem(playerid, slotid));
	return 1;
}

Dialog:Dialog_InventoryItemList(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;
	if(Character_GetDeathState(playerid) > 0)
	{
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi interagire con l'inventario ora.");
		Character_ShowInventory(playerid, playerid);
		return 0;
	}
	new
		itemid = Character_GetSlotItem(playerid, listitem)
		;
	if(itemid == 0)
	{
		return Dialog_Show(playerid, Dialog_InvEmptyItemAction, DIALOG_STYLE_LIST, "Slot Libero", "Deposita arma\nDisassembla arma\nDeposita denaro", "Avanti", "Indietro");
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
	SelectedListItem[playerid] = listitem;
	return 1;
}

Dialog:Dialog_InvEmptyItemAction(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Character_ShowInventory(playerid, playerid);
	if(listitem == 0) // /dep
	{
		pc_cmd_depositaarma(playerid, "");
	}
	else if(listitem == 1) // Dis
	{
		pc_cmd_disassembla(playerid, "");
	}
	else if(listitem == 2)
	{
		return Dialog_Show(playerid, Dialog_DepositInvMoney, DIALOG_STYLE_INPUT, "Deposita denaro", "Inserisci la quantità di denaro che vuoi depositare\nall'interno del tuo inventario.", "Deposita", "Chiudi");
	}
	Character_ShowInventory(playerid, playerid);
	return 1;
}

Dialog:Dialog_DepositInvMoney(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;
	new val = strval(inputtext);
	if(val <= 0 || val > Character_GetMoney(playerid))
		return Dialog_Show(playerid, Dialog_DepositInvMoney, DIALOG_STYLE_INPUT, "Deposita denaro", "{FF0000}Non possiedi la quantità inserita.{FFFFFF}\nInserisci la quantità di denaro che vuoi depositare\nall'interno del tuo inventario.", "Deposita", "Chiudi");
	Character_DepositMoneyInInv(playerid, val);
	return 1;
}

stock Character_DepositMoneyInInv(playerid, money)
{
	Character_GiveMoney(playerid, -money, "deposit_inv_money");
	Character_GiveItem(playerid, gItem_Money, money, 0, true);
	Player_InfoStr(playerid, str_format("Hai depositato: ~g~$~w~%d", money), true, 2500);
	return 1;
}

Dialog:Dialog_InvItemAction(playerid, response, listitem, inputtext[])
{
	if(!response)
	{
		return Character_ShowInventory(playerid, playerid);
	}
	new 
		slotid = SelectedListItem[playerid],
		itemid = Character_GetSlotItem(playerid, slotid),
		amount = Character_GetSlotAmount(playerid, slotid),
		extra = Character_GetSlotExtra(playerid, slotid);
	if(itemid == 0) // Safeness
		return printf("itemid == 0"); // pls give me assert
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
				return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando all'interno di un veicolo.");
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
				title = str_format("%s (%d)", ServerItem_GetName(itemid), amount);
				content = str_format("Inserisci la quantità di %s che vuoi gettare (1 di default).", ServerItem_GetName(itemid));
				style = DIALOG_STYLE_INPUT;
			}
			Dialog_Show_s(playerid, Dialog_InvItemDrop, style, title, content, "Getta", "Annulla");
			return 1;
		}
		case 2:
		{
			new String:title = Item_GetDialogTitle(itemid, amount, extra),
				String:content;
			content = (ServerItem_IsUnique(itemid) ? 
						@("Inserisci il nome o l'id del giocatore\na cui vuoi dare l'oggetto.\n{00FF00}Esempio:{FFFFFF} Mario Rossi")
					:
						@("Inserisci il nome o l'id del giocatore\nseguito dalla quantità.\n{00FF00}Esempio:{FFFFFF} Mario Rossi 5\nOppure: {00FF00}Mario Rossi{FFFFFF} per dare solo 1."));
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
		slotid = SelectedListItem[playerid],
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
	//if(!Character_DropItem(playerid, slotid, amount))
		//return SendClientMessage(playerid, COLOR_ERROR, "Slot non valida o quantità non valida.");
	//new str[20];
	//format(str, sizeof(str), "%d %d", slotid, amount);
	//pc_cmd_getta(playerid, str);
	Character_DropItem(playerid, slotid, amount);
	return 1;
}

Dialog:Dialog_InvItemGivePlayer(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Character_ShowInventory(playerid, playerid);
	new 
		id, amount,
		slotid = SelectedListItem[playerid],
		itemid = Character_GetSlotItem(playerid, slotid),
		extra = Character_GetSlotExtra(playerid, slotid),
		slotAmount = Character_GetSlotAmount(playerid, slotid);
	if(itemid == 0) // Safeness
		return 0;
	if(sscanf(inputtext, "k<u>D(1)", id, amount))
	{
		new String:title = Item_GetDialogTitle(itemid, slotAmount, extra),
			String:content;
		content = (ServerItem_IsUnique(itemid) ? 
					@("Inserisci il nome o l'id del giocatore\na cui vuoi dare l'oggetto.\n{00FF00}Esempio:{FFFFFF} Mario Rossi")
				:
					@("Inserisci il nome o l'id del giocatore\nseguito dalla quantità.\n{00FF00}Esempio:{FFFFFF} Mario Rossi 5\nOppure: {00FF00}Mario Rossi{FFFFFF} per dare solo 1."));
		Dialog_Show_s(playerid, Dialog_InvItemGivePlayer, DIALOG_STYLE_INPUT, title, @("{FF0000}I dati inseriti non sono validi.{FFFFFF}\n") % content, "Continua", "Annulla");
		return 1;
	}
	if(amount < 1 || amount > slotAmount)
	{
		new String:title = Item_GetDialogTitle(itemid, slotAmount, extra),
			String:content;
		content = (ServerItem_IsUnique(itemid) ? 
					@("Inserisci il nome o l'id del giocatore\na cui vuoi dare l'oggetto.\n{00FF00}Esempio:{FFFFFF} Mario Rossi")
				:
					@("Inserisci il nome o l'id del giocatore\nseguito dalla quantità.\n{00FF00}Esempio:{FFFFFF} Mario Rossi 5\nOppure: {00FF00}Mario Rossi{FFFFFF} per dare solo 1."));
		Dialog_Show_s(playerid, Dialog_InvItemGivePlayer, DIALOG_STYLE_INPUT, title, @("{FF0000}La quantità inserita non è valida.{FFFFFF}\n") % content, "Continua", "Annulla");
		return 1;
	}
	if(!Character_CanInteract(playerid))
	{
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi passare un oggetto in questo momento.");
		return 0;
	}
	Character_SendItemRequest(playerid, id, slotid, amount);
	/*new p[32];
	format(p, sizeof(p), "%d %d %d", id, slotid, amount);
	pc_cmd_passa(playerid, p);*/
	return 1;
}

stock String:Item_GetDialogTitle(itemid, amount, extra)
{
	new String:title;
	if(ServerItem_IsUnique(itemid))
	{
		if(ServerItem_GetType(itemid) == ITEM_TYPE_WEAPON && extra > 0)
			title = str_format("%s (Munizioni: %d)", ServerItem_GetName(itemid), extra);
		else
			title = str_format("%s", ServerItem_GetName(itemid));
	}
	else
	{
		title = str_format("%s (%d)", ServerItem_GetName(itemid), amount);
	}
	return title;
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

stock Character_GetItemAmount(playerid, itemid)
{
	new Inventory:playerInv = Character_GetInventory(playerid);
	return Inventory_GetItemAmount(playerInv, itemid);
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
	Character_SetBag(playerid, 0);
	return 1;
}

stock Character_SaveInventory(playerid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	Inventory_SaveInDatabase(Character_GetInventory(playerid), "character_inventory", "character_id", Character_GetID(playerid));
	return 1;
}

stock Character_LoadInventory(playerid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	Character_ResizeInventory(playerid);
	Inventory_LoadFromDatabase(Character_GetInventory(playerid), "character_inventory", "character_id", Character_GetID(playerid));
	return 1;
}

// Gives the item in the 'slotid' to 'giveTo' player.
stock Character_GiveItemToPlayer(playerid, giveTo, slotid, amount)
{
	if(!IsPlayerConnected(giveTo) || !Character_IsLogged(giveTo))
		return INVALID_PLAYER_ID;
	new
		itemid = Character_GetSlotItem(playerid, slotid),
		slotAmount = Inventory_GetItemAmount(Character_GetInventory(playerid), itemid),
		extra = Character_GetSlotExtra(playerid, slotid)
	;
	if(!Character_HasItem(playerid, itemid, amount) || slotAmount < 1 || amount > slotAmount || !Character_HasSpaceForItem(giveTo, itemid, amount))
		return 0;
	Character_DecreaseItemAmount(playerid, itemid, amount);
	Character_GiveItem(giveTo, itemid, amount, extra);
	if(ServerItem_GetType(itemid) == ITEM_TYPE_WEAPON)
	{
		SendFormattedMessage(playerid, COLOR_GREEN, "Hai dato l'arma (%s) con %d proiettili a %s.", ServerItem_GetName(itemid), extra, Character_GetRolePlayName(giveTo));
		SendFormattedMessage(giveTo, COLOR_GREEN, "%s ti ha dato un'arma (%s) con %d proiettili.", Character_GetRolePlayName(playerid), ServerItem_GetName(itemid), extra);
	}
	else
	{
		SendFormattedMessage(playerid, COLOR_GREEN, "Hai dato l'oggetto (%s, %d) a %s.", ServerItem_GetName(itemid), amount, Character_GetRolePlayName(giveTo));
		SendFormattedMessage(giveTo, COLOR_GREEN, "%s ti ha dato un oggetto (%s, %d).", Character_GetRolePlayName(playerid), ServerItem_GetName(itemid), amount);
	}
	Character_AMe(playerid, "prende degli oggetti e li da a %s.", Character_GetRolePlayName(giveTo));
	return 1;
}

stock Character_DropItem(playerid, slotid, amount)
{
	if(Request_IsPending(playerid, REQUEST_TYPE_ITEM))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi gettare un oggetto se hai una richiesta attiva.");
	if(IsPlayerInAnyVehicle(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando all'interno di un veicolo.");
	new 
		itemid = Character_GetSlotItem(playerid, slotid),
		slotAmount = Character_GetSlotAmount(playerid, slotid),
		extra = Character_GetSlotExtra(playerid, slotid);
	if(itemid == 0 || slotAmount == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Oggetto non valido o non hai abbastanza quantità.");
	if(amount < 1 || amount > slotAmount)
		return SendClientMessage(playerid, COLOR_ERROR, "La quantità inserita non è valida.");
	new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	GetXYInFrontOfPlayer(playerid, x, y, 1.5);
	new result = Drop_Create(Character_GetOOCNameStr(playerid), x, y, z - 0.9, Character_GetVirtualWorld(playerid), GetPlayerInterior(playerid), itemid, amount, extra, Inventory_GetSlotTempExtraMap(Character_GetInventory(playerid), slotid));
	if(result != -1)
	{
		ApplyAnimation(playerid, "GRENADE", "WEAPON_throwu", 3.0, 0, 0, 0, 0, 0);

		CallLocalFunction(#OnCharacterDropItem, "dd", playerid, slotid);

		SendFormattedMessage(playerid, COLOR_GREEN, "Hai gettato: %s (%d)", ServerItem_GetName(itemid), amount);
		Character_AMe(playerid, "getta degli oggetti a terra.");

		Character_DecreaseSlotAmount(playerid, slotid, amount);
	}
	
	return 1;
}

stock Character_SendItemRequest(playerid, toPlayerId, slotid, amount)
{
	if(!IsPlayerConnected(toPlayerId) || !Character_IsLogged(toPlayerId))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è connesso.");

	if(playerid == toPlayerId)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi inviare la richiesta a te stesso.");

	if(!IsPlayerInRangeOfPlayer(playerid, toPlayerId, 2.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");

	new itemid = Character_GetSlotItem(playerid, slotid);

	if(itemid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Slot non valida.");

	new slotAmount = Character_GetSlotAmount(playerid, slotid);

	if(amount < 1 || amount > slotAmount)
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai la quantità necessaria.");
	
	if(Request_IsPending(toPlayerId, REQUEST_TYPE_ITEM))
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore ha già una richiesta attiva.");

	if(Request_IsPending(playerid, REQUEST_TYPE_ITEM))
		return SendClientMessage(playerid, COLOR_ERROR, "Hai già una richiesta attiva.");

	if(!Character_IsAlive(toPlayerId))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su questo giocatore.");
	
	if(itemid == gItem_Phone && Character_IsInCall(toPlayerId) && Phone_IsBusy(Character_GetInventory(playerid), slotid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi inviare la richiesta se il giocatore è già al telefono.");

	Character_SetRequest(playerid, toPlayerId, REQUEST_TYPE_ITEM, itemid, amount, slotid);
	
	SendFormattedMessage(playerid, COLOR_SEND_REQUEST, "Hai proposto di dare l'oggetto (%s, Quantità: %d) a %s.", ServerItem_GetName(itemid), amount, Character_GetRolePlayName(toPlayerId));
	SendClientMessage(playerid, COLOR_SEND_REQUEST, "Ricorda che puoi utilizzare \"annulla oggetto\" per annullare.");
	SendFormattedMessage(toPlayerId, COLOR_RECEIVE_REQUEST, "%s vuole darti un oggetto (%s, Quantità: %d).", Character_GetRolePlayName(playerid), ServerItem_GetName(itemid), amount);
	SendClientMessage(toPlayerId, COLOR_RECEIVE_REQUEST, "Digita \"/accetta oggetto\" per accettare oppure \"/rifiuta oggetto\" per rifiutare.");
	return 1;
}