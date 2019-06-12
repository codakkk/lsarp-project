#include <YSI_Coding\y_hooks>

// Should I put here all globals? (gCharacterLogged, etc?)

new Text:txtAnimHelper,
	Text:Clock
;

// Player Texts
new
	PlayerText:pMoneyTD[MAX_PLAYERS],
	PlayerText:pInfoText[MAX_PLAYERS],
	PlayerText:pVehicleFuelText[MAX_PLAYERS],
	PlayerText:pJailTimeText[MAX_PLAYERS],
	PlayerText:pDeathTextDraw[MAX_PLAYERS]
;

// 3D Texts
new 
	Text3D:gPlayerAMe3DText[MAX_PLAYERS],
	Text3D:pAdminDuty3DText[MAX_PLAYERS]
;

new
	Timer:gLoginTimer[MAX_PLAYERS],
	MoneyTimer[MAX_PLAYERS],
	bool:pInfoBoxShown[MAX_PLAYERS],
	gLoginAttempts[MAX_PLAYERS char],
	gPlayerJetpack[MAX_PLAYERS], // Should I define it inside anticheat.pwn?
	pAdminDuty[MAX_PLAYERS],
	pDisableAdminAlerts[MAX_PLAYERS],
	pSupporterDuty[MAX_PLAYERS],
	bool:pAnimLoop[MAX_PLAYERS char],
	gBuyingVehicle[MAX_PLAYERS],
	gBuyingVehicleID[MAX_PLAYERS],
	gBuyingVehiclePrice[MAX_PLAYERS],
	VehicleColorNum1[MAX_PLAYERS],
	VehicleColorNum2[MAX_PLAYERS],
	gCurrentShowRoom[MAX_PLAYERS],
	gPlayerAMeExpiry[MAX_PLAYERS],
	pLastAdminQuestionTime[MAX_PLAYERS],
	pVehicleSellingTo[MAX_PLAYERS],
	pSellingVehicleID[MAX_PLAYERS],
	pVehicleSeller[MAX_PLAYERS],
	pVehicleSellingPrice[MAX_PLAYERS],
	pSelectedListItem[MAX_PLAYERS] // Used by Dialogs
	;

#include <player\components\timers.pwn>


hook OnPlayerClearData(playerid)
{
	printf("OnPlayerClearData");
	gPlayerJetpack[playerid] = 0;
	pAdminDuty[playerid] = 0;
	pSupporterDuty[playerid] = 0;
	Character_SetLastPickup(playerid, -1);
	pSelectedListItem[playerid] = -1;
	if(IsValidDynamic3DTextLabel(pAdminDuty3DText[playerid]))
	{
	   DestroyDynamic3DTextLabelEx(pAdminDuty3DText[playerid]);
	   pAdminDuty3DText[playerid] = Text3D:INVALID_3DTEXT_ID;
	}
	DestroyDynamic3DTextLabelEx(gPlayerAMe3DText[playerid]);
	gPlayerAMeExpiry[playerid] = 0;
	pLastAdminQuestionTime[playerid] = 0;
	return 1;
}