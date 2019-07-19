#include <YSI_Coding\y_hooks>

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(Account_HasHotKeysEnabled(playerid) && Character_IsAlive(playerid))
	{
		new elementId, E_ELEMENT_TYPE:type;
		if(Character_GetNearestPickup(playerid, elementId, type) && !IsPlayerInAnyVehicle(playerid))
		{
			if(PRESSED(KEY_YES))
			{
				// Should I write an "OnInteract" callback?
				if(type == ELEMENT_TYPE_BUILDING_ENTRANCE || type == ELEMENT_TYPE_HOUSE_ENTRANCE)
				{
					Character_Enter(playerid, elementId, type);
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
					new houseid = elementId;
					if(House_GetOwnerID(houseid) == PlayerInfo[playerid][pID])
					{
							// REMEMBER TO EDIT /casa IF THIS DIALOG IS EDITED
						Dialog_Show(playerid, Dialog_House, DIALOG_STYLE_LIST, "Casa", "Apri/Chiudi Porta\nInventario\nDeposita Soldi\nRitira Soldi\nVendi\nVendi a Giocatore\nCambia Interior", "Continua", "Chiudi");
					}
				}
			}
		}
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

stock Character_Enter(playerid, elementId, E_ELEMENT_TYPE:type)
{
    if((type != ELEMENT_TYPE_BUILDING_ENTRANCE && type != ELEMENT_TYPE_HOUSE_ENTRANCE))
	   return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata di un edificio.");
    
    if(type == ELEMENT_TYPE_BUILDING_ENTRANCE)
    {
		if(Building_GetType(elementId) == BUILDING_TYPE_GARAGE)
			return Internal_EnterGarage(playerid, elementId);
		return Internal_EnterBuilding(playerid, elementId);
    }
    else if(type == ELEMENT_TYPE_HOUSE_ENTRANCE)
    {
	   return Internal_EnterHouse(playerid, elementId);
    }
    return 1;
}

static Internal_EnterGarage(playerid, buildingid)
{
	if(Building_IsLocked(buildingid))
		return GameTextForPlayer(playerid, "~r~Chiuso", 8000, 1);
	
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

	new vehicleid = GetPlayerVehicleID(playerid);
	
	SetPlayerInterior(playerid, interiorId);
	Character_SetVirtualWorld(playerid, world);
	
	if(vehicleid > 0)
	{
		LinkVehicleToInterior(vehicleid, interiorId);
		SetVehicleVirtualWorld(vehicleid, world);
		SetVehiclePos(vehicleid, x, y, z);
	}
	else
	{
		SetPlayerPos(playerid, x, y, z);
	}
	return 1;
}

static Internal_EnterBuilding(playerid, buildingid)
{
	if(IsPlayerInAnyVehicle(playerid))
		return 0;
	if(Building_IsLocked(buildingid))
		return GameTextForPlayer(playerid, "~r~Chiuso", 8000, 1);
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

	Character_AMe(playerid, "apre la porta ed entra.");

    Streamer_UpdateEx(playerid, x, y, z, world, interiorId);

    SetPlayerInterior(playerid, interiorId);
    Character_SetVirtualWorld(playerid, world);
    // Should I use Fading?
	SetPlayerPos(playerid, x, y, z);
	return 1;
}

static Internal_EnterHouse(playerid, houseid)
{
	if(IsPlayerInAnyVehicle(playerid))
		return 0;
	if(House_IsLocked(houseid))
	   return GameTextForPlayer(playerid, "~r~Chiuso", 8000, 1);
	new 
	   Float:x = 0.0, 
	   Float:y = 0.0, 
	   Float:z = 0.0,
	   interiorId = House_GetExitInterior(houseid),
	   world = House_GetExitWorld(houseid);
	House_GetExitPosition(houseid, x, y, z);

	Character_AMe(playerid, "apre la porta ed entra.");

    Streamer_UpdateEx(playerid, x, y, z, world, interiorId);

    SetPlayerInterior(playerid, interiorId);
    Character_SetVirtualWorld(playerid, world);
    // Should I use Fading?
	SetPlayerPos(playerid, x, y, z);
	return 1;
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
	
	if(vehicleid > 0)
	{
		LinkVehicleToInterior(vehicleid, interiorId);
		SetVehicleVirtualWorld(vehicleid, world);
		SetVehiclePos(vehicleid, x, y, z);
	}
	else
	{
		SetPlayerPos(playerid, x, y, z);
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
    SetPlayerPos(playerid, x, y, z);
	return 1;
}

static Internal_ExitBuilding(playerid, buildingid)
{
	if(IsPlayerInAnyVehicle(playerid))
		return 0;
	if(Building_IsLocked(buildingid))
		return GameTextForPlayer(playerid, "~r~Chiuso", 8000, 1), 0;
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
    SetPlayerPos(playerid, x, y, z);
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
    
    Character_GiveMoney(playerid, -BuildingInfo[buildingid][bPrice]);
    
    Building_SetOwner(buildingid, playerid);
    
    PlayerInfo[playerid][pBuildingKey] = BuildingInfo[buildingid][bID];

    SendFormattedMessage(playerid, COLOR_GREEN, "Hai acquistato questo edificio (%s) per $%d.", BuildingInfo[buildingid][bName], Building_GetPrice(buildingid));
    Building_Save(buildingid);
    Character_Save(playerid);
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

    House_Save(houseid);
    Character_Save(playerid);
    return 1;
}
