
#include <YSI_Coding\y_hooks>

static enum _:E_DIALOG_HOUSE_DATA
{
    Building:Building,
    Inventory:Inventory,
    SlotID,
	ItemToDeposit
}

static
    Map:BuildingInventory,
    Map:PlayerBuildingInventory; // <playerid, E_DIALOG_HOUSE_DATA>

hook OnGameModeInit()
{
    BuildingInventory = map_new();
	PlayerBuildingInventory = map_new();

    mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `building_inventory` ( \
                        `building_id` int(11) NOT NULL, \
                        `slot_id` tinyint(4) NOT NULL, \
                        `item` int(11) NOT NULL, \
                        `amount` int(11) NOT NULL, \
                        `extra` int(11) NOT NULL, \
                        PRIMARY KEY (`building_id`,`slot_id`), \
                        CONSTRAINT `building_inventory` FOREIGN KEY (`building_id`) REFERENCES `buildings` (`id`) ON DELETE CASCADE \
                        ) ENGINE=InnoDB DEFAULT CHARSET=latin1");
	return 1;
}

hook OnBuildingLoaded(Building:building)
{
	Building_InitializeInventory(building);
	Building_LoadInventory(building);
	return 1;
}

static Inventory:Building_InitializeInventory(Building:building)
{
    if(map_has_key(BuildingInventory, Building_GetID(building)))
		return Inventory:0;
	new Inventory:inv = Inventory_New(30);
	map_add(BuildingInventory, Building_GetID(building), List:inv);
	return inv;
}

stock Inventory:Building_GetInventory(Building:building)
{
	new Inventory:inv;
	if(map_get_safe(BuildingInventory, Building_GetID(building), List:inv))
	{
		return inv;
	}
	return Inventory:0;
}

stock Building_SaveInventory(Building:building)
{
	Inventory_SaveInDatabase(Building_GetInventory(building), "building_inventory", "building_id", Building_GetID(building));
	return 1;
}

static Building_LoadInventory(Building:building)
{
    Inventory_LoadFromDatabase(Building_GetInventory(building), "building_inventory", "building_id", Building_GetID(building));
    return 1;
}

stock Building_ShowInventory(Building:building, playerid)
{
    if(!Building_IsValid(building))
        return 0;
    
    new Inventory:inventory = Building_GetInventory(building);
	
	if(map_has_key(PlayerBuildingInventory, playerid))
	{
		map_set_cell(PlayerBuildingInventory, playerid, Building, _:building);
		map_set_cell(PlayerBuildingInventory, playerid, Inventory, List:inventory);
		map_set_cell(PlayerBuildingInventory, playerid, SlotID, -1);
	}
	else
	{
		new data[E_DIALOG_HOUSE_DATA];
		data[Building] = building;
		data[Inventory] = inventory;
		data[SlotID] = -1;
		map_add_arr(PlayerBuildingInventory, playerid, data);
	}
	Inventory_ShowStr(inventory, playerid, str_format("Inventario Edificio (%d/%d)", Inventory_GetUsedSpace(inventory), Inventory_GetSpace(inventory)), Dialog_BizInventory, "Avanti", "Chiudi");
	return 1;
}

Dialog:Dialog_BizInventory(playerid, response, listitem, inputtext[])
{
	if(!response)
        return 0;
    
    Internal_ShowBuildingSlotAction(playerid, listitem);
	return 1;
}

Dialog:BuildingInventoryAction(playerid, response, listitem, inputtext[])
{
	new Building:building = Internal_GetBuilding(playerid);
    if(!response)
        return Building_ShowInventory(building, playerid);
    new 
		slotid = Internal_GetSlotID(playerid),
		itemid = Inventory_GetSlotItem(Internal_GetInventory(playerid), slotid),
		amount = Inventory_GetSlotAmount(Internal_GetInventory(playerid), itemid)
	;

	if(listitem == 0)
	{
		if(itemid == 0)
		{
			Inventory_Show(Character_GetInventory(playerid), playerid, "Seleziona l'oggetto da depositare", BuildingInvDepSelectItem, "Seleziona", "Indietro");
		}
		else
		{
			if(ServerItem_IsUnique(itemid))
				return SendClientMessage(playerid, COLOR_ERROR, "Non puoi depositare ulteriori oggetti in questo slot.");
			
			if(!Character_HasItem(playerid, itemid))
				return SendClientMessage(playerid, COLOR_ERROR, "Non hai quest'oggetto nell'inventario.");

			map_set_cell(PlayerBuildingInventory, playerid, ItemToDeposit, itemid);

			Dialog_Show_s(playerid, BuildingInvDepositAmount, DIALOG_STYLE_INPUT, @("Quantità da depositare"), str_format("Inserisci l'ammontare che vuoi depositare all'interno dell'inventario del tuo edificio.\nOggetto: %s.\nQuantità: %d.\nStack Massime: %d", ServerItem_GetName(itemid), Character_GetItemAmount(playerid, itemid), ServerItem_GetMaxStack(itemid)), "Deposita", "Indietro");	
		}
	}
	else if(listitem == 1)
	{
		if(ServerItem_IsUnique(itemid) || amount == 1)
		{
			if(!Character_HasSpaceForItem(playerid, itemid, 1))
				return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario."), Internal_ShowBuildingSlotAction(playerid, Internal_GetSlotID(playerid));
			
			Inventory_DecreaseSlotAmount(Internal_GetInventory(playerid), slotid, 1);
			Character_GiveItem(playerid, itemid, 1, 0, true);

			Character_SaveInventory(playerid);
			Building_SaveInventory(building);

			SendFormattedMessage(playerid, COLOR_ORANGE, "Hai ritirato gli oggetti dall'inventario dell'edificio. (Oggetto: %s - Quantità: 1)", ServerItem_GetName(itemid));
			return Building_ShowInventory(building, playerid);
		}
		else
		{
			Dialog_Show_s(playerid, BuildingInvWithdrawAmount, DIALOG_STYLE_INPUT, @("Ammontare da ritirare"), str_format("Inserisci l'ammontare che vuoi ritirare.\nOggetto: %s\nQuantità disponibile: %d", ServerItem_GetName(itemid), amount), "Ritira", "Indietro");
		}
	}
    return 1;
}

Dialog:BuildingInvWithdrawAmount(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Internal_ShowBuildingSlotAction(playerid, Internal_GetSlotID(playerid));
	
	new 
		val = strval(inputtext),
		slotid = Internal_GetSlotID(playerid),
		Inventory:inventoryRef = Internal_GetInventory(playerid),
		itemid = Inventory_GetSlotItem(inventoryRef, slotid),
		slotAmount = Inventory_GetSlotAmount(inventoryRef, slotid)
		;

	if(! (0 < val <= slotAmount))
		return Dialog_Show_s(playerid, BuildingInvWithdrawAmount, DIALOG_STYLE_INPUT, @("Ammontare da ritirare"), str_format("{FF0000}Ammontare da ritirare non valido.{FFFFFF}\nInserisci l'ammontare che vuoi ritirare.\nOggetto: %s\nQuantità disponibile: %d", ServerItem_GetName(itemid), slotAmount), "Ritira", "Indietro");

	if(!Character_HasSpaceForItem(playerid, itemid, val))
		return Dialog_Show_s(playerid, BuildingInvWithdrawAmount, DIALOG_STYLE_INPUT, @("Ammontare da ritirare"), str_format("{FF0000}Non hai abbastanza spazio nell'inventario.{FFFFFF}\nInserisci l'ammontare che vuoi ritirare.\nOggetto: %s\nQuantità disponibile: %d", ServerItem_GetName(itemid), slotAmount), "Ritira", "Indietro");

	Inventory_DecreaseSlotAmount(inventoryRef, slotid, val);
	Character_GiveItem(playerid, itemid, val, 0, true);

	Character_SaveInventory(playerid);

	new Building:building = Internal_GetBuilding(playerid);
	Building_SaveInventory(building);

	Building_ShowInventory(building, playerid);
	return 1;
}


Dialog:BuildingInvDepSelectItem(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Internal_ShowBuildingSlotAction(playerid, Internal_GetSlotID(playerid));
	
	new 
		Building:building = Internal_GetBuilding(playerid), 
		Inventory:inventoryRef = Internal_GetInventory(playerid),
		itemid = Character_GetSlotItem(playerid, listitem),
		amount = Character_GetSlotAmount(playerid, listitem)
	;
	if(ServerItem_IsUnique(itemid) || amount == 1)
	{
		if(!Inventory_HasSpaceForItem(Internal_GetInventory(playerid), itemid, amount))
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario dell'edificio.");
			return Inventory_Show(Character_GetInventory(playerid), playerid, "Seleziona l'oggetto da depositare", BuildingInvDepSelectItem, "Seleziona", "Indietro");
		}
		Inventory_AddItem(inventoryRef, itemid, amount, 0);
		Character_DecreaseItemAmount(playerid, itemid, amount);

		Character_SaveInventory(playerid);
		Building_SaveInventory(building);

		SendFormattedMessage(playerid, COLOR_ORANGE, "Hai depositato gli oggetti nell'inventario del tuo edificio. (Oggetto: %s - Quantità: %d)", ServerItem_GetName(itemid), amount);
		Building_ShowInventory(building, playerid);
	}
	else
	{
		map_set_cell(PlayerBuildingInventory, playerid, ItemToDeposit, itemid);
		Dialog_Show_s(playerid, BuildingInvDepositAmount, DIALOG_STYLE_INPUT, @("Quantità da depositare"), str_format("Inserisci l'ammontare che vuoi depositare all'interno dell'inventario del tuo edificio.\nOggetto: %s.\nQuantità: %d.\nStack Massime: %d", ServerItem_GetName(itemid), Character_GetItemAmount(playerid, itemid), ServerItem_GetMaxStack(itemid)), "Deposita", "Indietro");	
	}
	return 1;
}


Dialog:BuildingInvDepositAmount(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Internal_ShowBuildingSlotAction(playerid, Internal_GetSlotID(playerid));
	new 
		Building:building = Internal_GetBuilding(playerid), 
		val = strval(inputtext),
		Inventory:inventoryRef = Internal_GetInventory(playerid),
		itemid = Internal_GetItemToDeposit(playerid),
		playerAmount = Character_GetItemAmount(playerid, itemid),
		itemMaxStacks = ServerItem_GetMaxStack(itemid)
	;
	if(! (0 < val <= playerAmount))
		return Dialog_Show_s(playerid, BuildingInvDepositAmount, DIALOG_STYLE_INPUT, @("Quantità da depositare"), str_format("{FF0000}Non possiedi l'ammontare inserito.{FFFFFF}\nInserisci l'ammontare che vuoi depositare all'interno dell'inventario del tuo edificio.\nOggetto: %s.\nQuantità: %d.\nStack Massime: %d", ServerItem_GetName(itemid), playerAmount, itemMaxStacks), "Deposita", "Indietro");	
	
	if(!Inventory_HasSpaceForItem(inventoryRef, itemid, val))
		return Dialog_Show_s(playerid, BuildingInvDepositAmount, DIALOG_STYLE_INPUT, @("Quantità da depositare"), str_format("{FF0000}L'edificio non ha abbastanza spazio.{FFFFFF}\nInserisci l'ammontare che vuoi depositare all'interno dell'inventario del tuo edificio.\nOggetto: %s.\nQuantità: %d.\nStack Massime: %d", ServerItem_GetName(itemid), playerAmount, itemMaxStacks), "Deposita", "Indietro");	

	Inventory_AddItem(inventoryRef, itemid, val, 0);
	Character_DecreaseItemAmount(playerid, itemid, val);

	Character_SaveInventory(playerid);
	Building_SaveInventory(building);

	SendFormattedMessage(playerid, COLOR_ORANGE, "Hai depositato gli oggetti nell'inventario del tuo edificio. (Oggetto: %s - Quantità: %d)", ServerItem_GetName(itemid), val);
	
	Building_ShowInventory(building, playerid);
	return 1;
}

static Internal_ShowBuildingSlotAction(playerid, slotid)
{
	if(slotid == -1)
		return 0;
	
	new itemid = Inventory_GetSlotItem(Internal_GetInventory(playerid), slotid);    
	
	map_set_cell(PlayerBuildingInventory, playerid, SlotID, slotid);
    
	if(itemid == 0)
    {
        Dialog_Show(playerid, BuildingInventoryAction, DIALOG_STYLE_LIST, "Inventario Casa", "Deposita", "Seleziona", "Indietro");
    }
    else
    {
        Dialog_Show(playerid, BuildingInventoryAction, DIALOG_STYLE_LIST, "Inventario Casa", "Deposita\nRitira", "Seleziona", "Indietro");
    }
	return 1;
}

static Building:Internal_GetBuilding(playerid)
{
	if(!map_has_key(PlayerBuildingInventory, playerid))
		return INVALID_BUILDING;
	return Building:map_get(PlayerBuildingInventory, playerid, Building);
}

static Inventory:Internal_GetInventory(playerid)
{
	if(!map_has_key(PlayerBuildingInventory, playerid))
		return INVENTORY_NULL;
	return Inventory:map_get(PlayerBuildingInventory, playerid, Inventory);
}

static Internal_GetSlotID(playerid)
{
	if(!map_has_key(PlayerBuildingInventory, playerid))
		return -1;
	return map_get(PlayerBuildingInventory, playerid, SlotID);
}

static Internal_GetItemToDeposit(playerid)
{
	if(!map_has_key(PlayerBuildingInventory, playerid))
		return 0;
	return map_get(PlayerBuildingInventory, playerid, ItemToDeposit);
}