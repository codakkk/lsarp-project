#include <YSI\y_hooks>

stock bool:Character_InitializeInventory(playerid)
{
    //if(!gCharacterLogged[playerid])
        //return false;
    new Inventory:inventory = Inventory_New(PLAYER_INVENTORY_START_SIZE);
    map_add(PlayerInventory, playerid, List:inventory);
    printf("Inventory Initialized");
    return true;
}

stock bool:Character_UnloadInventory(playerid)
{
    if(!gCharacterLogged[playerid])
        return false;
    map_remove_deep(PlayerInventory, playerid);
    return true;
}

/*hook OnCharacterSaveData(playerid)
{
    Character_SaveInventory(playerid);
    return 1;
}*/

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
    printf("OnPlayerWeaponShot Called %d", GetPlayerWeaponState(playerid));

    if(GetPlayerWeaponState(playerid) == WEAPONSTATE_LAST_BULLET && AC_GetPlayerAmmo(playerid) == 1 && /*Has Weapon && */ Character_HasSpaceForWeapon(playerid, weaponid, 0))
    {
        SendClientMessage(playerid, COLOR_GREEN, "Hai finito i colpi dell'arma. L'arma è stata rimessa nell'inventario.");
        Character_GiveItem(playerid, weaponid, 1, 0, false);
    }
    return 1;
}

stock Inventory:Character_GetInventory(playerid)
{
    new Inventory:inv;
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

stock Character_DecreaseAmountBySlot(playerid, slotid, amount = 1)
{
    new Inventory:playerInventory = Character_GetInventory(playerid);
    
    new result = Inventory_DecreaseAmountBySlot(playerInventory, slotid, amount);

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

stock Character_ShowInventory(playerid, targetid)
{
    if(!gCharacterLogged[playerid])
        return 0;

    new Inventory:playerInventory = Character_GetInventory(playerid);

    //if(Inventory_IsEmpty(playerInventory))
        //return SendClientMessage(playerid, COLOR_ERROR, "L'inventario è vuoto!");
    new
        string[4096],
        temp[16],
        tempItem[E_INVENTORY_DATA]
    ;
    for_inventory(i : playerInventory)
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
    format(title, sizeof(title), "Inventario (%d/%d)", Inventory_GetUsedSpace(playerInventory), Inventory_GetSpace(playerInventory));
    Dialog_Show(targetid, Dialog_InventoryItemList, DIALOG_STYLE_TABLIST_HEADERS, title, "Nome\tQuantità\tTipo\n%s", "Avanti", "Chiudi", string);
    return 1;
}

Dialog:Dialog_InventoryItemList(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    new 
        title[48],
        slotid = pInventoryListItem[playerid][listitem],
        itemid = Character_GetItem(playerid, slotid)
        ;
    if(itemid == 0)
        return Character_ShowInventory(playerid, playerid);
    pInventorySelectedListItem[playerid] = listitem;
    format(title, sizeof(title), "%s (Quantità: %d)", ServerItem_GetName(itemid), Character_GetItemAmount(playerid, slotid));
    Dialog_Show(playerid, Dialog_InvItemAction, DIALOG_STYLE_LIST, title, "Usa\nGetta\nDai a un giocatore", "Continua", "Annulla");
    return 1;
}

Dialog:Dialog_InvItemAction(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
        return Character_ShowInventory(playerid, playerid);
    }
    switch(listitem)
    {
        case 0:
        {
            new selected = pInventorySelectedListItem[playerid],
                slotid = pInventoryListItem[playerid][selected],
                itemid = Character_GetItem(playerid, slotid);
            Trigger_OnPlayerInvItemUse(playerid, slotid, itemid, ServerItem_GetType(itemid));
            return 1;
        }
        case 1: return 1;
        case 2: return 1;
    }
    return 1;
}

stock Character_GetItem(playerid, slotid)
{
    new Inventory:playerInv = Character_GetInventory(playerid);
    return Inventory_GetItemID(playerInv, slotid);
}

stock Character_GetItemAmount(playerid, slotid)
{
    new Inventory:playerInv = Character_GetInventory(playerid);
    return Inventory_GetSlotAmount(playerInv, slotid);
}


stock Character_HasItem(playerid, itemid, min = 1) return Inventory_HasItem(Character_GetInventory(playerid), itemid, min);

stock Character_HasSpaceForItem(playerid, itemid, amount) return Inventory_HasSpaceForItem(Character_GetInventory(playerid), itemid, amount);
stock Character_HasSpaceForWeapon(playerid, weaponid, ammo) return Inventory_HasSpaceForWeapon(Character_GetInventory(playerid), weaponid, ammo);

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
    memset(pInventoryListItem[playerid], -1);
    return 1;
}

stock Character_SaveInventory(playerid)
{
    if(!gCharacterLogged[playerid])
        return 0;

    new
        Inventory:inventory = Character_GetInventory(playerid),
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
    mysql_format(gMySQL, query, sizeof(query), "INSERT INTO `character_inventory` \
    (CharacterID, Items, ItemsAmount, ItemsExtraData, EquippedBag) VALUES('%d', '%e', '%e', '%e', '%d') ON DUPLICATE KEY UPDATE \
    Items = VALUES(Items), ItemsAmount = VALUES(ItemsAmount), ItemsExtraData = VALUES(ItemsExtraData), EquippedBag = VALUES(EquippedBag)",
    PlayerInfo[playerid][pID], tempItems, tempAmounts, tempExtra, pInventoryBag[playerid]
    );
    mysql_tquery(gMySQL, query);
    return 1;
}

stock Character_LoadInventory(playerid)
{
    if(!gCharacterLogged[playerid])
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
                Inventory_SetItem(inventory, i, tempItems[i], tempAmounts[i], tempExtraData[i]);
                //Character_GiveItem(playerid, tempItems[i], tempAmounts[i], tempExtraData[i]);
            }

            if(Character_HasBag(playerid))
            {
                SendFormattedMessage(playerid, COLOR_GREEN, "Stai utilizzando: %s.", ServerItem_GetName(Character_GetBag(playerid)));
            }
        }
    }
    new query[256];
    mysql_format(gMySQL, query, sizeof(query), "SELECT Items, ItemsAmount, ItemsExtraData, EquippedBag FROM `character_inventory` WHERE CharacterID = '%d'", PlayerInfo[playerid][pID]);
    mysql_tquery_inline(gMySQL, query, using inline OnLoad);
    return 1;
}