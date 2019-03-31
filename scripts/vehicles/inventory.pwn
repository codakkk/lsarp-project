#include <YSI\y_hooks>

stock bool:Vehicle_InitializeInventory(vehicleid)
{
    if(VehicleInfo[vehicleid][vModel] == 0)
        return false;
    new Inventory:inv = Inventory_New(2);
    map_add(VehicleInventory, vehicleid, List:inv);
    printf("Vehicle %d Inventory initialized", vehicleid);
    return true;
}

stock bool:Vehicle_UnloadInventory(vehicleid)
{
    map_remove_deep(VehicleInventory, vehicleid);
    printf("Vehicle %d unloaded", vehicleid);
    return true;
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

stock Vehicle_DecreaseItemAmountBySlot(vehicleid, slotid, amount = 1)
{
    new Inventory:inventory = Vehicle_GetInventory(vehicleid);
    
    new result = Inventory_DecreaseAmountBySlot(inventory, slotid, amount);

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

stock Vehicle_GetItemID(vehicleid, slotid)
{
    new Inventory:inventory = Vehicle_GetInventory(vehicleid);
    if(!list_valid(inventory))
        return 0;
    new result = Inventory_GetItemID(inventory, slotid);
    return result;
}

stock Vehicle_GetInventorySize(vehicleid)
{
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
        tempItem[E_INVENTORY_DATA];
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
    Dialog_Show(playerid, Dialog_InventoryItemList, DIALOG_STYLE_TABLIST_HEADERS, title, "Nome\tQuantità\tTipo\n%s", "Avanti", "Chiudi", string);
    return true;
}

stock Vehicle_SaveInventory(vehicleid)
{
    new Inventory:inventory = Vehicle_GetInventory(vehicleid);
    if(!list_valid(inventory))
        return;
    new
        query[1024],
        tempItems[128],
        tempAmounts[128],
        tempExtra[128],
        tempItem[E_INVENTORY_DATA]
    ;
    for_inventory(i : inventory)
    {
        if(iter_sizeof(i) == 0)
        {
            tempItem[gInvItem] = tempItem[gInvAmount] = tempItem[gInvExtra] = 0; 
        }
        else
        { 
            iter_get_arr(i, tempItem);
        }
        format(tempItems, sizeof(tempItems), "%s%d%c", tempItems, tempItem[gInvItem], '|');
        format(tempAmounts, sizeof(tempAmounts), "%s%d%c", tempAmounts, tempItem[gInvAmount], '|');
        format(tempExtra, sizeof(tempExtra), "%s%d%c", tempExtra, tempItem[gInvExtra], '|');
    }
    mysql_format(gMySQL, query, sizeof(query), "INSERT INTO `vehicle_inventory` \
    (VehicleID, Items, ItemsAmount, ItemsExtraData) VALUES('%d', '%e', '%e', '%e') \
    ON DUPLICATE KEY UPDATE \
    Items = VALUES(Items), ItemsAmount = VALUES(ItemsAmount), ItemsExtraData = VALUES(ItemsExtraData)",
    VehicleInfo[vehicleid][vID], tempItems, tempAmounts, tempExtra);
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
    new query[256];
    mysql_format(gMySQL, query, sizeof(query), "SELECT * FROM `vehicle_inventory` WHERE VehicleID = '%d'", vehicleid);
    mysql_tquery_inline(gMySQL, query, using inline OnLoad);
}