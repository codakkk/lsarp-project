#include <YSI/y_hooks>

#include <anticheat/anticheat_funcs.pwn>
// Should I use Nex-AC? To add a better layer of security?


// Is this Timer expansive?
// probably yes, but still better than OnPlayerUpdate (called each tick)
ptask AntiCheatTimer[500](playerid) 
{
    if(!gAccountLogged[playerid] || !gCharacterLogged[playerid] /*|| AccountInfo[playerid][aAdmin] > 2*/)
        return 0;

    new 
        Float:hp, 
        Float:ac_hp,
        Float:armour,
        Float:ac_armour;
    
    GetPlayerHealth(playerid, hp);
    GetPlayerArmour(playerid, armour);

    AC_GetPlayerHealth(playerid, ac_hp);
    AC_GetPlayerArmour(playerid, ac_armour);
    //printf("%f - %f", hp, ac_hp);
    if(hp > ac_hp)
    {
        //AC_SetPlayerHealth(playerid, ac_hp);
    }
    else // Lower (like for falling damage or weapons damage)
    {
        //AC_SetPlayerHealth(playerid, hp);
    }

    if(armour > ac_armour)
    {
        //AC_SetPlayerArmour(playerid, ac_armour);
    }
    else // like hp
    {
        //AC_SetPlayerArmour(playerid, armour);
    }

    if(GetPlayerMoney(playerid) != AC_GetPlayerMoney(playerid))
    {
        AC_GivePlayerMoney(playerid, 0); // Resets player money
    }

    // Anti-cheats we can pass-on if we're admins goes here.
    if(AccountInfo[playerid][aAdmin] > 1)
        return 1;
    
    if(GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_USEJETPACK && !gPlayerJetpack[playerid])
    {
        ClearAnimations(playerid, 1);
        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);
        SetPlayerPos(playerid, x, y, z);
    }
    return 1;
}