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
    MySQL_TQueryInline(gMySQL, using inline OnInsert, "INSERT INTO buildings (Name, OwnerName, WelcomeText, EnterX, EnterY, EnterZ, EnterInterior, EnterWorld, ExitX, ExitY, ExitZ, ExitInterior, Ownable, OwnerID, Price, Locked) \
						   VALUES('SET_NAME', '', '', '%f', '%f', '%f', '%d', '%d', '0.0', '0.0', '0.0', '0', '0', '0', '0', '1')", 
										  x, y, z, interior, world);
    return 1;
}

stock Building_LoadAll()
{
	printf("Building_LoadAll");

	new Cache:result = mysql_query(gMySQL, "SELECT * FROM `buildings` ORDER BY ID", true);

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
    mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `buildings` WHERE ID = '%d'", buildingid);
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
    Name = '%e', OwnerName = '%e', \
    WelcomeText = '%e', \
    EnterX = '%f', EnterY = '%f', EnterZ = '%f', \
    EnterInterior = '%d', EnterWorld = '%d', \
    ExitX = '%f', ExitY = '%f', ExitZ = '%f', \
    ExitInterior = '%d', \
    Ownable = '%d', OwnerID = '%d', \
    Price = '%d', Locked = '%d', \
	Faction = '%d', \
	Type = '%d', \
	Money = '%d' \
    WHERE ID = '%d'", 
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