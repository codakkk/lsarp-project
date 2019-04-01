
#define MAX_DROP_TIME_AS_MIN        1 //
#define for_drops(%0) for_list(%0 : DropList)

enum _:E_DROP_INFO // _: for tag mismatch thing
{
    Float:dX,
    Float:dY,
    Float:dZ,
    dItem,
    dItemAmount,
    dItemExtra,
    String:dCreatedBy,
    dCreatedTime,
    dWorld,
    dInterior,
    dObject,
    dPickup
};
new 
    List:DropList;

hook OnGameModeInit()
{
    DropList = list_new();
}

stock bool:Drop_GetData(dropid, drop[E_DROP_INFO])
{
    if(list_get_arr_safe(DropList, dropid, drop))
        return true;
    return false;
}

hook GlobalSecondTimer(playerid)
{
    for(new i = 0; i < list_size(DropList); ++i)
    {
        new dropCreatedTime = gettime() - Drop_GetCreatedTime(i);

        if( dropCreatedTime >= MAX_DROP_TIME_AS_MIN * 60 * 1000)
        {
            Drop_Destroy(i);
        }
    }
}

stock Drop_Create(Float:x, Float:y, Float:z, world, interior, itemid, amount, extra, String:createdBy)
{
    if(itemid == 0 || amount == 0)
        return -1;
    new Drop[E_DROP_INFO];
    Drop[dX] = x;
    Drop[dY] = y;
    Drop[dZ] = z;
    Drop[dWorld] = world;
    Drop[dInterior] = interior;
    Drop[dItem] = itemid;
    Drop[dItemAmount] = amount;
    Drop[dItemExtra] = extra;
    Drop[dCreatedBy] = createdBy;
    Drop[dCreatedTime] = gettime();
    Drop[dObject] = CreateDynamicObject(ServerItem_GetModelID(itemid), x, y, z, 0.0, 0.0, 0.0, world, interior);
    Drop[dPickup] = Pickup_Create(1007, list_size(DropList), x, y, z, ELEMENT_TYPE_DROP, world, interior);
    new id = list_add_arr(DropList, Drop);
    return id;
}

stock Drop_Destroy(dropid)
{
    new Drop[E_DROP_INFO];
    if(Drop_GetData(dropid, Drop))
    {
        DestroyDynamicObject(Drop[dObject]);
        Pickup_Destroy(Drop[dPickup]);
        
        list_remove_deep(DropList, dropid);
    }

    return 1;
}

stock Drop_GetPosition(dropid, &Float:x, &Float:y, &Float:z)
{
    new Drop[E_DROP_INFO];
    Drop_GetData(dropid, Drop);
    x = Drop[dX];
    y = Drop[dY];
    z = Drop[dZ];
}

stock Drop_GetItem(dropid)
{
    new Drop[E_DROP_INFO];
    Drop_GetData(dropid, Drop);
    return Drop[dItem];
}

stock Drop_GetItemAmount(dropid)
{
    new Drop[E_DROP_INFO];
    Drop_GetData(dropid, Drop);
    return Drop[dItemAmount];
}

stock Drop_GetItemExtra(dropid)
{
    new Drop[E_DROP_INFO];
    Drop_GetData(dropid, Drop);
    return Drop[dItemExtra];
}

stock String:Drop_GetCreatedByStr(dropid)
{
    new Drop[E_DROP_INFO];
    Drop_GetData(dropid, Drop);
    return Drop[dCreatedBy];
}

stock Drop_GetCreatedBy(dropid, name[])
{
    new Drop[E_DROP_INFO];
    Drop_GetData(dropid, Drop);
    str_get(Drop[dCreatedBy], name); 
}

stock Drop_GetCreatedTime(dropid)
{
    new Drop[E_DROP_INFO];
    Drop_GetData(dropid, Drop);
    return Drop[dCreatedTime];
}