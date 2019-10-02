#include <house_system\core>
#include <pickup_system\core>

#include <YSI_Coding\y_hooks>

static
	House:PlayerHouseKey[MAX_PLAYERS]
;

static 
	List:CharacterBuildings[MAX_PLAYERS],
	List:CharacterHouses[MAX_PLAYERS];

hook OnCharacterClearData(playerid)
{
	if(list_valid(CharacterBuildings[playerid]))
		list_delete_deep(CharacterBuildings[playerid]);
	
	if(list_valid(CharacterHouses[playerid]))
		list_delete_deep(CharacterHouses[playerid]);

	PlayerHouseKey[playerid] = INVALID_HOUSE;
	return 1;
}

hook OnCharacterLoad(playerid)
{
	inline OnLoad()
	{
		new hid = 0;
		cache_get_value_index_int(0, 0, hid);
		PlayerHouseKey[playerid] = House_Find(hid);
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT house_key FROM `characters` WHERE id = '%d'", Character_GetID(playerid));

	for_building(building)
	{
		if(Building_GetOwnerID(building) == Character_GetID(playerid))
		{
			Character_AddOwnedBuilding(playerid, building);
		}
	}

	for_house(house)
	{
		if(House_GetOwnerID(house) == Character_GetID(playerid))
		{
			Character_AddOwnedHouse(playerid, house);
		}
	}
	return 1;
}

hook OnCharacterTryBuy(playerid)
{
	new House:house = Character_GetHouse(playerid, false);
	if(House_IsValid(house))
		return Character_BuyHouse(playerid, house);
				
	new Building:building, E_ELEMENT_TYPE:type;
	if(Character_GetNearestPickup(playerid, building, type) && type == ELEMENT_TYPE_BUILDING_ENTRANCE && Building_IsValid(building))
		return Character_BuyBuilding(playerid, building);
	return 1;
}

CMD:myhouses(playerid, params[])
{
	if(Character_GetOwnedHouseCount(playerid) == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non possiedi case.");
	SendClientMessage(playerid, 0xFF6347FF, "Le case che possiedi sono:");

	return 1;
}

CMD:mybiz(playerid, params[])
{
	if(Character_GetOwnedHouseCount(playerid) == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non possiedi businesses.");
	SendClientMessage(playerid, 0xFF6347FF, "I business che possiedi sono:");
	for_list(i : CharacterBuildings[playerid])
	{
		new Building:building = Building:iter_get(i);
		SendClientMessage(playerid, 0xFF6347FF, Building_GetAddress(building));
	}
	return 1;
}

stock Character_HasHouseKey(playerid)
{
    return House_IsValid(PlayerHouseKey[playerid]);
}

stock House:Character_GetHouseKey(playerid)
{
    return PlayerHouseKey[playerid];
}

stock Character_SetHouseKey(playerid, House:house)
{
    PlayerHouseKey[playerid] = house;
	new hid = House_IsValid(house) ? House_GetID(house) : INVALID_HOUSE_ID;
	mysql_tquery_f(gMySQL, "UPDATE `characters` SET house_key = '%d' WHERE id = '%d'", hid, Character_GetID(playerid));
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(Player_HasHotKeysEnabled(playerid) && Character_IsAlive(playerid) && !IsPlayerInAnyVehicle(playerid))
	{
		if(PRESSED(KEY_YES))
		{
			// Should I write an "OnInteract" callback?
			Character_Enter(playerid);
		}
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

stock Character_Enter(playerid)
{
	#pragma unused playerid
	return 1;
}

stock Character_BuyBuilding(playerid, Building:building)
{
    if(!Building_IsOwnable(building))
	   return SendClientMessage(playerid, COLOR_ERROR, "Questa proprietà non è in vendita.");

    if(Character_GetMoney(playerid) < Building_GetPrice(building))
	   return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi per acquistare questa proprietà.");
    
	if(!Character_CanOwnBuilding(playerid, building))
		return SendClientMessage(playerid, COLOR_ERROR, "Hai raggiunto il limite massimo di business. Acquista un pacchetto premium per incrementarlo.");
    
	Character_GiveMoney(playerid, -Building_GetPrice(building));
    
    Building_SetOwner(building, playerid);

	//Character_AddOwnedBuilding(playerid, building);

	new name[MAX_BUILDING_NAME];
	Building_GetName(building, name);
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai acquistato questo business (%s) per $%d.", name, Building_GetPrice(building));
    return 1;
}

stock Character_BuyHouse(playerid, House:house)
{
    if(!House_IsValid(house))
	   return 0;

	if(House_GetOwnerID(house) == Character_GetID(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Sei già proprietario di questa casa.");

    if(House_GetOwnerID(house) != 0)
	   return SendClientMessage(playerid, COLOR_ERROR, "Questa proprietà non è in vendita.");

    //if(Character_HasHouseKey(playerid))
	   //return SendClientMessage(playerid, COLOR_ERROR, "Sei già proprietario di una casa.");

    new price = House_GetPrice(house);
    if(Character_GetMoney(playerid) < price)
	   return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi per acquistare questa proprietà.");

    House_SetOwner(house, playerid);

    Character_GiveMoney(playerid, -price, "Character_BuyHouse");

	new zone[64];
	House_GetAddress(house, zone);
    SendFormattedMessage(playerid, 0x33AA33FF, "Hai acquistato l'immobile situato a %s.", zone);
    return 1;
}

stock Character_ShowHouseDialog(playerid)
{
	new House:house = Character_GetHouseMenu(playerid);
	if(House_IsValid(house))
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
	
	if(Character_DoesOwnBuilding(playerid, building))
		return true;

	list_add(CharacterBuildings[playerid], _:building);
	return true;
}

stock bool:Character_RemoveOwnedBuilding(playerid, Building:building)
{
	if(!Character_IsLogged(playerid))
		return false;
	
	if(!list_valid(CharacterBuildings[playerid]))
		return false;
	
	new index = -1;
	for(new i = 0, j = list_size(CharacterBuildings[playerid]); i < j; ++i)
	{
		if(list_get(CharacterBuildings[playerid], i) == _:building)
		{
			index = i;
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

stock Character_GetOwnedBuildingCount(playerid)
{
	if(!list_valid(CharacterBuildings[playerid]))
		return 0;
	return list_size(CharacterBuildings[playerid]);
}

stock bool:Character_CanOwnBuilding(playerid, Building:building)
{
	#pragma unused building
	static const maxBuildings[] = {1, 2, 3, 4};
	new m = maxBuildings[Account_GetPremiumLevel(playerid)];
	if(Character_GetOwnedBuildingCount(playerid) >= m)
		return false;
	return true;
}

stock bool:Character_DoesOwnBuilding(playerid, Building:building)
{
	if(!list_valid(CharacterBuildings[playerid]) || list_size(CharacterBuildings[playerid]) == 0)
		return false;
	new bool:result = false;
	for_list(i : CharacterBuildings[playerid])
	{
		if(_:building == iter_get(i))
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
	new List:list = list_new(), Building:building = INVALID_BUILDING;
	for_list(i : CharacterBuildings[playerid])
	{
		building = Building:iter_get(i);
		if(Building_IsValid(building) && Building_GetType(building) == _:type)
			list_add(list, _:building);
	}
	return list;
}

stock bool:Character_AddOwnedHouse(playerid, House:house)
{
	if(!Character_IsLogged(playerid))
		return false;
	
	if(!list_valid(CharacterHouses[playerid]))
		CharacterHouses[playerid] = list_new();
	
	list_add(CharacterHouses[playerid], house);
	return true;
}

stock bool:Character_RemoveOwnedHouse(playerid, House:house)
{
	if(!Character_IsLogged(playerid))
		return false;
	
	if(!list_valid(CharacterHouses[playerid]))
		return false;
	
	new index = -1;
	for(new i = 0, j = list_size(CharacterHouses[playerid]); i < j; ++i)
	{
		if(list_get(CharacterHouses[playerid], i) == _:house)
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

stock Character_GetOwnedHouseCount(playerid)
{
	if(!list_valid(CharacterHouses[playerid]))
		return 0;
	return list_size(CharacterHouses[playerid]);
}

stock bool:Character_CanOwnHouse(playerid, House:house)
{
	static const maxBuildings[] = {2, 3, 4, 5};
	new max = maxBuildings[Account_GetPremiumLevel(playerid)];
	if(Character_GetOwnedHouseCount(playerid) >= max)
		return false;
	return true;
}

stock bool:Character_DoesOwnHouse(playerid, House:house)
{
	if(!list_valid(CharacterHouses[playerid]) || list_size(CharacterHouses[playerid]) == 0)
		return false;
	new bool:result = false;
	for_list(i : CharacterHouses[playerid])
	{
		if(iter_get(i) == _:house)
		{
			result = true;
			break;
		}
	}
	return result;
}