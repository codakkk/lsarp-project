#include <YSI\y_hooks>

// #define Character_%0(%1, Inventory_%0(Character_GetInventory(%1), // It is a convenient way to call Inventory_Func with player inventory

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

stock Character_ShowInventory(playerid, targetid)
{
    if(!gCharacterLogged[playerid])
        return 0;

    new Inventory:playerInventory = Character_GetInventory(playerid);

    new 
        String:string = @("Nome\tQuantità\tTipo\n") + Inventory_ParseForDialog(playerInventory),
        String:title = str_format("Inventario (%d/%d)", Inventory_GetUsedSpace(playerInventory), Inventory_GetSpace(playerInventory));
    Dialog_Show_s(targetid, Dialog_InventoryItemList, DIALOG_STYLE_TABLIST_HEADERS, title, string, "Avanti", "Chiudi");
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
        return Character_ShowInventory(playerid, playerid);
    new String:title = str_format("%s (Quantità: %d)", ServerItem_GetName(itemid), Character_GetSlotAmount(playerid, listitem));
    Dialog_Show_s(playerid, Dialog_InvItemAction, DIALOG_STYLE_LIST, title, @("Usa\nGetta\nDai a un giocatore"), "Continua", "Annulla");
    pSelectedListItem[playerid] = listitem;
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
        itemid = Character_GetSlotItem(playerid, slotid);
    if(itemid == 0) // Safeness
        return 0;
    switch(listitem)
    {
        case 0:
        {
            Trigger_OnPlayerInvItemUse(playerid, slotid, itemid, ServerItem_GetType(itemid));
            return 1;
        }
        case 1: 
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
                content = str_format("Inserisci la quantità di %s che vuoi gettare (1 di default).", ServerItem_GetName(itemid));
                style = DIALOG_STYLE_INPUT;
            }
            Dialog_Show_s(playerid, Dialog_InvItemDrop, style, title, content, "Getta", "Annulla");
            return 1;
        }
        case 2:
        {
            new String:title = str_format("%s (Quantità: %d)", ServerItem_GetName(itemid), Character_GetSlotAmount(playerid, listitem));
            new String:content = @("Inserisci il nome o l'id del giocatore\nseguito dalla quantità.\nEsempio: Mario Rossi 5\nOppure: Mario Rossi per dare solo 1.");
            Dialog_Show_s(playerid, Dialog_InvItemGivePlayer, DIALOG_STYLE_INPUT, title, content, "Continua", "Annulla");
            return 1;
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
        itemid = Character_GetSlotItem(playerid, slotid);
    sscanf(inputtext, "D(1)", amount);
    if(amount <= 0)
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
            content = str_format("Inserisci la quantità di %s che vuoi gettare (1 di default).", ServerItem_GetName(itemid));
            style = DIALOG_STYLE_INPUT;
        }
        Dialog_Show_s(playerid, Dialog_InvItemDrop, style, title, content, "Getta", "Annulla");
        return 1;
    }
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    Drop_Create(x, y, z - 0.9, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), itemid, amount, 0, Character_GetOOCNameStr(playerid));
    
    ApplyAnimation(playerid, "GRENADE", "WEAPON_throwu", 3.0, 0, 0, 0, 0, 0);

    Character_DecreaseSlotAmount(playerid, slotid, amount);
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai gettato: %s (%d)", ServerItem_GetName(itemid), amount);
    return 1;
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
    if(sscanf(inputtext, "uD(1)", id, amount) || !IsPlayerConnected(id) || !gCharacterLogged[id])
    {
        new String:title = str_format("%s (Quantità: %d)", ServerItem_GetName(itemid), Character_GetSlotAmount(playerid, listitem));
        Dialog_Show_s(playerid, Dialog_InvItemGivePlayer, DIALOG_STYLE_INPUT, title, @("{FF0000}Il giocatore non è connesso.\nInserisci il nome o l'id del giocatore\nseguito dalla quantità.\nEsempio: Mario Rossi 5\nOppure: Mario Rossi per dare solo 1."), "Continua", "Annulla");
        return 1;
    }
    if(amount < 1 || amount > slotAmount)
    {
        new String:title = str_format("%s (Quantità: %d)", ServerItem_GetName(itemid), Character_GetSlotAmount(playerid, listitem));
        Dialog_Show_s(playerid, Dialog_InvItemGivePlayer, DIALOG_STYLE_INPUT, title, @("{FF0000}La quantità inserita non è valida.\nInserisci il nome o l'id del giocatore\nseguito dalla quantità.\nEsempio: Mario Rossi 5\nOppure: Mario Rossi per dare solo 1."), "Continua", "Annulla");
        return 1;
    }
    if(!Character_HasSpaceForItem(id, itemid, amount))
    {
        new String:title = str_format("%s (Quantità: %d)", ServerItem_GetName(itemid), Character_GetSlotAmount(playerid, listitem));
        Dialog_Show_s(playerid, Dialog_InvItemGivePlayer, DIALOG_STYLE_INPUT, title, @("{FF0000}Il giocatore non ha abbastanza spazio nell'inventario.\nInserisci il nome o l'id del giocatore\nseguito dalla quantità.\nEsempio: Mario Rossi 5\nOppure: Mario Rossi per dare solo 1."), "Continua", "Annulla");
        return 1;
    }
    Character_DecreaseSlotAmount(playerid, slotid, amount);
    Character_GiveItem(id, itemid, amount);
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai dato l'oggetto (%s, %d) a %s (%d).", ServerItem_GetName(itemid), amount, Character_GetOOCName(id), id);
    SendFormattedMessage(id, COLOR_GREEN, "%s (%d) ti ha dato un oggetto (%s, %d).", Character_GetOOCName(playerid), playerid, ServerItem_GetName(itemid), amount);
    Character_AMe(playerid, "prende degli oggetti e li da a %s", Character_GetOOCName(id));
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