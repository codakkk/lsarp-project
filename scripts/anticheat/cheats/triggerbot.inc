
#include <YSI_Coding\y_hooks>

static
    bool:PlayerAiming[MAX_PLAYERS char] = {false, ...};

hook OnPlayerUpdate(playerid)
{
    PlayerAiming{playerid} = false;
    if(!PlayerAiming{playerid})
    {
        static 
            cameraMode,
            keys, ud, lr
        ;
        cameraMode = GetPlayerCameraMode(playerid);
        GetPlayerKeys(playerid, keys, ud, lr);
        if(/*cameraMode == 53 || cameraMode == 7 ||*/ keys & KEY_FIRE)
        {
            PlayerAiming{playerid} = true;
            printf("Aiming true");
        }
    }
    
    return 1;
}

hook OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
    printf("Shot - TickRate: %d", GetServerTickRate());
    if(!PlayerAiming{playerid})
    {
        SendMessageToAdmins(true, COLOR_ERROR, "Il giocatore %s (%d) è sospetto di Triggerbot.", Character_GetOOCName(playerid), playerid);
        return 0;
    }
    else PlayerAiming{playerid} = false;
    return 1;
}