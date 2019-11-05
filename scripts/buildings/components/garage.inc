
#include <pickup_system\core>
#include <buildings\core>
#include <player_system\core>

#include <YSI_Coding\y_hooks>

#define CHOPSHOP_X						(2771.7817)
#define CHOPSHOP_Y						(-1606.4154)
#define CHOPSHOP_Z						(10.9219)


flags:gcmds(CMD_USER);
CMD:gcmds(playerid, params[])
{
    SendClientMessage(playerid, -1, "[GARAGE]: /ripara - /vernicia");
    SendClientMessage(playerid, -1, "Ricorda che puoi acquistare materiali meccanici ad un qualsiasi Chopshop.");
    return 1;
}
alias:gcmds("gcmd");


stock bool:Character_HasGarage(playerid)
{
    for_player_building(building : playerid)
    {
        if(Building_GetType(building) == BUILDING_TYPE_GARAGE)
            return true;
    }
    return false;
}

stock bool:Character_CanUseGarage(playerid)
{
    if(Character_GetOwnedBuildingsList(playerid) <= 0)
        return false;
    
    new Building:current = Character_GetCurrentBuilding(playerid);

    new Building:garageid, Float:x, Float:y, Float:z;
    for_player_building(building : playerid)
    {
        if(current == building)
            return true;
        Building_GetEnterPos(building, x, y, z);
        if(GetPlayerDistanceFromPoint(playerid, x, y, z) <= 25.0)
            return true;
    }
    return false;
}

hook function Character_Enter(playerid)
{
    printf("[Character_Enter]: Garage");
    new vehicleid = GetPlayerVehicleID(playerid);
    new Float:range = Vehicle_IsValid(vehicleid) ? 5.0 : 1.5;

    new Building:building = Building:0;

    static 
        Float:finalX, Float:finalY, Float:finalZ, finalInterior, finalWorld;
    
    if(!IsPlayerInAnyVehicle(playerid))
    {
        building = Character_GetCurrentBuilding(playerid);
    }
    else
    {
        static Float:x, Float:y, Float:z, Float:ex, Float:ey, Float:ez;
        for_building_of<BUILDING_TYPE_GARAGE>(b)
        {
            Building_GetEnterPos(b, x, y, z);
            Building_GetExitPos(b, ex, ey, ez);
            if(IsPlayerInRangeOfPoint(playerid, range, x, y, z) && Character_GetVirtualWorld(playerid) == Building_GetEnterWorld(b))
            {
                building = b;
                finalX = ex;
                finalY = ey;
                finalZ = ez;
                finalInterior = Building_GetExitInterior(b);
                finalWorld = Building_GetExitWorld(b);
                break;
            }
            if(IsPlayerInRangeOfPoint(playerid, range, ex, ey, ez) && Character_GetVirtualWorld(playerid) == Building_GetExitWorld(b))
            {
                building = b;
                finalX = x;
                finalY = y;
                finalZ = z;
                finalInterior = Building_GetEnterInterior(b);
                finalWorld = Building_GetEnterWorld(b);
                break;
            }
        }

    }

    if(!Building_IsValid(building) || Building_GetType(building) != BUILDING_TYPE_GARAGE)
        return continue(playerid);
    
    if(vehicleid > 0 && GetPlayerVehicleSeat(playerid) != 0)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Devi essere il guidatore per poter entrare o uscire dall'edificio.");
        return 0;
    }

    if(Building_IsLocked(building))
    {
        GameTextForPlayer(playerid, "~r~Chiuso", 5000, 1);
        return 0;
    }

    printf("%f - %f - %f", finalX, finalY, finalZ);

    if(finalX == 0.0 && finalY == 0.0 && finalZ == 0.0)
        return 0;

    if(vehicleid > 0)
    {
        foreach(new p : Player)
        {
            if(!Character_IsLogged(p) || GetPlayerVehicleID(p) != vehicleid) 
                continue;
            defer SetPlayerInSeatAfterTime(p, vehicleid, GetPlayerVehicleSeat(p));
            Character_SetSafePosition(p, finalX, finalY, finalZ, finalInterior, finalWorld);
        }
        defer SetVehiclePosDyn(vehicleid, finalX, finalY, finalZ, finalInterior, finalWorld);
    }
    else
    {
        Character_SetSafePosition(playerid, finalX, finalY, finalZ, finalInterior, finalWorld);
    }
    return 1;
}

timer SetPlayerInSeatAfterTime[100](playerid, vehicleid, seatid)
{
    if(!Character_IsLogged(playerid))
        return 0;
    PutPlayerInVehicle(playerid, vehicleid, seatid);
    return 1;
}

timer SetVehiclePosDyn[50](vehicleid, Float:x, Float:y, Float:z, interior, world)
{
    LinkVehicleToInterior(vehicleid, interior);
    SetVehicleVirtualWorld(vehicleid, world);
    SetVehiclePos(vehicleid, x, y, z);
    return 1;
}