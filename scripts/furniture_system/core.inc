#include <furniture_system\furnitures_list>

#include <YSI_Coding\y_hooks>

static 
    FurniturePage[MAX_PLAYERS char] = {0, ...},
    SelectedFurniture[MAX_PLAYERS] = {0, ...},
    FurnitureListItem[MAX_PLAYERS][20] = {{0}, ...}
;

static enum e_FurnitureCustomData
{
    DATABASE_ID,
    FURNITURE_PRICE,
    FURNITURE_NAME[64]
};
static 
    FurnitureCustomData[e_FurnitureCustomData],
    szCustomData = e_FurnitureCustomData;

hook OnGameModeInit()
{
    mysql_tquery(gMySQL, "UPDATE `furnitures` SET 'object_id' = '0' WHERE 1");
    Furniture_LoadAll();
    return 1;
}

flags:arredamento(CMD_ALIVE_USER);
CMD:arredamento(playerid, params[])
{
    if(EditObject_IsEditing(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
    // "Mura e scale\nTavoli per cucine e salotti\nSedie da uffici e da cucina\nLetti singoli e matrimoniali\nDivani e poltrone\nDecorazioni\nElettronica\nVario\nCucina\nBagno\nBottiglie, bicchieri e tazze\nPorte\nVasi\nPoster e graffiti\nContenitori rifiuti\nVestiti\nBombole e barili\nOggetti per il mare\nQuadri\nScatole\nColonne\nCibo\nUtensili\nMeccanica\nSpeciali\nLavagne"
    Dialog_Show(playerid, Dialog_FurnitureOptions, DIALOG_STYLE_LIST, "Arredamento", "Mobili piazzati\nAcquista", "Avanti", "Chiudi");
    return 1;
}
alias:arredamento("arreda");

hook OnPlayerEditDynObject(playerid, objectid, response, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz)
{
    if(EditObject_GetType(playerid) == EDIT_TYPE_BUYING_FURNITURE)
    {
        if(response == EDIT_RESPONSE_CANCEL)
        {
            Furniture_CancelBuy(playerid);
        }
        else if(response == EDIT_RESPONSE_FINAL)
        {
            Furniture_FinishBuy(playerid, objectid);
        }
    }
    else if(EditObject_GetType(playerid) == EDIT_TYPE_FURNITURE)
    {
        if(response == EDIT_RESPONSE_CANCEL)
        {

            EditObject_End(playerid);
        }
        else if(response == EDIT_RESPONSE_FINAL)
        {

            EditObject_End(playerid);
        }
    }
    return 1;
}

Dialog:Dialog_FurnitureOptions(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    if(listitem == 0)
    {
        return 1;
    }
    else if(listitem == 1)
    {
        static const info[] = "Mura e scale\nTavoli per cucine e salotti\nSedie da uffici e da cucina\nLetti singoli e matrimoniali\nDivani e poltrone\nDecorazioni\nElettronica\nVario\nCucina\nBagno\nBottiglie, bicchieri e tazze\nPorte\nVasi\nPoster e graffiti\nContenitori rifiuti\nVestiti\nBombole e barili\nOggetti per il mare\nQuadri\nScatole\nColonne\nCibo\nUtensili\nMeccanica\nSpeciali\nLavagne";
        Dialog_Show(playerid, Dialog_CategorySelect, DIALOG_STYLE_LIST, "Categoria", info, "Continua", "Indietro");
    }
    return 1;
}

Dialog:Dialog_CategorySelect(playerid, response, listitem, inputtext[])
{
    if(!response)
        return pc_cmd_arredamento(playerid, "");

    new String:dyn = @("Mobile\tPrezzo\n"), name[64], count = 0;

    for(new i = 0, j = sizeof(BuyableFurnitures); i < j; ++i)
    {
        if(BuyableFurniture_GetCategory(i) != listitem)
            continue;
        BuyableFurniture_GetName(i, name);
        dyn += str_format("%s\t$%d\n", name, BuyableFurniture_GetPrice(i));
        FurnitureListItem[playerid][count++] = i;
    }
    Dialog_Show_s(playerid, Dialog_FurnitureBuy, DIALOG_STYLE_TABLIST_HEADERS, @("Arredamento"), dyn, "Acquista", "Chiudi");
    return 1;
}

Dialog:Dialog_FurnitureBuy(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    SelectedFurniture[playerid] = FurnitureListItem[playerid][listitem];
    Furniture_StartBuy(playerid, SelectedFurniture[playerid]);
    return 1;
}

stock Furniture_ShowList(playerid, page)
{
    FurniturePage{playerid} = page;

    return 1;
}

stock Furniture_CreateTemporary(modelid, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, worldid, interior)
{
    new objectid = CreateDynamicObject(modelid, x, y, z, rx, ry, rz, worldid, interior);
    FurnitureCustomData[DATABASE_ID] = 0;
    FurnitureCustomData[FURNITURE_PRICE] = 0;
    Streamer_SetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
    return objectid;
}

stock Furniture_Store(objectid, furnitureid)
{
    if(Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData) && FurnitureCustomData[DATABASE_ID] > 0)
    {
        new
            Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz,
            name[64];
        
        BuyableFurniture_GetName(furnitureid, name);
        GetDynamicObjectPos(objectid, x, y, z);
        GetDynamicObjectRot(objectid, rx, ry, rz);
        
        inline OnInsert()
        {
            FurnitureCustomData[DATABASE_ID] = cache_insert_id();
            FurnitureCustomData[FURNITURE_PRICE] = BuyableFurniture_GetPrice(furnitureid);
            Streamer_SetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData);
        }

        MySQL_TQueryInline(gMySQL, using inline OnInsert, "INSERT INTO `furnitures` \
        (object_id, world, interior, model, price, pos_x, pos_y, pos_z, rot_x, rot_y, rot_z, name) \
        VALUES('%d', '%d', '%d', '%d', '%d', '%f', '%f', '%f', '%f', '%f', '%f', '%e')",
        objectid,
        Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_WORLD_ID),
        Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_INTERIOR_ID),
        Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_MODEL_ID),
        0,
        x, y, z,
        rx, ry, rz,
        name);
        return 1;
    }
    return 0;
}

stock Furniture_LoadAll()
{
    printf("Loading furnitures...");
    inline OnLoad()
    {
        new rows = cache_num_rows();

        new id, modelid, world, interior,
            Float:x, Float:y, Float:z,
            Float:rx, Float:ry, Float:rz,
            price;
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_index_int(i, 0, id);
            cache_get_value_index_int(i, 2, world);
            cache_get_value_index_int(i, 3, interior);
            cache_get_value_index_int(i, 4, modelid);
            cache_get_value_index_int(i, 5, price);
            
            cache_get_value_index_float(i, 6, x);
            cache_get_value_index_float(i, 7, y);
            cache_get_value_index_float(i, 8, z);

            cache_get_value_index_float(i, 9, rx);
            cache_get_value_index_float(i, 10, ry);
            cache_get_value_index_float(i, 11, rz);

            cache_get_value_index(i, 12, FurnitureCustomData[FURNITURE_NAME]);

            new objectid = CreateDynamicObject(modelid, x, y, z, rx, ry, rz, world, interior);

            FurnitureCustomData[DATABASE_ID] = id;
            FurnitureCustomData[FURNITURE_PRICE] = price;

            Streamer_SetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);

            Furniture_SetObject(objectid, id);
        }
        printf("%d furnitures loaded.", rows);
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM furnitures WHERE 1");
}

stock Furniture_SetObject(objectid, dbid)
{
    mysql_tquery_f(gMySQL, "UPDATE furnitures SET object_id = '%d' WHERE id = '%d'", objectid, dbid);
    return 1;
}

stock Furniture_StartBuy(playerid, furnitureid)
{
    if(Character_GetMoney(playerid) < BuyableFurniture_GetPrice(furnitureid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza denaro per acquistare questo mobile.");

    new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
    GetXYInFrontOfPlayer(playerid, x, y, 2);
    
    new objectid = Furniture_CreateTemporary(BuyableFurniture_GetModel(furnitureid), x + 3.0, y, z + 1.0, 0.0, 0.0, 0.0, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid));
    EditObject_Start(playerid, EDIT_TYPE_BUYING_FURNITURE, objectid, 0);

    SendClientMessage(playerid, -1, "Premi {FF6347}ESC{FFFFFF} per annullare e {FF6347}SPAZIO{FFFFFF} per spostare la visuale. Clicca sul {FF6347}FLOPPY{FFFFFF} per salvare.");
    return 1;
}

stock Furniture_FinishBuy(playerid, objectid)
{
    new furnitureid = SelectedFurniture[playerid];

    if(Character_GetMoney(playerid) < BuyableFurniture_GetPrice(furnitureid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza denaro per acquistare questo mobile.");

    Furniture_Store(objectid, furnitureid);

    Character_GiveMoney(playerid, -BuyableFurniture_GetPrice(furnitureid), "furniture buy");

    new name[64];
    BuyableFurniture_GetName(furnitureid, name);
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai acquistato questo mobile (%s) per $%d.", name, BuyableFurniture_GetPrice(furnitureid));

    EditObject_End(playerid);
    return 1;
}

stock Furniture_CancelBuy(playerid)
{
    new objectid = EditObject_GetObjectID(playerid);
    DestroyDynamicObject(objectid);
    
    SendClientMessage(playerid, -1, "Hai cancellato l'acquisto del mobile.");
    
    EditObject_End(playerid);
    return 1;
}