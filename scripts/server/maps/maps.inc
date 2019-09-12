


/*#include <server\maps\lsa_pwncitta_28agosto>
#include <server\maps\lsa_pwncitta_28agosto_removes>

#include <server\maps\lsa_pwnexteriorstxstudio_28agosto>
#include <server\maps\lsa_pwnexteriorstxstudio_28agosto_removes>

#include <server\maps\lsa_pwninteriors_28agosto>
#include <server\maps\lsa_pwninteriors_28agosto_removes>*/

#include <YSI_Coding\y_hooks>

hook OnPlayerConnect(playerid)
{
	// CANTIERE DISTRETTO LVPD EXTERIOR
	RemoveBuildingForPlayer(playerid, 7984, 1707.390, 1206.609, 24.234, 0.250);
	RemoveBuildingForPlayer(playerid, 8124, 1707.390, 1206.609, 24.234, 0.250);
	RemoveBuildingForPlayer(playerid, 1616, 1700.430, 1250.680, 16.031, 0.250);

	// Las_Venturas_Repairs_Exterior
	RemoveBuildingForPlayer(playerid, 7161, 2380.734, 1993.226, 9.843, 0.250);
	RemoveBuildingForPlayer(playerid, 6865, 2368.984, 1984.242, 14.773, 0.250);
	RemoveBuildingForPlayer(playerid, 6907, 2380.734, 1993.226, 9.843, 0.250);
	RemoveBuildingForPlayer(playerid, 647, 2374.718, 2012.179, 11.656, 0.250);
	RemoveBuildingForPlayer(playerid, 712, 2378.882, 2013.140, 18.703, 0.250);
	RemoveBuildingForPlayer(playerid, 647, 2381.804, 2012.179, 11.656, 0.250);
	RemoveBuildingForPlayer(playerid, 1532, 2394.007, 2041.156, 9.835, 0.250);
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
return 1;
}
