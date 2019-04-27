#include <YSI_Coding\y_hooks>

forward OnPlayerPickUpElementPickup(playerid, pickupid, elementid, E_ELEMENT_TYPE:type);

hook ClearData(playerid)
{
    printf("On CLear");
}

hook OnPlayerPickUpDynPickup(playerid, pickupid)
{
    if(!Character_IsLogged(playerid) || IsPlayerInRangeOfPoint(playerid, 5.0, 0.0, 0.0, 0.0))
	   return Y_HOOKS_BREAK_RETURN_0;
    new 
	   elementid = PickupInfo[pickupid][elementID],
	   E_ELEMENT_TYPE:type = PickupInfo[pickupid][elementType];

    CallLocalFunction("OnPlayerPickUpElementPickup", "iiii", playerid, pickupid, elementid, _:type);

    pLastPickup[playerid] = pickupid;
    return 1;
}

// 1239 -> Info
stock Pickup_Create(modelid, eId, Float:x, Float:y, Float:z, E_ELEMENT_TYPE:eType = ELEMENT_TYPE_NONE, worldid = -1, interiorid = -1)
{
    new pickupId = CreateDynamicPickup(modelid, 1, x, y, z, worldid, interiorid);
    PickupInfo[pickupId][elementType] = eType;
    PickupInfo[pickupId][elementID] = eId;
    return pickupId;
}

stock Pickup_Destroy(pickupid)
{
    if(!IsValidDynamicPickup(pickupid))
	   return 0;
    DestroyDynamicPickup(pickupid);
    PickupInfo[pickupid][elementType] = ELEMENT_TYPE_NONE;
    PickupInfo[pickupid][elementID] = -1;
    return 1;
}

stock Pickup_GetPosition(pickupid, &Float:x, &Float:y, &Float:z)
{
    Streamer_GetFloatData(STREAMER_TYPE_PICKUP, pickupid, E_STREAMER_X, x);
    Streamer_GetFloatData(STREAMER_TYPE_PICKUP, pickupid, E_STREAMER_Y, y);
    Streamer_GetFloatData(STREAMER_TYPE_PICKUP, pickupid, E_STREAMER_Z, z);
    return 1;
}

stock Pickup_GetVirtualWorld(pickupid)
{
	return Streamer_GetIntData(STREAMER_TYPE_PICKUP, pickupid, E_STREAMER_WORLD_ID);
}

stock Pickup_GetInfo(pickupid, &id, &E_ELEMENT_TYPE:type)
{
    if(pickupid == -1)
	   return 0;
    id = PickupInfo[pickupid][elementID];
    type = PickupInfo[pickupid][elementType];
    return 1;
}