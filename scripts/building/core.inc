#include <building\garage>
#include <YSI_Coding\y_hooks>



enum _:E_BUILDING_INFO
{
    bID,
    bExists,
    bName[MAX_BUILDING_NAME],
    bWelcomeText[MAX_WELCOME_TEXT_LENGTH],
    Float:bEnterX,
    Float:bEnterY,
    Float:bEnterZ,
    bEnterInterior,
    bEnterWorld,
    Float:bExitX,
    Float:bExitY,
    Float:bExitZ,
    bExitInterior,
	bExitWorld,
    bOwnable,
    bOwnerID,
    bOwnerName[MAX_PLAYER_NAME],
    bPrice,
    bLocked,
	bFaction,
	bType,
	bMoney,
    // Volatile
    bEnterPickupID,
    Text3D:bEnter3DText,
    bExitPickupID,
	Text3D:bExit3DText,
};

static
	Map:Buildings;

stock Map:GetBuildingsMap()
{
	return Buildings;
}

hook OnGameModeInit()
{
	Buildings = map_new();
	mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS `buildings` ( \
						`id` int(11) NOT NULL AUTO_INCREMENT, \
						`name` varchar(64) DEFAULT NULL, \
						`owner_name` varchar(24) DEFAULT NULL, \
						`welcome_text` varchar(120) DEFAULT NULL, \
						`enter_x` float DEFAULT '0', \
						`enter_y` float DEFAULT '0', \
						`enter_z` float DEFAULT '0', \
						`enter_interior` int(11) DEFAULT '0', \
						`enter_world` int(11) DEFAULT '0', \
						`exit_x` float DEFAULT '0', \
						`exit_y` float DEFAULT '0', \
						`exit_z` float DEFAULT '0', \
						`exit_interior` int(11) DEFAULT '0', \
						`ownable` int(11) DEFAULT '0', \
						`owner_id` int(11) DEFAULT NULL, \
						`price` int(11) DEFAULT '0', \
						`locked` int(11) DEFAULT '1', \
						`faction` int(11) NOT NULL DEFAULT '-1', \
						`type` tinyint(4) NOT NULL DEFAULT '0', \
						`money` int(11) NOT NULL DEFAULT '0', \
						PRIMARY KEY (`id`), \
						KEY `OwnerID` (`owner_id`), \
						CONSTRAINT `BuildingChars` FOREIGN KEY (`owner_id`) REFERENCES `characters` (`id`) ON DELETE SET NULL \
						) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1", false);
	Building_LoadAll();
	return 1;
}

hook OnCharacterLoad(playerid)
{
	new buildingid = Character_GetBuildingKey(playerid);
	if(buildingid > 0)
	{
		if(!Building_IsValid(buildingid) || Building_GetOwnerID(buildingid) != Character_GetID(playerid))
		{
			SendClientMessage(playerid, COLOR_ERROR, "L'edificio che possedevi non esiste più o non è più tuo.");
			Character_SetBuildingKey(playerid, 0);
		}
	}
	return 1;
}

stock Building_Create(Float:x, Float:y, Float:z, interior, world)
{
    inline OnInsert()
    {
		new id = cache_insert_id();

		new data[E_BUILDING_INFO];

		set(data[bName], "SET_NAME");
		set(data[bWelcomeText], "");
		set(data[bOwnerName], "");
		
		data[bEnterX] = x;
		data[bEnterY] = y;
		data[bEnterZ] = z;
		data[bEnterInterior] = interior;
		data[bEnterWorld] = world;
		data[bOwnable] = 0;
		data[bOwnerID] = 0;
		data[bPrice] = 0;
		data[bLocked] = 1;
		data[bExists] = 1;

		map_add_arr(Buildings, id, data);

		Building_CreateElements(id);

		
    }
    MySQL_TQueryInline(gMySQL, using inline OnInsert, "INSERT INTO buildings (name, owner_name, welcome_text, enter_x, enter_y, enter_z, enter_interior, enter_world, exit_x, exit_y, exit_z, exit_interior, ownable, owner_id, price, locked) \
						   VALUES('SET_NAME', '', '', '%f', '%f', '%f', '%d', '%d', '0.0', '0.0', '0.0', '0', '0', '0', '0', '1')", 
										  x, y, z, interior, world);
    return 1;
}

stock Building_LoadAll()
{
	printf("Building_LoadAll");

	new Cache:result = mysql_query(gMySQL, "SELECT * FROM `buildings` ORDER BY id", true);

	printf("\nLoading Buildings...");
	new count = cache_num_rows();
	new data[E_BUILDING_INFO];
	for(new i = 0; i < count; ++i)
	{
		cache_get_value_index_int(i, 0, data[bID]);
		cache_get_value_index(i, 1, data[bName]);
		cache_get_value_index(i, 2, data[bOwnerName]);
		cache_get_value_index(i, 3, data[bWelcomeText]);
		cache_get_value_index_float(i, 4, data[bEnterX]);
		cache_get_value_index_float(i, 5, data[bEnterY]);
		cache_get_value_index_float(i, 6, data[bEnterZ]);
		cache_get_value_index_int(i, 7, data[bEnterInterior]);
		cache_get_value_index_int(i, 8, data[bEnterWorld]);
		cache_get_value_index_float(i, 9, data[bExitX]);
		cache_get_value_index_float(i, 10, data[bExitY]);
		cache_get_value_index_float(i, 11, data[bExitZ]);
		cache_get_value_index_int(i, 12, data[bExitInterior]);
		cache_get_value_index_int(i, 13, data[bOwnable]);
		cache_get_value_index_int(i, 14, data[bOwnerID]);
		cache_get_value_index_int(i, 15, data[bPrice]);
		cache_get_value_index_int(i, 16, data[bLocked]);
		cache_get_value_index_int(i, 17, data[bFaction]);
		cache_get_value_index_int(i, 18, data[bType]);

		cache_get_value_index_int(i, 18, data[bMoney]);

		data[bExitWorld] = BUILDING_START_WORLD + data[bID];

		data[bExists] = 1;

		map_add_arr(Buildings, data[bID], data);

		Building_CreateElements(data[bID]);
		//printf("Type: %d", BuildingInfo[i][bType]);
		//Iter_Add(Buildings, i);
	}
	printf("Loaded %d buildings.\n", count);
	cache_delete(result);
	return 1;
}

stock Building_SetPosition(buildingid, Float:x, Float:y, Float:z, vw, int)
{
    if(!Building_IsValid(buildingid))
	   return 0;
	
	map_set_cell(Buildings, buildingid, bEnterX, x);
	map_set_cell(Buildings, buildingid, bEnterY, y);
	map_set_cell(Buildings, buildingid, bEnterZ, z);
	map_set_cell(Buildings, buildingid, bEnterWorld, vw);
	map_set_cell(Buildings, buildingid, bEnterInterior, int);
    Building_Save(buildingid);

    Building_DestroyElements(buildingid);
    Building_CreateElements(buildingid);
    return 1;
}

stock Building_SetName(buildingid, name[MAX_BUILDING_NAME])
{
    if(!Building_IsValid(buildingid))
	   return 0;
	new 
		Iter:iter = Iter:map_iter_at(Buildings, buildingid)
	;

	iter_set_cells(iter, bName, name);
    Building_Save(buildingid);

    Building_DestroyElements(buildingid);
	Building_CreateElements(buildingid);
    return 1;
}

stock Building_SetInterior(buildingid, Float:x, Float:y, Float:z, interiorid)
{
    if(!Building_IsValid(buildingid))
	   return 0;

	map_set_cell(Buildings, buildingid, bExitX, x);
	map_set_cell(Buildings, buildingid, bExitY, y);
	map_set_cell(Buildings, buildingid, bExitZ, z);
	map_set_cell(Buildings, buildingid, bExitInterior, interiorid);
	map_set_cell(Buildings, buildingid, bExitWorld, BUILDING_START_WORLD + buildingid);

    Building_Save(buildingid);

	Building_DestroyElements(buildingid);
	Building_CreateElements(buildingid);
    return 1;
}

stock Building_SetOwnable(buildingid, ownable)
{
    if(!Building_IsValid(buildingid))
	   return 0;
	map_set_cell(Buildings, buildingid, bOwnable, ownable);
    Building_Save(buildingid);
    return 1;
}

stock Building_SetPrice(buildingid, price)
{
    if(!Building_IsValid(buildingid))
	   return 0;
    map_set_cell(Buildings, buildingid, bPrice, price);
    Building_Save(buildingid);
    return 1;
}

stock Building_SetWelcomeText(buildingid, text[MAX_WELCOME_TEXT_LENGTH])
{
	if(!Building_IsValid(buildingid))
	   return 0;
	new Iter:iter = Iter:map_iter_at(Buildings, buildingid);
	iter_set_cells(iter, bWelcomeText, text);
	return 1;
}

stock Building_GetWelcomeText(buildingid, text[MAX_WELCOME_TEXT_LENGTH])
{
	if(!Building_IsValid(buildingid))
	   return 0;
	new Iter:iter = map_iter_at(Buildings, buildingid);
    iter_get_md_arr(iter, {bWelcomeText}, text);
	return 1;
}

stock String:Building_GetWelcomeTextStr(buildingid)
{
	new text[MAX_WELCOME_TEXT_LENGTH];
	Building_GetWelcomeText(buildingid, text);
	return str_new(text);
}

stock Building_SetOwner(buildingid, playerid)
{
    if(!Building_IsValid(buildingid) || !Building_IsOwnable(buildingid) || !Character_IsLogged(playerid))
	   return 0;
	map_set_cell(Buildings, buildingid, bOwnerID, Character_GetID(playerid));
	new Iter:iter = Iter:map_iter_at(Buildings, buildingid);
	iter_set_cells(iter, bOwnerName, Character_GetOOCName(playerid));
    Log(Character_GetOOCName(playerid), "", "Building_SetOwner", buildingid);
    return 1;
}

stock Building_ResetOwner(buildingid)
{
    if(!Building_IsValid(buildingid))
	   return 0;
    map_set_cell(Buildings, buildingid, bOwnerID, 0);
	new Iter:iter = Iter:map_iter_at(Buildings, buildingid);
	iter_set_cells(iter, bOwnerName, "");
    Log("", "", "Building_ResetOwner", buildingid);
    return 1;
}

stock Building_Delete(buildingid)
{
	if(!map_has_key(Buildings, buildingid))
		return 0;
	
    new query[64];
    mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `buildings` WHERE id = '%d'", buildingid);
    mysql_tquery(gMySQL, query);

	Building_DestroyElements(buildingid);
	
	// UPDATE OWNERID -> BUILDING_ID TO 0.
    
	map_remove_deep(Buildings, buildingid);
    return 1;
}

stock Building_Save(buildingid)
{
    if(!Building_IsValid(buildingid))
	   return 0;

	new name[64], ownerName[MAX_PLAYER_NAME], welcomeText[MAX_WELCOME_TEXT_LENGTH],
		Float:enterX, Float:enterY, Float:enterZ,
		Float:exitX, Float:exitY, Float:exitZ,
    	query[512];
	
	Building_GetName(buildingid, name);
	Building_GetOwnerName(buildingid, ownerName);
	Building_GetWelcomeText(buildingid, welcomeText);
	Building_GetEnterPos(buildingid, enterX, enterY, enterZ);
	Building_GetExitPos(buildingid, exitX, exitY, exitZ);

    mysql_format(gMySQL, query, sizeof(query), "UPDATE `buildings` SET \
    name = '%e', owner_name = '%e', \
    welcome_text = '%e', \
    enter_x = '%f', enter_y = '%f', enter_z = '%f', \
    enter_interior = '%d', enter_world = '%d', \
    exit_x = '%f', exit_y = '%f', exitz = '%f', \
    exit_interior = '%d', \
    ownable = '%d', owner_id = '%d', \
    price = '%d', locked = '%d', \
	faction = '%d', \
	type = '%d', \
	money = '%d' \
    WHERE id = '%d'", 
    name, ownerName,
	welcomeText,
    enterX, enterY, enterZ,
    Building_GetEnterInterior(buildingid), Building_GetEnterWorld(buildingid),
    exitX, exitY, exitZ,
    Building_GetExitInterior(buildingid),
    Building_IsOwnable(buildingid), Building_GetOwnerID(buildingid),
    Building_GetPrice(buildingid), Building_IsLocked(buildingid),
	Building_GetFaction(buildingid),
	Building_GetType(buildingid),
	Building_GetMoney(buildingid),
    Building_GetID(buildingid));
	mysql_tquery(gMySQL, query);
    return 1;
}

stock Building_CreateElements(buildingid)
{
	new Float:enterX, Float:enterY, Float:enterZ, Float:exitX, Float:exitY, Float:exitZ,
		name[MAX_BUILDING_NAME];
	
	Building_GetName(buildingid, name);
	Building_GetEnterPos(buildingid, enterX, enterY, enterZ);
	Building_GetExitPos(buildingid, exitX, exitY, exitZ);
    
	new enterPickup = Pickup_Create(1239, buildingid, enterX, enterY, enterZ, ELEMENT_TYPE_BUILDING_ENTRANCE, Building_GetEnterWorld(buildingid), Building_GetEnterInterior(buildingid));
	map_set_cell(Buildings, buildingid, bEnterPickupID, enterPickup);

    new Text3D:enterText = CreateDynamic3DTextLabel(name, COLOR_LIGHTBLUE/*2*/, enterX, enterY, enterZ + 0.55, 20, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, Building_GetEnterWorld(buildingid));
	map_set_cell(Buildings, buildingid, bEnter3DText, enterText);

    new exitPickup = Pickup_Create(1007, buildingid, exitX, exitY, exitZ, ELEMENT_TYPE_BUILDING_EXIT, Building_GetExitWorld(buildingid), Building_GetExitInterior(buildingid));
	map_set_cell(Buildings, buildingid, bExitPickupID, exitPickup);

	new Text3D:exitText = CreateDynamic3DTextLabel("Uscita", COLOR_LIGHTBLUE/*2*/, exitX, exitY, exitZ + 0.55, 20, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, Building_GetExitWorld(buildingid), Building_GetExitInterior(buildingid));
	map_set_cell(Buildings, buildingid, bExit3DText, exitText);

}

stock Building_DestroyElements(buildingid)
{
	if(!Building_IsValid(buildingid))
		return 0;
    Pickup_Destroy(map_get(Buildings, buildingid, bEnterPickupID));
    Pickup_Destroy(map_get(Buildings, buildingid, bExitPickupID));
	new Text3D:t = Text3D:map_get(Buildings, buildingid, bEnter3DText);
    DestroyDynamic3DTextLabelEx(t);
	t = Text3D:map_get(Buildings, buildingid, bExit3DText);
	DestroyDynamic3DTextLabelEx(t);
	return 1;
}

// GET/SET

stock Building_IsValid(a) return map_has_key(Buildings, a);

stock Building_GetName(buildingid, name[MAX_BUILDING_NAME])
{
	if(!Building_IsValid(buildingid))
		return 0;
	new Iter:iter = map_iter_at(Buildings, buildingid);
    iter_get_md_arr(iter, {bName}, name);
    return 1;
}

stock Building_IsOwnable(a)
{
	if(!Building_IsValid(a))
		return 0;
    return map_get(Buildings, a, bOwnable);
}

stock Building_IsOwned(a)
{
	if(!Building_IsValid(a))
		return 0;
    return Building_IsOwnable(a) && Building_GetOwnerID(a) != 0;
}

stock Building_GetOwnerID(a)
{
	if(!Building_IsValid(a))
		return 0;
    return map_get(Buildings, a, bOwnerID);
}

stock Building_GetOwnerName(a, name[MAX_PLAYER_NAME])
{
	if(!Building_IsValid(a))
		return 0;
    new Iter:iter = map_iter_at(Buildings, a);
    iter_get_md_arr(iter, {bOwnerName}, name);
    return 1;
}

stock Building_IsLocked(a)
{
	if(!Building_IsValid(a))
		return 0;
    return map_get(Buildings, a, bLocked);
}

stock Building_GetPrice(a)
{
	if(!Building_IsValid(a))
		return 0;
    return map_get(Buildings, a, bPrice);
}

stock Building_SetFaction(buildingid, factionid)
{
	if(!Building_IsValid(buildingid))
		return 0;
	map_set_cell(Buildings, buildingid, bFaction, factionid);
	return 1;
}

stock Building_GetFaction(buildingid)
{
	if(!Building_IsValid(buildingid))
		return INVALID_FACTION_ID;
	return map_get(Buildings, buildingid, bFaction);
}

stock Building_GetEnterInterior(buildingid)
{
	if(!Building_IsValid(buildingid))
		return 0;
	return map_get(Buildings, buildingid, bEnterInterior);
}

stock Building_GetEnterWorld(buildingid)
{
	if(!Building_IsValid(buildingid))
		return 0;
	return map_get(Buildings, buildingid, bEnterWorld);
}

stock Building_GetExitInterior(buildingid)
{
	if(!Building_IsValid(buildingid))
		return 0;
	return map_get(Buildings, buildingid, bExitInterior);
}

stock Building_GetExitWorld(buildingid)
{
	if(!Building_IsValid(buildingid))
		return 0;
	return map_get(Buildings, buildingid, bExitWorld);
}

stock Building_GetEnterPos(buildingid, &Float:x, &Float:y, &Float:z)
{
	if(!Building_IsValid(buildingid))
		return 0;
	x = Float:map_get(Buildings, buildingid, bEnterX);
	y = Float:map_get(Buildings, buildingid, bEnterY);
	z = Float:map_get(Buildings, buildingid, bEnterZ);
	return 1;
}

stock Building_GetExitPos(buildingid, &Float:x, &Float:y, &Float:z)
{
	if(!Building_IsValid(buildingid))
		return 0;
	x = Float:map_get(Buildings, buildingid, bExitX);
	y = Float:map_get(Buildings, buildingid, bExitY);
	z = Float:map_get(Buildings, buildingid, bExitZ);
	return 1;
}

stock Building_GetType(buildingid)
{
	if(!Building_IsValid(buildingid))
		return -1;
	return map_get(Buildings, buildingid, bType);
}

stock Building_SetType(buildingid, type)
{
	if(!Building_IsValid(buildingid) || type < 0 || type >= BUILDING_TYPE_LAST)
		return 0;
	map_set_cell(Buildings, buildingid, bType, type);
	return 1;
}

stock Building_GetID(buildingid)
{
	if(!Building_IsValid(buildingid))
		return 0;
	return map_get(Buildings, buildingid, bID);
}

stock Building_SetLocked(buildingid, locked)
{
	if(!Building_IsValid(buildingid))
		return 0;
	map_set_cell(Buildings, buildingid, bLocked, locked);
	return 1;
}

stock Building_GetMoney(buildingid)
{
	if(!Building_IsValid(buildingid))
		return 0;
	return map_get(Buildings, buildingid, bMoney);
}

stock Building_GiveMoney(buildingid, amount)
{
	return Building_SetMoney(buildingid, map_get(Buildings, buildingid, bMoney) + amount);
}

stock Building_SetMoney(buildingid, amount)
{
	if(!Building_IsValid(buildingid))
		return 0;
	map_set_cell(Buildings, buildingid, bMoney, amount);
	return 1;
}

stock Character_ShowBuildingDialog(playerid)
{
	new buildingid = Character_GetNearBuildingIDMenu(playerid);
	if(buildingid != -1 && Building_IsOwnable(buildingid))
	{
		return Dialog_Show(playerid, Dialog_Building, DIALOG_STYLE_LIST, "Edificio", "Apri/Chiudi Porta\nInventario\nDeposita Soldi\nRitira Soldi\nVendi\nVendi a Giocatore", "Continua", "Chiudi");
	}
	return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata o all'interno di un tuo edificio."), 0;
}

stock Character_AcceptBuilding(playerid)
{
	if(!Request_IsPending(playerid) || Request_GetType(playerid) != REQUEST_TYPE_REPAIR)
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai una richiesta di riparazione attiva."), 0;
	
    if(!IsPlayerInRangeOfPlayer(playerid, Request_GetSender(playerid), 5.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore."), 0;
	
	new senderid = Request_GetSender(playerid),
		buildingid = Request_GetItem(playerid),
        price = Request_GetAmount(playerid)
    ;

	if(buildingid == -1)
	{
		ResetPendingRequest(playerid);
		ResetPendingRequest(senderid);
		return 0;
	}

	if(Building_GetOwnerID(buildingid) != Character_GetID(senderid))
	{
		SendClientMessage(playerid, COLOR_ERROR, "Il giocatore che ti ha inviato la richiesta non è più proprietario dell'edificio.");
		ResetPendingRequest(playerid);
		ResetPendingRequest(senderid);
		return 0;
	}

    Character_GiveMoney(playerid, -price, "accept_building");
    Character_GiveMoney(senderid, price, "sell_building_to_player");
    
    SendFormattedMessage(senderid, COLOR_ACCEPT_REQUEST, "%s ha accettato la tua richiesta di vendita del tuo edificio. Hai guadagnato $%d.", Character_GetRolePlayName(playerid), price);
    SendFormattedMessage(playerid, COLOR_ACCEPT_REQUEST, "%s ti ha venduto il suo edificio per $%d.", Character_GetRolePlayName(senderid), price);

    ResetPendingRequest(playerid);
    ResetPendingRequest(senderid);
	return 1;
}

Dialog:Dialog_Building(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;
	new buildingid = Character_GetNearBuildingIDMenu(playerid);
	if(buildingid == -1)
		return 0;
	switch(listitem)
	{
		case 0:
		{
			Building_SetLocked(buildingid, !Building_IsLocked(buildingid));
			if(Building_IsLocked(buildingid))
				SendClientMessage(playerid, COLOR_GREEN, "Hai chiuso il tuo edificio.");
			else
				SendClientMessage(playerid, COLOR_GREEN, "Hai aperto il tuo edificio.");
			Building_Save(buildingid);
			return 1;
		}
		case 1:
		{
			return SendClientMessage(playerid, COLOR_ERROR, "Non ancora disponibile ^^");
		}
		case 2:
		{
			return Dialog_Show(playerid, Dialog_BuildingDepositMoney, DIALOG_STYLE_INPUT, "Deposita Denaro", "Inserisci l'ammontare di denaro che vuoi depositare all'interno del tuo edificio.", "Deposita", "Indietro");
		}
		case 3:
		{
			return Dialog_Show(playerid, DialogBuildingWithdrawMoney, DIALOG_STYLE_INPUT, "Ritira Denaro", "Inserisci l'ammontare di denaro che vuoi ritirare dall'interno del tuo edificio.", "Ritira", "Indietro");
		}
		case 4:
		{
			new money = Building_GetPrice(buildingid)/2;
			new name[MAX_BUILDING_NAME];
			Building_GetName(buildingid, name);
			return Dialog_Show(playerid, Dialog_BuildingSell, DIALOG_STYLE_MSGBOX, "Vendita Edificio", "Sei sicuro di voler vendere questo edificio (%s) per $%d?", "Vendi", "Indietro", name, money);
		}
		case 5:
		{
			return Dialog_Show(playerid, Dialog_BuildingSellToPlayer, DIALOG_STYLE_INPUT, "Vendita Edificio a Giocatore", "Inserisci l'id, il nome o il codice (#XXXXX) del giocatore a cui vuoi vendere l'edificio, seguito dal prezzo.\nEsempi:\n> 1 50010\n> Pinco_Pallino 50000\n> #0001 50000", "Vendi", "Indietro");
		}
	}
	return 1;
}

Dialog:Dialog_BuildingDepositMoney(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Character_ShowBuildingDialog(playerid);
	
	new buildingid = Character_GetNearBuildingIDMenu(playerid);
	if(buildingid == -1)
		return 0;
	
	new money =  strval(inputtext);
	if(money <= 0 || money > Character_GetMoney(playerid))
		return Dialog_Show(playerid, Dialog_BuildingDepositMoney, DIALOG_STYLE_INPUT, "Deposita Denaro", "{FF0000}Non disponi della quantità di denaro inserita.\n{FFFFFF}Inserisci l'ammontare di denaro che vuoi depositare all'interno del tuo edificio.", "Deposita", "Indietro");
	
	Building_GiveMoney(buildingid, money);
	Character_GiveMoney(playerid, -money, "building_deposit");

	SendFormattedMessage(playerid, -1, "Hai depositato $%d all'interno dell'edificio.", money);

	Character_Save(playerid);
	Building_Save(buildingid);
	return 1;
}

Dialog:DialogBuildingWithdrawMoney(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Character_ShowBuildingDialog(playerid);
	
	new buildingid = Character_GetNearBuildingIDMenu(playerid);
	if(buildingid == -1)
		return 0;
	
	new money = strval(inputtext);
	if(money <= 0 || money > Building_GetMoney(buildingid))
		return Dialog_Show(playerid, DialogBuildingWithdrawMoney, DIALOG_STYLE_INPUT, "Ritira Denaro", "{FF0000}L'edificio non dispone della quantità di denaro inserita.\n{FFFFFF}Inserisci l'ammontare di denaro che vuoi ritirare dall'interno del tuo edificio.", "Ritira", "Indietro");
	
	Building_GiveMoney(buildingid, -money);
	Character_GiveMoney(playerid, money, "building_withdraw");

	SendFormattedMessage(playerid, -1, "Hai ritirato $%d dall'interno dell'edificio.", money);

	Character_Save(playerid);
	Building_Save(buildingid);
	return 1;
}

Dialog:Dialog_BuildingSell(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Character_ShowBuildingDialog(playerid);
	
	new buildingid = Character_GetNearBuildingIDMenu(playerid);

	if(buildingid == -1)
		return 0;
	
	new price = Building_GetPrice(buildingid)/2,
		name[MAX_BUILDING_NAME];
	
	Building_GetName(buildingid, name);

	Character_GiveMoney(playerid, price, "building_sell");

	SendFormattedMessage(playerid, -1, "Hai venduto il tuo edificio (%s) per $%d.", name, price);

	Building_ResetOwner(buildingid);

	Building_Save(buildingid);
	Character_Save(playerid);
	return 1;
}

Dialog:Dialog_BuildingSellToPlayer(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Character_ShowBuildingDialog(playerid);
	
	new buildingid = Character_GetNearBuildingIDMenu(playerid);

	if(buildingid == -1)
		return 0;
	
	new id, money;

	if(sscanf(inputtext, "k<u>d", id, money))
		return Dialog_Show(playerid, Dialog_BuildingSellToPlayer, DIALOG_STYLE_INPUT, "Vendita Edificio a Giocatore", "{FF0000}Il formato inserito non è giusto.\nInserisci l'id, il nome o il codice (#XXXXX) del giocatore a cui vuoi vendere l'edificio, seguito dal prezzo.\nEsempi:\n> 1 50010\n> Pinco_Pallino 50000\n> #0001 50000", "Vendi", "Indietro");

	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return Dialog_Show(playerid, Dialog_BuildingSellToPlayer, DIALOG_STYLE_INPUT, "Vendita Edificio a Giocatore", "{FF0000}Il giocatore non è collegato.\nInserisci l'id, il nome o il codice (#XXXXX) del giocatore a cui vuoi vendere l'edificio, seguito dal prezzo.\nEsempi:\n> 1 50010\n> Pinco_Pallino 50000\n> #0001 50000", "Vendi", "Indietro");

	if(!IsPlayerInRangeOfPlayer(playerid, id, 8.0))
		return Dialog_Show(playerid, Dialog_BuildingSellToPlayer, DIALOG_STYLE_INPUT, "Vendita Edificio a Giocatore", "{FF0000}Il giocatore è troppo distante.\nInserisci l'id, il nome o il codice (#XXXXX) del giocatore a cui vuoi vendere l'edificio, seguito dal prezzo.\nEsempi:\n> 1 50010\n> Pinco_Pallino 50000\n> #0001 50000", "Vendi", "Indietro");

	if(money < 0 || money > Character_GetMoney(id))
		return Dialog_Show(playerid, Dialog_BuildingSellToPlayer, DIALOG_STYLE_INPUT, "Vendita Edificio a Giocatore", "{FF0000}Il giocatore non ha abbastanza denaro.\nInserisci l'id, il nome o il codice (#XXXXX) del giocatore a cui vuoi vendere l'edificio, seguito dal prezzo.\nEsempi:\n> 1 50010\n> Pinco_Pallino 50000\n> #0001 50000", "Vendi", "Indietro");

	if(Character_HasBuildingKey(id))
		return Dialog_Show(playerid, Dialog_BuildingSellToPlayer, DIALOG_STYLE_INPUT, "Vendita Edificio a Giocatore", "{FF0000}Il giocatore possiede già un edificio.\nInserisci l'id, il nome o il codice (#XXXXX) del giocatore a cui vuoi vendere l'edificio, seguito dal prezzo.\nEsempi:\n> 1 50010\n> Pinco_Pallino 50000\n> #0001 50000", "Vendi", "Indietro");

	Character_SetRequest(playerid, id, REQUEST_TYPE_BUILDING, buildingid, money);
	SendFormattedMessage(id, COLOR_RECEIVE_REQUEST, "%s vuole venderti il suo edificio per $%d.", Character_GetRolePlayName(playerid), money);
	SendFormattedMessage(playerid, COLOR_SEND_REQUEST, "Hai proposto il tuo edificio a %s per $%d.", Character_GetRolePlayName(id), money);
	return 1;
}


stock Character_GetNearBuildingID(playerid)
{
	if(Character_GetVirtualWorld(playerid) < BUILDING_START_WORLD)
		return -1;
	return Character_GetVirtualWorld(playerid) - BUILDING_START_WORLD;
}

stock Character_GetNearBuilding(playerid, bool:inside)
{
	if(inside)
	{
		/*foreach(new b : Buildings)
		{
			if(BuildingInfo[b][bExists] && Building_GetExitWorld(b) == Character_GetVirtualWorld(playerid))
				return b;
		}*/
		return Character_GetNearBuildingID(playerid);
	}
	else
	{
		new id, E_ELEMENT_TYPE:type;
		if(Character_GetNearestPickup(playerid, id, type) && (type == ELEMENT_TYPE_BUILDING_ENTRANCE || type == ELEMENT_TYPE_BUILDING_EXIT))
			return id;
	}
	return -1;
}

stock Character_GetNearBuildingIDMenu(playerid)
{
	new buildingid = Character_GetNearBuilding(playerid, true);
	if(buildingid == -1)
		buildingid = Character_GetNearBuilding(playerid, false);
	if(buildingid != -1)
	{
		if(Account_IsAdminDuty(playerid) || House_GetOwnerID(buildingid) == Character_GetID(playerid) || House_GetID(buildingid) == Character_GetHouseKey(playerid))
			return buildingid;
	}
	return -1;
}