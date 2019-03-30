
ptask SavePlayerTimer[10000](playerid)
{
    if(!gAccountLogged[playerid] || !gCharacterLogged[playerid])
        return 0;
    Character_Save(playerid);
    return 1;
}