

#if defined _included_PICKUP
    #endinput
#endif 

#define _included_PICKUP

#include <YSI_Coding\y_hooks>

// We call them "elements" to avoid problems with Pickups ID
enum E_ELEMENT_TYPE
{
    ELEMENT_TYPE_NONE,
    ELEMENT_TYPE_DEALERSHIP,
    ELEMENT_TYPE_BUILDING_ENTRANCE,
    ELEMENT_TYPE_BUILDING_EXIT,
    ELEMENT_TYPE_DROP,
    ELEMENT_TYPE_HOUSE_ENTRANCE,
    ELEMENT_TYPE_HOUSE_EXIT,
    ELEMENT_TYPE_LOOTZONE,
	ELEMENT_TYPE_ARREST,
	ELEMENT_TYPE_JAIL_EXIT,
	ELEMENT_TYPE_CHOPSHOP,
    ELEMENT_TYPE_ATM
};

static enum e_PICKUP_DATA
{
    ID,
    E_ELEMENT_TYPE:Type
};

static 
    TempData[e_PICKUP_DATA],
    szTempData = e_PICKUP_DATA,
    LastPickup[MAX_PLAYERS] = {0, ...};

forward OnPlayerPickUpElementPickup(playerid, pickupid, elementid, E_ELEMENT_TYPE:type);

hook OnPlayerPickUpDynPickup(playerid, pickupid)
{
    if(!Character_IsLogged(playerid) || IsPlayerInRangeOfPoint(playerid, 5.0, 0.0, 0.0, 0.0))
	   return Y_HOOKS_BREAK_RETURN_0;
    
    Streamer_GetArrayData(STREAMER_TYPE_PICKUP, pickupid, E_STREAMER_EXTRA_ID, TempData, szTempData);
    
    LastPickup[playerid] = pickupid;

	CallLocalFunction("OnPlayerPickUpElementPickup", "iiii", playerid, pickupid, TempData[ID], _:TempData[Type]);
    return 1;
}

// 1239 -> Info
stock Pickup_Create(modelid, eId, Float:x, Float:y, Float:z, E_ELEMENT_TYPE:eType = ELEMENT_TYPE_NONE, worldid = -1, interiorid = -1)
{
    new pickupId = CreateDynamicPickup(modelid, 1, x, y, z, worldid, interiorid);
    TempData[ID] = eId;
    TempData[Type] = eType;
    Streamer_SetArrayData(STREAMER_TYPE_PICKUP, pickupId, E_STREAMER_EXTRA_ID, TempData, szTempData);
    return pickupId;
}

stock Pickup_Destroy(pickupid)
{
    if(!IsValidDynamicPickup(pickupid))
	   return 0;
    DestroyDynamicPickup(pickupid);
    return 1;
}

stock Pickup_GetPosition(pickupid, &Float:x, &Float:y, &Float:z)
{
    if(!IsValidDynamicPickup(pickupid))
        return 0;
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
    Streamer_GetArrayData(STREAMER_TYPE_PICKUP, pickupid, E_STREAMER_EXTRA_ID, TempData, szTempData);
    id = TempData[ID];
    type = TempData[Type];
    return 1;
}

stock Character_GetNearestPickup(playerid, &elementid, &E_ELEMENT_TYPE:type)
{
    if(!IsPlayerConnected(playerid) || !Character_IsLogged(playerid) || LastPickup[playerid] == -1)
        return 0;
    
    new Float:x, Float:y, Float:z, pickupid = LastPickup[playerid];
    Pickup_GetPosition(pickupid, x, y, z);

    if(GetPlayerDistanceFromPoint(playerid, x, y, z) >= 2.0)
        return 0;
    
    return Pickup_GetInfo(pickupid, elementid, type);
}

stock Character_IsNearPickupType(playerid, E_ELEMENT_TYPE:type)
{
    new e, E_ELEMENT_TYPE:t;
    return Character_GetNearestPickup(playerid, e, t) && t == type;
}

// Is player near one of these pickup @types
// Returns 0 if none provided.
stock Character_IsNearPickupTypes(playerid, {E_ELEMENT_TYPE}:...)
{
    new args = numargs();
    if(args > 1)
    {
        new elementId, E_ELEMENT_TYPE:t;
        if(Character_GetNearestPickup(playerid, elementId, t))
        {
            for(new i = 1; i < args; ++i)
            {
                if(_:t == getarg(i))
                    return 1;
            }
        }
    }
    return 0;
}