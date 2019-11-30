
#include <faction_system\government\core>
#include <buildings\core>
#include <YSI_Coding\y_hooks>

static enum _:E_SELLING_ITEMS
{
    Name[32],
    ItemID,
    Price,
    Amount,
    Extra, // used for duplicated items (like phones)  or for weapon ammo
    bool:AllowMultiple // Allow multiple buy (like ammos)
};
static Map:SellingItemsByType; // should be removed

stock DefineSellableItem(buildingType, const name[], itemid, price, amount = 1, extra = 0, bool:allowMultiple = false)
{
    new data[E_SELLING_ITEMS];
    format(data[Name], 32, name);
    data[ItemID] = itemid;
    data[Price] = price;
    data[Amount] = amount;
    data[Extra] = extra;
    data[AllowMultiple] = allowMultiple;
    
    new List:items = List:map_get(SellingItemsByType, buildingType);
    list_add_arr(items, data);
}

stock bool:Building_ShowSellingItems(Building:building, playerid, String:title)
{
    new 
        response[e_DIALOG_RESPONSE_INFO],
        data[E_SELLING_ITEMS],    
        List:items = List:map_get(SellingItemsByType, Building_GetType(building));
    
    if(!list_valid(items) || list_size(items) <= 0)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Questo edificio non ha nulla da vendere.");
        return;
    }

    new String:info = str_new("Oggetto\tPrezzo ($)\n");
    for_list(i : items)
    {
        iter_get_arr(i, data);
        if(!ServerItem_IsValid(data[ItemID]))
            continue;
        str_append_format(info, "%s\t{85bb65}${FFFFFF}%s\n", data[Name], FormatNumber(Tax_CalculateByValue(TAX_TYPE_VAT, data[Price])));
    }
    
    pawn_guard(title);
    pawn_guard(info);
    yield 1;
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_TABLIST_HEADERS, title, info, @("Compra"), @("Chiudi"));        
        if(!response[E_DIALOG_RESPONSE_Response])
            break;

        new listitem = response[E_DIALOG_RESPONSE_Listitem];
        list_get_arr(items, listitem, data);

        if(data[ItemID] <= 0)
            continue;

        new taxedPrice = Tax_CalculateByValue(TAX_TYPE_VAT, data[Price]);

        if(Character_GetMoney(playerid) < taxedPrice)
        {
            SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi per acquistare l'oggetto.");
            continue;
        }

        if(!Character_HasSpaceForItem(playerid, data[ItemID], data[Amount]))
        {
            SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario.");
            continue;
        }
        
        Character_GiveItem(playerid, data[ItemID], data[Amount], data[Extra]);

        Building_GiveMoney(building, data[Price]);
        Character_GiveMoney(playerid, -taxedPrice, "buy_item_247");

        Gov_CollectTax(TAX_TYPE_VAT, data[Price]);

        SendClientMessageStr(playerid, COLOR_ORANGE, str_format("Hai acquistato \"%s\" per $%s (%d percento di VAT).", data[Name], FormatNumber(taxedPrice), Tax_GetValue(TAX_TYPE_VAT)));
        break;
    }
}

hook OnGameModeInit()
{
    SellingItemsByType = map_new();
    for(new i = 0; i < BUILDING_TYPE_LAST; ++i)
    {
        map_add(SellingItemsByType, i, list_new());
    }
    return 1;
}

hook OnPlayerEnterBuilding(playerid, Building:building)
{
    new List:items = List:map_get(SellingItemsByType, Building_GetType(building));
    if(list_valid(items) && list_size(items) > 0)
        SendClientMessage(playerid, COLOR_ORANGE, "Questo edificio ha degli oggetti in vendita. Usa \"/compra\" per visualizzarne la lista.");
    return 1;
}