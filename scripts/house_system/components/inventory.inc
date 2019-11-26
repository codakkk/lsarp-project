#include <YSI_Coding\y_hooks>

static enum _:E_DIALOG_HOUSE_DATA
{
    House:House,
    Inventory:Inventory,
    SlotID,
	ItemToDeposit
}

static 
	Map:PlayerHouseInventory; // <playerid, E_DIALOG_HOUSE_DATA>

hook OnGameModeInit()
{
	PlayerHouseInventory = map_new();

	mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `house_inventory` ( \
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
    
    new Inventory:inventory = House_GetInventory(house);
	
	if(map_has_key(PlayerHouseInventory, playerid))
	{
		map_set_cell(PlayerHouseInventory, playerid, House, _:house);
		map_set_cell(PlayerHouseInventory, playerid, Inventory, List:inventory);
		map_set_cell(PlayerHouseInventory, playerid, SlotID, -1);
	}
	else
	{
		new data[E_DIALOG_HOUSE_DATA];
		data[House] = house;
		data[Inventory] = inventory;
		data[SlotID] = -1;
		map_add_arr(PlayerHouseInventory, playerid, data);
	}
	Inventory_ShowStr(inventory, playerid, str_format("Inventario Casa (%d/%d)", Inventory_GetUsedSpace(inventory), Inventory_GetSpace(inventory)), Dialog_HouseInventory, "Avanti", "Chiudi", .forceDialog = true);
	return 1;
}

Dialog:Dialog_HouseInventory(playerid, response, listitem, inputtext[])
{
	if(!response) return 1;
    
    Internal_ShowHouseSlotAction(playerid, listitem);
	return 1;
}

Dialog:HouseInventoryAction(playerid, response, listitem, inputtext[])
{
    if(!response)
        return House_ShowInventory(Internal_GetHouse(playerid), playerid);
    new 
		slotid = Internal_GetSlotID(playerid),
		itemid = Inventory_GetSlotItem(Internal_GetInventory(playerid), slotid),
		amount = Inventory_GetSlotAmount(Internal_GetInventory(playerid), itemid)
	;

	if(listitem == 0)
	{
		if(itemid == 0)
		{
			Inventory_Show(Character_GetInventory(playerid), playerid, "Seleziona l'oggetto da depositare", HouseInventoryDepSelectItem, "Seleziona", "Indietro", .forceDialog = true);
		}
		else
		{
			if(ServerItem_IsUnique(itemid))
				return SendClientMessage(playerid, COLOR_ERROR, "Non puoi depositare ulteriori oggetti in questo slot.");
			
			if(!Character_HasItem(playerid, itemid))
				return SendClientMessage(playerid, COLOR_ERROR, "Non hai quest'oggetto nell'inventario.");

			map_set_cell(PlayerHouseInventory, playerid, ItemToDeposit, itemid);

			Dialog_Show_s(playerid, HouseInventoryDepositAmount, DIALOG_STYLE_INPUT, @("Quantità da depositare"), str_format("Inserisci l'ammontare che vuoi depositare all'interno dell'inventario della tua casa.\nOggetto: %s.\nQuantità: %d.\nStack Massime: %d", ServerItem_GetName(itemid), Character_GetItemAmount(playerid, itemid), ServerItem_GetMaxStack(itemid)), "Deposita", "Indietro");	
		}
	}
	else if(listitem == 1)
	{
		if(ServerItem_IsUnique(itemid) || amount == 1)
		{
			if(!Character_HasSpaceForItem(playerid, itemid, 1))
				return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario."), Internal_ShowHouseSlotAction(playerid, Internal_GetSlotID(playerid));
			new Inventory:invRef = Internal_GetInventory(playerid);

			Character_GiveItem(playerid, itemid, 1, Inventory_GetSlotExtra(invRef, slotid, 0), true, Inventory_GetSlotTempExtraMap(invRef, slotid));
			Inventory_DecreaseSlotAmount(invRef, slotid, 1);

			Character_SaveInventory(playerid);
			House_SaveInventory(Internal_GetHouse(playerid));

			SendFormattedMessage(playerid, COLOR_ORANGE, "Hai ritirato gli oggetti dall'inventario della casa. (Oggetto: %s - Quantità: 1)", ServerItem_GetName(itemid));
			return House_ShowInventory(Internal_GetHouse(playerid), playerid);
		}
		else
		{
			Dialog_Show_s(playerid, HouseInvWithdrawAmount, DIALOG_STYLE_INPUT, @("Ammontare da ritirare"), str_format("Inserisci l'ammontare che vuoi ritirare.\nOggetto: %s\nQuantità disponibile: %d", ServerItem_GetName(itemid), amount), "Ritira", "Indietro");
		}
	}
    return 1;
}

Dialog:HouseInvWithdrawAmount(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Internal_ShowHouseSlotAction(playerid, Internal_GetSlotID(playerid));
	
	new 
		val = strval(inputtext),
		slotid = Internal_GetSlotID(playerid),
		Inventory:inventoryRef = Internal_GetInventory(playerid),
		itemid = Inventory_GetSlotItem(inventoryRef, slotid),
		slotAmount = Inventory_GetSlotAmount(inventoryRef, slotid)
		;

	if(! (0 < val <= slotAmount))
		return Dialog_Show_s(playerid, HouseInvWithdrawAmount, DIALOG_STYLE_INPUT, @("Ammontare da ritirare"), str_format("{FF0000}Ammontare da ritirare non valido.{FFFFFF}\nInserisci l'ammontare che vuoi ritirare.\nOggetto: %s\nQuantità disponibile: %d", ServerItem_GetName(itemid), slotAmount), "Ritira", "Indietro");

	if(!Character_HasSpaceForItem(playerid, itemid, val))
		return Dialog_Show_s(playerid, HouseInvWithdrawAmount, DIALOG_STYLE_INPUT, @("Ammontare da ritirare"), str_format("{FF0000}Non hai abbastanza spazio nell'inventario.{FFFFFF}\nInserisci l'ammontare che vuoi ritirare.\nOggetto: %s\nQuantità disponibile: %d", ServerItem_GetName(itemid), slotAmount), "Ritira", "Indietro");

	Character_GiveItem(playerid, itemid, val, Inventory_GetSlotExtra(inventoryRef, slotid, 0), true, Inventory_GetSlotTempExtraMap(inventoryRef, slotid));
	Inventory_DecreaseSlotAmount(inventoryRef, slotid, val);

	Character_SaveInventory(playerid);
	House_SaveInventory(Internal_GetHouse(playerid));

	House_ShowInventory(Internal_GetHouse(playerid), playerid);
	return 1;
}


Dialog:HouseInventoryDepSelectItem(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Internal_ShowHouseSlotAction(playerid, Internal_GetSlotID(playerid));
	
	new 
		Inventory:inventoryRef = Internal_GetInventory(playerid),
		itemid = Character_GetSlotItem(playerid, listitem),
		amount = Character_GetSlotAmount(playerid, listitem),
		extra = Character_GetSlotExtra(playerid, listitem, 0),
		Map:tempExtra = Inventory_GetSlotTempExtraMap(Character_GetInventory(playerid), listitem)
	;
	if(ServerItem_IsUnique(itemid) || amount == 1)
	{
		if(!Inventory_HasSpaceForItem(Internal_GetInventory(playerid), itemid, amount))
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario della casa.");
			return Inventory_Show(Character_GetInventory(playerid), playerid, "Seleziona l'oggetto da depositare", HouseInventoryDepSelectItem, "Seleziona", "Indietro", .forceDialog = true);
		}
		Inventory_AddItem(inventoryRef, itemid, amount, extra, tempExtra);
		Character_DecreaseSlotAmount(playerid, listitem, amount);

		Character_SaveInventory(playerid);
		House_SaveInventory(Internal_GetHouse(playerid));

		SendFormattedMessage(playerid, COLOR_ORANGE, "Hai depositato gli oggetti nell'inventario della tua casa. (Oggetto: %s - Quantità: %d)", ServerItem_GetName(itemid), amount);
		House_ShowInventory(Internal_GetHouse(playerid), playerid);
	}
	else
	{
		map_set_cell(PlayerHouseInventory, playerid, ItemToDeposit, itemid);
		Dialog_Show_s(playerid, HouseInventoryDepositAmount, DIALOG_STYLE_INPUT, @("Quantità da depositare"), str_format("Inserisci l'ammontare che vuoi depositare all'interno dell'inventario della tua casa.\nOggetto: %s.\nQuantità: %d.\nStack Massime: %d", ServerItem_GetName(itemid), Character_GetItemAmount(playerid, itemid), ServerItem_GetMaxStack(itemid)), "Deposita", "Indietro");	
	}
	return 1;
}


Dialog:HouseInventoryDepositAmount(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Internal_ShowHouseSlotAction(playerid, Internal_GetSlotID(playerid));
	new 
		val = strval(inputtext),
		Inventory:inventoryRef = Internal_GetInventory(playerid),
		itemid = Internal_GetItemToDeposit(playerid),
		playerAmount = Character_GetItemAmount(playerid, itemid),
		itemMaxStacks = ServerItem_GetMaxStack(itemid),
		extra = Character_GetSlotExtra(playerid, Internal_GetSlotID(playerid), 0),
		Map:tempExtra = Inventory_GetSlotTempExtraMap(Character_GetInventory(playerid), Internal_GetSlotID(playerid))
	;
	if(! (0 < val <= playerAmount))
		return Dialog_Show_s(playerid, HouseInventoryDepositAmount, DIALOG_STYLE_INPUT, @("Quantità da depositare"), str_format("{FF0000}Non possiedi l'ammontare inserito.{FFFFFF}\nInserisci l'ammontare che vuoi depositare all'interno dell'inventario della tua casa.\nOggetto: %s.\nQuantità: %d.\nStack Massime: %d", ServerItem_GetName(itemid), playerAmount, itemMaxStacks), "Deposita", "Indietro");	
	
	if(!Inventory_HasSpaceForItem(inventoryRef, itemid, val))
		return Dialog_Show_s(playerid, HouseInventoryDepositAmount, DIALOG_STYLE_INPUT, @("Quantità da depositare"), str_format("{FF0000}La casa non ha abbastanza spazio.{FFFFFF}\nInserisci l'ammontare che vuoi depositare all'interno dell'inventario della tua casa.\nOggetto: %s.\nQuantità: %d.\nStack Massime: %d", ServerItem_GetName(itemid), playerAmount, itemMaxStacks), "Deposita", "Indietro");	

	Inventory_AddItem(inventoryRef, itemid, val, extra, tempExtra);
	Character_DecreaseSlotAmount(playerid, Internal_GetSlotID(playerid), val);
	
	Character_SaveInventory(playerid);
	House_SaveInventory(Internal_GetHouse(playerid));

	SendFormattedMessage(playerid, COLOR_ORANGE, "Hai depositato gli oggetti nell'inventario della tua casa. (Oggetto: %s - Quantità: %d)", ServerItem_GetName(itemid), val);
	
	House_ShowInventory(Internal_GetHouse(playerid), playerid);
	return 1;
}

static Internal_ShowHouseSlotAction(playerid, slotid)
{
	if(slotid == -1)
		return 0;
	
	new itemid = Inventory_GetSlotItem(Internal_GetInventory(playerid), slotid);    
	
	map_set_cell(PlayerHouseInventory, playerid, SlotID, slotid);
    
	if(itemid == 0)
    {
        Dialog_Show(playerid, HouseInventoryAction, DIALOG_STYLE_LIST, "Inventario Casa", "Deposita", "Seleziona", "Indietro");
    }
    else
    {
        Dialog_Show(playerid, HouseInventoryAction, DIALOG_STYLE_LIST, "Inventario Casa", "Deposita\nRitira", "Seleziona", "Indietro");
    }
	return 1;
}

static House:Internal_GetHouse(playerid)
{
	if(!map_has_key(PlayerHouseInventory, playerid))
		return INVALID_HOUSE;
	return House:map_get(PlayerHouseInventory, playerid, House);
}

static Inventory:Internal_GetInventory(playerid)
{
	if(!map_has_key(PlayerHouseInventory, playerid))
		return INVENTORY_NULL;
	return Inventory:map_get(PlayerHouseInventory, playerid, Inventory);
}

static Internal_GetSlotID(playerid)
{
	if(!map_has_key(PlayerHouseInventory, playerid))
		return -1;
	return map_get(PlayerHouseInventory, playerid, SlotID);
}

static Internal_GetItemToDeposit(playerid)
{
	if(!map_has_key(PlayerHouseInventory, playerid))
		return 0;
	return map_get(PlayerHouseInventory, playerid, ItemToDeposit);
}