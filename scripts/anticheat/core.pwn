#include <YSI/y_hooks>

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


stock AC_GetPlayerMoney(playerid)
{
    return PlayerInfo[playerid][pMoney];
}

stock AC_SetPlayerSkin(playerid, skinid, reason[] = "")
{
    #pragma unused reason
    if(!gCharacterLogged[playerid])
        return 0;
    SetPlayerSkin(playerid, skinid);
    PlayerInfo[playerid][pSkin] = skinid;
    return 1;
}

stock AC_GivePlayerMoney(playerid, money, reason[] = "")
{
    if(!gAccountLogged[playerid] || !gCharacterLogged[playerid])
        return 0;
    PlayerInfo[playerid][pMoney] += money;
    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, PlayerInfo[playerid][pMoney]);

    // Should I log here?
    #pragma unused reason
    return 1;
}

stock AC_ResetPlayerMoney(playerid, reason[]="")
{
    if(!gCharacterLogged[playerid])
        return 0;
    PlayerInfo[playerid][pMoney] = 0;
    ResetPlayerMoney(playerid);
    return 1;
}

stock AC_SetPlayerHealth(playerid, Float:health)
{
    PlayerInfo[playerid][pHealth] = health;
    SetPlayerHealth(playerid, health); 
    return 1;
}

stock AC_GetPlayerHealth(playerid, &Float:h)
{
    if(!gCharacterLogged[playerid])
        return 0;
    h = PlayerInfo[playerid][pHealth];
    return 1;
}

stock AC_SetPlayerArmour(playerid, Float:armour)
{
    PlayerInfo[playerid][pArmour] = armour;
    SetPlayerArmour(playerid, armour);
    return 1;
}

stock AC_GetPlayerArmour(playerid, &Float:a)
{
    if(!gCharacterLogged[playerid])
        return 0;
    a = PlayerInfo[playerid][pArmour];
    return 1;
}

stock AC_GivePlayerWeapon(playerid, weaponid, ammo)
{
    GivePlayerWeapon(playerid, weaponid, ammo);
    return 1;
}

stock AC_ResetPlayerWeapons(playerid)
{
    ResetPlayerWeapons(playerid);
    return 1;
}