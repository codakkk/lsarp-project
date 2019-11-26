
#include <YSI_Coding\y_hooks>

flags:porta(CMD_ALIVE_USER);
CMD:porta(playerid, params[])
{
    new Building:building, House:house;
    if(Character_TryGetBuilding(playerid, building) || Character_TryGetHouse(playerid, house))
    {
        if(!Player_IsAdminDuty(playerid))
        {
            if(House_IsValid(house) && !Character_CanUseHouseMenu(playerid, house))
                return 0;
            if(Building_IsValid(building) && !Character_CanUseBuildingMenu(playerid, building, 10))
                return 0;
        }
        Character_OpenNearDoor(playerid);
    }
    return 1;
}

stock Character_OpenNearDoor(playerid)
{
    new 
        bool:isExterior, EntityType:entityType, entityid,
        Float:x, Float:y, Float:z, Float:rotX, Float:rotY, Float:rotZ
    ;
    if(Character_GetFurnitureWorld(playerid, isExterior, entityType, entityid, .onlyAllowed=false, .worldAllowed=true))
    {
        new List:furnitures = Furniture_GetByEntityFilter(entityType, entityid, isExterior);
        for_list(i : furnitures)
        {
            new objectid = iter_get(i);
            printf("%d", objectid);
            
            GetDynamicObjectPos(objectid, x, y, z);

            new modelid = Streamer_GetIntData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_MODEL_ID);
            
            if(!IsPlayerInRangeOfPoint(playerid, 2.0, x, y, z) || !IsOpenableDoor(modelid))
                continue;
            
            Furniture_SetOpened(objectid, !Furniture_IsOpened(objectid));
            
            GetDynamicObjectRot(objectid, rotX, rotY, rotZ);
            
            rotZ = (Furniture_IsOpened(objectid)) ? rotZ - 90 : rotZ + 90;
            SetDynamicObjectRot(objectid, rotX, rotY, rotZ);
            break;
        }
    }
}

stock bool:IsOpenableDoor(modelid)
{
    return modelid == 3089 || modelid == 2875 || modelid == 2664 || modelid == 19302 || modelid == 14819 || modelid == 3061 || modelid == 19303 || modelid == 1532 || modelid == 19860 || modelid == 2911 || modelid == 1507 || modelid == 1505 || modelid == 1493 || modelid == 1495 || modelid == 2948 || modelid == 2947 || modelid == 2946 || modelid == 2944 || modelid == 2959 || modelid == 3109 || modelid == 3029 || modelid == 1566 || modelid == 1569 || modelid == 2877 || modelid == 19799 || modelid == 19875 || modelid == 19857 || modelid == 19858 || modelid == 19859 || modelid == 1535 || modelid == 2634 || modelid == 557 || modelid == 1536 || modelid == 1522 || modelid == 1506 || modelid == 1504 || modelid == 1501 || modelid == 1498 || modelid == 1497 || modelid == 1496 || modelid == 2924 || modelid == 2970|| modelid == 2949;
}

// piscio