#include <dealership\player>
#include <YSI_Coding\y_hooks>

static enum e_DEALERSHIP_VEHICLE_DATA
{
	VehicleModel,
	VehiclePrice
};

static enum e_DEALERSHIP_DATA
{
    ID,
	bool:OnlyPremium,
	Name[32],
    Float:X,
    Float:Y,
    Float:Z,
	VirtualWorld,
    //Where will we position the vehicle when choosing its color, etc?
    Float:VehX,
    Float:VehY,
    Float:VehZ,
    Float:VehA,

	List:Vehicles,

    Pickup,
    Text3D:Text3D,
}; 


static 
	Map:Dealerships;

// Handling
static 
	SelectedDealership[MAX_PLAYERS],
	SelectedListItem[MAX_PLAYERS char]
;

hook OnGameModeInit()
{
	Dealerships = map_new();
	mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS dealerships \
						(`id` INT(11) NOT NULL AUTO_INCREMENT, \
						`name` VARCHAR(32) DEFAULT 'Concessionaria', \
						`x` FLOAT DEFAULT '0.0', \
						`y` FLOAT DEFAULT '0.0', \
						`z` FLOAT DEFAULT '0.0', \
						`vehicle_x` FLOAT DEFAULT '0.0', \
						`vehicle_y` FLOAT DEFAULT '0.0', \
						`vehicle_z` FLOAT DEFAULT '0.0', \
						`vehicle_a` FLOAT DEFAULT '0.0', \
						`virtual_world` INT(11) DEFAULT '0', \
						`only_premium` INT(1) DEFAULT '0', \
						PRIMARY KEY (`id`) \
						) ENGINE=InnoDB DEFAULT CHARSET=latin1;");

	mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS dealership_vehicles \
						(`dealership_id` INT(11), \
						`modelid` INT(11) DEFAULT '0', \
						`price` INT(11) DEFAULT '0', \
						PRIMARY KEY (`dealership_id`, `modelid`), \
						CONSTRAINT `dealership_vehicles` FOREIGN KEY (`dealership_id`) REFERENCES `dealerships` (`id`) ON DELETE CASCADE \
						) ENGINE=InnoDB DEFAULT CHARSET=latin1;");

    Dealership_LoadAll();
    return 1;
}

stock Dealership_Create(Float:x, Float:y, Float:z, world)
{
    inline OnCreate()
    {
		new data[e_DEALERSHIP_DATA];

	   	data[ID] = cache_insert_id();
    	
		set(data[Name], "Concessionaria");
    	data[OnlyPremium] = false;
		data[X] = x;
    	data[Y] = y;
    	data[Z] = z;
		data[VirtualWorld] = world;
		data[Vehicles] = list_new();

		map_add_arr(Dealerships, data[ID], data);

    	Dealership_CreateElements(data[ID]);
    }
    MySQL_TQueryInline(gMySQL, using inline OnCreate, "INSERT INTO `dealerships` (name, x, y, z) VALUES('%e', '%f', '%f', '%f')", "Concessionaria", x, y, z);
    return 1;
}

stock Dealership_SetName(dealershipid, name[])
{
    if(!Dealership_IsValid(dealershipid))
	   return 0;
    new Iter:iter = Iter:map_iter_at(Dealerships, dealershipid);
	iter_set_cells(iter, Name, name, 32);

	mysql_tquery_f(gMySQL, "UPDATE `dealerships` SET name = '%e' WHERE id = '%d'", name, Dealership_GetID(dealershipid));

    Dealership_DestroyElements(dealershipid);
    Dealership_CreateElements(dealershipid);
    return 1;
}

stock Dealership_IsValid(dealershipid)
{
	return map_has_key(Dealerships, dealershipid);
}

stock Dealership_GetID(dealershipid)
{
	if(!Dealership_IsValid(dealershipid))
		return 0;
	return map_get(Dealerships, dealershipid, ID);
}

stock Dealership_GetName(dealershipid, text[32])
{
	if(!Dealership_IsValid(dealershipid))
	   return 0;
	new Iter:iter = map_iter_at(Dealerships, dealershipid);
    iter_get_md_arr(iter, {Name}, text);
	return 1;
}

stock bool:Dealership_IsOnlyPremium(dealershipid)
{
	if(!Dealership_IsValid(dealershipid))
		return false;
	return bool:map_get(Dealerships, Dealership_GetID(dealershipid), OnlyPremium);
}

stock Dealership_SetOnlyPremium(dealershipid, bool:value)
{
	if(!Dealership_IsValid(dealershipid))
		return 0;
	map_set_cell(Dealerships, Dealership_GetID(dealershipid), OnlyPremium, value);

	mysql_tquery_f(gMySQL, "UPDATE `dealerships` SET only_premium = '%d' WHERE id = '%d'", value?1:0, Dealership_GetID(dealershipid));

	return true;
}

stock Dealership_Delete(dealershipid)
{
    if(!Dealership_IsValid(dealershipid))
	   return 0;
	
    inline OnDelete()
    {
	   Dealership_DestroyElements(dealershipid); 
	   
	   list_delete_deep(Dealership_GetVehiclesList(dealershipid));
	   
	   map_remove_deep(Dealerships, Dealership_GetID(dealershipid)); 
    }
    MySQL_TQueryInline(gMySQL, using inline OnDelete, "DELETE FROM `dealerships` WHERE id = '%d'", Dealership_GetID(dealershipid));
    return 1;
}

stock List:Dealership_GetVehiclesList(dealershipid)
{
	if(!Dealership_IsValid(dealershipid))
		return List:0;
	return List:map_get(Dealerships, Dealership_GetID(dealershipid), Vehicles);
}

stock Dealership_SetPosition(dealership_id, Float:new_x, Float:new_y, Float:new_z)
{
    if(!Dealership_IsValid(dealership_id))
	   return 0;
    
    Dealership_DestroyElements(dealership_id);

	map_set_cell(Dealerships, Dealership_GetID(dealership_id), X, new_x);
	map_set_cell(Dealerships, Dealership_GetID(dealership_id), Y, new_y);
	map_set_cell(Dealerships, Dealership_GetID(dealership_id), Z, new_z);
    
	mysql_tquery_f(gMySQL, "UPDATE `dealerships` SET x = '%f', y = '%f', z = '%f' WHERE id = '%d'", new_x, new_y, new_z, Dealership_GetID(dealership_id));

    Dealership_CreateElements(dealership_id);
    return 1;
}

stock Dealership_GetPosition(dealership_id, &Float:x, &Float:y, &Float:z)
{
	if(!Dealership_IsValid(dealership_id))
	   return 0;
	x = Float:map_get(Dealerships, Dealership_GetID(dealership_id), X);
	y = Float:map_get(Dealerships, Dealership_GetID(dealership_id), Y);
	z = Float:map_get(Dealerships, Dealership_GetID(dealership_id), Z);
	return 1;
}

stock bool:Dealership_HasModel(dealershipid, modelid)
{
	if(!Dealership_IsValid(dealershipid))
	   return false;
	new bool:has = false;
	for_list(i : Dealership_GetVehiclesList(dealershipid))
	{
		new temp = iter_get(i, VehicleModel);
		if(temp == modelid)
		{
			has = true;
			break;
		}
	}

	return has;
}

stock Dealership_GetModelAt(dealershipid, slotid)
{
	if(!Dealership_IsValid(dealershipid))
	   return 0;
	
	new List:vehicles = Dealership_GetVehiclesList(dealershipid);
	
	if(slotid < 0 || slotid >= list_size(vehicles))
		return 0;
	
	return list_get(vehicles, slotid, VehicleModel);
}

stock Dealership_GetPriceAt(dealershipid, slotid)
{
	if(!Dealership_IsValid(dealershipid))
	   return 0;
	
	new List:vehicles = Dealership_GetVehiclesList(dealershipid);
	
	if(slotid < 0 || slotid >= list_size(vehicles))
		return 0;
	
	return list_get(vehicles, slotid, VehiclePrice);
}

static Dealership_SetPriceAt(dealershipid, slotid, newPrice)
{
	if(!Dealership_IsValid(dealershipid) || newPrice < 0)
	   return 0;
	
	new List:vehicles = Dealership_GetVehiclesList(dealershipid);
	
	if(slotid < 0 || slotid >= list_size(vehicles))
		return 0;
	list_set_cell(vehicles, slotid, VehiclePrice, newPrice);
	mysql_tquery_f(gMySQL, "UPDATE `dealership_vehicles` SET price = '%d' WHERE id = '%d' AND modelid = '%d'", newPrice, Dealership_GetModelAt(dealershipid, slotid));
	return 1;
}

stock Dealership_DeleteVehicle(dealershipid, slotid)
{
    if(!Dealership_IsValid(dealershipid))
	   return 0;

	new modelid = Dealership_GetModelAt(dealershipid, slotid);
	
	if(modelid == 0 || !list_remove(Dealership_GetVehiclesList(dealershipid), slotid))
		return 0;

	mysql_tquery_f(gMySQL, "DELETE FROM `dealership_vehicles` WHERE dealership_id = '%d' AND modelid = '%d'", Dealership_GetID(dealershipid), modelid);
    return 1;
}

stock Dealership_AddVehicle(dealershipid, modelid, price)
{
    if(!Dealership_IsValid(dealershipid) || Dealership_HasModel(dealershipid, modelid))
	   return 0;
	new data[e_DEALERSHIP_VEHICLE_DATA];
	
	data[VehicleModel] = modelid;
    data[VehiclePrice] = price;

	list_add_arr(Dealership_GetVehiclesList(dealershipid), data);

	mysql_tquery_f(gMySQL, "INSERT INTO `dealership_vehicles` (dealership_id, modelid, price) VALUES('%d', '%d', '%d')", Dealership_GetID(dealershipid), modelid, price);
    return 1;
}

stock Dealership_SetVehicleSpawnPos(dealership_id, Float:x, Float:y, Float:z, Float:a)
{
    if(!Dealership_IsValid(dealership_id))
	   return 0;
	map_set_cell(Dealerships, Dealership_GetID(dealership_id), VehX, x);
	map_set_cell(Dealerships, Dealership_GetID(dealership_id), VehY, y);
	map_set_cell(Dealerships, Dealership_GetID(dealership_id), VehZ, z);
	map_set_cell(Dealerships, Dealership_GetID(dealership_id), VehA, a);
    
	mysql_tquery_f(gMySQL, "UPDATE `dealerships` SET vehicle_x = '%f', vehicle_y = '%f', vehicle_z = '%f', vehicle_a = '%f' WHERE id = '%d'", x, y, z, a, Dealership_GetID(dealership_id));
    return 1;
}

stock Dealership_GetVehicleSpawnPos(dealershipid, &Float:x, &Float:y, &Float:z, &Float:a)
{
    if(!Dealership_IsValid(dealershipid))
	   return 0;
	x = Float:map_get(Dealerships, Dealership_GetID(dealershipid), VehX);
	y = Float:map_get(Dealerships, Dealership_GetID(dealershipid), VehY);
	z = Float:map_get(Dealerships, Dealership_GetID(dealershipid), VehZ);
	a = Float:map_get(Dealerships, Dealership_GetID(dealershipid), VehA);
    return 1;
}

stock Dealership_LoadAll()
{
    inline OnLoad()
    {
		new 
			count = cache_num_rows(),
			data[e_DEALERSHIP_DATA]
		;
		for(new i = 0; i < count; ++i)
		{
			cache_get_value_name_int(i, "id", data[ID]);
			cache_get_value_name(i, "name", data[Name]);
			cache_get_value_name_float(i, "x", data[X]);
			cache_get_value_name_float(i, "y", data[Y]);
			cache_get_value_name_float(i, "z", data[Z]);
			cache_get_value_name_float(i, "vehicle_x", data[VehX]);
			cache_get_value_name_float(i, "vehicle_y", data[VehY]);
			cache_get_value_name_float(i, "vehicle_z", data[VehZ]);
			cache_get_value_name_float(i, "vehicle_a", data[VehA]);
			cache_get_value_name_int(i, "virtual_world", data[VirtualWorld]);
			
			data[Vehicles] = list_new();
			map_add_arr(Dealerships, data[ID], data);
			
			Dealership_CreateElements(data[ID]);

			Dealership_LoadVehicles(data[ID]);
		}
		printf("%d dealerships loaded.", map_size(Dealerships));
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `dealerships`");
}

static Dealership_LoadVehicles(dealershipid)
{
	if(!Dealership_IsValid(dealershipid))
		return 0;
	inline OnVehiclesLoad()
	{
		new List:vehicles = Dealership_GetVehiclesList(dealershipid);
		new rows = cache_num_rows();
		new vehicleData[e_DEALERSHIP_VEHICLE_DATA];

		for(new i = 0; i < rows; ++i)
		{
			cache_get_value_name_int(i, "modelid", vehicleData[VehicleModel]);
			cache_get_value_name_int(i, "price", vehicleData[VehiclePrice]);

			if(vehicleData[VehicleModel] == 0 || vehicleData[VehiclePrice] < 0)
				continue;
			
			list_add_arr(vehicles, vehicleData);
		}
	}
	MySQL_TQueryInline(gMySQL, using inline OnVehiclesLoad, "SELECT * FROM `dealership_vehicles` WHERE dealership_id = '%d'", Dealership_GetID(dealershipid));
	return 1;
}

stock Dealership_CreateElements(dealershipid)
{
	if(!Dealership_IsValid(dealershipid))
		return 0;
	new Float:x, Float:y, Float:z;
	new nName[32];
	
	Dealership_GetPosition(dealershipid, x, y, z);
	Dealership_GetName(dealershipid, nName);

	new pickupid = Pickup_Create(1239, Dealership_GetID(dealershipid), x, y, z, ELEMENT_TYPE_DEALERSHIP, Dealership_GetVirtualWorld(dealershipid), 0);
    new Text3D:text = CreateDynamic3DTextLabel(nName, COLOR_GREY, x, y, z + 0.5, 10.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, Dealership_GetVirtualWorld(dealershipid));
    printf("Create elements for dealership id %d", dealershipid);
	map_set_cell(Dealerships, Dealership_GetID(dealershipid), Pickup, pickupid);
	map_set_cell(Dealerships, Dealership_GetID(dealershipid), Text3D, text);
	return 1;
}

stock Dealership_GetVirtualWorld(dealershipid)
{
	if(!Dealership_IsValid(dealershipid))
		return 0;
    return map_get(Dealerships, Dealership_GetID(dealershipid), VirtualWorld);
}

stock Dealership_DestroyElements(dealershipid)
{
	if(!Dealership_IsValid(dealershipid))
		return 0;
	
    Pickup_Destroy(map_get(Dealerships, Dealership_GetID(dealershipid), Pickup));
	new Text3D:text = Text3D:map_get(Dealerships, Dealership_GetID(dealershipid), Text3D);
    DestroyDynamic3DTextLabelEx(text);
    return 1;
}

stock Dealership_ShowInternalVehicles(dealershipid, playerid)
{
    if(Account_GetAdminLevel(playerid) < 2) // Safeness
	   return 0;
    new
	   String:string = @("Modello\tPrezzo ($)\n"),
	   List:vehicles = Dealership_GetVehiclesList(dealershipid);
    
	if(list_size(vehicles) <= 0)
		return Dialog_Show(playerid, DialogNull, DIALOG_STYLE_MSGBOX, "Errore", "Attualmente questa concessionaria non ha veicoli in vendita.", "Ok", "");

    for_list(iter : vehicles)
    {
		new 
			modelid = iter_get(iter, VehicleModel),
			price = iter_get(iter, VehiclePrice)
		;

		if(modelid <= 0 || price < 0)
			continue;
		
		string += str_format("%s\t$%d\n", Vehicle_GetNameFromModel(modelid), price);
	}
	return Dialog_Show_s(playerid, Dialog_DealershipEditVehList, DIALOG_STYLE_TABLIST_HEADERS, @("Concessionaria"), string, "Modifica", "Indietro");
}

Dialog:Dialog_DealershipEditVehList(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Dealership_ShowOptions(SelectedDealership[playerid], playerid);
	new List:vehicles = Dealership_GetVehiclesList(SelectedDealership[playerid]);

	if(listitem >= list_size(vehicles))
		return Dealership_ShowInternalVehicles(SelectedDealership[playerid], playerid);
	
	new modelid = Dealership_GetModelAt(SelectedDealership[playerid], listitem);

	SelectedListItem{playerid} = listitem;

	Dialog_Show_s(playerid, Dialog_DealershipEditVehicle, DIALOG_STYLE_LIST, str_format("%s [%d]", Vehicle_GetNameFromModel(modelid), modelid), @("Rimuovi Veicolo\nModifica Prezzo"), "Continua", "Annulla");
	return 1;
}

Dialog:Dialog_DealershipEditVehicle(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Dealership_ShowInternalVehicles(SelectedDealership[playerid], playerid);
	new 
		modelid = Dealership_GetModelAt(SelectedDealership[playerid], SelectedListItem{playerid}),
		price = Dealership_GetPriceAt(SelectedDealership[playerid], SelectedListItem{playerid});
	if(listitem == 0)
	{
		Dialog_Show(playerid, Dialog_DealershipDeleteVeh, DIALOG_STYLE_MSGBOX, "Concessionaria", "Sei sicuro di voler rimuovere questo veicolo (%s) dalla concessionaria?", "Rimuovi", "Annulla", Vehicle_GetNameFromModel(modelid));
	}
	else if(listitem == 1)
	{
		Dialog_Show(playerid, Dialog_DealershipVehPrice, DIALOG_STYLE_INPUT, "Modifica Prezzo", "Inserisci il nuovo prezzo del veicolo (%s).\nPrezzo Attuale: %d", "Continua", "Annulla", Vehicle_GetNameFromModel(modelid), price);
	}
	return 1;
}

Dialog:Dialog_DealershipDeleteVeh(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Dealership_ShowInternalVehicles(SelectedDealership[playerid], playerid);
	
	SendFormattedMessage(playerid, COLOR_GREEN, "Hai rimosso il veicolo (%s) dalla concessionaria.", Vehicle_GetNameFromModel(Dealership_GetModelAt(SelectedDealership[playerid], SelectedListItem{playerid})));
	Dealership_DeleteVehicle(SelectedDealership[playerid], SelectedListItem{playerid});

	Dealership_ShowInternalVehicles(SelectedDealership[playerid], playerid);
	return 1;
}

Dialog:Dialog_DealershipVehPrice(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Dealership_ShowInternalVehicles(SelectedDealership[playerid], playerid);
	new 
	   dealershipid = SelectedDealership[playerid],
	   modelid = Dealership_GetModelAt(dealershipid, SelectedListItem{playerid}),
	   price = Dealership_GetPriceAt(dealershipid, SelectedListItem{playerid}),
	   val = strval(inputtext);
    
	if(val <= 0)
	   return Dialog_Show(playerid, Dialog_DealershipVehPrice, DIALOG_STYLE_INPUT, "Modifica Prezzo", "{FF0000}Il prezzo non è valido!\n{FFFFFF}Inserisci il nuovo prezzo per la %s.\nPrezzo Attuale: %d", "Continua", "Annulla", Vehicle_GetNameFromModel(modelid), price);

	Dealership_SetPriceAt(SelectedDealership[playerid], SelectedListItem{playerid}, val);
	SendClientMessage(playerid, COLOR_GREEN, "Prezzo veicolo cambiato con successo.");
	Dealership_ShowInternalVehicles(SelectedDealership[playerid], playerid);
	return 1;
}



stock Dealership_ShowList(playerid)
{
	new 
		String:string = @("ID\tNome\tLocation\n"),
		zone[32],
		data[e_DEALERSHIP_DATA]
	;
	
	for_map(i : Dealerships)
	{
		iter_get_arr(i, data);
		
		Get2DZoneName(data[X], data[Y], data[Z], zone, sizeof(zone));

		string += str_format("%d\t%s\t%s\n", iter_get_key(i), data[Name], zone);
	}
	Dialog_Show_s(playerid, Dialog_SelectDealership, DIALOG_STYLE_TABLIST_HEADERS, @("Lista Concessionarie"), string, "Seleziona", "Chiudi");
	return 1;
}

stock Dealership_ShowOptions(dealershipid, playerid)
{
	if(!Dealership_GetID(dealershipid))
		return 0;
	
	if(Account_GetAdminLevel(playerid) < 2) // Safeness
	   return 0;
	
    if(map_size(Dealerships) == 0)
	   return SendClientMessage(playerid, COLOR_ERROR, "Non esistono concessionarie. Usa /acreatedealership per crearne una.");

	SelectedDealership[playerid] = dealershipid;

	new string[64], name[32];
	Dealership_GetName(dealershipid, name);

	format(string, sizeof(string), "%s [%d]", name, dealershipid);

	if(Dealership_IsOnlyPremium(dealershipid))
		Dialog_Show(playerid, Dialog_DealershipOptions, DIALOG_STYLE_LIST, string, "Lista Veicoli\nModifica Nome\nAggiungi Veicolo\nSposta (Posizione Corrente)\nPosizione Veicolo\nRimuovi Limitazione Premium\nCancella (Definitivamente)", "Avanti", "Chiudi");
	else
		Dialog_Show(playerid, Dialog_DealershipOptions, DIALOG_STYLE_LIST, string, "Lista Veicoli\nModifica Nome\nAggiungi Veicolo\nSposta (Posizione Corrente)\nPosizione Veicolo\nAggiungi Limitazione Premium\nCancella (Definitivamente)", "Avanti", "Chiudi");
	return 1;
}

//
//
//
//  	COMMANDS
//
//
//

flags:adealershipcmds(CMD_DEVELOPER);
CMD:adealershipcmds(playerid, params[])
{
	SendClientMessage(playerid, -1, "[DEALERSHIP]: /acreatedealership - /adealershipmenu");
	return 1;
}

flags:acreatedealership(CMD_DEVELOPER);
CMD:acreatedealership(playerid, params[])
{
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    new id = Dealership_Create(x, y, z, Character_GetVirtualWorld(playerid));
    
	if(id > 0)
	{
		SendClientMessage(playerid, COLOR_GREEN, "Concessionaria creata.");
    	SendClientMessage(playerid, COLOR_GREEN, "Ricorda di settare tutte le impostazioni giuste.");
    	SendClientMessage(playerid, COLOR_GREEN, "Utilizza /adealershipmenu per cominciare a modificare questa concessionaria.");
	}
	return 1;
}

flags:adealershipmenu(CMD_DEVELOPER);
CMD:adealershipmenu(playerid, params[])
{
    Dealership_ShowList(playerid);
    return 1;
}

//
//
//	DEALERSHIP DIALOGS
//
//
//

Dialog:Dialog_SelectDealership(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;
	new dealershipid = map_key_at(Dealerships, listitem);
	
	Dealership_ShowOptions(dealershipid, playerid);
	return 1;
}


Dialog:Dialog_DealershipOptions(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Dealership_ShowList(playerid);
	
	new dealershipid = SelectedDealership[playerid];
	
	if(!Dealership_IsValid(dealershipid))
		return 0;
	
	switch(listitem)
    {
	   case 0: // Vehicle list
	   {
		  return Dealership_ShowInternalVehicles(dealershipid, playerid);
	   }
	   case 1: // Edit Name
	   {
		 return Dialog_Show(playerid, Dialog_DealershipName, DIALOG_STYLE_INPUT, "Dealership Name", "Inserisci il nome della concessionaria.\nLimite Caratteri: 30.", "Modifica", "Indietro");
	   }
	   case 2: // Add Vehicle
	   {
		  return Dialog_Show(playerid, Dialog_DealershipAddVehicle, DIALOG_STYLE_INPUT, "Aggiungi Veicolo", "Inserisci il modello che vuoi inserire seguito dal prezzo!\nEsempio: 522 50000", "Aggiungi", "Indietro");
	   }
	   case 3: // Move
	   {
		  new Float:x, Float:y, Float:z;
		  GetPlayerPos(playerid, x, y, z);
		  Dealership_SetPosition(SelectedDealership[playerid], x, y, z);
		  SendFormattedMessage(playerid, COLOR_GREEN, "La concessionaria (%d) è stata spostata con successo.", dealershipid);
		  return 1;
	   }
	   case 4:
	   {
			if(!IsPlayerInAnyVehicle(playerid))
				return SendClientMessage(playerid, COLOR_ERROR, "Devi essere a bordo di un veicolo.");
			new Float:x, Float:y, Float:z, Float:a;
			GetVehiclePos(GetPlayerVehicleID(playerid), x, y, z);
			GetVehicleZAngle(GetPlayerVehicleID(playerid), a);
			if(Dealership_SetVehicleSpawnPos(dealershipid, x, y, z, a))
				SendClientMessage(playerid, COLOR_GREEN, "Posizione veicolo cambiata con successo.");
		  	return 1;
	   }
	   case 5:
	   {
			Dealership_SetOnlyPremium(dealershipid, !Dealership_IsOnlyPremium(dealershipid));
			if(Dealership_IsOnlyPremium(dealershipid))
				SendClientMessage(playerid, -1, "Hai settato la concessionaria accessibile ai soli utenti premium.");
			else
				SendClientMessage(playerid, -1, "Hai settato la concessionaria accessibile a tutti i giocatori.");
		   	return 1;
	   }
	   case 6: // Delete (def)
	   {
		   	new name[32];
			Dealership_GetName(dealershipid, name);
		  	Dialog_Show(playerid, Dialog_DealershipDelete, DIALOG_STYLE_MSGBOX, "Dealership Delete", "Sei sicuro di voler rimuovere questa concessionaria\ndefinitivamente?\nInfo: %s [%d]", "Elimina", "Annulla", name, SelectedDealership[playerid]);
		  	return 1;
	   }
    }
	return 1;
}

Dialog:Dialog_DealershipName(playerid, response, listitem, inputtext[])
{
	new dealershipid = SelectedDealership[playerid];
	
	if(!Dealership_IsValid(dealershipid))
		return 0;

	if(!response)
		return Dealership_ShowOptions(dealershipid, playerid);
	
	if(strlen(inputtext) > 30)
		return Dialog_Show(playerid, Dialog_DealershipName, DIALOG_STYLE_INPUT, "Dealership Edit Name", "Il nome inserito è troppo lungo!\nInserisci il nome della concessionaria.\nLimite Caratteri: 30.", "Modifica", "Indietro");
	
	if(Dealership_SetName(dealershipid, inputtext))
	{
		SendFormattedMessage(playerid, COLOR_GREEN, "Hai cambiato il nome della concessionaria. Nuovo nome: %s", inputtext);
	}
	else
	{
		SendClientMessage(playerid, COLOR_ERROR, "Il cambio del nome non è andato a buon fine.");
	}
    return Dealership_ShowOptions(dealershipid, playerid);
}

Dialog:Dialog_DealershipAddVehicle(playerid, response, listitem, inputtext[])
{
	new dealershipid = SelectedDealership[playerid];
	if(!response)
		return Dealership_ShowOptions(dealershipid, playerid);
	
	new modelid, price;

    if(isnull(inputtext) || sscanf(inputtext, "k<vehicle>d", modelid, price) || price < 0)
    {
	   return Dialog_Show(playerid, Dialog_DealershipAddVehicle, DIALOG_STYLE_INPUT, "Aggiungi Veicolo", "{FF0000}La stringa che hai inserito non è corretta!\n{FFFFFF}Inserisci il modello che vuoi inserire seguito dal prezzo!\n-- Esempi --\n- 522 50000\n- NRG 50000", "Aggiungi", "Indietro");
    }
    if(Dealership_AddVehicle(SelectedDealership[playerid], modelid, price))
    	SendFormattedMessage(playerid, COLOR_GREEN, "Hai aggiunto %s (%d) per $%d alla concessionaria (%d).", Vehicle_GetNameFromModel(modelid), modelid, price, SelectedDealership[playerid]);
	else
		SendClientMessage(playerid, COLOR_ERROR, "La concessionaria possiede già questo modello.");
	return 1;
}


Dialog:Dialog_DealershipDelete(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
	   return Dealership_ShowOptions(SelectedDealership[playerid], playerid);
    }
    if(Dealership_Delete(SelectedDealership[playerid]))
    {
		SendFormattedMessage(playerid, COLOR_ERROR, "Concessionaria [%d] eliminata con successo.", SelectedDealership[playerid]);
    }
    else
    {
	   SendClientMessage(playerid, COLOR_ERROR, "C'è stato un errore durante l'eliminazione della Concessionaria.");
    }
    
    return 1;
}

stock Dealership_GetColorByIndex(index)
{
	static const CarColors[] =
	{
		0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,
		26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,
		52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,
		78,79,80,81,82,83,84,85,86,87,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,
		104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,

		/*127,*/128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,
		150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,
		173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,
		196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,
		219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,
		242,243,244,245,246,247,248,249,250,251,252,253,254,255
	};
	return CarColors[index%254];
}