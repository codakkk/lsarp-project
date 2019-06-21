#include <furniture_system\furnitures_list>

#include <YSI_Coding\y_hooks>

#define MAX_FURNITURE_PER_PAGE      (20)

static 
    FurniturePage[MAX_PLAYERS char] = {0, ...},
    SelectedFurniture[MAX_PLAYERS] = {-1, ...},
    FurnitureListItem[MAX_PLAYERS][sizeof(BuyableFurnitures)]
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
    mysql_tquery(gMySQL, "UPDATE `furnitures` SET object_id = '0' WHERE 1");
    Furniture_LoadAll();
    return 1;
}

hook OnPlayerClearData(playerid)
{
    FurniturePage{playerid} = 0;
    SelectedFurniture[playerid] = -1;
    return 1;
}

//flags:arredamento(CMD_ALIVE_USER);
CMD:arredamento(playerid, params[])
{
    if(EditObject_IsEditing(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
    new canFurnish = Character_CanFurnish(playerid);
    if(canFurnish)
    {
        Dialog_Show(playerid, Dialog_Furniture, DIALOG_STYLE_LIST, "Arredamento", "Mobili piazzati\nAcquista", "Avanti", "Chiudi");
    }
    else return SendClientMessage(playerid, COLOR_ERROR, "Non sei nei pressi di un edificio.");
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
            SetDynamicObjectPos(objectid, x, y, z);
            SetDynamicObjectRot(objectid, rx, ry, rz);

            Furniture_FinishBuy(playerid, objectid);
        }
    }
    else if(EditObject_GetType(playerid) == EDIT_TYPE_FURNITURE)
    {
        new Float:tx, Float:ty, Float:tz, Float:trx, Float:try, Float:trz;
        GetDynamicObjectPos(objectid, tx, ty, tz);
        GetDynamicObjectRot(objectid, trx, try, trz);
        if(response == EDIT_RESPONSE_CANCEL)
        {
            SetDynamicObjectPos(objectid, tx, ty, tz);
            SetDynamicObjectRot(objectid, trx, try, trz);

            SendClientMessage(playerid, COLOR_ERROR, "Hai annullato le modifiche apportate al mobile.");

            EditObject_End(playerid);
        }
        else if(response == EDIT_RESPONSE_FINAL)
        {
            Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);

            SetDynamicObjectPos(objectid, x, y, z);
            SetDynamicObjectRot(objectid, rx, ry, rz);
            
            mysql_tquery_f(gMySQL, "UPDATE furnitures SET \
            pos_x = '%f', pos_y = '%f', pos_z = '%f', \
            rot_x = '%f', rot_y = '%f', rot_z = '%f' WHERE id = '%d'", 
            x, y, z,
            rx, ry, rz,
            FurnitureCustomData[DATABASE_ID]);

            EditObject_End(playerid);

            SendClientMessage(playerid, COLOR_GREEN, "Hai spostato il mobile con successo.");
        }
    }
    return 1;
}

Dialog:Dialog_Furniture(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    if(listitem == 0)
    {
        return Character_ShowFurnitures(playerid, 0);
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
    new String:dyn = @("Mobile\n");
    new name[64], count = 0;
    for(new i = 0, j = sizeof(BuyableFurnitures); i < j; ++i)
    {
        if(BuyableFurniture_GetCategory(i) != listitem)
            continue;
        BuyableFurniture_GetName(i, name);
        dyn += str_format("%s\n", name);
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

stock Character_ShowFurnitures(playerid, page)
{
    FurniturePage{playerid} = page;

    new world = GetPlayerVirtualWorld(playerid);
    
    inline OnSelect()
    {
        new 
            rows = cache_num_rows(),
            String:str = @("")    
        ;

        if(!rows)
            return SendClientMessage(playerid, COLOR_ERROR, "Non hai ancora piazzato nessun mobile.");

        if(rows > page * MAX_FURNITURE_PER_PAGE)
            str += String:str_format(">>> Pagina %d\n", page + 2);
        else if(rows <= page * MAX_FURNITURE_PER_PAGE && !rows)
            return SendClientMessage(playerid, COLOR_ERROR, "Non hai ancora piazzato nessun mobile.");

        new objectid = 0;
        new count = 0;
        for(new i = page * MAX_FURNITURE_PER_PAGE; i < rows; ++i)
        {
            if(count == MAX_FURNITURE_PER_PAGE)
                break;

            cache_get_value_index_int(i, 1, objectid);

            if(!IsValidDynamicObject(objectid))
                continue;
            
            if(Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_WORLD_ID) != world)
                continue;

            Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);

            str += str_format("%s [ID: %d - Object ID: %d]\n", FurnitureCustomData[FURNITURE_NAME], page * MAX_FURNITURE_PER_PAGE + count, objectid);

            FurnitureListItem[playerid][count] = objectid;
            count++;
        }

        Dialog_Show_s(playerid, Dialog_CharacterFurnitures, DIALOG_STYLE_LIST, @("Lista mobili piazzati"), str, "Modifica", "Indietro");
    }

    MySQL_TQueryInline(gMySQL, using inline OnSelect, "SELECT * FROM `furnitures` WHERE world = '%d' AND object_id != '0'", world);
    return 1;
}

Dialog:Dialog_CharacterFurnitures(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
        if(FurniturePage{playerid} <= 0)
            return pc_cmd_arredamento(playerid, "");
        else
            return Character_ShowFurnitures(playerid, FurniturePage{playerid}-1);
    }
    if(listitem == 0)
        return Character_ShowFurnitures(playerid, FurniturePage{playerid}+1);
    
    new slot = listitem - 1;
    SelectedFurniture[playerid] = FurnitureListItem[playerid][slot];
    Furniture_ShowOptions(playerid);
    return 1;
}

Dialog:Dialog_FurnitureOptions(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
        Character_ShowFurnitures(playerid, FurniturePage{playerid});
        SelectedFurniture[playerid] = -1;
        return 1;   
    }
    
    new 
        selected = SelectedFurniture[playerid],
        Float:x, Float:y, Float:z;
    
    GetDynamicObjectPos(selected, x, y, z);

    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, selected, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);

    switch(listitem)
    {
        case 0:
        {
            if(GetPlayerDistanceFromPoint(playerid, x, y, z) > 25.0)
                return SendClientMessage(playerid, COLOR_ERROR, "Sei troppo distante dall'oggetto.");
            SendClientMessage(playerid, COLOR_WHITE, "Premi {FF6347}ESC{FFFFFF} per annullare e {FF6347}SPAZIO{FFFFFF} per spostare la visuale. Clicca sul {FF6347}FLOPPY{FFFFFF} per salvare.");
            EditObject_Start(playerid, EDIT_TYPE_FURNITURE, selected, FurnitureCustomData[DATABASE_ID]);
        }
        case 1:
        {
            Dialog_Show(playerid, Dialog_RenameFurniture, DIALOG_STYLE_INPUT, "Rinomina", "Inserisci il nuovo nome per il mobile:", "Rinomina", "Chiudi");
        }
        case 2:
        {
            Dialog_Show(playerid, Dialog_SellFurniture, DIALOG_STYLE_MSGBOX, "Rimuovi", "Sei sicuro di voler rimuovere questo mobile (%s)?", "Rimuovi", "Annulla", FurnitureCustomData[FURNITURE_NAME]);
        }
        case 3: // GOTO
        {
            SetPlayerPos(playerid, x, y, z);
            SetPlayerInterior(playerid, Streamer_GetIntData(STREAMER_TYPE_OBJECT, selected, E_STREAMER_INTERIOR_ID));
            SetPlayerVirtualWorld(playerid, Streamer_GetIntData(STREAMER_TYPE_OBJECT, selected, E_STREAMER_WORLD_ID));
            SendFormattedMessage(playerid, COLOR_GREEN, "Ti sei gotato al mobile id %d.", selected);
        }
    }
    return 1;
}

Dialog:Dialog_RenameFurniture(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Furniture_ShowOptions(playerid);
    if(strlen(inputtext) < 4 || strlen(inputtext) > 63)
        return Dialog_Show(playerid, Dialog_RenameFurniture, DIALOG_STYLE_INPUT, "Rinomina", "{FF0000}Il nome inserito non è valido (Troppo lungo o troppo corto).{FFFFFF}\nInserisci il nuovo nome per il mobile:", "Rinomina", "Chiudi");
    
    new selected = SelectedFurniture[playerid];

    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, selected, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
    set(FurnitureCustomData[FURNITURE_NAME], inputtext);
    Streamer_SetArrayData(STREAMER_TYPE_OBJECT, selected, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);

    mysql_tquery_f(gMySQL, "UPDATE furnitures SET name = '%e' WHERE id = '%d'", inputtext, FurnitureCustomData[DATABASE_ID]);

    return 1;
}


Dialog:Dialog_SellFurniture(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Furniture_ShowOptions(playerid);
    new selected = SelectedFurniture[playerid];
    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, selected, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);

    //Character_GiveMoney(playerid, FurnitureCustomData[FURNITURE_PRICE]/2, "furniture _ sell");

    Player_Info(playerid, "Hai ~r~rimosso~w~ il mobile.", true, 5000);

    if(FurnitureCustomData[DATABASE_ID] > 0)
    {
        mysql_tquery_f(gMySQL, "DELETE FROM furnitures WHERE id = '%d' AND object_id = '%d';", FurnitureCustomData[DATABASE_ID], selected);
    }

    FurnitureCustomData[DATABASE_ID] = 0;
    FurnitureCustomData[FURNITURE_PRICE] = 0;
    FurnitureCustomData[FURNITURE_NAME] = EOS;

    Streamer_SetArrayData(STREAMER_TYPE_OBJECT, selected, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);



    DestroyDynamicObject(selected);
    SelectedFurniture[playerid] = -1;
    pc_cmd_arredamento(playerid, "");
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
    if(Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData) && FurnitureCustomData[DATABASE_ID] == 0)
    {
        new
            Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz,
            name[64];
        
        BuyableFurniture_GetName(furnitureid, name);
        GetDynamicObjectPos(objectid, x, y, z);
        GetDynamicObjectRot(objectid, rx, ry, rz);
        
        inline OnInsert()
        {
            new tname[64];
            BuyableFurniture_GetName(furnitureid, tname);
            FurnitureCustomData[DATABASE_ID] = cache_insert_id();
            FurnitureCustomData[FURNITURE_PRICE] = BuyableFurniture_GetPrice(furnitureid);
            set(FurnitureCustomData[FURNITURE_NAME], tname);
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
    //if(Character_GetMoney(playerid) < BuyableFurniture_GetPrice(furnitureid))
        //return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza denaro per acquistare questo mobile.");

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

    //if(Character_GetMoney(playerid) < BuyableFurniture_GetPrice(furnitureid))
        //return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza denaro per acquistare questo mobile.");

    Furniture_Store(objectid, furnitureid);

    //Character_GiveMoney(playerid, -BuyableFurniture_GetPrice(furnitureid), "furniture buy");

    new name[64];
    BuyableFurniture_GetName(furnitureid, name);
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai piazzato questo mobile (%s) con successo.", name);

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

stock Furniture_ShowOptions(playerid)
{
    if(Account_GetAdminLevel(playerid) > 1)
    {
        Dialog_Show(playerid, Dialog_FurnitureOptions, DIALOG_STYLE_LIST, "Opzioni mobili", "Sposta\nRinomina\nRimuovi\nGotati", "Seleziona", "Indietro");
    }
    else
    {
        Dialog_Show(playerid, Dialog_FurnitureOptions, DIALOG_STYLE_LIST, "Opzioni mobili", "Sposta\nRinomina\nRimuovi", "Seleziona", "Indietro");
    }
    return 1;
}

stock Character_CanFurnish(playerid)
{
    new currentHouse = Character_GetHouseID(playerid);
    if(currentHouse != -1 && House_GetOwnerID(currentHouse) == Character_GetID(playerid) && IsPlayerInRangeOfHouseExit(playerid, currentHouse, 50.0))
    {
        return 1;
    }
    /*for_house(h)
    {
        if(House_GetOwnerID(h) == Character_GetID(playerid) && IsPlayerInRangeOfHouseEntrance(playerid, h, 5.0))
            return 1;
    }*/
    return 0;
}

stock Character_GetFurnitureSlot(playerid)
{
    static const limit[] = {40, 60, 75, 100};

    //MySQL_TQueryInline(gMySQL, using inline OnSelect, "SELECT FurnitureSlots FROM Account WHERE ID = '%d';", Account_GetID(playerid));

    mysql_query(MySQL:handle, const query[], bool:use_cache = true)

    return limit[Account_GetPremiumLevel(playerid)];
}