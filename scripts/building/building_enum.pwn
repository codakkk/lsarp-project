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

enum E_INTERIORS_BUILDING
{
	iBuildingInt,
	Float:iBuildingX,
	Float:iBuildingY,
	Float:iBuildingZ
};
new const Float:gBuildingInteriors[][E_INTERIORS_BUILDING] =
{
	{2, -219.7809, 70.5098, 1320.2789},
	{17, -25.884498, -185.868988, 1003.546875},
	{9, 369.579528, -4.487294, 1001.858886},
	{5, 372.3708, -133.4988, 1001.4920},
	{10, 375.962463, -65.816848, 1001.507812},
	{15, 207.4636, -111.2659, 1005.1328},
	{11, 501.980987, -69.150199, 998.757812},
	{1, 286.148986, -40.644397, 1001.515625}
};