#include <YSI_Coding\y_hooks>
#include <player\pickup.pwn>
#include <player\accessors.pwn>

hook GlobalPlayerSecondTimer(playerid)
{
	if(PendingRequestInfo[playerid][rdPending])
	{
		if(GetTickCount() - PendingRequestInfo[playerid][rdTime] > PENDING_REQUEST_TIME * 60 * 1000) // 60 seconds
		{
			new byPlayer = PendingRequestInfo[playerid][rdByPlayer];
			SendFormattedMessage(playerid, COLOR_ERROR, "La richiesta da %s (%d) è scaduta.", Character_GetOOCName(byPlayer), byPlayer);
			ResetPendingRequest(playerid);
	
			SendFormattedMessage(byPlayer, COLOR_ERROR, "La richiesta inviata a %s (%d) è scaduta.", Character_GetOOCName(playerid), playerid);
			ResetPendingRequest(byPlayer);
		}
	}

	if(Character_IsJailed(playerid))
	{
		PlayerInfo[playerid][pJailTime]--;
		PlayerTextDrawSetStringStr(playerid, pJailTimeText[playerid], str_format("~n~~n~~g~TEMPO RIMANENTE: ~w~~n~%d secondi", Character_GetJailTime(playerid)));
		if(!Character_IsJailed(playerid))
		{
			if(Character_IsICJailed(playerid))
			{
				SendClientMessage(playerid, COLOR_GREEN, "Hai scontato la tua pena. Digita /lasciacarcere al punto per uscire.");
				Player_Info(playerid, "Digita ~y~/lasciacarcere~w~ per usicre.", true, 3000);
			}
			else
			{
				SendClientMessage(playerid, COLOR_GREEN, "(( Hai scontato la tua pena. ))");
				Character_Spawn(playerid);
			}
			PlayerTextDrawHide(playerid, pJailTimeText[playerid]);
			Character_Save(playerid);
		}
	}

	new vehicleid = GetPlayerVehicleID(playerid);
	if(vehicleid > 0)
	{
		new string[18];
		format(string, sizeof(string), "~y~%0.1f%s", Vehicle_GetFuel(vehicleid), "%");
		PlayerTextDrawSetString(playerid, pVehicleFuelText[playerid], string);
	}
	return 1;
}


hook OnCharacterPreSaveData(playerid, disconnect)
{
	if(disconnect)
	{
		if(pAdminDuty[playerid])
		{
			AC_SetPlayerHealth(playerid, 100.0);
			AC_SetPlayerArmour(playerid, 0.0);
		}
	}
}

hook OnPlayerClearData(playerid)
{
    printf("player.pwn/OnPlayerClearData");
    Character_SetVehsDestroyTime(playerid);
    Character_Clear(playerid);
	if(PendingRequestInfo[playerid][rdPending])
	{
		new toPlayer = PendingRequestInfo[playerid][rdToPlayer],
			fromPlayer = PendingRequestInfo[playerid][rdByPlayer];
		if(PendingRequestInfo[toPlayer][rdPending] && PendingRequestInfo[toPlayer][rdByPlayer] == playerid)
		{
			SendFormattedMessage(toPlayer, COLOR_ERROR, "La richiesta di %s (%d) è stata annullata poiché si è disconnesso!", Character_GetOOCName(playerid), playerid);
			ResetPendingRequest(toPlayer);
		}
		else if(PendingRequestInfo[fromPlayer][rdPending] && PendingRequestInfo[fromPlayer][rdToPlayer] == playerid)
		{
			SendFormattedMessage(fromPlayer, COLOR_ERROR, "La richiesta inviata a %s (%d) è stata annullata poiché si è disconnesso!", Character_GetOOCName(playerid), playerid);
			ResetPendingRequest(fromPlayer);
		}
	}
	ResetPendingRequest(playerid);
	Character_SetDraggedBy(playerid, INVALID_PLAYER_ID);
	stop pDragTimer[playerid];
	PlayerTextDrawHide(playerid, pVehicleFuelText[playerid]);
    return 1;
}

hook OnCharacterDisconnected(playerid)
{
    Character_Save(playerid, _, 1);
    return 1;
}

hook OnPlayerPickUpElmPickup(playerid, pickupid, elementid, E_ELEMENT_TYPE:type)
{
    if(type == ELEMENT_TYPE_BUILDING_ENTRANCE)
    {
		new buildingid = elementid,
			String:string;
		if(AccountInfo[playerid][aAdmin] > 1)
		{
			string += str_format("~r~ID:~w~ %d~n~", elementid);
		}
		if(Building_IsOwnable(buildingid))
		{
			if(Building_GetOwnerID(buildingid) != 0)
			{
				new bname[MAX_BUILDING_NAME], ownerName[MAX_PLAYER_NAME];
				Building_GetName(buildingid, bname);
				Building_GetOwnerName(buildingid, ownerName);
				string += str_format("~y~%s~n~~w~Proprietario: ~y~%s~w~.~n~", bname, ownerName);
			}
			else
			{
				new bname[MAX_BUILDING_NAME];
				Building_GetName(buildingid, bname);
				string += str_format("~y~%s~n~~g~Prezzo:~w~$%d~n~", bname, Building_GetPrice(buildingid));
			}
		}
		else
		{
			new bname[MAX_BUILDING_NAME];
			Building_GetName(buildingid, bname);
			string += str_format("~y~%s~n~", bname);
		}
		GameTextForPlayerStr(playerid, string, 2000, 5);
	}
	else if(type == ELEMENT_TYPE_HOUSE_ENTRANCE)
	{
		new houseid = elementid, String:string;
		if(AccountInfo[playerid][aAdmin] > 1)
		{
			string += str_format("~r~ID:~w~ %d~n~", houseid);
		}
		if(House_IsOwned(houseid))
		{
			string += str_format("~y~Proprietario: ~w~%S~n~", House_GetOwnerNameStr(houseid));
			if(House_GetOwnerID(houseid) == PlayerInfo[playerid][pID])
				{
				if(Bit_Get(gPlayerBitArray[e_pHotKeys], playerid))
						string += @("Premi ~b~~k~~SNEAK_ABOUT~~w~ (/casa) per il menu.~n~");
					else
						string += @("Digita ~b~/casa~w~ per il menu.~n~");
				}
		}
		else
		{
			string += str_format("~g~In vendita!~n~~g~Prezzo: ~w~$%d~n~", House_GetPrice(houseid));
		}
		string += str_format("~r~Interno:~w~ %d~n~", House_GetInteriorID(houseid));
		GameTextForPlayerStr(playerid, string, 2000, 5);
	}
	else if(type == ELEMENT_TYPE_DROP)
	{
		Player_Info(playerid, "Premi ~y~~k~~PED_DUCK~~w~ per interagire", false);
	}
    return 1;
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
			if(Vehicle_GetOwnerID(vehicleid) != 0)
			{
				if(gVehicleDestroyTime[vehicleid] != -1)
				{
					SendTwoLinesMessage(playerid, COLOR_ERROR, "(( Attenzione: Il proprietario del veicolo (%s) si è disconnesso. Il veicolo verrà despawnato in %d minuti. ))", VehicleInfo[vehicleid][vOwnerName], gVehicleDestroyTime[vehicleid]);
				}
				new
					playerName[MAX_PLAYER_NAME];
				
				FixName(VehicleInfo[vehicleid][vOwnerName], playerName);
				SendFormattedMessage(playerid, COLOR_GREEN, "(( Questo veicolo (%s) appartiene a %s ))", GetVehicleName(vehicleid), playerName);
			}

			if(Vehicle_IsEngineOff(vehicleid))
			{
				if(Bit_Get(gPlayerBitArray[e_pHotKeys], playerid))
				SendClientMessage(playerid, -1, "Premi {00FF00}Y{FFFFFF} (oppure digita {00FF00}/motore{FFFFFF}) per accendere il motore.");
				else 
					SendClientMessage(playerid, -1, "Digita {00FF00}/motore{FFFFFF} per accendere il motore.");
			}
		}
		SetPlayerArmedWeapon(playerid, 0);
    }
	else if(newstate == PLAYER_STATE_ONFOOT && oldstate == PLAYER_STATE_DRIVER)
	{
		new vehicleid = GetPlayerVehicleID(playerid), seat = GetPlayerVehicleSeat(playerid);
		if(vehicleid > 0 && VehicleInfo[vehicleid][vLocked])
		{
			if(VehicleInfo[vehicleid][vOwnerID] != PlayerInfo[playerid][pID] && (!IsABike(vehicleid) && !IsAMotorBike(vehicleid)))
			{
				PutPlayerInVehicle(playerid, vehicleid, seat);
				GameTextForPlayer(playerid, "~r~Veicolo chiuso", 1000, 1);
			}
			else
			{
				VehicleInfo[vehicleid][vLocked] = 0;
				Vehicle_UpdateLockState(vehicleid);
			}
		}
	}
	if(oldstate == PLAYER_STATE_DRIVER && newstate != PLAYER_STATE_DRIVER)
	{
		PlayerTextDrawHide(playerid, pVehicleFuelText[playerid]);
	}
}

hook OnPlayerRequestSpawn(playerid)
{
    if(!Character_IsLogged(playerid))
	   return 0;
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(newkeys & KEY_NO/* && (GetTickCount() - PlayerInfo[playerid][pKeyTick]) > 2500*/)
	{
		if(pAnimLoop{playerid} == true)
		{
			ClearAnimations(playerid);
			ApplyAnimation(playerid, "CARRY", "crry_prtial", 2.0, 0, 0, 0, 0, 0);
			TextDrawHideForPlayer(playerid, txtAnimHelper);
			pAnimLoop{playerid} = false;
		}
	}
	if(Bit_Get(gPlayerBitArray[e_pHotKeys], playerid))
	{
		if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER && !gBuyingVehicle[playerid])
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
		else
		{
			new elementId, E_ELEMENT_TYPE:type, pickupid = Character_GetLastPickup(playerid);
			if(Pickup_GetInfo(pickupid, elementId, type) && !IsPlayerInAnyVehicle(playerid) && IsPlayerInRangeOfPickup(playerid, pickupid, 2.0))
			{
				if(PRESSED(KEY_SECONDARY_ATTACK))
				{
					// Should I write an "OnInteract" callback?
					if(type == ELEMENT_TYPE_BUILDING_ENTRANCE || type == ELEMENT_TYPE_HOUSE_ENTRANCE)
					{
						Character_Enter(playerid, pickupid, elementId, type);
					}
					else if(type == ELEMENT_TYPE_BUILDING_EXIT || type == ELEMENT_TYPE_HOUSE_EXIT)
					{
						Character_Exit(playerid, pickupid, elementId, type);
					}
				}
				else if(PRESSED(KEY_WALK))
				{
					if(type == ELEMENT_TYPE_HOUSE_ENTRANCE || type == ELEMENT_TYPE_HOUSE_EXIT)
					{
						new houseid = elementId;
						if(House_GetOwnerID(houseid) == PlayerInfo[playerid][pID])
						{
								// REMEMBER TO EDIT /casa IF THIS DIALOG IS EDITED
							Dialog_Show(playerid, Dialog_House, DIALOG_STYLE_LIST, "Casa", "Apri/Chiudi Porta\nInventario\nDeposita Soldi\nRitira Soldi\nVendi\nVendi a Giocatore\nCambia Interior", "Continua", "Chiudi");
						}
					}
				}
			}
			else if( (PRESSED(KEY_NO)) && !gBuyingVehicle[playerid])
			{
				new vehicleid = GetPlayerVehicleID(playerid);
				if(vehicleid <= 0)
					vehicleid = Character_GetClosestVehicle(playerid, 3.5);
				if(Vehicle_GetOwnerID(vehicleid) == Character_GetID(playerid)) //We're owners
				{
					if((IsABike(vehicleid) || IsAMotorBike(vehicleid)) && Vehicle_IsEngineOn(vehicleid))
						SendClientMessage(playerid, -1, "Prima spegni il motore!");
					else
					{
						if(Vehicle_IsLocked(vehicleid))
						{
							SendFormattedMessage(playerid, COLOR_GREEN, "Hai aperto il tuo veicolo (%s).", GetVehicleName(vehicleid));
							if(IsABike(vehicleid) || IsAMotorBike(vehicleid))
							{
								//ApplyAnimation(playerid, "BOMBER", "BOM_Plant_Crouch_In", 4.1, 0, 0, 0, 0, 1000, 0);
								Character_AMe(playerid, "toglie la catena dal suo veicolo");
							}
							else
							{
								Character_AMe(playerid, "prende le chiavi e apre il suo veicolo");
							}
							Vehicle_UnLock(vehicleid);
						}
						else
						{
							SendFormattedMessage(playerid, COLOR_GREEN, "Hai chiuso il tuo veicolo (%s).", GetVehicleName(vehicleid));
							if(IsABike(vehicleid) || IsAMotorBike(vehicleid))
							{
							//ApplyAnimation(playerid, "BOMBER", "BOM_Plant_Crouch_In", 4.1, 0, 0, 0, 0, 1000, 0);
								Character_AMe(playerid, "prende la catena e l'attacca al veicolo");
							}
							else
							{
								Character_AMe(playerid, "prende le chiavi e chiude il suo veicolo");
							}
							Vehicle_Lock(vehicleid);
						}
					}
				}
			}
		}
	}
	return 1;
}

hook OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart)
{
	if(Character_IsDead(playerid)) return AC_SetPlayerHealth(playerid, 0.0);
	return 1;
}

hook OnPlayerDeath(playerid, killerid, reason)
{
	if(killerid != INVALID_PLAYER_ID)
		Log(Character_GetOOCName(playerid), Character_GetOOCName(killerid), "OnPlayerDeath", reason);
	
	SetPlayerDrunkLevel(playerid, 0);

	new deathState = Character_GetDeathState(playerid) < 2 ? Character_GetDeathState(playerid)+1 : 0;
	Character_SetDeathState(playerid, deathState);

	GetPlayerPos(playerid, PlayerDeathState[playerid][pDeathX], PlayerDeathState[playerid][pDeathY], PlayerDeathState[playerid][pDeathZ]);
	GetPlayerFacingAngle(playerid, PlayerDeathState[playerid][pDeathA]);

	PlayerDeathState[playerid][pDeathInt] = GetPlayerInterior(playerid);
	PlayerDeathState[playerid][pDeathWorld] = GetPlayerVirtualWorld(playerid);
	
	if(deathState == 1)
	{
		printf("Death State: 1");
		if(killerid != INVALID_PLAYER_ID)
			SendFormattedMessage(playerid, COLOR_ERROR, "Sei stato ucciso da \"%s\"", Character_GetOOCName(killerid));
		Character_SetDeathTime(playerid, GetTickCount());
	}
	return 1;
}

hook OnPlayerSpawn(playerid)
{
	if(IsPlayerNPC(playerid))
		return 1;

    if(!gAccountLogged[playerid] || !Character_IsLogged(playerid))
		return KickEx(playerid);
	
	SetPlayerSkin(playerid, PlayerInfo[playerid][pSkin]);

	if(Character_GetFightingStyle(playerid) == 0)
		Character_SetFightingStyle(playerid, FIGHT_STYLE_NORMAL);
	
	SetPlayerDrunkLevel(playerid, 0);
	SetPlayerSkillLevel(playerid, WEAPONSKILL_PISTOL, 1);
	SetPlayerSkillLevel(playerid, WEAPONSKILL_MICRO_UZI, 1);
	SetPlayerSkillLevel(playerid, WEAPONSKILL_SAWNOFF_SHOTGUN, 1);

	TextDrawShowForPlayer(playerid, Clock);
    
	stop pDragTimer[playerid];
	Character_SetDragged(playerid, false);
	Character_SetDraggedBy(playerid, INVALID_PLAYER_ID);
	Character_SetDragging(playerid, false);

	if(Character_IsDead(playerid))
	{
		new deathState = Character_GetDeathState(playerid);
		if(deathState == 1)
		{
			new Float:x, Float:y, Float:z, interior, world;

			DestroyDynamic3DTextLabelEx(PlayerDeathState[playerid][pDeathText]);
			PlayerDeathState[playerid][pDeathText] = CreateDynamic3DTextLabel("(( QUESTO GIOCATORE È IN SISTEMA MORTE ))", COLOR_ADMIN, 0, 0, 0.5, 30.0, playerid, INVALID_VEHICLE_ID, 1, -1, -1, -1, 100.0);
			AC_SetPlayerHealth(playerid, 20.0);
			AC_SetPlayerArmour(playerid, 0.0);

			SetCameraBehindPlayer(playerid);

			Character_GetDeathStateData(playerid, x, y, z, interior, world);
			
			SetPlayerPos(playerid, x, y, z);
			SetPlayerInterior(playerid, interior);
			SetPlayerVirtualWorld(playerid, world);

			ApplyAnimation(playerid, "WUZI", "CS_Dead_Guy", 4.0, 0, 0, 0, 1, 0, 1);

			Character_AMe(playerid, "è in gravi condizioni e sviene");

			SendClientMessage(playerid, COLOR_ERROR, "Sei in gravi condizioni, digita \"/accetta morte\" per accettare di morire");
		}
		else if(deathState == 2)
		{
			AC_ResetPlayerWeapons(playerid, true);
			DestroyDynamic3DTextLabelEx(PlayerDeathState[playerid][pDeathText]);
			
			PlayerDeathState[playerid][pDeathText] = Text3D:0;
			Character_SetDeathState(playerid, 0);
			Character_SetDeathTime(playerid, 0);

			Character_OffDuty(playerid);


			Character_Spawn(playerid);
		}
	}
    return 1;
}

stock Character_PayDay(playerid)
{
    new expForNewLevel = (PlayerInfo[playerid][pLevel]+1)*2;
    PlayerInfo[playerid][pExp]++;

    SendClientMessage(playerid, COLOR_YELLOW, "_______________________[PAYDAY]_____________________");
    //SendFormattedMessage(playerid, COLOR_YELLOW, );
    new paycheck = 0;

	if(AccountInfo[playerid][aPremium] > 0)
	{
		static const premiumPaycheck[] = {500, 1000, 1500};
		paycheck += premiumPaycheck[AccountInfo[playerid][aPremium]-1];
		SendFormattedMessage(playerid, COLOR_YELLOW, "Hai ricevuto $%d aggiuntivi come bonus Premium.", paycheck);
	}
	new factionid = Character_GetFaction(playerid);
	if(factionid != -1)
	{
		new salary = Faction_GetRankSalary(factionid, Character_GetRank(playerid)-1);
		if(salary > 0)
		{
			paycheck += salary;
			SendFormattedMessage(playerid, COLOR_YELLOW, "Hai ricevuto $%d aggiuntivi perché fai parte di una fazione!", salary);
		}
	}

	if(Character_GetLevel(playerid) < 4)
	{
		static const payCheck[] = {2000, 1000, 500};
		paycheck += payCheck[Character_GetLevel(playerid) - 1];
	}

	Character_AddPayCheck(playerid, paycheck);
	SendFormattedMessage(playerid, COLOR_YELLOW, "Stipendio: $%d.", Character_GetPayCheck(playerid));
	if(paycheck > 0)
		SendClientMessage(playerid, COLOR_YELLOW, "Dirigiti verso NOMEPOSTO per ritirare il tuo stipendio.");
    if(PlayerInfo[playerid][pExp] < expForNewLevel)
    {
	   SendFormattedMessage(playerid, COLOR_YELLOW, "Livello attuale: %d. Attualmente hai %d/%d punti esperienza.", PlayerInfo[playerid][pLevel], PlayerInfo[playerid][pExp], expForNewLevel);
    }
    else
    {
	   PlayerInfo[playerid][pLevel]++;
	   PlayerInfo[playerid][pExp] = 0;
	   SetPlayerScore(playerid, PlayerInfo[playerid][pLevel]);
	   expForNewLevel = (PlayerInfo[playerid][pLevel]+1) * 2;
	   SendFormattedMessage(playerid, COLOR_YELLOW, "Congratulazioni! Hai raggiunto il livello %d. Per il prossimo livello hai bisogno di %d punti esperienza.", PlayerInfo[playerid][pLevel], expForNewLevel);
    }
    SendClientMessage(playerid, COLOR_YELLOW, "____________________________________________________");
	//Character_GiveMoney(playerid, paycheck, "paycheck");
    Character_Save(playerid);
    GameTextForPlayer(playerid, "~y~PayDay", 5000, 1);
}

stock OnCharacterLoad(playerid)
{
    LoadCharacterResult(playerid);

    gCharacterLogged[playerid] = 1;

    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, PlayerInfo[playerid][pMoney]);

    SetPlayerScore(playerid, PlayerInfo[playerid][pLevel]);

    SetPlayerName(playerid, PlayerInfo[playerid][pName]); // disable to avoid GTA restart everytime during "debugging"
    gPlayerAMe3DText[playerid] = CreateDynamic3DTextLabel(" ", 0xFFFFFFFF, 0.0, 0.0, 0.60, 15.0, playerid, _, 1);
    
    AC_SetPlayerHealth(playerid, 100);

    SetSpawnInfo(playerid, NO_TEAM, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    SpawnPlayer(playerid);
    
    CallLocalFunction("OnPlayerCharacterLoad", "i", playerid);

	Character_Spawn(playerid);


	Character_SetFactionOOC(playerid, true);
	Player_SetOOCEnabled(playerid, true);
	Character_SetFactionOOC(playerid, true);

    return 1;
}

stock LoadCharacterResult(playerid)
{
    new count;
    cache_get_row_count(count);
    if(count > 0)
    {
		cache_get_value_index_int(0, 0, PlayerInfo[playerid][pID]);
		cache_get_value_index(0, 2, PlayerInfo[playerid][pName]);
		cache_get_value_index_int(0, 3, PlayerInfo[playerid][pMoney]);
		cache_get_value_index_int(0, 4, PlayerInfo[playerid][pLevel]);
		cache_get_value_index_int(0, 5, PlayerInfo[playerid][pAge]);
		cache_get_value_index_int(0, 6, PlayerInfo[playerid][pSex]);

		cache_get_value_index_float(0, 7, PlayerRestore[playerid][pLastX]);
		cache_get_value_index_float(0, 8, PlayerRestore[playerid][pLastY]);
		cache_get_value_index_float(0, 9, PlayerRestore[playerid][pLastZ]);
		cache_get_value_index_int(0, 10, PlayerRestore[playerid][pLastInterior]);
		cache_get_value_index_int(0, 11, PlayerRestore[playerid][pLastVirtualWorld]);

		cache_get_value_index_int(0, 12, PlayerRestore[playerid][pFirstSpawn]);

		cache_get_value_index_float(0, 13, PlayerRestore[playerid][pLastHealth]);
		cache_get_value_index_float(0, 14, PlayerRestore[playerid][pLastArmour]);

		cache_get_value_index_int(0, 15, PlayerInfo[playerid][pSkin]);

		cache_get_value_index_float(0, 16, PlayerRestore[playerid][pLastAngle]);
		cache_get_value_index_int(0, 17, PlayerRestore[playerid][pSpawned]);

		cache_get_value_index_int(0, 18, PlayerInfo[playerid][pPayDay]);
		cache_get_value_index_int(0, 19, PlayerInfo[playerid][pExp]);

		cache_get_value_index_int(0, 20, PlayerInfo[playerid][pBuildingKey]);
		cache_get_value_index_int(0, 21, PlayerInfo[playerid][pHouseKey]);

		cache_get_value_index_int(0, 22, PlayerInfo[playerid][pFaction]);
		cache_get_value_index_int(0, 23, PlayerInfo[playerid][pRank]);

		cache_get_value_index_int(0, 24, PlayerInfo[playerid][pJailTime]);

		cache_get_value_index_int(0, 25, PlayerInfo[playerid][pPayCheck]);
		return 1;
    }
    return 0;
}

stock Character_Save(playerid, spawned = true, disconnected = false)
{
    // This should never happen.
    if(!gAccountLogged[playerid] || !Character_IsLogged(playerid))
	   return 0;
    
    CallLocalFunction("OnCharacterPreSaveData", "ii", playerid, disconnected);
    
    new 
		query[1024],
		Float:_x, Float:_y, Float:_z,
		Float:angle,
		Float:hp, Float:armour,
		isSpawned = false;

    //if() 
    isSpawned = !Character_IsDead(playerid) && spawned;

    GetPlayerPos(playerid, _x, _y, _z);
    GetPlayerFacingAngle(playerid, angle);
    AC_GetPlayerHealth(playerid, hp);
    AC_GetPlayerArmour(playerid, armour);

    mysql_format(gMySQL, query, sizeof(query), "UPDATE `characters` SET \
	   	Money = '%d', Level = '%d', Age = '%d', Sex = '%d', \
	   	LastX = '%f', LastY = '%f', LastZ = '%f', LastAngle = '%f', LastInterior = '%d', LastVirtualWorld = '%d', Health = '%f', Armour = '%f', Skin = '%d', \
	   	Spawned = '%d', PayDay = '%d', Exp = '%d',  \
	   	BuildingKey = '%d', HouseKey = '%d', \
		Faction = '%d', FactionRank = '%d', \
		JailTime = '%d', PayCheck = '%d' \
	   	WHERE ID = '%d'", 
	   	Character_GetMoney(playerid), PlayerInfo[playerid][pLevel], PlayerInfo[playerid][pAge], PlayerInfo[playerid][pSex], 
	   	_x, _y, _z, angle, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid),
	   	hp, armour, PlayerInfo[playerid][pSkin],
	   	isSpawned, PlayerInfo[playerid][pPayDay], PlayerInfo[playerid][pExp],
	   	Character_GetBuildingKey(playerid), Character_GetHouseKey(playerid),
		Character_GetFaction(playerid), Character_GetRank(playerid),
		Character_GetJailTime(playerid), Character_GetPayCheck(playerid),
	   	Character_GetID(playerid));
    
    mysql_tquery(gMySQL, query);

    // Save Vehicles
    Character_SaveAllVehicles(playerid);
    // Save Others
	Character_SaveWeapons(playerid);
    // Save A' Mammt
    CallLocalFunction(#OnCharacterSaveData, "d", playerid);
    return 1;
}

stock Character_SaveWeapons(playerid)
{
	//Character_DeleteAllWeapons(playerid);
	new query[255];
	new weaponid, ammo;
	for(new i = 0; i < 13; ++i)
	{
		GetPlayerWeaponData(playerid, i, weaponid, ammo);
		if(weaponid == 0 || !AC_AntiWeaponCheck(playerid, weaponid, ammo))
			continue;
		format(query, sizeof(query), "INSERT INTO `player_weapons` (CharacterID, WeaponID, Ammo) VALUES('%d', '%d', '%d') ON DUPLICATE KEY UPDATE Ammo = VALUES(Ammo);", Character_GetID(playerid), weaponid, ammo);
		mysql_pquery(gMySQL, query);
	}
}

stock Character_LoadWeapons(playerid)
{
	inline OnLoad()
	{
		new rows = cache_num_rows(), w, a;
		for(new i = 0; i < rows; ++i)
		{
			cache_get_value_index_int(i, 0, w);
			cache_get_value_index_int(i, 1, a);
			if(! (0 <= w <= 46))
			{
				printf("Character_LoadWeapons(%d) failed. invalid weapon id (id %d)", playerid, w);
				continue;
			}
			AC_GivePlayerWeapon(playerid, w, a);
		}
		printf("Player %d weapons loaded!", playerid);
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT WeaponID, Ammo FROM `player_weapons` WHERE CharacterID = '%d'", Character_GetID(playerid));
}

stock Character_DeleteAllWeapons(playerid)
{
	new query[256];
	format(query, sizeof(query), "DELETE FROM `player_weapons` WHERE CharacterID = '%d'", Character_GetID(playerid));
	mysql_pquery(gMySQL, query);
	return 1;
}

stock Character_Delete(playerid, character_db_id, character_name[])
{
    #pragma unused playerid
    new query[256];
    mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `characters` WHERE ID = '%d' AND LOWER(Name) = LOWER('%e')", character_db_id, character_name);
    mysql_tquery(gMySQL, query);

    mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `character_inventory` WHERE CharacterID = '%d'", character_db_id);
    mysql_tquery(gMySQL, query);

	format(query, sizeof(query), "DELETE FROM `player_weapons` WHERE CharacterID = '%d'", character_db_id);
	mysql_pquery(gMySQL, query);

    inline OnVehicles()
    {
	   new count = cache_num_rows(), vid;
	   for(new i = 0; i < count; ++i)
	   {
		  cache_get_value_index_int(i, 0, vid);
		  mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `vehicle_inventory` WHERE VehicleID = '%d'", vid);
		  mysql_tquery(gMySQL, query);
	   }
	   mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `player_vehicles` WHERE OwnerID = '%d'", character_db_id);
	   mysql_tquery(gMySQL, query);
    }
    MySQL_TQueryInline(gMySQL, using inline OnVehicles, "SELECT ID FROM `player_vehicles` WHERE OwnerID = '%d'", character_db_id);


    foreach(new b : Buildings)
    {
	   if(Building_GetOwnerID(b) == character_db_id)
	   {
		  Building_ResetOwner(b);
		  break;
	   }
    }

    for(new i = 0; i < list_size(HouseList); ++i)
    {
	   new ownerid = House_GetOwnerID(i);
	   if(character_db_id == ownerid)
	   {
		  House_ResetOwner(i);
		  House_Save(i);
		  break;
	   }
    }

    // Delete all others

    return 1;
}

stock Character_GetOwnedVehicleCount(playerid)
{
	new count = 0;
	foreach(new i : Vehicles)
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
    new
	   Float:x, Float:y, Float:z, Float:a;
    
    GetVehiclePos(vehicleid, x, y, z);
    GetVehicleZAngle(vehicleid, a);
    inline OnInsert()
    {    
	   VehicleInfo[vehicleid][vID] = cache_insert_id();
	   VehicleInfo[vehicleid][vOwnerID] = PlayerInfo[playerid][pID];
	   set(VehicleInfo[vehicleid][vOwnerName], PlayerInfo[playerid][pName]);
	   VehicleInfo[vehicleid][vModel] = GetVehicleModel(vehicleid);
	   VehicleInfo[vehicleid][vColor1] = Vehicle_GetColor1(vehicleid);
	   VehicleInfo[vehicleid][vColor2] = Vehicle_GetColor2(vehicleid);
	   VehicleInfo[vehicleid][vX] = x;
	   VehicleInfo[vehicleid][vY] = y;
	   VehicleInfo[vehicleid][vZ] = z;
	   VehicleInfo[vehicleid][vA] = a;
	   VehicleInfo[vehicleid][vLocked] = 0;
	   gVehicleDestroyTime[vehicleid] = -1;
	   Vehicle_InitializeInventory(vehicleid);
	   mysql_tquery_f(gMySQL, "INSERT INTO `vehicle_inventory` (VehicleID) VALUES('%d')", VehicleInfo[vehicleid][vID]);
    }
    MySQL_TQueryInline(gMySQL, using inline OnInsert, "INSERT INTO `player_vehicles` (OwnerID, Model, Color1, Color2, X, Y, Z, Angle, Locked) \
	   VALUES('%d', '%d', '%d', '%d', '%f', '%f', '%f', '%f', '0')",
	   PlayerInfo[playerid][pID], GetVehicleModel(vehicleid), Vehicle_GetColor1(vehicleid), Vehicle_GetColor2(vehicleid), x, y, z, a);
	return 1;
}

stock Character_LoadVehicles(playerid)
{
    inline OnLoad()
    {
	   new count = 0, modelid = 0;
	   cache_get_row_count(count);

	   for(new i = 0; i < count; ++i)
	   {
		  new id = 0;
		  cache_get_value_index_int(i, 0, id);
		  cache_get_value_index_int(i, 2, modelid);
		  
		  if(id == 0 || modelid == 0)
			 continue;
		  
		  new alreadyLoaded = 0;

		  foreach(new v : Vehicles)
		  {
			 if(VehicleInfo[v][vID] == id)
			 {
				alreadyLoaded = v;
				break;
			 }
		  }
		  if(alreadyLoaded > 0)
		  {
			 gVehicleDestroyTime[alreadyLoaded] = -1;
			 continue;
		  }

		  new vehicleid = Vehicle_Create(modelid, 0, 0, 0, 0, 0, 0, 0, 0);
		  VehicleInfo[vehicleid][vID] = id;
		  cache_get_value_index_int(i, 1, VehicleInfo[vehicleid][vOwnerID]);
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

		  SetVehicleHealth(vehicleid, VehicleRestore[vehicleid][vLastHealth]);

		  set(VehicleInfo[vehicleid][vOwnerName], PlayerInfo[playerid][pName]);
		  
		  gVehicleDestroyTime[vehicleid] = -1;
		  
		  Vehicle_Reload(vehicleid);
		  CallLocalFunction(#OnPlayerVehicleLoaded, "d", vehicleid);
	   }
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `player_vehicles` WHERE OwnerID = '%d' ORDER BY ID", PlayerInfo[playerid][pID]);
}

stock Character_UnloadVehicles(playerid)
{
    foreach(new i : Vehicles)
    {
	   if(!VehicleInfo[i][vID] || VehicleInfo[i][vModel] == 0 || VehicleInfo[i][vOwnerID] != PlayerInfo[playerid][pID])
		  continue;
	   
	   Vehicle_Unload(i);
	   Iter_SafeRemove(Vehicles, i, i);
    }
    return 1;
}

stock Character_SaveAllVehicles(playerid)
{
    foreach(new i : Vehicles)
    {
	   if(!VehicleInfo[i][vID] || VehicleInfo[i][vModel] == 0 || VehicleInfo[i][vOwnerID] != PlayerInfo[playerid][pID])
		  continue;
	   Vehicle_Save(i);
    }
}

// Remember that this function doesn't return the same length everytime. 
// (MAX_PLAYER_NAME not allowed for checks outside)
stock GetPlayerNameEx(playerid)
{
    // Is it necessary? I don't think so
    if(!gAccountLogged[playerid] || !Character_IsLogged(playerid))
	   return "";

    new 
	   string[32];
    if(/* MASK */false)
    {
	   format(string, sizeof(string), "Sconosciuto #%d", PlayerInfo[playerid][pID]);
    }
    else
    {
	   GetPlayerName(playerid, string, MAX_PLAYER_NAME);
    }
    return string;
}

stock Character_AMe(playerid, text[], GLOBAL_TAG_TYPES:...)
{
    gPlayerAMeExpiry[playerid] = 4; // Seconds

    new 
	   textString[148],
	   temp[148];
    format(textString, sizeof(textString), text, ___2);

    format(temp, sizeof(temp), "* %s *", textString);
    SendTwoLinesMessage(playerid, 0xD0AEEBFF, "> %s %s ", Character_GetOOCName(playerid), textString); //0xD6C3E3FF
    
    format(temp, 120, "%s", textString);
	if(strlen(temp) > 43)
	    strins(temp, "-\n-", 43, sizeof(temp));
	if(strlen(temp) > 95)
	    strins(temp, "-\n-", 90, sizeof(temp));
	strins(temp, "* ", 0, sizeof(temp));
	strins(temp, " *", strlen(temp), sizeof(temp));

    UpdateDynamic3DTextLabelText(gPlayerAMe3DText[playerid], 0xD0AEEBFF, temp);
    

    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    foreach(new i : Player)
    {
	   if(IsPlayerInRangeOfPoint(playerid, 25.0, x, y, z))
		  Streamer_Update(i);
    }
    return 1;
}

stock Character_CharacterAMe(playerid, otherid, text[], GLOBAL_TAG_TYPES:...)
{
	new s[148];
	format(s, sizeof(s), "%s %s", Character_GetOOCName(playerid), text, Character_GetOOCName(otherid));
	return Character_AMe(playerid, s);
}

stock Character_Me(playerid, text[], GLOBAL_TAG_TYPES:...)
{
    if(!Character_IsLogged(playerid) || strlen(text) > 256)
	   return 0;
    new 
	   formattedText[256];
    format(formattedText, sizeof(formattedText), text, ___2);

    format(formattedText, sizeof(formattedText), "* %s %s", Character_GetOOCName(playerid), formattedText);
    ProxDetector(playerid, 15.0, formattedText, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF); //0xD6C3E3FF
    return 1;
}

stock Character_MeLow(playerid, text[], GLOBAL_TAG_TYPES:...)
{
    if(!Character_IsLogged(playerid) || strlen(text) > 256)
	   return 0;
    
    new String:str = str_format(text, ___2);
    str = str_format("* %s %S", Character_GetOOCName(playerid), str);
    ProxDetectorStr(playerid, 7.0, str, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF); //0xD6C3E3FF
    return 1;
}

stock Character_Do(playerid, text[], GLOBAL_TAG_TYPES:...)
{
    if(!Character_IsLogged(playerid) || strlen(text) > 256)
	   return 0;
    
    new String:str = str_format(text, ___2);
    str += str_format(" (( %s ))", Character_GetOOCName(playerid));
    ProxDetectorStr(playerid, 20.0, str, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF);
    return 1;
}

stock Character_DoLow(playerid, text[], GLOBAL_TAG_TYPES:...)
{
    if(!Character_IsLogged(playerid) || strlen(text) > 256)
	   return 0;
    
    new String:str = str_format(text, ___2);
    str += str_format(" (( %s ))", Character_GetOOCName(playerid));
    ProxDetectorStr(playerid, 7.0, str, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF);
    return 1;
}

stock Character_SetVehsDestroyTime(playerid)
{
    if(!Character_IsLogged(playerid))
	   return 0;
    foreach(new i : Vehicles)
    {
	   if(!VehicleInfo[i][vID] || !VehicleInfo[i][vModel] || VehicleInfo[i][vOwnerID] != PlayerInfo[playerid][pID])
		  continue;
	   gVehicleDestroyTime[i] = 60; // MINUTES
    }
    return 1;
}

stock Character_ShowStats(playerid, targetid)
{
    if(!Character_IsLogged(playerid))
	   return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato!");
    new 
	   Float:hp, Float:armour,
		expForNewLevel = (PlayerInfo[playerid][pLevel]+1) * 2;
    AC_GetPlayerHealth(playerid, hp);
    AC_GetPlayerArmour(playerid, armour);
    SendClientMessage(targetid, COLOR_GREEN, "_____________________[STATISTICHE]_____________________");
	SendFormattedMessage(targetid, -1, "[Account] Account: %s - E-Mail: %s", AccountInfo[playerid][aName], AccountInfo[playerid][aEMail]);
    SendFormattedMessage(targetid, -1, "[Personaggio] Nome: %s - Livello: %d - Skin: %d", Character_GetOOCName(playerid), Character_GetLevel(playerid), GetPlayerSkin(playerid));
	if(Character_GetFaction(playerid) != -1)
		SendClientMessageStr(targetid, -1, str_format("[Personaggio] Fazione: %S - Rank: %S", Faction_GetNameStr(Character_GetFaction(playerid)), Faction_GetRankNameStr(Character_GetFaction(playerid), Character_GetRank(playerid)-1)));
    SendFormattedMessage(targetid, -1, "[Denaro] Soldi: $%d - Stipendio: $%d", Character_GetMoney(playerid), Character_GetPayCheck(playerid));
    SendFormattedMessage(targetid, -1, "[Salute] HP: %.2f - Armatura: %.2f", hp, armour);
	SendFormattedMessage(targetid, -1, "[Altro] Interior: %d - VW: %d", GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));
    if(PlayerInfo[playerid][pBuildingKey] != 0 && PlayerInfo[playerid][pHouseKey] != 0)
	   SendFormattedMessage(targetid, COLOR_YELLOW, "[Proprietà]: Edificio: %d - Casa: %d", Character_GetBuildingKey(playerid), Character_GetHouseKey(playerid));
    else if(PlayerInfo[playerid][pBuildingKey] != 0)
	   SendFormattedMessage(targetid, COLOR_YELLOW, "[Proprietà]: Edificio: %d", Character_GetBuildingKey(playerid));
    else if(PlayerInfo[playerid][pHouseKey] != 0)
	   SendFormattedMessage(targetid, COLOR_YELLOW, "[Proprietà]: Casa: %d", Character_GetHouseKey(playerid));
	SendFormattedMessage(playerid, -1, "Ti mancano %d/%d punti esperienza per salire di livello.", PlayerInfo[playerid][pExp], expForNewLevel);
    SendFormattedMessage(targetid, COLOR_YELLOW, "Tempo rimanente al PayDay: %d minuti", 60 - PlayerInfo[playerid][pPayDay]);
    SendClientMessage(targetid, COLOR_GREEN, "_______________________________________________________");
    return 1;
}

stock Character_Clear(playerid)
{
    new CleanData[E_PLAYER_DATA];
    PlayerInfo[playerid] = CleanData;

    new CleanRestoreData[E_PLAYER_RESTORE_DATA];
    PlayerRestore[playerid] = CleanRestoreData;

	new CleanPlayerDeathState[e_DeathState];
	PlayerDeathState[playerid] = CleanPlayerDeathState;
}

stock Character_Enter(playerid, pickupid, elementId, E_ELEMENT_TYPE:type)
{
    if(!IsPlayerInRangeOfPickup(playerid, pickupid, 2.5) || (type != ELEMENT_TYPE_BUILDING_ENTRANCE && type != ELEMENT_TYPE_HOUSE_ENTRANCE))
	   return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata di un edificio!");
    new 
	   Float:x = 0.0, 
	   Float:y = 0.0, 
	   Float:z = 0.0,
	   interiorId = 0,
	   world = 0,
	   locked = 0;
    if(type == ELEMENT_TYPE_BUILDING_ENTRANCE)
    {
		x = BuildingInfo[elementId][bExitX]; 
		y = BuildingInfo[elementId][bExitY]; 
		z = BuildingInfo[elementId][bExitZ];
		interiorId = BuildingInfo[elementId][bExitInterior];
		world = BuildingInfo[elementId][bExitWorld];
		locked = BuildingInfo[elementId][bLocked];
		if(!locked)
		{
			new String:str = Building_GetWelcomeTextStr(elementId);
			if(str_len(str) > 0)
				SendClientMessageStr(playerid, COLOR_GREEN, str);	
		}
    }
    else if(type == ELEMENT_TYPE_HOUSE_ENTRANCE)
    {
	   House_GetExitPosition(elementId, x, y, z);
	   interiorId = House_GetExitInterior(elementId);
	   world = elementId;
	   locked = House_IsLocked(elementId);
    }
    if(locked)
	   return GameTextForPlayer(playerid, "~r~Chiuso", 8000, 1);
    
    Character_AMe(playerid, "apre la porta ed entra");
    Streamer_UpdateEx(playerid, x, y, z, world, interiorId);
    SetPlayerInterior(playerid, interiorId);
    SetPlayerVirtualWorld(playerid, world);
    SetPlayerPos(playerid, x, y, z);
    return 1;
}

stock Character_Exit(playerid, pickupid, elementId, E_ELEMENT_TYPE:type)
{
    if(!IsPlayerInRangeOfPickup(playerid, pickupid, 2.5) || (type != ELEMENT_TYPE_BUILDING_EXIT && type != ELEMENT_TYPE_HOUSE_EXIT))
	   return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'uscita di un edificio!");
    new 
	   Float:x = 0.0, 
	   Float:y = 0.0, 
	   Float:z = 0.0,
	   interiorId = 0,
	   world = 0,
	   locked = 0;
    
    if(type == ELEMENT_TYPE_BUILDING_EXIT)
    {
	   x = BuildingInfo[elementId][bEnterX]; 
	   y = BuildingInfo[elementId][bEnterY]; 
	   z = BuildingInfo[elementId][bEnterZ];
	   interiorId = BuildingInfo[elementId][bEnterInterior];
	   world = BuildingInfo[elementId][bEnterWorld];
	   locked = BuildingInfo[elementId][bLocked];
    }
    else if(type == ELEMENT_TYPE_HOUSE_EXIT)
    {
	   House_GetEnterPosition(elementId, x, y, z);
	   interiorId = House_GetEnterInterior(elementId);
	   world = House_GetEnterWorld(elementId);
	   locked = House_IsLocked(elementId);
    }
    if(locked)
	   return GameTextForPlayer(playerid, "~r~Chiuso", 8000, 1);
    Character_AMe(playerid, "apre la porta ed esce");
    Streamer_UpdateEx(playerid, x, y, z, world, interiorId);
    SetPlayerInterior(playerid, interiorId);
    SetPlayerVirtualWorld(playerid, world);
    SetPlayerPos(playerid, x, y, z);
    return 1;
}

stock Character_BuyBuilding(playerid, buildingid)
{
    if(!Building_IsValid(buildingid))
	   return 0;
    
    if(!Building_IsOwnable(buildingid) || Building_GetOwnerID(buildingid) != 0)
	   return SendClientMessage(playerid, COLOR_ERROR, "Questa proprietà non è in vendita!");

    if(Character_GetMoney(playerid) < Building_GetPrice(buildingid))
	   return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi per acquistare questa proprietà!");
    
    Character_GiveMoney(playerid, -BuildingInfo[buildingid][bPrice]);
    
    Building_SetOwner(buildingid, playerid);
    
    PlayerInfo[playerid][pBuildingKey] = BuildingInfo[buildingid][bID];

    SendFormattedMessage(playerid, COLOR_GREEN, "Hai acquistato questo edificio (%s) per $%d.", BuildingInfo[buildingid][bName], Building_GetPrice(buildingid));
    Building_Save(buildingid);
    Character_Save(playerid);
    return 1;
}

stock Character_BuyHouse(playerid, houseid)
{
    if(!House_IsValid(houseid))
	   return 0;

    if(House_GetOwnerID(houseid) != 0)
	   return SendClientMessage(playerid, COLOR_ERROR, "Questa proprietà non è in vendita!");

    if(Character_HasHouseKey(playerid))
	   return SendClientMessage(playerid, COLOR_ERROR, "Sei già proprietario di una casa!");

    new price = House_GetPrice(houseid);
    if(Character_GetMoney(playerid) < price)
	   return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi per acquistare questa proprietà!");

    House_SetOwner(houseid, playerid);

    Character_GiveMoney(playerid, -price, "Character_BuyHouse");

    SendFormattedMessage(playerid, COLOR_GREEN, "Congratulazioni! Hai acquistato questa casa per $%d!", price);

    House_Save(houseid);
    Character_Save(playerid);
    return 1;
}

// 283,75
// 284

stock Character_Spawn(playerid)
{
	PreloadAnimations(playerid);
	if(PlayerRestore[playerid][pFirstSpawn]) // First Login/Spawn
    {
		PlayerRestore[playerid][pFirstSpawn] = 0;

		Character_SetSkin(playerid, 46);

		SetPlayerPos(playerid, 1748.1887, -1860.0414, 13.5792);

		if(AccountInfo[playerid][aCharactersCount] <= 2)
		{
			static const characterFirstLoginMoney[] = {30000, 20000, 10000};
			Character_GiveMoney(playerid, characterFirstLoginMoney[AccountInfo[playerid][aCharactersCount]], "FirstSpawn");
			SendFormattedMessage(playerid, -1, "(( Ti sono stati dati {85bb65}$%d{FFFFFF} per cominciare! ))", characterFirstLoginMoney[AccountInfo[playerid][aCharactersCount]]);
		}
		
		new query[128];
		mysql_format(gMySQL, query, sizeof(query), "UPDATE `characters` SET FirstSpawn = '0' WHERE ID = '%d'", PlayerInfo[playerid][pID]);
		mysql_tquery(gMySQL, query);

		AccountInfo[playerid][aCharactersCount]++;
		
		mysql_format(gMySQL, query, sizeof(query), "UPDATE `accounts` SET CharactersCounter = '%d' WHERE ID = '%d'", AccountInfo[playerid][aCharactersCount], AccountInfo[playerid][aID]);
		mysql_tquery(gMySQL, query);

		Character_Save(playerid);
		return 1;
    }

	Character_FreezeForTime(playerid, 2000);

    if(PlayerRestore[playerid][pSpawned] && PlayerRestore[playerid][pLastX] != 0 && PlayerRestore[playerid][pLastY] != 0 && PlayerRestore[playerid][pLastZ] != 0)
    {
		PlayerRestore[playerid][pSpawned] = 0;
		SetPlayerPos(playerid, PlayerRestore[playerid][pLastX], PlayerRestore[playerid][pLastY], PlayerRestore[playerid][pLastZ]);
		SetPlayerFacingAngle(playerid, PlayerRestore[playerid][pLastAngle]);
		SetPlayerInterior(playerid, PlayerRestore[playerid][pLastInterior]);
		SetPlayerVirtualWorld(playerid, PlayerRestore[playerid][pLastVirtualWorld]);
		defer LoadCharacterDataAfterTime(playerid);
    }
    else
    {
		if(Character_GetFaction(playerid) == -1 && Character_GetHouseKey(playerid) == -1)
		{
			SetPlayerPos(playerid, 1723.3232, -1867.1775, 13.5705);
			SetPlayerInterior(playerid, 0);
			SetPlayerVirtualWorld(playerid, 0);
		}
		else if( Character_GetHouseKey(playerid) != -1)
		{
			new Float:x, Float:y, Float:z,housekey = Character_GetHouseKey(playerid);
			House_GetEnterPosition(housekey, x, y, z);
			SetPlayerPos(playerid, x, y, z);
			SetPlayerInterior(playerid, House_GetEnterInterior(housekey));
			SetPlayerVirtualWorld(playerid, House_GetEnterWorld(housekey));
		}
		else if( Character_GetFaction(playerid) != -1)
		{
			new Float:x, Float:y, Float:z, factionid = Character_GetFaction(playerid);
			Faction_GetSpawnPos(factionid, x, y, z);
			SetPlayerPos(playerid, x, y, z);
			SetPlayerInterior(playerid, Faction_GetSpawnInterior(factionid));
			SetPlayerVirtualWorld(playerid, Faction_GetSpawnWorld(factionid));
		}
		AC_SetPlayerHealth(playerid, 100.0);
		AC_SetPlayerArmour(playerid, 0.0);
    }
	if(Character_IsJailed(playerid))
	{
		Character_SetToJailPos(playerid);
		PlayerTextDrawShow(playerid, pJailTimeText[playerid]);
		return 1;
	}
	return 1;
}

stock Character_FreezeForTime(playerid, timeInMillis)
{
	Character_SetFreezed(playerid, true);
	defer UnFreezeTimer(playerid, timeInMillis);
}

timer UnFreezeTimer[timeInMillis](playerid, timeInMillis)
{
	#pragma unused timeInMillis
	Character_SetFreezed(playerid, false);
}

timer LoadCharacterDataAfterTime[1000](playerid)
{
	AC_SetPlayerHealth(playerid, PlayerRestore[playerid][pLastHealth]);
	AC_SetPlayerArmour(playerid, PlayerRestore[playerid][pLastArmour]);
	Character_LoadWeapons(playerid);
}

stock Character_HasWeaponInSlot(playerid, slotid)
{
	new weapon, ammo;
	GetPlayerWeaponData(playerid, slotid, weapon, ammo);
	return weapon != 0 && ammo > 0 && AC_AntiWeaponCheck(playerid, weapon, ammo);
}

stock Player_Info(playerid, text[], bool:forced = false, time = 2500)
{
	if(pInfoBoxShown[playerid] == true && !forced)
		return 1;

	PlayerTextDrawHide(playerid, pInfoText[playerid]);
	PlayerTextDrawSetString(playerid, pInfoText[playerid], text);
	PlayerTextDrawShow(playerid, pInfoText[playerid]);

	SetTimerEx("DeleteInfoBox", time, false, "d", playerid);
	pInfoBoxShown[playerid] = true;

	return 1;
}

stock Player_InfoStr(playerid, String:string, bool:forced = false, time = 2500)
{
	new ptr[1][] = {{}}, size = str_len(string) + 1, Var:var = amx_alloc(size);
	amx_to_ref(var, ptr);
	str_get(string, ptr[0], .size=size);

	new result = Player_Info(playerid, ptr[0], forced, time);

	amx_free(var);
	amx_delete(var);
	return result;
}

forward DeleteInfoBox(playerid); 
public DeleteInfoBox(playerid)
{
	PlayerTextDrawHide(playerid, pInfoText[playerid]);

	pInfoBoxShown[playerid] = false;

	return 1;
}

stock Character_GetClosestVehicle(playerid, Float:radius)
{
	if(IsPlayerInAnyVehicle(playerid))
		return GetPlayerVehicleID(playerid);

	new 
		Float:x, Float:y, Float:z;
	foreach(new i : Vehicles)
	{
		GetVehiclePos(i, x, y, z);
		if(IsPlayerInRangeOfPoint(playerid, radius, x, y, z))
		{
			return i;
		}
	}
    return 0;
}