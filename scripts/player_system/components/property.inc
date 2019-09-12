
#include <pickup_system\core>

#include <YSI_Coding\y_hooks>

static 
	PlayerBuildingKey[MAX_PLAYERS],
	PlayerHouseKey[MAX_PLAYERS]
;

hook OnPlayerClearData(playerid)
{
	PlayerBuildingKey[playerid] = PlayerHouseKey[playerid] = 0;
	return 1;
}

hook OnCharacterLoad(playerid)
{
	inline OnLoad()
	{
		cache_get_value_index_int(0, 0, PlayerBuildingKey[playerid]);
		cache_get_value_index_int(0, 1, PlayerHouseKey[playerid]);
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT building_key, house_key FROM `characters` WHERE id = '%d'", Character_GetID(playerid));
	return 1;
}

hook OnCharacterTryBuy(playerid)
{
	new elementid, E_ELEMENT_TYPE:type;
	if(Character_GetNearestPickup(playerid, elementid, type))
	{
		if(type == ELEMENT_TYPE_HOUSE_ENTRANCE && House_IsValid(elementid))
			return Character_BuyHouse(playerid, elementid);
		
		if(type == ELEMENT_TYPE_BUILDING_ENTRANCE && Building_IsValid(elementid))
			return Character_BuyBuilding(playerid, elementid);
	}
	return 1;
}

stock Character_HasBuildingKey(playerid)
{
    return PlayerBuildingKey[playerid] > 0;
}

stock Character_GetBuildingKey(playerid)
{
    return PlayerBuildingKey[playerid];
}

stock Character_SetBuildingKey(playerid, buildingid)
{
    PlayerBuildingKey[playerid] = buildingid;
	mysql_tquery_f(gMySQL, "UPDATE `characters` SET building_key = '%d' WHERE id = '%d'", buildingid, Character_GetID(playerid));
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

stock Character_Enter(playerid)
{   
	new buildingid, E_ELEMENT_TYPE:type;
	printf("Last Enter");
    if(Character_GetNearestPickup(playerid, buildingid, type) && type == ELEMENT_TYPE_BUILDING_ENTRANCE)
    {
		if(IsPlayerInAnyVehicle(playerid))
			return 0;
		
		if(Building_IsLocked(buildingid))
			return GameTextForPlayer(playerid, "~r~Porta Chiusa", 5000, 1);
		new 
			Float:x = 0.0, 
			Float:y = 0.0, 
			Float:z = 0.0,
			interiorId = Building_GetExitInterior(buildingid),
			world = Building_GetExitWorld(buildingid)
		;

		new String:str = Building_GetWelcomeTextStr(buildingid);
		if(str_len(str) > 0)
			SendClientMessageStr(playerid, COLOR_GREEN, str);
		
		Building_GetExitPos(buildingid, x, y, z);

		Streamer_UpdateEx(playerid, x, y, z, world, interiorId);

		SetPlayerInterior(playerid, interiorId);
		Character_SetVirtualWorld(playerid, world);
		// Should I use Fading?
		Character_SetSafePosition(playerid, x, y, z, interiorId, world);

		CallLocalFunction(#OnPlayerEnterBuilding, "dd", playerid, buildingid);
		return 1;
    }
    return 0;
}

stock Character_Exit(playerid, elementId, E_ELEMENT_TYPE:type)
{
    if((type != ELEMENT_TYPE_BUILDING_EXIT && type != ELEMENT_TYPE_HOUSE_EXIT))
	   return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'uscita di un edificio."), 0;

    if(type == ELEMENT_TYPE_BUILDING_EXIT)
    {
		if(Building_GetType(elementId) == BUILDING_TYPE_GARAGE)
			return Internal_ExitGarage(playerid, elementId);
	   	return Internal_ExitBuilding(playerid, elementId);
    }
    else if(type == ELEMENT_TYPE_HOUSE_EXIT)
    {
	   return Internal_ExitHouse(playerid, elementId);
    }
    return 1;
}


static Internal_ExitGarage(playerid, buildingid)
{
	if(Building_IsLocked(buildingid))
		return GameTextForPlayer(playerid, "~r~Chiuso", 8000, 1), 0;
	new 
	   Float:x = 0.0, 
	   Float:y = 0.0, 
	   Float:z = 0.0,
	   interiorId = Building_GetEnterInterior(buildingid),
	   world = Building_GetEnterWorld(buildingid)
	;
	Building_GetEnterPos(buildingid, x, y, z);

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

static Internal_ExitBuilding(playerid, buildingid)
{
	if(IsPlayerInAnyVehicle(playerid))
		return 0;
	if(Building_IsLocked(buildingid))
		return GameTextForPlayer(playerid, "~r~Chiuso", 4000, 1), 0;
	new 
	   Float:x = 0.0, 
	   Float:y = 0.0, 
	   Float:z = 0.0,
	   interiorId = Building_GetEnterInterior(buildingid),
	   world = Building_GetEnterWorld(buildingid)
	;
	
	Character_AMe(playerid, "apre la porta ed esce.");
	
    SetPlayerInterior(playerid, interiorId);
    Character_SetVirtualWorld(playerid, world);
	
	Building_GetEnterPos(buildingid, x, y, z);
    Streamer_UpdateEx(playerid, x, y, z, world, interiorId);
    Character_SetSafePosition(playerid, x, y, z, interiorId, world);
	return 1;
}

stock Character_BuyBuilding(playerid, buildingid)
{
    if(!Building_IsValid(buildingid))
	   return 0;
    
    if(!Building_IsOwnable(buildingid) || Building_GetOwnerID(buildingid) != 0)
	   return SendClientMessage(playerid, COLOR_ERROR, "Questa proprietà non è in vendita.");

    if(Character_GetMoney(playerid) < Building_GetPrice(buildingid))
	   return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi per acquistare questa proprietà.");
    
    Character_GiveMoney(playerid, -Building_GetPrice(buildingid));
    
    Building_SetOwner(buildingid, playerid);
    
    Character_SetBuildingKey(playerid, buildingid);

	new name[MAX_BUILDING_NAME];
	Building_GetName(buildingid, name);

    SendFormattedMessage(playerid, COLOR_GREEN, "Hai acquistato questo edificio (%s) per $%d.", name, Building_GetPrice(buildingid));
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

    SendFormattedMessage(playerid, COLOR_GREEN, "Congratulazioni! Hai acquistato questa casa per $%d.", price);
    return 1;
}

stock Character_ShowHouseDialog(playerid)
{
	new houseid = Character_GetNearHouseIDMenu(playerid);
	if(House_IsValid(houseid))
	{
		return Dialog_Show(playerid, Dialog_House, DIALOG_STYLE_LIST, "Casa", "Apri/Chiudi Porta\nInventario\nDeposita Soldi\nRitira Soldi\nVendi\nVendi a Giocatore\nCambia Interior", "Continua", "Chiudi");
	}
	return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata o all'interno di casa tua."), 0;
}