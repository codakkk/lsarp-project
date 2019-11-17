
#include <pickup_system\core>

#define IC_JAIL_X						(1810.0928)
#define IC_JAIL_Y						(-1575.3351)
#define IC_JAIL_Z 						(5700.4287)
#define IC_JAIL_INT						(0)
#define IC_JAIL_VW						(5114)

#define OOC_JAIL_X						(197.1209)
#define OOC_JAIL_Y						(176.3204)
#define OOC_JAIL_Z 						(1003.0234)
#define OOC_JAIL_INT					(3)

#include "divisions/core"
#include <faction_system\police\beanbag_taser>
#include <faction_system\police\trace>
#include <faction_system\police\mdc>

#include <YSI_Coding\y_hooks>



static enum e_POLICE_ARREST
{
	Float:X,
	Float:Y,
	Float:Z,
	Interior,
};
static PoliceArrestPositions[][e_POLICE_ARREST] = 
{
	{1791.9943,-1532.5957,5700.4287, 0}
	//{DEFAULT_SPAWN_X, DEFAULT_SPAWN_Y, DEFAULT_SPAWN_Z, 0}
};

hook OnGameModeInit()
{
    for(new i = 0, j = sizeof(PoliceArrestPositions); i < j; ++i)
	{
		Pickup_Create(1239, i, PoliceArrestPositions[i][X], PoliceArrestPositions[i][Y], PoliceArrestPositions[i][Z], ELEMENT_TYPE_ARREST, -1, -1);
		CreateDynamic3DTextLabel("Punto Arresto", COLOR_BLUE, PoliceArrestPositions[i][X], PoliceArrestPositions[i][Y], PoliceArrestPositions[i][Z], 15.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, -1, -1);
	}
	
	// /lasciacarcere
	Pickup_Create(1239, 0, 2649.7790, -1948.9510, -58.7273, ELEMENT_TYPE_JAIL_EXIT, .worldid = -1, .interiorid = 0);
	CreateDynamic3DTextLabel("/lasciacarcere", COLOR_BLUE, 2649.7790, -1948.9510, -58.7273 + 0.55, 20.0, .worldid = -1, .interiorid = 0);
    return 1;
}

hook OnCharacterSpawned(playerid)
{
	if(Character_IsFactionDuty(playerid) && Faction_GetType(Character_GetFaction(playerid)) == FACTION_TYPE_POLICE)
	{
		SetPlayerColor(playerid, COLOR_SLATEBLUE);
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