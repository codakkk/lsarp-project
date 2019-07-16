#include <YSI_Coding\y_hooks>

hook OnGameModeInit()
{
    Dealership_LoadAll();
    return 1;
}

stock Dealership_GetModelPrice(modelid)
{
	foreach(new i : Dealerships)
	{
		for(new x = 0; x < MAX_VEHICLES_IN_SHOWROOM; ++x)
		{
			if(DealershipInfo[i][dsModels][x] == modelid)
				return DealershipInfo[i][dsPrices][x];
		}
	}
	return 0;
}

stock Dealership_Create(Float:x, Float:y, Float:z, world)
{
    new free_id = Iter_Free(Dealerships);
    if(free_id == -1)
    {
	   printf("Trying to create a new ShowRoom. Success: Nope.");
	   return -1;
    }

    set(DealershipInfo[free_id][dsName], "Concessionaria");
    DealershipInfo[free_id][dsX] = x;
    DealershipInfo[free_id][dsY] = y;
    DealershipInfo[free_id][dsZ] = z;
	DealershipInfo[free_id][dsVirtualWorld] = world;

    for(new i = 0; i < MAX_VEHICLES_IN_SHOWROOM; ++i)
    {
	   DealershipInfo[free_id][dsModels][i] = 0;
	   DealershipInfo[free_id][dsPrices][i] = 0;
    }
    
    Iter_Add(Dealerships, free_id);
    
    Dealership_CreateElements(free_id);
    inline OnCreate()
    {
	   DealershipInfo[free_id][dsID] = cache_insert_id();
	   Dealership_Save(free_id);

    }
    MySQL_TQueryInline(gMySQL, using inline OnCreate, "INSERT INTO `dealerships` (X, Y, Z) VALUES('%f', '%f', '%f')", x, y, z);
    return free_id;
}

stock Dealership_SetName(dealership_id, name[])
{
    if(!DealershipInfo[dealership_id][dsID])
	   return 0;
    set(DealershipInfo[dealership_id][dsName], name);
    Dealership_DestroyElements(dealership_id);
    Dealership_CreateElements(dealership_id);
    Dealership_Save(dealership_id);
    return 1;
}

stock Dealership_Delete(dealership_id)
{
    if(!DealershipInfo[dealership_id][dsID])
	   return 0;
    inline OnDelete()
    {
	   DealershipInfo[dealership_id][dsID] = 0;
	   set(DealershipInfo[dealership_id][dsName], "");
	   DealershipInfo[dealership_id][dsX] = 0.0;
	   DealershipInfo[dealership_id][dsY] = 0.0;
	   DealershipInfo[dealership_id][dsZ] = 0.0;
	   DealershipInfo[dealership_id][dsVirtualWorld] = 0;
	   DealershipInfo[dealership_id][dsVehX] = 0.0;
	   DealershipInfo[dealership_id][dsVehY] = 0.0;
	   DealershipInfo[dealership_id][dsVehZ] = 0.0;
	   DealershipInfo[dealership_id][dsVehA] = 0.0;

	   Dealership_DestroyElements(dealership_id); 
    }
    MySQL_TQueryInline(gMySQL, using inline OnDelete, "DELETE FROM `dealerships` WHERE ID = '%d'", DealershipInfo[dealership_id][dsID]);
    return 1;
}

stock Dealership_SetPosition(dealership_id, Float:new_x, Float:new_y, Float:new_z)
{
    if(!DealershipInfo[dealership_id][dsID])
	   return 0;
    
    Dealership_DestroyElements(dealership_id);
    DealershipInfo[dealership_id][dsX] = new_x;
    DealershipInfo[dealership_id][dsY] = new_y;
    DealershipInfo[dealership_id][dsZ] = new_z;
    Dealership_CreateElements(dealership_id);
    return 1;
}

stock Dealership_DeleteVehicle(dealership_id, id)
{
    if(!DealershipInfo[dealership_id][dsID])
	   return 0;
    DealershipInfo[dealership_id][dsModels][id] = 0;
    DealershipInfo[dealership_id][dsPrices][id] = 0;
    Dealership_Save(dealership_id);
    return 1;
}

stock Dealership_AddVehicle(dealership_id, modelid, price)
{
    if(!DealershipInfo[dealership_id][dsID])
	   return 0;
    new free_id = Dealership_FreeVehicleSlot(dealership_id);
    if(free_id == -1)
	   return 0;
    DealershipInfo[dealership_id][dsModels][free_id] = modelid;
    DealershipInfo[dealership_id][dsPrices][free_id] = price;
    Dealership_Save(dealership_id);
    return 1;
}

stock Dealership_SetVehicleSpawnPos(dealership_id, Float:x, Float:y, Float:z, Float:a)
{
    if(!DealershipInfo[dealership_id][dsID])
	   return 0;
    DealershipInfo[dealership_id][dsVehX] = x;
    DealershipInfo[dealership_id][dsVehY] = y;
    DealershipInfo[dealership_id][dsVehZ] = z;
    DealershipInfo[dealership_id][dsVehA] = a;
    Dealership_Save(dealership_id);
    return 1;
}

stock Dealership_FreeVehicleSlot(dealership_id)
{
    if(!DealershipInfo[dealership_id][dsID])
	   return -1;
    for(new i = 0; i < MAX_VEHICLES_IN_SHOWROOM; ++i)
    {
	   if(!DealershipInfo[dealership_id][dsModels][i] && !DealershipInfo[dealership_id][dsPrices][i])
		  return i;
    }
    return -1;
}

stock Dealership_Save(dealership_id)
{
    if(!DealershipInfo[dealership_id][dsID])
	   return 0;
    
    new 
	   tempModels[256],
	   tempPrices[256];
    for(new i = 0; i < MAX_VEHICLES_IN_SHOWROOM; ++i)
    {
	   format(tempModels, sizeof(tempModels), "%s%d%c", tempModels, DealershipInfo[dealership_id][dsModels][i], i == MAX_VEHICLES_IN_SHOWROOM-1 ? ' ' : '|');
	   format(tempPrices, sizeof(tempPrices), "%s%d%c", tempPrices, DealershipInfo[dealership_id][dsPrices][i], i == MAX_VEHICLES_IN_SHOWROOM-1 ? ' ' : '|');
    }

    new query[1024];
    mysql_format(gMySQL, query, sizeof(query), "UPDATE `dealerships` SET Name = '%e', Models = '%e', Prices = '%e', \
    X = '%f', Y = '%f', Z = '%f', VirtualWorld = '%d', \
    VehicleX = '%f', VehicleY = '%f', VehicleZ = '%f' \
    WHERE ID = '%d'", 
    DealershipInfo[dealership_id][dsName],
    tempModels, tempPrices,
    DealershipInfo[dealership_id][dsX], DealershipInfo[dealership_id][dsY], DealershipInfo[dealership_id][dsZ], DealershipInfo[dealership_id][dsVirtualWorld],
    DealershipInfo[dealership_id][dsVehX], DealershipInfo[dealership_id][dsVehY], DealershipInfo[dealership_id][dsVehZ],
    DealershipInfo[dealership_id][dsID]);
    mysql_tquery(gMySQL, query);
    return 1;
}

stock Dealership_LoadAll()
{
    inline OnLoad()
    {
		new 
			count = 0,
			temp[255],
			splitTemplate[64];
	   	cache_get_row_count(count);
	   
		if(count > MAX_DEALERSHIPS)
			count = MAX_DEALERSHIPS;
		format(splitTemplate, sizeof(splitTemplate), "p<|>a<i>[%d]", MAX_VEHICLES_IN_SHOWROOM);
		for(new i = 0; i < count; ++i)
		{
			cache_get_value_index_int(i, 0, DealershipInfo[i][dsID]);
			cache_get_value_index(i, 1, DealershipInfo[i][dsName]);
			if(isnull(DealershipInfo[i][dsName]))
				set(DealershipInfo[i][dsName], "Concessionaria");
			cache_get_value_index(i, 2, temp);
			sscanf(temp, splitTemplate, DealershipInfo[i][dsModels]);
			
			cache_get_value_index(i, 3, temp);
			sscanf(temp, splitTemplate, DealershipInfo[i][dsPrices]);
			
			cache_get_value_index_float(i, 4, DealershipInfo[i][dsX]);
			cache_get_value_index_float(i, 5, DealershipInfo[i][dsY]);
			cache_get_value_index_float(i, 6, DealershipInfo[i][dsZ]);

			cache_get_value_index_float(i, 7, DealershipInfo[i][dsVehX]);
			cache_get_value_index_float(i, 8, DealershipInfo[i][dsVehY]);
			cache_get_value_index_float(i, 9, DealershipInfo[i][dsVehZ]);

			cache_get_value_index_int(i, 10, DealershipInfo[i][dsVirtualWorld]);
			
			Dealership_CreateElements(i);

		  //printf(%s [%d]", DealershipInfo[i][dsName], i);

			Iter_Add(Dealerships, i);
		}
		printf("%d dealerships loaded", count);
	}
	MySQL_PQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `dealerships`");
}

stock Dealership_CreateElements(dealership_id)
{
    DealershipInfo[dealership_id][dsPickup] = Pickup_Create(1239, dealership_id, DealershipInfo[dealership_id][dsX], DealershipInfo[dealership_id][dsY], DealershipInfo[dealership_id][dsZ], ELEMENT_TYPE_DEALERSHIP, DealershipInfo[dealership_id][dsVirtualWorld], 0);
    new nName[25];
    format(nName, sizeof(nName), "%s", DealershipInfo[dealership_id][dsName]);
    DealershipInfo[dealership_id][ds3DText] = CreateDynamic3DTextLabel(nName, COLOR_GREY, DealershipInfo[dealership_id][dsX], DealershipInfo[dealership_id][dsY], DealershipInfo[dealership_id][dsZ] + 0.5, 10.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, DealershipInfo[dealership_id][dsVirtualWorld]);
}

stock Dealership_DestroyElements(dealership_id)
{
    Pickup_Destroy(DealershipInfo[dealership_id][dsPickup]);
    DealershipInfo[dealership_id][dsPickup] = -1;
    if(IsValidDynamic3DTextLabel(DealershipInfo[dealership_id][ds3DText]))
	   DestroyDynamic3DTextLabelEx(DealershipInfo[dealership_id][ds3DText]);
    //DealershipInfo[dealership_id][ds3DText] = 0;
}

stock Dealership_ShowInternalVehicles(dealership_id, playerid)
{
    if(AccountInfo[playerid][aAdmin] < 2) // Safeness
	   return 0;
    new
	   string[1024],
	   count = 0;
    
    for(new i = 0; i < MAX_VEHICLES_IN_SHOWROOM; ++i)
    {
	   if(!DealershipInfo[dealership_id][dsModels][i] || !DealershipInfo[dealership_id][dsPrices][i])
		  continue;
	   new 
		  modelid = DealershipInfo[dealership_id][dsModels][i],
		  price = DealershipInfo[dealership_id][dsPrices][i]
	   ;
	   format(string, sizeof(string), "%s%s\t$%d\n", string, Vehicle_GetNameFromModel(modelid), price);
	   gDealershipItemList[playerid][count] = i;
	   count++;
    }
    if(count == 0)
	   return Dialog_Show(playerid, Dialog_EditShowRoom, DIALOG_STYLE_MSGBOX, "Errore", "Questa concessionaria non ha veicoli.", "Ok", "");
    SetPVarInt(playerid, "ShowRoomEdit_Phase", 0);
    return Dialog_Show(playerid, Dialog_ShowRoomEditVeh, DIALOG_STYLE_TABLIST_HEADERS, "ShowRoom Edit", "Modello\tPrezzo\n%s", "Modifica", "Indietro", string);
}

stock Dealership_ShowList(playerid, params[]="")
{
    if(AccountInfo[playerid][aAdmin] < 2) // Safeness
	   return 0;
    if(Iter_Count(Dealerships) == 0)
	   return SendClientMessage(playerid, COLOR_ERROR, "Non esistono concessionarie. Usa /acreateshowroom per crearne una.");
    new id = -1;
    if(sscanf(params, "d", id))
    {
		new 
			temp[256], 
			locName[32],
			count = 0
			;
		
		foreach(new i : Dealerships)
		{
			Get2DZoneName(DealershipInfo[i][dsX], DealershipInfo[i][dsY], DealershipInfo[i][dsZ], locName, sizeof(locName));
			format(temp, sizeof(temp), "%s%d\t%s\t%s\n", temp, i, DealershipInfo[i][dsName], locName);
			gDealershipListItemList[playerid][count] = i;
			count++;
		}
		return Dialog_Show(playerid, Dialog_AShowRoomList, DIALOG_STYLE_TABLIST_HEADERS, "ShowRoom Edit", "ID\tName\tLocation\n%s", "Edit", "Close", temp);
    }
    else
    {
	   if(!DealershipInfo[id][dsID])
		  return SendClientMessage(playerid, COLOR_ERROR, "Non esiste una concessionaria con questo ID.");
	   gAdminSelectedDealership[playerid] = id;
	   new string[64];
	   format(string, sizeof(string), "%s [%d]", DealershipInfo[id][dsName], id);
	   return Dialog_Show(playerid, Dialog_EditShowRoom, DIALOG_STYLE_LIST, string, "Lista Veicoli\nModifica Nome\nAggiungi Veicolo\nSposta (Posizione Corrente)\nPosizione Veicolo\n{FF0000}Cancella (Definitivamente)", "Avanti", "Chiudi");
    }
}
