
#define MAX_DROP_TIME_AS_MIN		1 //
#define MAX_DROPS					100
#define for_drops(%0) for_list(%0 : DropList)

enum _:E_DROP_INFO // _: for tag mismatch thing
{
	dCreated,
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
	DropInfo[MAX_DROPS][E_DROP_INFO],
	Iterator:DropsIterator<MAX_DROPS>;