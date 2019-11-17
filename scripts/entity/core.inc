
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
	ENTITY_TYPE_BAG = 6
}

#include <YSI_Coding\y_hooks>

// Used with hook function
stock bool:Entity_GetWorldPosition(EntityType:entityType, entityId, &Float:x, &Float:y, &Float:z)
{
    #pragma unused entityType, entityId, x, y, z
    return false;
}