
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
