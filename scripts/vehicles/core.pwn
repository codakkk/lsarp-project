#include <vehicles\accessors.pwn>
#include <YSI_Coding\y_hooks>

hook OnGameModeInit()
{
	mysql_tquery(gMySQL, "UPDATE `vehicles` SET Spawned = 0 WHERE 1;");
	new query[256];
	mysql_format(gMySQL, query, sizeof(query), "SELECT * FROM vehicles WHERE Faction != '%d';", INVALID_FACTION_ID);
	mysql_tquery(gMySQL, query, "LoadVehicles");
	return 1;
}

task OnVehicleCheck[120000]() 
{
	foreach(new v : Vehicles)
	{
		if(!Vehicle_IsValid(v) || Vehicle_GetFaction(v) != INVALID_FACTION_ID)
			continue;
		
		if(Vehicle_GetSpawnExpiry(v) != 0 && gettime() > Vehicle_GetSpawnExpiry(v))
		{
			Vehicle_Despawn(v);
			continue;
		}
	}
}

// TODO: Write a Vehicle_LoadAll that loads factions vehicles

hook OnCharacterDisconnected(playerid)
{
	foreach(new v : Vehicles)
	{
		if(!Vehicle_GetID(v) || Vehicle_GetOwnerID(v) == 0 || Vehicle_GetModel(v) == 0 || Vehicle_GetFaction(v) != INVALID_FACTION_ID)
			continue;
		Vehicle_SetSpawnExpiry(v, gettime() + 60 * 60);
	}
	return 1;
}

hook OnVehicleSpawn(vehicleid)
{
	printf("Hook OnVehicleSpawn %d", vehicleid);
	Vehicle_Reload(vehicleid);
	return 1;
}

hook OnVehicleDeath(vehicleid, killerid)
{
    printf("Hook OnVehicleDeath %d", vehicleid);
    vDestroyed{vehicleid} = true;
    return 1;
}

stock Vehicle_Reload(vehicleid)
{
    Vehicle_ResetStats(vehicleid);
	SetVehiclePos(vehicleid, VehicleInfo[vehicleid][vX], VehicleInfo[vehicleid][vY], VehicleInfo[vehicleid][vZ]);
	SetVehicleZAngle(vehicleid, VehicleInfo[vehicleid][vA]);
	Vehicle_SetEngineOff(vehicleid);
	if(vDestroyed{vehicleid})
	{
		vDestroyed{vehicleid} = false;
		SetVehicleHealth(vehicleid, 350.0);
	}
	else
	{
		SetVehicleHealth(vehicleid, 999.0);
	}
	/*if(VehicleRestore[vehicleid][vSpawned])
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
		Vehicle_SetEngineOff(vehicleid);
		if(vDestroyed{vehicleid})
		{
			vDestroyed{vehicleid} = false;
			SetVehicleHealth(vehicleid, 350.0);
		}
		else
		{
			SetVehicleHealth(vehicleid, 999.0);
		}
	}*/
	//new Float:health;
	//GetVehicleHealth(vehicleid, health);
	Vehicle_UpdateLockState(vehicleid);
	
	ChangeVehicleColor(vehicleid, VehicleInfo[vehicleid][vColor1], VehicleInfo[vehicleid][vColor2]);
}

stock Vehicle_Create(modelid, Float:x, Float:y, Float:z, Float:rotation, color1, color2, respawn_delay, addsiren=0)
{
    new vehicleid = CreateVehicle(modelid, x, y, z, rotation, color1, color2, respawn_delay, addsiren);
	if(vehicleid <= 0)
		return -1;
	VehicleInfo[vehicleid][vOwnerID] = 0;
	set(VehicleInfo[vehicleid][vOwnerName], "");
	VehicleInfo[vehicleid][vModel] = modelid;
	VehicleInfo[vehicleid][vColor1] = color1;
	VehicleInfo[vehicleid][vColor2] = color2;
	VehicleInfo[vehicleid][vX] = x;
	VehicleInfo[vehicleid][vY] = y;
	VehicleInfo[vehicleid][vZ] = z;
	VehicleInfo[vehicleid][vA] = rotation;
	VehicleInfo[vehicleid][vLocked] = 0;
	VehicleInfo[vehicleid][vFaction] = INVALID_FACTION_ID;
	VehicleFuel[vehicleid] = 100.0;
	Vehicle_UpdateLockState(vehicleid);
    Iter_Add(Vehicles, vehicleid);
	return vehicleid;
}

stock Vehicle_InsertInDatabase(vehicleid)
{
	if(!IsValidVehicle(vehicleid))
		return 0;
	new
		Float:x, Float:y, Float:z, Float:a;

	GetVehiclePos(vehicleid, x, y, z);
	GetVehicleZAngle(vehicleid, a);
	inline OnInsert()
	{
		VehicleInfo[vehicleid][vID] = cache_insert_id();
		VehicleInfo[vehicleid][vModel] = GetVehicleModel(vehicleid);
		VehicleInfo[vehicleid][vColor1] = Vehicle_GetColor1(vehicleid);
		VehicleInfo[vehicleid][vColor2] = Vehicle_GetColor2(vehicleid);
		VehicleInfo[vehicleid][vX] = x;
		VehicleInfo[vehicleid][vY] = y;
		VehicleInfo[vehicleid][vZ] = z;
		VehicleInfo[vehicleid][vA] = a;
		Vehicle_SetSpawnExpiry(vehicleid, 0);
		VehicleInfo[vehicleid][vLocked] = 0;
		Vehicle_UpdateLockState(vehicleid);
		Vehicle_SetTemporary(vehicleid, false);
		Vehicle_InitializeInventory(vehicleid);
	}
	MySQL_TQueryInline(gMySQL, using inline OnInsert, "INSERT INTO `vehicles` (OwnerID, Model, Color1, Color2, X, Y, Z, Angle, Locked, Faction) \
		VALUES('%d', '%d', '%d', '%d', '%f', '%f', '%f', '%f', '0', '%d')",
		Vehicle_GetOwnerID(vehicleid), GetVehicleModel(vehicleid), Vehicle_GetColor1(vehicleid), Vehicle_GetColor2(vehicleid), x, y, z, a, Vehicle_GetFaction(vehicleid));
	return 1;
}

stock Vehicle_Destroy(vehicleid)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;
    Vehicle_Reset(vehicleid);
	DestroyVehicle(vehicleid);
	Iter_SafeRemove(Vehicles, vehicleid, vehicleid);
	return 1;
}

stock Vehicle_Reset(vehicleid)
{
	VehicleInfo[vehicleid][vID] = 0;
	Vehicle_SetTemporary(vehicleid, false);
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
	Vehicle_SetLastChopShopTime(vehicleid, 0);
	Vehicle_SetSpawnExpiry(vehicleid, 0);
    VehicleRestore[vehicleid][vSpawned] = 0;
    VehicleRestore[vehicleid][vLastX] = 0.0;
    VehicleRestore[vehicleid][vLastY] = 0.0;
    VehicleRestore[vehicleid][vLastZ] = 0.0;
    VehicleRestore[vehicleid][vLastA] = 0.0;
    VehicleRestore[vehicleid][vLastHealth] = 0.0;
    VehicleRestore[vehicleid][vEngine] = 0;
	vDestroyed{vehicleid} = false;
	Vehicle_SetFuel(vehicleid, 0.0);
	return 1;
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

stock Vehicle_Lock(vehicleid)
{
    VehicleInfo[vehicleid][vLocked] = 1;
    Vehicle_UpdateLockState(vehicleid);
    return 1;
}

stock Vehicle_UnLock(vehicleid)
{
    VehicleInfo[vehicleid][vLocked] = 0;
    Vehicle_UpdateLockState(vehicleid);
    return 1;
}

stock Vehicle_IsLightOn(vehicleid)
{
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    return lights;
}

stock Vehicle_IsLightOff(vehicleid)
{
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    return !lights;
}

stock Vehicle_SetLightState(vehicleid, s)
{
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    SetVehicleParamsEx(vehicleid, engine, s ? VEHICLE_PARAMS_ON : VEHICLE_PARAMS_OFF, alarm, doors, bonnet, boot, objective);
    return 1;
}

stock Vehicle_IsEngineOff(vehicleid)
{
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    return !engine;
}

stock Vehicle_IsEngineOn(vehicleid)
{
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    return engine;
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
    if(!Vehicle_GetID(vehicleid) || !Vehicle_GetOwnerID(vehicleid) || !Vehicle_GetModel(vehicleid))
	   return 0;
    foreach(new i : Player)
    {
	   if(!Character_IsLogged(i))
		  continue;
	   if(Vehicle_GetOwnerID(vehicleid) == Character_GetID(i))
		  return 1;
    }
    return 0;
}

stock Vehicle_Save(vehicleid)
{
    if(Vehicle_IsTemporary(vehicleid) || !Vehicle_GetID(vehicleid) || !Vehicle_GetModel(vehicleid))
	   return 0;
    new 
	   Float:x, 
	   Float:y, 
	   Float:z, 
	   Float:a,
	   Float:hp,
	   vehSpawned = 0;
    
    GetVehiclePos(vehicleid, x, y, z);
    GetVehicleZAngle(vehicleid, a);
    GetVehicleHealth(vehicleid, hp);

	/*if(x != 0.0 && y != 0.0 && z != 0.0)
	{
		vehSpawned = 1;
	}*/

    new query[512];
    mysql_format(gMySQL, query, sizeof(query), "UPDATE `vehicles` SET \
    Model = '%d', \
	OwnerID = '%d', \
    Color1 = '%d', Color2 = '%d', \
    X = '%f', Y = '%f', Z = '%f', Angle = '%f', \
    Locked = '%d', \
    LastX = '%f', LastY = '%f', LastZ = '%f', LastA = '%f', \
    LastHealth = '%f', \
    Engine = '%d', \
	Fuel = '%f', \
	Faction = '%d', \
	LastChopShopTime = '%d' \
    WHERE ID = '%d'", 
    Vehicle_GetModel(vehicleid),
	Vehicle_GetOwnerID(vehicleid),
    Vehicle_GetColor1(vehicleid), Vehicle_GetColor2(vehicleid),
    VehicleInfo[vehicleid][vX], VehicleInfo[vehicleid][vY], VehicleInfo[vehicleid][vZ], VehicleInfo[vehicleid][vA],
    Vehicle_IsLocked(vehicleid),
    x, y, z, a,
    hp,
    Vehicle_IsEngineOn(vehicleid),
	Vehicle_GetFuel(vehicleid),
	Vehicle_GetFaction(vehicleid),
	Vehicle_GetLastChopShopTime(vehicleid),
    Vehicle_GetID(vehicleid));
    mysql_tquery(gMySQL, query);
    CallLocalFunction(#OnVehicleSaved, "d", vehicleid);
    return 1;
}

// Deletes a vehicle (vehicleid). If it's a player vehicle, all their references are destroyed too (database, ecc).
stock Vehicle_Delete(vehicleid)
{
	if(!Vehicle_IsValid(vehicleid) || !Vehicle_GetID(vehicleid))
		return 0;
	mysql_tquery_f(gMySQL, "DELETE FROM `vehicles` WHERE ID = '%d'", Vehicle_GetID(vehicleid));

	//mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `vehicle_inventory` WHERE VehicleID = '%d'", Vehicle_GetID(vehicleid));
	//mysql_tquery(gMySQL, query);

	Vehicle_Destroy(vehicleid);
	return 1;
}

stock Vehicle_GetOwnerName(vehicleid, name[])
{
	if(!Vehicle_IsValid(vehicleid) || Vehicle_GetOwnerID(vehicleid) == 0)
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

stock Vehicle_Despawn(vehicleid)
{
	if(!Vehicle_GetID(vehicleid) || Vehicle_IsTemporary(vehicleid))
		return 0;
	mysql_tquery_f(gMySQL, "UPDATE `vehicles` SET Spawned = '0' WHERE ID = '%d';", VehicleInfo[vehicleid][vID]);

	Vehicle_Save(vehicleid);

	CallLocalFunction(#OnVehicleUnloaded, "d", vehicleid);

	Vehicle_Destroy(vehicleid);
	return 1;
}

stock Vehicle_IsOwner(vehicleid, playerid, bool:onlyEffectiveOwner)
{
	if(Vehicle_GetOwnerID(vehicleid) == Character_GetID(playerid) || pAdminDuty[playerid])
		return 1;
	if(!onlyEffectiveOwner)
	{
		if(Character_GetFaction(playerid) != INVALID_FACTION_ID && Character_GetFaction(playerid) == Vehicle_GetFaction(vehicleid))
			return 1;
		
	}
	return 0;
}

forward Vehicle_LoadAll();
public Vehicle_LoadAll()
{
	new count = 0, dbid = 0, modelid = 0, ownerid = 0;
	cache_get_row_count(count);

	for(new i = 0; i < count; ++i)
	{
		cache_get_value_index_int(i, 0, dbid);
		cache_get_value_index_int(i, 1, ownerid);
		cache_get_value_index_int(i, 2, modelid);
		
		if(dbid == 0 || modelid == 0)
			continue;

		new vehicleid = Vehicle_Create(modelid, 0, 0, 0, 0, 0, 0, 0, 0);
		VehicleInfo[vehicleid][vID] = dbid;
		VehicleInfo[vehicleid][vOwnerID] = ownerid;
		VehicleInfo[vehicleid][vModel] = modelid;
		cache_get_value_index_int(i, 3, VehicleInfo[vehicleid][vColor1]);
		cache_get_value_index_int(i, 4, VehicleInfo[vehicleid][vColor2]);
		cache_get_value_index_float(i, 5, VehicleInfo[vehicleid][vX]);
		cache_get_value_index_float(i, 6, VehicleInfo[vehicleid][vY]);
		cache_get_value_index_float(i, 7, VehicleInfo[vehicleid][vZ]);
		cache_get_value_index_float(i, 8, VehicleInfo[vehicleid][vA]);
		cache_get_value_index_int(i, 9, VehicleInfo[vehicleid][vLocked]);
		cache_get_value_index_float(i, 10, VehicleRestore[vehicleid][vLastX]);
		cache_get_value_index_float(i, 11, VehicleRestore[vehicleid][vLastY]);
		cache_get_value_index_float(i, 12, VehicleRestore[vehicleid][vLastZ]);
		cache_get_value_index_float(i, 13, VehicleRestore[vehicleid][vLastA]);
		cache_get_value_index_float(i, 14, VehicleRestore[vehicleid][vLastHealth]);
		cache_get_value_index_int(i, 15, VehicleRestore[vehicleid][vSpawned]);
		cache_get_value_index_int(i, 16, VehicleRestore[vehicleid][vEngine]);
		cache_get_value_index_float(i, 17, VehicleFuel[vehicleid]);	  
		cache_get_value_index_int(i, 18, VehicleInfo[vehicleid][vFaction]);
		cache_get_value_index_int(i, 19, VehicleInfo[vehicleid][vLastChopShopTime]);
		
		vDestroyed{vehicleid} = false;

		Vehicle_Reload(vehicleid);

		SetVehicleHealth(vehicleid, VehicleRestore[vehicleid][vLastHealth]);

		if(Vehicle_GetOwnerID(vehicleid) > 0)
		{
			foreach(new p : Player)
			{
				if(Character_GetID(p) == Vehicle_GetOwnerID(vehicleid))
				{
					Vehicle_SetOwnerName(vehicleid, Character_GetOOCName(p));
					break;
				}
			}
		}
		
		Vehicle_SetSpawnExpiry(vehicleid, 0);

		mysql_tquery_f(gMySQL, "UPDATE `vehicles` SET Spawned = 1 WHERE ID = '%d';", dbid);

		CallLocalFunction(#OnVehicleLoaded, "d", vehicleid);
	}
	return 1;
}