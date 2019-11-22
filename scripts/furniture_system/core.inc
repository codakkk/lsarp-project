
#if defined _furniture_included
	#endinput
#endif
#define _furniture_included

#define for_furnitures(%0:%1) for(new List:__def__list__ = List:map_get(GetFurnituresMap(), %1), Iter:%0=list_iter(__def__list__);iter_inside(%0);iter_move_next(%0))

#include <furniture_system\furnitures_list>
#include <furniture_system\components\door>
#include <furniture_system\commands>
/*
    Should I create a separate Map for exterior maps? NO
*/

#include <YSI_Coding\y_hooks>


#define MAX_FURNITURE_PER_PAGE      (15)
#define MAX_EXTERIOR_DISTANCE       (10.0)
#define MAX_EXTERIOR_OBJECTS        (10)

static enum e_FurnitureCustomData
{
    FURNITURE_DATABASE_ID,
    FURNITURE_PRICE,
    FURNITURE_NAME[64],
    FURNITURE_CREATED_BY_CHARACTER,
    FURNITURE_CREATED_BY_ACCOUNT,
    FURNITURE_LAST_EDIT_ACCOUNT,
    FURNITURE_LAST_EDIT_CHARACTER,
    bool:FURNITURE_IS_EXTERIOR,
    EntityType:FURNITURE_BELONGS_TO_TYPE,
    FURNITURE_BELONGS_TO_ID, // Unique entity id
    bool:FURNITURE_IS_OPENED,
};
static 
    FurnitureCustomData[e_FurnitureCustomData],
    szCustomData = e_FurnitureCustomData;

static enum _:E_FURNITURE_ENTITY_DATA
{
    List:All,
    List:Interior,
    List:Exterior
};

static Map:FurnitureByEntityType[EntityType];
static List:FurnitureLabels[MAX_PLAYERS];

hook OnGameModeInit()
{
    for(new EntityType:i = EntityType:0; i < EntityType; ++i)
	{
		FurnitureByEntityType[i] = map_new();
	}
    printf("%d - %d", _:ENTITY_TYPE_WORLD, _:EntityType);
    mysql_tquery(gMySQL, "UPDATE `furnitures` SET object_id = '0' WHERE 1;");
    /*mysql_tquery(gMySQL, "DELETE FROM `furnitures` WHERE 1;");
    mysql_tquery(gMySQL, "ALTER TABLE `furnitures` ADD belongs_to_type INT(11);");
    mysql_tquery(gMySQL, "ALTER TABLE `furnitures` ADD belongs_to_id INT(11);");
    mysql_tquery(gMySQL, "ALTER TABLE `furnitures` ADD is_exterior INT(11);");
    mysql_tquery(gMySQL, "ALTER TABLE `furnitures` DROP COLUMN linked_to;");*/
    Furniture_Load(ENTITY_TYPE_WORLD, 0);
    return 1;
}

hook OnHouseLoaded(House:house)
{
    if(!Furniture_AreLoaded(ENTITY_TYPE_HOUSE, House_GetID(house)))
    {
        Furniture_Load(ENTITY_TYPE_HOUSE, House_GetID(house));
    }
    return 1;
}

hook OnBuildingLoaded(Building:building)
{
    if(!Furniture_AreLoaded(ENTITY_TYPE_BUILDING, Building_GetID(building)))
    {
        Furniture_Load(ENTITY_TYPE_BUILDING, Building_GetID(building));
    }
    return 1;
}

hook OnCharacterDisconnected(playerid, reason)
{
    if(EditObject_GetType(playerid) == EDIT_TYPE_BUYING_FURNITURE)
    {
        Furniture_CancelBuy(playerid);
    }
    return 1;
}

hook OnCharacterClearData(playerid)
{
    Character_ToggleFurnitureLabels(playerid, false);
    return 1;
}

hook OnPlyVirtualWorldChange(playerid, oldWorld, newWorld)
{
    new EntityType:type, entityid;
    
    if(HOUSE_START_WORLD <= newWorld < BUILDING_START_WORLD)
    {
        type = ENTITY_TYPE_HOUSE;
        entityid = newWorld - HOUSE_START_WORLD;
    }
    else if(newWorld >= BUILDING_START_WORLD)
    {
        type = ENTITY_TYPE_BUILDING;
        entityid = newWorld - BUILDING_START_WORLD;
    }

    if(type != ENTITY_TYPE_NONE)
    {
        Furniture_Load(type, entityid);
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
        if(Furniture_IsShowingLabels(playerid))
        {
            Character_ToggleFurnitureLabels(playerid, false);
        }
    }
    return 1;
}

static stock Furniture_ValidatePos(objectid, Float:newX, Float:newY, Float:newZ)
{
    if(!IsValidDynamicObject(objectid))
        return 0;
    
    if(!Furniture_IsExterior(objectid))
        return 1;

    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);

    new 
        EntityType:entityType = FurnitureCustomData[FURNITURE_BELONGS_TO_TYPE],
        entityId = FurnitureCustomData[FURNITURE_BELONGS_TO_ID],
        Float:x, Float:y, Float:z
    ;

    if(!Entity_GetWorldPosition(entityType, entityId, x, y, z))
        return false;
    
    new Float:dist = (x - newX) * (x - newX) + (y - newY) * (y - newY) + (z - newZ) * (z - newZ);
    return !(dist > MAX_EXTERIOR_DISTANCE * MAX_EXTERIOR_DISTANCE);
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

            if(!Furniture_ValidatePos(objectid, x, y, z))
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
            if(!Furniture_ValidatePos(objectid, x, y, z) && !Player_IsAdminDuty(playerid))
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

            Character_UpdateLabels(playerid);
        }
    }
    return 1;
}

stock Character_ShowFurnitureDialog(playerid)
{
    new bool:isExterior, EntityType:entityType, entityid;

    new bool:furnable = Character_GetFurnitureWorld(playerid, isExterior, entityType, entityid, .onlyAllowed=true, .worldAllowed=true);

    if(!furnable)
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando qui.");
    
    new response[e_DIALOG_RESPONSE_INFO];

    yield 1;
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_LIST, @("Arredamento"), @("Mobili piazzati\nPiazza"), @("Avanti"), @("Chiudi"));

        if(!response[E_DIALOG_RESPONSE_Response]) break;

        if(response[E_DIALOG_RESPONSE_Listitem] == 0)
            ShowPlacedFurnituresDialog(playerid, isExterior, entityType, entityid);
        else
            ShowFurnituresCategoriesDialog(playerid, isExterior, entityType, entityid);
    }
    return true;
}

static ShowPlacedFurnituresDialog(playerid, bool:isExterior, EntityType:type, entity)
{
    new 
        response[e_DIALOG_RESPONSE_INFO],
        count = 0,
        startIndex = 0,
        lastIndex = 0,
        objectid = 0,
        page = 0
    ;

    new String:string = str_new("");
    new List:furnitures = Furniture_GetByEntityFilter(type, entity, isExterior);

    pawn_guard(string);
    yield 1;
    for(;;)
    {
        count = list_size(furnitures);
        if(count == 0)
        {
            SendClientMessage(playerid, COLOR_ERROR, "Non ci sono mobili piazzati.");
            break;
        }
        
        startIndex = page * MAX_FURNITURE_PER_PAGE;
        lastIndex = (startIndex + MAX_FURNITURE_PER_PAGE);

        if(lastIndex > count)
            lastIndex = count;

        str_clear(string);

        for(new i = startIndex; i < lastIndex; ++i)
        {
            objectid = list_get(furnitures, i);
            Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
            
            if(FurnitureCustomData[FURNITURE_IS_EXTERIOR] != isExterior)
                continue;
            
            new modelid = Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_MODEL_ID);
            str_append_format(string, "%s [Model ID: %d]\n", FurnitureCustomData[FURNITURE_NAME], modelid);
        }

        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_LIST, @("Arredamento"), string, @("Avanti"), @("Chiudi"));
    
        if(!response[E_DIALOG_RESPONSE_Response]) break;

        new listitem = response[E_DIALOG_RESPONSE_Listitem];

        // handle pages 

        if(listitem == lastIndex)
        {
            page--;
            continue;
        }
        if(listitem == lastIndex)
        {
            page++;
            continue;
        }        
        HandleFurnitureOption(playerid, startIndex + listitem, furnitures);
    }
}

static HandleFurnitureOption(playerid, index, List:furnitures)
{
    new 
        response[e_DIALOG_RESPONSE_INFO];
    new String:string = str_new("Sposta\nRinomina\nRimuovi\nResetta Textures\n");
    if(Account_GetAdminLevel(playerid) > 1)
        str_append_format(string, "Gotati");
    pawn_guard(string);

    new objectid = list_get(furnitures, index);

    yield 1;
    for(;;)
    {
        if(!IsValidDynamicObject(objectid))
            break;
        
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_LIST, @("Opzioni mobile"), string, @("Avanti"), @("Chiudi"));
    
        if(!response[E_DIALOG_RESPONSE_Response]) break;
        

        Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);

        switch(response[E_DIALOG_RESPONSE_Listitem])
        {
            case 0:
            {
                new Float:x, Float:y, Float:z;
                GetDynamicObjectPos(objectid, x, y, z);
                if(GetPlayerDistanceFromPoint(playerid, x, y, z) > 25.0)
                {
                    SendClientMessage(playerid, COLOR_ERROR, "Sei troppo distante dall'oggetto.");
                    continue;
                }
                SendClientMessage(playerid, COLOR_WHITE, "Premi {FF6347}ESC{FFFFFF} per annullare e {FF6347}SPAZIO{FFFFFF} per spostare la visuale. Clicca sul {FF6347}FLOPPY{FFFFFF} per salvare.");
                EditObject_Start(playerid, EDIT_TYPE_FURNITURE, objectid, FurnitureCustomData[FURNITURE_DATABASE_ID]);
                break;
            }
            case 1:
            {
                HandleRenameFurnitureDialog(playerid, objectid);
            }
            case 2:
            {
                Furniture_Remove(playerid, objectid);
                break;
            }
            case 3:
            {
                Furniture_RemoveTextures(objectid);
                SendClientMessage(playerid, COLOR_GREEN, "Hai rimosso le texture al mobile.");
                Furniture_ShowOptions(playerid);
            }
            case 4: // GOTO
            {
                if(!Player_IsAdminDuty(playerid))
                    return 0;
                HandleGotoFurniture(playerid, objectid);
                break;
            }
        }
    }
    return 1;
}

static HandleRenameFurnitureDialog(playerid, objectid)
{
    new response[e_DIALOG_RESPONSE_INFO];
    yield 1;
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_INPUT, @("Rinomina mobile"), @("Inserisci il nuovo nome per il mobile:"), @("Rinomina"), @("Indietro"));
    
        if(!response[E_DIALOG_RESPONSE_Response]) break;

        if(strlen(response[E_DIALOG_RESPONSE_InputText]) < 4 || strlen(response[E_DIALOG_RESPONSE_InputText]) > 63)
            continue;
    
        if(Furniture_Rename(objectid, response[E_DIALOG_RESPONSE_InputText]))
        {
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai rinominato l'oggetto. Nuovo nome: %s", response[E_DIALOG_RESPONSE_InputText]);
        }
        else
            SendClientMessage(playerid, COLOR_ERROR, "Non è stato possibile rinominare l'oggetto. ID invalido.");
        
        break;
    }
}

static HandleSellFurnitureDialog(playerid, objectid)
{
    Furniture_Remove(playerid, objectid);
}

static HandleGotoFurniture(playerid, objectid)
{
    new Float:x, Float:y, Float:z;
    GetDynamicObjectPos(objectid, x, y, z);
    SetPlayerPos(playerid, x, y, z);
    SetPlayerInterior(playerid, Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_INTERIOR_ID));
    Character_SetVirtualWorld(playerid, Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_WORLD_ID));
    SendFormattedMessage(playerid, COLOR_GREEN, "Ti sei gotato al mobile id %d.", objectid);
}

static ShowFurnituresCategoriesDialog(playerid, bool:isExterior, EntityType:belongsToType, belongsToId)
{
    new response[e_DIALOG_RESPONSE_INFO];
    yield 1;
    new String:info = @("Inserisci ID Manualmente\nMura e scale\nTavoli per cucine e salotti\nSedie da uffici e da cucina\nLetti singoli e matrimoniali\nDivani e poltrone\nDecorazioni\nElettronica\nVario\nCucina\nBagno\nBottiglie, bicchieri e tazze\nPorte\nVasi\nPoster e graffiti\nContenitori rifiuti\nVestiti\nBombole e barili\nOggetti per il mare\nQuadri\nScatole\nColonne\nCibo\nUtensili\nMeccanica\nSpeciali\nLavagne");
    pawn_guard(info);
    new listitem = 0;
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_LIST, @("Categoria"), info, @("Seleziona"), @("Indietro"));
        
        if(!response[E_DIALOG_RESPONSE_Response]) break;

        listitem = response[E_DIALOG_RESPONSE_Listitem];

        if(listitem == 0)
        {
            HandleManualFurnitureIDDialog(playerid, isExterior, belongsToType, belongsToId);
            break;
        }
        else
        {
            listitem = listitem-1; 
            
            ShowCategoryFurnitureDialog(playerid, listitem, isExterior, belongsToType, belongsToId);
            break;
        }
    }
}

static HandleManualFurnitureIDDialog(playerid, bool:isExterior, EntityType:belongsToType, belongsToId)
{
    new response[e_DIALOG_RESPONSE_INFO];
    yield 1;
    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_INPUT, @("Inserisci l'ID dell'arredamento"), @("Inserisci l'objectid che vuoi piazzare.\nRicorda di seguire le regole e di evitare abusi."), @("Piazza"), @("Annulla"));
        
        if(!response[E_DIALOG_RESPONSE_Response]) break;

        new objectid = strval(response[E_DIALOG_RESPONSE_InputText]);
        if(objectid <= 0)
            continue;

        Furniture_StartBuy(playerid, objectid, isExterior, belongsToType, belongsToId);
    }
}

static ShowCategoryFurnitureDialog(playerid, categoryid, bool:isExterior, EntityType:belongsToType, belongsToId)
{
    new response[e_DIALOG_RESPONSE_INFO];
    yield 1;
    new String:info = str_new("Mobile\n");
    pawn_guard(info);
    new name[64];

    for(new i = 0, j = sizeof(BuyableFurnitures); i < j; ++i)
    {
        if(BuyableFurniture_GetCategory(i) != categoryid)
            continue;
        BuyableFurniture_GetName(i, name);
        str_append_format(info, "%s\n", name);
    }

    new listitem = 0;

    for(;;)
    {
        await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_TABLIST_HEADERS, @("Arredamento"), info, @("Acquista"), @("Chiudi"));
        
        if(!response[E_DIALOG_RESPONSE_Response]) break;

        listitem = response[E_DIALOG_RESPONSE_Listitem];
        Furniture_StartBuy(playerid, BuyableFurniture_GetModel(listitem), isExterior, belongsToType, belongsToId);
        break;
    }
}

stock Furniture_CreateTemporary(modelid, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, worldid, interior, bool:isExterior, EntityType:belongsToType, entityid)
{
    new objectid = Server_CreateObject(modelid, x, y, z, rx, ry, rz, worldid, interior);
    FurnitureCustomData[FURNITURE_DATABASE_ID] = 0;
    FurnitureCustomData[FURNITURE_PRICE] = 0;
    FurnitureCustomData[FURNITURE_BELONGS_TO_TYPE] = belongsToType;
    FurnitureCustomData[FURNITURE_BELONGS_TO_ID] = entityid;
    FurnitureCustomData[FURNITURE_IS_EXTERIOR] = isExterior;
    Streamer_SetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
    return objectid;
}

// CreatedBy = -1 means that Server created it.
stock Furniture_Store(objectid, createdBy = -1)
{
    if(Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData) && FurnitureCustomData[FURNITURE_DATABASE_ID] == 0)
    {
        new
            Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz,
            account_id = -1,
            character_id = -1;
        
        if(createdBy >= 0 && Character_IsLogged(createdBy))
        {
            account_id = Account_GetID(createdBy);
            character_id = Character_GetID(createdBy);
        }

        GetDynamicObjectPos(objectid, x, y, z);
        GetDynamicObjectRot(objectid, rx, ry, rz);

        new String:query = str_format("INSERT INTO `furnitures` \
        (object_id, world, interior, model, price, pos_x, pos_y, pos_z, rot_x, rot_y, rot_z, name, \
        created_by_account, created_by_character, last_edit_by_account, last_edit_by_character, belongs_to_type, belongs_to_id, is_exterior) \
        VALUES('%d', '%d', '%d', '%d', '%d', '%f', '%f', '%f', '%f', '%f', '%f', '%e', '%d', '%d', '%d', '%d', '%d', '%d', '%d')",
        objectid,
        Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_WORLD_ID),
        Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_INTERIOR_ID),
        Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_MODEL_ID),
        0,
        x, y, z,
        rx, ry, rz,
        "Mobile",
        account_id,
        character_id,
        account_id,
        character_id,
        _:FurnitureCustomData[FURNITURE_BELONGS_TO_TYPE],
        FurnitureCustomData[FURNITURE_BELONGS_TO_ID],
        FurnitureCustomData[FURNITURE_IS_EXTERIOR] ? 1 : 0
        );
        await mysql_aquery_s(gMySQL, query, .parallel=true);

        FurnitureCustomData[FURNITURE_DATABASE_ID] = cache_insert_id();
        FurnitureCustomData[FURNITURE_PRICE] = 0;
        FurnitureCustomData[FURNITURE_LAST_EDIT_ACCOUNT] = FurnitureCustomData[FURNITURE_CREATED_BY_ACCOUNT] = account_id;
        FurnitureCustomData[FURNITURE_LAST_EDIT_CHARACTER] = FurnitureCustomData[FURNITURE_CREATED_BY_CHARACTER] = character_id;

        set(FurnitureCustomData[FURNITURE_NAME], "Mobile");
        Streamer_SetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData);

        Furniture_AddToWorld(objectid);
        return 1;
    }
    return 0;
}

stock Furniture_Load(EntityType:entityType, entityid)
{
    if(Furniture_AreLoaded(entityType, entityid))
    {
        printf("[Furniture] Furnitures already loaded for entity %d id %d.", _:entityType, entityid);
        return 0;
    }

    new data[E_FURNITURE_ENTITY_DATA];
    data[All] = list_new();
    data[Interior] = list_new();
    data[Exterior] = list_new();

    map_add_arr(FurnitureByEntityType[entityType], entityid, data);

    await mysql_aquery_s(gMySQL, str_format("SELECT * FROM furnitures WHERE belongs_to_type = '%d' AND belongs_to_id = '%d' AND object_id = '0';", _:entityType, entityid));

    new rows = cache_num_rows();
    new modelid, vw, interior,
        Float:x, Float:y, Float:z,
        Float:rx, Float:ry, Float:rz
        ;

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

        new type = 0;
        cache_get_value_name_int(i, "belongs_to_type", type);
        FurnitureCustomData[FURNITURE_BELONGS_TO_TYPE] = EntityType:type;
        cache_get_value_name_int(i, "belongs_to_id", FurnitureCustomData[FURNITURE_BELONGS_TO_ID]);

        cache_get_value_name_int(i, "is_exterior", type);
        FurnitureCustomData[FURNITURE_IS_EXTERIOR] = type ? true : false;
        new objectid = Server_CreateObject(modelid, x, y, z, rx, ry, rz, vw, interior);
        
        Furniture_LoadTextures(objectid);

        Streamer_SetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
        
        Furniture_SetObject(objectid, FurnitureCustomData[FURNITURE_DATABASE_ID]);

        Furniture_AddToWorld(objectid);
    }
    return 1;
}

stock Furniture_SetObject(objectid, dbid)
{
    mysql_pquery_f(gMySQL, "UPDATE furnitures SET object_id = '%d' WHERE id = '%d'", objectid, dbid);
    return 1;
}

stock Furniture_StartBuy(playerid, modelid, bool:isExterior, EntityType:belongsToType, entityId)
{
    new maxFurnitures = isExterior ? Character_GetExtFurnitureSlot(playerid) : Character_GetFurnitureSlot(playerid);
    
    if(Character_GetFurnitureCount(playerid) >= maxFurnitures)
        return SendFormattedMessage(playerid, COLOR_ERROR, "Hai raggiunto il limite di mobili per questo edificio (%d).", maxFurnitures);

    //new world = isExterior ? 0 : Entity_GetVirtualWorld(belongsToType, entityId);
    //new interior = isExterior ? 0 : Entity_GetInterior(belongsToType, entityId);

    new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
    GetXYInFrontOfPlayer(playerid, x, y, 5);
    
    new objectid = Furniture_CreateTemporary(modelid, x, y, z, 0.0, 0.0, 0.0, Character_GetVirtualWorld(playerid), GetPlayerInterior(playerid), isExterior, belongsToType, entityId);
    EditObject_Start(playerid, EDIT_TYPE_BUYING_FURNITURE, objectid, 0);

    SendClientMessage(playerid, -1, "Premi {FF6347}ESC{FFFFFF} per annullare e {FF6347}SPAZIO{FFFFFF} per spostare la visuale. Clicca sul {FF6347}FLOPPY{FFFFFF} per salvare.");
    return 1;
}

stock Furniture_FinishBuy(playerid, objectid)
{
    Furniture_Store(objectid, playerid);

    SendClientMessage(playerid, -1, "Hai piazzato questo mobile con successo.");

    EditObject_End(playerid);
    Character_UpdateLabels(playerid);
    //Furniture_UpdateInfoList(playerid);
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

static Furniture_ShowOptions(playerid)
{
    new String:s = @("Sposta\nRinomina\nRimuovi\nResetta Textures\n");
    if(Account_GetAdminLevel(playerid) > 1)
    {
        s += @("Gotati");
    }
    
    Dialog_Show_s(playerid, Dialog_FurnitureOptions, DIALOG_STYLE_LIST, @("Opzioni mobili"), s, "Seleziona", "Indietro");
    return 1;
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

    mysql_pquery_f(gMySQL, "UPDATE furnitures SET \
            pos_x = '%f', pos_y = '%f', pos_z = '%f', \
            rot_x = '%f', rot_y = '%f', rot_z = '%f', \
            last_edit_by_account = '%d', last_edit_by_character = '%d' \
            WHERE id = '%d'", 
            tx, ty, tz,
            trx, try, trz,
            Account_GetID(playerid),
            Character_GetID(playerid),
            FurnitureCustomData[FURNITURE_DATABASE_ID]);
    
    //Furniture_UpdateInfoList(playerid);
    return 1;
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
    new bool:isExterior, EntityType:type, entityid;
    if(Character_GetFurnitureWorld(playerid, isExterior, type, entityid, .onlyAllowed=true) && Furniture_AreLoaded(type, entityid))
    {
        new List:list = Furniture_GetByEntity(type, entityid);
        if(list_valid(list))
            return list_size(list);
    }
    return 0;
}

stock Furniture_Remove(playerid, objectid)
{
    if(Furniture_Delete(objectid))
    {
        Player_Info(playerid, "Hai ~r~rimosso~w~ il mobile.", true, 5000);
        Character_UpdateLabels(playerid);
    }
    return 1;
}

stock bool:Furniture_Delete(objectid)
{
    if(!IsValidDynamicObject(objectid))
        return false;

    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);

    if(FurnitureCustomData[FURNITURE_DATABASE_ID] > 0)
    {
        mysql_pquery_f(gMySQL, "DELETE FROM furnitures WHERE id = '%d';", FurnitureCustomData[FURNITURE_DATABASE_ID]);
    }

    Furniture_RemoveFromWorld(objectid);

    new data[e_FurnitureCustomData];
    FurnitureCustomData = data;
    Streamer_SetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);

    Furniture_RemoveTextures(objectid);

    DestroyDynamicObject(objectid);
    return true;
}

stock bool:Furniture_AddToWorld(objectid)
{
    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
    if(Furniture_AreLoaded(FurnitureCustomData[FURNITURE_BELONGS_TO_TYPE], FurnitureCustomData[FURNITURE_BELONGS_TO_ID]))
    {
        new List:list = Furniture_GetByEntityFilter(FurnitureCustomData[FURNITURE_BELONGS_TO_TYPE], FurnitureCustomData[FURNITURE_BELONGS_TO_ID], FurnitureCustomData[FURNITURE_IS_EXTERIOR]);
        list_add(list, objectid);
        
        list = Furniture_GetByEntity(FurnitureCustomData[FURNITURE_BELONGS_TO_TYPE], FurnitureCustomData[FURNITURE_BELONGS_TO_ID]);
        list_add(list, objectid);
        return true;
    }
    return false;
}

stock bool:Furniture_RemoveFromWorld(objectid)
{
    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
    if(Furniture_AreLoaded(FurnitureCustomData[FURNITURE_BELONGS_TO_TYPE], FurnitureCustomData[FURNITURE_BELONGS_TO_ID]))
    {
        new List:list = Furniture_GetByEntity(FurnitureCustomData[FURNITURE_BELONGS_TO_TYPE], FurnitureCustomData[FURNITURE_BELONGS_TO_ID]);
        new index = list_find(list, objectid);
        if(index != -1)
            list_remove(list, index);
        
        list = Furniture_GetByEntityFilter(FurnitureCustomData[FURNITURE_BELONGS_TO_TYPE], FurnitureCustomData[FURNITURE_BELONGS_TO_ID], FurnitureCustomData[FURNITURE_IS_EXTERIOR]);
        index = list_find(list, objectid);
        if(index != -1)
            list_remove(list, index);
        return true;
    }
    return false;
}

stock Furniture_ShowCategoryList(playerid)
{
    static const info[] = "Inserisci ID Manualmente\nMura e scale\nTavoli per cucine e salotti\nSedie da uffici e da cucina\nLetti singoli e matrimoniali\nDivani e poltrone\nDecorazioni\nElettronica\nVario\nCucina\nBagno\nBottiglie, bicchieri e tazze\nPorte\nVasi\nPoster e graffiti\nContenitori rifiuti\nVestiti\nBombole e barili\nOggetti per il mare\nQuadri\nScatole\nColonne\nCibo\nUtensili\nMeccanica\nSpeciali\nLavagne";
    Dialog_Show(playerid, Dialog_CategorySelect, DIALOG_STYLE_LIST, "Categoria", info, "Continua", "Indietro");
    return 1;
}

stock bool:IsFurniture(objectid)
{
    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
    return FurnitureCustomData[FURNITURE_DATABASE_ID] > 0;
}

stock Furniture_Rename(objectid, const name[])
{
    if(!IsFurniture(objectid))
        return 0;
    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
    format(FurnitureCustomData[FURNITURE_NAME], sizeof(FurnitureCustomData[FURNITURE_NAME]), name);
    Streamer_SetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);

    mysql_pquery_f(gMySQL, "UPDATE furnitures SET name = '%e' WHERE id = '%d'", name, FurnitureCustomData[FURNITURE_DATABASE_ID]);
    return 1;
}

stock bool:Character_ToggleFurnitureLabels(playerid, bool:show)
{
    new bool:isExterior, EntityType:type, entityid;
    if(show && !Character_GetFurnitureWorld(playerid, isExterior, type, entityid, .onlyAllowed=true, .worldAllowed=true))
        return false;
    if(!show && !Furniture_IsShowingLabels(playerid))
        return false;
    printf("IsExterior: %d - Type: %d - EntityID: %d", isExterior?1:0, _:type, entityid);
    if(show)
    {
        if(!list_valid(FurnitureLabels[playerid]))
            FurnitureLabels[playerid] = list_new();
        else
            list_clear(FurnitureLabels[playerid]);

        new List:furnitures = Furniture_GetByEntity(type, entityid);
        if(!list_valid(furnitures) || list_size(furnitures) <= 0)
            return false;

        printf("Count: %d - Exterior: %s", list_size(furnitures), isExterior ? ("Si") : ("No"));
        
        new String:text = str_new("");
        new characterName[MAX_PLAYER_NAME], accountName[MAX_PLAYER_NAME];
        new Float:x, Float:y, Float:z;

        pawn_guard(text);
        for_list(i : furnitures)
        {
            pawn_guard(i);
            new objectid = iter_get(i);
            Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);

            if(FurnitureCustomData[FURNITURE_IS_EXTERIOR] != isExterior)
                continue;

            str_clear(text);

            GetDynamicObjectPos(objectid, x, y, z);
            
            if(type == ENTITY_TYPE_HOUSE)
                str_append_format(text, "Casa - %d\n", entityid);
            else if(type == ENTITY_TYPE_BUILDING)
                str_append_format(text, "Edificio - %d\n", entityid);
            else
                str_append_format(text, "World\n");

            str_append_format(text, "%s\n", FurnitureCustomData[FURNITURE_NAME]);

            if(FurnitureCustomData[FURNITURE_LAST_EDIT_CHARACTER] > 0)
            {
                await mysql_aquery_s(gMySQL, str_format("SELECT c.name as characterName, a.name as accountName FROM `characters` c LEFT JOIN `accounts` a ON c.account_id = a.id WHERE c.id = '%d'", FurnitureCustomData[FURNITURE_LAST_EDIT_CHARACTER]));

                str_append_format(text, "{FF6347}Object ID:{FFFFFF} %d\nExterior: %s\n", objectid, FurnitureCustomData[FURNITURE_IS_EXTERIOR] ? ("Si") : ("No"));

                if(cache_num_rows() > 0)
                {
                    cache_get_value_name(0, "characterName", characterName);

                    str_append_format(text, "{FF6347}(( Ultima Modifica: %s )){FFFFFF}\n", characterName);

                    if(Account_GetAdminLevel(playerid) > 1 && Player_IsAdminDuty(playerid))
                    {
                        cache_get_value_name(0, "accountName", accountName);
                        str_append_format(text, "{FF6347}(( Account: %s )){FFFFFF}\n", accountName);
                    }
                }
            }
            else
            {
                str_append_format(text, "(( Ultima Modifica: Server ))\n");
            }

            new Text3D:labelid = CreateDynamic3DTextLabelStr(text, -1, x, y, z + 0.2, 20.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, Character_GetVirtualWorld(playerid), GetPlayerInterior(playerid), playerid, STREAMER_3D_TEXT_LABEL_SD, -1);  

            list_add(FurnitureLabels[playerid], labelid);
        }
    }
    else
    {
        for_list(i : FurnitureLabels[playerid])
        {
            DestroyDynamic3DTextLabel(Text3D:iter_get(i));       
        }
        list_delete(FurnitureLabels[playerid]);
        FurnitureLabels[playerid] = INVALID_LIST;
    }
    return true;
}

static Character_UpdateLabels(playerid)
{
    if(Furniture_IsShowingLabels(playerid))
    {
        Character_ToggleFurnitureLabels(playerid, false);
        Character_ToggleFurnitureLabels(playerid, true);
    }
}

stock bool:Furniture_IsShowingLabels(playerid)
{
    return list_valid(FurnitureLabels[playerid]);
}

stock Furniture_SetTexture(objectid, index, modelid, const txdName[], const textureName[], materialColor)
{
    if(!IsValidDynamicObject(objectid) || index < 0 || index > 15)
        return 0;
    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
    mysql_pquery_f(gMySQL, "INSERT INTO `furniture_textures` \
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
    mysql_pquery_f(gMySQL, "DELETE FROM furniture_textures WHERE furniture_id = '%d';", FurnitureCustomData[FURNITURE_DATABASE_ID]);
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

// worldAllowed is used for ENTITY_TYPE_WORLD if admin duty
stock bool:Character_GetFurnitureWorld(playerid, &bool:isExterior, &EntityType:type, &entityid, bool:onlyAllowed = true, bool:worldAllowed = false)
{
    type = ENTITY_TYPE_NONE;
    entityid = 0;
    isExterior = false;
    new Float:x, Float:y, Float:z,
        worldid = Character_GetVirtualWorld(playerid);
    onlyAllowed = onlyAllowed && !Player_IsAdminDuty(playerid);
    if(worldid < HOUSE_START_WORLD)
    {
        new Float:dist = 0.0,
            Float:nearest = MAX_EXTERIOR_DISTANCE;
        for_house(house)
        {
            if(onlyAllowed)
            {
                if(!Character_DoesOwnHouse(playerid, house) || House_GetEnterWorld(house) != Character_GetVirtualWorld(playerid))  
                    continue;
            }
            House_GetEnterPosition(house, x, y, z);
            
            dist = GetPlayerDistanceFromPoint(playerid, x, y, z);
            if(dist > MAX_EXTERIOR_DISTANCE)
                continue;
            
            if(dist < nearest)
            {
                nearest = dist;
                type = ENTITY_TYPE_HOUSE;
                entityid = House_GetID(house);
            }
        }
        for_building(building)
        {
            if(onlyAllowed)
            {
                if(!Character_DoesOwnBuilding(playerid, building) || Building_GetEnterWorld(building) != Character_GetVirtualWorld(playerid))  
                    continue;
            }
            Building_GetEnterPos(building, x, y, z);
            
            dist = GetPlayerDistanceFromPoint(playerid, x, y, z);
            if(dist > MAX_EXTERIOR_DISTANCE)
                continue;
            
            if(dist < nearest)
            {
                nearest = dist;
                type = ENTITY_TYPE_BUILDING;
                entityid = Building_GetID(building);
            }
        }

        if(Player_IsAdminDuty(playerid) && type == ENTITY_TYPE_NONE && worldAllowed)
        {
            type = ENTITY_TYPE_WORLD;
            entityid = 0;
        }
        isExterior = true;
    }
    else
    {
        if(HOUSE_START_WORLD <= worldid < BUILDING_START_WORLD)
        {
            new House:house = House_Find(worldid - HOUSE_START_WORLD);
            if(House_IsValid(house))
            {
                if(!onlyAllowed || (onlyAllowed && Character_DoesOwnHouse(playerid, house)))
                {
                    type = ENTITY_TYPE_HOUSE;
                    entityid = House_GetID(house);
                }
            }       
        }
        else if(worldid >= BUILDING_START_WORLD)
        {
            new Building:building = Building_Find(worldid - BUILDING_START_WORLD);
            if(Building_IsValid(building))
            {
                if(!onlyAllowed || (onlyAllowed && Character_DoesOwnBuilding(playerid, building)))
                {
                    type = ENTITY_TYPE_BUILDING;
                    entityid = Building_GetID(building);
                }
            }
        }
    }
    return type != ENTITY_TYPE_NONE;
}

stock Character_ShowFurnitures(playerid, page)
{
    #pragma unused playerid
    /*if(rows > page * MAX_FURNITURE_PER_PAGE + MAX_FURNITURE_PER_PAGE)
        str += String:str_format(">>> Pagina %d\n", page + 2);
    if(FurnitureLinkedTo[playerid] >= BUILDING_START_WORLD)
        Dialog_Show_s(playerid, Dialog_CharacterFurnitures, DIALOG_STYLE_LIST, str_format("Lista mobili piazzati edificio %d", FurnitureLinkedTo[playerid] - BUILDING_START_WORLD), str, "Modifica", "Indietro");
    else if(FurnitureLinkedTo[playerid] >= HOUSE_START_WORLD)
        Dialog_Show_s(playerid, Dialog_CharacterFurnitures, DIALOG_STYLE_LIST, str_format("Lista mobili piazzati casa %d", FurnitureLinkedTo[playerid] - HOUSE_START_WORLD), str, "Modifica", "Indietro");
    */
    return 1;
}

stock bool:Furniture_IsExterior(objectid)
{
    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, FurnitureCustomData, szCustomData);
    return FurnitureCustomData[FURNITURE_IS_EXTERIOR];
}

stock List:Furniture_GetByEntity(EntityType:entityType, entityid)
{
    if(!Furniture_AreLoaded(entityType, entityid))
        return List:0;
    return List:map_get(FurnitureByEntityType[entityType], entityid, All);
}

stock List:Furniture_GetByEntityFilter(EntityType:entityType, entityid, bool:exterior)
{
    if(!Furniture_AreLoaded(entityType, entityid))
        return List:0;
    return List:map_get(FurnitureByEntityType[entityType], entityid, exterior ? Exterior : Interior);
}

stock bool:Furniture_AreLoaded(EntityType:entityType, entityid)
{
    return map_has_key(FurnitureByEntityType[entityType], entityid);
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

