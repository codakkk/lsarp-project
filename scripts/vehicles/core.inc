
#include <YSI_Coding\y_hooks>

// Remove OwnerName from enum and create an indipendent variable (VehicleOwnerName)

static enum E_VEHICLE_DATA
{
    ID,
    OwnerID,
    Model,
    Color1,
    Color2,
    Float:X,
    Float:Y,
    Float:Z,
    Float:A,
	LastChopShopTime,
	Faction,
	SpawnExpiry,
	Plate[9],
	Price
};

enum E_VEHICLE_RESTORE_DATA
{
    //vSpawned,
    Float:LastX,
    Float:LastY,
    Float:LastZ,
    Float:LastA,
    Float:LastHealth,
};


static enum e_Bit1_VehicleData
{
	e_vSpawned,
	e_vDestroyed,
	e_vLocked,
	e_vTemporary,
	e_vEngine,
	e_vDespawn
};

static 
	VehicleInfo[MAX_VEHICLES][E_VEHICLE_DATA],
	VehicleRestore[MAX_VEHICLES][E_VEHICLE_RESTORE_DATA],
	VehicleOwnerName[MAX_VEHICLES][MAX_PLAYER_NAME],
	Float:VehicleFuel[MAX_VEHICLES],
	BitArray:gVehicleBitState[e_Bit1_VehicleData]<MAX_VEHICLES>
;

hook OnGameModeInit()
{
	mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `vehicles` ( \
						`id` int(11) NOT NULL AUTO_INCREMENT, \
						`owner_id` int(11) NOT NULL, \
						`model` int(11) NOT NULL, \
						`color_1` int(11) NOT NULL, \
						`color_2` int(11) NOT NULL, \
						`x` float DEFAULT NULL, \
						`y` float DEFAULT NULL, \
						`z` float DEFAULT NULL, \
						`a` float DEFAULT NULL, \
						`locked` int(1) DEFAULT NULL, \
						`last_x` float DEFAULT '0', \
						`last_y` float DEFAULT '0', \
						`last_z` float DEFAULT '0', \
						`last_a` float DEFAULT '0', \
						`last_health` float DEFAULT '0', \
						`spawned` int(1) DEFAULT '0', \
						`engine` int(1) DEFAULT '0', \
						`fuel` float NOT NULL DEFAULT '0', \
						`faction` tinyint(4) NOT NULL DEFAULT '-1', \
						`last_chop_shop_time` int(11) NOT NULL DEFAULT '0', \
						PRIMARY KEY (`id`), \
						KEY `OwnerID` (`owner_id`), \
						CONSTRAINT `CharacterVehicle` FOREIGN KEY (`owner_id`) REFERENCES `characters` (`id`) ON DELETE CASCADE \
						) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=latin1");
	mysql_tquery(gMySQL, "UPDATE `vehicles` SET spawned = '0' WHERE 1;");
	new query[256];
	mysql_format(gMySQL, query, sizeof(query), "SELECT * FROM vehicles WHERE faction != '%d';", INVALID_FACTION_ID);
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
			new Float:x, Float:y, Float:z, Float:a;
			
			GetVehiclePos(v, x, y, z);
			GetVehicleZAngle(v, a);

			Vehicle_SetDestroyed(v, true);
			Vehicle_Respawn(v);
			
			SetVehiclePos(v, x, y, z);
			SetVehicleZAngle(v, a);
		}
	}
}

/*task OnVehicleCheck[120000]() 
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
}*/

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
				Vehicle_AddFuel(vehicleid, -0.25);
			}
		}
		else
		{
			Player_Info(playerid, "Benzina ~r~finita~w~.", true, 4000);
			Vehicle_SetEngineStatus(vehicleid, false);
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
			Vehicle_SetEngineStatus(vehicleid, false);
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
	
	SetVehicleNumberPlate(vehicleid, VehicleInfo[vehicleid][Plate]);

	SetVehiclePos(vehicleid, VehicleInfo[vehicleid][X], VehicleInfo[vehicleid][Y], VehicleInfo[vehicleid][Z]);
	SetVehicleZAngle(vehicleid, VehicleInfo[vehicleid][A]);
	
	ChangeVehicleColor(vehicleid, VehicleInfo[vehicleid][Color1], VehicleInfo[vehicleid][Color2]);

	Vehicle_SetEngineStatus(vehicleid, false);
	if(Vehicle_IsDestroyed(vehicleid))
	{
		Vehicle_SetDestroyed(vehicleid, false);
		//SetVehicleHealth(vehicleid, VEHICLE_MIN_HEALTH);
		SetVehicleHealth(vehicleid, VEHICLE_MIN_HEALTH_FOR_ENGINE+50.0);
	}
	else
	{
		SetVehicleHealth(vehicleid, 999.0);
	}
	/*if(VehicleRestore[vehicleid][Spawned])
	{   
		VehicleRestore[vehicleid][Spawned] = 0;
		if(Vehicle_IsEngineOn(vehicleid))
		{
			Vehicle_SetEngineOn(vehicleid);
		}
		SetVehiclePos(vehicleid, VehicleRestore[vehicleid][LastX], VehicleRestore[vehicleid][LastY], VehicleRestore[vehicleid][LastZ]);
		SetVehicleZAngle(vehicleid, VehicleRestore[vehicleid][LastA]);
	}
	else
	{
		SetVehiclePos(vehicleid, VehicleInfo[vehicleid][X], VehicleInfo[vehicleid][Y], VehicleInfo[vehicleid][Z]);
		SetVehicleZAngle(vehicleid, VehicleInfo[vehicleid][A]);
		Vehicle_SetEngineStatus(vehicleid, false);
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
	
	ChangeVehicleColor(vehicleid, VehicleInfo[vehicleid][Color1], VehicleInfo[vehicleid][Color2]);
}

stock Vehicle_Respawn(vehicleid)
{
	SetVehicleToRespawn(vehicleid);
	Vehicle_SetEngineStatus(vehicleid, false);
	Vehicle_SetLightState(vehicleid, false);
	Vehicle_SetFuel(vehicleid, 100.0);
	SetVehiclePos(vehicleid, VehicleInfo[vehicleid][X], VehicleInfo[vehicleid][Y], VehicleInfo[vehicleid][Z]);
	SetVehicleZAngle(vehicleid, VehicleInfo[vehicleid][A]);
	return 1;
}

stock Vehicle_Create(modelid, Float:x, Float:y, Float:z, Float:rotation, color1, color2, respawn_delay, addsiren=0)
{
    new vehicleid = CreateVehicle(modelid, x, y, z, rotation, color1, color2, respawn_delay, addsiren);
	if(vehicleid <= 0)
		return -1;
	VehicleInfo[vehicleid][OwnerID] = 0;
	set(VehicleOwnerName[vehicleid], "");
	VehicleInfo[vehicleid][Model] = modelid;
	VehicleInfo[vehicleid][Color1] = color1;
	VehicleInfo[vehicleid][Color2] = color2;
	VehicleInfo[vehicleid][X] = x;
	VehicleInfo[vehicleid][Y] = y;
	VehicleInfo[vehicleid][Z] = z;
	VehicleInfo[vehicleid][A] = rotation;
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
		VehicleInfo[vehicleid][ID] = cache_insert_id();
		VehicleInfo[vehicleid][Model] = GetVehicleModel(vehicleid);
		VehicleInfo[vehicleid][Color1] = Vehicle_GetColor1(vehicleid);
		VehicleInfo[vehicleid][Color2] = Vehicle_GetColor2(vehicleid);
		VehicleInfo[vehicleid][X] = x;
		VehicleInfo[vehicleid][Y] = y;
		VehicleInfo[vehicleid][Z] = z;
		VehicleInfo[vehicleid][A] = a;
		Vehicle_CancelDespawn(vehicleid);
		Vehicle_SetLocked(vehicleid, false);
		Vehicle_UpdateLockState(vehicleid);
		Vehicle_SetTemporary(vehicleid, false);
		Vehicle_InitializeInventory(vehicleid);
		//Vehicle_GeneratePlate(vehicleid);
	}
	MySQL_TQueryInline(gMySQL, using inline OnInsert, "INSERT INTO `vehicles` (owner_id, model, color_1, color_2, x, y, z, angle, locked, faction, price) \
		VALUES('%d', '%d', '%d', '%d', '%f', '%f', '%f', '%f', '0', '%d', '%d')",
		Vehicle_GetOwnerID(vehicleid), GetVehicleModel(vehicleid), Vehicle_GetColor1(vehicleid), Vehicle_GetColor2(vehicleid), x, y, z, a, Vehicle_GetFaction(vehicleid),
		Vehicle_GetPrice(vehicleid));
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
	VehicleInfo[vehicleid][ID] = 0;
	Vehicle_SetTemporary(vehicleid, false);
    VehicleInfo[vehicleid][OwnerID] = 0;
    set(VehicleOwnerName[vehicleid], "");
    VehicleInfo[vehicleid][Model] = 0;
    VehicleInfo[vehicleid][Color1] = 0;
    VehicleInfo[vehicleid][Color2] = 0;
    VehicleInfo[vehicleid][X] = 0.0;
    VehicleInfo[vehicleid][Y] = 0.0;
    VehicleInfo[vehicleid][Z] = 0.0;
    VehicleInfo[vehicleid][A] = 0.0;
	VehicleInfo[vehicleid][Faction] = INVALID_FACTION_ID;
    Vehicle_SetLocked(vehicleid, false);
	Vehicle_SetLastChopShopTime(vehicleid, 0);
	Vehicle_CancelDespawn(vehicleid);
    //VehicleRestore[vehicleid][Spawned] = 0;
    VehicleRestore[vehicleid][LastX] = 0.0;
    VehicleRestore[vehicleid][LastY] = 0.0;
    VehicleRestore[vehicleid][LastZ] = 0.0;
    VehicleRestore[vehicleid][LastA] = 0.0;
    VehicleRestore[vehicleid][LastHealth] = 0.0;
    Vehicle_SetEngineStatus(vehicleid, false);
	Vehicle_SetDestroyed(vehicleid, false);
	Vehicle_SetFuel(vehicleid, 0.0);
	return 1;
}

stock Vehicle_Park(vehicleid, Float:nx, Float:ny, Float:nz, Float:na)
{
    if(!Vehicle_IsValid(vehicleid))
	   return 0;
    VehicleInfo[vehicleid][X] = nx;
    VehicleInfo[vehicleid][Y] = ny;
    VehicleInfo[vehicleid][Z] = nz;
    VehicleInfo[vehicleid][A] = na;
    if(Vehicle_GetID(vehicleid) != 0)
		mysql_tquery_f(gMySQL, "UPDATE `vehicles` SET x = '%f', y = '%f', z = '%f', angle = '%f' WHERE id = '%d'", nx, ny, nz, na, Vehicle_GetID(vehicleid));
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

stock Vehicle_GetLightState(vehicleid)
{
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    return lights;
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

// Deletes a vehicle (vehicleid). If it's a player vehicle, all their references are destroyed too (database, ecc).
stock Vehicle_Delete(vehicleid)
{
	if(!Vehicle_IsValid(vehicleid) || !Vehicle_GetID(vehicleid))
		return 0;
	mysql_tquery_f(gMySQL, "DELETE FROM `vehicles` WHERE id = '%d'", Vehicle_GetID(vehicleid));

	//mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `vehicle_inventory` WHERE vehicle_id = '%d'", Vehicle_GetID(vehicleid));
	//mysql_tquery(gMySQL, query);

	Vehicle_Destroy(vehicleid);
	return 1;
}

stock Vehicle_GetOwnerName(vehicleid)
{
	if(!Vehicle_IsValid(vehicleid) || Vehicle_GetOwnerID(vehicleid) == 0)
	{
		new name[MAX_PLAYER_NAME] = "Sconosciuto";
		return name;
	}
	return VehicleOwnerName[vehicleid];
}

stock String:Vehicle_GetOwnerNameStr(vehicleid)
{
	if(!IsValidVehicle(vehicleid) || Vehicle_GetOwnerID(vehicleid) == 0)
		return STRING_NULL;
	return str_new(VehicleOwnerName[vehicleid]);
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
	if(Vehicle_GetID(vehicleid) <= 0)
		return 0;
	set(VehicleInfo[vehicleid][Plate], plate);
	mysql_tquery_f(gMySQL, "UPDATE `vehicles` SET plate = '%e' WHERE id = '%d'", plate, Vehicle_GetID(vehicleid));
	return SetVehicleNumberPlate(vehicleid, plate);
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
	
	new Float:x, Float:y, Float:z, Float:a, Float:health;

	GetVehiclePos(vehicleid, x, y, z);
	GetVehicleZAngle(vehicleid, a);
	GetVehicleHealth(vehicleid, health);

	mysql_tquery_f(gMySQL, "UPDATE `vehicles` SET spawned = '0', last_x = '%f', last_y = '%f', last_z = '%f', last_a = '%f', last_health = '%f' WHERE id = '%d';", 
	x, y, z, a, health, Vehicle_GetID(vehicleid));

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
	new count = 0, dbid = 0, modelid = 0, ownerid = 0, locked = 0,
		bool:alreadyLoaded = false;
	
	cache_get_row_count(count);

	for(new i = 0; i < count; ++i)
	{
		cache_get_value_name_int(i, "id", dbid);
		cache_get_value_name_int(i, "owner_id", ownerid);
		cache_get_value_name_int(i, "model", modelid);
		
		if(dbid == 0 || modelid == 0)
			continue;
		
		// Just safeness. Probably useless
		foreach(new v : Vehicle)
		{
			if(Vehicle_GetID(v) == dbid)
			{
				alreadyLoaded = true;
				break;
			}
		}

		if(alreadyLoaded)
			continue;

		new vehicleid = Vehicle_Create(modelid, 0.0, 0.0, 0.0, 0.0, 0, 0, -1);
		
		VehicleInfo[vehicleid][ID] = dbid;
		VehicleInfo[vehicleid][OwnerID] = ownerid;
		
		cache_get_value_name_float(i, "x", VehicleInfo[vehicleid][X]);
		cache_get_value_name_float(i, "y", VehicleInfo[vehicleid][Y]);
		cache_get_value_name_float(i, "z", VehicleInfo[vehicleid][Z]);
		cache_get_value_name_float(i, "angle", VehicleInfo[vehicleid][A]);
		cache_get_value_name_int(i, "color_1", VehicleInfo[vehicleid][Color1]);
		cache_get_value_name_int(i, "color_2", VehicleInfo[vehicleid][Color2]);
		
		SetVehiclePos(vehicleid, VehicleInfo[vehicleid][X], VehicleInfo[vehicleid][Y], VehicleInfo[vehicleid][Z]);
		SetVehicleZAngle(vehicleid, VehicleInfo[vehicleid][A]);
		ChangeVehicleColor(vehicleid, VehicleInfo[vehicleid][Color1], VehicleInfo[vehicleid][Color2]);

		cache_get_value_name_int(i, "locked", locked);
		cache_get_value_name_float(i, "last_x", VehicleRestore[vehicleid][LastX]);
		cache_get_value_name_float(i, "last_y", VehicleRestore[vehicleid][LastY]);
		cache_get_value_name_float(i, "last_z", VehicleRestore[vehicleid][LastZ]);
		cache_get_value_name_float(i, "last_a", VehicleRestore[vehicleid][LastA]);
		cache_get_value_name_float(i, "last_health", VehicleRestore[vehicleid][LastHealth]);
		//cache_get_value_name_int(i, 15, VehicleRestore[vehicleid][Spawned]);
		//cache_get_value_name_int(i, 16, VehicleRestore[vehicleid][Engine]);
		cache_get_value_name_float(i, "fuel", VehicleFuel[vehicleid]);	  
		cache_get_value_name_int(i, "faction", VehicleInfo[vehicleid][Faction]);
		cache_get_value_name_int(i, "last_chop_shop_time", VehicleInfo[vehicleid][LastChopShopTime]);
		cache_get_value_name_int(i, "plate", VehicleInfo[vehicleid][Plate]);
		cache_get_value_name_int(i, "price", VehicleInfo[vehicleid][Price]);
		
		Vehicle_SetEngineStatus(vehicleid, false);
		Vehicle_SetDestroyed(vehicleid, false);
		Vehicle_SetLocked(vehicleid, locked > 0 ? true : false);

		Vehicle_Reload(vehicleid);
		
		SetVehicleHealth(vehicleid, VehicleRestore[vehicleid][LastHealth]);

		if(ownerid > 0)
		{
			/*new bool:found = false;
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
			{*/
				Vehicle_SetOwnerName(vehicleid, Character_GetNameFromDatabase(ownerid, true));
			//}
		}

		Vehicle_CancelDespawn(vehicleid);

		mysql_tquery_f(gMySQL, "UPDATE `vehicles` SET spawned = 1 WHERE id = '%d';", dbid);

		CallLocalFunction(#OnVehicleLoaded, "d", vehicleid);
	}
	return 1;
}

flags:avplate(CMD_MODERATOR);
CMD:avplate(playerid, params[])
{
	new vehicleid, plate[9];
	if(sscanf(params, "ds[8]", vehicleid, plate))
		return SendClientMessage(playerid, COLOR_ERROR, "/avplate <vehicleid> <testo (max 8)>");

	if(!Vehicle_IsValid(vehicleid))
		return SendClientMessage(playerid, COLOR_ERROR, "L'id del veicolo inserito non è valido.");
	
	Vehicle_SetPlate(vehicleid, plate);
	Vehicle_Respawn(vehicleid);

	SendFormattedMessage(playerid, -1, "Hai settato la targa del veicolo id %d ed è stato respawnato. Testo: %s", vehicleid, plate);
	Log(Account_GetName(playerid), plate, "/avplate", vehicleid);
	return 1;
}

stock Vehicle_IsValid(vehicleid)
{
	return IsValidVehicle(vehicleid);
}

stock Vehicle_GetID(vehicleid)
{
	return VehicleInfo[vehicleid][ID];
}

stock Vehicle_GetModel(vehicleid)
{
	return VehicleInfo[vehicleid][Model];
}

stock Vehicle_GetFaction(vehicleid)
{
	return VehicleInfo[vehicleid][Faction];
}

stock Vehicle_HasFaction(vehicleid)
{
	return VehicleInfo[vehicleid][Faction] != INVALID_FACTION_ID;
}

stock Vehicle_SetFaction(vehicleid, factionid)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;
	VehicleInfo[vehicleid][Faction] = factionid;
	if(Vehicle_GetID(vehicleid) != 0)
		mysql_tquery_f(gMySQL, "UPDATE `vehicles` SET faction = '%d' WHERE id = '%d'", Faction_GetID(factionid), Vehicle_GetID(vehicleid));
	return 1;
}

stock Vehicle_SetOwnerName(vehicleid, const name[])
{
	set(VehicleOwnerName[vehicleid], name);
}

stock Vehicle_GetOwnerID(vehicleid)
{
	return VehicleInfo[vehicleid][OwnerID];
}

stock Vehicle_SetOwner(vehicleid, playerid)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;
	VehicleInfo[vehicleid][OwnerID] = Character_GetID(playerid);
	Vehicle_SetOwnerName(vehicleid, Character_GetOOCName(playerid));
	if(Vehicle_GetID(vehicleid) != 0)
		mysql_tquery_f(gMySQL, "UPDATE `vehicles` SET owner_id = '%d' WHERE id = '%d'", Vehicle_GetOwnerID(vehicleid), Vehicle_GetID(vehicleid));
	return 1;
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
	return VehicleInfo[vehicleid][LastChopShopTime];
}

stock Vehicle_SetLastChopShopTime(vehicleid, time)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;
	VehicleInfo[vehicleid][LastChopShopTime] = time;
	if(Vehicle_GetID(vehicleid) != 0)
		mysql_tquery_f(gMySQL, "UPDATE `vehicles` SET last_chop_shop_time = '%d' WHERE id = '%d'", time, Vehicle_GetID(vehicleid));
	return 1;
}

stock Vehicle_SetLocked(vehicleid, bool:locked)
{
	Bit_Set(gVehicleBitState[e_vLocked], vehicleid, locked);
	if(Vehicle_GetID(vehicleid) != 0)
		mysql_tquery_f(gMySQL, "UPDATE `vehicles` SET locked = '%d' WHERE id = '%d'", locked?1:0, Vehicle_GetID(vehicleid));
	return 1;
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
	if(!Vehicle_IsValid(vehicleid))
		return 0;
	Bit_Set(gVehicleBitState[e_vEngine], vehicleid, engineStatus);
	new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    SetVehicleParamsEx(vehicleid, engineStatus ? VEHICLE_PARAMS_ON : VEHICLE_PARAMS_OFF, lights, alarm, doors, bonnet, boot, objective);
	
	if(Vehicle_GetID(vehicleid) != 0)
		mysql_tquery_f(gMySQL, "UPDATE `vehicles` SET engine = '%d' WHERE id = '%d'", engineStatus?VEHICLE_PARAMS_ON : VEHICLE_PARAMS_OFF, Vehicle_GetID(vehicleid));

	return 1;
}

stock Vehicle_GetEngineStatus(vehicleid)
{
	return Bit_Get(gVehicleBitState[e_vEngine], vehicleid);
}

stock Vehicle_IsOwnable(vehicleid)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;
	if(Vehicle_HasFaction(vehicleid))
		return 0;
	// IS FACTION VEHICLE?
	return 1;
}

stock Vehicle_GetColor1(vehicleid)
{
	return VehicleInfo[vehicleid][Color1];
}

stock Vehicle_GetColor2(vehicleid)
{
	return VehicleInfo[vehicleid][Color2];
}

stock Vehicle_GetColors(vehicleid, &color1, &color2)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;
	color1 = VehicleInfo[vehicleid][Color1];
	color2 = VehicleInfo[vehicleid][Color2];
	return 1;
}

stock Vehicle_SetColors(vehicleid, color1, color2)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;
	VehicleInfo[vehicleid][Color1] = color1;
	VehicleInfo[vehicleid][Color2] = color2;

	if(Vehicle_GetID(vehicleid) != 0)
		mysql_tquery_f(gMySQL, "UPDATE `vehicles` SET color_1 = '%d', color_2 = '%d' WHERE id = '%d'", color1, color2, Vehicle_GetID(vehicleid));

	return ChangeVehicleColor(vehicleid, color1, color2);
}

stock Float:Vehicle_GetFuel(vehicleid)
{
	return VehicleFuel[vehicleid];
}

stock Vehicle_SetFuel(vehicleid, Float:setValue)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;
	if(setValue > 100)
		setValue = 100;
	else if(setValue < 0)
		setValue = 0;
	VehicleFuel[vehicleid] = setValue;

	if(Vehicle_GetID(vehicleid) != 0)
		mysql_tquery_f(gMySQL, "UPDATE `vehicles` SET fuel = '%f' WHERE id = '%d'", setValue, Vehicle_GetID(vehicleid));
	return 1;
}

stock Vehicle_AddFuel(vehicleid, Float:addValue)
{
	Vehicle_SetFuel(vehicleid, VehicleFuel[vehicleid] + addValue);
}

stock Float:Vehicle_GetHealth(vehicleid)
{
	new Float:h;
	GetVehicleHealth(vehicleid, h);
	return h;
}

stock Vehicle_GetSpawnExpiry(vehicleid)
{
	return VehicleInfo[vehicleid][SpawnExpiry];
}

stock Vehicle_SetSpawnExpiry(vehicleid, time)
{
	VehicleInfo[vehicleid][SpawnExpiry] = time;
}

stock Vehicle_GetSpawnPos(vehicleid, &Float:x, &Float:y, &Float:z)
{
	if(!IsValidVehicle(vehicleid))
		return 0;
	x = VehicleInfo[vehicleid][X];
	y = VehicleInfo[vehicleid][Y];
	z = VehicleInfo[vehicleid][Z];
	return 1;
}

stock Vehicle_GetPrice(vehicleid)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;
	return VehicleInfo[vehicleid][Price];
}

stock Vehicle_SetPrice(vehicleid, price)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;
	VehicleInfo[vehicleid][Price] = price;
	if(Vehicle_GetID(vehicleid) != 0)
		mysql_tquery_f(gMySQL, "UPDATE `vehicles` SET price = '%d' WHERE id = '%d'", price, Vehicle_GetID(vehicleid));
	return 1;
}