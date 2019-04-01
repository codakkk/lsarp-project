#include <YSI\y_hooks>

hook OnGameModeInit()
{
    ShowRoom_LoadAll();
    return 1;
}


stock ShowRoom_Create(Float:x, Float:y, Float:z)
{
    new free_id = Iter_Free(ShowRooms);
    if(free_id == -1)
    {
        printf("> Trying to create a new ShowRoom. Success: Nope.");
        return -1;
    }

    set(ShowRoomInfo[free_id][srName], "Concessionaria");
    ShowRoomInfo[free_id][srX] = x;
    ShowRoomInfo[free_id][srY] = y;
    ShowRoomInfo[free_id][srZ] = z;

    for(new i = 0; i < MAX_VEHICLES_IN_SHOWROOM; ++i)
    {
        ShowRoomInfo[free_id][srModels][i] = 0;
        ShowRoomInfo[free_id][srPrices][i] = 0;
    }
    
    Iter_Add(ShowRooms, free_id);
    
    ShowRoom_CreateElements(free_id);
    inline OnCreate()
    {
        ShowRoomInfo[free_id][srID] = cache_insert_id();
        ShowRoom_Save(free_id);

    }
    new query[128];
    mysql_format(gMySQL, query, sizeof(query), "INSERT INTO `showrooms` (X, Y, Z) VALUES('%f', '%f', '%f')", x, y, z);
    mysql_tquery_inline(gMySQL, query, using inline OnCreate);
    return free_id;
}

stock ShowRoom_SetName(showroom_id, name[])
{
    if(!ShowRoomInfo[showroom_id][srID])
        return 0;
    set(ShowRoomInfo[showroom_id][srName], name);
    ShowRoom_DestroyElements(showroom_id);
    ShowRoom_CreateElements(showroom_id);
    ShowRoom_Save(showroom_id);
    return 1;
}

stock ShowRoom_Delete(showroom_id)
{
    if(!ShowRoomInfo[showroom_id][srID])
        return 0;
    inline OnDelete()
    {
        ShowRoomInfo[showroom_id][srID] = 0;
        set(ShowRoomInfo[showroom_id][srName], "");
        ShowRoomInfo[showroom_id][srX] = 0.0;
        ShowRoomInfo[showroom_id][srY] = 0.0;
        ShowRoomInfo[showroom_id][srZ] = 0.0;
        ShowRoomInfo[showroom_id][srVehX] = 0.0;
        ShowRoomInfo[showroom_id][srVehY] = 0.0;
        ShowRoomInfo[showroom_id][srVehZ] = 0.0;
        ShowRoomInfo[showroom_id][srVehA] = 0.0;

        ShowRoom_DestroyElements(showroom_id); 
    }
    new query[128];
    mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `showrooms` WHERE ID = '%d'", ShowRoomInfo[showroom_id][srID]);
    mysql_tquery_inline(gMySQL, query, using inline OnDelete);
    return 1;
}

stock ShowRoom_SetPosition(showroom_id, Float:new_x, Float:new_y, Float:new_z)
{
    if(!ShowRoomInfo[showroom_id][srID])
        return 0;
    
    ShowRoom_DestroyElements(showroom_id);
    ShowRoomInfo[showroom_id][srX] = new_x;
    ShowRoomInfo[showroom_id][srY] = new_y;
    ShowRoomInfo[showroom_id][srZ] = new_z;
    ShowRoom_CreateElements(showroom_id);
    return 1;
}

stock ShowRoom_DeleteVehicle(showroom_id, id)
{
    if(!ShowRoomInfo[showroom_id][srID])
        return 0;
    ShowRoomInfo[showroom_id][srModels][id] = 0;
    ShowRoomInfo[showroom_id][srPrices][id] = 0;
    ShowRoom_Save(showroom_id);
    return 1;
}

stock ShowRoom_AddVehicle(showroom_id, modelid, price)
{
    if(!ShowRoomInfo[showroom_id][srID])
        return 0;
    new free_id = ShowRoom_FreeVehicleSlot(showroom_id);
    if(free_id == -1)
        return 0;
    ShowRoomInfo[showroom_id][srModels][free_id] = modelid;
    ShowRoomInfo[showroom_id][srPrices][free_id] = price;
    ShowRoom_Save(showroom_id);
    return 1;
}

stock ShowRoom_SetVehicleSpawnPos(showroom_id, Float:x, Float:y, Float:z, Float:a)
{
    if(!ShowRoomInfo[showroom_id][srID])
        return 0;
    ShowRoomInfo[showroom_id][srVehX] = x;
    ShowRoomInfo[showroom_id][srVehY] = y;
    ShowRoomInfo[showroom_id][srVehZ] = z;
    ShowRoomInfo[showroom_id][srVehA] = a;
    ShowRoom_Save(showroom_id);
    return 1;
}

stock ShowRoom_FreeVehicleSlot(showroom_id)
{
    if(!ShowRoomInfo[showroom_id][srID])
        return -1;
    for(new i = 0; i < MAX_VEHICLES_IN_SHOWROOM; ++i)
    {
        if(!ShowRoomInfo[showroom_id][srModels][i] && !ShowRoomInfo[showroom_id][srPrices][i])
            return i;
    }
    return -1;
}

stock ShowRoom_Save(showroom_id)
{
    if(!ShowRoomInfo[showroom_id][srID])
        return 0;
    
    new 
        tempModels[256],
        tempPrices[256];
    for(new i = 0; i < MAX_VEHICLES_IN_SHOWROOM; ++i)
    {
        format(tempModels, sizeof(tempModels), "%s%d%c", tempModels, ShowRoomInfo[showroom_id][srModels][i], i == MAX_VEHICLES_IN_SHOWROOM-1 ? ' ' : '|');
        format(tempPrices, sizeof(tempPrices), "%s%d%c", tempPrices, ShowRoomInfo[showroom_id][srPrices][i], i == MAX_VEHICLES_IN_SHOWROOM-1 ? ' ' : '|');
    }

    new query[1024];
    mysql_format(gMySQL, query, sizeof(query), "UPDATE `showrooms` SET Name = '%e', Models = '%e', Prices = '%e', \
    X = '%f', Y = '%f', Z = '%f', \
    VehicleX = '%f', VehicleY = '%f', VehicleZ = '%f' \
    WHERE ID = '%d'", 
    ShowRoomInfo[showroom_id][srName],
    tempModels, tempPrices,
    ShowRoomInfo[showroom_id][srX], ShowRoomInfo[showroom_id][srY], ShowRoomInfo[showroom_id][srZ],
    ShowRoomInfo[showroom_id][srVehX], ShowRoomInfo[showroom_id][srVehY], ShowRoomInfo[showroom_id][srVehZ],
    ShowRoomInfo[showroom_id][srID]);
    mysql_tquery(gMySQL, query);
    return 1;
}

stock ShowRoom_LoadAll()
{
    inline OnLoad()
    {
        printf("Loading dealerships...");
        new 
            count = 0,
            temp[255],
            splitTemplate[64];
        cache_get_row_count(count);
        
        if(count > MAX_SHOWROOMS)
            count = MAX_SHOWROOMS;
        format(splitTemplate, sizeof(splitTemplate), "p<|>a<i>[%d]", MAX_VEHICLES_IN_SHOWROOM);
        for(new i = 0; i < count; ++i)
        {
            cache_get_value_index_int(i, 0, ShowRoomInfo[i][srID]);
            cache_get_value_index(i, 1, ShowRoomInfo[i][srName]);
            if(isnull(ShowRoomInfo[i][srName]))
                set(ShowRoomInfo[i][srName], "Concessionaria");
            cache_get_value_index(i, 2, temp);
            sscanf(temp, splitTemplate, ShowRoomInfo[i][srModels]);
            
            cache_get_value_index(i, 3, temp);
            sscanf(temp, splitTemplate, ShowRoomInfo[i][srPrices]);
            
            cache_get_value_index_float(i, 4, ShowRoomInfo[i][srX]);
            cache_get_value_index_float(i, 5, ShowRoomInfo[i][srY]);
            cache_get_value_index_float(i, 6, ShowRoomInfo[i][srZ]);

            cache_get_value_index_float(i, 7, ShowRoomInfo[i][srVehX]);
            cache_get_value_index_float(i, 8, ShowRoomInfo[i][srVehY]);
            cache_get_value_index_float(i, 9, ShowRoomInfo[i][srVehZ]);

            ShowRoom_CreateElements(i);

            //printf("> %s [%d]", ShowRoomInfo[i][srName], i);

            Iter_Add(ShowRooms, i);
        }
        printf("> %d dealerships loaded", count);
    }
    mysql_tquery_inline(gMySQL, "SELECT * FROM `showrooms`", using inline OnLoad);
}

stock ShowRoom_CreateElements(showroom_id)
{
    ShowRoomInfo[showroom_id][srPickup] = Pickup_Create(1239, showroom_id, ShowRoomInfo[showroom_id][srX], ShowRoomInfo[showroom_id][srY], ShowRoomInfo[showroom_id][srZ], ELEMENT_TYPE_DEALERSHIP, 0, 0);
    new nName[25];
    format(nName, sizeof(nName), "%s", ShowRoomInfo[showroom_id][srName]);
    ShowRoomInfo[showroom_id][sr3DText] = CreateDynamic3DTextLabel(nName, COLOR_GREY, ShowRoomInfo[showroom_id][srX], ShowRoomInfo[showroom_id][srY], ShowRoomInfo[showroom_id][srZ] + 0.5, 10.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0);
}

stock ShowRoom_DestroyElements(showroom_id)
{
    Pickup_Destroy(ShowRoomInfo[showroom_id][srPickup]);
    ShowRoomInfo[showroom_id][srPickup] = -1;
    if(IsValidDynamic3DTextLabel(ShowRoomInfo[showroom_id][sr3DText]))
        DestroyDynamic3DTextLabel(ShowRoomInfo[showroom_id][sr3DText]);
    //ShowRoomInfo[showroom_id][sr3DText] = 0;
}