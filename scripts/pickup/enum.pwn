
enum E_PICKUP_DATA
{
    elementID,
    E_ELEMENT_TYPE:elementType
};
new     
    PickupInfo[4096 /* Max Pickups in SAMP */][E_PICKUP_DATA];

// We call them "elements" to avoid problems with Pickups ID
enum E_ELEMENT_TYPE
{
    ELEMENT_TYPE_NONE,
    ELEMENT_TYPE_DEALERSHIP,
    ELEMENT_TYPE_BUILDING_ENTRANCE,
    ELEMENT_TYPE_BUILDING_EXIT,
    ELEMENT_TYPE_DROP,
};