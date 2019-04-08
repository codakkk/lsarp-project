#include <YSI_Coding\y_hooks>

hook OnPlayerClearData(playerid)
{
    pCurrentVehicleInventory[playerid] = 0;
    return 1;
}

stock Vehicle_InitializeInventory(vehicleid)
{
    if(VehicleInfo[vehicleid][vModel] == 0)
        return 0;
    new Inventory:inv = Inventory_New(2);
    map_add(VehicleInventory, vehicleid, List:inv);
    printf("Vehicle %d Inventory initialized", vehicleid);
    return 1;
}

stock Vehicle_UnloadInventory(vehicleid)
{
    map_remove_deep(VehicleInventory, vehicleid);
    printf("Vehicle %d unloaded", vehicleid);
    return 1;
}

stock Inventory:Vehicle_GetInventory(vehicleid)
{
    new Inventory:inv;
    map_get_safe(VehicleInventory, vehicleid, List:inv);
    return inv;
}

stock bool:Vehicle_HasInventory(vehicleid)
{
    return list_valid(Vehicle_GetInventory(vehicleid));
}

hook OnPlayerVehicleSaved(vehicleid)
{
    Vehicle_SaveInventory(vehicleid);
    return 1;
}

hook OnPlayerVehicleUnLoaded(vehicleid)
{
    Vehicle_UnloadInventory(vehicleid);
    return 1;
}

hook OnPlayerVehicleLoaded(vehicleid)
{
    Vehicle_InitializeInventory(vehicleid);
    Vehicle_LoadInventory(vehicleid);
    return 1;
}

stock Vehicle_AddItem(vehicleid, itemid, amount = 1, extra = 0)
{
    new Inventory:vehicleInventory = Vehicle_GetInventory(vehicleid);
    new result = Inventory_AddItem(vehicleInventory, itemid, amount, extra);
    Vehicle_SaveInventory(vehicleid);
    return result;
}

stock Vehicle_DecreaseSlotAmount(vehicleid, slotid, amount = 1)
{
    new Inventory:inventory = Vehicle_GetInventory(vehicleid);
    
    new result = Inventory_DecreaseSlotAmount(inventory, slotid, amount);

    Vehicle_SaveInventory(vehicleid);
    return result;
}

stock Vehicle_DecreaseItemAmount(vehicleid, itemid, amount = 1)
{
    new Inventory:inventory = Vehicle_GetInventory(vehicleid);
    
    new result = Inventory_DecreaseItemAmount(inventory, itemid, amount);

    Vehicle_SaveInventory(vehicleid);
    return result;
}

stock bool:Vehicle_HasSpaceForItem(vehicleid, itemid, amount)
{
    new Inventory:inventory = Vehicle_GetInventory(vehicleid);
    if(!list_valid(inventory))
        return false;
    return Inventory_HasSpaceForItem(inventory, itemid, amount);
}

stock bool:Vehicle_HasSpaceForWeapon(vehicleid, weaponid, ammo) return Inventory_HasSpaceForWeapon(Vehicle_GetInventory(vehicleid), weaponid, ammo);

stock bool:Vehicle_IsSlotUsed(vehicleid, slotid)
{
    return Inventory_IsSlotUsed(Vehicle_GetInventory(vehicleid), slotid);
}

stock Vehicle_GetItemID(vehicleid, slotid)
{
    new Inventory:inventory = Vehicle_GetInventory(vehicleid);
    if(!list_valid(inventory))
        return 0;
    new result = Inventory_GetSlotItem(inventory, slotid);
    return result;
}

stock Vehicle_GetSlotAmount(vehicleid, slotid)
{
    return Inventory_GetSlotAmount(Vehicle_GetInventory(vehicleid), slotid);
}

stock Vehicle_GetSlotExtra(vehicleid, slotid) // Should it be called "GetSlotExtra"?
{
    return Inventory_GetSlotExtra(Vehicle_GetInventory(vehicleid), slotid);
}

stock Vehicle_GetInventorySize(vehicleid)
{
    #pragma unused vehicleid
    return 4;
}

stock bool:Vehicle_ShowInventory(vehicleid, playerid)
{
    if(!gAccountLogged[playerid] || !gCharacterLogged[playerid])
        return false;
    new Inventory:inventory = Vehicle_GetInventory(vehicleid);
    if(!list_valid(inventory))
        return false;
    
    new 
        string[4096],
        temp[16],
        tempItem[E_ITEM_DATA];
    for_inventory(i : inventory)
    {
        if(iter_sizeof(i) == 0) // If no item
        {
            tempItem[gInvItem] = tempItem[gInvAmount] = tempItem[gInvExtra] = 0;
            format(string, sizeof(string), "%s{808080}Slot Libera\t{808080}--\t{808080}--\n", string);
        }
        else
        {
            iter_get_arr(i, tempItem);
            new itemid = tempItem[gInvItem],
                itemAmount = tempItem[gInvAmount];
            if(ServerItem_IsUnique(itemid))
                temp = "--";
            else
                format(temp, sizeof(temp), "%d", itemAmount);
            format(string, sizeof(string), "%s{FFFFFF}%s\t{FFFFFF}%s\t{FFFFFF}%s\n", string, ServerItem[itemid][sitemName], temp, ServerItem_GetTypeName(itemid));
        }
    }
    new title[48];
    format(title, sizeof(title), "Inventario (%d/%d)", Inventory_GetUsedSpace(inventory), Inventory_GetSpace(inventory));
    Dialog_Show(playerid, Dialog_VehicleInventory, DIALOG_STYLE_TABLIST_HEADERS, title, "Nome\tQuantità\tTipo\n%s", "Avanti", "Chiudi", string);
    pCurrentVehicleInventory[playerid] = vehicleid;
    return true;
}

Dialog:Dialog_VehicleInventory(playerid, response, listitem, inputtext[])
{
    new vehicleid = pCurrentVehicleInventory[playerid];
    if(!response || vehicleid == 0)
        return 0;
    if(Vehicle_IsSlotUsed(vehicleid, listitem) == false)
        return Vehicle_ShowInventory(vehicleid, playerid);
    new itemid = Vehicle_GetItemID(vehicleid, listitem);
    new title[48];
    format(title, sizeof(title), "%s (%d)", ServerItem_GetName(itemid), Vehicle_GetSlotAmount(vehicleid, listitem));
    Dialog_Show(playerid, D_VehicleInventoryItem, DIALOG_STYLE_LIST, title, "Preleva", "Avanti", "Chiudi");
    pSelectedListItem[playerid] = listitem;
    return 1;
}

Dialog:D_VehicleInventoryItem(playerid, response, listitem, inputtext[])
{
    if(!response || pSelectedListItem[playerid] == -1)
        return 0;
    new vehicleid = pCurrentVehicleInventory[playerid],
        slotid = pSelectedListItem[playerid],
        itemid = Vehicle_GetItemID(vehicleid, slotid);
    if(itemid == 0)
        return 0;
    if(!IsPlayerInRangeOfVehicle(playerid, vehicleid, 5.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al veicolo!");
    switch(listitem)
    {
        //case 0: // use
        //{
            //Trigger_OnPlayerInvItemUse(playerid, slotid, itemid, ServerItem_GetType(itemid));
        //}
        case 0: // withdraw
        {
            printf("%s Unique: %s", ServerItem_GetName(itemid), (ServerItem_IsUnique(itemid) ? ("Yes") : ("No")));
            if(ServerItem_IsUnique(itemid))
            {
                new title[48];
                format(title, sizeof(title), "%s (%d)", ServerItem_GetName(itemid), Vehicle_GetSlotAmount(vehicleid, slotid));
                Dialog_Show(playerid, D_VehInvItemAmount, DIALOG_STYLE_MSGBOX, title, "Sei sicuro di voler prelevare %s dal tuo veicolo?", "Preleva", "Chiudi", ServerItem_GetName(itemid));
            }
            else
            {
                new title[48];
                format(title, sizeof(title), "%s (%d)", ServerItem_GetName(itemid), Vehicle_GetSlotAmount(vehicleid, slotid));
                Dialog_Show(playerid, D_VehInvItemAmount, DIALOG_STYLE_INPUT, title, "Inserisci la quantità da ritirare.", "Avanti", "Chiudi");
            }
        }
    }
    return 1;
}

Dialog:D_VehInvItemAmount(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    new 
        amount = 1, 
        vehicleid = pCurrentVehicleInventory[playerid],
        slotid = pSelectedListItem[playerid],
        itemid = Vehicle_GetItemID(vehicleid, slotid),
        extra = Vehicle_GetSlotExtra(vehicleid, slotid),
        slotAmount = Vehicle_GetSlotAmount(vehicleid, slotid);
    if(!ServerItem_IsUnique(itemid) && (sscanf(inputtext, "d", amount) || amount < 1 || amount > slotAmount))
    {
        new title[48];
        format(title, sizeof(title), "%s (%d)", ServerItem_GetName(itemid), slotAmount);
        Dialog_Show(playerid, D_VehInvItemAmount, DIALOG_STYLE_INPUT, title, "{FF0000}La quantità inserita non è valida!\n{FFFFFF}Inserisci la quantità da ritirare.", "Avanti", "Chiudi");
        return 1;
    }
    if(!Character_HasSpaceForItem(playerid, itemid, amount))
    {
        new title[48];
        format(title, sizeof(title), "%s (%d)", ServerItem_GetName(itemid), slotAmount);
        if(!ServerItem_IsUnique(itemid))
            Dialog_Show(playerid, D_VehInvItemAmount, DIALOG_STYLE_INPUT, title, "{FF0000}Non hai abbastanza spazio nell'inventario!\n{FFFFFF}Inserisci la quantità da ritirare.", "Avanti", "Chiudi");
        else
            SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario!");
        return 1;
    }
    if(Vehicle_DecreaseSlotAmount(vehicleid, slotid, amount))
    {
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai ritirato l'oggetto (%s, %d) dal veicolo.", ServerItem_GetName(itemid), amount);
        Character_GiveItem(playerid, itemid, amount, extra);
    }
    return 1;
}


stock Vehicle_SaveInventory(vehicleid)
{
    new Inventory:inventory = Vehicle_GetInventory(vehicleid);
    if(!list_valid(inventory))
        return false;
    new
        query[512],
        tempItems[128],
        tempAmounts[128],
        tempExtras[128]
    ;

    if(Inventory_ParseForSave(inventory, tempItems, tempAmounts, tempExtras))
    {
        mysql_format(gMySQL, query, sizeof(query), "INSERT INTO `vehicle_inventory` \
        (VehicleID, Items, ItemsAmount, ItemsExtraData) VALUES('%d', '%e', '%e', '%e') \
        ON DUPLICATE KEY UPDATE \
        Items = VALUES(Items), ItemsAmount = VALUES(ItemsAmount), ItemsExtraData = VALUES(ItemsExtraData)",
        VehicleInfo[vehicleid][vID], tempItems, tempAmounts, tempExtras);
        mysql_tquery(gMySQL, query);
    }

    return true;
}

stock Vehicle_LoadInventory(vehicleid)
{
    inline OnLoad()
    {
        if(cache_num_rows() > 0)
        {
            new 
                temp[256], 
                tempItems[MAX_ITEMS_PER_VEHICLE],
                tempAmounts[MAX_ITEMS_PER_VEHICLE],
                tempExtraData[MAX_ITEMS_PER_VEHICLE],
                sscanf_format[16];

            format(sscanf_format, sizeof(sscanf_format), "p<|>a<i>[%d]", MAX_ITEMS_PER_VEHICLE);

            // Load Items Ids
            cache_get_value_index(0, 0, temp, sizeof(temp));
            sscanf(temp, sscanf_format, tempItems);

            // Load Items Amounts
            cache_get_value_index(0, 1, temp, sizeof(temp));
            sscanf(temp, sscanf_format, tempAmounts);

            // Load Items Extra
            cache_get_value_index(0, 2, temp, sizeof(temp));
            sscanf(temp, sscanf_format, tempExtraData);

            new Inventory:inventory = Vehicle_GetInventory(vehicleid);
            for(new i = 0; i < Vehicle_GetInventorySize(vehicleid); ++i)
            {
                Inventory_SetItem(inventory, i, tempItems[i], tempAmounts[i], tempExtraData[i]);
            }
            // CallLocalFunction(#OnPlayerVehicleInventoryLoaded, "d", vehicleid);
        }
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `vehicle_inventory` WHERE VehicleID = '%d'", VehicleInfo[vehicleid][vID]);
}