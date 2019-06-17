#include <YSI_Coding\y_hooks>

#define foreach_faction(%0)			for_list(%0:FactionsList)

#define MAX_FACTIONS 			20
#define MAX_FACTION_RANKS 		15
#define MAX_FACTION_SKINS		15
#define INVALID_FACTION_ID		(-1)
enum // Faction Type
{
	FACTION_TYPE_POLICE = 0,
	FACTION_TYPE_MEDICAL,
	FACTION_TYPE_GOVERNAMENT,
	FACTION_TYPE_IMPORT_WEAPONS,
	FACTION_TYPE_IMPORT_DRUGS,
	FACTION_TYPE_IMPORT_FOODS,
	FACTION_TYPE_OTHERS,
	// Add types here.
	FACTION_TYPE_LAST
}

enum _:E_FACTION_DATA
{
	fID,
	fCreated,
	fName[32],
	fShortName[8],
	fType,
	Float:fSpawnX,
	Float:fSpawnY,
	Float:fSpawnZ,
	fInterior,
	fWorld,
};

enum _:E_RANK_DATA
{
	rankName[16],
	rankSalary
};

new 
	FactionInfo[MAX_FACTIONS][E_FACTION_DATA],
	Iterator:Factions<MAX_FACTIONS>,
	FactionRankInfo[MAX_FACTIONS][MAX_FACTION_RANKS][E_RANK_DATA],
	FactionSkins[MAX_FACTIONS][MAX_FACTION_SKINS],
	FactionSkinsCount[MAX_FACTIONS char],
	pAdminSelectedFaction[MAX_PLAYERS char],
	pAdminSelectedFactionRank[MAX_PLAYERS char],
	pAdminSelectedFactionSkin[MAX_PLAYERS char]
;

enum e_POLICE_ARREST
{
	Float:aX,
	Float:aY,
	Float:aZ,
	aInterior,
};
new PoliceArrestPositions[][e_POLICE_ARREST] = 
{
	{2638.6470, -1955.7460, -58.7273, 0}
};

stock IsPlayerNearAnyArrestPos(playerid)
{
	new id, E_ELEMENT_TYPE:type;
	if(Pickup_GetInfo(Character_GetLastPickup(playerid), id, type) && type == ELEMENT_TYPE_ARREST && IsPlayerInRangeOfPickup(playerid, Character_GetLastPickup(playerid), 3.0))
	{
		return 1;
	}
	return 0;
}
	//Iterator:Factions<MAX_FACTIONS>;