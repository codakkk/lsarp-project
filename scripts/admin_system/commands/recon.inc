#include <YSI_Coding\y_hooks>


static 
    SpectatingPlayer[MAX_PLAYERS] = {INVALID_PLAYER_ID, ...},
    Float:RestoreHealth[MAX_PLAYERS],
    Float:RestoreArmour[MAX_PLAYERS]
    //RestoreWeapons[MAX_PLAYERS][13],
    //RestoreAmmos[MAX_PLAYERS][13]
;

hook OnCharacterClearData(playerid)
{
    foreach(new i : Player)
    {
        if(Player_GetSpectatingPlayer(i) == playerid)
        {
            SendFormattedMessage(playerid, COLOR_ERROR, "L'utente (%s - ID: %d) che stavi reconnando si è disconnesso.", Character_GetOOCName(playerid), playerid);
            callcmd::recon(i, "off");
        }
    }
    SpectatingPlayer[playerid] = INVALID_PLAYER_ID;
    RestoreHealth[playerid] = 0.0;
    RestoreArmour[playerid] = 0.0;
    return 1;
}
hook OnPlayerInteriorChange(playerid, newinteriorid, oldinteriorid)
{
    foreach(new i : Player)
    {
        if(GetPlayerState(i) == PLAYER_STATE_SPECTATING && Player_GetSpectatingPlayer(i) == playerid)
        {
            SetPlayerInterior(i, newinteriorid);
        }
    }
    return 1;
}

hook OnPlyVirtualWorldChange(playerid, oldWorld, newWorld)
{
    foreach(new i : Player)
    {
        if(i == playerid)
            continue;
        if(GetPlayerState(i) == PLAYER_STATE_SPECTATING && Player_GetSpectatingPlayer(i) == playerid)
        {
            Character_SetVirtualWorld(i, newWorld);
        }
    }
    return 1;
}

hook OnPlayerStateChange(playerid, newstate, oldstate)
{
    if(oldstate == PLAYER_STATE_ONFOOT && (newstate == PLAYER_STATE_DRIVER || newstate == PLAYER_STATE_PASSENGER) || ( (oldstate == PLAYER_STATE_DRIVER || oldstate == PLAYER_STATE_PASSENGER) && newstate == PLAYER_STATE_ONFOOT))
    {
        foreach(new i : Player)
        {
            if(GetPlayerState(i) == PLAYER_STATE_SPECTATING && Player_GetSpectatingPlayer(i) == playerid)
            {
                Recon(i, playerid);
            }
        }
    }
}

flags:scan(CMD_JR_MODERATOR);
CMD:scan(playerid, params[])
{
    new id;
    if(sscanf(params, "k<u>", id))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /scan [playerid/partofname]");
    
    if(!Character_IsLogged(id) || id == playerid)
        return SendClientMessage(playerid, COLOR_ERROR, "ID non valido.");
    
    SendFormattedMessage(playerid, -1, "Scan di %s", Character_GetOOCName(id));
    new speed = IsPlayerInAnyVehicle(id) ? Vehicle_GetSpeed(GetPlayerVehicleID(id)) : Player_GetSpeed(id);
    SendFormattedMessage(playerid, -1, "Velocità attuale: %d KM/h", speed);
    return 1;
}

flags:recon(CMD_JR_MODERATOR);
CMD:recon(playerid, params[])
{
    if(!isnull(params) && !strcmp(params, "off", true) && Character_IsLogged(SpectatingPlayer[playerid]))
    {
        
        PlayerSpectatePlayer(playerid, INVALID_PLAYER_ID);
	    PlayerSpectateVehicle(playerid, INVALID_VEHICLE_ID);
		TogglePlayerSpectating(playerid, false);
        SpectatingPlayer[playerid] = INVALID_PLAYER_ID;
        //SpawnPlayer(playerid);
        return 1;
    }
    new id;
    if(sscanf(params, "k<u>", id))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /recon <playerid/partofname/#####>. Usa /recon off per smettere.");

    if(!Character_IsLogged(id) || id == playerid)
        return SendClientMessage(playerid, COLOR_ERROR, "ID non valido.");

    /*if(GetPlayerState(playerid) != PLAYER_STATE_SPECTATING)
    {
        for(new i = 0; i < 13; ++i)
        {
            Character_GetWeaponData(playerid, i, RestoreWeapons[playerid][i], RestoreAmmos[playerid][i]);
        }
        GetPlayerHealth(playerid, RestoreHealth[playerid]);
        GetPlayerArmour(playerid, RestoreArmour[playerid]);
    }*/
    SendClientMessage(playerid, COLOR_GREEN, "Usa /scan per ulteriori informazioni.");
    Recon(playerid, id);
    return 1;
}

static FindNextReconnableID(playerid)
{
    new currentSpectating = SpectatingPlayer[playerid],
        start = GetPlayerPoolSize() == currentSpectating ? 0 : currentSpectating + 1;
    for(new i = start, j = GetPlayerPoolSize(); i <= j; ++i)
    {
        if(!Character_IsLogged(i) || i == playerid)
            continue;
        
        if(SpectatingPlayer[i] != INVALID_PLAYER_ID) 
            i = SpectatingPlayer[i];

        return i;
    }

    return INVALID_PLAYER_ID;
}

static FindPrevReconnableID(playerid)
{
    new currentSpectating = SpectatingPlayer[playerid],
        start = currentSpectating == 0 ? GetPlayerPoolSize() : currentSpectating - 1;
    for(new i = start; i >= 0; --i)
    {
        if(!Character_IsLogged(i) || i == playerid)
            continue;
        
        if(SpectatingPlayer[i] != INVALID_PLAYER_ID) 
            i = SpectatingPlayer[i];

        return i;
    }
    return INVALID_PLAYER_ID;
}

static bool:Recon(playerid, id)
{
    if(!Character_IsLogged(id))
        return false;
    
    SetPlayerInterior(playerid, GetPlayerInterior(id));
    Character_SetVirtualWorld(playerid, Character_GetVirtualWorld(id));
    
    TogglePlayerSpectating(playerid, true);

    if(IsPlayerInAnyVehicle(id))
        PlayerSpectateVehicle(playerid, GetPlayerVehicleID(id));
    else
        PlayerSpectatePlayer(playerid, id);
    
    //if(SpectatingPlayer[playerid] != id)
        //SendFormattedMessage(playerid, COLOR_GREEN, "Stai reconnando %s (%d).", Character_GetOOCName(id), id);
    
    SpectatingPlayer[playerid] = id;
    return true;
}

stock Player_GetSpectatingPlayer(playerid)
{
    if(!Character_IsLogged(playerid))
        return INVALID_PLAYER_ID;
    return SpectatingPlayer[playerid];
}