
#include <pickup_system\core>

#include <YSI_Coding\y_hooks>

static
	PlayerHouseKey[MAX_PLAYERS]
;

static 
	List:CharacterBuildings[MAX_PLAYERS],
	List:CharacterHouses[MAX_PLAYERS];

hook OnPlayerClearData(playerid)
{
	if(list_valid(CharacterBuildings[playerid]))
		list_delete_deep(CharacterBuildings[playerid]);
	
	if(list_valid(CharacterHouses[playerid]))
		list_delete_deep(CharacterHouses[playerid]);

	PlayerHouseKey[playerid] = 0;
	return 1;
}

hook OnCharacterLoad(playerid)
{
	inline OnLoad()
	{
		cache_get_value_index_int(0, 0, PlayerHouseKey[playerid]);
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT house_key FROM `characters` WHERE id = '%d'", Character_GetID(playerid));

	for_building(building)
	{
		if(Building_GetOwnerID(building) == Character_GetID(playerid))
		{
			Character_AddOwnedBuilding(playerid, building);
		}
	}

	for_map(i : GetHousesMap())
	{
		new houseid = iter_get_key(i);
		if(House_GetOwnerID(houseid) == Character_GetID(playerid))
		{
			Character_AddOwnedHouse(playerid, houseid);
		}
	}
	return 1;
}

hook OnCharacterTryBuy(playerid)
{
	new houseid = Character_GetHouse(playerid, false);
	if(House_IsValid(houseid))
		return Character_BuyHouse(playerid, houseid);
				
	new Building:building, E_ELEMENT_TYPE:type;
	if(Character_GetNearestPickup(playerid, building, type) && type == ELEMENT_TYPE_BUILDING_ENTRANCE && Building_IsValid(building))
		return Character_BuyBuilding(playerid, building);
	return 1;
}

CMD:myhouses(playerid, params[])
{
	SendClientMessage(playerid, 0xFF6347FF, "Le case che possiedi sono:");
	return 1;
}

stock Character_HasHouseKey(playerid)
{
    return PlayerHouseKey[playerid] > 0;
}

stock Character_GetHouseKey(playerid)
{
    return PlayerHouseKey[playerid];
}

stock Character_SetHouseKey(playerid, houseid)
{
    PlayerHouseKey[playerid] = houseid;
	mysql_tquery_f(gMySQL, "UPDATE `characters` SET house_key = '%d' WHERE id = '%d'", houseid, Character_GetID(playerid));
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(Player_HasHotKeysEnabled(playerid) && Character_IsAlive(playerid))
	{
		new elementId, E_ELEMENT_TYPE:type;
		if(Character_GetNearestPickup(playerid, elementId, type) && !IsPlayerInAnyVehicle(playerid))
		{
			if(PRESSED(KEY_YES))
			{
				// Should I write an "OnInteract" callback?
				if(type == ELEMENT_TYPE_BUILDING_ENTRANCE || type == ELEMENT_TYPE_HOUSE_ENTRANCE)
				{
					Character_Enter(playerid);
				}
				else if(type == ELEMENT_TYPE_BUILDING_EXIT || type == ELEMENT_TYPE_HOUSE_EXIT)
				{
					Character_Exit(playerid, elementId, type);
				}
			}
			else if(PRESSED(KEY_WALK))
			{
				if(type == ELEMENT_TYPE_HOUSE_ENTRANCE || type == ELEMENT_TYPE_HOUSE_EXIT)
				{
					Character_ShowHouseDialog(playerid);
				}
				else if(type == ELEMENT_TYPE_BUILDING_ENTRANCE || type == ELEMENT_TYPE_BUILDING_EXIT)
				{
					Character_ShowBizDialog(playerid);
				}
			}
		}
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

stock Character_Exit(playerid, elementId, E_ELEMENT_TYPE:type)
{
    if((type != ELEMENT_TYPE_BUILDING_EXIT && type != ELEMENT_TYPE_HOUSE_EXIT))
	   return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'uscita di un edificio."), 0;
	new Building:building = Building_Find(elementId);
    if(Building_IsValid(building) && type == ELEMENT_TYPE_BUILDING_EXIT)
    {
		if(Building_GetType(building) == BUILDING_TYPE_GARAGE)
			return Internal_ExitGarage(playerid, building);
	   	return Internal_ExitBuilding(playerid, building);
    }
    else if(type == ELEMENT_TYPE_HOUSE_EXIT)
    {
	   return Internal_ExitHouse(playerid, elementId);
    }
    return 1;
}


static Internal_ExitGarage(playerid, Building:building)
{
	if(Building_IsLocked(building))
		return GameTextForPlayer(playerid, "~r~Chiuso", 8000, 1), 0;
	new 
	   Float:x = 0.0, 
	   Float:y = 0.0, 
	   Float:z = 0.0,
	   interiorId = Building_GetEnterInterior(building),
	   world = Building_GetEnterWorld(building)
	;
	Building_GetEnterPos(building, x, y, z);

	new vehicleid = GetPlayerVehicleID(playerid);
	
	
	SetPlayerInterior(playerid, interiorId);
	Character_SetVirtualWorld(playerid, world);
	
	if(vehicleid > 0 && GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
	{
		LinkVehicleToInterior(vehicleid, interiorId);
		SetVehicleVirtualWorld(vehicleid, world);
		SetVehiclePos(vehicleid, x, y, z);
	}
	else
	{
		Character_SetSafePosition(playerid, x, y, z, interiorId, world);
	}
	return 1;
}

static Internal_ExitHouse(playerid, houseid)
{
	if(IsPlayerInAnyVehicle(playerid))
		return 0;
	if(House_IsLocked(houseid))
		return GameTextForPlayer(playerid, "~r~Chiuso", 8000, 1), 0;
	new 
	   Float:x = 0.0, 
	   Float:y = 0.0, 
	   Float:z = 0.0,
	   interiorId = House_GetEnterInterior(houseid),
	   world = House_GetEnterWorld(houseid)
	;
	
	Character_AMe(playerid, "apre la porta ed esce.");
	
    SetPlayerInterior(playerid, interiorId);
    Character_SetVirtualWorld(playerid, world);
	
	House_GetEnterPosition(houseid, x, y, z);
    Streamer_UpdateEx(playerid, x, y, z, world, interiorId);
    Character_SetSafePosition(playerid, x, y, z, interiorId, world);
	return 1;
}

static Internal_ExitBuilding(playerid, Building:building)
{
	if(IsPlayerInAnyVehicle(playerid))
		return 0;
	if(Building_IsLocked(building))
		return GameTextForPlayer(playerid, "~r~Chiuso", 4000, 1), 0;
	new 
	   Float:x = 0.0, 
	   Float:y = 0.0, 
	   Float:z = 0.0,
	   interiorId = Building_GetEnterInterior(building),
	   world = Building_GetEnterWorld(building)
	;
	
	Character_AMe(playerid, "apre la porta ed esce.");
	
    SetPlayerInterior(playerid, interiorId);
    Character_SetVirtualWorld(playerid, world);
	
	Building_GetEnterPos(building, x, y, z);
    Streamer_UpdateEx(playerid, x, y, z, world, interiorId);
    Character_SetSafePosition(playerid, x, y, z, interiorId, world);
	return 1;
}

stock Character_Enter(playerid)
{
	#pragma unused playerid
	return 1;
}

stock Character_BuyBuilding(playerid, Building:building)
{
    if(!Building_IsOwned(building))
	   return SendClientMessage(playerid, COLOR_ERROR, "Questa proprietà non è in vendita.");

    if(Character_GetMoney(playerid) < Building_GetPrice(building))
	   return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi per acquistare questa proprietà.");
    
    Character_GiveMoney(playerid, -Building_GetPrice(building));
    
    Building_SetOwner(building, playerid);

	Character_AddOwnedBuilding(playerid, building);

	new name[MAX_BUILDING_NAME];
	Building_GetName(building, name);
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai acquistato questo business (%s) per $%d.", name, Building_GetPrice(building));
    return 1;
}

stock Character_BuyHouse(playerid, houseid)
{
    if(!House_IsValid(houseid))
	   return 0;

	if(House_GetOwnerID(houseid) == Character_GetID(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Sei già proprietario di questa casa.");

    if(House_GetOwnerID(houseid) != 0)
	   return SendClientMessage(playerid, COLOR_ERROR, "Questa proprietà non è in vendita.");

    //if(Character_HasHouseKey(playerid))
	   //return SendClientMessage(playerid, COLOR_ERROR, "Sei già proprietario di una casa.");

    new price = House_GetPrice(houseid);
    if(Character_GetMoney(playerid) < price)
	   return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi per acquistare questa proprietà.");

    House_SetOwner(houseid, playerid);

    Character_GiveMoney(playerid, -price, "Character_BuyHouse");

	new zone[64];
	House_GetAddress(houseid, zone);
    SendFormattedMessage(playerid, 0x33AA33FF, "Hai acquistato l'immobile situato a %s, Las Venturas, San Andreas.", zone);
    return 1;
}

stock Character_ShowHouseDialog(playerid)
{
	new houseid = Character_GetHouseMenu(playerid);
	if(House_IsValid(houseid))
	{
		return Dialog_Show(playerid, Dialog_House, DIALOG_STYLE_LIST, "Casa", "Apri/Chiudi Porta\nInventario\nDeposita Soldi\nRitira Soldi\nVendi\nVendi a Giocatore\nCambia Interior", "Continua", "Chiudi");
	}
	return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata o all'interno di casa tua."), 0;
}

stock bool:Character_AddOwnedBuilding(playerid, Building:building)
{
	if(!Character_IsLogged(playerid))
		return false;
	
	if(!list_valid(CharacterBuildings[playerid]))
		CharacterBuildings[playerid] = list_new();
	
	list_add(CharacterBuildings[playerid], Building_GetID(building));
	return true;
}

stock bool:Character_RemoveOwnedBuilding(playerid, Building:building)
{
	if(!Character_IsLogged(playerid))
		return false;
	
	if(!list_valid(CharacterBuildings[playerid]))
		return false;
	
	new index = -1;
	for_list(i : CharacterBuildings[playerid])
	{
		if(iter_get(i) == Building_GetID(building))
		{
			index = iter_get(i);
			break;
		}
	}
	if(index != -1)
	{
		list_remove(CharacterBuildings[playerid], index);
		return true;
	}
	return false;
}

stock List:Character_GetOwnedBuildingsList(playerid)
{
	if(!list_valid(CharacterBuildings[playerid]))
		return List:0;
	return CharacterBuildings[playerid];
}

stock bool:Character_DoesOwnBuilding(playerid, Building:building)
{
	if(!list_valid(CharacterBuildings[playerid]) || list_size(CharacterBuildings[playerid]) == 0)
		return false;
	new bool:result = false;
	for_list(i : CharacterBuildings[playerid])
	{
		if(Building_GetID(building) == iter_get(i))
		{
			result = true;
			break;
		}
	}
	return result;
}

// Returns a List of buildings by type owned by player
stock List:Character_GetBuildingsByType(playerid, E_BUILDING_TYPE:type)
{
	if(!list_valid(CharacterBuildings[playerid]) || list_size(CharacterBuildings[playerid]) == 0)
		return List:0;
	new List:list = list_new(), buildingid = 0;
	for_list(i : CharacterBuildings[playerid])
	{
		buildingid = iter_get(i);
		if(Building_GetType(Building_Find(buildingid)) == _:type)
			list_add(list, buildingid);
	}
	return list;
}

stock bool:Character_AddOwnedHouse(playerid, houseid)
{
	if(!Character_IsLogged(playerid) || !House_IsValid(houseid))
		return false;
	
	if(!list_valid(CharacterHouses[playerid]))
		CharacterHouses[playerid] = list_new();
	
	list_add(CharacterHouses[playerid], houseid);
	return true;
}

stock bool:Character_RemoveOwnedHouse(playerid, houseid)
{
	if(!Character_IsLogged(playerid) || !House_IsValid(houseid))
		return false;
	
	if(!list_valid(CharacterHouses[playerid]))
		return false;
	new index = -1;
	for(new i = 0, j = list_size(CharacterHouses[playerid]); i < j; ++i)
	{
		if(list_get(CharacterHouses[playerid], i) == houseid)
		{
			index = i;
			break;
		}
	}
	if(index != -1)
	{
		list_remove(CharacterHouses[playerid], index);
		return true;
	}
	return false;
}

stock List:Character_GetOwnedHousesList(playerid)
{
	if(!list_valid(CharacterHouses[playerid]))
		return List:0;
	return CharacterHouses[playerid];
}

stock bool:Character_DoesOwnHouse(playerid, houseid)
{
	if(!list_valid(CharacterHouses[playerid]) || list_size(CharacterHouses[playerid]) == 0)
		return false;
	new bool:result = false;
	for_list(i : CharacterHouses[playerid])
	{
		if(iter_get(i) == houseid)
		{
			result = true;
			break;
		}
	}
	return result;
}