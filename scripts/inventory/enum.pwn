#define for_inventory(%0:%1) for_list(%0:%1)

#define MAX_ITEMS_IN_SERVER		   (100)
#define MAX_ITEM_NAME			    (32)
#define MAX_ITEMS_PER_PLAYER		  (50)
#define MAX_ITEMS_PER_VEHICLE		 (50)
#define INVALID_ITEM_ID			  (-1)
#define PLAYER_INVENTORY_START_SIZE	(10)

#define ServerItem_GetName(%0) (ServerItem[%0][sitemName])



enum // Error Type
{
	//INVENTORY_RESULT_ENUM = 0, // NOT TRUE: -50 because we need positive numbers for diff.
	INVENTORY_INVALID,
	INVENTORY_ADD_SUCCESS,
	INVENTORY_NO_SPACE,
	INVENTORY_INVALID_SLOTID,
	INVENTORY_FAILED_INVALID_ITEM,
	INVENTORY_FAILED_INVALID_AMOUNT,
	INVENTORY_ADD_INVALID_ITEM,
	INVENTORY_DECREASE_SUCCESS,
	INVENTORY_DECREASE_FAILED,
};

enum ITEM_TYPE
{
	ITEM_TYPE_NONE = 0,
	ITEM_TYPE_MATERIAL,
	ITEM_TYPE_WEAPON,
	ITEM_TYPE_AMMO,
	ITEM_TYPE_FOOD,
	ITEM_TYPE_DRINK,
	ITEM_TYPE_MEDIK,
	ITEM_TYPE_BAG,
	ITEM_TYPE_FUEL,
	ITEM_TYPE_DRUG
};

enum EXTRA_TYPE_ID
{
	EXTRA_BAG_SIZE = 0
};

enum E_SERVER_ITEM_DATA
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
	ServerItem[MAX_ITEMS_IN_SERVER][E_SERVER_ITEM_DATA],
	Iterator:ServerItems<MAX_ITEMS_IN_SERVER>
;

// Common Enum for inventories (characters, houses, vehicles, etc)
enum _:E_ITEM_DATA
{
	gInvItem,
	gInvAmount,
	gInvExtra
};

// Player Inventory
new
	Map:PlayerInventory, // Map<playerid, Inventory:PINV>
	pInventoryBag[MAX_PLAYERS];


new stock
	Map:VehicleInventory, // <Key: vehicle_id, Value: List:Items>
	Iterator:VehicleItemsSlot[MAX_VEHICLES]<MAX_ITEMS_PER_VEHICLE>;

new stock 
	Map:HouseInventory;
// GLOBAL ITEMS ID
new stock
	gItem_Hamburger,
	gItem_Beans,
	gItem_EnergyBar,
	gItem_DriedMeat,
	gItem_PeanutButter,
	gItem_CannedTuna,
	gItem_RawMeat,
	gItem_RawFish,

	gItem_BottleOfWater,
	gItem_SmallBottleOfWater,
	gItem_Milk,
	gItem_RainWater,

	gItem_RationK,
	gItem_Morfine,
	gItem_Bandages,
	gItem_Medikit,

	gItem_SmallBackPack,
	gItem_BigBackPack,

	// AMMOS
	gItem_LightAmmo,
	gItem_BuckShotAmmo,
	gItem_HeavyAmmo,
	gItem_RifleAmmo,

	// Fuel
	gItem_SmallFuelTank,
	gItem_MediumFuelTank,
	gItem_BigFuelTank
;