
#include <faction_system\national_guard\beanbag_taser>
#include <faction_system\national_guard\mdc>

#include <YSI_Coding\y_hooks>

static enum e_POLICE_ARREST
{
	Float:aX,
	Float:aY,
	Float:aZ,
	aInterior,
};
static PoliceArrestPositions[][e_POLICE_ARREST] = 
{
	{2638.6470, -1955.7460, -58.7273, 0},
	{DEFAULT_SPAWN_X, DEFAULT_SPAWN_Y, DEFAULT_SPAWN_Z, 0}
};

hook OnGameModeInit()
{
    for(new i = 0, j = sizeof(PoliceArrestPositions); i < j; ++i)
	{
		Pickup_Create(1239, i, PoliceArrestPositions[i][aX], PoliceArrestPositions[i][aY], PoliceArrestPositions[i][aZ], ELEMENT_TYPE_ARREST, -1, PoliceArrestPositions[i][aInterior]);
		CreateDynamic3DTextLabel("Punto Arresto", COLOR_BLUE, PoliceArrestPositions[i][aX], PoliceArrestPositions[i][aY], PoliceArrestPositions[i][aZ], 15.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, -1, PoliceArrestPositions[i][aInterior]);
	}
    return 1;
}

stock IsPlayerNearAnyArrestPos(playerid)
{
	new id, E_ELEMENT_TYPE:type;
	if(Character_GetNearestPickup(playerid, id, type) && type == ELEMENT_TYPE_ARREST)
	{
		return 1;
	}
	return 0;
}