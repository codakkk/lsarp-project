
enum E_VEHICLE_DATA
{
    vID,
	bool:vTemporary,
    vOwnerID,
    vOwnerName[MAX_PLAYER_NAME], // Currently not saved in DB
    vModel,
    vColor1,
    vColor2,
    Float:vX,
    Float:vY,
    Float:vZ,
    Float:vA,
    vLocked,
	vLastChopShopTime,
	vFaction,
	vSpawnExpiry
};

new stock 
    VehicleInfo[MAX_VEHICLES][E_VEHICLE_DATA],
    Iterator:Vehicles<MAX_VEHICLES>;

new bool:vDestroyed[MAX_PLAYERS char],
	pCurrentVehicleInventory[MAX_PLAYERS]; // Current Opened Vehicle Inventory Dialog

enum E_VEHICLE_RESTORE_DATA
{
    vSpawned,
    Float:vLastX,
    Float:vLastY,
    Float:vLastZ,
    Float:vLastA,
    Float:vLastHealth,
    vEngine
};
new 
	VehicleRestore[MAX_VEHICLES][E_VEHICLE_RESTORE_DATA],
	Float:VehicleFuel[MAX_VEHICLES];