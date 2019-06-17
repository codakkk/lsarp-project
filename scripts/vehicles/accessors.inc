stock Vehicle_IsValid(vehicleid)
{
	return IsValidVehicle(vehicleid);
}

stock Vehicle_GetID(vehicleid)
{
	return VehicleInfo[vehicleid][vID];
}

stock Vehicle_GetModel(vehicleid)
{
	return VehicleInfo[vehicleid][vModel];
}

stock Vehicle_GetFaction(vehicleid)
{
	return VehicleInfo[vehicleid][vFaction];
}

stock Vehicle_HasFaction(vehicleid)
{
	return VehicleInfo[vehicleid][vFaction] != -1;
}

stock Vehicle_SetFaction(vehicleid, factionid)
{
	VehicleInfo[vehicleid][vFaction] = factionid;
}

stock Vehicle_SetOwnerName(vehicleid, const name[])
{
	set(VehicleInfo[vehicleid][vOwnerName], name);
}

stock Vehicle_GetOwnerID(vehicleid)
{
	return VehicleInfo[vehicleid][vOwnerID];
}

stock Vehicle_SetOwnerID(vehicleid, dbid)
{
	VehicleInfo[vehicleid][vOwnerID] = dbid;
}

stock Vehicle_SetTemporary(vehicleid, bool:istemporary)
{
	Bit_Set(gVehicleBitState[e_vTemporary], vehicleid, istemporary);
}

stock Vehicle_IsTemporary(vehicleid)
{
	return Bit_Get(gVehicleBitState[e_vTemporary], vehicleid);
}

stock Vehicle_GetLastChopShopTime(vehicleid)
{
	return VehicleInfo[vehicleid][vLastChopShopTime];
}

stock Vehicle_SetLastChopShopTime(vehicleid, time)
{
	VehicleInfo[vehicleid][vLastChopShopTime] = time;
}

stock Vehicle_SetLocked(vehicleid, bool:locked)
{
	Bit_Set(gVehicleBitState[e_vLocked], vehicleid, locked);
}

stock Vehicle_IsLocked(vehicleid)
{
    return Bit_Get(gVehicleBitState[e_vLocked], vehicleid);
}

stock Vehicle_SetDestroyed(vehicleid, bool:destroyed)
{
	Bit_Set(gVehicleBitState[e_vDestroyed], vehicleid, destroyed);
}

stock Vehicle_IsDestroyed(vehicleid)
{
    return Bit_Get(gVehicleBitState[e_vDestroyed], vehicleid);
}

stock Vehicle_SetDespawn(vehicleid, bool:despawn)
{
	Bit_Set(gVehicleBitState[e_vDespawn], vehicleid, despawn);
}

stock Vehicle_IsDespawn(vehicleid)
{
    return Bit_Get(gVehicleBitState[e_vDespawn], vehicleid);
}

// Updates Vehicle Params too.
stock Vehicle_SetEngineStatus(vehicleid, bool:engineStatus)
{
	Bit_Set(gVehicleBitState[e_vEngine], vehicleid, engineStatus);
	new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    SetVehicleParamsEx(vehicleid, engineStatus ? VEHICLE_PARAMS_ON : VEHICLE_PARAMS_OFF, lights, alarm, doors, bonnet, boot, objective);
}

stock Vehicle_GetEngineStatus(vehicleid)
{
	return Bit_Get(gVehicleBitState[e_vEngine], vehicleid);
}

stock Vehicle_IsOwnable(vehicleid)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;
	// IS FACTION VEHICLE?
	return 1;
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

stock Vehicle_GetSpawnExpiry(vehicleid)
{
	return VehicleInfo[vehicleid][vSpawnExpiry];
}

stock Vehicle_SetSpawnExpiry(vehicleid, time)
{
	VehicleInfo[vehicleid][vSpawnExpiry] = time;
}