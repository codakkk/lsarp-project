
#include <YSI_Coding\y_hooks>

static VehicleSirenObject[MAX_VEHICLES] = {0, ...};

hook OnVehicleSpawn(vehicleid)
{
    RemoveSirenFromVehicle(vehicleid);
    return 1;
}

CMD:sirena(playerid, params[])
{
	new vehicleid = GetPlayerVehicleID(playerid);

	if(vehicleid <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei in un veicolo.");

	new id;
	if(sscanf(params, "d", id) || !(0 <= id <= 2)) 
        return SendClientMessage(playerid, COLOR_GREY, "/sirena [0(rimuovi)/1/2]");

	if(id == 0)
	{
		if(!HasSiren(vehicleid)) 
            return SendClientMessage(playerid, COLOR_ERROR, "La sirena è già spenta.");

		RemoveSirenFromVehicle(vehicleid);

		Player_Info(playerid, "Sirena ~r~spenta~w~.");
		return 1;
	}

	if(HasSiren(vehicleid)) 
        return SendClientMessage(playerid, COLOR_ERROR, "La sirena è già accesa.");

	AddSirenToVehicle(vehicleid, id);

	Player_Info(playerid, "Sirena ~g~accesa~w~.");
	return 1;
}

static RemoveSirenFromVehicle(vehicleid)
{
    if(IsValidDynamicObject(VehicleSirenObject[vehicleid]))
        DestroyDynamicObject(VehicleSirenObject[vehicleid]);
    VehicleSirenObject[vehicleid] = 0;
}

static AddSirenToVehicle(vehicleid, type)
{
    new modelid = GetVehicleModel(vehicleid), Float:fSize[3], Float:fSeat[3];

	GetVehicleModelInfo(modelid, VEHICLE_MODEL_INFO_SIZE, fSize[0], fSize[1], fSize[2]);
    GetVehicleModelInfo(modelid, VEHICLE_MODEL_INFO_FRONTSEAT, fSeat[0], fSeat[1], fSeat[2]);

	VehicleSirenObject[vehicleid] = CreateDynamicObject(18646, 0.0, 0.0, 1000.0, 0.0, 0.0, 0.0);

	if(type == 1)
        AttachDynamicObjectToVehicle(VehicleSirenObject[vehicleid], vehicleid, -fSeat[0], fSeat[1], fSize[2] / 2.0, 0.0, 0.0, 0.0);
	else if(type == 2)
        AttachDynamicObjectToVehicle(VehicleSirenObject[vehicleid], vehicleid, 0.0, 0.75, 0.275, 0.0, 0.1, 0.0);
}

static HasSiren(vehicleid)
{
    return VehicleSirenObject[vehicleid] > 0;
}