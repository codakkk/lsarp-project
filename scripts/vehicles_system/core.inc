
#if defined _included_VEHICLES
    #endinput
#endif 

#define _included_VEHICLES

native OnVehicleCreate(vehicleid);
native OnVehicleLoaded(vehicleid); // Called when a vehicle is successfully loaded
native OnVehicleUnload(vehicleid); // Called when a vehicle is going to be unloaded.
native OnVehicleUnloaded(vehicleid); // Called when a vehicle is successfully unloaded

#include "inventory"
#include "components"
#include <YSI_Coding\y_hooks>

#define VEHICLE_MIN_HEALTH					300.0
#define VEHICLE_MIN_HEALTH_FOR_ENGINE		350.0
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
	Plate[9],
	Price,
	Float:MaxHealth,
	World
};

static 
	VehicleInfo[MAX_VEHICLES][E_VEHICLE_DATA],
	Text3D:VehicleSign[MAX_VEHICLES];

static enum e_VEHICLE_DAMAGE_TYPE
{
	E_DAMAGE_TYPE_PANELS,
	E_DAMAGE_TYPE_DOORS,
	E_DAMAGE_TYPE_LIGHTS,
	E_DAMAGE_TYPE_TIRES
};

static 
	VehicleDamages[MAX_VEHICLES][e_VEHICLE_DAMAGE_TYPE],
	CleanVehicleDamages[e_VEHICLE_DAMAGE_TYPE];

static enum e_Bit1_VehicleData
{
	e_vSpawned,
	e_vDestroyed,
	e_vLocked,
	e_vTemporary,
	e_vEngine
};

static BitArray:VehicleBitState[e_Bit1_VehicleData]<MAX_VEHICLES>;

static Float:VehicleFuel[MAX_VEHICLES];

static Map:VehiclesMap;

stock Map:GetVehiclesMap() { return VehiclesMap; }

hook OnGameModeLateInit()
{
	VehiclesMap = map_new();
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
						`world` int(11) DEFAULT '0', \
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
						`price` int(11) DEFAULT '0', \
						`max_health` float DEFAULT '999.0', \
						`panels` int(11) DEFAULT '0', \
						`doors` int(11) DEFAULT '0', \
						`lights` int(11) DEFAULT '0', \
						`tires` int(11) DEFAULT '0', \
						PRIMARY KEY (`id`), \
						KEY `OwnerID` (`owner_id`), \
						CONSTRAINT `CharacterVehicle` FOREIGN KEY (`owner_id`) REFERENCES `characters` (`id`) ON DELETE CASCADE \
						) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=latin1");
	mysql_tquery(gMySQL, "UPDATE `vehicles` SET spawned = '0' WHERE 1;");
	new query[256];
	mysql_format(gMySQL, query, sizeof(query), "SELECT * FROM vehicles WHERE faction != '%d'", INVALID_FACTION_ID);
	mysql_tquery(gMySQL, query, #Vehicle_LoadAll);
	
	mysql_format(gMySQL, query, sizeof(query), "SELECT * FROM vehicles WHERE job != '%d'", 0);
	mysql_tquery(gMySQL, query, #Vehicle_LoadAll);
	return 1;
}

hook OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	if(ispassenger || Player_IsAdminDuty(playerid))
		return 1;
	if(vehicleid > 0)
	{
		new vehicleFaction = Vehicle_GetFaction(vehicleid);
		if(Faction_IsValid(vehicleFaction) && Character_GetFaction(playerid) != Vehicle_GetFaction(vehicleid))
		{
			ClearAnimations(playerid, 1);
			SendClientMessage(playerid, COLOR_ERROR, "Questo veicolo appartiene ad una fazione di cui non fai parte.");
		}
	}
	return 1;
}

hook OnPlayerStateChange(playerid, newstate, oldstate)
{
	if(Player_IsAdminDuty(playerid))
		return 1;
	if(oldstate != PLAYER_STATE_DRIVER && newstate == PLAYER_STATE_DRIVER)
	{
		new vehicleid = GetPlayerVehicleID(playerid);
		new vehicleFaction = Vehicle_GetFaction(vehicleid);
		if(Faction_IsValid(vehicleFaction) && Character_GetFaction(playerid) != Vehicle_GetFaction(vehicleid))
		{
			RemovePlayerFromVehicle(playerid);
			SendClientMessage(playerid, COLOR_ERROR, "Questo veicolo appartiene ad una fazione di cui non fai parte.");
		}
	}
	return 1;
}

task OnVehicleMinuteUpdate[60000]()
{
	foreach(new i : Vehicle)
	{
		if(Vehicle_GetID(i) == 0)
			continue;
		Vehicle_SaveStatus(i);
	}
}

task OnVehicleUpdate[250]()
{
	foreach(new v : Vehicle)
	{
		if(!Vehicle_IsValid(v))
			continue;
		if(Vehicle_GetHealth(v) <= VEHICLE_MIN_HEALTH)
		{
			SetVehicleHealth(v, VEHICLE_MIN_HEALTH);
		}
	}
}

ptask OnPlayerVehicleFuelUpdate[1000 * 30](playerid)
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
		if(health < VEHICLE_MIN_HEALTH_FOR_ENGINE && !Player_IsAdminDuty(playerid))
		{
			Vehicle_SetEngineStatus(vehicleid, false);
			SendClientMessage(playerid, COLOR_ERROR, "Il motore del veicolo si � spento poich� � danneggiato.");
		}

	}
	return 1;
}

hook OnVehicleSpawn(vehicleid)
{
	SetVehicleNumberPlate(vehicleid, VehicleInfo[vehicleid][Plate]);
	Vehicle_Reload(vehicleid);
	return 1;
}

hook OnVehicleDeath(vehicleid, killerid)
{
    Vehicle_SetDestroyed(vehicleid, true);
    return 1;
}

stock Vehicle_Respawn(vehicleid)
{
	SetVehicleToRespawn(vehicleid);
	Vehicle_SetEngineStatus(vehicleid, false);
	Vehicle_SetLightState(vehicleid, false);
	Vehicle_SetFuel(vehicleid, Vehicle_GetFuel(vehicleid));
	SetVehicleZAngle(vehicleid, VehicleInfo[vehicleid][A]);
	SetVehiclePos(vehicleid, VehicleInfo[vehicleid][X], VehicleInfo[vehicleid][Y], VehicleInfo[vehicleid][Z]);
	SetVehicleVirtualWorld(vehicleid, VehicleInfo[vehicleid][World]);
	ChangeVehicleColor(vehicleid, Vehicle_GetColor1(vehicleid), Vehicle_GetColor2(vehicleid));
	return 1;
}

stock Vehicle_Create(modelid, Float:x, Float:y, Float:z, Float:rotation, color1, color2, respawn_delay=-1, addsiren=0)
{
    new vehicleid = CreateVehicle(modelid, x, y, z, rotation, color1, color2, respawn_delay, addsiren);
	if(vehicleid <= 0)
		return -1;
	Vehicle_Reset(vehicleid);
	VehicleInfo[vehicleid][OwnerID] = 0;
	VehicleInfo[vehicleid][Model] = modelid;
	VehicleInfo[vehicleid][Color1] = color1;
	VehicleInfo[vehicleid][Color2] = color2;
	VehicleInfo[vehicleid][X] = x;
	VehicleInfo[vehicleid][Y] = y;
	VehicleInfo[vehicleid][Z] = z;
	VehicleInfo[vehicleid][A] = rotation;
	Vehicle_SetMaxHealth(vehicleid, 999.0);
	Vehicle_SetLocked(vehicleid, false);
	Vehicle_SetEngineStatus(vehicleid, false);
	Vehicle_SetFaction(vehicleid, INVALID_FACTION_ID);
	Vehicle_SetFuel(vehicleid, 100.0);
	SetVehicleParamsEx(vehicleid, 0, 0, 0, 0, 0, 0, 0);
	CallLocalFunction(#OnVehicleCreate, "d", vehicleid);
	//Vehicle_UpdateLockState(vehicleid);
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
	new String:query = str_format("INSERT INTO `vehicles` (owner_id, model, color_1, color_2, x, y, z, angle, locked, faction, price, last_health, max_health) \
		VALUES('%d', '%d', '%d', '%d', '%f', '%f', '%f', '%f', '0', '%d', '%d', '999.0', '%f')",
		Vehicle_GetOwnerID(vehicleid), GetVehicleModel(vehicleid), Vehicle_GetColor1(vehicleid), Vehicle_GetColor2(vehicleid), x, y, z, a, Vehicle_GetFaction(vehicleid),
		Vehicle_GetPrice(vehicleid), Vehicle_GetMaxHealth(vehicleid));
	
	await mysql_aquery_s(gMySQL, query, .parallel=true);
	
	
	VehicleInfo[vehicleid][ID] = cache_insert_id();
	VehicleInfo[vehicleid][Model] = GetVehicleModel(vehicleid);
	VehicleInfo[vehicleid][X] = x;
	VehicleInfo[vehicleid][Y] = y;
	VehicleInfo[vehicleid][Z] = z;
	VehicleInfo[vehicleid][A] = a;
	Vehicle_SetLocked(vehicleid, false);
	Vehicle_SetFuel(vehicleid, 100.0);
	Vehicle_SetTemporary(vehicleid, false);
	Vehicle_InitializeInventory(vehicleid);
	Vehicle_GeneratePlate(vehicleid);

	map_add(VehiclesMap, VehicleInfo[vehicleid][ID], vehicleid);
	return 1;
}

stock Vehicle_Destroy(vehicleid)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;
	new id = VehicleInfo[vehicleid][ID];
	if(id != 0 && map_has_key(VehiclesMap, id))
		map_remove(VehiclesMap, id);
	
    Vehicle_Reset(vehicleid);
	DestroyVehicle(vehicleid);
	return 1;
}

stock Vehicle_Reset(vehicleid)
{
	VehicleInfo[vehicleid][ID] = 0;
    VehicleInfo[vehicleid][OwnerID] = 0;
    VehicleInfo[vehicleid][Model] = 0;
    VehicleInfo[vehicleid][Color1] = 0;
    VehicleInfo[vehicleid][Color2] = 0;
    VehicleInfo[vehicleid][X] = 0.0;
    VehicleInfo[vehicleid][Y] = 0.0;
    VehicleInfo[vehicleid][Z] = 0.0;
    VehicleInfo[vehicleid][A] = 0.0;
	VehicleInfo[vehicleid][Faction] = INVALID_FACTION_ID;
	VehicleDamages[vehicleid] = CleanVehicleDamages;
	Vehicle_SetTemporary(vehicleid, false);
    Vehicle_SetLocked(vehicleid, false);
	Vehicle_SetLastChopShopTime(vehicleid, 0);
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
	VehicleInfo[vehicleid][World] = GetVehicleVirtualWorld(vehicleid);
    if(Vehicle_GetID(vehicleid) != 0)
		mysql_pquery_f(gMySQL, "UPDATE `vehicles` SET x = '%f', y = '%f', z = '%f', angle = '%f', world = '%d' WHERE id = '%d'", nx, ny, nz, na, GetVehicleVirtualWorld(vehicleid), Vehicle_GetID(vehicleid));
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

/*stock Vehicle_UpdateLockState(vehicleid)
{
    if(IsABike(vehicleid) || IsAMotorBike(vehicleid))
	   return 1;
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    SetVehicleParamsEx(vehicleid, engine, lights, alarm, Vehicle_IsLocked(vehicleid) ? VEHICLE_PARAMS_ON : VEHICLE_PARAMS_OFF, bonnet, boot, objective);
    if(Vehicle_IsLocked(vehicleid))
	   Vehicle_SetDoorStatus(vehicleid, 0, 0, 0, 0);
    return 1;
}*/

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

	mysql_pquery_f(gMySQL, "DELETE FROM `vehicles` WHERE id = '%d'", Vehicle_GetID(vehicleid));

	//mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `vehicle_inventory` WHERE vehicle_id = '%d'", Vehicle_GetID(vehicleid));
	//mysql_tquery(gMySQL, query);

	Vehicle_Destroy(vehicleid);
	return 1;
}

// TODO: This doesn't work neither
stock Vehicle_GeneratePlate(vehicleid)
{
	new tempPlate[9];
	static const RandomLetters[26][5] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};
	format(tempPlate, sizeof(tempPlate), "%d%s%s%s%d", random(10), RandomLetters[random(26)], RandomLetters[random(26)], RandomLetters[random(26)], vehicleid);
	
	inline OnPlate()
	{
		if(cache_num_rows() > 0)
			return Vehicle_GeneratePlate(vehicleid);
		Vehicle_SetPlate(vehicleid, tempPlate);
		Vehicle_Respawn(vehicleid);
	}
	MySQL_TQueryInline(gMySQL, using inline OnPlate, "SELECT id FROM vehicles WHERE Plate = '%e'", tempPlate);
	return 1;
}

stock Vehicle_SetPlate(vehicleid, plate[])
{
	if(Vehicle_GetID(vehicleid) <= 0)
		return 0;
	set(VehicleInfo[vehicleid][Plate], plate);
	mysql_pquery_f(gMySQL, "UPDATE `vehicles` SET plate = '%e' WHERE id = '%d'", plate, Vehicle_GetID(vehicleid));
	return SetVehicleNumberPlate(vehicleid, plate);
}

stock Vehicle_Despawn(vehicleid)
{
	if(!Vehicle_GetID(vehicleid) || Vehicle_IsTemporary(vehicleid))
		return 0;
	
	CallLocalFunction(#OnVehicleUnload, "d", vehicleid);

	Vehicle_SaveStatus(vehicleid);
	
	mysql_pquery_f(gMySQL, "UPDATE `vehicles` SET spawned = '0' WHERE id = '%d'", Vehicle_GetID(vehicleid));
	Vehicle_Destroy(vehicleid);

	CallLocalFunction(#OnVehicleUnloaded, "d", vehicleid);
	return 1;
}

stock bool:Vehicle_IsOwner(vehicleid, playerid, bool:onlyEffectiveOwner)
{
	if(Vehicle_GetOwnerID(vehicleid) == Character_GetID(playerid) || Player_IsAdminDuty(playerid))
		return true;
	if(!onlyEffectiveOwner)
	{
		new JobType:vehicleJob = Vehicle_GetJob(vehicleid), JobType:characterJob = Character_GetJobType(playerid);
		if(vehicleJob != JOB_TYPE_NONE && characterJob != JOB_TYPE_NONE && vehicleJob == characterJob)
			return true;
		new vehicleFaction = Vehicle_GetFaction(vehicleid), characterFaction = Character_GetFaction(playerid);
		if(Faction_IsValid(vehicleFaction) && Faction_IsValid(characterFaction) && characterFaction == vehicleFaction)
			return true;
	}
	return false;
}

forward Vehicle_LoadAll();
public Vehicle_LoadAll()
{
	new 
		count = 0, dbid = 0, modelid = 0, ownerid = 0, locked = 0,
		Float:health = 0.0, bool:alreadyLoaded = false;
	new Float:x, Float:y, Float:z, Float:a, color1, color2, factionid;
	
	count = cache_num_rows();
	for(new i = 0; i < count; ++i)
	{
		alreadyLoaded = false;
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

		cache_get_value_name_float(i, "x", x);
		cache_get_value_name_float(i, "y", y);
		cache_get_value_name_float(i, "z", z);
		cache_get_value_name_float(i, "angle", a);
		cache_get_value_name_int(i, "color_1", color1);
		cache_get_value_name_int(i, "color_2", color2);
		cache_get_value_name_int(i, "faction", factionid);
		
		new addsiren = (Faction_IsValid(factionid) && Faction_GetType(factionid) == FACTION_TYPE_POLICE);

		new vehicleid = Vehicle_Create(modelid, x, y, z, a, color1, color2, -1, addsiren);
		
		VehicleInfo[vehicleid][ID] = dbid;
		VehicleInfo[vehicleid][OwnerID] = ownerid;
		VehicleInfo[vehicleid][Color1] = color1;
		VehicleInfo[vehicleid][Color2] = color2;
		VehicleInfo[vehicleid][Faction] = factionid;

		cache_get_value_name_int(i, "world", VehicleInfo[vehicleid][World]);
		
		cache_get_value_name_int(i, "locked", locked);
		
		cache_get_value_name_float(i, "fuel", VehicleFuel[vehicleid]);	  
		
		cache_get_value_name_int(i, "last_chop_shop_time", VehicleInfo[vehicleid][LastChopShopTime]);
		cache_get_value_name(i, "plate", VehicleInfo[vehicleid][Plate]);
		cache_get_value_name_int(i, "price", VehicleInfo[vehicleid][Price]);

		cache_get_value_name_float(i, "last_health", health);
		cache_get_value_name_float(i, "max_health", VehicleInfo[vehicleid][MaxHealth]);
		
		cache_get_value_name_int(i, "doors", VehicleDamages[vehicleid][E_DAMAGE_TYPE_DOORS]);
		cache_get_value_name_int(i, "panels", VehicleDamages[vehicleid][E_DAMAGE_TYPE_PANELS]);
		cache_get_value_name_int(i, "lights", VehicleDamages[vehicleid][E_DAMAGE_TYPE_LIGHTS]);
		cache_get_value_name_int(i, "tires", VehicleDamages[vehicleid][E_DAMAGE_TYPE_TIRES]);
		
		SetVehicleNumberPlate(vehicleid, VehicleInfo[vehicleid][Plate]);
		ChangeVehicleColor(vehicleid, color1, color2);

		SetVehicleToRespawn(vehicleid);

		SetVehicleZAngle(vehicleid, a);
		SetVehiclePos(vehicleid, x, y, z);
		SetVehicleVirtualWorld(vehicleid, VehicleInfo[vehicleid][World]);


		Vehicle_SetDestroyed(vehicleid, false);
		Vehicle_SetEngineStatus(vehicleid, false);
		Vehicle_SetLocked(vehicleid, locked > 0 ? true : false);

		SetVehicleHealth(vehicleid, health);
		UpdateVehicleDamageStatus(vehicleid, VehicleDamages[vehicleid][E_DAMAGE_TYPE_PANELS], VehicleDamages[vehicleid][E_DAMAGE_TYPE_DOORS], VehicleDamages[vehicleid][E_DAMAGE_TYPE_LIGHTS], VehicleDamages[vehicleid][E_DAMAGE_TYPE_TIRES]);

		mysql_pquery_f(gMySQL, "UPDATE `vehicles` SET spawned = 1 WHERE id = '%d';", dbid);

		if(Faction_IsValid(Vehicle_GetFaction(vehicleid)) || Vehicle_GetJob(vehicleid) != JOB_TYPE_NONE)
			Vehicle_Repair(vehicleid);

		map_add(VehiclesMap, VehicleInfo[vehicleid][ID], vehicleid);

		CallLocalFunction(#OnVehicleLoaded, "d", vehicleid);
	}
	return 1;
}

flags:avplate(CMD_MODERATOR);
CMD:avplate(playerid, params[])
{
	new vehicleid, plate[9];
	if(sscanf(params, "ds[8]", vehicleid, plate))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /avplate <vehicleid> <testo (max 8)>");

	if(!Vehicle_IsValid(vehicleid))
		return SendClientMessage(playerid, COLOR_ERROR, "L'id del veicolo inserito non � valido.");
	
	inline OnLoad()
	{
		if(cache_num_rows() > 0)
		{
			new id;
			cache_get_value_name_int(0, "id", id);
			SendFormattedMessage(playerid, COLOR_ERROR, "Esiste gi� un veicolo (ID database: %d) con questa targa.", id);
			return 0;
		}
		Vehicle_SetPlate(vehicleid, plate);
		Vehicle_Respawn(vehicleid);

		SendFormattedMessage(playerid, -1, "Hai settato la targa del veicolo id %d ed � stato respawnato. Testo: %s", vehicleid, plate);
		Log(Account_GetName(playerid), plate, "/avplate", vehicleid);
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT id FROM `vehicles` WHERE plate = '%e'", plate);

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
	return Faction_IsValid(Vehicle_GetFaction(vehicleid));
}

stock Vehicle_SetFaction(vehicleid, factionid)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;
	VehicleInfo[vehicleid][Faction] = factionid;
	if(Vehicle_GetID(vehicleid) != 0)
		mysql_pquery_f(gMySQL, "UPDATE `vehicles` SET faction = '%d' WHERE id = '%d'", Faction_GetID(factionid), Vehicle_GetID(vehicleid));
	return 1;
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
	if(Vehicle_GetID(vehicleid) != 0)
		mysql_pquery_f(gMySQL, "UPDATE `vehicles` SET owner_id = '%d' WHERE id = '%d'", Vehicle_GetOwnerID(vehicleid), Vehicle_GetID(vehicleid));
	return 1;
}

stock Vehicle_SetTemporary(vehicleid, bool:istemporary)
{
	Bit_Set(VehicleBitState[e_vTemporary], vehicleid, istemporary);
}

stock Vehicle_IsTemporary(vehicleid)
{
	return Bit_Get(VehicleBitState[e_vTemporary], vehicleid);
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
		mysql_pquery_f(gMySQL, "UPDATE `vehicles` SET last_chop_shop_time = '%d' WHERE id = '%d'", time, Vehicle_GetID(vehicleid));
	return 1;
}

stock Vehicle_SetLocked(vehicleid, bool:locked)
{
	Bit_Set(VehicleBitState[e_vLocked], vehicleid, locked);
	if(Vehicle_GetID(vehicleid) != 0)
		mysql_pquery_f(gMySQL, "UPDATE `vehicles` SET locked = '%d' WHERE id = '%d'", locked?1:0, Vehicle_GetID(vehicleid));
	return 1;
}

stock Vehicle_IsLocked(vehicleid)
{
    return Bit_Get(VehicleBitState[e_vLocked], vehicleid);
}

stock Vehicle_SetDestroyed(vehicleid, bool:destroyed)
{
	Bit_Set(VehicleBitState[e_vDestroyed], vehicleid, destroyed);
}

stock Vehicle_IsDestroyed(vehicleid)
{
    return Bit_Get(VehicleBitState[e_vDestroyed], vehicleid);
}

// Updates Vehicle Params too.
stock Vehicle_SetEngineStatus(vehicleid, bool:engineStatus)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;
	Bit_Set(VehicleBitState[e_vEngine], vehicleid, engineStatus);
	new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    SetVehicleParamsEx(vehicleid, engineStatus ? VEHICLE_PARAMS_ON : VEHICLE_PARAMS_OFF, lights, alarm, doors, bonnet, boot, objective);
	
	if(IsABike(vehicleid))
		SetVehicleParamsEx(vehicleid, VEHICLE_PARAMS_ON, lights, alarm, doors, bonnet, boot, objective);

	if(Vehicle_GetID(vehicleid) != 0)
		mysql_pquery_f(gMySQL, "UPDATE `vehicles` SET engine = '%d' WHERE id = '%d'", engineStatus?VEHICLE_PARAMS_ON : VEHICLE_PARAMS_OFF, Vehicle_GetID(vehicleid));

	return 1;
}

stock Vehicle_GetEngineStatus(vehicleid)
{
	return Bit_Get(VehicleBitState[e_vEngine], vehicleid);
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
		mysql_pquery_f(gMySQL, "UPDATE `vehicles` SET color_1 = '%d', color_2 = '%d' WHERE id = '%d'", color1, color2, Vehicle_GetID(vehicleid));

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
		mysql_pquery_f(gMySQL, "UPDATE `vehicles` SET fuel = '%f' WHERE id = '%d'", setValue, Vehicle_GetID(vehicleid));
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
		mysql_pquery_f(gMySQL, "UPDATE `vehicles` SET price = '%d' WHERE id = '%d'", price, Vehicle_GetID(vehicleid));
	return 1;
}

stock Float:Vehicle_GetMaxHealth(vehicleid)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0.0;
	return VehicleInfo[vehicleid][MaxHealth];
}

stock Vehicle_SetMaxHealth(vehicleid, Float:health)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;
	VehicleInfo[vehicleid][MaxHealth] = health;
	if(Vehicle_GetID(vehicleid) != 0)
		mysql_pquery_f(gMySQL, "UPDATE `vehicles` SET max_health = '%f' WHERE id = '%d'", health, Vehicle_GetID(vehicleid));
	return 1;
}

stock Vehicle_Reload(vehicleid)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;

	Vehicle_ResetStats(vehicleid);
	

	SetVehicleZAngle(vehicleid, VehicleInfo[vehicleid][A]);
	SetVehiclePos(vehicleid, VehicleInfo[vehicleid][X], VehicleInfo[vehicleid][Y], VehicleInfo[vehicleid][Z]);
	
	SetVehicleVirtualWorld(vehicleid, VehicleInfo[vehicleid][World]);

	ChangeVehicleColor(vehicleid, Vehicle_GetColor1(vehicleid), Vehicle_GetColor2(vehicleid));

	Vehicle_SetEngineStatus(vehicleid, false);

	if(Vehicle_IsDestroyed(vehicleid) /*&& !Faction_IsValid(Vehicle_GetFaction(vehicleid))*/)
	{
		Vehicle_SetDestroyed(vehicleid, false);
		SetVehicleHealth(vehicleid, VEHICLE_MIN_HEALTH);
	}
	else
	{
		SetVehicleHealth(vehicleid, Vehicle_GetMaxHealth(vehicleid));
	}

	if(Vehicle_HasFaction(vehicleid))
	{
		Vehicle_Repair(vehicleid);
	}
	
	if(IsABike(vehicleid))
		Vehicle_SetEngineStatus(vehicleid, true);
	return 1;
}

stock Vehicle_SaveStatus(i)
{
	if(!Vehicle_IsValid(i) || Vehicle_GetID(i) <= 0)
		return 0;
	new Float:health;
	GetVehicleHealth(i, health);
	GetVehicleDamageStatus(i, VehicleDamages[i][E_DAMAGE_TYPE_PANELS], VehicleDamages[i][E_DAMAGE_TYPE_DOORS], VehicleDamages[i][E_DAMAGE_TYPE_LIGHTS], VehicleDamages[i][E_DAMAGE_TYPE_TIRES]);
	mysql_pquery_f(gMySQL, "UPDATE `vehicles` SET last_health = '%f', panels = '%d', doors = '%d', lights = '%d', tires = '%d' WHERE id = '%d'", 
		health, VehicleDamages[i][E_DAMAGE_TYPE_PANELS], VehicleDamages[i][E_DAMAGE_TYPE_DOORS], VehicleDamages[i][E_DAMAGE_TYPE_LIGHTS], VehicleDamages[i][E_DAMAGE_TYPE_TIRES], Vehicle_GetID(i));
	return 1;
}

stock Vehicle_Repair(vehicleid)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;
	RepairVehicle(vehicleid);
	SetVehicleHealth(vehicleid, Vehicle_GetMaxHealth(vehicleid));
	return 1;
}

stock Vehicle_RemoveSign(vehicleid)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;
	if(IsValidDynamic3DTextLabel(VehicleSign[vehicleid]))
		DestroyDynamic3DTextLabelEx(VehicleSign[vehicleid]);
	VehicleSign[vehicleid] = Text3D:INVALID_3DTEXT_ID;
	return 1;
}

stock Vehicle_HasSign(vehicleid)
{
	return IsValidDynamic3DTextLabel(VehicleSign[vehicleid]);
}

stock Vehicle_SetSign(vehicleid, pos, const text[])
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;
	
	if(IsValidDynamic3DTextLabel(VehicleSign[vehicleid]))
		Update3DTextLabelText(VehicleSign[vehicleid], COLOR_WHITE, text);
	else
	{
		new model = Vehicle_GetModel(vehicleid);
		new Float:x, Float:y, Float:z, Float:un;

		GetVehicleModelInfo(model, VEHICLE_MODEL_INFO_WHEELSREAR, x, un, un);
		GetVehicleModelInfo(model, VEHICLE_MODEL_INFO_SIZE, un, y, un); y /= -2.0;
		GetVehicleModelInfo(model, VEHICLE_MODEL_INFO_REAR_BUMPER_Z, un, un, z);
		
		if(z <= -5.0 || z >= 5.0) z = 0.0;
		x += -0.2;
		y += 0.3;
		z += 0.0;
			
		switch(pos)
		{
			case 1: x = -x;
			case 2: x = 0.0;
		}
		VehicleSign[vehicleid] = CreateDynamic3DTextLabel(text, COLOR_WHITE, x, y, z, 20.0, INVALID_PLAYER_ID, vehicleid, 1);
	}
	return 1;
}

stock Vehicle_GetSpeed(vehicleid)
{
	if(!Vehicle_IsValid(vehicleid))
		return 0;
	static
        Float:x,
        Float:y,
        Float:z,
        vel;
    GetVehicleVelocity( vehicleid, x, y, z );
    vel = floatround( floatsqroot( x*x + y*y + z*z ) * 180 );
	return vel;
}

stock Player_GetSpeed(playerid)
{
	static
        Float:x,
        Float:y,
        Float:z,
        vel;
	GetPlayerVelocity(playerid, x, y, z);
    vel = floatround( floatsqroot( x*x + y*y + z*z ) * 180 );
	return vel;
}