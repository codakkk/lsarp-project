#include <YSI\y_hooks>

hook OnPlayerPickUpElmPickup(playerid, pickupid, elementid, E_ELEMENT_TYPE:type)
{
    if(type != ELEMENT_TYPE_BUILDING_ENTRANCE && type != ELEMENT_TYPE_BUILDING_EXIT)
        return 1;
    if(type == ELEMENT_TYPE_BUILDING_ENTRANCE)
    {
        new buildingid = elementid;
        new string[128];
        if(AccountInfo[playerid][aAdmin] > 1)
        {
            format(string, sizeof(string), "%s~r~ID:~w~ %d~n~", string, elementid);
        }
        if(Building_IsOwnable(buildingid))
        {
            if(Building_GetOwnerID(buildingid) != 0)
            {
                format(string, sizeof(string), "%s~y~%s~n~~y~Proprietario: %s.~n~", string, Building_GetName(buildingid), Building_GetOwnerName(buildingid));
            }
            else
            {
                format(string, sizeof(string), "%s~y~%s~n~~g~Prezzo:~w~$%d~n~", string, Building_GetName(buildingid), Building_GetPrice(buildingid));
            }
        }
        else
        {
            format(string, sizeof(string), "%s~y~%s~n~", string, Building_GetName(buildingid));
        }
        GameTextForPlayer(playerid, string, 2000, 5);
    }
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    new pickupid = pLastPickup[playerid];
    if(pickupid != -1 && PRESSED(KEY_SECONDARY_ATTACK))
    {
        // Should I write an "OnInteract" callback?
        new 
            elementId, 
            E_ELEMENT_TYPE:type;
        Pickup_GetInfo(pickupid, elementId, type);
        if(type == ELEMENT_TYPE_BUILDING_ENTRANCE)
        {
            Player_Enter(playerid, pickupid, elementId, type);
        }
        else if(type == ELEMENT_TYPE_BUILDING_EXIT)
        {
            Player_Exit(playerid, pickupid, elementId, type);
        }
    }
    return 1;
}

stock Player_Enter(playerid, pickupid, elementId, E_ELEMENT_TYPE:type)
{
    if(!IsPlayerInRangeOfPickup(playerid, pickupid, 2.5))
        return 0;
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
        world = elementId;
        locked = BuildingInfo[elementId][bLocked];
    }
    if(locked)
        return GameTextForPlayer(playerid, "~r~Chiuso", 8000, 1);
    
    Character_AMe(playerid, "apre la porta ed entra");
    Streamer_UpdateEx(playerid, x, y, z, world, interiorId);
    SetPlayerInterior(playerid, interiorId);
    SetPlayerVirtualWorld(playerid, world);
    SetPlayerPos(playerid, x, y, z);
    return 1;
}

stock Player_Exit(playerid, pickupid, elementId, E_ELEMENT_TYPE:type)
{
    if(!IsPlayerInRangeOfPickup(playerid, pickupid, 2.5))
        return 0;
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
    if(locked)
        return GameTextForPlayer(playerid, "~r~Chiuso", 8000, 1);
    Character_AMe(playerid, "apre la porta ed esce");
    Streamer_UpdateEx(playerid, x, y, z, world, interiorId);
    SetPlayerInterior(playerid, interiorId);
    SetPlayerVirtualWorld(playerid, world);
    SetPlayerPos(playerid, x, y, z);
    return 1;
}