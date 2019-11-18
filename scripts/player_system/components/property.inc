
#define for_player_building(%0:%1) for(new Iter:__iter=list_iter(Character_GetOwnedBuildingsList(%1)), Building:%0=(iter_inside(__iter) ? (Building:iter_get(__iter)) : (Building:0));iter_inside(__iter);%0=(iter_move_next(__iter) != ITER_NULL ? (Building:iter_get(__iter)) : (Building:0)))

#include <house_system\core>
#include <pickup_system\core>
#include <YSI_Coding\y_hooks>

static
	House:PlayerHouseRent[MAX_PLAYERS],
	PlayerHouseRentPrice[MAX_PLAYERS]
;

static 
	List:CharacterBuildings[MAX_PLAYERS],
	List:CharacterHouses[MAX_PLAYERS];

hook OnCharacterClearData(playerid)
{
	if(list_valid(CharacterBuildings[playerid]))
		list_delete(CharacterBuildings[playerid]);
	
	if(list_valid(CharacterHouses[playerid]))
		list_delete(CharacterHouses[playerid]);

	CharacterBuildings[playerid] = CharacterHouses[playerid] = INVALID_LIST;
	PlayerHouseRent[playerid] = INVALID_HOUSE;
	return 1;
}

hook OnCharacterLoad(playerid)
{
	inline OnLoad()
	{
		new hid = 0;
		cache_get_value_name_int(0, "house_rent", hid);
		cache_get_value_name_int(0, "house_rent_price", PlayerHouseRentPrice[playerid]);
		PlayerHouseRent[playerid] = House_Find(hid);
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT house_rent, house_rent_price FROM `characters` WHERE id = '%d'", Character_GetID(playerid));
	return 1;
}

hook OnCharacterLoaded(playerid)
{
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

hook OnBuildingDeleted(Building:building)
{
	foreach(new i : Player)
	{
		Character_RemoveOwnedBuilding(i, building);
	}
	return 1;
}

flags:affittacasa(CMD_ALIVE_USER);
CMD:affittacasa(playerid, params[])
{
	if(Character_HasRentHouse(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Sei già affittuario di una casa.");
	new House:house = Character_GetCurrentHouse(playerid);
	if(House_IsValid(house))
	{
		if(!House_IsRentable(house))
			return SendClientMessage(playerid, COLOR_ERROR, "Questa casa non è in affitto.");		
		
		SendFormattedMessage(playerid, COLOR_ORANGE, "Hai affittato questa casa per $%s per PayDay.", FormatNumber(House_GetRentPrice(house)));
		SendFormattedMessage(playerid, COLOR_ORANGE, "Indirizzo: %s", House_GetAddress(house));

		Character_SetHouseRentPrice(playerid, House_GetRentPrice(house));
		Character_SetRentHouse(playerid, house);
	}
	else
		SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino ad una casa in affitto.");
	return 1;
}

hook OnCharacterTryBuy(playerid)
{
	static response[e_DIALOG_RESPONSE_INFO];

	new House:house = Character_GetHouse(playerid, false);
	new Building:building = Character_GetBuilding(playerid, false);
	
	if(House_IsValid(house))
	{
		if(!House_IsOwnable(house))
			return SendClientMessage(playerid, COLOR_ERROR, "Questa proprietà non è in vendita.");
		yield 1;
		await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_MSGBOX, House_GetAddressStr(house), str_format("Sei sicuro di voler acquistare questa casa per $%d?", House_GetPrice(house)), @("Acquista"), @("Annulla"));
		
		if(response[E_DIALOG_RESPONSE_Response])
		{
			Character_BuyHouse(playerid, house);
		}
	}	
	else if(Building_IsValid(building))
	{
		if(!Building_IsOwnable(building) || Building_GetOwnerID(building) > 0)
			return SendClientMessage(playerid, COLOR_ERROR, "Questa proprietà non è in vendita.");

		yield 1;
		await_arr(response) ShowPlayerAsyncDialogStr(playerid, DIALOG_STYLE_MSGBOX, Building_GetAddressStr(building), str_format("Sei sicuro di voler acquistare questa biz per $%d?", Building_GetPrice(building)), @("Acquista"), @("Annulla"));
		
		if(response[E_DIALOG_RESPONSE_Response])
		{
			Character_BuyBuilding(playerid, building);
		}
	}
	return 1;
}

CMD:myhouses(playerid, params[])
{
	new House:rent = Character_GetRentHouse(playerid);
	if(Character_GetOwnedHouseCount(playerid) == 0 && !House_IsValid(rent))
		return SendClientMessage(playerid, 0xFF6347FF, "Non possiedi una o più case.");
	
	SendClientMessage(playerid, 0xFF6347FF, "Le case che possiedi sono:");

	if(House_IsValid(rent))
		SendFormattedMessage(playerid, 0xFF6347FF, "In Affitto: %s", House_GetAddress(rent));
	
	if(list_valid(CharacterHouses[playerid]))
	{
		for_list(i : CharacterHouses[playerid])
		{
			SendFormattedMessage(playerid, 0xFF6347FF, "- %s", House_GetAddress(House:iter_get(i)));
		}
	}
	return 1;
}

CMD:mybiz(playerid, params[])
{
	if(Character_GetOwnedHouseCount(playerid) == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non possiedi uno o più businesses.");
	SendClientMessage(playerid, 0xFF6347FF, "I business che possiedi sono:");
	if(Character_GetOwnedBuildingCount(playerid) <= 0)
		return SendClientMessage(playerid, 0xFF6347FF, "Non possiedi uno o più biz.");
	for_list(i : CharacterBuildings[playerid])
	{
		SendFormattedMessage(playerid, 0xFF6347FF, "- %s", Building_GetAddress(Building:iter_get(i)));
	}
	return 1;
}

stock Character_HasRentHouse(playerid)
{
    return House_IsValid(PlayerHouseRent[playerid]);
}

stock House:Character_GetRentHouse(playerid)
{
    return PlayerHouseRent[playerid];
}

stock Character_SetRentHouse(playerid, House:house)
{
    PlayerHouseRent[playerid] = house;
	new hid = House_IsValid(house) ? House_GetID(house) : INVALID_HOUSE_ID;
	mysql_pquery_f(gMySQL, "UPDATE `characters` SET house_rent = '%d' WHERE id = '%d'", hid, Character_GetID(playerid));
}

stock bool:Character_SetHouseRentPrice(playerid, price)
{
	PlayerHouseRentPrice[playerid] = price;
	mysql_pquery_f(gMySQL, "UPDATE `characters` SET house_rent_price = '%d' WHERE id = '%d'", price, Character_GetID(playerid));
	return true;
}

stock Character_GetHouseRentPrice(playerid)
{
	return PlayerHouseRentPrice[playerid];
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(Player_HasHotKeysEnabled(playerid) && Character_IsAlive(playerid) && !IsPlayerInAnyVehicle(playerid))
	{
		if(PRESSED(KEY_YES))
		{
			callcmd::entra(playerid, "");
		}
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

// Pref callcmd::entra(playerid, ""); instead of Character_Enter because the first makes some considerations.
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

    if(!House_IsOwnable(house))
	   return SendClientMessage(playerid, COLOR_ERROR, "Questa proprietà non è in vendita.");

    new price = House_GetPrice(house);
    if(Character_GetMoney(playerid) < price)
	   return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi per acquistare questa proprietà.");

	if(!Character_CanOwnHouse(playerid, house))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi acquistare ulteriori immobili.");

    if(House_SetOwner(house, playerid))
	{
    	Character_GiveMoney(playerid, -price, "Character_BuyHouse");
    	
		SendFormattedMessage(playerid, 0x33AA33FF, "Hai acquistato l'immobile situato a %s.", House_GetAddress(house));
	}
    return 1;
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
	for_player_building(i : playerid)
	{
		if(building == i)
		{
			result = true;
			break;
		}
	}
	return result;
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
	#pragma unused house
	static const maxBuildings[] = {2, 3, 4, 5};
	new m = maxBuildings[Account_GetPremiumLevel(playerid)];
	if(Character_GetOwnedHouseCount(playerid) >= m)
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