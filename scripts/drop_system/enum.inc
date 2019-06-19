
#define MAX_DROP_TIME_AS_MIN		60 //
#define MAX_DROPS					500
#define for_drops(%0) for_list(%0 : DropList)

enum E_DROP_INFO // _: for tag mismatch thing
{
	dCreated,
    Float:dX,
    Float:dY,
    Float:dZ,
	Float:dRotX,
	Float:dRotY,
	Float:dRotZ,
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