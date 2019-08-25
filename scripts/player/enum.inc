#include <YSI_Coding\y_hooks>

#define MAX_VEHICLES_PER_PLAYER			20
#define MAX_DAMAGES_PER_PLAYER			(24)
#define PENDING_REQUEST_TIME			1

new
	// probably Iterators aren't worth for this.
	Iterator:pTogglePM[MAX_PLAYERS]<MAX_PLAYERS>,
	Iterator:pToggleOOC[MAX_PLAYERS]<MAX_PLAYERS>
;

enum e_Bit1_Data 
{
	e_pCharacterLogged,
    e_pFreezed,
	e_pToggleFactionOOC,
	e_pFactionDuty,
	e_pSelectingUniform,
	e_pCuffed,
	e_pDragged,
	e_pDragging,
	e_pMasked,
	e_pLegHit,
	e_pAnimLoop,
	e_pBuyingVehicle, // Is Buying something
	e_pCanChopShop,
	e_pCanGetAlive
};

new 
    BitArray:gCharacterBitState[e_Bit1_Data]<MAX_PLAYERS>;



enum _:e_PendingType
{
	REQUEST_TYPE_NONE,
	REQUEST_TYPE_WEAPON,
	REQUEST_TYPE_ITEM,
	REQUEST_TYPE_REPAIR,
	REQUEST_TYPE_VEHICLE_SPRAY,
	REQUEST_TYPE_SEARCH,
	REQUEST_TYPE_BUILDING,
	REQUEST_TYPE_CARE,
	REQUEST_TYPE_FACTION,
	REQUEST_TYPE_VEHICLE,

	REQUEST_TYPE_LAST // MUST BE LAST ONE
}

stock GetRequestTypeName(type)
{
    new l[16];
    switch(type)
    {
	   case REQUEST_TYPE_WEAPON: l = "Arma";
	   case REQUEST_TYPE_ITEM: l = "Oggetto";
	   case REQUEST_TYPE_REPAIR: l = "Riparazione";
	   case REQUEST_TYPE_VEHICLE_SPRAY: l = "Verniciatura";
	   case REQUEST_TYPE_SEARCH: l = "Perquisizione";
	   case REQUEST_TYPE_BUILDING: l = "Edificio";
	   case REQUEST_TYPE_CARE: l = "Cura";
	   default: l = "Sconosciuto";
    }
    return l;
}