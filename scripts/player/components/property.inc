#include <YSI_Coding\y_hooks>

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(Account_HasHotKeysEnabled(playerid) && Character_IsAlive(playerid))
	{
		new elementId, E_ELEMENT_TYPE:type, pickupid = Character_GetLastPickup(playerid);
		if(Pickup_GetInfo(pickupid, elementId, type) && !IsPlayerInAnyVehicle(playerid) && IsPlayerInRangeOfPickup(playerid, pickupid, 2.0))
		{
			if(PRESSED(KEY_YES))
			{
				// Should I write an "OnInteract" callback?
				if(type == ELEMENT_TYPE_BUILDING_ENTRANCE || type == ELEMENT_TYPE_HOUSE_ENTRANCE)
				{
					Character_Enter(playerid, pickupid, elementId, type);
				}
				else if(type == ELEMENT_TYPE_BUILDING_EXIT || type == ELEMENT_TYPE_HOUSE_EXIT)
				{
					Character_Exit(playerid, pickupid, elementId, type);
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

stock Character_Enter(playerid, pickupid, elementId, E_ELEMENT_TYPE:type)
{
    if(!IsPlayerInRangeOfPickup(playerid, pickupid, 2.5) || (type != ELEMENT_TYPE_BUILDING_ENTRANCE && type != ELEMENT_TYPE_HOUSE_ENTRANCE))
	   return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata di un edificio.");
    new 
	   Float:x = 0.0, 
	   Float:y = 0.0, 
	   Float:z = 0.0,
	   interiorId = 0,
	   world = 0,
	   locked = 0;
    if(type == ELEMENT_TYPE_BUILDING_ENTRANCE)
    {
		x = BuildingInfo[elementId][bExitX]; 
		y = BuildingInfo[elementId][bExitY]; 
		z = BuildingInfo[elementId][bExitZ];
		interiorId = BuildingInfo[elementId][bExitInterior];
		world = BuildingInfo[elementId][bExitWorld];
		locked = BuildingInfo[elementId][bLocked];
		if(!locked)
		{
			new String:str = Building_GetWelcomeTextStr(elementId);
			if(str_len(str) > 0)
				SendClientMessageStr(playerid, COLOR_GREEN, str);	
		}
    }
    else if(type == ELEMENT_TYPE_HOUSE_ENTRANCE)
    {
	   House_GetExitPosition(elementId, x, y, z);
	   interiorId = House_GetExitInterior(elementId);
	   world = House_GetExitWorld(elementId);
	   locked = House_IsLocked(elementId);
    }
    if(locked)
	   return GameTextForPlayer(playerid, "~r~Chiuso", 8000, 1);
    
    Character_AMe(playerid, "apre la porta ed entra.");
    Streamer_UpdateEx(playerid, x, y, z, world, interiorId);
    SetPlayerInterior(playerid, interiorId);
    Character_SetVirtualWorld(playerid, world);
    SetPlayerPos(playerid, x, y, z);
    return 1;
}

stock Character_Exit(playerid, pickupid, elementId, E_ELEMENT_TYPE:type)
{
    if(!IsPlayerInRangeOfPickup(playerid, pickupid, 2.5) || (type != ELEMENT_TYPE_BUILDING_EXIT && type != ELEMENT_TYPE_HOUSE_EXIT))
	   return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'uscita di un edificio.");
    new 
	   Float:x = 0.0, 
	   Float:y = 0.0, 
	   Float:z = 0.0,
	   interiorId = 0,
	   world = 0,
	   locked = 0;
    
    if(type == ELEMENT_TYPE_BUILDING_EXIT)
    {
	   x = BuildingInfo[elementId][bEnterX]; 
	   y = BuildingInfo[elementId][bEnterY]; 
	   z = BuildingInfo[elementId][bEnterZ];
	   interiorId = BuildingInfo[elementId][bEnterInterior];
	   world = BuildingInfo[elementId][bEnterWorld];
	   locked = BuildingInfo[elementId][bLocked];
    }
    else if(type == ELEMENT_TYPE_HOUSE_EXIT)
    {
	   House_GetEnterPosition(elementId, x, y, z);
	   interiorId = House_GetEnterInterior(elementId);
	   world = House_GetEnterWorld(elementId);
	   locked = House_IsLocked(elementId);
    }
    if(locked)
	   return GameTextForPlayer(playerid, "~r~Chiuso", 8000, 1);
    Character_AMe(playerid, "apre la porta ed esce.");
    Streamer_UpdateEx(playerid, x, y, z, world, interiorId);
    SetPlayerInterior(playerid, interiorId);
    Character_SetVirtualWorld(playerid, world);
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

    if(Character_HasHouseKey(playerid))
	   return SendClientMessage(playerid, COLOR_ERROR, "Sei già proprietario di una casa.");

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
