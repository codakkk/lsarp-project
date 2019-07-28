#include <vehicles\accessors>
#include <YSI_Coding\y_hooks>

#define VEHICLE_MIN_HEALTH	250.0
#define VEHICLE_MIN_HEALTH_FOR_ENGINE	350.0

hook OnGameModeInit()
{
	mysql_tquery(gMySQL, "UPDATE `vehicles` SET Spawned = 0 WHERE 1;");
	new query[256];
	mysql_format(gMySQL, query, sizeof(query), "SELECT * FROM vehicles WHERE Faction != '%d';", INVALID_FACTION_ID);
	mysql_tquery(gMySQL, query, #Vehicle_LoadAll);
	return 1;
}

task OnVehicleUpdate[500]()
{
	foreach(new v : Vehicle)
	{
		if(!Vehicle_IsValid(v))
			continue;
		if(Vehicle_GetHealth(v) <= VEHICLE_MIN_HEALTH)
		{
			Vehicle_SetDestroyed(v, true);
			Vehicle_Respawn(v);
		}
	}
}

task OnVehicleCheck[120000]() 
{
	foreach(new v : Vehicle)
	{
		if(!Vehicle_IsValid(v) || Vehicle_GetFaction(v) != INVALID_FACTION_ID)
			continue;
		//printf("V: %d", v);
		if(Vehicle_IsDespawn(v) && gettime() > Vehicle_GetSpawnExpiry(v))
		{
			ITER_SAFE_REMOVE(Vehicle, v)
			{
				Vehicle_Despawn(v);
			}
			continue;
		}
	}
}

ptask OnPlayerVehicleFuelUpdate[15000](playerid)
{
	new vehicleid = GetPlayerVehicleID(playerid);
	if(vehicleid <= 0)
		return 1;
	if(Vehicle_IsEngineOn(vehicleid))
	{
		if(Vehicle_GetFuel(vehicleid) > 0.0)
		{
			//!IsAHelicopter(vehicleid) && !IsAPlane(vehicleid) && !IsABoat(vehicleid)
			if(Vehicle_IsEngineOn(vehicleid) && !IsABike(vehicleid))
			{
				Vehicle_GiveFuel(vehicleid, -0.25);
			}
		}
		else
		{
			Player_Info(playerid, "Benzina ~r~finita~w~.", true, 4000);
			Vehicle_SetEngineOff(vehicleid);
		}
	}
	return 1;
}

ptask OnPlayerVehicleCheck[1000 + 1](playerid)
{
	new vehicleid = GetPlayerVehicleID(playerid);
	if(vehicleid <= 0)
		return 1;
	if(Vehicle_IsEngineOn(vehicleid))
	{
		
		new Float:health = Vehicle_GetHealth(vehicleid);
		if(health < VEHICLE_MIN_HEALTH_FOR_ENGINE && !Account_IsAdminDuty(playerid))
		{
			Vehicle_SetEngineOff(vehicleid);
			SendClientMessage(playerid, COLOR_ERROR, "Il motore del veicolo si è spento poiché è danneggiato.");
		}

	}
	return 1;
}

// TODO: Write a Vehicle_LoadAll that loads factions vehicles

hook OnVehicleSpawn(vehicleid)
{
	printf("Hook OnVehicleSpawn %d", vehicleid);
	Vehicle_Reload(vehicleid);
	return 1;
}

hook OnVehicleDeath(vehicleid, killerid)
{
    printf("Hook OnVehicleDeath %d", vehicleid);
    Vehicle_SetDestroyed(vehicleid, true);
    return 1;
}

stock Vehicle_Reload(vehicleid)
{
    Vehicle_ResetStats(vehicleid);
	
	SetVehiclePos(vehicleid, VehicleInfo[vehicleid][vX], VehicleInfo[vehicleid][vY], VehicleInfo[vehicleid][vZ]);
	SetVehicleZAngle(vehicleid, VehicleInfo[vehicleid][vA]);
	
	ChangeVehicleColor(vehicleid, VehicleInfo[vehicleid][vColor1], VehicleInfo[vehicleid][vColor2]);

	Vehicle_SetEngineOff(vehicleid);
	if(Vehicle_IsDestroyed(vehicleid))
	{
		Vehicle_SetDestroyed(vehicleid, false);
		SetVehicleHealth(vehicleid, VEHICLE_MIN_HEALTH_FOR_ENGINE+50.0);
	}
	else
	{
		SetVehicleHealth(vehicleid, 999.0);
	}
	/*if(VehicleRestore[vehicleid][vSpawned])
	{   
		VehicleRestore[vehicleid][vSpawned] = 0;
		if(Vehicle_IsEngineOn(vehicleid))
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
		if(Vehicle_IsDestroyed(vehicleid))
		{
			Vehicle_SetDestroyed(vehicleid, false);
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

stock Vehicle_Respawn(vehicleid)
{
	SetVehicleToRespawn(vehicleid);
	Vehicle_SetEngineOff(vehicleid);
	Vehicle_SetLightState(vehicleid, false);
	Vehicle_SetFuel(vehicleid, 100.0);
	SetVehiclePos(vehicleid, VehicleInfo[vehicleid][vX], VehicleInfo[vehicleid][vY], VehicleInfo[vehicleid][vZ]);
	SetVehicleZAngle(vehicleid, VehicleInfo[vehicleid][vA]);
	return 1;
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
	Vehicle_SetLocked(vehicleid, false);
	Vehicle_SetEngineStatus(vehicleid, false);
	Vehicle_SetFaction(vehicleid, INVALID_FACTION_ID);
	Vehicle_SetFuel(vehicleid, 100.0);
	SetVehicleParamsEx(vehicleid, 0, 0, 0, 0, 0, 0, 0);
	Vehicle_UpdateLockState(vehicleid);
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
		Vehicle_CancelDespawn(vehicleid);
		Vehicle_SetLocked(vehicleid, false);
		Vehicle_UpdateLockState(vehicleid);
		Vehicle_SetTemporary(vehicleid, false);
		Vehicle_InitializeInventory(vehicleid);
		//Vehicle_GeneratePlate(vehicleid);
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
    Vehicle_SetLocked(vehicleid, false);
	Vehicle_SetLastChopShopTime(vehicleid, 0);
	Vehicle_CancelDespawn(vehicleid);
    //VehicleRestore[vehicleid][vSpawned] = 0;
    VehicleRestore[vehicleid][vLastX] = 0.0;
    VehicleRestore[vehicleid][vLastY] = 0.0;
    VehicleRestore[vehicleid][vLastZ] = 0.0;
    VehicleRestore[vehicleid][vLastA] = 0.0;
    VehicleRestore[vehicleid][vLastHealth] = 0.0;
    Vehicle_SetEngineOff(vehicleid);
	Vehicle_SetDestroyed(vehicleid, false);
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
    Vehicle_SetLocked(vehicleid, true);
    //Vehicle_UpdateLockState(vehicleid);
    return 1;
}

stock Vehicle_UnLock(vehicleid)
{
    Vehicle_SetLocked(vehicleid, false);
    //Vehicle_UpdateLockState(vehicleid);
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
    SetVehicleParamsEx(vehicleid, engine, lights, alarm, Vehicle_IsLocked(vehicleid) ? VEHICLE_PARAMS_ON : VEHICLE_PARAMS_OFF, bonnet, boot, objective);
    if(Vehicle_IsLocked(vehicleid))
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
	   Float:hp;
	   //vehSpawned = 0;
    
    GetVehiclePos(vehicleid, x, y, z);
    GetVehicleZAngle(vehicleid, a);
    GetVehicleHealth(vehicleid, hp);

	/*if(x != 0.0 && y != 0.0 && z != 0.0)
	{
		vehSpawned = 1;
	}*/

    new query[1024];
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
    Vehicle_GetEngineStatus(vehicleid),
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

// TODO: This doesn't work neither
stock Vehicle_GeneratePlate(vehicleid)
{
	/*inline OnPlate(string:plate[])
	{
		if(cache_num_rows() > 0)
			return Vehicle_GeneratePlate(vehicleid);
		Vehicle_SetPlate(vehicleid, plate);
		Vehicle_Respawn(vehicleid);
		mysql_tquery_f(gMySQL, "UPDATE vehicles SET Plate = '%e' WHERE ID = '%d'", plate, Vehicle_GetID(vehicleid));
	}

	new query[128], tempPlate[9];

	static const RandomLetters[26][5] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};
	format(tempPlate, sizeof(tempPlate), "%d%s%s%s%d", random(10), RandomLetters[random(26)], RandomLetters[random(26)], RandomLetters[random(26)], vehicleid);
	
	format(query, sizeof(query), "SELECT ID FROM vehicles WHERE Plate = '%e'", tempPlate);
	mysql_tquery_inline(gMySQL, query, using inline OnPlate, "s", tempPlate);*/
}

stock Vehicle_SetPlate(vehicleid, plate[])
{
	set(VehicleInfo[vehicleid][vPlate], plate);
	return SetVehicleNumberPlate(vehicleid, plate);
}

stock Vehicle_UpdatePlate(vehicleid)
{
	SetVehicleNumberPlate(vehicleid, VehicleInfo[vehicleid][vPlate]);
	return 1;
}

stock Vehicle_SetToDespawn(vehicleid, minutes)
{
	Vehicle_SetSpawnExpiry(vehicleid, gettime() + minutes * 60); // minutes * 60 because -> N miunutes * 60 seconds.
	Vehicle_SetDespawn(vehicleid, true);
	return 1;
}

stock Vehicle_CancelDespawn(vehicleid)
{
	Vehicle_SetSpawnExpiry(vehicleid, 0);
	Vehicle_SetDespawn(vehicleid, false);
	return 1;
}

stock Vehicle_Despawn(vehicleid)
{
	if(!Vehicle_GetID(vehicleid) || Vehicle_IsTemporary(vehicleid))
		return 0;
	mysql_tquery_f(gMySQL, "UPDATE `vehicles` SET Spawned = '0' WHERE ID = '%d';", Vehicle_GetID(vehicleid));

	Vehicle_Save(vehicleid);

	CallLocalFunction(#OnVehicleUnloaded, "d", vehicleid);
	
	Vehicle_Destroy(vehicleid);
	return 1;
}

stock Vehicle_IsOwner(vehicleid, playerid, bool:onlyEffectiveOwner)
{
	if(Vehicle_GetOwnerID(vehicleid) == Character_GetID(playerid) || Account_IsAdminDuty(playerid))
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
	new count = 0, dbid = 0, modelid = 0, ownerid = 0, locked = 0, plate[32];
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
		Vehicle_SetOwnerID(vehicleid, ownerid);
		cache_get_value_index_int(i, 3, VehicleInfo[vehicleid][vColor1]);
		cache_get_value_index_int(i, 4, VehicleInfo[vehicleid][vColor2]);
		cache_get_value_index_float(i, 5, VehicleInfo[vehicleid][vX]);
		cache_get_value_index_float(i, 6, VehicleInfo[vehicleid][vY]);
		cache_get_value_index_float(i, 7, VehicleInfo[vehicleid][vZ]);
		cache_get_value_index_float(i, 8, VehicleInfo[vehicleid][vA]);
		cache_get_value_index_int(i, 9, locked);
		cache_get_value_index_float(i, 10, VehicleRestore[vehicleid][vLastX]);
		cache_get_value_index_float(i, 11, VehicleRestore[vehicleid][vLastY]);
		cache_get_value_index_float(i, 12, VehicleRestore[vehicleid][vLastZ]);
		cache_get_value_index_float(i, 13, VehicleRestore[vehicleid][vLastA]);
		cache_get_value_index_float(i, 14, VehicleRestore[vehicleid][vLastHealth]);
		//cache_get_value_index_int(i, 15, VehicleRestore[vehicleid][vSpawned]);
		//cache_get_value_index_int(i, 16, VehicleRestore[vehicleid][vEngine]);
		cache_get_value_index_float(i, 17, VehicleFuel[vehicleid]);	  
		cache_get_value_index_int(i, 18, VehicleInfo[vehicleid][vFaction]);
		cache_get_value_index_int(i, 19, VehicleInfo[vehicleid][vLastChopShopTime]);
		cache_get_value_index_int(i, 20, VehicleInfo[vehicleid][vPlate]);
		
		Vehicle_SetEngineStatus(vehicleid, false);
		Vehicle_SetDestroyed(vehicleid, false);
		Vehicle_SetLocked(vehicleid, locked > 0 ? true : false);

		Vehicle_Reload(vehicleid);

		SetVehicleHealth(vehicleid, VehicleRestore[vehicleid][vLastHealth]);

		if(ownerid > 0)
		{
			new bool:found = false;
			foreach(new p : Player)
			{
				if(Character_GetID(p) == ownerid)
				{
					Vehicle_SetOwnerName(vehicleid, Character_GetOOCName(p));
					found = true;
					break;
				}
			}
			if(!found)
			{
				Vehicle_SetOwnerName(vehicleid, Character_GetNameFromDatabase(ownerid, true));
			}
		}
		Vehicle_UpdatePlate(vehicleid);
		Vehicle_CancelDespawn(vehicleid);

		mysql_tquery_f(gMySQL, "UPDATE `vehicles` SET Spawned = 1 WHERE ID = '%d';", dbid);

		CallLocalFunction(#OnVehicleLoaded, "d", vehicleid);
	}
	return 1;
}