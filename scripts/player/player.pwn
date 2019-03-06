#include <player/player_global.pwn>
#include <player/player_inventory.pwn>
#include <player/player_commands.pwn>
#include <YSI\y_hooks>

hook OnPlayerClearData(playerid)
{
    Character_SetVehsDestroyTime(playerid);
    Character_Clear(playerid);
    return 1;
}

hook OnCharacterSaveData(playerid)
{
    Character_Save(playerid, _, 1);
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
                SendFormattedMessage(playerid, COLOR_ERROR, "(( Attenzione: Il proprietario del veicolo si è disconnesso. Il veicolo verrà distrutto in %d minuti. ))", gVehicleDestroyTime[vehicleid]);
            }
            new
                playerName[MAX_PLAYER_NAME];
            
            FixName(VehicleInfo[vehicleid][vOwnerName], playerName);
            SendFormattedMessage(playerid, COLOR_GREEN, "> Questo veicolo (%s) appartiene a %s", GetVehicleName(vehicleid), playerName);

            if(Vehicle_IsEngineOff(vehicleid))
            {
                SendClientMessage(playerid, -1, "> Premi SPAZIO (/motore) per accendere il motore.");
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
    new vehicleid = GetPlayerVehicleID(playerid);
    if(vehicleid > 0)
    {
        if( (PRESSED(KEY_HANDBRAKE) && Vehicle_IsEngineOff(vehicleid)) || PRESSED(KEY_ACTION) && Vehicle_IsEngineOn(vehicleid))
        {
            pc_cmd_motore(playerid, NULL);
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
        SendClientMessage(playerid, -1, "> First Spawn <");
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
        SendClientMessage(playerid, -1, "> Spawned");
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

    Character_LoadVehicles(playerid);
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
        Spawned = '%d', PayDay = '%d', Exp = '%d' \
        WHERE ID = '%d'", 
        PlayerInfo[playerid][pMoney], PlayerInfo[playerid][pLevel], PlayerInfo[playerid][pAge], PlayerInfo[playerid][pSex], 
        _x, _y, _z, angle, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid),
        hp, armour, PlayerInfo[playerid][pSkin],
        isSpawned, PlayerInfo[playerid][pPayDay], PlayerInfo[playerid][pExp],
        PlayerInfo[playerid][pID]);
    
    mysql_tquery(gMySQL, query);

    // Save Vehicles
    Character_SaveAllVehicles(playerid);
    // Save Others

    // Save A' Mammt

    return 1;
}

stock Character_Delete(playerid, character_db_id, character_name[])
{
    #pragma unused playerid
    new query[256];
    mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `characters` WHERE ID = '%d' AND LOWER(Name) = LOWER('%e')", character_db_id, character_name);
    
    // Delete cars
    // Delete all others

    mysql_tquery(gMySQL, query);
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
    }
    new query[512];
    mysql_format(gMySQL, query, sizeof(query), "INSERT INTO `player_vehicles` (OwnerID, Model, Color1, Color2, X, Y, Z, Angle, Locked)\
        VALUES('%d', '%d', '%d', '%d', '%f', '%f', '%f', '%f', '0')",
        PlayerInfo[playerid][pID], model, color1, color2, x, y, z, a);
    mysql_tquery_inline(gMySQL, query, using inline OnInsert);
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
        }
    }
    new query[128];
    mysql_format(gMySQL, query, sizeof(query), "SELECT * FROM `player_vehicles` WHERE OwnerID = '%d' ORDER BY ID", PlayerInfo[playerid][pID]);
    mysql_tquery_inline(gMySQL, query, using inline OnLoad);
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

stock Character_AMe(playerid, text[])
{
    gPlayerAMeExpiry[playerid] = 10; // Seconds

    new string[128];
    format(string, sizeof(string), "* %s *", text);

    UpdateDynamic3DTextLabelText(gPlayerAMe3DText[playerid], 0xD0AEEBFF, string);
    
    format(string, sizeof(string), "* %s %s *", Character_GetOOCName(playerid), text);
    SendClientMessage(playerid, 0xD0AEEBFF, string); //0xD6C3E3FF

    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    foreach(new i : Player)
    {
        if(IsPlayerInRangeOfPoint(playerid, 25.0, x, y, z))
            Streamer_Update(i);
    }
    return 1;
}

stock Character_Me(playerid, text[])
{
    if(!gCharacterLogged[playerid] || strlen(text) > 256)
        return 0;
    new string[256];
    format(string, sizeof(string), "* %s %s", Character_GetOOCName(playerid), text);
    ProxDetector(playerid, 20.0, string, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF); //0xD6C3E3FF
    return 1;
}

stock Character_Do(playerid, text[])
{
    if(!gCharacterLogged[playerid] || strlen(text) > 256)
        return 0;
    new string[256];
    format(string, sizeof(string), "%s (( %s ))", text, Character_GetOOCName(playerid));
    ProxDetector(playerid, 20.0, string, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF);
    return 1;
}

stock Character_GetOOCName(playerid)
{
    new name[24];
    FixName(PlayerInfo[playerid][pName], name);
    return name;
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
        Float:hp, Float:armour;
    AC_GetPlayerHealth(playerid, hp);
    AC_GetPlayerArmour(playerid, armour);
    SendClientMessage(targetid, COLOR_YELLOW, "_______________________________________________________");
    SendFormattedMessage(targetid, COLOR_YELLOW, "Nome: %s - Età: %d - Sesso: %s", Character_GetOOCName(playerid), PlayerInfo[playerid][pAge], GetSexName(playerid));
    SendFormattedMessage(targetid, COLOR_YELLOW, "Soldi: %d", AC_GetPlayerMoney(playerid));
    SendFormattedMessage(targetid, COLOR_YELLOW, "HP: %.2f - Armatura: %.2f - Int: %d - VW: %d", hp, armour, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));
    SendFormattedMessage(targetid, COLOR_YELLOW, "Livello: %d - Esperienza: %d", PlayerInfo[playerid][pLevel], PlayerInfo[playerid][pExp]);
    SendFormattedMessage(targetid, COLOR_YELLOW, "Tempo rimanente al PayDay: %d minuti", PlayerInfo[playerid][pPayDay]);
    SendClientMessage(targetid, COLOR_YELLOW, "_______________________________________________________");
    return 1;
}

stock Character_Clear(playerid)
{
    PlayerInfo[playerid][pID] = 0;
    set(PlayerInfo[playerid][pName], "");
    PlayerInfo[playerid][pMoney] = 0;
    PlayerInfo[playerid][pLevel] = 0;
    PlayerInfo[playerid][pExp] = 0;
    PlayerInfo[playerid][pAge] = 0;
    PlayerInfo[playerid][pSex] = 0;
    PlayerInfo[playerid][pSkin] = 0;
    PlayerInfo[playerid][pPayDay] = 0;
    PlayerInfo[playerid][pHealth] = 0.0;
    PlayerInfo[playerid][pArmour] = 0.0;
    PlayerRestore[playerid][pSpawned] = 0;
    PlayerRestore[playerid][pFirstSpawn] = 0;
    PlayerRestore[playerid][pLastX] = 0.0;
    PlayerRestore[playerid][pLastY] = 0.0;
    PlayerRestore[playerid][pLastZ] = 0.0;
    PlayerRestore[playerid][pLastAngle] = 0.0;
    PlayerRestore[playerid][pLastHealth] = 0.0;
    PlayerRestore[playerid][pLastArmour] = 0.0;
    PlayerRestore[playerid][pLastInterior] = 0;
    PlayerRestore[playerid][pLastVirtualWorld] = 0;
}