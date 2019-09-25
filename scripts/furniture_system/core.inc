#include <furniture_system\furnitures_list>
#include <furniture_system\components\door>
/*
    TODO: Must fix local ids (/furnitureid) when editing exteriors
    Should I create a separate Map for exterior maps? NO
    
*/

#include <YSI_Coding\y_hooks>

#define EXTERIOR_WORLD              (0)

#define MAX_FURNITURE_PER_PAGE      (15)
#define MAX_EXTERIOR_DISTANCE       (10.0)
#define MAX_EXTERIOR_OBJECTS        (10)

static 
    FurniturePage[MAX_PLAYERS char] = {0, ...},
    SelectedFurniture[MAX_PLAYERS] = {-1, ...},
    FurnitureLinkedTo[MAX_PLAYERS] = {-1, ...}, // Used by exterior editing. Says which house/building the current furniture is linked to.
    FurnitureWorldID[MAX_PLAYERS] = {-1, ...}, // Represents the worldid of our current editing world.
    //FurnitureListItem[MAX_PLAYERS][sizeof(BuyableFurnitures)],
    List:FurnitureListItem[MAX_PLAYERS],
    List:FurnitureLabels[MAX_PLAYERS],
    ShowingLabelsForWorld[MAX_PLAYERS] = {-1, ...},
    Map:Furnitures, // Map<worldid, Pool<objectid>>
    Map:ObjectIDToIndex // <objectid, pool index id> used with pool with all furnitures to avoid looping on each element to find the index.
;

static enum e_FurnitureCustomData
{
    FURNITURE_DATABASE_ID,
    FURNITURE_PRICE,
    FURNITURE_NAME[64],
    FURNITURE_CREATED_BY_CHARACTER,
    FURNITURE_CREATED_BY_ACCOUNT,
    FURNITURE_LAST_EDIT_ACCOUNT,
    FURNITURE_LAST_EDIT_CHARACTER,
    FURNITURE_LINKED_TO, // Used by Exterior's furnitures.
    bool:FURNITURE_IS_OPENED,
};
static 
    FurnitureCustomData[e_FurnitureCustomData],
    szCustomData = e_FurnitureCustomData;

#include <furniture_system\commands>

hook OnGameModeInit()
{
    Furnitures = map_new();
    ObjectIDToIndex = map_new();

    mysql_tquery(gMySQL, "UPDATE `furnitures` SET object_id = '0' WHERE 1");
    Furniture_Load(0);

    return 1;
}

hook OnPlayerClearData(playerid)
{
    FurniturePage{playerid} = 0;
    SelectedFurniture[playerid] = -1;
    FurnitureLinkedTo[playerid] = -1;
    ShowingLabelsForWorld[playerid] = -1;
    FurnitureWorldID[playerid] = -1;
    if(list_valid(FurnitureListItem[playerid]))
        list_delete(FurnitureListItem[playerid]);
    if(Furniture_IsShowingInfo(playerid))
    {
        Furniture_DeleteLabels(playerid);
    }

    if(EditObject_GetType(playerid) == EDIT_TYPE_BUYING_FURNITURE)
    {
        Furniture_CancelBuy(playerid);
    }
    return 1;
}

hook OnPlyVirtualWorldChange(playerid, oldWorld, newWorld)
{
    if(newWorld >= HOUSE_START_WORLD)
    {
        Furniture_Load(newWorld);
    }
    if(oldWorld >= HOUSE_START_WORLD && newWorld != oldWorld)
    {
        if(EditObject_IsEditing(playerid))
        {
            if(EditObject_GetType(playerid) == EDIT_TYPE_BUYING_FURNITURE)
            {
                Furniture_CancelBuy(playerid);
            }
            else if(EditObject_GetType(playerid) == EDIT_TYPE_FURNITURE)
            {
                SendClientMessage(playerid, COLOR_ERROR, "Hai annullato le modifiche apportate al mobile.");
                EditObject_End(playerid);
            }
        }
        if(Furniture_IsShowingInfo(playerid))
        {
            Furniture_DeleteLabels(playerid);
        }
    }
    return 1;
}

static stock Internal_ValidateObjectPos(objectid, Float:newX, Float:newY, Float:newZ)
{
    if(!IsValidDynamicObject(objectid))
        return 0;
    
    if(!Furniture_IsExterior(objectid))
        return 1;

    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);

    new 
        linked = FurnitureCustomData[FURNITURE_LINKED_TO],
        Float:x, Float:y, Float:z
    ;
    printf("Linked: %d - House id: %d", linked, linked - HOUSE_START_WORLD);
    
    if(linked >= BUILDING_START_WORLD)
    {
        Building_GetEnterPos(linked - BUILDING_START_WORLD, x, y, z);
    }
    else if(linked >= HOUSE_START_WORLD)
    {
        House_GetEnterPosition(linked - HOUSE_START_WORLD, x, y, z);
    }
    new Float:dist = (x - newX) * (x - newX) + (y - newY) * (y - newY) + (z - newZ) * (z - newZ);
    if(dist > MAX_EXTERIOR_DISTANCE * MAX_EXTERIOR_DISTANCE) //avoid floatsqrt call. So multiply MAX_EXTERIOR_DISTANCE by itself
    {
        return 0;
    }
    return 1;
}

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

            if(!Internal_ValidateObjectPos(objectid, x, y, z))
                return SendClientMessage(playerid, COLOR_ERROR, "Non puoi piazzare l'oggetto troppo lontano dalla tua casa."), Furniture_CancelBuy(playerid), 0;

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
            if(!Internal_ValidateObjectPos(objectid, x, y, z) && !Player_IsAdminDuty(playerid))
            {
                SetDynamicObjectPos(objectid, tx, ty, tz);
                SetDynamicObjectRot(objectid, trx, try, trz);
                return SendClientMessage(playerid, COLOR_ERROR, "Non puoi piazzare l'oggetto troppo lontano dalla tua casa."), EditObject_End(playerid), 0;
            }

            Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
            SetDynamicObjectPos(objectid, x, y, z);
            SetDynamicObjectRot(objectid, rx, ry, rz);
            
            Furniture_Update(objectid, playerid);

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
        Furniture_ShowCategoryList(playerid);
    }
    return 1;
}

Dialog:Dialog_CategorySelect(playerid, response, listitem, inputtext[])
{
    if(!response)
        return pc_cmd_arredamento(playerid, "");
    
    if(listitem == 0) // Inserisci ID Manualmente
    {
        Dialog_Show(playerid, Dialog_ManualFurnitureID, DIALOG_STYLE_INPUT, "Inserisci l'ID dell'arredamento", "Inserisci l'objectid che vuoi piazzare.\nRicorda di seguire le regole e di evitare abusi.", "Piazza", "Annulla");
        return 1;
    }

    listitem = listitem-1; // If category, they're choosen from id 0.

    if(!list_valid(FurnitureListItem[playerid]))
        FurnitureListItem[playerid] = list_new();
    
    list_clear(FurnitureListItem[playerid]);

    new String:dyn = @("Mobile\n");
    new name[64];
    for(new i = 0, j = sizeof(BuyableFurnitures); i < j; ++i)
    {
        if(BuyableFurniture_GetCategory(i) != listitem)
            continue;
        BuyableFurniture_GetName(i, name);
        dyn += str_format("%s\n", name);
        //FurnitureListItem[playerid][count++] = i;
        list_add(FurnitureListItem[playerid], i);
    }
    Dialog_Show_s(playerid, Dialog_FurnitureBuy, DIALOG_STYLE_TABLIST_HEADERS, @("Arredamento"), dyn, "Acquista", "Chiudi");
    return 1;
}

Dialog:Dialog_ManualFurnitureID(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    new objectid = strval(inputtext);
    if(objectid <= 0)
        return Dialog_Show(playerid, Dialog_ManualFurnitureID, DIALOG_STYLE_INPUT, "L'ID inserito non è valido.\nInserisci l'ID dell'arredamento", "Inserisci l'objectid che vuoi piazzare.\nRicorda di seguire le regole e di evitare abusi.", "Piazza", "Annulla");
    
    SelectedFurniture[playerid] = 0; // First

    Furniture_StartBuy(playerid, objectid);
    return 1;
}


Dialog:Dialog_FurnitureBuy(playerid, response, listitem, inputtext[])
{
    new selected = list_get(FurnitureListItem[playerid], listitem);
    list_delete(FurnitureListItem[playerid]);
    if(!response)
        return 0;
    SelectedFurniture[playerid] = selected;
    Furniture_StartBuy(playerid, BuyableFurniture_GetModel(SelectedFurniture[playerid]));
    return 1;
}

Dialog:Dialog_CharacterFurnitures(playerid, response, listitem, inputtext[])
{
    new 
        slot = listitem, 
        listSize = list_size(FurnitureListItem[playerid]),
        selected = 0 <= slot < listSize ? list_get(FurnitureListItem[playerid], slot) : -1
    ;
    printf("List Size: %d", listSize);
    list_delete(FurnitureListItem[playerid]);

    if(!response)
    {
        if(FurniturePage{playerid} <= 0)
            return pc_cmd_arredamento(playerid, "");
        else
            return Character_ShowFurnitures(playerid, FurniturePage{playerid}-1);
    }
    if(listitem == listSize)
        return Character_ShowFurnitures(playerid, FurniturePage{playerid}+1);
    else if(listitem == listSize+1)
        return Character_ShowFurnitures(playerid, FurniturePage{playerid}-1);
    
    SelectedFurniture[playerid] = selected;
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
            EditObject_Start(playerid, EDIT_TYPE_FURNITURE, selected, FurnitureCustomData[FURNITURE_DATABASE_ID]);
        }
        case 1:
        {
            Dialog_Show(playerid, Dialog_RenameFurniture, DIALOG_STYLE_INPUT, "Rinomina", "Inserisci il nuovo nome per il mobile:", "Rinomina", "Chiudi");
        }
        case 2:
        {
            Dialog_Show(playerid, Dialog_SellFurniture, DIALOG_STYLE_MSGBOX, "Rimuovi", "Sei sicuro di voler rimuovere questo mobile (%s)?", "Rimuovi", "Annulla", FurnitureCustomData[FURNITURE_NAME]);
        }
        case 3:
        {
            Furniture_RemoveTextures(selected);
            SendClientMessage(playerid, COLOR_GREEN, "Hai rimosso le texture al mobile.");
            Furniture_ShowOptions(playerid);
        }
        case 4: // GOTO
        {
            if(!Player_IsAdminDuty(playerid))
                return 0;
            SetPlayerPos(playerid, x, y, z);
            SetPlayerInterior(playerid, Streamer_GetIntData(STREAMER_TYPE_OBJECT, selected, E_STREAMER_INTERIOR_ID));
            Character_SetVirtualWorld(playerid, Streamer_GetIntData(STREAMER_TYPE_OBJECT, selected, E_STREAMER_WORLD_ID));
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

    if(Furniture_Rename(Character_GetVirtualWorld(playerid), selected, inputtext))
    {
        return SendFormattedMessage(playerid, COLOR_GREEN, "Hai rinominato l'oggetto. Nuovo nome: %s", inputtext);
    }
    return SendClientMessage(playerid, COLOR_ERROR, "Non è stato possibile rinominare l'oggetto. ID invalido.");
}


Dialog:Dialog_SellFurniture(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Furniture_ShowOptions(playerid);
    new selected = SelectedFurniture[playerid];
    
    Furniture_Remove(playerid, selected);

    SelectedFurniture[playerid] = -1;
    pc_cmd_arredamento(playerid, "");
    return 1;
}

stock Furniture_CreateTemporary(modelid, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, worldid, interior, linked_to)
{
    new objectid = CreateDynamicObject(modelid, x, y, z, rx, ry, rz, worldid, interior);
    FurnitureCustomData[FURNITURE_DATABASE_ID] = 0;
    FurnitureCustomData[FURNITURE_PRICE] = 0;
    FurnitureCustomData[FURNITURE_LINKED_TO] = linked_to;
    Streamer_SetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
    return objectid;
}

// CreatedBy = -1 means that Server created it.
stock Furniture_Store(objectid, furnitureid, createdBy = -1, linked_to = -1)
{
    if(Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData) && FurnitureCustomData[FURNITURE_DATABASE_ID] == 0)
    {
        new
            Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz,
            name[64],
            account_id = -1,
            character_id = -1;
        
        if(createdBy >= 0 && Character_IsLogged(createdBy))
        {
            account_id = Account_GetID(createdBy);
            character_id = Character_GetID(createdBy);
        }

        BuyableFurniture_GetName(furnitureid, name);
        GetDynamicObjectPos(objectid, x, y, z);
        GetDynamicObjectRot(objectid, rx, ry, rz);
        
        linked_to = linked_to == -1 ? Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_WORLD_ID) : linked_to;

        inline OnInsert()
        {
            new tname[64];
            BuyableFurniture_GetName(furnitureid, tname);
            FurnitureCustomData[FURNITURE_DATABASE_ID] = cache_insert_id();
            FurnitureCustomData[FURNITURE_PRICE] = BuyableFurniture_GetPrice(furnitureid);
            FurnitureCustomData[FURNITURE_LAST_EDIT_ACCOUNT] = FurnitureCustomData[FURNITURE_CREATED_BY_ACCOUNT] = account_id;
            FurnitureCustomData[FURNITURE_LAST_EDIT_CHARACTER] = FurnitureCustomData[FURNITURE_CREATED_BY_CHARACTER] = character_id;
            FurnitureCustomData[FURNITURE_LINKED_TO] = linked_to;
            set(FurnitureCustomData[FURNITURE_NAME], tname);
            Streamer_SetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData);

            Furniture_AddToWorld(objectid, Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_WORLD_ID));
        }

        MySQL_TQueryInline(gMySQL, using inline OnInsert, "INSERT INTO `furnitures` \
        (object_id, world, interior, model, price, pos_x, pos_y, pos_z, rot_x, rot_y, rot_z, name, \
        created_by_account, created_by_character, last_edit_by_account, last_edit_by_character, linked_to) \
        VALUES('%d', '%d', '%d', '%d', '%d', '%f', '%f', '%f', '%f', '%f', '%f', '%e', '%d', '%d', '%d', '%d', '%d')",
        objectid,
        Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_WORLD_ID),
        Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_INTERIOR_ID),
        Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_MODEL_ID),
        0,
        x, y, z,
        rx, ry, rz,
        name,
        account_id,
        character_id,
        account_id,
        character_id,
        linked_to);


        return 1;
    }
    return 0;
}

stock Furniture_Load(worldid)
{
    if(Furniture_AreLoaded(worldid))
        return printf("Furnitures already loaded for world %d.", worldid);
    
    map_add(Furnitures, worldid, pool_new());
    inline OnLoad()
    {
        
        /*
        // Commented because we create Pool and then add it to Furnitures map.
        new rows = cache_num_rows();
        if(!rows)
            return 1;*/
        new rows = cache_num_rows();
        new modelid, vw, interior,
            Float:x, Float:y, Float:z,
            Float:rx, Float:ry, Float:rz
            ;
        new Pool:pool = Pool:map_get(Furnitures, worldid);

        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", FurnitureCustomData[FURNITURE_DATABASE_ID]);
            cache_get_value_name_int(i, "world", vw);
            cache_get_value_name_int(i, "interior", interior);
            cache_get_value_name_int(i, "model", modelid);
            cache_get_value_name_int(i, "price", FurnitureCustomData[FURNITURE_PRICE]);
            
            cache_get_value_name_float(i, "pos_x", x);
            cache_get_value_name_float(i, "pos_y", y);
            cache_get_value_name_float(i, "pos_z", z);

            cache_get_value_name_float(i, "rot_x", rx);
            cache_get_value_name_float(i, "rot_y", ry);
            cache_get_value_name_float(i, "rot_z", rz);

            cache_get_value_name(i, "name", FurnitureCustomData[FURNITURE_NAME]);

            cache_get_value_name_int(i, "created_by_account", FurnitureCustomData[FURNITURE_CREATED_BY_ACCOUNT]);
            cache_get_value_name_int(i, "created_by_character", FurnitureCustomData[FURNITURE_CREATED_BY_CHARACTER]);
            cache_get_value_name_int(i, "last_edit_by_account", FurnitureCustomData[FURNITURE_LAST_EDIT_ACCOUNT]);
            cache_get_value_name_int(i, "last_edit_by_character", FurnitureCustomData[FURNITURE_LAST_EDIT_CHARACTER]);

            cache_get_value_name_int(i, "linked_to", FurnitureCustomData[FURNITURE_LINKED_TO]);

            new objectid = CreateDynamicObject(modelid, x, y, z, rx, ry, rz, vw, interior);
            if(IsOpenableDoor(modelid))
                printf("Create object: %d - ID: %d", objectid, FurnitureCustomData[FURNITURE_DATABASE_ID]);
            Furniture_LoadTextures(objectid);

            Streamer_SetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
            
            Furniture_SetObject(objectid, FurnitureCustomData[FURNITURE_DATABASE_ID]);

            map_add(ObjectIDToIndex, objectid, pool_add(pool, objectid));
        }
        
        map_add(Furnitures, worldid, pool);

        //printf("%d furnitures loaded for world %d.", rows, worldid);
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM furnitures WHERE world = '%d' AND object_id = '0';", worldid);

    return 1;
}

// Checks if furnitures are loaded for a specific world
stock Furniture_AreLoaded(world)
{
    return map_valid(Furnitures) && map_has_key(Furnitures, world) && pool_valid(Pool:map_get(Furnitures, world));
}

stock Furniture_SetObject(objectid, dbid)
{
    mysql_tquery_f(gMySQL, "UPDATE furnitures SET object_id = '%d' WHERE id = '%d'", objectid, dbid);
    return 1;
}

stock Furniture_StartBuy(playerid, modelid)
{
    //if(Character_GetMoney(playerid) < BuyableFurniture_GetPrice(furnitureid))
        //return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza denaro per acquistare questo mobile.");

    new maxFurnitures = 0;
    if(Character_GetVirtualWorld(playerid) == EXTERIOR_WORLD) // Exterior
    {
        maxFurnitures = Character_GetExtFurnitureSlot(playerid);
    }
    else
    {
        maxFurnitures = Character_GetFurnitureSlot(playerid);
    }
    
    //printf("Max Furnitures: %d", Character_GetFurnitureSlot(playerid));
    //printf("Current Furniture Count: %d", Character_GetFurnitureCount(playerid));

    if(Character_GetFurnitureCount(playerid) >= maxFurnitures)
        return SendFormattedMessage(playerid, COLOR_ERROR, "Hai raggiunto il limite di mobili per questo edificio (%d).", maxFurnitures);

    new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
    GetXYInFrontOfPlayer(playerid, x, y, 2);
    
    new objectid = Furniture_CreateTemporary(modelid, x + 3.0, y, z + 1.0, 0.0, 0.0, 0.0, Character_GetVirtualWorld(playerid), GetPlayerInterior(playerid), FurnitureLinkedTo[playerid]);
    EditObject_Start(playerid, EDIT_TYPE_BUYING_FURNITURE, objectid, 0);

    SendClientMessage(playerid, -1, "Premi {FF6347}ESC{FFFFFF} per annullare e {FF6347}SPAZIO{FFFFFF} per spostare la visuale. Clicca sul {FF6347}FLOPPY{FFFFFF} per salvare.");
    return 1;
}

stock Furniture_FinishBuy(playerid, objectid)
{
    new furnitureid = SelectedFurniture[playerid];

    Furniture_Store(objectid, furnitureid, playerid, FurnitureLinkedTo[playerid]);

    new name[64];
    BuyableFurniture_GetName(furnitureid, name);

    SendFormattedMessage(playerid, COLOR_GREEN, "Hai piazzato questo mobile (%s) con successo.", name);

    EditObject_End(playerid);

    Furniture_UpdateInfoList(playerid);
    return 1;
}

stock Furniture_CancelBuy(playerid)
{
    new objectid = EditObject_GetObjectID(playerid);

    if(IsValidDynamicObject(objectid))
        DestroyDynamicObject(objectid);
    
    SendClientMessage(playerid, -1, "Hai cancellato l'acquisto del mobile.");
    
    EditObject_End(playerid);
    return 1;
}

stock Furniture_ShowOptions(playerid)
{
    new String:s = @("Sposta\nRinomina\nRimuovi\nResetta Textures\n");
    if(Account_GetAdminLevel(playerid) > 1)
    {
        s += @("Gotati");
    }
    
    Dialog_Show_s(playerid, Dialog_FurnitureOptions, DIALOG_STYLE_LIST, @("Opzioni mobili"), s, "Seleziona", "Indietro");
    return 1;
}

stock Furniture_IsShowingInfo(playerid)
{
    return list_valid(FurnitureLabels[playerid]);//list_valid(FurnitureLabels[playerid]);
}

stock Furniture_Update(objectid, playerid)
{
    if(!IsValidDynamicObject(objectid))
        return 0;
    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);

    FurnitureCustomData[FURNITURE_LAST_EDIT_ACCOUNT] = Account_GetID(playerid);
    FurnitureCustomData[FURNITURE_LAST_EDIT_CHARACTER] = Character_GetID(playerid);

    Streamer_SetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);

    new 
        Float:tx, Float:ty, Float:tz, 
        Float:trx, Float:try, Float:trz;
    GetDynamicObjectPos(objectid, tx, ty, tz);
    GetDynamicObjectRot(objectid, trx, try, trz);

    mysql_tquery_f(gMySQL, "UPDATE furnitures SET \
            pos_x = '%f', pos_y = '%f', pos_z = '%f', \
            rot_x = '%f', rot_y = '%f', rot_z = '%f', \
            last_edit_by_account = '%d', last_edit_by_character = '%d' \
            WHERE id = '%d'", 
            tx, ty, tz,
            trx, try, trz,
            Account_GetID(playerid),
            Character_GetID(playerid),
            FurnitureCustomData[FURNITURE_DATABASE_ID]);
    
    Furniture_UpdateInfoList(playerid);
    return 1;
}

// Adds (or update if objectid key exists) a text label.
stock Furniture_UpdateInfoList(playerid)
{
    if(!Furniture_IsShowingInfo(playerid))
        return 0;
    new world = Character_GetVirtualWorld(playerid),
        Pool:furnitures = Pool:map_get(Furnitures, world);
    
    if(!pool_valid(furnitures))
        return 0;

    for_list(i : FurnitureLabels[playerid])
    {
        new Text3D:label = Text3D:iter_get(i);
        DestroyDynamic3DTextLabelEx(label);
    }
    
    list_clear(FurnitureLabels[playerid]);

    if(ShowingLabelsForWorld[playerid] == -1)
    {
        if(world == 0 && !Player_IsAdminDuty(playerid))
            ShowingLabelsForWorld[playerid] = Internal_GetNearbyFurnableWorld(playerid);
        else
            ShowingLabelsForWorld[playerid] = world;

    }

    printf("Showing ids for world %d", ShowingLabelsForWorld[playerid]);

    new text[128 + MAX_PLAYER_NAME], Text3D:labelid;
    for_pool(i : furnitures)
    {
        new objectid = iter_get_value(i), index = iter_get_key(i);

        Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);

        if(FurnitureCustomData[FURNITURE_LINKED_TO] != ShowingLabelsForWorld[playerid])
            continue;

        new Float:x, Float:y, Float:z;
        GetDynamicObjectPos(objectid, x, y, z);
        
        format(text, sizeof(text), "%s\nID: %d\n(( Ultima Modifica Da: %s ))", FurnitureCustomData[FURNITURE_NAME], index, Character_GetNameFromDatabase(FurnitureCustomData[FURNITURE_LAST_EDIT_CHARACTER], true));
        
        if(Account_GetAdminLevel(playerid) > 1 && Player_IsAdminDuty(playerid))
        {
            format(text, sizeof(text), "%s\n{FF6347}(( Account: %s )){FFFFFF}\n{FF6347}(( Object ID: %d )){FFFFFF}", text, Account_GetNameFromDatabase(Account_GetID(playerid)), objectid);
        }

        labelid = CreateDynamic3DTextLabel(text, -1, x, y, z + 0.2, 20.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, Character_GetVirtualWorld(playerid), GetPlayerInterior(playerid), playerid);  

        list_add(FurnitureLabels[playerid], labelid);
    }
    
    return 1;
}

// Returns furning house
stock Character_CanFurnish(playerid, bool:outsideCheck = false)
{
    new currentHouse = Character_GetHouseID(playerid);
    if(outsideCheck && !House_IsValid(currentHouse))
    {
        new Float:x, Float:y, Float:z;
        for_map(i : GetHousesMap())
        {
            new houseid = iter_get_key(i);
            
            // Can't build inside houses with enter world != 0 (houses in complex, example)
            if(House_GetEnterWorld(houseid) != 0)
                continue;
            
            if( (House_GetOwnerID(houseid) == Character_GetID(playerid) || Player_IsAdminDuty(playerid)) && Character_GetVirtualWorld(playerid) == House_GetEnterWorld(houseid) && GetPlayerInterior(playerid) == House_GetEnterInterior(houseid))
            {
                House_GetEnterPosition(houseid, x, y, z);
                if(GetPlayerDistanceFromPoint(playerid, x, y, z) >= 15.0)
                    continue;
                return houseid;
            }
        }
    }
    if(House_IsValid(currentHouse) && (House_GetOwnerID(currentHouse) == Character_GetID(playerid) || Player_IsAdminDuty(playerid)))
    {
        return currentHouse;
    }
    return INVALID_HOUSE_ID;
}

stock Character_GetFurnitureSlot(playerid)
{
    static const limit[] = {200, 300, 400, 600};

    new slots = limit[Account_GetPremiumLevel(playerid)];
    new Cache:cache = mysql_query_f(gMySQL, true, "SELECT FurnitureSlots FROM Account WHERE ID = '%d';", Account_GetID(playerid));
    if(cache != MYSQL_INVALID_CACHE && cache_num_rows() > 0)
    {
        new amount = 0;
        cache_get_value_index_int(0, 0, amount);
        slots += amount;
    }
    cache_delete(cache);
    return slots;
}

stock Character_GetExtFurnitureSlot(playerid)
{
    static const limit[] = {5, 5, 10, 20};
    return limit[Account_GetPremiumLevel(playerid)];
}

stock Character_GetFurnitureCount(playerid)
{
    new worldid = Character_GetVirtualWorld(playerid);
    if(map_has_key(Furnitures, worldid))
    {
        new Pool:pool = Pool:map_get(Furnitures, worldid);
        if(worldid == 0) // So exterior
        {
            return Furniture_GetExtCount(FurnitureLinkedTo[playerid]);
        }
        return pool_size(pool);
    }
    return 0;
}

stock Furniture_Remove(playerid, objectid)
{
    if(Internal_RemoveFurniture(Character_GetVirtualWorld(playerid), objectid))
    {
        Player_Info(playerid, "Hai ~r~rimosso~w~ il mobile.", true, 5000);
        Furniture_UpdateInfoList(playerid);
    }
    return 1;
}

stock bool:Furniture_RemoveByObjectID(objectid)
{
    if(!IsValidDynamicObject(objectid) || !Furniture_IsValid(objectid))
        return false;
    return Internal_RemoveFurniture(Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_WORLD_ID), objectid) >= 1 ? true : false;
}

// Talking on Discord with a person


stock Internal_RemoveFurniture(worldid, objectid)
{
    if(!IsValidDynamicObject(objectid))
        return 0;
    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);

    if(FurnitureCustomData[FURNITURE_DATABASE_ID] > 0)
    {
        mysql_tquery_f(gMySQL, "DELETE FROM furnitures WHERE id = '%d';", FurnitureCustomData[FURNITURE_DATABASE_ID]);
    }

    new data[e_FurnitureCustomData];
    FurnitureCustomData = data;
    Streamer_SetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);

    Furniture_RemoveTextures(objectid);

    DestroyDynamicObject(objectid);

    if(map_has_key(Furnitures, worldid) && map_has_key(ObjectIDToIndex, objectid))
    {
        new Pool:pool = Pool:map_get(Furnitures, worldid);
        pool_remove(pool, map_get(ObjectIDToIndex, objectid));
        map_remove(ObjectIDToIndex, objectid);
    }
    return 1;
}

stock Furniture_AddToWorld(objectid, worldid)
{
    if(map_has_key(Furnitures, worldid))
    {
        new Pool:pool = Pool:map_get(Furnitures, worldid);
        map_add(ObjectIDToIndex, objectid, pool_add(pool, objectid));
        foreach(new i : Player)
        {
            if(Character_GetVirtualWorld(i) != worldid)
                continue;
            Furniture_UpdateInfoList(i);
        }
        return 1;
    }
    return 0;
}

stock Furniture_ShowCategoryList(playerid)
{
    static const info[] = "Inserisci ID Manualmente\nMura e scale\nTavoli per cucine e salotti\nSedie da uffici e da cucina\nLetti singoli e matrimoniali\nDivani e poltrone\nDecorazioni\nElettronica\nVario\nCucina\nBagno\nBottiglie, bicchieri e tazze\nPorte\nVasi\nPoster e graffiti\nContenitori rifiuti\nVestiti\nBombole e barili\nOggetti per il mare\nQuadri\nScatole\nColonne\nCibo\nUtensili\nMeccanica\nSpeciali\nLavagne";
    Dialog_Show(playerid, Dialog_CategorySelect, DIALOG_STYLE_LIST, "Categoria", info, "Continua", "Indietro");
    return 1;
}

stock Furniture_IsValidLocalId(worldid, localid)
{
    new Pool:pool;
    
    if(!map_get_safe(Furnitures, worldid, pool) || !pool_has(pool, localid))
        return 0;
    
    return 1;
}

stock Furniture_GetFurnitureObjectID(worldid, localid)
{
    new Pool:pool;
    
    if(!map_get_safe(Furnitures, worldid, pool) || !pool_has(pool, localid))
        return -1;
    
    return pool_get(pool, localid);
}

stock Furniture_Rename(worldid, objectid, const name[])
{
    #pragma unused worldid
    if(!IsValidDynamicObject(objectid))
        return 0;
    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
    set(FurnitureCustomData[FURNITURE_NAME], name);
    Streamer_SetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);

    mysql_tquery_f(gMySQL, "UPDATE furnitures SET name = '%e' WHERE id = '%d'", name, FurnitureCustomData[FURNITURE_DATABASE_ID]);
    return 1;
}

stock Furniture_CreateLabels(playerid)
{
    FurnitureLabels[playerid] = list_new();
    Furniture_UpdateInfoList(playerid);
    return 1;
}

stock Furniture_DeleteLabels(playerid)
{
    for_list(i : FurnitureLabels[playerid])
    {
        DestroyDynamic3DTextLabel(Text3D:iter_get(i));
    }
    list_delete(FurnitureLabels[playerid]);
    ShowingLabelsForWorld[playerid] = -1;
    return 1;
}

stock Furniture_SetTexture(objectid, index, modelid, const txdName[], const textureName[], materialColor)
{
    if(!IsValidDynamicObject(objectid) || index < 0 || index > 15)
        return 0;
    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
    mysql_tquery_f(gMySQL, "INSERT INTO `furniture_textures` \
			(furniture_id, material_index, model_id, txd_name, texture_name, material_color) \
			VALUES('%d', '%d', '%d', '%s', '%s', '%d') \
			ON DUPLICATE KEY UPDATE \
			model_id = VALUES(model_id), \
            txd_name = VALUES(txd_name), \
            texture_name = VALUES(texture_name), \
            material_color = VALUES(material_color)", 
            FurnitureCustomData[FURNITURE_DATABASE_ID],
            index, modelid, txdName, textureName, materialColor);
    return SetDynamicObjectMaterial(objectid, index, modelid, txdName, textureName, materialColor);
}

stock Furniture_RemoveTextures(objectid)
{
    if(!IsValidDynamicObject(objectid))
        return 0;
    
    for(new i = 0; i < 15; ++i) 
        SetDynamicObjectMaterial(objectid, i, 19341, "invalid", "invalid");
    
    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
    mysql_tquery_f(gMySQL, "DELETE FROM furniture_textures WHERE furniture_id = '%d';", FurnitureCustomData[FURNITURE_DATABASE_ID]);
    return 1;
}

stock Furniture_LoadTextures(objectid)
{
    if(!IsValidDynamicObject(objectid))
        return 0;
    
    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);

    if(FurnitureCustomData[FURNITURE_DATABASE_ID] == 0)
        return 0;
    
    inline OnLoad()
    {
        new rows = cache_num_rows();
        new materialIndex, modelId, txdName[64], textureName[64], materialColor;
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_index_int(i, 1, materialIndex);
            cache_get_value_index_int(i, 2, modelId);
            cache_get_value_index(i, 3, txdName);
            cache_get_value_index(i, 4, textureName);
            cache_get_value_index_int(i, 5, materialColor);

            SetDynamicObjectMaterial(objectid, materialIndex, modelId, txdName, textureName, materialColor);
        }
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM furniture_textures WHERE furniture_id = '%d';", FurnitureCustomData[FURNITURE_DATABASE_ID]);
    return 1;
}

stock Internal_GetNearbyFurnableWorld(playerid)
{
    new 
        Float:x, Float:y, Float:z, 
        Float:nearest = MAX_EXTERIOR_DISTANCE, 
        Float:dist = 0.0,
        linked_to = -1;
    for_map(i : GetHousesMap())
    {
        new houseid = iter_get_key(i);
        if(House_GetOwnerID(houseid) != Character_GetID(playerid) || House_GetEnterWorld(houseid) != Character_GetVirtualWorld(playerid))
            continue;
        House_GetEnterPosition(houseid, x, y, z);
        dist = GetPlayerDistanceFromPoint(playerid, x, y, z);
        if(dist > MAX_EXTERIOR_DISTANCE)
            continue;
        if(dist < nearest)
        {
            nearest = dist;
            linked_to = House_GetExitWorld(houseid);
        }
    }
    return linked_to;
}

stock Character_ShowFurnitures(playerid, page)
{
    if(page < 0)
        return pc_cmd_arredamento(playerid, "");
    
    FurniturePage{playerid} = page;


    new 
        world = Character_GetVirtualWorld(playerid),
        linked_to = world
    ;
    

    if(world == 0) // Exterior?
    {
        // Find nearest house.
        linked_to = Internal_GetNearbyFurnableWorld(playerid);
        if(linked_to == -1)
            return SendClientMessage(playerid, COLOR_ERROR, "Non sei nelle vicinanze di una tua proprietà.");
    }
    
    new 
        Pool:furnituresInWorld = Pool:map_get(Furnitures, world), 
        rows = pool_size(furnituresInWorld)
    ;
    
    if(!pool_valid(furnituresInWorld))
        return SendClientMessage(playerid, COLOR_ERROR, "Non ci sono mobili.");
    
    FurnitureWorldID[playerid] = FurnitureLinkedTo[playerid] = linked_to;
    
    new String:str = @("");

    if(!list_valid(FurnitureListItem[playerid]))
        FurnitureListItem[playerid] = list_new();

    list_clear(FurnitureListItem[playerid]);

    // MODIFICARE QUESTO PERCHE' CON LE POOL NON FUNZIONERA' MAI
    //for(new i = page * MAX_FURNITURE_PER_PAGE; i < rows; ++i)
    new 
        objectid = 0,
        counter = 0,
        startIndex = page * MAX_FURNITURE_PER_PAGE,
        lastIndex = startIndex + MAX_FURNITURE_PER_PAGE,
        playerFurnituresCount
        ;
        
    for_pool(i : furnituresInWorld)
    {
        
        if(counter < startIndex)
        {
            counter++;
            continue;
        }

        if(counter == lastIndex)
            break;

        objectid = iter_get_value(i);
        
        if(Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_WORLD_ID) != world)
            continue;

        Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);

        if(FurnitureCustomData[FURNITURE_LINKED_TO] != linked_to)
            continue;

        new modelid = Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_MODEL_ID);
        str += str_format("%s [ID: %d - Model ID: %d]\n", FurnitureCustomData[FURNITURE_NAME], iter_get_key(i), modelid);

        list_add(FurnitureListItem[playerid], objectid);
        counter++;
        playerFurnituresCount++;
    }
    
    if(playerFurnituresCount <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non ci sono mobili piazzati.");

    if(rows > page * MAX_FURNITURE_PER_PAGE + MAX_FURNITURE_PER_PAGE)
        str += String:str_format(">>> Pagina %d\n", page + 2);
    if(FurnitureLinkedTo[playerid] >= BUILDING_START_WORLD)
        Dialog_Show_s(playerid, Dialog_CharacterFurnitures, DIALOG_STYLE_LIST, str_format("Lista mobili piazzati edificio %d", FurnitureLinkedTo[playerid] - BUILDING_START_WORLD), str, "Modifica", "Indietro");
    else if(FurnitureLinkedTo[playerid] >= HOUSE_START_WORLD)
        Dialog_Show_s(playerid, Dialog_CharacterFurnitures, DIALOG_STYLE_LIST, str_format("Lista mobili piazzati casa %d", FurnitureLinkedTo[playerid] - HOUSE_START_WORLD), str, "Modifica", "Indietro");
    return 1;
}

stock Furniture_IsExterior(objectid)
{
    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
    return FurnitureCustomData[FURNITURE_LINKED_TO] != Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_WORLD_ID);
}

stock House_RemoveExteriorFurnitures(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    new 
        worldid = HOUSE_START_WORLD + houseid,
        Pool:furnituresInWorld = Pool:map_get(Furnitures, 0),
        objectid = 0
    ;
    for_pool(i : furnituresInWorld)
    {
        objectid = iter_get_value(i);
        if(!IsValidDynamicObject(objectid))
            continue;
        Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
        if(FurnitureCustomData[FURNITURE_LINKED_TO] != worldid)
            continue;
        Internal_RemoveFurniture(0, objectid);
    }
    return 1;
}

// Should I delete Pool when removing all furnitures?
stock House_RemoveAllfurnitures(houseid)
{
    if(!House_IsValid(houseid))
        return 0;
    House_RemoveExteriorFurnitures(houseid);
    new 
        worldid = HOUSE_START_WORLD + houseid;
    if(!map_has_key(Furnitures, worldid))
        return 1;
    new
        Pool:furnituresInWorld = Pool:map_get(Furnitures, worldid),
        objectid = 0
    ;
    for_pool(i : furnituresInWorld)
    {
        objectid = iter_get_value(i);
        if(!IsValidDynamicObject(objectid))
            continue;
        Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
        if(FurnitureCustomData[FURNITURE_LINKED_TO] != worldid)
            continue;
        Internal_RemoveFurniture(worldid, objectid);
    }
    return 1;
}

stock Furniture_GetExtCount(linkedtoWorld)
{
    new 
        Pool:pool = Pool:map_get(Furnitures, 0),
        objectid,
        count = 0;
    for_pool(i : pool)
    {
        objectid = iter_get_value(i);
        if(objectid <= 0)
            continue;
        Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);  
        if(FurnitureCustomData[FURNITURE_LINKED_TO] != linkedtoWorld)
            continue;
        count++;
    }
    return count;
}

stock Character_SetFurnitureLinkedTo(playerid, worldid)
{
    FurnitureLinkedTo[playerid] = worldid;
    return 1;
}

stock Character_GetFurnitureLinkedTo(playerid)
{
    return FurnitureLinkedTo[playerid];
}

stock Pool:Furniture_GetAllInWorld(worldid)
{
    if(!map_has_key(Furnitures, worldid))
        return Pool:0;
    return Pool:map_get(Furnitures, worldid);
}

stock Furniture_SetOpened(objectid, bool:s)
{
    if(!IsValidDynamicObject(objectid))
        return 0;
    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
    FurnitureCustomData[FURNITURE_IS_OPENED] = s;
    Streamer_SetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
    return 1;
}

stock bool:Furniture_IsOpened(objectid)
{
    if(!IsValidDynamicObject(objectid))
        return false;
    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
    return FurnitureCustomData[FURNITURE_IS_OPENED];
}

stock bool:Furniture_IsValid(objectid)
{
    if(!IsValidDynamicObject(objectid))
        return false;
    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
    return FurnitureCustomData[FURNITURE_DATABASE_ID] > 0;
}