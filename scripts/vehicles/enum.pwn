
enum E_VEHICLE_DATA
{
    vID,
    vOwnerID,
    vOwnerName[MAX_PLAYER_NAME], // Currently not saved in DB
    vModel,
    vColor1,
    vColor2,
    Float:vX,
    Float:vY,
    Float:vZ,
    Float:vA,
	vLastChopShopTime,
	vFaction,
	vSpawnExpiry
};

new stock 
    VehicleInfo[MAX_VEHICLES][E_VEHICLE_DATA],
    Iterator:Vehicles<MAX_VEHICLES>;

new pCurrentVehicleInventory[MAX_PLAYERS]; // Current Opened Vehicle Inventory Dialog

enum E_VEHICLE_RESTORE_DATA
{
    //vSpawned,
    Float:vLastX,
    Float:vLastY,
    Float:vLastZ,
    Float:vLastA,
    Float:vLastHealth,
};
new 
	VehicleRestore[MAX_VEHICLES][E_VEHICLE_RESTORE_DATA],
	Float:VehicleFuel[MAX_VEHICLES];

enum e_Bit1_VehicleData
{
	e_vSpawned,
	e_vDestroyed,
	e_vLocked,
	e_vTemporary,
	e_vEngine,
	e_vDespawn
};
new BitArray:gVehicleBitState[e_Bit1_VehicleData]<MAX_VEHICLES>;