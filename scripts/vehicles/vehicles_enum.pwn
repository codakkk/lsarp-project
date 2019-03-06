
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
    vLocked
};

new stock 
    VehicleInfo[MAX_VEHICLES][E_VEHICLE_DATA],
    Iterator:Vehicles<MAX_VEHICLES>;

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
new VehicleRestore[MAX_VEHICLES][E_VEHICLE_RESTORE_DATA];