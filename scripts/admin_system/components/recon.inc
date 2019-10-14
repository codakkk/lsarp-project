#include <YSI_Coding\y_hooks>

//  PlayerText
static PlayerText:ReconPlayerText[MAX_PLAYERS];

static 
    SpectatingPlayer[MAX_PLAYERS] = {INVALID_PLAYER_ID, ...},
    Float:RestoreHealth[MAX_PLAYERS],
    Float:RestoreArmour[MAX_PLAYERS]
    //RestoreWeapons[MAX_PLAYERS][13],
    //RestoreAmmos[MAX_PLAYERS][13]
;

hook OnPlayerConnect(playerid)
{
    ReconPlayerText[playerid] = CreatePlayerTextDraw(playerid, 320.000000, 340.000000, "_");
	PlayerTextDrawBackgroundColor(playerid, ReconPlayerText[playerid], 255);
	PlayerTextDrawAlignment(playerid, ReconPlayerText[playerid], 2); // 2 Means centered.
	PlayerTextDrawFont(playerid, ReconPlayerText[playerid], 1);
	PlayerTextDrawLetterSize(playerid, ReconPlayerText[playerid], 0.400000, 1.600000);
	PlayerTextDrawColor(playerid, ReconPlayerText[playerid], -1);
	PlayerTextDrawSetOutline(playerid, ReconPlayerText[playerid], 1);
	PlayerTextDrawSetProportional(playerid, ReconPlayerText[playerid], 1);
	PlayerTextDrawSetSelectable(playerid, ReconPlayerText[playerid], 0);
    return 1;
}

hook OnCharacterClearData(playerid)
{
    foreach(new i : Player)
    {
        if(Player_GetSpectatingPlayer(i) == playerid)
        {
            SendFormattedMessage(playerid, COLOR_ERROR, "L'utente (%s - ID: %d) che stavi reconnando si è disconnesso", Character_GetOOCName(playerid), playerid);
            pc_cmd_recon(i, "off");
        }
    }
    SpectatingPlayer[playerid] = INVALID_PLAYER_ID;
    RestoreHealth[playerid] = 0.0;
    RestoreArmour[playerid] = 0.0;
    return 1;
}

hook OnCharacterSpawn(playerid)
{
    PlayerTextDrawHide(playerid, ReconPlayerText[playerid]);
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

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    if(GetPlayerState(playerid) == PLAYER_STATE_SPECTATING && SpectatingPlayer[playerid] != INVALID_PLAYER_ID)
    {
        if(PRESSED(KEY_RIGHT))
        {
            return Recon(playerid, FindNextReconnableID(playerid));       
        }
        else if(PRESSED(KEY_LEFT))
        {
            return Recon(playerid, FindPrevReconnableID(playerid));
        }
        else if(PRESSED(KEY_SPRINT))
        {
            return Recon(playerid, Player_GetSpectatingPlayer(playerid));
        }
    }
    return 1;
}

flags:recon(CMD_JR_MODERATOR);
CMD:recon(playerid, params[])
{
    if(!isnull(params) && !strcmp(params, "off", true))
    {
        PlayerTextDrawHide(playerid, ReconPlayerText[playerid]);
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
    
    PlayerTextDrawSetStringStr(playerid, ReconPlayerText[playerid], str_format("~g~ID~w~: %d~n~~g~Account~w~: %s~n~~g~Personaggio~w~: %s~n~Usa ~g~~k~~GO_LEFT~~w~ o ~g~~k~~GO_RIGHT~~w~ per cambiare giocatore.~n~Usa ~g~~k~~KEY_SPRINT~~w~ in caso di bug.", id, Account_GetName(id), Character_GetOOCName(id)));
    PlayerTextDrawShow(playerid, ReconPlayerText[playerid]);

    SpectatingPlayer[playerid] = id;
    return true;
}

stock Player_GetSpectatingPlayer(playerid)
{
    if(!Character_IsLogged(playerid))
        return INVALID_PLAYER_ID;
    return SpectatingPlayer[playerid];
}