#define MAX_BUILDINGS           (100)
#define MAX_BUILDING_NAME       (64)
#define MAX_WELCOME_TEXT_LENGTH (120)

enum E_BUILDING_INFO
{
    bID,
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
    bOwnable,
    bOwnerID,
    bOwnerName[MAX_PLAYER_NAME],
    bPrice,
    bLocked,
    bExists,
    // Volatile
    bEnterPickupID,
    Text3D:bEnter3DText,
    bExitPickupID
};

new
    BuildingInfo[MAX_BUILDINGS][E_BUILDING_INFO],
    Iterator:Buildings<MAX_BUILDINGS>;