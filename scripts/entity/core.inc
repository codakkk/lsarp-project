
#if defined _included_ENTITY_CORE
    #endinput
#endif 

#define _included_ENTITY_CORE 

enum EntityType
{
    ENTITY_TYPE_NONE = 0,
    ENTITY_TYPE_PLAYER = 1,
    ENTITY_TYPE_HOUSE = 2,
    ENTITY_TYPE_BUILDING = 3,
	ENTITY_TYPE_VEHICLE = 4,
    ENTITY_TYPE_DROP = 5,
	ENTITY_TYPE_BAG = 6,
    ENTITY_TYPE_WORLD = 7, // Probably used only by Furniture System.
}

#include <YSI_Coding\y_hooks>

// Used with hook function
stock bool:Entity_GetWorldPosition(EntityType:entityType, entityId, &Float:x, &Float:y, &Float:z)
{
    #pragma unused entityType, entityId, x, y, z
    return false;
}

// Must use hook function
stock Entity_GetVirtualWorld(EntityType:entityType, GLOBAL_TAG_TYPES:entityId)
{
    if(entityType == ENTITY_TYPE_PLAYER && Character_IsLogged(entityId))
        return Character_GetVirtualWorld(entityId);
    else if(entityType == ENTITY_TYPE_HOUSE && House_IsValid(House:entityId))
        return House_GetExitWorld(House:entityId);
    else if(entityType == ENTITY_TYPE_BUILDING && Building_IsValid(Building:entityId))
        return Building_GetExitWorld(Building:entityId);
    return 0;
}

stock Entity_GetInterior(EntityType:entityType, GLOBAL_TAG_TYPES:entityId)
{
    if(entityType == ENTITY_TYPE_PLAYER && Character_IsLogged(entityId))
        return GetPlayerInterior(entityId);
    else if(entityType == ENTITY_TYPE_HOUSE && House_IsValid(House:entityId))
        return House_GetEnterInterior(House:entityId);
    else if(entityType == ENTITY_TYPE_BUILDING && Building_IsValid(Building:entityId))
        return Building_GetEnterInterior(Building:entityId);
    return 0;
}

// Must use hook function
stock Entity_GetUniqueID(EntityType:entityType, GLOBAL_TAG_TYPES:entityId)
{
    if(entityType == ENTITY_TYPE_PLAYER && Character_IsLogged(entityId))
        return Character_GetID(entityId);
    else if(entityType == ENTITY_TYPE_HOUSE && House_IsValid(House:entityId))
        return House_GetID(House:entityId);
    else if(entityType == ENTITY_TYPE_BUILDING && Building_IsValid(Building:entityId))
        return Building_GetID(Building:entityId);
    return 0;
}