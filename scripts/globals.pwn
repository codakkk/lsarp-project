#include <YSI\y_hooks>

// Should I put here all globals? (gCharacterLogged, etc?)

new
    gAccountLogged[MAX_PLAYERS], // Did we logged with our Account? (1/0)
    gCharacterLogged[MAX_PLAYERS], // Did we selected a Character? (1/0)
    gLoginAttempts[MAX_PLAYERS],
    gPlayerJetpack[MAX_PLAYERS], // Should I define it inside anticheat.pwn?
    pAdminDuty[MAX_PLAYERS],
    pDisableAdminAlerts[MAX_PLAYERS],
    Text3D:pAdminDuty3DText[MAX_PLAYERS],
    pSupporterDuty[MAX_PLAYERS],
    pDeathState[MAX_PLAYERS], // 0 = Not dead, 1 = Waiting for healing, 2 = Death
    gPlayerLastPickup[MAX_PLAYERS],
    gBuyingVehicle[MAX_PLAYERS],
    gBuyingVehicleID[MAX_PLAYERS],
    gBuyingVehiclePrice[MAX_PLAYERS],
    VehicleColorNum1[MAX_PLAYERS],
    VehicleColorNum2[MAX_PLAYERS],
    gCurrentShowRoom[MAX_PLAYERS],
    gAdminVehicle[MAX_PLAYERS],
    gRespawnVehicle[MAX_VEHICLES],
    //gPlayerVehicles[MAX_PLAYERS][]
   
    Text3D:gPlayerAMe3DText[MAX_PLAYERS],
    gPlayerAMeExpiry[MAX_PLAYERS],

    pVehicleKey[MAX_PLAYERS],

    gVehicleDestroyTime[MAX_VEHICLES],
    pLastAdminQuestionTime[MAX_PLAYERS],

    pVehicleSellingTo[MAX_PLAYERS],
    pSellingVehicleID[MAX_PLAYERS],
    pVehicleSeller[MAX_PLAYERS],
    pVehicleSellingPrice[MAX_PLAYERS]
    ;

hook OnPlayerClearData(playerid)
{
    gAccountLogged[playerid] = 0;
    gCharacterLogged[playerid] = 0;
    gPlayerJetpack[playerid] = 0;
    pDeathState[playerid] = 0;
    pAdminDuty[playerid] = 0;
    pSupporterDuty[playerid] = 0;
    gPlayerLastPickup[playerid] = -1;
    if(gAdminVehicle[playerid] != 0)
    {
        DestroyVehicle(gAdminVehicle[playerid]);
    }
    gAdminVehicle[playerid] = 0;
    if(IsValidDynamic3DTextLabel(pAdminDuty3DText[playerid]))
    {
        DestroyDynamic3DTextLabel(pAdminDuty3DText[playerid]);
        pAdminDuty3DText[playerid] = Text3D:INVALID_3DTEXT_ID;
    }
    DestroyDynamic3DTextLabel(gPlayerAMe3DText[playerid]);
    gPlayerAMeExpiry[playerid] = 0;
    pVehicleKey[playerid] = 0;
    pLastAdminQuestionTime[playerid] = 0;
    return 1;
}