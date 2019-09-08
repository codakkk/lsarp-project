


/*#include <server\maps\lsa_pwncitta_28agosto>
#include <server\maps\lsa_pwncitta_28agosto_removes>

#include <server\maps\lsa_pwnexteriorstxstudio_28agosto>
#include <server\maps\lsa_pwnexteriorstxstudio_28agosto_removes>

#include <server\maps\lsa_pwninteriors_28agosto>
#include <server\maps\lsa_pwninteriors_28agosto_removes>*/

#include <YSI_Coding\y_hooks>



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
return 1;
}
