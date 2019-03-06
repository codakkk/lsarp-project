#define MAX_ITEMS_IN_SERVER             (100)
#define MAX_ITEM_NAME                   (32)
#define MAX_ITEMS_PER_PLAYER            (10)
#define INVALID_ITEM_ID                 (-1)
#define PLAYER_INVENTORY_START_SIZE     (10)

#define ServerItem_GetName(%0) (ServerItem[%0][sitemName])
#define ServerItem_GetType(%0) (ServerItem[%0][sitemType])
#define ServerItem_GetMaxStack(%0) (ServerItem[%0][sitemMaxStack])


enum // Error Type
{
    INVENTORY_RESULT_ENUM = -50, // -50 because we need positive numbers for diff.
    INVENTORY_NO_SPACE,
    INVENTORY_INVALID_SLOTID,
    INVENTORY_ADD_SUCCESS,
    INVENTORY_FAILED_INVALID_ITEM,
    INVENTORY_FAILED_INVALID_AMOUNT,
    INVENTORY_ADD_INVALID_ITEM,
    INVENTORY_DECREASE_SUCCESS,
    INVENTORY_DECREASE_FAILED,
};

enum ITEM_TYPE
{
    ITEM_TYPE_NONE,
    ITEM_TYPE_WEAPON,
    ITEM_TYPE_FOOD,
    ITEM_TYPE_DRINK,
    ITEM_TYPE_MEDIK,
    ITEM_TYPE_BAG,
};

enum EXTRA_TYPE_ID
{
    EXTRA_BAG_SIZE = 0,
};

enum E_INVENTORY_ITEMS_DATA
{
    sitemID,
    sitemName[MAX_ITEM_NAME],
    ITEM_TYPE:sitemType,
    sitemUnique,
    sitemMaxStack,
    sitemExtraData[5],
    sitemModelID
};
new     
    ServerItem[MAX_ITEMS_IN_SERVER][E_INVENTORY_ITEMS_DATA],
    Iterator:ServerItems<MAX_ITEMS_IN_SERVER>,
    // Global items id.
    gItem_RationK;

enum E_PLAYER_INVENTORY_DATA
{
    pInvItem,
    pInvAmount,
    pInvExtra
};
new
    PlayerInventory[MAX_PLAYERS][MAX_ITEMS_PER_PLAYER][E_PLAYER_INVENTORY_DATA],
    Iterator:PlayerItemsSlot[MAX_PLAYERS]<MAX_ITEMS_PER_PLAYER>,
    pInventoryBag[MAX_PLAYERS],
    pInventoryListItem[MAX_PLAYERS][MAX_ITEMS_PER_PLAYER];

