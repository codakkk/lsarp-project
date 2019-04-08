#include <YSI_Coding\y_hooks>

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
                string += str_format("%s~y~%s~n~~y~Proprietario: %s.~n~", string, bname, ownerName);
            }
            else
            {
                new bname[MAX_BUILDING_NAME];
                Building_GetName(buildingid, bname);
                string += str_format("%s~y~%s~n~~g~Prezzo:~w~$%d~n~", string, bname, Building_GetPrice(buildingid));
            }
        }
        else
        {
            new bname[MAX_BUILDING_NAME];
            Building_GetName(buildingid, bname);
            string += str_format("%s~y~%s~n~", string, bname);
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
    return 1;
}

hook OnPlayerInvItemUse(playerid, slot_id, item_id)
{
    new 
        bool:decrease = false,
        decreaseAmount = 1;
    // printf("player_inventory.pwn/OnPlayerInvItemUse", playerid);
    if(ServerItem_IsBag(item_id))
    {
        // If character has bag
        if(Character_HasBag(playerid))
        {
            SendClientMessage(playerid, COLOR_ERROR, "Stai già indossando uno zaino! Usa '/rimuovi zaino' per rimuoverlo.");
            return 1;
        }   
        Character_AMe(playerid, "indossa lo zaino");
        SendFormattedMessage(playerid, COLOR_GREEN, "Stai indossando '%s'. Usa '/rimuovi zaino' per rimuoverlo.", ServerItem_GetName(item_id));
        Character_SetBag(playerid, item_id);
        decrease = true;
        decreaseAmount = 1;
    }
    else if(ServerItem_IsWeapon(item_id))
    {
        new 
            weaponSlot = Weapon_GetSlot(item_id), 
            weapon, 
            ammo;
        if(GetPlayerWeaponData(playerid, weaponSlot, weapon, ammo))
        {
            if(weapon != 0 && ammo > 0)
                return SendClientMessage(playerid, COLOR_ERROR, "Hai già un arma equipaggiata per questa slot!");
            if(!Weapon_RequireAmmo(item_id))
            {
                AC_GivePlayerWeapon(playerid, item_id, 1);
                decrease = true;
                decreaseAmount = 1;
            }
            else
            {
				new extra = Character_GetItemExtraBySlot(playerid, slot_id);
				if(extra > 0)
				{
					AC_GivePlayerWeapon(playerid, item_id, extra);
					decrease = true;
					decreaseAmount = 1;
				}
				else
				{
					if(Character_HasItem(playerid, Weapon_GetAmmoType(item_id), 1) == -1)
						SendClientMessage(playerid, COLOR_ERROR, "Non hai i proiettili necessari!");
					else
					{
						SetPVarInt(playerid, "InventorySelect_WeaponItem", item_id);
						new ammos = Inventory_GetItemAmount(Character_GetInventory(playerid), Weapon_GetAmmoType(item_id));
						Dialog_Show(playerid, Dialog_InvSelectAmmo, DIALOG_STYLE_INPUT, "Inserisci le munizioni", "Immetti la quantità di munizioni che vuoi inserire nell'arma.\nQuantità: {00FF00}%d{FFFFFF}.", "Usa", "Annulla", ammos);
					}
				}
            }
        }
    }
    else if(ServerItem_IsAmmo(item_id))
    {
        new currentWeapon = GetPlayerWeapon(playerid);
        if(currentWeapon == 0 || Weapon_GetAmmoType(currentWeapon) != item_id)
            return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare queste munizioni su quest'arma.");
		new ammos = Inventory_GetItemAmount(Character_GetInventory(playerid), item_id);
		SetPVarInt(playerid, "InventorySelect_CurrentWeaponItem", currentWeapon);
		Dialog_Show(playerid, Dialog_InvSelectAddAmmo, DIALOG_STYLE_INPUT, "Inserisci le munizioni", "Immetti la quantità di munizioni che vuoi inserire nell'arma.\nQuantità: {00FF00}%d{FFFFFF}.", "Usa", "Annulla", ammos);
    }
    if(decrease)
    {
        Character_DecreaseSlotAmount(playerid, slot_id, decreaseAmount);
    }
    return 1;
}

Dialog:Dialog_InvSelectAddAmmo(playerid, response, listitem, inputtext[])
{
    if(!response)
	{
		DeletePVar(playerid, "InventorySelect_CurrentWeaponItem");
        return 0;
	}
    new ammo = strval(inputtext), 
		weaponid = GetPVarInt(playerid, "InventorySelect_CurrentWeaponItem"),
		ammoType = Weapon_GetAmmoType(weaponid),
		amount = Inventory_GetItemAmount(Character_GetInventory(playerid), ammoType)
	;
    if(ammo <= 0 || ammo > amount)
        return Dialog_Show(playerid, Dialog_InvSelectAmmo, DIALOG_STYLE_INPUT, "Inserisci le munizioni", 
					"{FF0000}Munizioni non valide!{FFFFFF}\nImmetti la quantità di munizioni che vuoi inserire nell'arma.\nQuantità: {00FF00}%d{FFFFFF}", "Usa", "Annulla", 
					amount);
    AC_GivePlayerAmmo(playerid, weaponid, ammo);
    Character_DecreaseItemAmount(playerid, ammoType, ammo);
	Character_AMe(playerid, "prende delle munizioni e le inserisce nell'arma");
	DeletePVar(playerid, "InventorySelect_CurrentWeaponItem");
    return 1;
}


hook OnPlayerStateChange(playerid, newstate, oldstate)
{
    if(newstate == PLAYER_STATE_DRIVER)
    {
        new vehicleid = GetPlayerVehicleID(playerid);
        if(VehicleInfo[vehicleid][vModel] != 0)
        {
            if(gVehicleDestroyTime[vehicleid] != -1)
            {
                SendTwoLinesMessage(playerid, COLOR_ERROR, "(( Attenzione: Il proprietario del veicolo (%s) si è disconnesso. Il veicolo verrà distrutto in %d minuti. ))", VehicleInfo[vehicleid][vOwnerName], gVehicleDestroyTime[vehicleid]);
            }
            new
                playerName[MAX_PLAYER_NAME];
            
            FixName(VehicleInfo[vehicleid][vOwnerName], playerName);
            SendFormattedMessage(playerid, COLOR_GREEN, "Questo veicolo (%s) appartiene a %s", GetVehicleName(vehicleid), playerName);

            if(Vehicle_IsEngineOff(vehicleid))
            {
				if(Bit_Get(gPlayerBitArray[e_pHotKeys], playerid))
                	SendClientMessage(playerid, -1, "Premi Y (oppure digita /motore) per accendere il motore.");
				else 
					SendClientMessage(playerid, -1, "Digita /motore per accendere il motore.");
            }
        }
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
}

hook OnPlayerRequestSpawn(playerid)
{
    if(!gCharacterLogged[playerid])
        return 0;
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    if(Bit_Get(gPlayerBitArray[e_pHotKeys], playerid))
    {
        if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
        {
            new vehicleid = GetPlayerVehicleID(playerid);
            if( (PRESSED(KEY_YES) ) )
            {
                pc_cmd_motore(playerid, "");
            }
            else if( (PRESSED(KEY_LOOK_BEHIND)) )
            {
                Vehicle_SetLightState(vehicleid, !Vehicle_IsLightOff(vehicleid));
            }
        }
        new pickupid = pLastPickup[playerid];
        if(pickupid != -1 && Bit_Get(gPlayerBitArray[e_pHotKeys], playerid) && IsPlayerInRangeOfPickup(playerid, pickupid, 2.0) && !IsPlayerInAnyVehicle(playerid))
        {
            new 
                elementId, 
                E_ELEMENT_TYPE:type;
            Pickup_GetInfo(pickupid, elementId, type);
            if(PRESSED(KEY_SECONDARY_ATTACK))
            {
                // Should I write an "OnInteract" callback?
                if(type == ELEMENT_TYPE_BUILDING_ENTRANCE || type == ELEMENT_TYPE_HOUSE_ENTRANCE)
                {
                    Player_Enter(playerid, pickupid, elementId, type);
                }
                else if(type == ELEMENT_TYPE_BUILDING_EXIT || type == ELEMENT_TYPE_HOUSE_EXIT)
                {
                    Player_Exit(playerid, pickupid, elementId, type);
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
        if( (PRESSED(KEY_NO)))
        {
            new vehicleid = GetClosestVehicle(playerid, 3.5);
            if(Vehicle_GetOwnerID(vehicleid) == Character_GetID(playerid)) //We're owners
            {
                if((IsABike(vehicleid) || IsAMotorBike(vehicleid)) && Vehicle_IsEngineOn(vehicleid))
                {
                    SendClientMessage(playerid, -1, "Prima spegni il motore!");
                }
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
    return 1;
}


hook OnPlayerSpawn(playerid)
{
    if(IsPlayerNPC(playerid))
        return 1;

    if(!gAccountLogged[playerid] || !gCharacterLogged[playerid])
        return KickEx(playerid);
    
    AC_SetPlayerHealth(playerid, 100.0);
    AC_SetPlayerArmour(playerid, 0.0);
    

    if(PlayerRestore[playerid][pFirstSpawn]) // First Login/Spawn
    {
        // SendClientMessage(playerid, -1, "> First Spawn <");
        PlayerRestore[playerid][pFirstSpawn] = 0;
        AC_SetPlayerSkin(playerid, 46);
        // Should I give first login money here?
        SetPlayerPos(playerid, 1748.1887, -1860.0414, 13.5792);

        AC_GivePlayerMoney(playerid, 30000, "");

        new query[128];
        mysql_format(gMySQL, query, sizeof(query), "UPDATE `characters` SET FirstSpawn = '0' WHERE ID = '%d'", PlayerInfo[playerid][pID]);
        mysql_tquery(gMySQL, query);
        Character_Save(playerid);
    }
    else if(PlayerRestore[playerid][pSpawned] && PlayerRestore[playerid][pLastX] != 0 && PlayerRestore[playerid][pLastY] != 0 && PlayerRestore[playerid][pLastZ] != 0)
    {
        PlayerRestore[playerid][pSpawned] = 0;
        SetPlayerPos(playerid, PlayerRestore[playerid][pLastX], PlayerRestore[playerid][pLastY], PlayerRestore[playerid][pLastZ]);
        SetPlayerFacingAngle(playerid, PlayerRestore[playerid][pLastAngle]);
        SetPlayerInterior(playerid, PlayerRestore[playerid][pLastInterior]);
        SetPlayerVirtualWorld(playerid, PlayerRestore[playerid][pLastVirtualWorld]);
        AC_SetPlayerHealth(playerid, PlayerRestore[playerid][pLastHealth]);
        AC_SetPlayerArmour(playerid, PlayerRestore[playerid][pLastArmour]);
        // SendClientMessage(playerid, -1, "> Spawned");
    }
    else if(pDeathState[playerid] == 2 && !pAdminDuty[playerid])
    {
        // Spawn in hospital (?)
    }
    else
    {
        AC_SetPlayerHealth(playerid, 100.0);
        AC_SetPlayerArmour(playerid, 0.0);
        SetPlayerPos(playerid, 1723.3232, -1867.1775, 13.5705);
    }
    SetPlayerSkin(playerid, PlayerInfo[playerid][pSkin]);
    return 1;
}

stock Character_PayDay(playerid)
{
    new expForNewLevel = (PlayerInfo[playerid][pLevel]+1)*2;
    PlayerInfo[playerid][pExp]++;

    SendClientMessage(playerid, COLOR_YELLOW, "_______________________[PAYDAY]_____________________");
    //SendFormattedMessage(playerid, COLOR_YELLOW, );
    
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
        return 1;
    }
    return 0;
}

stock Character_Save(playerid, spawned = true, disconnected = false)
{
    // This should never happen.
    if(!gAccountLogged[playerid] || !gCharacterLogged[playerid])
        return 0;
    
    CallLocalFunction("OnCharacterPreSaveData", "ii", playerid, disconnected);
    
    new 
        query[1024],
        Float:_x, Float:_y, Float:_z,
        Float:angle,
        Float:hp, Float:armour,
        isSpawned = false;

    //if() 
    isSpawned = pDeathState[playerid] == 0 && spawned;

    GetPlayerPos(playerid, _x, _y, _z);
    GetPlayerFacingAngle(playerid, angle);
    AC_GetPlayerHealth(playerid, hp);
    AC_GetPlayerArmour(playerid, armour);

    mysql_format(gMySQL, query, sizeof(query), "UPDATE `characters` SET \
        Money = '%d', Level = '%d', Age = '%d', Sex = '%d', \
        LastX = '%f', LastY = '%f', LastZ = '%f', LastAngle = '%f', LastInterior = '%d', LastVirtualWorld = '%d', Health = '%f', Armour = '%f', Skin = '%d', \
        Spawned = '%d', PayDay = '%d', Exp = '%d',  \
        BuildingKey = '%d', HouseKey = '%d' \
        WHERE ID = '%d'", 
        PlayerInfo[playerid][pMoney], PlayerInfo[playerid][pLevel], PlayerInfo[playerid][pAge], PlayerInfo[playerid][pSex], 
        _x, _y, _z, angle, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid),
        hp, armour, PlayerInfo[playerid][pSkin],
        isSpawned, PlayerInfo[playerid][pPayDay], PlayerInfo[playerid][pExp],
        PlayerInfo[playerid][pBuildingKey], PlayerInfo[playerid][pHouseKey],
        PlayerInfo[playerid][pID]);
    
    mysql_tquery(gMySQL, query);

    // Save Vehicles
    Character_SaveAllVehicles(playerid);
    // Save Others

    // Save A' Mammt
    CallLocalFunction(#OnCharacterSaveData, "d", playerid);
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

stock Character_AddVehicle(playerid, model, color1, color2)
{
    new 
        vehicleid = GetPlayerVehicleID(playerid),
        Float:x, Float:y, Float:z, Float:a;
    
    GetVehiclePos(vehicleid, x, y, z);
    GetVehicleZAngle(vehicleid, a);
    inline OnInsert()
    {    
        VehicleInfo[vehicleid][vID] = cache_insert_id();
        VehicleInfo[vehicleid][vOwnerID] = PlayerInfo[playerid][pID];
        set(VehicleInfo[vehicleid][vOwnerName], PlayerInfo[playerid][pName]);
        VehicleInfo[vehicleid][vModel] = model;
        VehicleInfo[vehicleid][vColor1] = color1;
        VehicleInfo[vehicleid][vColor2] = color2;
        VehicleInfo[vehicleid][vX] = x;
        VehicleInfo[vehicleid][vY] = y;
        VehicleInfo[vehicleid][vZ] = z;
        VehicleInfo[vehicleid][vA] = a;
        VehicleInfo[vehicleid][vLocked] = 0;
        gVehicleDestroyTime[vehicleid] = -1;
        Vehicle_SetEngineOff(vehicleid);
        Vehicle_InitializeInventory(vehicleid);
        mysql_tquery_f(gMySQL, "INSERT INTO `vehicle_inventory` (VehicleID) VALUES('%d')", VehicleInfo[vehicleid][vID]);
    }
    MySQL_TQueryInline(gMySQL, using inline OnInsert, "INSERT INTO `player_vehicles` (OwnerID, Model, Color1, Color2, X, Y, Z, Angle, Locked) \
        VALUES('%d', '%d', '%d', '%d', '%f', '%f', '%f', '%f', '0')",
        PlayerInfo[playerid][pID], model, color1, color2, x, y, z, a);
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

            new vehicleid = CreateVehicle(modelid, 0, 0, 0, 0, 0, 0, 0, 0);
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
            Iter_Add(Vehicles, vehicleid);
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
    if(!gAccountLogged[playerid] || !gCharacterLogged[playerid])
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

stock Character_Me(playerid, text[], GLOBAL_TAG_TYPES:...)
{
    if(!gCharacterLogged[playerid] || strlen(text) > 256)
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
    if(!gCharacterLogged[playerid] || strlen(text) > 256)
        return 0;
    
    new String:str = str_format(text, ___2);
    str = str_format("* %s %S", Character_GetOOCName(playerid), str);
    ProxDetectorStr(playerid, 7.0, str, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF); //0xD6C3E3FF
    return 1;
}

stock Character_Do(playerid, text[], GLOBAL_TAG_TYPES:...)
{
    if(!gCharacterLogged[playerid] || strlen(text) > 256)
        return 0;
    
    new String:str = str_format(text, ___2);
    str += str_format(" (( %s ))", Character_GetOOCName(playerid));
    ProxDetectorStr(playerid, 20.0, str, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF);
    return 1;
}

stock Character_DoLow(playerid, text[], GLOBAL_TAG_TYPES:...)
{
    if(!gCharacterLogged[playerid] || strlen(text) > 256)
        return 0;
    
    new String:str = str_format(text, ___2);
    str += str_format(" (( %s ))", Character_GetOOCName(playerid));
    ProxDetectorStr(playerid, 7.0, str, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF);
    return 1;
}

stock Character_GetOOCName(playerid)
{
    new name[24];
    FixName(PlayerInfo[playerid][pName], name);
    return name;
}

stock String:Character_GetOOCNameStr(playerid)
{
    return @(Character_GetOOCName(playerid));
}

stock Character_SetVehsDestroyTime(playerid)
{
    if(!gCharacterLogged[playerid])
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
    if(!gCharacterLogged[playerid])
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato!");
    new 
        Float:hp, Float:armour,
		expForNewLevel = (PlayerInfo[playerid][pLevel]+1) * 2;
    AC_GetPlayerHealth(playerid, hp);
    AC_GetPlayerArmour(playerid, armour);
    SendClientMessage(targetid, COLOR_GREEN, "_____________________[STATISTICHE]_____________________");
	SendFormattedMessage(targetid, -1, "[Account]: Account: %s", AccountInfo[playerid][aName]);
    SendFormattedMessage(targetid, -1, "[Personaggio]: Nome: %s - Età: %d - Sesso: %s", Character_GetOOCName(playerid), PlayerInfo[playerid][pAge], GetSexName(playerid));
	SendFormattedMessage(targetid, -1, "[Personaggio]: Livello: %d - Skin: %d", PlayerInfo[playerid][pLevel], GetPlayerSkin(playerid));
    SendFormattedMessage(targetid, -1, "[Denaro]: Soldi: $%d", AC_GetPlayerMoney(playerid));
    SendFormattedMessage(targetid, -1, "[Salute]: HP: %.2f - Armatura: %.2f", hp, armour);
	SendFormattedMessage(targetid, -1, "[Altro]: Interior: %d - VW: %d", GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));
    if(PlayerInfo[playerid][pBuildingKey] != 0 && PlayerInfo[playerid][pHouseKey] != 0)
        SendFormattedMessage(targetid, COLOR_YELLOW, "[Proprietà]: Edificio: %d - Casa: %d", PlayerInfo[playerid][pBuildingKey], PlayerInfo[playerid][pHouseKey]);
    else if(PlayerInfo[playerid][pBuildingKey] != 0)
        SendFormattedMessage(targetid, COLOR_YELLOW, "[Proprietà]: Edificio: %d", PlayerInfo[playerid][pBuildingKey]);
    else if(PlayerInfo[playerid][pHouseKey] != 0)
        SendFormattedMessage(targetid, COLOR_YELLOW, "[Proprietà]: Casa: %d", PlayerInfo[playerid][pHouseKey]);
	SendFormattedMessage(playerid, -1, "Ti mancano %d/%d punti esperienza per salire di livello.", PlayerInfo[playerid][pExp], expForNewLevel);
    SendFormattedMessage(targetid, COLOR_YELLOW, "Tempo rimanente al PayDay: %d minuti", PlayerInfo[playerid][pPayDay]);
    SendClientMessage(targetid, COLOR_GREEN, "_______________________________________________________");
    return 1;
}

stock Character_Clear(playerid)
{
    new CleanData[E_PLAYER_DATA];
    PlayerInfo[playerid] = CleanData;

    new CleanRestoreData[E_PLAYER_RESTORE_DATA];
    PlayerRestore[playerid] = CleanRestoreData;
}

stock Player_Enter(playerid, pickupid, elementId, E_ELEMENT_TYPE:type)
{
    if(!IsPlayerInRangeOfPickup(playerid, pickupid, 2.5))
        return 0;
    if(type != ELEMENT_TYPE_BUILDING_ENTRANCE && type != ELEMENT_TYPE_HOUSE_ENTRANCE)
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
        world = elementId;
        locked = BuildingInfo[elementId][bLocked];
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

stock Player_Exit(playerid, pickupid, elementId, E_ELEMENT_TYPE:type)
{
    if(!IsPlayerInRangeOfPickup(playerid, pickupid, 2.5))
        return 0;
    if(type != ELEMENT_TYPE_BUILDING_EXIT && type != ELEMENT_TYPE_HOUSE_EXIT)
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

stock Player_BuyBuilding(playerid, buildingid)
{
    if(!Building_IsValid(buildingid))
        return 0;
    
    if(!Building_IsOwnable(buildingid) || Building_GetOwnerID(buildingid) != 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Questa proprietà non è in vendita!");

    if(AC_GetPlayerMoney(playerid) < Building_GetPrice(buildingid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi per acquistare questa proprietà!");
    
    AC_GivePlayerMoney(playerid, -BuildingInfo[buildingid][bPrice]);
    
    Building_SetOwner(buildingid, playerid);
    
    PlayerInfo[playerid][pBuildingKey] = BuildingInfo[buildingid][bID];

    SendFormattedMessage(playerid, COLOR_GREEN, "Hai acquistato questo edificio (%s) per $%d.", BuildingInfo[buildingid][bName], Building_GetPrice(buildingid));
    Building_Save(buildingid);
    Character_Save(playerid);
    return 1;
}

stock Player_BuyHouse(playerid, houseid)
{
    if(!House_IsValid(houseid))
        return 0;

    if(House_GetOwnerID(houseid) != 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Questa proprietà non è in vendita!");

    if(Character_HasHouseKey(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Sei già proprietario di una casa!");

    new price = House_GetPrice(houseid);
    if(AC_GetPlayerMoney(playerid) < price)
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi per acquistare questa proprietà!");

    House_SetOwner(houseid, playerid);

    AC_GivePlayerMoney(playerid, -price, "Player_BuyHouse");

    SendFormattedMessage(playerid, COLOR_GREEN, "Congratulazioni! Hai acquistato questa casa per $%d!", price);

    House_Save(houseid);
    Character_Save(playerid);
    return 1;
}

stock Character_HasBuildingKey(playerid)
{
    return PlayerInfo[playerid][pBuildingKey] > 0;
}

stock Character_GetBuildingKey(playerid)
{
    return PlayerInfo[playerid][pBuildingKey];
}

stock Character_HasHouseKey(playerid)
{
    return PlayerInfo[playerid][pHouseKey] > 0;
}

stock Character_GetHouseKey(playerid)
{
    return PlayerInfo[playerid][pHouseKey];
}

stock Character_GetID(playerid)
{
    return PlayerInfo[playerid][pID];
}

stock Character_IsFreezed(playerid)
{
    return Bit_Get(gPlayerBitArray[e_pFreezed], playerid);
}

stock Character_SetFreezed(playerid, freeze)
{
    Bit_Set(gPlayerBitArray[e_pFreezed], playerid, freeze);
    TogglePlayerControllable(playerid, !freeze);
}