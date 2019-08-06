#include <YSI_Coding\y_hooks>

// Should I put here all globals? (gCharacterLogged, etc?)

new Text:txtAnimHelper,
	Text:Clock
;

// Player Texts


new
	MoneyTimer[MAX_PLAYERS],
	bool:pInfoBoxShown[MAX_PLAYERS char],
	gLoginAttempts[MAX_PLAYERS char],
	pDisableAdminAlerts[MAX_PLAYERS],
	gBuyingVehicleID[MAX_PLAYERS],
	gBuyingVehiclePrice[MAX_PLAYERS],
	pLastAdminQuestionTime[MAX_PLAYERS],
	pVehicleSellingTo[MAX_PLAYERS],
	pSellingVehicleID[MAX_PLAYERS],
	pVehicleSeller[MAX_PLAYERS],
	pVehicleSellingPrice[MAX_PLAYERS],
	pSelectedListItem[MAX_PLAYERS] // Used by Dialogs
	;


hook OnPlayerClearData(playerid)
{
	printf("OnPlayerClearData");
	pSelectedListItem[playerid] = -1;
	pLastAdminQuestionTime[playerid] = 0;
	return 1;
}