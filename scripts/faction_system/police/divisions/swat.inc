
#include <YSI_Coding\y_hooks>

flags:swat(CMD_POLICE);
CMD:swat(playerid, params[])
{
    if(!Character_IsFactionDuty(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Devi essere in servizio per poter utilizzare questo comando.");
    if(Character_GetDivision(playerid) != POLICE_DIVISION_SWAT)
        return SendClientMessage(playerid, COLOR_ERROR, "Non fai parte della divisione S.W.A.T.");
    SetPlayerHealth(playerid, 100.0);
    SetPlayerArmour(playerid, 150.0);

    Character_ResetWeapons(playerid);

    Character_GiveWeapon(playerid, WEAPON_NITESTICK, 1);
    Character_GiveWeapon(playerid, WEAPON_CAMERA, 500);
    Character_GiveWeapon(playerid, WEAPON_DEAGLE, 50);
    Character_GiveWeapon(playerid, WEAPON_SPRAYCAN, 99999);
    Character_GiveWeapon(playerid, WEAPON_SHOTGUN, 50);
    Character_GiveWeapon(playerid, WEAPON_M4, 500);
    return 1;
}

