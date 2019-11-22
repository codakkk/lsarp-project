


/*#include <server\maps\lsa_pwncitta_28agosto>
#include <server\maps\lsa_pwncitta_28agosto_removes>

#include <server\maps\lsa_pwnexteriorstxstudio_28agosto>
#include <server\maps\lsa_pwnexteriorstxstudio_28agosto_removes>

#include <server\maps\lsa_pwninteriors_28agosto>
#include <server\maps\lsa_pwninteriors_28agosto_removes>*/
#include <modelsize>
#include <YSI_Coding\y_hooks>

stock Server_CreateObject(modelid, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, worldid = -1, interiorid = -1, playerid = -1, Float:streamdistance = 0.0, Float:drawdistance = 0.0)
{
	#pragma unused streamdistance, drawdistance
	return CreateDynamicObject(modelid, x, y, z, rx, ry, rz, worldid, interiorid, playerid, 200 + GetColSphereRadius(modelid));
}

hook OnPlayerConnect(playerid)
{
	// CANTIERE DISTRETTO LVPD EXTERIOR
	RemoveBuildingForPlayer(playerid, 7984, 1707.390, 1206.609, 24.234, 0.250);
	RemoveBuildingForPlayer(playerid, 8124, 1707.390, 1206.609, 24.234, 0.250);
	RemoveBuildingForPlayer(playerid, 1616, 1700.430, 1250.680, 16.031, 0.250);

	// Las_Venturas_Repairs_Exterior
	RemoveBuildingForPlayer(playerid, 7161, 2380.734, 1993.226, 9.843, 0.250);
	RemoveBuildingForPlayer(playerid, 6865, 2368.984, 1984.242, 14.773, 0.250);
	RemoveBuildingForPlayer(playerid, 1532, 2394.007, 2041.156, 9.835, 0.250);

	// Fix
	RemoveBuildingForPlayer(playerid, 1245, 2414.7500, 1944.1719, 6.3672, 0.25);
	RemoveBuildingForPlayer(playerid, 1503, 2424.1563, 2808.0781, 10.2109, 0.25);
	RemoveBuildingForPlayer(playerid, 1503, 1749.7109, 776.4375, 10.2109, 0.25);
	RemoveBuildingForPlayer(playerid, 1245, 2675.5234, 1172.9297, 11.1563, 0.25);
	RemoveBuildingForPlayer(playerid, 1245, 1637.9219, 2396.5625, 11.1719, 0.25);
	RemoveBuildingForPlayer(playerid, 1245, 1704.2891, 2382.1406, 11.1719, 0.25);
	RemoveBuildingForPlayer(playerid, 1245, 1582.9453, 1972.2500, 11.1719, 0.25);
	RemoveBuildingForPlayer(playerid, 1245, 1483.4141, 2033.8281, 11.3750, 0.25);
	RemoveBuildingForPlayer(playerid, 1503, 921.5391, 1652.1172, 8.0391, 0.25);
	RemoveBuildingForPlayer(playerid, 1503, 921.2188, 1814.3125, 8.0391, 0.25);
	RemoveBuildingForPlayer(playerid, 1250, 997.4141, 1707.5234, 10.8516, 0.25);
	RemoveBuildingForPlayer(playerid, 1251, 997.0469, 1710.9531, 11.2656, 0.25);
	RemoveBuildingForPlayer(playerid, 1251, 998.0938, 1755.6875, 11.2656, 0.25);
	RemoveBuildingForPlayer(playerid, 1250, 997.7266, 1759.1250, 10.8516, 0.25);
	RemoveBuildingForPlayer(playerid, 1503, 1436.9922, 1928.0391, 10.2109, 0.25);
	RemoveBuildingForPlayer(playerid, 1245, 1501.6797, 1925.5859, 11.1719, 0.25);
	RemoveBuildingForPlayer(playerid, 1245, 1501.6797, 1925.5859, 11.1719, 0.25);

	// Environment CInese
	RemoveBuildingForPlayer(playerid, 1264, 2260.593, 2293.250, 10.203, 0.250);
	RemoveBuildingForPlayer(playerid, 3460, 2270.460, 2287.531, 13.765, 0.250);
	RemoveBuildingForPlayer(playerid, 1264, 2260.593, 2293.250, 10.203, 0.250);
	RemoveBuildingForPlayer(playerid, 3460, 2270.460, 2287.531, 13.765, 0.250);
	// Tribunale.inc
	RemoveBuildingForPlayer(playerid, 6106, 1226.953, -1656.156, 24.773, 0.250);
	RemoveBuildingForPlayer(playerid, 6102, 1226.953, -1656.156, 24.773, 0.250);
	return 1;
}

hook OnGameModeInit()
{
	new tmpobjid;
	#include "Palestra_LV"
	#include "Monolocale_1"
	#include "Monolocale_2"
	#include "Condominio"
	#include "Fogne"
	#include "Med_LV"
	#include "LVMPD_Interior"
	#include "82th_Police_District"
	#include "Caveau_Banca_Interior"
	#include "The_Visage_Corridoio"
	#include "Appartamento_Visage"
	#include "24_7_1_Interior"
	#include "Banca_Nuova_Interior"
	#include "Complex_1_Interior"
	#include "LVFRD_Interior"
	#include "The_Visage_Pianoterra_Interior"
	#include "Bar_Rustico_Interior"
	#include "Cantiere_Distretto_LVPD_Exterior"
	#include "Las_Venturas_Correctional_Facility_Exterior"
	#include "Las_Venturas_Correctional_Facility_Interior"
	#include "Las_Venturas_Repairs_Exterior"
	#include "Cancelli_PD_Medic"
	#include "Environment_Cinese_1_1"
	#include "Tribunale"
return 1;
}
