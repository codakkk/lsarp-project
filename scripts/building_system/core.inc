
#if defined _included_BUILDING
    #endinput
#endif 

#define _included_BUILDING

enum //E_BUILDING_TYPE
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

#include <pickup_system\core>
#include <faction_system\core>
#include <player_system\core>

#include <building_system\components\inventory>
#include <building_system\components\garage>
#include <building_system\components\shop>
#include <building_system\components\ads>
#include <building_system\components\gas_station>

#include <building_system\commands\admin>

#include <YSI_Coding\y_hooks>


forward OnBuildingLoaded(buildingid);
forward OnBuildingDeleted(buildingid);

static enum _:E_BUILDING_INFO
{
    ID,
    Exists,
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
    Locked,
	Faction,
	Type,
	Money,
    // Volatile
    EnterPickupID,
    Text3D:Enter3DText,
    ExitPickupID,
	Text3D:Exit3DText,
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
	for_map(i : Buildings)
	{
        new buildingid = iter_get_key(i);
		if(Building_GetOwnerID(buildingid) == Character_GetID(playerid))
		{
			new Iter:iter = Iter:map_iter_at(Buildings, buildingid);
			iter_set_cells(iter, OwnerName, newCharacterName, MAX_PLAYER_NAME);
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
		data[ID] = id;
		set(data[Name], "SET_NAME");
		set(data[WelcomeText], "");
		set(data[OwnerName], "");
		
		data[EnterX] = x;
		data[EnterY] = y;
		data[EnterZ] = z;
		data[EnterInterior] = interior;
		data[EnterWorld] = world;
		data[Ownable] = 0;
		data[OwnerID] = 0;
		data[Price] = 0;
		data[Locked] = 1;
		data[Exists] = 1;

		map_add_arr(Buildings, id, data);

		Building_CreateElements(id);

		CallLocalFunction(#OnBuildingLoaded, "d", id);
    }
    MySQL_TQueryInline(gMySQL, using inline OnInsert, "INSERT INTO buildings (name, welcome_text, enter_x, enter_y, enter_z, enter_interior, enter_world, exit_x, exit_y, exit_z, exit_interior, ownable, owner_id, price, locked) \
						   VALUES('SET_NAME', '', '%f', '%f', '%f', '%d', '%d', '0.0', '0.0', '0.0', '0', '0', '0', '0', '1')", 
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
			cache_get_value_name_int(i, "locked", data[Locked]);
			cache_get_value_name_int(i, "faction", data[Faction]);
			cache_get_value_name_int(i, "type", data[Type]);

			cache_get_value_name_int(i, "money", data[Money]);

			data[ExitWorld] = BUILDING_START_WORLD + data[ID];

			data[Exists] = 1;



			printf("Building ID: %d - %d", data[ID], i);
			map_add_arr(Buildings, data[ID], data);

			Building_CreateElements(data[ID]);

			CallLocalFunction(#OnBuildingLoaded, "d", data[ID]);
			//printf("Type: %d", BuildingInfo[i][Type]);
			//Iter_Add(Buildings, i);
		}
		printf("%d buildings loaded.\n", count);

		for_map(i : Buildings)
		{
			// DON'T USE CACHE FUNCTIONS AFTER THIS IF
			new buildingid = iter_get_key(i);
			if(Building_GetOwnerID(buildingid) > 0)
			{
				new Cache:cache = mysql_query_f(gMySQL, true, "SELECT name FROM `characters` WHERE id = '%d'", Building_GetOwnerID(buildingid));
				if(cache_num_rows() > 0)
				{
					new name[MAX_PLAYER_NAME];
					cache_get_value_name(0, "name", name);

					iter_set_cells(i, OwnerName, name);
				}
				cache_delete(cache);
			}
		}
	}

	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `buildings`");
	return 1;
}

stock Building_SetPosition(buildingid, Float:x, Float:y, Float:z, vw, int)
{
    if(!Building_IsValid(buildingid))
	   return 0;
	
	map_set_cell(Buildings, buildingid, EnterX, x);
	map_set_cell(Buildings, buildingid, EnterY, y);
	map_set_cell(Buildings, buildingid, EnterZ, z);
	map_set_cell(Buildings, buildingid, EnterWorld, vw);
	map_set_cell(Buildings, buildingid, EnterInterior, int);
    
	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET enter_x = '%f', enter_y = '%f', enter_z = '%f', enter_interior = '%d', enter_world = '%d' WHERE id = '%d'", 
					x, y, z, int, vw, Building_GetID(buildingid));

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

	iter_set_cells(iter, Name, name);
    
	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET name = '%e' WHERE id = '%d'", 
					name, Building_GetID(buildingid));

    Building_DestroyElements(buildingid);
	Building_CreateElements(buildingid);
    return 1;
}

stock Building_SetInterior(buildingid, Float:x, Float:y, Float:z, interiorid)
{
    if(!Building_IsValid(buildingid))
	   return 0;

	map_set_cell(Buildings, buildingid, ExitX, x);
	map_set_cell(Buildings, buildingid, ExitY, y);
	map_set_cell(Buildings, buildingid, ExitZ, z);
	map_set_cell(Buildings, buildingid, ExitInterior, interiorid);
	map_set_cell(Buildings, buildingid, ExitWorld, BUILDING_START_WORLD + buildingid);

    mysql_tquery_f(gMySQL, "UPDATE `buildings` SET exit_x = '%f', exit_y = '%f', exit_z = '%f', exit_interior = '%d' WHERE id = '%d'", 
					x, y, z, interiorid, Building_GetID(buildingid));

	Building_DestroyElements(buildingid);
	Building_CreateElements(buildingid);
    return 1;
}

stock Building_SetOwnable(buildingid, ownable)
{
    if(!Building_IsValid(buildingid))
	   return 0;
	map_set_cell(Buildings, buildingid, Ownable, ownable);
    mysql_tquery_f(gMySQL, "UPDATE `buildings` SET ownable = '%d' WHERE id = '%d'", 
					ownable, Building_GetID(buildingid));
    return 1;
}

stock Building_SetPrice(buildingid, price)
{
    if(!Building_IsValid(buildingid))
	   return 0;
    map_set_cell(Buildings, buildingid, Price, price);
    
	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET price = '%d' WHERE id = '%d'", 
					price, Building_GetID(buildingid));
    return 1;
}

stock Building_SetWelcomeText(buildingid, text[MAX_WELCOME_TEXT_LENGTH])
{
	if(!Building_IsValid(buildingid))
	   return 0;
	new Iter:iter = Iter:map_iter_at(Buildings, buildingid);
	iter_set_cells(iter, WelcomeText, text);

	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET welcome_text = '%e' WHERE id = '%d'", 
					text, Building_GetID(buildingid));
	return 1;
}

stock Building_GetWelcomeText(buildingid, text[MAX_WELCOME_TEXT_LENGTH])
{
	if(!Building_IsValid(buildingid))
	   return 0;
	new Iter:iter = map_iter_at(Buildings, buildingid);
    iter_get_md_arr(iter, {WelcomeText}, text);
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
	map_set_cell(Buildings, buildingid, OwnerID, Character_GetID(playerid));
	new Iter:iter = Iter:map_iter_at(Buildings, buildingid);
	iter_set_cells(iter, OwnerName, Character_GetOOCName(playerid));

	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET owner_id = '%d' WHERE id = '%d'", Character_GetID(playerid), Building_GetID(buildingid));

	Character_AddOwnedBuilding(playerid, buildingid);

    Log(Character_GetOOCName(playerid), "", "Building_SetOwner", buildingid);
    return 1;
}

stock Building_ResetOwner(buildingid)
{
    if(!Building_IsValid(buildingid))
	   return 0;
    map_set_cell(Buildings, buildingid, OwnerID, 0);
	new Iter:iter = Iter:map_iter_at(Buildings, buildingid);
	iter_set_cells(iter, OwnerName, "");

	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET owner_id = '0' WHERE id = '%d'", Building_GetID(buildingid));
	
	foreach(new i : Player)
	{
		if(!Character_DoesOwnBuilding(i, buildingid))
			continue;
		Character_RemoveOwnedBuilding(i, buildingid);
	}

	/*foreach(new i : Player)
	{
		if(!Character_IsLogged(i) || !Character_DoesOwnBuilding(playerid, buildingid))
			continue;
		SendFormattedMessage(i, COLOR_ORANGE, "Il business (ID %d) di cui eri proprietario non ti appartiene piu'.", buildingid);
		Character_RemoveOwnedBuilding(playerid, buildingid);
	}*/

    Log("", "", "Building_ResetOwner", buildingid);
    return 1;
}

stock Building_Delete(buildingid)
{
	if(!map_has_key(Buildings, buildingid))
		return 0;
	
    new query[64];
    mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `buildings` WHERE id = '%d'", Building_GetID(buildingid));
    mysql_tquery(gMySQL, query);

	Building_DestroyElements(buildingid);
	
	// UPDATE OWNERID -> BUILDING_ID TO 0.
    CallLocalFunction(#OnBuildingDeleted, "d", buildingid);

	map_remove_deep(Buildings, buildingid);
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
	map_set_cell(Buildings, buildingid, EnterPickupID, enterPickup);

    new Text3D:enterText = CreateDynamic3DTextLabel(name, COLOR_LIGHTBLUE/*2*/, enterX, enterY, enterZ + 0.55, 20, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, Building_GetEnterWorld(buildingid));
	map_set_cell(Buildings, buildingid, Enter3DText, enterText);

    new exitPickup = Pickup_Create(1007, buildingid, exitX, exitY, exitZ, ELEMENT_TYPE_BUILDING_EXIT, Building_GetExitWorld(buildingid), Building_GetExitInterior(buildingid));
	map_set_cell(Buildings, buildingid, ExitPickupID, exitPickup);

	new Text3D:exitText = CreateDynamic3DTextLabel("Uscita", COLOR_LIGHTBLUE/*2*/, exitX, exitY, exitZ + 0.55, 20, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, Building_GetExitWorld(buildingid), Building_GetExitInterior(buildingid));
	map_set_cell(Buildings, buildingid, Exit3DText, exitText);

}

stock Building_DestroyElements(buildingid)
{
	if(!Building_IsValid(buildingid))
		return 0;
    Pickup_Destroy(map_get(Buildings, buildingid, EnterPickupID));
    Pickup_Destroy(map_get(Buildings, buildingid, ExitPickupID));
	new Text3D:t = Text3D:map_get(Buildings, buildingid, Enter3DText);
    DestroyDynamic3DTextLabelEx(t);
	t = Text3D:map_get(Buildings, buildingid, Exit3DText);
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
    iter_get_md_arr(iter, {Name}, name);
    return 1;
}

stock Building_IsOwnable(a)
{
	if(!Building_IsValid(a))
		return 0;
    return map_get(Buildings, a, Ownable);
}

stock Building_IsOwned(a)
{
	if(!Building_IsValid(a))
		return 0;
    return Building_IsOwnable(a) && Building_GetOwnerID(a) > 0;
}

stock Building_GetOwnerID(a)
{
	if(!Building_IsValid(a))
		return 0;
    return map_get(Buildings, a, OwnerID);
}

stock Building_GetOwnerName(a, name[MAX_PLAYER_NAME])
{
	if(!Building_IsValid(a))
		return 0;
    new Iter:iter = map_iter_at(Buildings, a);
    iter_get_md_arr(iter, {OwnerName}, name);
    return 1;
}

stock Building_IsLocked(a)
{
	if(!Building_IsValid(a))
		return 0;
    return map_get(Buildings, a, Locked);// || Building_GetType(a) == BUILDING_TYPE_GAS_STATION;
}

stock Building_GetPrice(a)
{
	if(!Building_IsValid(a))
		return 0;
    return map_get(Buildings, a, Price);
}

stock Building_SetFaction(buildingid, factionid)
{
	if(!Building_IsValid(buildingid))
		return 0;
	map_set_cell(Buildings, buildingid, Faction, factionid);

	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET faction = '%d' WHERE id = '%d'", 
					factionid, Building_GetID(buildingid));
	return 1;
}

stock Building_GetFaction(buildingid)
{
	if(!Building_IsValid(buildingid))
		return INVALID_FACTION_ID;
	return map_get(Buildings, buildingid, Faction);
}

stock Building_GetEnterInterior(buildingid)
{
	if(!Building_IsValid(buildingid))
		return 0;
	return map_get(Buildings, buildingid, EnterInterior);
}

stock Building_GetEnterWorld(buildingid)
{
	if(!Building_IsValid(buildingid))
		return 0;
	return map_get(Buildings, buildingid, EnterWorld);
}

stock Building_GetExitInterior(buildingid)
{
	if(!Building_IsValid(buildingid))
		return 0;
	return map_get(Buildings, buildingid, ExitInterior);
}

stock Building_GetExitWorld(buildingid)
{
	if(!Building_IsValid(buildingid))
		return 0;
	return map_get(Buildings, buildingid, ExitWorld);
}

stock Building_GetEnterPos(buildingid, &Float:x, &Float:y, &Float:z)
{
	if(!Building_IsValid(buildingid))
		return 0;
	x = Float:map_get(Buildings, buildingid, EnterX);
	y = Float:map_get(Buildings, buildingid, EnterY);
	z = Float:map_get(Buildings, buildingid, EnterZ);
	return 1;
}

stock Building_GetExitPos(buildingid, &Float:x, &Float:y, &Float:z)
{
	if(!Building_IsValid(buildingid))
		return 0;
	x = Float:map_get(Buildings, buildingid, ExitX);
	y = Float:map_get(Buildings, buildingid, ExitY);
	z = Float:map_get(Buildings, buildingid, ExitZ);
	return 1;
}

stock Building_GetType(buildingid)
{
	if(!Building_IsValid(buildingid))
		return -1;
	return map_get(Buildings, buildingid, Type);
}

stock Building_SetType(buildingid, type)
{
	if(!Building_IsValid(buildingid) || type < 0 || type >= BUILDING_TYPE_LAST)
		return 0;
	map_set_cell(Buildings, buildingid, Type, type);

	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET type = '%d' WHERE id = '%d'", 
					type, Building_GetID(buildingid));
	return 1;
}

stock Building_GetID(buildingid)
{
	if(!Building_IsValid(buildingid))
		return 0;
	return map_get(Buildings, buildingid, ID);
}

stock Building_SetLocked(buildingid, locked)
{
	if(!Building_IsValid(buildingid))
		return 0;
	map_set_cell(Buildings, buildingid, Locked, locked);

	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET locked = '%d' WHERE id = '%d'", 
					locked, Building_GetID(buildingid));
	return 1;
}

stock Building_GetMoney(buildingid)
{
	if(!Building_IsValid(buildingid))
		return 0;
	return map_get(Buildings, buildingid, Money);
}

stock Building_GiveMoney(buildingid, amount)
{
	return Building_SetMoney(buildingid, map_get(Buildings, buildingid, Money) + amount);
}

stock Building_SetMoney(buildingid, amount)
{
	if(!Building_IsValid(buildingid))
		return 0;
	map_set_cell(Buildings, buildingid, Money, amount);
	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET money = '%d' WHERE id = '%d'", 
					amount, Building_GetID(buildingid));
	return 1;
}

stock Building_GetAddress(buildingid)
{
    new zone[32], Float:x, Float:y, Float:z;
    Building_GetEnterPos(buildingid, x, y, z);
    Get2DZoneName(x, y, z, zone, sizeof(zone));
    return zone;
}

stock Character_AcceptBuilding(playerid)
{
	if(!Request_IsPending(playerid, REQUEST_TYPE_BUILDING))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai una richiesta di riparazione attiva."), 0;
	
    if(!IsPlayerInRangeOfPlayer(playerid, Request_GetSender(playerid, REQUEST_TYPE_BUILDING), 5.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore."), 0;
	
	new senderid = Request_GetSender(playerid, REQUEST_TYPE_BUILDING),
		buildingid = Request_GetItem(playerid, REQUEST_TYPE_BUILDING),
        price = Request_GetAmount(playerid, REQUEST_TYPE_BUILDING)
    ;

	if(buildingid == -1)
	{
		Request_Reset(playerid, REQUEST_TYPE_BUILDING);
		Request_Reset(senderid, REQUEST_TYPE_BUILDING);
		return 0;
	}

	if(Building_GetOwnerID(buildingid) != Character_GetID(senderid))
	{
		SendClientMessage(playerid, COLOR_ERROR, "Il giocatore che ti ha inviato la richiesta non è più proprietario dell'edificio.");
		Request_Reset(playerid, REQUEST_TYPE_BUILDING);
		Request_Reset(senderid, REQUEST_TYPE_BUILDING);
		return 0;
	}

    Character_GiveMoney(playerid, -price, "accept_building");
    Character_GiveMoney(senderid, price, "sell_building_to_player");
    
	Building_SetOwner(buildingid, playerid);

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

stock Character_GetNearBuilding(playerid, bool:inside)
{
	if(inside)
	{
		/*foreach(new b : Buildings)
		{
			if(BuildingInfo[b][Exists] && Building_GetExitWorld(b) == Character_GetVirtualWorld(playerid))
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

stock Character_GetNearBuildingIDMenu(playerid, minRank = 3)
{
	new buildingid = Character_GetNearBuilding(playerid, true);
	if(!Building_IsValid(buildingid))
		buildingid = Character_GetNearBuilding(playerid, false);
	if(Building_IsValid(buildingid))
	{
		if(Player_IsAdminDuty(playerid) || Character_DoesOwnBuilding(playerid, buildingid))
			return buildingid;
		if(Faction_IsValid(Character_GetFaction(playerid)) && Character_GetFaction(playerid) == Building_GetFaction(buildingid) && Character_GetRank(playerid) <= minRank)
			return buildingid;
	}
	return -1;
}

stock Character_ShowBizDialog(playerid)
{
	new buildingid = Character_GetNearBuildingIDMenu(playerid);
	if(Building_IsValid(buildingid) && Building_GetOwnerID(buildingid) == Character_GetID(playerid))
	{
		return Dialog_Show(playerid, Dialog_Building, DIALOG_STYLE_LIST, "Edificio", "Apri/Chiudi Porta\nInventario\nDeposita Soldi\nRitira Soldi\nVendi\nVendi a Giocatore\nGestisci Negozio", "Continua", "Chiudi");
	}
	return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata o all'interno di un tuo edificio."), 0;
}

flags:biz(CMD_ALIVE_USER);
CMD:biz(playerid, params[])
{
	return Character_ShowBizDialog(playerid);
}
alias:biz("edificio", "building");

Dialog:Dialog_Building(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;
	new buildingid = Character_GetNearBuildingIDMenu(playerid);
	if(!Building_IsValid(buildingid) || Building_GetOwnerID(buildingid) != Character_GetID(playerid))
		return 0;
	switch(listitem)
	{
		case 0:
		{
			if(Building_GetType(buildingid) == BUILDING_TYPE_GAS_STATION)
			{
				Building_SetLocked(buildingid, 1);
				return SendClientMessage(playerid, COLOR_ERROR, "Non puoi aprire questo tipo di edificio.");
			}
			Building_SetLocked(buildingid, !Building_IsLocked(buildingid));
			if(Building_IsLocked(buildingid))
				SendClientMessage(playerid, COLOR_GREEN, "Hai chiuso il tuo edificio.");
			else
				SendClientMessage(playerid, COLOR_GREEN, "Hai aperto il tuo edificio.");
			return 1;
		}
		case 1:
		{
			return Building_ShowInventory(buildingid, playerid);
		}
		case 2:
		{
			return Dialog_Show(playerid, Dialog_BuildingDepositMoney, DIALOG_STYLE_INPUT, "Deposita Denaro", "Inserisci l'ammontare di denaro che vuoi depositare all'interno del tuo edificio.\nAttualmente ci sono $%d nella cassa dell'edificio.", "Deposita", "Indietro", Building_GetMoney(buildingid));
		}
		case 3:
		{
			return Dialog_Show(playerid, DialogBuildingWithdrawMoney, DIALOG_STYLE_INPUT, "Ritira Denaro", "Inserisci l'ammontare di denaro che vuoi ritirare dall'interno del tuo edificio.\nAttualmente ci sono $%d nella cassa dell'edificio.", "Ritira", "Indietro", Building_GetMoney(buildingid));
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
		case 6:
		{
			return Character_EditSellableItems(playerid);
		}
	}
	return 1;
}

Dialog:Dialog_BuildingDepositMoney(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Character_ShowBizDialog(playerid);
	
	new buildingid = Character_GetNearBuildingIDMenu(playerid);
	if(!Building_IsValid(buildingid) || Building_GetOwnerID(buildingid) != Character_GetID(playerid))
		return 0;
	
	new money =  strval(inputtext);
	if(money <= 0 || money > Character_GetMoney(playerid))
		return Dialog_Show(playerid, Dialog_BuildingDepositMoney, DIALOG_STYLE_INPUT, "Deposita Denaro", "{FF0000}Non disponi della quantità di denaro inserita.\n{FFFFFF}Inserisci l'ammontare di denaro che vuoi depositare all'interno del tuo edificio.\nAttualmente ci sono $%d nella cassa dell'edificio.", "Deposita", "Indietro", Building_GetMoney(buildingid));
	
	Building_GiveMoney(buildingid, money);
	Character_GiveMoney(playerid, -money, "building_deposit");

	SendFormattedMessage(playerid, -1, "Hai depositato $%d all'interno dell'edificio. Nella cassa dell'edificio ci sono $%d.", money, Building_GetMoney(buildingid));

	return 1;
}

Dialog:DialogBuildingWithdrawMoney(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Character_ShowBizDialog(playerid);
	
	new buildingid = Character_GetNearBuildingIDMenu(playerid);
	if(!Building_IsValid(buildingid) || Building_GetOwnerID(buildingid) != Character_GetID(playerid))
		return 0;
	
	new money = strval(inputtext);
	if(money <= 0 || money > Building_GetMoney(buildingid))
		return Dialog_Show(playerid, DialogBuildingWithdrawMoney, DIALOG_STYLE_INPUT, "Ritira Denaro", "{FF0000}La cassa non dispone della quantità di denaro inserita.\n{FFFFFF}Inserisci l'ammontare di denaro che vuoi ritirare dalla cassa del tuo edificio.\nAttualmente ci sono $%d nella cassa dell'edificio.", "Ritira", "Indietro", Building_GetMoney(buildingid));
	
	Building_GiveMoney(buildingid, -money);
	Character_GiveMoney(playerid, money, "building_withdraw");

	SendFormattedMessage(playerid, -1, "Hai ritirato $%d dall'interno dell'edificio. Nella cassa dell'edificio ci sono $%d.", money, Building_GetMoney(buildingid));

	return 1;
}

Dialog:Dialog_BuildingSell(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Character_ShowBizDialog(playerid);
	
	new buildingid = Character_GetNearBuildingIDMenu(playerid);

	if(!Building_IsValid(buildingid) || Building_GetOwnerID(buildingid) != Character_GetID(playerid))
		return 0;
	
	new price = Building_GetPrice(buildingid)/2,
		name[MAX_BUILDING_NAME];
	
	Building_GetName(buildingid, name);

	Character_GiveMoney(playerid, price, "building_sell");

	SendFormattedMessage(playerid, -1, "Hai venduto il tuo edificio (%s) per $%d.", name, price);

	Building_ResetOwner(buildingid);
	return 1;
}

Dialog:Dialog_BuildingSellToPlayer(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Character_ShowBizDialog(playerid);
	
	new buildingid = Character_GetNearBuildingIDMenu(playerid);

	if(!Building_IsValid(buildingid) || Building_GetOwnerID(buildingid) != Character_GetID(playerid))
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

	if(id == playerid)
		return Dialog_Show(playerid, Dialog_BuildingSellToPlayer, DIALOG_STYLE_INPUT, "Vendita Edificio a Giocatore", "{FF0000}Non puoi vendere l'edificio a te stesso.\nInserisci l'id, il nome o il codice (#XXXXX) del giocatore a cui vuoi vendere l'edificio, seguito dal prezzo.\nEsempi:\n> 1 50010\n> Pinco_Pallino 50000\n> #0001 50000", "Vendi", "Indietro");

	//if(Character_HasBuildingKey(id))
		//return Dialog_Show(playerid, Dialog_BuildingSellToPlayer, DIALOG_STYLE_INPUT, "Vendita Edificio a Giocatore", "{FF0000}Il giocatore possiede già un edificio.\nInserisci l'id, il nome o il codice (#XXXXX) del giocatore a cui vuoi vendere l'edificio, seguito dal prezzo.\nEsempi:\n> 1 50010\n> Pinco_Pallino 50000\n> #0001 50000", "Vendi", "Indietro");

	Character_SetRequest(playerid, id, REQUEST_TYPE_BUILDING, buildingid, money);
	SendFormattedMessage(id, COLOR_RECEIVE_REQUEST, "%s vuole venderti il suo edificio per $%d.", Character_GetRolePlayName(playerid), money);
	SendFormattedMessage(playerid, COLOR_SEND_REQUEST, "Hai proposto il tuo edificio a %s per $%d.", Character_GetRolePlayName(id), money);
	return 1;
}