#include <YSI_Coding\y_hooks>

/*ptask OnVehicleTimer()[120000]
{
	foreach(new vehicleid : Vehicles)
	{
		if(!VehicleInfo[vehicleid][vModel])
			continue;

		VehicleInfo[vehicleid][vFuel] -= 1;
	}
}*/

hook OnPlayerCharacterLoad(playerid)
{
    Character_LoadVehicles(playerid);
    return 1;
}

hook OnVehicleSpawn(vehicleid)
{
    printf("Hook OnVehicleSpawn %d", vehicleid);
    if(VehicleInfo[vehicleid][vModel] != 0)
	   Vehicle_Reload(vehicleid);
    return 1;
}

hook OnVehicleDeath(vehicleid, killerid)
{
    printf("Hook OnVehicleDeath %d", vehicleid);
    gRespawnVehicle[vehicleid] = 1;
    return 1;
}

stock Vehicle_Reload(vehicleid)
{
    Vehicle_ResetStats(vehicleid);
	if(VehicleRestore[vehicleid][vSpawned])
	{   
		VehicleRestore[vehicleid][vSpawned] = 0;
		if(VehicleRestore[vehicleid][vEngine])
		{
			Vehicle_SetEngineOn(vehicleid);
		}
		SetVehiclePos(vehicleid, VehicleRestore[vehicleid][vLastX], VehicleRestore[vehicleid][vLastY], VehicleRestore[vehicleid][vLastZ]);
		SetVehicleZAngle(vehicleid, VehicleRestore[vehicleid][vLastA]);
	}
	else
	{
		SetVehiclePos(vehicleid, VehicleInfo[vehicleid][vX], VehicleInfo[vehicleid][vY], VehicleInfo[vehicleid][vZ]);
		SetVehicleZAngle(vehicleid, VehicleInfo[vehicleid][vA]);
		if(gRespawnVehicle[vehicleid])
		{
			gRespawnVehicle[vehicleid] = 0;
			SetVehicleHealth(vehicleid, 350.0);
		}
		else
		{
			SetVehicleHealth(vehicleid, 999.0);
		}
	}
	new Float:health;
	GetVehicleHealth(vehicleid, health);
	Vehicle_UpdateLockState(vehicleid);
	
	ChangeVehicleColor(vehicleid, VehicleInfo[vehicleid][vColor1], VehicleInfo[vehicleid][vColor2]);
}

stock Vehicle_Create(modelid, Float:x, Float:y, Float:z, Float:rotation, color1, color2, respawn_delay, addsiren=0)
{
    new vehicleid = CreateVehicle(modelid, x, y, z, rotation, color1, color2, respawn_delay, addsiren);
	VehicleInfo[vehicleid][vModel] = modelid;
	VehicleInfo[vehicleid][vColor1] = color1;
	VehicleInfo[vehicleid][vColor2] = color2;
	VehicleInfo[vehicleid][vX] = x;
	VehicleInfo[vehicleid][vY] = y;
	VehicleInfo[vehicleid][vZ] = z;
	VehicleInfo[vehicleid][vA] = rotation;
	VehicleFuel[vehicleid] = 100.0;
	VehicleInfo[vehicleid][vLocked] = 0;
    Iter_Add(Vehicles, vehicleid);
	return vehicleid;
}

stock Vehicle_Destroy(vehicleid)
{
	if(!IsValidVehicle(vehicleid))
		return 0;
    Vehicle_Reset(vehicleid);
	DestroyVehicle(vehicleid);
	Iter_Remove(Vehicles, vehicleid);
	return 1;
}

stock Vehicle_Reset(vehicleid)
{
	VehicleInfo[vehicleid][vID] = 0;
    VehicleInfo[vehicleid][vOwnerID] = 0;
    set(VehicleInfo[vehicleid][vOwnerName], "");
    VehicleInfo[vehicleid][vModel] = 0;
    VehicleInfo[vehicleid][vColor1] = 0;
    VehicleInfo[vehicleid][vColor2] = 0;
    VehicleInfo[vehicleid][vX] = 0.0;
    VehicleInfo[vehicleid][vY] = 0.0;
    VehicleInfo[vehicleid][vZ] = 0.0;
    VehicleInfo[vehicleid][vA] = 0.0;
    VehicleInfo[vehicleid][vLocked] = 0;
    VehicleRestore[vehicleid][vSpawned] = 0;
    VehicleRestore[vehicleid][vLastX] = 0.0;
    VehicleRestore[vehicleid][vLastY] = 0.0;
    VehicleRestore[vehicleid][vLastZ] = 0.0;
    VehicleRestore[vehicleid][vLastA] = 0.0;
    VehicleRestore[vehicleid][vLastHealth] = 0.0;
    VehicleRestore[vehicleid][vEngine] = 0;
	VehicleFuel[vehicleid] = 0.0;
    gVehicleDestroyTime[vehicleid] = 0;
	return 1;
}

stock Vehicle_IsValid(vehicleid)
{
	return IsValidVehicle(vehicleid);
}

stock Vehicle_Park(vehicleid, Float:nx, Float:ny, Float:nz, Float:na)
{
    if(!VehicleInfo[vehicleid][vID] || VehicleInfo[vehicleid][vModel] == 0)
	   return 0;
    VehicleInfo[vehicleid][vX] = nx;
    VehicleInfo[vehicleid][vY] = ny;
    VehicleInfo[vehicleid][vZ] = nz;
    VehicleInfo[vehicleid][vA] = na;
    Vehicle_Save(vehicleid);
    return 1;
}

stock Vehicle_GetOwnerID(vehicleid)
{
    if(!VehicleInfo[vehicleid][vID] || VehicleInfo[vehicleid][vModel] == 0)
	   return 0;
    return VehicleInfo[vehicleid][vOwnerID];
}

stock Vehicle_Lock(vehicleid)
{
    //if(VehicleInfo[vehicleid][vOwnerID] != PlayerInfo[playerid][pID])
	   //return SendClientMessage(playerid, COLOR_ERROR, "Non possiedi le chiavi di questo veicolo");
    if(Vehicle_IsLocked(vehicleid))
	   return 0;
    VehicleInfo[vehicleid][vLocked] = 1;
    Vehicle_UpdateLockState(vehicleid);
    return 1;
}

stock Vehicle_UnLock(vehicleid)
{
    //if(VehicleInfo[vehicleid][vOwnerID] != PlayerInfo[playerid][pID])
	   //return SendClientMessage(playerid, COLOR_ERROR, "Non possiedi le chiavi di questo veicolo");
    if(!Vehicle_IsLocked(vehicleid))
	   return 0;
    VehicleInfo[vehicleid][vLocked] = 0;
    Vehicle_UpdateLockState(vehicleid);
    return 1;
}

stock Vehicle_IsLocked(vehicleid)
{
    return VehicleInfo[vehicleid][vLocked];
}

stock Vehicle_IsLightOn(vehicleid)
{
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    return !IsABike(vehicleid) && lights;
}

stock Vehicle_IsLightOff(vehicleid)
{
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    return !IsABike(vehicleid) && !lights;
}

stock Vehicle_SetLightState(vehicleid, s)
{
    if(!IsABike(vehicleid))
	   return 0;
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    SetVehicleParamsEx(vehicleid, engine, s ? VEHICLE_PARAMS_ON : VEHICLE_PARAMS_OFF, alarm, doors, bonnet, boot, objective);
    return 1;
}

stock Vehicle_IsEngineOff(vehicleid)
{
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    return !IsABike(vehicleid) && !engine;
}

stock Vehicle_IsEngineOn(vehicleid)
{
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    return !IsABike(vehicleid) && engine;
}

stock Vehicle_SetEngineOn(vehicleid)
{
    if(IsABike(vehicleid))
	   return 0;
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    SetVehicleParamsEx(vehicleid, VEHICLE_PARAMS_ON, lights, alarm, doors, bonnet, boot, objective);
    return 1;
}

stock Vehicle_SetEngineOff(vehicleid)
{
    if(IsABike(vehicleid))
	   return 0;
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    SetVehicleParamsEx(vehicleid, VEHICLE_PARAMS_OFF, lights, alarm, doors, bonnet, boot, objective);
    return 1;
}

stock Vehicle_UpdateLockState(vehicleid)
{
    if(IsABike(vehicleid) || IsAMotorBike(vehicleid))
	   return 1;
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    SetVehicleParamsEx(vehicleid, engine, lights, alarm, VehicleInfo[vehicleid][vLocked], bonnet, boot, objective);
    if(VehicleInfo[vehicleid][vLocked])
	   Vehicle_SetDoorStatus(vehicleid, 0, 0, 0, 0);
    return 1;
}

stock Vehicle_IsTrunkOpened(vehicleid)
{
    if(IsABike(vehicleid) || IsAMotorBike(vehicleid))
	   return 1;
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    return boot;
}

stock Vehicle_OpenTrunk(vehicleid)
{
    if(IsABike(vehicleid) || IsAMotorBike(vehicleid))
	   return 1;
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    SetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, 1, objective);
    return 1;
}

stock Vehicle_CloseTrunk(vehicleid)
{
    if(IsABike(vehicleid) || IsAMotorBike(vehicleid))
	   return 1;
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    SetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, 0, objective);
    return 1;
}

stock Vehicle_ResetStats(vehicleid)
{
    SetVehicleParamsEx(vehicleid, 0, 0, 0, 0, 0, 0, 0);
}

stock Vehicle_DecodeDoors(vehicleid, &bonnet, &boot, &driver_door, &passenger_door)
{
    new panels, doors, lights, tires;
    GetVehicleDamageStatus(vehicleid, panels, doors, lights, tires);
    bonnet = doors & 7;
    boot = doors >> 8 & 7;
    driver_door = doors >> 16 & 7;
    passenger_door = doors >> 24 & 7;
}

stock Vehicle_SetDoorStatus(vehicleid, bonnet, boot, driver_door, passenger_door)
{
    new panels, doors, lights, tires, status = (bonnet | (boot << 8) | (driver_door << 16) | (passenger_door << 24));
    GetVehicleDamageStatus(vehicleid, panels, doors, lights, tires);
    UpdateVehicleDamageStatus(vehicleid, panels, status, lights, tires);
    return 1;
}

stock Vehicle_IsOwnerConnected(vehicleid)
{
    if(!VehicleInfo[vehicleid][vID] || !VehicleInfo[vehicleid][vOwnerID] || !VehicleInfo[vehicleid][vModel])
	   return 0;
    foreach(new i : Player)
    {
	   if(!Character_IsLogged(i))
		  continue;
	   if(VehicleInfo[vehicleid][vOwnerID] == PlayerInfo[i][pID])
		  return 1;
    }
    return 0;
}

stock Vehicle_Unload(vehicleid)
{
    new i = vehicleid;
    if(!VehicleInfo[vehicleid][vID] || VehicleInfo[i][vModel] == 0)
	   return 0;
	
	if(VehicleInfo[i][vOwnerID] != 0)
	{
    	CallLocalFunction(#OnPlayerVehicleUnLoaded, "d", vehicleid);
		Vehicle_Save(i);
	}
    
    Vehicle_Destroy(i);
    return 1;
}

stock Vehicle_Save(vehicleid)
{
    if(!VehicleInfo[vehicleid][vID] || VehicleInfo[vehicleid][vModel] == 0)
	   return 0;
    new 
	   Float:x, 
	   Float:y, 
	   Float:z, 
	   Float:a,
	   Float:hp,
	   vehSpawned = 0,
	   vehEngine = Vehicle_IsEngineOn(vehicleid);
    
    GetVehiclePos(vehicleid, x, y, z);
    GetVehicleZAngle(vehicleid, a);
    GetVehicleHealth(vehicleid, hp);

    if(x != 0.0 && y != 0.0 && z != 0.0)
    {
	   vehSpawned = 1;
    }

    new query[512];
    mysql_format(gMySQL, query, sizeof(query), "UPDATE `player_vehicles` SET \
    Model = '%d', \
    Color1 = '%d', Color2 = '%d', \
    X = '%f', Y = '%f', Z = '%f', Angle = '%f', \
    Locked = '%d', \
    LastX = '%f', LastY = '%f', LastZ = '%f', LastA = '%f', \
    LastHealth = '%f', \
    Spawned = '%d', \
    Engine = '%d', \
	Fuel = '%f' \
    WHERE ID = '%d'", 
    VehicleInfo[vehicleid][vModel],
    VehicleInfo[vehicleid][vColor1], VehicleInfo[vehicleid][vColor2],
    VehicleInfo[vehicleid][vX], VehicleInfo[vehicleid][vY], VehicleInfo[vehicleid][vZ], VehicleInfo[vehicleid][vA],
    VehicleInfo[vehicleid][vLocked],
    x, y, z, a,
    hp,
    vehSpawned, 
    vehEngine,
	VehicleFuel[vehicleid],
    VehicleInfo[vehicleid][vID]);
    mysql_tquery(gMySQL, query);
    CallLocalFunction(#OnPlayerVehicleSaved, "d", vehicleid);
    return 1;
}

// Deletes a vehicle (vehicleid). If it's a player vehicle, all their references are destroyed too (database, ecc).
stock Vehicle_Delete(vehicleid)
{
	if(!Vehicle_IsValid(vehicleid) || VehicleInfo[vehicleid][vID] == 0)
		return 0;
	new query[256];
	if(VehicleInfo[vehicleid][vOwnerID] != 0) // We suppose it's a player vehicle
	{
		mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `player_vehicles` WHERE ID = '%d' AND OwnerID = '%d'", Vehicle_GetID(vehicleid), Vehicle_GetOwnerID(vehicleid));
		mysql_tquery(gMySQL, query);

		mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `vehicle_inventory` WHERE VehicleID = '%d'", VehicleInfo[vehicleid][vID]);
		mysql_tquery(gMySQL, query);
	}

	Vehicle_Destroy(vehicleid);
	return 1;
}

stock Vehicle_IsOwnable(vehicleid)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;
	// IS FACTION VEHICLE?
	return 1;
}

stock Vehicle_GetID(vehicleid)
{
	//if(!IsValidVehicle(vehicleid))
		//return 0;
	return VehicleInfo[vehicleid][vID];
}

stock Vehicle_GetOwnerName(vehicleid, name[])
{
	if(!IsValidVehicle(vehicleid) || VehicleInfo[vehicleid][vOwnerID] == 0)
		return 0;
	set(name, VehicleInfo[vehicleid][vOwnerName]);
	return 1;
}

stock String:Vehicle_GetOwnerNameStr(vehicleid)
{
	if(!IsValidVehicle(vehicleid) || VehicleInfo[vehicleid][vOwnerID] == 0)
		return STRING_NULL;
	return str_new(VehicleInfo[vehicleid][vOwnerName]);
}

stock Vehicle_GetColor1(vehicleid)
{
	return VehicleInfo[vehicleid][vColor1];
}

stock Vehicle_GetColor2(vehicleid)
{
	return VehicleInfo[vehicleid][vColor2];
}

stock Vehicle_GetColors(vehicleid, &color1, &color2)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;
	color1 = VehicleInfo[vehicleid][vColor1];
	color2 = VehicleInfo[vehicleid][vColor2];
	return 1;
}

stock Float:Vehicle_GetFuel(vehicleid)
{
	return VehicleFuel[vehicleid];
}

stock Vehicle_SetFuel(vehicleid, Float:setValue)
{
	if(setValue > 100)
		setValue = 100;
	else if(setValue < 0)
		setValue = 0;
	VehicleFuel[vehicleid] = setValue;
}

stock Vehicle_GiveFuel(vehicleid, Float:value)
{
	VehicleFuel[vehicleid] += value;
	if(VehicleFuel[vehicleid] > 100.0)
		VehicleFuel[vehicleid] = 100.0;
	else if(VehicleFuel[vehicleid] < 0.0)
		VehicleFuel[vehicleid] = 0.0;
}

stock Float:Vehicle_GetHealth(vehicleid)
{
	new Float:h;
	GetVehicleHealth(vehicleid, h);
	return h;
}