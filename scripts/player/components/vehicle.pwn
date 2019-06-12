#include <YSI_Coding\y_hooks>

hook OnPlayerCharacterLoad(playerid)
{
	new query[256];
	mysql_format(gMySQL, query, sizeof(query), "SELECT * FROM `vehicles` WHERE OwnerID = '%d' AND Spawned = 0;", Character_GetID(playerid));
	mysql_tquery(gMySQL, query, #Vehicle_LoadAll);
	foreach(new i : Vehicle)
	{
		if(Vehicle_GetOwnerID(i) == Character_GetID(playerid))
			Vehicle_CancelDespawn(i);
	}
	return 1;
}

hook OnCharacterSaveData(playerid)
{
	Character_SaveAllVehicles(playerid);
	return 1;
}

hook OnPlayerClearData(playerid)
{
	PlayerTextDrawHide(playerid, pVehicleFuelText[playerid]);	
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook GlobalPlayerSecondTimer(playerid)
{
	new vehicleid = GetPlayerVehicleID(playerid);
	if(vehicleid > 0)
	{
		new string[18];
		format(string, sizeof(string), "~y~%0.1f%s", Vehicle_GetFuel(vehicleid), "%");
		PlayerTextDrawSetString(playerid, pVehicleFuelText[playerid], string);
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnCharacterDisconnected(playerid)
{
	foreach(new v : Vehicle)
	{
		if(!Vehicle_GetID(v) || Vehicle_GetOwnerID(v) != Character_GetID(playerid) || Vehicle_GetModel(v) == 0 || Vehicle_GetFaction(v) != INVALID_FACTION_ID)
			continue;
		Vehicle_SetToDespawn(v, 60);
	}
	return 1;
}

hook OnPlayerExitVehicle(playerid, vehicleid)
{
	new seat = GetPlayerVehicleSeat(playerid);
	if(Vehicle_IsLocked(vehicleid))
	{
		if(Vehicle_GetOwnerID(vehicleid) != Character_GetID(playerid))// && (!IsABike(vehicleid) && !IsAMotorBike(vehicleid)))
		{
			PutPlayerInVehicle(playerid, vehicleid, seat);
			GameTextForPlayer(playerid, "~r~Veicolo chiuso", 1000, 1);
		}
		/*else
		{
			new r[8];
			valstr(r, vehicleid);
			pc_cmd_vapri(playerid, r);
		}*/
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	printf("OnPlayerEnterVehicle - %d - %d", vehicleid, Vehicle_IsLocked(vehicleid) ? 1 : 0);
	if(Vehicle_IsLocked(vehicleid))
	{
		/*if(Vehicle_IsOwner(vehicleid, playerid, false))
		{
			new r[8];
			valstr(r, vehicleid);
			pc_cmd_vapri(playerid, r);
		}
		else
		{*/
			new Float:x, Float:y, Float:z;
			GetPlayerPos(playerid, x, y, z);
			SetPlayerPos(playerid, x, y, z);
			GameTextForPlayer(playerid, "~r~Veicolo chiuso", 5000, 1);
			//return 0;
		//}
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerStateChange(playerid, newstate, oldstate)
{
	if(newstate == PLAYER_STATE_DRIVER)
    {
		new vehicleid = GetPlayerVehicleID(playerid);
		if(VehicleInfo[vehicleid][vModel] != 0) // Probably a useless check
		{
			if(!IsABike(vehicleid))
			{
				PlayerTextDrawShow(playerid, pVehicleFuelText[playerid]);
			}
			if(Vehicle_GetOwnerID(vehicleid) > 0)
			{
				new despawnTime = Vehicle_GetSpawnExpiry(vehicleid);
				if(Vehicle_IsDespawn(vehicleid) && gettime() < despawnTime && Vehicle_GetFaction(vehicleid) == INVALID_FACTION_ID)
				{
					SendTwoLinesMessage(playerid, COLOR_ERROR, "(( Attenzione: Il proprietario del veicolo (%s) si è disconnesso. Il veicolo verrà despawnato in %d minuti. ))", VehicleInfo[vehicleid][vOwnerName], (despawnTime - gettime())/60);
				}
				new
					playerName[MAX_PLAYER_NAME];
				
				FixName(VehicleInfo[vehicleid][vOwnerName], playerName);
				SendFormattedMessage(playerid, COLOR_GREEN, "(( Questo veicolo (%s) appartiene a %s ))", Vehicle_GetName(vehicleid), playerName);
			}
		}
		SetPlayerArmedWeapon(playerid, 0);
	}
	else if(oldstate == PLAYER_STATE_DRIVER && newstate != PLAYER_STATE_DRIVER)
	{
		PlayerTextDrawHide(playerid, pVehicleFuelText[playerid]);
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(!Character_IsAlive(playerid))
		return Y_HOOKS_CONTINUE_RETURN_0;
	if(Account_HasHotKeysEnabled(playerid))
	{
		if(!gBuyingVehicle[playerid])
		{
			if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
			{
				new vehicleid = GetPlayerVehicleID(playerid);
				if( (PRESSED(KEY_YES) ) )
				{
					pc_cmd_motore(playerid, "");
				}
				else if( (PRESSED(KEY_LOOK_BEHIND)) && Vehicle_IsEngineOn(vehicleid))
				{
					Vehicle_SetLightState(vehicleid, !Vehicle_IsLightOn(vehicleid));
				}
			}
			if(PRESSED(KEY_NO))
			{
				new vehicleid = GetPlayerVehicleID(playerid);
				if(vehicleid <= 0)
					vehicleid = Character_GetClosestVehicle(playerid, 3.5);
				if(Vehicle_IsOwner(vehicleid, playerid, false)) //We're owners
				{
					if(Vehicle_IsLocked(vehicleid))
					{
						SendFormattedMessage(playerid, COLOR_GREEN, "Hai aperto il tuo veicolo (%s).", Vehicle_GetName(vehicleid));
						Character_AMe(playerid, "prende le chiavi e apre il suo veicolo.");
						Vehicle_UnLock(vehicleid);
					}
					else
					{
						SendFormattedMessage(playerid, COLOR_GREEN, "Hai chiuso il tuo veicolo (%s).", Vehicle_GetName(vehicleid));
						Character_AMe(playerid, "prende le chiavi e chiude il suo veicolo.");
						Vehicle_Lock(vehicleid);
					}
				}
			}
		}
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

stock Character_GetOwnedVehicleCount(playerid)
{
	new count = 0;
	foreach(new i : Vehicle)
	{
		if(Vehicle_GetOwnerID(i) == Character_GetID(playerid))
			count++;
	}
	return count;
}

stock Character_AddOwnedVehicle(playerid, vehicleid)
{
	if(!Vehicle_IsValid(vehicleid) || Character_GetOwnedVehicleCount(playerid) == MAX_VEHICLES_PER_PLAYER)
		return 0;
	printf("Added");
	Vehicle_SetOwnerID(vehicleid, Character_GetID(playerid));
	Vehicle_SetOwnerName(vehicleid, Character_GetOOCName(playerid));

	// If vehicle is temporary
	if(Vehicle_GetID(vehicleid) == 0 || Vehicle_IsTemporary(vehicleid))
	{
		printf("Is temporary");
		Vehicle_InsertInDatabase(vehicleid);
	}
	Vehicle_Save(vehicleid);
	return 1;
}

stock Character_SaveAllVehicles(playerid)
{
	foreach(new i : Vehicle)
	{
		if(!VehicleInfo[i][vID] || VehicleInfo[i][vModel] == 0 || VehicleInfo[i][vOwnerID] != PlayerInfo[playerid][pID])
			continue;
		Vehicle_Save(i);
	}
}

stock Character_GetClosestVehicle(playerid, Float:radius)
{
	if(IsPlayerInAnyVehicle(playerid))
		return GetPlayerVehicleID(playerid);

	new 
		Float:x, Float:y, Float:z;
	foreach(new i : Vehicle)
	{
		GetVehiclePos(i, x, y, z);
		if(IsPlayerInRangeOfPoint(playerid, radius, x, y, z))
		{
			return i;
		}
	}
    return 0;
}