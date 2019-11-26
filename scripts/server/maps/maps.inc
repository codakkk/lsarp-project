


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

	// Exterior_Gov.inc
	RemoveBuildingForPlayer(playerid, 1359, 1080.250, 1004.453, 10.960, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1003.484, 11.101, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1008.750, 11.101, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1019.281, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1014.015, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1024.546, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1029.812, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1359, 1141.906, 1031.320, 10.960, 0.250);
	RemoveBuildingForPlayer(playerid, 1359, 1025.539, 1045.007, 10.960, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1061.414, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1040.351, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1045.617, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1050.882, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1056.148, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1035.078, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 976.289, 1069.273, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1066.679, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1071.945, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 976.289, 1074.539, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1077.210, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 976.289, 1079.804, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1082.476, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 976.289, 1085.070, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1087.742, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 976.289, 1090.335, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1093.007, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 976.289, 1095.601, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1098.273, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1359, 1150.171, 1098.492, 10.960, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 976.289, 1100.867, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 976.289, 1106.132, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1103.539, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 976.289, 1111.398, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1359, 1033.515, 1112.492, 10.960, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1108.804, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1114.070, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 976.289, 1116.664, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1119.335, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 976.289, 1121.929, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 976.289, 1127.203, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1124.609, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 976.289, 1132.468, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 976.289, 1137.734, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1129.875, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1135.140, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 976.289, 1143.000, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 976.289, 1148.265, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 976.289, 1158.796, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 976.289, 1153.531, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1359, 1095.656, 1139.289, 10.960, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1156.203, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1145.671, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1150.937, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1140.406, 11.171, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 979.125, 1161.687, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 984.390, 1161.687, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 994.921, 1161.687, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 989.656, 1161.687, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1005.453, 1161.687, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1000.187, 1161.687, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1010.718, 1161.687, 11.406, 0.250);
	RemoveBuildingForPlayer(playerid, 1215, 1021.898, 1163.140, 10.335, 0.250);
	RemoveBuildingForPlayer(playerid, 1215, 1032.906, 1163.140, 10.335, 0.250);
	RemoveBuildingForPlayer(playerid, 1411, 1178.671, 1161.468, 11.171, 0.250);
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
	//#include "Bar_Rustico_Interior"
	#include "Cantiere_Distretto_LVPD_Exterior"
	#include "Las_Venturas_Correctional_Facility_Exterior"
	#include "Las_Venturas_Correctional_Facility_Interior"
	#include "Las_Venturas_Repairs_Exterior"
	#include "Cancelli_PD_Medic"
	#include "Environment_Cinese_1_1"
	#include "Tribunale"
	#include "Exterior_Gov"
	#include "GarageGrande_Texturizzato"
	#include "GarageMedio_Texturizzato"
	#include "GaragePiccolo_Texturizzato"
	#include "Sushi_Interior"
	#include "Craw_Bar"

	#include "Interior_Piccolo_Vuoto"
	#include "Interior_Medio_Vuoto"
	#include "Interior_Grande_Vuoto"
return 1;
}
