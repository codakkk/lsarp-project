
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

stock bool:Drop_GetData(dropid, drop[E_DROP_INFO])
{
    if(list_get_arr_safe(DropList, dropid, drop))
        return true;
    return false;
}

