#define MAX_BUILDINGS		 (100)
#define MAX_BUILDING_NAME	  (64)
#define MAX_WELCOME_TEXT_LENGTH (120)
#define BUILDING_START_WORLD	(5000)

enum //E_BUILDING_TYPE
{
	BUILDING_TYPE_STORE = 0,
	BUILDING_TYPE_PAYCHECK,
	// Insert here other types

	BUILDING_TYPE_LAST
}

enum E_BUILDING_INFO
{
    bID,
    bExists,
    bName[MAX_BUILDING_NAME],
    bWelcomeText[MAX_WELCOME_TEXT_LENGTH],
    Float:bEnterX,
    Float:bEnterY,
    Float:bEnterZ,
    bEnterInterior,
    bEnterWorld,
    Float:bExitX,
    Float:bExitY,
    Float:bExitZ,
    bExitInterior,
	bExitWorld,
    bOwnable,
    bOwnerID,
    bOwnerName[MAX_PLAYER_NAME],
    bPrice,
    bLocked,
	bFaction,
	bType,
    // Volatile
    bEnterPickupID,
    Text3D:bEnter3DText,
    bExitPickupID,
	Text3D:bExit3DText,
};

new
    BuildingInfo[MAX_BUILDINGS][E_BUILDING_INFO],
    Iterator:Buildings<MAX_BUILDINGS>;