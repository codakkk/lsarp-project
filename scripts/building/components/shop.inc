
#include <building\core>

#include <YSI_Coding\y_hooks>

static 
    Map:SellingItems; // <buildingid, map<itemid, price>>

static SelectedItemID[MAX_PLAYERS char];

hook OnGameModeInit()
{
    SellingItems = map_new();

    mysql_tquery_f(gMySQL, "CREATE TABLE IF NOT EXISTS `building_selling_items` ( \
                        `building_id` int(11) NOT NULL, \
                        `item_id` tinyint(4) NOT NULL, \
                        `price` int(11) NOT NULL, \
                        PRIMARY KEY (`building_id`,`item_id`), \
                        CONSTRAINT `building_selling_items` FOREIGN KEY (`building_id`) REFERENCES `buildings` (`id`) ON DELETE CASCADE \
                        ) ENGINE=InnoDB DEFAULT CHARSET=latin1");
    return 1;
}

hook OnBuildingLoaded(buildingid)
{
    map_add(SellingItems, buildingid, map_new());

    inline OnLoad()
    {
        new rows = cache_num_rows();
        if(rows <= 0)
            return 0;
        
        new Map:map = Map:map_get(SellingItems, buildingid);

        new itemid, price;
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "item_id", itemid);
            cache_get_value_name_int(i, "price", price);

            if(itemid <= 0 || price <= 0 || !ServerItem_IsValid(itemid))
                continue;
            
            map_add(map, itemid, price);
        }
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `building_selling_items` WHERE building_id = '%d'", buildingid);
    return 1;
}

hook OnBuildingDeleted(buildingid)
{
    //map_remove
    return 1;
}


hook OnPlayerEnterBuilding(playerid, buildingid)
{
    if(Building_CanSellItems(buildingid) && map_size(Map:map_get(SellingItems, buildingid)) > 0)
    {
        SendClientMessage(playerid, COLOR_ORANGE, "Questo edificio ha degli oggetti in vendita. Usa \"/compra\" per visualizzarne la lista.");
    }
    return 1;
}

hook OnCharacterTryBuy(playerid)
{
    new buildingid = Character_GetNearBuildingID(playerid);
    if(!Building_IsValid(buildingid))
        return 0;
    
    if(Building_GetType(buildingid) != BUILDING_TYPE_STORE)
        return 0;

    if(!Building_CanSellItems(buildingid))
        return SendClientMessage(playerid, COLOR_ERROR, "Questo edificio non ha nulla da vendere.");
    
    Building_ShowSellingItems(buildingid, playerid);
    return 1;
}

/*CMD:addsell(playerid, params[])
{
    new id, item, price;
    if(sscanf(params, "dk<item>d", id, item, price))
        return SendClientMessage(playerid, COLOR_ERROR, "/addsell <buildingid> <itemid/nome> <prezzo>");
    
    if(price < 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Il prezzo inserito non è valido.");

    if(Building_AddSellableItem(id, item, price))
    {
        SendFormattedMessage(playerid, COLOR_ERROR, "L'oggetto (%s) è stato aggiunto alla lista dei vendibili al prezzo di $%d per nell'edificio %d.", ServerItem_GetName(item), price, id);
    }
    else
    {
        SendClientMessage(playerid, COLOR_ERROR, "Oggetto già presente.");
    }
    return 1;
}*/

Dialog:Dialog_BuildingSellingItems(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    
    new buildingid = Character_GetNearBuildingID(playerid);

    if(!Building_IsValid(buildingid) || !Building_CanSellItems(buildingid))
        return 0;

    new Map:items = Map:map_get(SellingItems, buildingid);
    
    if(map_size(items) <= 0 || listitem >= map_size(items))
        return SendClientMessage(playerid, COLOR_ERROR, "L'oggetto non è piu' disponibile.");

    new itemid = map_key_at(items, listitem),
        price = map_value_at(items, listitem);

    new Inventory:buildingInv = Building_GetInventory(buildingid);

    new quantity = Inventory_GetItemAmount(buildingInv, itemid);
    if(quantity <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "L'oggetto non è disponibile.");
    
    SelectedItemID{playerid} = itemid;

    if(quantity == 1)
        Dialog_Show_s(playerid, Dialog_BuyBuildingItem, DIALOG_STYLE_MSGBOX, @("Acquista Oggetto"), str_format("Sei sicuro di voler acquistare quest'oggetto (%s) per {85bb65}${FFFFFF}%d?", ServerItem_GetName(itemid), price), "Acquista", "Annulla");
    else
        Dialog_Show_s(playerid, Dialog_BuyBuildingItem, DIALOG_STYLE_INPUT, @("Acquista Oggetto"), str_format("Inserisci la quantità dell'oggetto (%s) che vuoi acquistare.\nDisponibilità: %d\nPrezzo: {85bb65}${FFFFFF}%d", ServerItem_GetName(itemid), quantity, price), "Acquista", "Annulla");
    return 1;
}

Dialog:Dialog_BuyBuildingItem(playerid, response, listitem, inputtext[])
{
    new buildingid = Character_GetNearBuildingID(playerid);
    if(!Building_IsValid(buildingid) || !Building_CanSellItems(buildingid))
        return 0;
    
    if(!response)
        return Building_ShowSellingItems(buildingid, playerid);
    
    new itemid = SelectedItemID{playerid};

    if(!Building_HasSellableItem(buildingid, itemid))
        return SendClientMessage(playerid, COLOR_ERROR, "L'oggetto non è piu' disponibile.");

    new 
        price = Building_GetSellableItemPrice(buildingid, itemid),
        Inventory:buildingInv = Building_GetInventory(buildingid),
        buildingQuantity = Inventory_GetItemAmount(buildingInv, itemid);

    if(buildingQuantity <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "L'oggetto non è piu' disponibile.");

    new buyingQuantity = 0;

    if(buildingQuantity == 1)
    {   
        buyingQuantity = 1;
    }
    else
    {
        if(sscanf(inputtext, "d", buyingQuantity))
        {
            return Dialog_Show_s(playerid, Dialog_BuyBuildingItem, DIALOG_STYLE_INPUT, @("Acquista Oggetto"), str_format("Inserisci la quantità dell'oggetto (%s) che vuoi acquistare.\nDisponibilità: %d\nPrezzo: {85bb65}${FFFFFF}%d", ServerItem_GetName(itemid), buildingQuantity, price), "Acquista", "Annulla");
        }
        
        if(buyingQuantity <= 0 || buyingQuantity > buildingQuantity)
            return Dialog_Show_s(playerid, Dialog_BuyBuildingItem, DIALOG_STYLE_INPUT, @("Acquista Oggetto"), str_format("{FF0000}L'edificio non dispone della quantità inserita.{FFFFFF}\nInserisci la quantità dell'oggetto (%s) che vuoi acquistare.\nDisponibilità: %d\nPrezzo: {85bb65}${FFFFFF}%d", ServerItem_GetName(itemid), buildingQuantity, price), "Acquista", "Annulla");
        
    }

    if(Character_GetMoney(playerid) < price * buyingQuantity)
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi per acquistare quest'oggetto.");
    
    if(!Character_HasSpaceForItem(playerid, itemid, buyingQuantity))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio per quest'oggetto.");

    SendFormattedMessage(playerid, COLOR_GREEN, "Hai acquistato l'oggetto (%s - Quantità: %d) dall'edificio.", ServerItem_GetName(itemid), buyingQuantity);

    Inventory_DecreaseItemAmount(buildingInv, itemid, buyingQuantity);
    Building_SaveInventory(Building_GetID(buildingid));
    Building_GiveMoney(buildingid, buyingQuantity * price);
    Character_GiveItem(playerid, itemid, buyingQuantity);
    Character_GiveMoney(playerid, -price * buyingQuantity, "buying_from_building");
    return 1;
}

stock Character_EditSellableItems(playerid)
{
    new buildingid = Character_GetNearBuildingID(playerid);
    if(!Building_IsValid(buildingid))
        return SendClientMessage(playerid, COLOR_ERROR, "Devi essere all'interno dell'edificio per utilizzare quest'opzione.");
    if(!Building_CanSellItems(buildingid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare quest'opzione su questo tipo di edificio.");
    Dialog_Show(playerid, Dialog_EditSellableItems, DIALOG_STYLE_LIST, "Gestione Negozio", "Lista\nAggiungi Oggetto", "Seleziona", "Chiudi");
    return 1;
}

Dialog:Dialog_EditSellableItems(playerid, response, listitem, inputtext[])
{
    new buildingid = Character_GetNearBuildingID(playerid);
    if(!response)
        return Character_ShowBizDialog(playerid);
    if(listitem == 0)
    {
        Building_ShowSellingItemsEdit(buildingid, playerid);
    }
    else if(listitem == 1)
        Dialog_Show(playerid, Dialog_AddSellableItem, DIALOG_STYLE_INPUT, "Aggiungi Oggetto", "Inserisci l'ID o il nome dell'oggetto che vuoi aggiungere tra i vendibili seguito dal prezzo.\nEsempi:\n- 83 10\n- Pane 10\n\n- Usa _ invece di spazio nei nomi. Esempio:\n- Componenti_Meccanici 5", "Aggiungi", "Chiudi");
    return 1;
}

Dialog:Dialog_EditSellableItemList(playerid, response, listitem, inputtext[])
{
    new buildingid = Character_GetNearBuildingID(playerid);
    
    if(!response)
        return Character_EditSellableItems(playerid);
    
    new Map:items = Map:map_get(SellingItems, buildingid);
    new 
        itemid = SelectedItemID{playerid} = map_key_at(items, listitem),
        price = map_value_at(items, listitem);

    Dialog_Show_s(playerid, Dialog_EditSellableItem, DIALOG_STYLE_LIST, str_format("Modifica Oggetto (%s - {85bb65}${FFFFFF}%d)", ServerItem_GetName(itemid), price), @("Modifica Prezzo\n{FF0000}Rimuovi{FFFFFF}"), "Seleziona", "Chiudi");
    return 1;
}

Dialog:Dialog_EditSellableItem(playerid, response, listitem, inputtext[])
{
    new buildingid = Character_GetNearBuildingID(playerid);

    if(!response)
        return Character_EditSellableItems(playerid);
    
    new String:title = str_format("%s (%d) - {85bb65}${FFFFFF}%d", ServerItem_GetName(SelectedItemID{playerid}), SelectedItemID{playerid}, Building_GetSellableItemPrice(buildingid, SelectedItemID{playerid}));
    if(listitem == 0)
    {
        Dialog_Show_s(playerid, Dialog_EditSellableItemPrice, DIALOG_STYLE_INPUT, title, @("Inserisci il nuovo prezzo per l'oggetto."), "Modifica", "Annulla");
    }
    else
    {
        Dialog_Show_s(playerid, Dialog_EditSellableItemRemove, DIALOG_STYLE_MSGBOX, title, @("Sei sicuro di voler rimuovere l'oggetto dalla lista?"), "Rimuovi", "Annulla");
    }
    return 1;
}

Dialog:Dialog_EditSellableItemPrice(playerid, response, listitem, inputtext[])
{
    new buildingid = Character_GetNearBuildingID(playerid);

    if(!response)
        return Character_ShowBizDialog(playerid);
    
    if(!Building_HasSellableItem(buildingid, SelectedItemID{playerid}))
    {
        SendClientMessage(playerid, COLOR_ERROR, "L'oggetto non è più in lista.");
        Character_EditSellableItems(playerid);
        return 1;
    }

    new val = strval(inputtext);
    if(val < 0)
    {
        new String:title = str_format("%s (%d) - {85bb65}${FFFFFF}%d", ServerItem_GetName(SelectedItemID{playerid}), SelectedItemID{playerid}, Building_GetSellableItemPrice(buildingid, SelectedItemID{playerid}));
        return Dialog_Show_s(playerid, Dialog_EditSellableItemPrice, DIALOG_STYLE_INPUT, title, @("{FF0000}Il prezzo inserito non è valido.{FFFFFF}\nInserisci il nuovo prezzo per l'oggetto."), "Modifica", "Annulla");
    }

    Building_SetSellableItemPrice(buildingid, SelectedItemID{playerid}, val);
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai modificato il prezzo dell'oggetto (%s). Nuovo Prezzo: $%d", ServerItem_GetName(SelectedItemID{playerid}), Building_GetSellableItemPrice(buildingid, SelectedItemID{playerid}));
    Building_ShowSellingItemsEdit(buildingid, playerid);
    return 1;
}

Dialog:Dialog_EditSellableItemRemove(playerid, response, listitem, inputtext[])
{
    new buildingid = Character_GetNearBuildingID(playerid);

    if(!response)
        return Character_EditSellableItems(playerid);
    
    Building_RemoveSellableItem(buildingid, SelectedItemID{playerid});
    SendFormattedMessage(playerid, COLOR_GREEN, "L'oggetto (%s) è stato rimosso dalla lista.", ServerItem_GetName(SelectedItemID{playerid}));
    SelectedItemID{playerid} = 0;
    return 1;
}

Dialog:Dialog_AddSellableItem(playerid, response, listitem, inputtext[])
{
    new buildingid = Character_GetNearBuildingID(playerid);
    
    if(!response)
        return Character_EditSellableItems(playerid);
    new itemid, price;
    if(sscanf(inputtext, "k<item>d", itemid, price))
        return Dialog_Show(playerid, Dialog_AddSellableItem, DIALOG_STYLE_INPUT, "Aggiungi Oggetto", "{FF0000}Il formato inserito non è valido.{FFFFFF}\nInserisci l'ID o il nome dell'oggetto che vuoi aggiungere tra i vendibili seguito dal prezzo.\nEsempi:\n- 83 10\n- Pane 10\n\n- Usa _ invece di spazio nei nomi. Esempio:\n- Componenti_Meccanici 5", "Aggiungi", "Chiudi");
    
    if(!ServerItem_IsValid(itemid))
        return Dialog_Show(playerid, Dialog_AddSellableItem, DIALOG_STYLE_INPUT, "Aggiungi Oggetto", "{FF0000}L'oggetto inserito non è valido.{FFFFFF}\nInserisci l'ID o il nome dell'oggetto che vuoi aggiungere tra i vendibili seguito dal prezzo.\nEsempi:\n- 83 10\n- Pane 10\n\n- Usa _ invece di spazio nei nomi. Esempio:\n- Componenti_Meccanici 5", "Aggiungi", "Chiudi");

    if(price < 0)
        return Dialog_Show(playerid, Dialog_AddSellableItem, DIALOG_STYLE_INPUT, "Aggiungi Oggetto", "{FF0000}Il prezzo inserito non è valido.{FFFFFF}\nInserisci l'ID o il nome dell'oggetto che vuoi aggiungere tra i vendibili seguito dal prezzo.\nEsempi:\n- 83 10\n- Pane 10\n\n- Usa _ invece di spazio nei nomi. Esempio:\n- Componenti_Meccanici 5", "Aggiungi", "Chiudi");
    
    if(Building_HasSellableItem(buildingid, itemid))
        return Dialog_Show(playerid, Dialog_AddSellableItem, DIALOG_STYLE_INPUT, "Aggiungi Oggetto", "{FF0000}L'oggetto è già in vendita.{FFFFFF}\nInserisci l'ID o il nome dell'oggetto che vuoi aggiungere tra i vendibili seguito dal prezzo.\nEsempi:\n- 83 10\n- Pane 10\n\n- Usa _ invece di spazio nei nomi. Esempio:\n- Componenti_Meccanici 5", "Aggiungi", "Chiudi");

    Building_AddSellableItem(buildingid, itemid, price); 
    
    SendFormattedMessage(playerid, COLOR_GREEN, "L'oggetto (%s) è stato aggiunto alla lista degli acquistabili al prezzo di $%d.", ServerItem_GetName(itemid), price);

    Building_ShowSellingItemsEdit(buildingid, playerid);
    return 1;
}

stock Building_CanSellItems(buildingid)
{
    new type = Building_GetType(buildingid);
    return Building_IsOwned(buildingid) && Building_IsOwnable(buildingid) && type == BUILDING_TYPE_STORE;
}

static Building_GetSellableItemPrice(buildingid, itemid)
{
    if(!Building_IsValid(buildingid))
        return -1;
    new Map:map = Map:map_get(SellingItems, buildingid);
    if(!map_has_key(map, itemid))
        return 0;
    return map_get(map, itemid);
}

static Building_AddSellableItem(buildingid, itemid, price)
{
    if(!Building_IsValid(buildingid) || !ServerItem_IsValid(itemid) || price < 0 || Building_HasSellableItem(buildingid, itemid))
        return 0;
    
    map_add(Map:map_get(SellingItems, buildingid), itemid, price);
    mysql_tquery_f(gMySQL, "INSERT INTO `building_selling_items` (building_id, item_id, price) VALUES('%d', '%d', '%d')", Building_GetID(buildingid), itemid, price);
    return 1;    
}

static Building_SetSellableItemPrice(buildingid, itemid, newprice)
{
    if(!Building_IsValid(buildingid) ||newprice < 0 || !Building_HasSellableItem(buildingid, itemid))
        return 0;
    new Map:items = Map:map_get(SellingItems, buildingid);
    map_set(items, itemid, newprice);
    mysql_tquery_f(gMySQL, "UPDATE `building_selling_items` SET price = '%d' WHERE building_id = '%d' AND item_id = '%d'", newprice, Building_GetID(buildingid), newprice);
    return 1;
}

static Building_RemoveSellableItem(buildingid, itemid)
{
    if(!Building_IsValid(buildingid) || !ServerItem_IsValid(itemid) || !Building_HasSellableItem(buildingid, itemid))
        return 0;
    
    new Map:items = Map:map_get(SellingItems, buildingid);
    map_remove(items, itemid);
    mysql_tquery_f(gMySQL, "DELETE FROM `building_selling_items` WHERE building_id = '%d' AND item_id = '%d'", Building_GetID(buildingid), itemid);
    return 1;    
}
 
static bool:Building_HasSellableItem(buildingid, itemid)
{
    if(!Building_IsValid(buildingid) || !ServerItem_IsValid(itemid) || !Building_IsValid(buildingid))
        return false;
    return map_has_key(Map:map_get(SellingItems, buildingid), itemid);
}

static Building_ShowSellingItems(buildingid, toPlayer)
{
    if(!Building_IsValid(buildingid) || !Character_IsLogged(toPlayer))
        return 0;
    new 
        Map:items = Map:map_get(SellingItems, buildingid),
        String:text = @("Oggetto\tPrezzo ({85bb65}${FFFFFF})\tDisponibilità\n"),
        Inventory:buildingInv = Building_GetInventory(buildingid);

    if(map_size(items) <= 0)
        return SendClientMessage(toPlayer, COLOR_ERROR, "L'edificio non ha nulla da vendere.");

    new itemid, price;
    for_map(i : items)
    {
        itemid = iter_get_key(i);
        price = iter_get_value(i);
        
        new isDisp = Inventory_HasItem(buildingInv, itemid);
        
        text += str_format("%s\t{85bb65}${FFFFFF}%d\t", ServerItem_GetName(itemid), price);

        if(isDisp)
            text += str_format("{00FF00}Disponibile{FFFFFF} (Q: %d)", Inventory_GetItemAmount(buildingInv, itemid));
        else
            text += @("{FF0000}Non disponibile{FFFFFF}");
        
        text += @("\n");
    }

    Dialog_Show_s(toPlayer, Dialog_BuildingSellingItems, DIALOG_STYLE_TABLIST_HEADERS, @("Lista oggetti acquistabili"), text, "Acquista", "Chiudi");
    return 1;
}

static Building_ShowSellingItemsEdit(buildingid, toPlayer)
{
    if(!Building_IsValid(buildingid))
        return 0;
    new 
        Map:items = Map:map_get(SellingItems, buildingid),
        String:str = @("{FFFFFF}Oggetto{FFFFFF}\t{FFFFFF}Prezzo{FFFFFF} ({85bb65}${FFFFFF})\n")
    ;
    new itemid, price;
    for_map(i : items)
    {
        itemid = iter_get_key(i);
        price = iter_get_value(i);
        str += str_format("%s (ID: %d)\t{85bb65}${FFFFFF}%d\n", ServerItem_GetName(itemid), itemid, price);
    }

    Dialog_Show_s(toPlayer, Dialog_EditSellableItemList, DIALOG_STYLE_TABLIST_HEADERS, @("Lista Oggetti"), str, "Modifica", "Chiudi");
    return 1;
}