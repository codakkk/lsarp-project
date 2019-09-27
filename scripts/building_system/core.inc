
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
#define INVALID_BUILDING			(Building:0)

#define for_building(%0) for(new Building:%0=Building:map_iter(GetBuildingsMap());iter_inside(Iter:%0);iter_move_next(Iter:%0))

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


forward OnBuildingLoaded(Building:building);
forward OnBuildingDeleted(Building:building);
forward OnPlayerEnterBuilding(playerid, Building:building);

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

static Map:Buildings;

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
	for_building(building)
	{
		if(Building_GetOwnerID(building) == Character_GetID(playerid))
		{
			iter_set_cells(Iter:building, OwnerName, newCharacterName, MAX_PLAYER_NAME);
		}
	}
	return 1;
}

stock Building:Building_Find(buildingid)
{
	return Building:map_iter_at(Buildings, buildingid);
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

		new Building:building = Building_Find(id);
		Building_CreateElements(building);

		CallLocalFunction(#OnBuildingLoaded, "d", _:building);
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
			
			new Building:building = Building_Find(data[ID]);

			Building_CreateElements(building);
			CallLocalFunction(#OnBuildingLoaded, "d", _:building);
			//printf("Type: %d", BuildingInfo[i][Type]);
			//Iter_Add(Buildings, i);
		}
		printf("%d buildings loaded.\n", count);

		new ownerid;
		for_building(building)
		{
			// DON'T USE CACHE FUNCTIONS AFTER THIS IF
			ownerid = Building_GetOwnerID(building);
			if(ownerid > 0)
			{
				inline OnLoadOwnerName()
				{
					if(cache_num_rows() <= 0)
						return 0;
					new name[MAX_PLAYER_NAME];
					cache_get_value_name(0, "name", name);
					iter_set_cells(Iter:building, OwnerName, name);
				}
				MySQL_TQueryInline(gMySQL, using inline OnLoadOwnerName, "SELECT name FROM `characters` WHERE id = '%d'", ownerid);
			}
		}
	}

	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `buildings`");
	return 1;
}

stock Building_SetPosition(Building:building, Float:x, Float:y, Float:z, vw, int)
{
	new buildingid = Building_GetID(building);

	iter_set_cell(Iter:building, EnterX, x);
	iter_set_cell(Iter:building, EnterY, y);
	iter_set_cell(Iter:building, EnterZ, z);
	iter_set_cell(Iter:building, EnterWorld, vw);
	iter_set_cell(Iter:building, EnterInterior, int);
    
	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET enter_x = '%f', enter_y = '%f', enter_z = '%f', enter_interior = '%d', enter_world = '%d' WHERE id = '%d'", 
					x, y, z, int, vw, buildingid);

    Building_DestroyElements(building);
    Building_CreateElements(building);
    return 1;
}

stock bool:Building_SetName(Building:building, name[MAX_BUILDING_NAME])
{

	iter_set_cells(Iter:building, Name, name);
    
	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET name = '%e' WHERE id = '%d'", name, Building_GetID(building));

    Building_DestroyElements(building);
	Building_CreateElements(building);
    return true;
}

stock Building_SetInterior(Building:building, Float:x, Float:y, Float:z, interiorid)
{

	iter_set_cell(Iter:building, ExitX, x);
	iter_set_cell(Iter:building, ExitY, y);
	iter_set_cell(Iter:building, ExitZ, z);
	iter_set_cell(Iter:building, ExitInterior, interiorid);
	iter_set_cell(Iter:building, ExitWorld, BUILDING_START_WORLD + Building_GetID(building));

    mysql_tquery_f(gMySQL, "UPDATE `buildings` SET exit_x = '%f', exit_y = '%f', exit_z = '%f', exit_interior = '%d' WHERE id = '%d'", 
					x, y, z, interiorid, Building_GetID(building));

	Building_DestroyElements(building);
	Building_CreateElements(building);
    return 1;
}

stock Building_SetOwnable(Building:building, ownable)
{
	iter_set_cell(Iter:building, Ownable, ownable);
    mysql_tquery_f(gMySQL, "UPDATE `buildings` SET ownable = '%d' WHERE id = '%d'", ownable, Building_GetID(building));
    return 1;
}

stock Building_SetPrice(Building:building, price)
{
    iter_set_cell(Iter:building, Price, price);
    
	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET price = '%d' WHERE id = '%d'", price, Building_GetID(building));
    return 1;
}

stock Building_SetWelcomeText(Building:building, text[MAX_WELCOME_TEXT_LENGTH])
{
	iter_set_cells(Iter:building, WelcomeText, text);

	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET welcome_text = '%e' WHERE id = '%d'", text, Building_GetID(building));
	return 1;
}

stock Building_GetWelcomeText(Building:building, text[MAX_WELCOME_TEXT_LENGTH])
{
    iter_get_md_arr(Iter:building, {WelcomeText}, text);
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

	iter_set_cell(Iter:building, OwnerID, Character_GetID(playerid));
	iter_set_cells(Iter:building, OwnerName, Character_GetOOCName(playerid));

	Character_AddOwnedBuilding(playerid, building);

	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET owner_id = '%d' WHERE id = '%d'", Character_GetID(playerid), Building_GetID(building));

    Log(Character_GetOOCName(playerid), "", "Building_SetOwner", Building_GetID(building));
    return 1;
}

stock Building_ResetOwner(Building:building)
{
    iter_set_cell(Iter:building, OwnerID, 0);
	iter_set_cells(Iter:building, OwnerName, "");

	new buildingid = Building_GetID(building);

	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET owner_id = '0' WHERE id = '%d'", buildingid);
	

	foreach(new i : Player)
	{
		if(!Character_DoesOwnBuilding(i, building))
			continue;
		Character_RemoveOwnedBuilding(i, building);
	}

    Log("", "", "Building_ResetOwner", buildingid);
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

	iter_erase_deep(Iter:building);
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
	iter_set_cell(Iter:building, EnterPickupID, enterPickup);

    new Text3D:enterText = CreateDynamic3DTextLabel(name, COLOR_LIGHTBLUE/*2*/, enterX, enterY, enterZ + 0.55, 20, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, Building_GetEnterWorld(building));
	iter_set_cell(Iter:building, Enter3DText, enterText);

    new exitPickup = Pickup_Create(1007, building, exitX, exitY, exitZ, ELEMENT_TYPE_BUILDING_EXIT, Building_GetExitWorld(building), Building_GetExitInterior(building));
	iter_set_cell(Iter:building, ExitPickupID, exitPickup);

	new Text3D:exitText = CreateDynamic3DTextLabel("Uscita", COLOR_LIGHTBLUE/*2*/, exitX, exitY, exitZ + 0.55, 20, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, Building_GetExitWorld(building), Building_GetExitInterior(building));
	iter_set_cell(Iter:building, Exit3DText, exitText);

}

stock Building_DestroyElements(Building:building)
{
    Pickup_Destroy(iter_get(Iter:building, EnterPickupID));
    Pickup_Destroy(iter_get(Iter:building, ExitPickupID));
	new Text3D:t = Text3D:iter_get(Iter:building, Enter3DText);
    DestroyDynamic3DTextLabelEx(t);
	t = Text3D:iter_get(Iter:building, Exit3DText);
	DestroyDynamic3DTextLabelEx(t);
	return 1;
}

// GET/SET

stock bool:Building_IsValid(Building:building) return iter_inside(Iter:building);

stock Building_GetName(Building:building, name[MAX_BUILDING_NAME])
{
    iter_get_md_arr(Iter:building, {Name}, name);
    return 1;
}

stock Building_IsOwnable(Building:building)
{
    return iter_get(Iter:building, Ownable);
}

stock bool:Building_IsOwned(Building:building)
{
    return Building_IsOwnable(building) && Building_GetOwnerID(building) > 0;
}

stock Building_GetOwnerID(Building:building)
{
    return iter_get(Iter:building, OwnerID);
}

stock Building_GetOwnerName(Building:building, name[MAX_PLAYER_NAME])
{
    iter_get_md_arr(Iter:building, {OwnerName}, name);
    return 1;
}

stock Building_IsLocked(Building:building)
{
    return iter_get(Iter:building, Locked);// || Building_GetType(a) == BUILDING_TYPE_GAS_STATION;
}

stock Building_GetPrice(Building:building)
{
    return iter_get(Iter:building, Price);
}

stock Building_SetFaction(Building:building, factionid)
{
	iter_set_cell(Iter:building, Faction, factionid);

	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET faction = '%d' WHERE id = '%d'", factionid, Building_GetID(building));
	return 1;
}

stock Building_GetFaction(Building:building)
{
	return iter_get(Iter:building, Faction);
}

stock Building_GetEnterInterior(Building:building)
{
	return iter_get(Iter:building, EnterInterior);
}

stock Building_GetEnterWorld(Building:building)
{
	return iter_get(Iter:building, EnterWorld);
}

stock Building_GetExitInterior(Building:building)
{
	return iter_get(Iter:building, ExitInterior);
}

stock Building_GetExitWorld(Building:building)
{
	return iter_get(Iter:building, ExitWorld);
}

stock Building_GetEnterPos(Building:building, &Float:x, &Float:y, &Float:z)
{
	x = Float:iter_get(Iter:building, EnterX);
	y = Float:iter_get(Iter:building, EnterY);
	z = Float:iter_get(Iter:building, EnterZ);
	return 1;
}

stock Building_GetExitPos(Building:building, &Float:x, &Float:y, &Float:z)
{
	x = Float:iter_get(Iter:building, ExitX);
	y = Float:iter_get(Iter:building, ExitY);
	z = Float:iter_get(Iter:building, ExitZ);
	return 1;
}

stock Building_GetType(Building:building)
{
	return iter_get(Iter:building, Type);
}

stock bool:Building_SetType(Building:building, type)
{
	if(type < 0 || type >= BUILDING_TYPE_LAST)
		return false;
	iter_set_cell(Iter:building, Type, type);

	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET type = '%d' WHERE id = '%d'", type, Building_GetID(building));
	return true;
}

stock Building_GetID(Building:building)
{
	return iter_get(Iter:building, ID);
}

stock Building_SetLocked(Building:building, locked)
{
	iter_set_cell(Iter:building, Locked, locked);

	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET locked = '%d' WHERE id = '%d'", 
					locked, Building_GetID(building));
	return 1;
}

stock Building_GetMoney(Building:building)
{
	return iter_get(Iter:building, Money);
}

stock Building_GiveMoney(Building:building, amount)
{
	return Building_SetMoney(building, iter_get(Iter:building, Money) + amount);
}

stock Building_SetMoney(Building:building, amount)
{
	iter_set_cell(Iter:building, Money, amount);
	mysql_tquery_f(gMySQL, "UPDATE `buildings` SET money = '%d' WHERE id = '%d'", 
					amount, Building_GetID(building));
	return 1;
}

stock Building_GetAddress(Building:building)
{
    new zone[32], Float:x, Float:y, Float:z;
    Building_GetEnterPos(building, x, y, z);
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

	new Building:building = Building_Find(buildingid);

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
		return building;
	building = Character_GetBuilding(playerid, true);
	return building;
}

stock Building:Character_GetBuilding(playerid, bool:inside)
{
	new buildingid = 0;
	if(inside)
	{
		buildingid = Character_GetNearBuildingID(playerid);
	}
	else
	{
		new id, E_ELEMENT_TYPE:type;
		if(Character_GetNearestPickup(playerid, id, type) && (type == ELEMENT_TYPE_BUILDING_ENTRANCE || type == ELEMENT_TYPE_BUILDING_EXIT))
			buildingid = id;
	}
	return Building_Find(buildingid);
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

stock Character_ShowBizDialog(playerid)
{
	new Building:building = Character_GetCurrentBuilding(playerid);
	if(Building_IsValid(building) && Character_CanUseBuildingMenu(playerid, building))
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
	new Building:building = Character_GetCurrentBuilding(playerid);
	if(!Building_IsValid(building) || !Character_CanUseBuildingMenu(playerid, building))
		return 0;
	
	switch(listitem)
	{
		case 0:
		{
			if(Building_GetType(building) == BUILDING_TYPE_GAS_STATION)
			{
				Building_SetLocked(building, 1);
				return SendClientMessage(playerid, COLOR_ERROR, "Non puoi aprire questo tipo di edificio.");
			}
			Building_SetLocked(building, !Building_IsLocked(building));
			if(Building_IsLocked(building))
				SendClientMessage(playerid, COLOR_GREEN, "Hai chiuso il tuo edificio.");
			else
				SendClientMessage(playerid, COLOR_GREEN, "Hai aperto il tuo edificio.");
			return 1;
		}
		case 1:
		{
			return Building_ShowInventory(building, playerid);
		}
		case 2:
		{
			return Dialog_Show(playerid, Dialog_BuildingDepositMoney, DIALOG_STYLE_INPUT, "Deposita Denaro", "Inserisci l'ammontare di denaro che vuoi depositare all'interno del tuo edificio.\nAttualmente ci sono $%d nella cassa dell'edificio.", "Deposita", "Indietro", Building_GetMoney(building));
		}
		case 3:
		{
			return Dialog_Show(playerid, DialogBuildingWithdrawMoney, DIALOG_STYLE_INPUT, "Ritira Denaro", "Inserisci l'ammontare di denaro che vuoi ritirare dall'interno del tuo edificio.\nAttualmente ci sono $%d nella cassa dell'edificio.", "Ritira", "Indietro", Building_GetMoney(building));
		}
		case 4:
		{
			new money = Building_GetPrice(building)/2;
			new name[MAX_BUILDING_NAME];
			Building_GetName(building, name);
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
	
	new Building:building = Character_GetCurrentBuilding(playerid);
	if(!Building_IsValid(building) || !Character_CanUseBuildingMenu(playerid, building))
		return 0;
	
	new money =  strval(inputtext);
	if(money <= 0 || money > Character_GetMoney(playerid))
		return Dialog_Show(playerid, Dialog_BuildingDepositMoney, DIALOG_STYLE_INPUT, "Deposita Denaro", "{FF0000}Non disponi della quantità di denaro inserita.\n{FFFFFF}Inserisci l'ammontare di denaro che vuoi depositare all'interno del tuo edificio.\nAttualmente ci sono $%d nella cassa dell'edificio.", "Deposita", "Indietro", Building_GetMoney(building));
	
	Building_GiveMoney(building, money);
	Character_GiveMoney(playerid, -money, "building_deposit");

	SendFormattedMessage(playerid, -1, "Hai depositato $%d all'interno dell'edificio. Nella cassa dell'edificio ci sono $%d.", money, Building_GetMoney(building));

	return 1;
}

Dialog:DialogBuildingWithdrawMoney(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Character_ShowBizDialog(playerid);
	
	new Building:building = Character_GetCurrentBuilding(playerid);
	if(!Building_IsValid(building) || !Character_CanUseBuildingMenu(playerid, building))
		return 0;
	
	new money = strval(inputtext);
	if(money <= 0 || money > Building_GetMoney(building))
		return Dialog_Show(playerid, DialogBuildingWithdrawMoney, DIALOG_STYLE_INPUT, "Ritira Denaro", "{FF0000}La cassa non dispone della quantità di denaro inserita.\n{FFFFFF}Inserisci l'ammontare di denaro che vuoi ritirare dalla cassa del tuo edificio.\nAttualmente ci sono $%d nella cassa dell'edificio.", "Ritira", "Indietro", Building_GetMoney(building));
	
	Building_GiveMoney(building, -money);
	Character_GiveMoney(playerid, money, "building_withdraw");

	SendFormattedMessage(playerid, -1, "Hai ritirato $%d dall'interno dell'edificio. Nella cassa dell'edificio ci sono $%d.", money, Building_GetMoney(building));

	return 1;
}

Dialog:Dialog_BuildingSell(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Character_ShowBizDialog(playerid);
	
	new Building:building = Character_GetCurrentBuilding(playerid);
	if(!Building_IsValid(building) || !Character_CanUseBuildingMenu(playerid, building))
		return 0;
	
	new price = Building_GetPrice(building)/2,
		name[MAX_BUILDING_NAME];
	
	Building_GetName(building, name);

	Character_GiveMoney(playerid, price, "building_sell");

	SendFormattedMessage(playerid, -1, "Hai venduto il tuo edificio (%s) per $%d.", name, price);

	Building_ResetOwner(building);
	return 1;
}

Dialog:Dialog_BuildingSellToPlayer(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Character_ShowBizDialog(playerid);
	
	new Building:building = Character_GetCurrentBuilding(playerid);
	if(!Building_IsValid(building) || !Character_CanUseBuildingMenu(playerid, building))
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

	Character_SetRequest(playerid, id, REQUEST_TYPE_BUILDING, building, money);
	SendFormattedMessage(id, COLOR_RECEIVE_REQUEST, "%s vuole venderti il suo edificio per $%d.", Character_GetRolePlayName(playerid), money);
	SendFormattedMessage(playerid, COLOR_SEND_REQUEST, "Hai proposto il tuo edificio a %s per $%d.", Character_GetRolePlayName(id), money);
	return 1;
}

hook function Character_Enter(playerid)
{   
	new Building:building, E_ELEMENT_TYPE:type;
    if(Character_GetNearestPickup(playerid, building, type) && type == ELEMENT_TYPE_BUILDING_ENTRANCE)
    {
		printf("Last Enter");
		if(IsPlayerInAnyVehicle(playerid))
			return 0;
		
		if(Building_IsLocked(building))
			return GameTextForPlayer(playerid, "~r~Porta Chiusa", 5000, 1), 0;
		new 
			Float:x = 0.0, 
			Float:y = 0.0, 
			Float:z = 0.0,
			interiorId = Building_GetExitInterior(building),
			world = Building_GetExitWorld(building)
		;

		new String:str = Building_GetWelcomeTextStr(building);
		if(str_len(str) > 0)
			SendClientMessageStr(playerid, COLOR_GREEN, str);
		
		Building_GetExitPos(building, x, y, z);

		Streamer_UpdateEx(playerid, x, y, z, world, interiorId);

		SetPlayerInterior(playerid, interiorId);
		Character_SetVirtualWorld(playerid, world);
		// Should I use Fading?
		Character_SetSafePosition(playerid, x, y, z, interiorId, world);

		CallLocalFunction(#OnPlayerEnterBuilding, "dd", playerid, _:building);
		return 1;
    }
    return 0;
}