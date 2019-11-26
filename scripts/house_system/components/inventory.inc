#include <YSI_Coding\y_hooks>

hook OnGameModeInit()
{
	mysql_pquery(gMySQL, "CREATE TABLE IF NOT EXISTS `house_inventory` ( \
							`house_id` int(11) NOT NULL, \
							`slot_id` tinyint(4) NOT NULL, \
							`item` int(11) NOT NULL, \
							`amount` int(11) NOT NULL, \
							`extra` int(11) NOT NULL, \
							PRIMARY KEY (`house_id`,`slot_id`), \
							CONSTRAINT `house_inventory` FOREIGN KEY (`house_id`) REFERENCES `houses` (`id`) ON DELETE CASCADE \
							) ENGINE=InnoDB DEFAULT CHARSET=latin1");

	return 1;
}

hook OnHouseLoaded(House:house)
{
	House_InitializeInventory(house);
	House_LoadInventory(house);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

stock Inventory:House_InitializeInventory(House:house)
{
	new Inventory:inv = Inventory_New(ENTITY_TYPE_HOUSE, House_GetID(house), 20);
	return inv;
}

stock Inventory:House_GetInventory(House:house)
{
	return Inventory_Get(ENTITY_TYPE_HOUSE, House_GetID(house));
}

stock House_SaveInventory(House:house)
{
	Inventory_SaveInDatabase(House_GetInventory(house), "house_inventory", "house_id", House_GetID(house));
	return 1;
}

stock House_LoadInventory(House:house)
{
	Inventory_LoadFromDatabase(House_GetInventory(house), "house_inventory", "house_id", House_GetID(house));
	return 1;
}

stock House_ShowInventory(House:house, playerid)
{
    if(!House_IsValid(house))
        return 0;
    
    new Inventory:houseInventory = House_GetInventory(house),
		Inventory:playerInventory = Character_GetInventory(playerid),
		String:info = str_new(""),
		String:title = str_format("Inventario Casa (%d/%d)", Inventory_GetUsedSpace(houseInventory), Inventory_GetSpace(houseInventory));
	
	pawn_guard(title);
	pawn_guard(info);

	new response[e_DIALOG_RESPONSE_INFO];

	yield 1;
	for(;;)
	{
		await_arr(response) Inventory_ShowAsyncDialog(playerid, houseInventory, title, @("Seleziona"), @("Chiudi"));

		if(!response[E_DIALOG_RESPONSE_Response])
			break;
		
		new slotid = response[E_DIALOG_RESPONSE_Listitem],
			itemid = Inventory_GetSlotItem(houseInventory, slotid);

		str_clear(info);
		str_append_format(info, "Deposita\n");
		
		if(ServerItem_IsValid(itemid))
			str_append_format(info, "Ritira\n");

		await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_LIST, @("Inventario Casa"), info, @("Seleziona"), @("Indietro"));

		if(!response[E_DIALOG_RESPONSE_Response])
			continue;

		if(response[E_DIALOG_RESPONSE_Listitem] == 0)
			HandleDepositInHouseDialog(playerid, playerInventory, house, houseInventory, slotid);
		else if(response[E_DIALOG_RESPONSE_Listitem] == 1)
			HandleWithdrawInHouseDialog(playerid, playerInventory, house, houseInventory, slotid);
	}

	return 1;
}

static HandleDepositInHouseDialog(playerid, Inventory:playerInventory, House:house, Inventory:houseInventory, slotid)
{
	new response[e_DIALOG_RESPONSE_INFO];
	new itemid = Inventory_GetSlotItem(houseInventory, slotid),
		String:error = str_new("");
	pawn_guard(error);

	// If selected itemid isn't equal to zero (so there's something)
	// First check if it's unique and if character has atleast one item.
	// It's used to unify first insert and concat
	if(itemid != 0)
	{
		if(ServerItem_IsUnique(itemid))
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi depositare ulteriori oggetti in questo slot.");
			return;
		}
		else if(!Character_HasItem(playerid, itemid))
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non hai quest'oggetto nell'inventario.");
			return;
		}
	}

	yield 1;
	for(;;)
	{
		await_arr(response) Inventory_ShowAsyncDialog(playerid, playerInventory, @("Inventario Giocatore - Seleziona l'oggetto da depositare"), @("Seleziona"), @("Indietro"));

		if(!response[E_DIALOG_RESPONSE_Response])
			break;
		
		new playerSlotId = response[E_DIALOG_RESPONSE_Listitem];
		new playerSelectedItem = Inventory_GetSlotItem(playerInventory, playerSlotId);
		new playerSelectedItemQuantity = Inventory_GetSlotAmount(playerInventory, playerSlotId);

		new quantityToDeposit = (ServerItem_IsUnique(playerSelectedItem) || playerSelectedItemQuantity == 1) ? 1 : 0;

		if(quantityToDeposit == 0)
		{
			yield 1;
			for(;;)
			{
				new String:info = str_format("Inserisci l'ammontare che vuoi depositare all'interno dell'inventario della tua casa.\nOggetto: %s.\nQuantità nello slot: %d.\nStack Massime: %d", ServerItem_GetName(playerSelectedItem), playerSelectedItemQuantity, ServerItem_GetMaxStack(playerSelectedItem));
				await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_INPUT, @("Quantità da depositare"), error + @("\n") + info, @("Deposita"), @("Indietro"));

				if(!response[E_DIALOG_RESPONSE_Response])
					break;
				
				str_clear(error);

				new val = strval(response[E_DIALOG_RESPONSE_InputText]);

				if(! (0 < val <= playerSelectedItemQuantity))
				{
					str_append_format(error, "{FF0000}Non possiedi la quantità inserita.\n{FFFFFF}");
					continue;
				}
				quantityToDeposit = val;
				break;
			}
		}
		
		if(quantityToDeposit > 0)
		{
			new bool:result = Inventory_TransferItem(playerInventory, playerSlotId, quantityToDeposit, houseInventory);
			if(result)
			{
				SendFormattedMessage(playerid, COLOR_ORANGE, "Hai depositato gli oggetti nell'inventario della tua casa. (Oggetto: %s - Quantità: %d)", ServerItem_GetName(itemid), quantityToDeposit);
				Character_SaveInventory(playerid);
				House_SaveInventory(house);
			}
			break;
		}
	}
}

static HandleWithdrawInHouseDialog(playerid, Inventory:playerInventory, House:house, Inventory:houseInventory, houseSlotId)
{
	new houseItemId = Inventory_GetSlotItem(houseInventory, houseSlotId),
		houseItemQuantity = Inventory_GetSlotAmount(houseInventory, houseSlotId);

	if(houseItemId == 0 || houseItemQuantity == 0)
		return;
	
	new quantityToWithdraw = (ServerItem_IsUnique(houseItemId) || houseItemQuantity == 1) ? 1 : 0;

	if(quantityToWithdraw == 0)
	{
		new response[e_DIALOG_RESPONSE_INFO],
			String:error = str_new("");
		pawn_guard(error);
		yield 1;
		for(;;)
		{
			new String:info = str_format("Inserisci l'ammontare che vuoi ritirare dell'oggetto dall'interno dell'inventario della tua casa.\nOggetto: %s.\nQuantità nello slot: %d.", ServerItem_GetName(houseItemId), houseItemQuantity);
			await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_INPUT, @("Quantità da depositare"), error + @("\n") + info, @("Ritira"), @("Indietro"));

			if(!response[E_DIALOG_RESPONSE_Response])
				break;
			
			str_clear(error);

			new val = strval(response[E_DIALOG_RESPONSE_InputText]);

			if(! (0 < val <= houseItemQuantity))
			{
				str_append_format(error, "{FF0000}La quantità inserita non è valida.\n{FFFFFF}");
				continue;
			}
			quantityToWithdraw = val;
			break;
		}
	}

	if(quantityToWithdraw > 0)
	{
		new bool:result = Inventory_TransferItem(houseInventory, houseSlotId, quantityToWithdraw, playerInventory);
		if(result)
		{
			Character_SaveInventory(playerid);
			House_SaveInventory(house);
			SendFormattedMessage(playerid, COLOR_ORANGE, "Hai ritirato gli oggetti dall'inventario della casa. (Oggetto: %s - Quantità: %d)", ServerItem_GetName(houseItemId), quantityToWithdraw);
		}
	}
}
