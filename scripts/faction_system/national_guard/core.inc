
#include <pickup_system\core>

#define IC_JAIL_X						(2638.62)
#define IC_JAIL_Y						(-1978.24)
#define IC_JAIL_Z 						(-58.73)
#define IC_JAIL_INT						(2)
#define IC_JAIL_VW						(5018)

#define OOC_JAIL_X						(197.1209)
#define OOC_JAIL_Y						(176.3204)
#define OOC_JAIL_Z 						(1003.0234)
#define OOC_JAIL_INT					(3)

#include <faction_system\national_guard\beanbag_taser>
#include <faction_system\national_guard\trace>
#include <faction_system\national_guard\mdc>

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
	{2661.1257,-1949.2670,-58.7113, 0}
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

stock IsPlayerNearAnyArrestPos(playerid)
{
	new id, E_ELEMENT_TYPE:type;
	if(Character_GetNearestPickup(playerid, id, type) && type == ELEMENT_TYPE_ARREST)
	{
		return 1;
	}
	return 0;
}

/*flags:espulsionezona(CMD_POLICE);
CMD:espulsionezona(playerid, params[])
{
	if(sscanf(params, "k<u>s[128]", id, reason))
		return SendClientMessage(playerid, COLOR_ERROR, "/espulsionezona <playerid/partofname/#####> <motivo>");
	
	SendFormattedMessage(id, COLOR_ERROR, "Sei stato espulso dalla zona sicura da %s.", Character_GetOOCName(playerid));
	SendFormattedMessage(playerid, COLOR_ERROR, "Hai espulso %s dalla zona governativa.", Character_GetOOCName(id));
	return 1;
}*/