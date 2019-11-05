
#if defined _included_BUILDING
    #endinput
#endif 

#define _included_BUILDING

enum _:E_BUILDING_TYPE
{
	BUILDING_TYPE_STORE = 0,
	BUILDING_TYPE_PAYCHECK = 1, // Not Used anymore
    BUILDING_TYPE_GARAGE = 2,
	BUILDING_TYPE_COMPLEX = 3,
	BUILDING_TYPE_BANK = 4,
	BUILDING_TYPE_ADS = 5,
	BUILDING_TYPE_GAS_STATION = 6,

	// Insert here other types
	BUILDING_TYPE_LAST
}

#define MAX_BUILDINGS		 		(100)
#define MAX_BUILDING_NAME	  		(64)
#define MAX_WELCOME_TEXT_LENGTH 	(120)
#define BUILDING_START_WORLD		(5000)
#define INVALID_BUILDING_ID			(0)
#define INVALID_BUILDING			(Building:VAR_NULL)

//#define for_building(%0) for(new Iter:%0_iter=map_iter(GetBuildingsMap()),Building:%0=Building:iter_get_value(%0_iter);iter_inside(Iter:%0_iter);(iter_move_next(%0_iter) ? (%0=Building:iter_get_value(%0_iter)) : (%0=INVALID_BUILDING)))
#define for_building(%0) for(new Iter:iter=map_iter(GetBuildingsMap()), Building:%0=Building:iter_get_value(iter);iter_inside(iter);%0=(iter_move_next(iter) != ITER_NULL ? (Building:iter_get_value(iter)) : (Building:0)))
#define for_building_of<%1>(%0) for(new Iter:__iter=list_iter(GetBuildingsListByType(%1)), Building:%0=(iter_inside(__iter) ? (Building:iter_get_value(__iter)) : (Building:0));iter_inside(__iter);%0=(iter_move_next(__iter) != ITER_NULL ? (Building:iter_get_value(__iter)) : (Building:0)))

#include <pickup_system\core>
#include <faction_system\core>
#include <player_system\core>

#include "components\inventory"
#include "components\garage"
#include "components\shop"
#include "components\ads"
#include "components\gas_station"
#include "components\employee"

#include "commands\admin"
#include "commands\bmenu"

#include <YSI_Coding\y_hooks>


forward OnBuildingLoaded(Building:building);
forward OnBuildingDeleted(Building:building);
forward OnPlayerEnterBuilding(playerid, Building:building);

static enum _:E_BUILDING_INFO
{
    ID,
    Name[MAX_BUILDING_NAME],
    WelcomeText[MAX_WELCOME_TEXT_LENGTH],
    Float:EnterX,
    Float:EnterY,
    Float:EnterZ,
    EnterInterior,
    EnterWorld,
    Float:ExitX,
    Float:ExitY,
    Float:ExitZ,
    ExitInterior,
	ExitWorld,
    Ownable,
    OwnerID,
    OwnerName[MAX_PLAYER_NAME],
    Price,
    IsLocked,
	Faction,
	Type,
	Money,
	Map:Data,

    // Volatile
    EnterPickupID,
    Text3D:Enter3DText,
    ExitPickupID,
	Text3D:Exit3DText,
};

static 
	Map:Buildings,
	List:BuildingsByType[_:E_BUILDING_TYPE];

static Building:CurrentEditing[MAX_PLAYERS] = {INVALID_BUILDING, ...};

stock Map:GetBuildingsMap()
{
	return Buildings;
}

stock List:GetBuildingsListByType(type)
{
	return BuildingsByType[type];
}

hook OnGameModeInit()
{
	Buildings = map_new();
	for(new i = 0; i < _:E_BUILDING_TYPE; ++i)
	{
		BuildingsByType[i] = list_new();
	}

	map_set_ordered(Buildings, true);
	mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS `buildings` ( \
						`id` int(11) NOT NULL AUTO_INCREMENT, \
						`name` varchar(64) DEFAULT NULL, \
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

hook OnCharacterChangeName(playerid, newCharacterName[])
{
	for_building(building)
	{
		if(Building_GetOwnerID(building) == Character_GetID(playerid))
		{
			var_set_cells(Variant:building, OwnerName, newCharacterName, MAX_PLAYER_NAME);
		}
	}
	return 1;
}

stock Building:Building_Find(buildingid)
{
	if(!map_has_key(Buildings, buildingid))
		return INVALID_BUILDING;
	return Building:map_get(Buildings, buildingid);
}

stock Building_Create(Float:x, Float:y, Float:z, interior, world)
{
    inline OnInsert()
    {
		new id = cache_insert_id();
		
		new Building:building = Building:var_new_buf(E_BUILDING_INFO);
		var_acquire(Variant:building);
		var_set_cell(Variant:building, ID, id);
		var_set_cells(Variant:building, Name, "SET NAME");
		var_set_cells(Variant:building, WelcomeText, "");
		var_set_cells(Variant:building, OwnerName, "");
		var_set_cell(Variant:building, EnterX, _:x);
		var_set_cell(Variant:building, EnterY, _:y);
		var_set_cell(Variant:building, EnterZ, _:z);
		var_set_cell(Variant:building, EnterInterior, interior);
		var_set_cell(Variant:building, EnterWorld, world);
		var_set_cell(Variant:building, Ownable, 0);
		var_set_cell(Variant:building, OwnerID, 0);
		var_set_cell(Variant:building, Price, 0);
		var_set_cell(Variant:building, ExitWorld, BUILDING_START_WORLD + id);
		var_set_cell(Variant:building, IsLocked, 1);
		var_set_cell(Variant:building, Type, BUILDING_TYPE_STORE);
		var_set_cell(Variant:building, Data, map_new());
		map_add(Buildings, id, _:building);
		list_add(GetBuildingsListByType(BUILDING_TYPE_STORE), building);

		Building_CreateElements(building);

		CallLocalFunction(#OnBuildingLoaded, "d", _:building);
    }
    MySQL_TQueryInline(gMySQL, using inline OnInsert, "INSERT INTO buildings (name, welcome_text, enter_x, enter_y, enter_z, enter_interior, enter_world, exit_x, exit_y, exit_z, exit_interior, ownable, owner_id, price, locked) \
						   VALUES('SET NAME', '', '%f', '%f', '%f', '%d', '%d', '0.0', '0.0', '0.0', '0', '0', '0', '0', '1')", 
										  x, y, z, interior, world);
    return 1;
}

stock Building_LoadAll()
{
	inline OnLoad()
	{
		new count = 0;
		cache_get_row_count(count);
		new data[E_BUILDING_INFO];
		new jsonDataStr[256];
		for(new i = 0; i < count; ++i)
		{
			cache_get_value_name_int(i, "id", data[ID]);
			cache_get_value_name(i, "name", data[Name]);
			cache_get_value_name(i, "welcome_text", data[WelcomeText]);
			cache_get_value_name_float(i, "enter_x", data[EnterX]);
			cache_get_value_name_float(i, "enter_y", data[EnterY]);
			cache_get_value_name_float(i, "enter_z", data[EnterZ]);
			cache_get_value_name_int(i, "enter_interior", data[EnterInterior]);
			cache_get_value_name_int(i, "enter_world", data[EnterWorld]);
			cache_get_value_name_float(i, "exit_x", data[ExitX]);
			cache_get_value_name_float(i, "exit_y", data[ExitY]);
			cache_get_value_name_float(i, "exit_z", data[ExitZ]);
			cache_get_value_name_int(i, "exit_interior", data[ExitInterior]);
			cache_get_value_name_int(i, "ownable", data[Ownable]);
			cache_get_value_name_int(i, "owner_id", data[OwnerID]);
			cache_get_value_name_int(i, "price", data[Price]);
			cache_get_value_name_int(i, "locked", data[IsLocked]);
			cache_get_value_name_int(i, "faction", data[Faction]);
			cache_get_value_name_int(i, "type", data[Type]);

			cache_get_value_name_int(i, "money", data[Money]);

			cache_get_value_name(i, "data", jsonDataStr);

			data[ExitWorld] = BUILDING_START_WORLD + data[ID];

			new Building:building = Building:var_new_buf(E_BUILDING_INFO);
			var_acquire(Variant:building);
			var_set_cell(Variant:building, ID, data[ID]);
			var_set_cells(Variant:building, Name, data[Name]);
			var_set_cells(Variant:building, WelcomeText, data[WelcomeText]);
			var_set_cells(Variant:building, OwnerName, data[OwnerName]);
			var_set_cell(Variant:building, EnterX, _:data[EnterX]);
			var_set_cell(Variant:building, EnterY, _:data[EnterY]);
			var_set_cell(Variant:building, EnterZ, _:data[EnterZ]);
			var_set_cell(Variant:building, EnterInterior, data[EnterInterior]);
			var_set_cell(Variant:building, EnterWorld, data[EnterWorld]);
			var_set_cell(Variant:building, ExitX, _:data[ExitX]);
			var_set_cell(Variant:building, ExitY, _:data[ExitY]);
			var_set_cell(Variant:building, ExitZ, _:data[ExitZ]);
			var_set_cell(Variant:building, ExitInterior, data[ExitInterior]);
			var_set_cell(Variant:building, ExitWorld, data[ExitWorld]);
			var_set_cell(Variant:building, Ownable, data[Ownable]);
			var_set_cell(Variant:building, OwnerID, data[OwnerID]);
			var_set_cell(Variant:building, Price, data[Price]);
			var_set_cell(Variant:building, IsLocked, data[IsLocked]);
			var_set_cell(Variant:building, Faction, data[Faction]);
			var_set_cell(Variant:building, Type, data[Type]);
			
			new Map:map = map_new(), Node:node, length, bool:isNull = false;
			cache_is_value_name_null(i, "data", isNull);
			if(!isNull)//if(strlen(jsonDataStr) > 0 && strcmp(jsonDataStr, "NULL", true))
			{
				JsonParse(jsonDataStr, node);
				new ret = JsonArrayLength(node, length);
				if(ret) 
				{
					printf("failed to get array length, error: %d", ret);
				}
				else
				{
					new entryName[32], entryIntValue, Float:entryFloatValue;
					for(new x = 0; x < length; ++x) 
					{
						new Node:item;
						
						if(JsonArrayObject(node, x, item) || JsonGetString(item, "name", entryName)) 
							continue;

						if(JsonGetInt(item, "value", entryIntValue)) 
						{
							JsonGetFloat(item, "value", entryFloatValue);
							map_str_add(map, entryName, entryFloatValue);
						}
						else
						{
							map_str_add(map, entryName, entryIntValue);
						}
					}
				}
			}

			var_set_cell(Variant:building, Data, map);

			map_add(Buildings, data[ID], building);
			list_add(BuildingsByType[Building_GetType(building)], building);

			Building_CreateElements(building);
			CallLocalFunction(#OnBuildingLoaded, "d", _:building);
		}
		printf("%d buildings loaded.\n", count);

		new ownerid;
		for_building(building)
		{
			// DON'T USE CACHE FUNCTIONS AFTER THIS IF
			ownerid = Building_GetOwnerID(building);
			if(ownerid > 0)
			{
				new Cache:cache = mysql_query_f(gMySQL, true, "SELECT name FROM `characters` WHERE id = '%d'", ownerid);
				if(cache_num_rows() <= 0)
					continue;
				new name[MAX_PLAYER_NAME];
				cache_get_value_name(0, "name", name);
				var_set_cells(Variant:building, OwnerName, name);
				cache_delete(cache);
			}
		}
	}

	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `buildings`");
	return 1;
}

stock Map:Building_GetDataMap(Building:building)
{
	return Map:var_get(Variant:building, Data);
}

stock bool:Building_GetFloatData(Building:building, const key[], &Float:ret)
{
	new Map:data = Map:var_get(Variant:building, Data);
	if(map_has_str_key(data, key))
	{
		ret = Float:map_str_get(data, key);
		return true;
	}
	return false;
}

stock bool:Building_SetFloatData(Building:building, const key[], Float:value)
{
	new Map:data = Map:var_get(Variant:building, Data);
	if(map_has_str_key(data, key))
	{
		map_str_set(data, key, value);
		Building_SaveData(building);
		return true;
	}
	map_str_add(data, key, value);
	return true;
}

stock bool:Building_SetIntData(Building:building, const key[], value)
{
	new Map:data = Map:var_get(Variant:building, Data);
	if(map_has_str_key(data, key))
	{
		map_str_set(data, key, value);
		Building_SaveData(building);
		return true;
	}
	map_str_add(data, key, value);
	return true;
}

stock bool:Building_GetIntData(Building:building, const key[], &ret)
{
	new Map:data = Map:var_get(Variant:building, Data);
	if(map_has_str_key(data, key))
	{
		ret = map_str_get(data, key);
		return true;
	}
	return false;
}

static Building_SaveData(Building:building)
{
	new Node:root = JsonArray(), Node:temp, n[32];
	for_map(i : Building_GetDataMap(building))
	{
		iter_get_key_str(i, n);
        new v = iter_get(i);

        temp = JsonObject("name", JsonString(n), "value", JsonInt(v));
        root = JsonAppend(root, JsonArray(temp));
	}
    new s[255];
    JsonStringify(root, s, sizeof(s));
	mysql_pquery_f(gMySQL, "UPDATE `buildings` SET data = '%e' WHERE id = '%d'", s, Building_GetID(building));
}

stock Building_SetPosition(Building:building, Float:x, Float:y, Float:z, vw, int)
{

	var_set_cell(Variant:building, EnterX, x);
	var_set_cell(Variant:building, EnterY, y);
	var_set_cell(Variant:building, EnterZ, z);
	var_set_cell(Variant:building, EnterWorld, vw);
	var_set_cell(Variant:building, EnterInterior, int);
    
	mysql_pquery_f(gMySQL, "UPDATE `buildings` SET enter_x = '%f', enter_y = '%f', enter_z = '%f', enter_interior = '%d', enter_world = '%d' WHERE id = '%d'", 
					x, y, z, int, vw, Building_GetID(building));

    Building_DestroyElements(building);
    Building_CreateElements(building);
    return 1;
}

stock bool:Building_SetName(Building:building, name[MAX_BUILDING_NAME])
{

	var_set_cells(Variant:building, Name, name);
    
	mysql_pquery_f(gMySQL, "UPDATE `buildings` SET name = '%e' WHERE id = '%d'", name, Building_GetID(building));

    Building_DestroyElements(building);
	Building_CreateElements(building);
    return true;
}

stock Building_SetInterior(Building:building, Float:x, Float:y, Float:z, interiorid)
{

	var_set_cell(Variant:building, ExitX, x);
	var_set_cell(Variant:building, ExitY, y);
	var_set_cell(Variant:building, ExitZ, z);
	var_set_cell(Variant:building, ExitInterior, interiorid);
	var_set_cell(Variant:building, ExitWorld, BUILDING_START_WORLD + Building_GetID(building));

    mysql_pquery_f(gMySQL, "UPDATE `buildings` SET exit_x = '%f', exit_y = '%f', exit_z = '%f', exit_interior = '%d' WHERE id = '%d'", 
					x, y, z, interiorid, Building_GetID(building));

	Building_DestroyElements(building);
	Building_CreateElements(building);
    return 1;
}

stock Building_SetOwnable(Building:building, ownable)
{
	var_set_cell(Variant:building, Ownable, ownable);
    mysql_pquery_f(gMySQL, "UPDATE `buildings` SET ownable = '%d' WHERE id = '%d'", ownable, Building_GetID(building));
    return 1;
}

stock Building_SetPrice(Building:building, price)
{
    var_set_cell(Variant:building, Price, price);
    
	mysql_pquery_f(gMySQL, "UPDATE `buildings` SET price = '%d' WHERE id = '%d'", price, Building_GetID(building));
    return 1;
}

stock Building_SetWelcomeText(Building:building, text[MAX_WELCOME_TEXT_LENGTH])
{
	var_set_cells(Variant:building, WelcomeText, text);

	mysql_pquery_f(gMySQL, "UPDATE `buildings` SET welcome_text = '%e' WHERE id = '%d'", text, Building_GetID(building));
	return 1;
}

stock Building_GetWelcomeText(Building:building, text[MAX_WELCOME_TEXT_LENGTH])
{
    var_get_md_arr(Variant:building, {WelcomeText}, text);
	return 1;
}

stock String:Building_GetWelcomeTextStr(Building:building)
{
	new text[MAX_WELCOME_TEXT_LENGTH];
	Building_GetWelcomeText(building, text);
	return str_new(text);
}

stock Building_SetOwner(Building:building, playerid)
{
    if(!Building_IsOwnable(building) || !Character_IsLogged(playerid))
	   return 0;

	Building_ResetOwner(building);

	var_set_cell(Variant:building, OwnerID, Character_GetID(playerid));
	var_set_cells(Variant:building, OwnerName, Character_GetOOCName(playerid));

	Character_AddOwnedBuilding(playerid, building);

	mysql_pquery_f(gMySQL, "UPDATE `buildings` SET owner_id = '%d' WHERE id = '%d'", Character_GetID(playerid), Building_GetID(building));

    Log(Character_GetOOCName(playerid), "", "Building_SetOwner", Building_GetID(building));
    return 1;
}

stock Building_ResetOwner(Building:building)
{
	mysql_pquery_f(gMySQL, "UPDATE `buildings` SET owner_id = '0' WHERE id = '%d'", Building_GetID(building));
	
	foreach(new i : Player)
	{
		if(!Character_DoesOwnBuilding(i, building))
			continue;
		Character_RemoveOwnedBuilding(i, building);
	}

    var_set_cell(Variant:building, OwnerID, 0);
	var_set_cells(Variant:building, OwnerName, "");

    Log("", "", "Building_ResetOwner", Building_GetID(building));
    return 1;
}

stock Building_Delete(Building:building)
{
    new query[64];
    mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `buildings` WHERE id = '%d'", Building_GetID(building));
    mysql_tquery(gMySQL, query);

	Building_DestroyElements(building);
	
	// UPDATE OWNERID -> BUILDING_ID TO 0.
    CallLocalFunction(#OnBuildingDeleted, "d", _:building);
	
	var_release(Variant:building);
	var_delete(Variant:building);

	map_remove(Buildings, Building_GetID(building));
    return 1;
}

stock Building_CreateElements(Building:building)
{
	new Float:enterX, Float:enterY, Float:enterZ, Float:exitX, Float:exitY, Float:exitZ,
		name[MAX_BUILDING_NAME];
	
	Building_GetName(building, name);
	Building_GetEnterPos(building, enterX, enterY, enterZ);
	Building_GetExitPos(building, exitX, exitY, exitZ);

	new enterPickup = Pickup_Create(1239, building, enterX, enterY, enterZ, ELEMENT_TYPE_BUILDING_ENTRANCE, Building_GetEnterWorld(building), Building_GetEnterInterior(building));
	var_set_cell(Variant:building, EnterPickupID, enterPickup);

    new Text3D:enterText = CreateDynamic3DTextLabel(name, COLOR_LIGHTBLUE/*2*/, enterX, enterY, enterZ + 0.55, 20, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, Building_GetEnterWorld(building));
	var_set_cell(Variant:building, Enter3DText, enterText);

    new exitPickup = Pickup_Create(1007, building, exitX, exitY, exitZ, ELEMENT_TYPE_BUILDING_EXIT, Building_GetExitWorld(building), Building_GetExitInterior(building));
	var_set_cell(Variant:building, ExitPickupID, exitPickup);

	new Text3D:exitText = CreateDynamic3DTextLabel("Uscita", COLOR_LIGHTBLUE/*2*/, exitX, exitY, exitZ + 0.55, 20, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, Building_GetExitWorld(building), Building_GetExitInterior(building));
	var_set_cell(Variant:building, Exit3DText, exitText);
}

stock Building_DestroyElements(Building:building)
{
    Pickup_Destroy(var_get(Variant:building, EnterPickupID));
    Pickup_Destroy(var_get(Variant:building, ExitPickupID));
	new Text3D:t = Text3D:var_get(Variant:building, Enter3DText);
    DestroyDynamic3DTextLabelEx(t);
	t = Text3D:var_get(Variant:building, Exit3DText);
	DestroyDynamic3DTextLabelEx(t);
	return 1;
}

// GET/SET

stock bool:Building_IsValid(Building:building) return var_valid(Variant:building);

stock Building_GetName(Building:building, name[MAX_BUILDING_NAME])
{
	var_get_md_arr(Variant:building, {Name}, name);
    return 1;
}

stock bool:Building_IsOwnable(Building:building)
{
    return bool:var_get(Variant:building, Ownable);
}

stock bool:Building_IsOwned(Building:building)
{
    return Building_IsOwnable(building) && Building_GetOwnerID(building) > 0;
}

stock Building_GetOwnerID(Building:building)
{
    return var_get(Variant:building, OwnerID);
}

stock Building_GetOwnerName(Building:building, name[MAX_PLAYER_NAME])
{
    var_get_md_arr(Variant:building, {OwnerName}, name);
    return 1;
}

stock Building_IsLocked(Building:building)
{
    return var_get(Variant:building, IsLocked);
}

stock Building_GetPrice(Building:building)
{
    return var_get(Variant:building, Price);
}

stock Building_SetFaction(Building:building, factionid)
{
	var_set_cell(Variant:building, Faction, factionid);

	mysql_pquery_f(gMySQL, "UPDATE `buildings` SET faction = '%d' WHERE id = '%d'", factionid, Building_GetID(building));
	return 1;
}

stock Building_GetFaction(Building:building)
{
	return var_get(Variant:building, Faction);
}

stock Building_GetEnterInterior(Building:building)
{
	return var_get(Variant:building, EnterInterior);
}

stock Building_GetEnterWorld(Building:building)
{
	return var_get(Variant:building, EnterWorld);
}

stock Building_GetExitInterior(Building:building)
{
	return var_get(Variant:building, ExitInterior);
}

stock Building_GetExitWorld(Building:building)
{
	return var_get(Variant:building, ExitWorld);
}

stock Building_GetEnterPos(Building:building, &Float:x, &Float:y, &Float:z)
{
	x = Float:var_get(Variant:building, EnterX);
	y = Float:var_get(Variant:building, EnterY);
	z = Float:var_get(Variant:building, EnterZ);
	return 1;
}

stock Building_GetExitPos(Building:building, &Float:x, &Float:y, &Float:z)
{
	x = Float:var_get(Variant:building, ExitX);
	y = Float:var_get(Variant:building, ExitY);
	z = Float:var_get(Variant:building, ExitZ);
	return 1;
}

stock Building_GetType(Building:building)
{
	return var_get(Variant:building, Type);
}

stock bool:Building_SetType(Building:building, type)
{
	if(type < 0 || type >= BUILDING_TYPE_LAST)
		return false;
	new lastType = Building_GetType(building);

	if(lastType == type)
		return true;

	var_set_cell(Variant:building, Type, type);

	new currentIndex = list_find(BuildingsByType[lastType], building);

	if(currentIndex != -1)
	{
		list_remove(BuildingsByType[lastType], currentIndex);
	}
	list_add(BuildingsByType[type], building);
	
	mysql_pquery_f(gMySQL, "UPDATE `buildings` SET type = '%d' WHERE id = '%d'", type, Building_GetID(building));
	return true;
}

stock Building_GetID(Building:building)
{
	return var_get(Variant:building, ID);
}

stock Building_SetLocked(Building:building, locked)
{
	var_set_cell(Variant:building, IsLocked, locked);

	mysql_pquery_f(gMySQL, "UPDATE `buildings` SET locked = '%d' WHERE id = '%d'", 
					locked, Building_GetID(building));
	return 1;
}

stock Building_GetMoney(Building:building)
{
	return var_get(Variant:building, Money);
}

stock Building_GiveMoney(Building:building, amount)
{
	return Building_SetMoney(building, var_get(Variant:building, Money) + amount);
}

stock Building_SetMoney(Building:building, amount)
{
	var_set_cell(Variant:building, Money, amount);
	mysql_pquery_f(gMySQL, "UPDATE `buildings` SET money = '%d' WHERE id = '%d'", 
					amount, Building_GetID(building));
	return 1;
}

stock Building_GetAddress(Building:building)
{
    new zone[64], Float:x, Float:y, Float:z;
    Building_GetEnterPos(building, x, y, z);
    Get2DZoneName(x, y, z, zone, sizeof(zone));
	format(zone, sizeof(zone), "%d %s, Las Venturas, San Andreas", Building_GetID(building), zone);
    return zone;
}

stock String:Building_GetAddressStr(Building:building)
{
    new String:string, zone[64], Float:x, Float:y, Float:z;
    Building_GetEnterPos(building, x, y, z);
    Get2DZoneName(x, y, z, zone, sizeof(zone));
	string = str_format("%d %s, Las Venturas, San Andreas", Building_GetID(building), zone);
    return string;
}

hook OnCharacterTryAccept(playerid, type)
{
    if(type != REQUEST_TYPE_BUILDING)
		return 0;
	
	new senderid = Request_GetSender(playerid, REQUEST_TYPE_BUILDING);
    if(!IsPlayerInRangeOfPlayer(playerid, senderid, 5.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore."), 0;
	
	new
		Building:building = Building:Request_GetItem(playerid, REQUEST_TYPE_BUILDING),
        price = Request_GetAmount(playerid, REQUEST_TYPE_BUILDING)
    ;

	if(!Building_IsValid(building))
	{
		Request_Reset(playerid, REQUEST_TYPE_BUILDING);
		Request_Reset(senderid, REQUEST_TYPE_BUILDING);
		return 0;
	}

	if(Building_GetOwnerID(building) != Character_GetID(senderid))
	{
		SendClientMessage(playerid, COLOR_ERROR, "Il giocatore che ti ha inviato la richiesta non è più proprietario dell'edificio.");
		Request_Reset(playerid, REQUEST_TYPE_BUILDING);
		Request_Reset(senderid, REQUEST_TYPE_BUILDING);
		return 0;
	}

	if(!Character_CanOwnBuilding(playerid, building))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi acquistare altri business."), 0;

    Character_GiveMoney(playerid, -price, "accept_building");
    Character_GiveMoney(senderid, price, "sell_building_to_player");
    
	Building_SetOwner(building, playerid);

    SendFormattedMessage(senderid, COLOR_ACCEPT_REQUEST, "%s ha accettato la tua richiesta di vendita del tuo edificio. Hai guadagnato $%d.", Character_GetRolePlayName(playerid), price);
    SendFormattedMessage(playerid, COLOR_ACCEPT_REQUEST, "%s ti ha venduto il suo edificio per $%d.", Character_GetRolePlayName(senderid), price);

    Request_Reset(playerid, REQUEST_TYPE_BUILDING);
    Request_Reset(senderid, REQUEST_TYPE_BUILDING);
	return 1;
}

stock Character_GetNearBuildingID(playerid)
{
	if(Character_GetVirtualWorld(playerid) < BUILDING_START_WORLD)
		return -1;
	return Character_GetVirtualWorld(playerid) - BUILDING_START_WORLD;
}

stock Building:Character_GetCurrentBuilding(playerid)
{
	new Building:building = Character_GetBuilding(playerid, false);
	if(Building_IsValid(building))
	{
		return building;
	}
	building = Character_GetBuilding(playerid, true);
	if(Building_IsValid(building))
	{
		new Float:x, Float:y, Float:z, Float:ex, Float:ey, Float:ez;
		Building_GetEnterPos(building, x, y, z);
		Building_GetExitPos(building, ex, ey, ez);
		if(IsPlayerInRangeOfPoint(playerid, 1.5, x, y, z) || IsPlayerInRangeOfPoint(playerid, 1.5, ex, ey, ez))
			return building;
	}
	return Building:0;
}

stock Building:Character_GetBuilding(playerid, bool:inside)
{
	new Building:building = INVALID_BUILDING;
	if(inside)
	{
		building = Building_Find(Character_GetNearBuildingID(playerid));
	}
	else
	{
		new Building:temp, E_ELEMENT_TYPE:type;
		if(Character_GetNearestPickup(playerid, temp, type) && (type == ELEMENT_TYPE_BUILDING_ENTRANCE || type == ELEMENT_TYPE_BUILDING_EXIT))
			building = temp;
	}
	return building;
}

stock bool:Character_CanUseBuildingMenu(playerid, Building:building, minRank = 3)
{
	if(!Building_IsValid(building))
		return false;
	if(Player_IsAdminDuty(playerid) || Character_DoesOwnBuilding(playerid, building))
		return true;	
	if(Faction_IsValid(Character_GetFaction(playerid)) && Character_GetFaction(playerid) == Building_GetFaction(building) && Character_GetRank(playerid) <= minRank)
		return true;
	return false;
}

stock Building:Character_GetBusinessMenu(playerid)
{
	new Building:building = Character_GetCurrentBuilding(playerid);
	if(Character_CanUseBuildingMenu(playerid, building))
		return building;
	return INVALID_BUILDING;
}

flags:businesscmds(CMD_ALIVE_USER);
CMD:businesscmds(playerid, params[])
{
	SendClientMessage(playerid, -1, "[Business] /biz o /bmenu"); 
}
alias:businesscmds("bizcmds");

hook function Character_Enter(playerid)
{   
	printf("[Character_Enter]: Building");
	if(IsPlayerInAnyVehicle(playerid))
		return continue(playerid);
	new Building:building, E_ELEMENT_TYPE:type;
    if(Character_GetNearestPickup(playerid, building, type) && Building_IsValid(building))
    {
		if(Building_IsLocked(building))
			return GameTextForPlayer(playerid, "~r~Porta Chiusa", 5000, 1), 0;
		new 
			Float:x = 0.0, 
			Float:y = 0.0, 
			Float:z = 0.0,
			interiorId,
			world
		;

		if(type == ELEMENT_TYPE_BUILDING_ENTRANCE)
		{
			new String:str = Building_GetWelcomeTextStr(building);
			if(str_len(str) > 0)
				SendClientMessageStr(playerid, COLOR_GREEN, str);
			Building_GetExitPos(building, x, y, z);
			interiorId = Building_GetExitInterior(building);
			world = Building_GetExitWorld(building);
			
			CallLocalFunction(#OnPlayerEnterBuilding, "dd", playerid, _:building);
		}
		else if(type == ELEMENT_TYPE_BUILDING_EXIT)
		{
			Building_GetEnterPos(building, x, y, z);
			interiorId = Building_GetEnterInterior(building);
			world = Building_GetEnterWorld(building);

			CallLocalFunction(#OnPlayerExitBuilding, "dd", playerid, _:building);
		}
		if(x != 0.0 && y != 0.0 && z != 0.0)
		{
			// Should I use Fading?
			Character_SetSafePosition(playerid, x, y, z, interiorId, world);
		}
		return 1;
    }
    return continue(playerid);
}

stock Character_SetEditingBuilding(playerid, Building:building)
{
	CurrentEditing[playerid] = building;
	return 1;
}

stock Building:Character_GetEditingBuilding(playerid)
{
	return CurrentEditing[playerid];
}

SSCANF:building(string[])
{
	new ret = strval(string);
	new Building:building = INVALID_BUILDING;
	if(ret > 0)
		building = Building_Find(ret);
	return _:building;
}