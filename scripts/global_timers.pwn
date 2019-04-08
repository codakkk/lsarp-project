
ptask GlobalPlayerSecondTimer[1000](playerid)
{
    if(gPlayerAMeExpiry[playerid] > 0)
    {
        gPlayerAMeExpiry[playerid]--;
        if(!gPlayerAMeExpiry[playerid])
        {
            UpdateDynamic3DTextLabelText(gPlayerAMe3DText[playerid], 0xFFFFFFFF, " ");
        }
    }
    return 1;
}

task GlobalMinuteTimer[60000]()
{
    // PAY DAY
    foreach(new p : Player)
    {
        if(!gCharacterLogged[p])
            continue;
        PlayerInfo[p][pPayDay]++;
        if(PlayerInfo[p][pPayDay] >= 60) // Pay Day Time in minutes
        {
            PlayerInfo[p][pPayDay] -= 60;
            Character_PayDay(p);
        }
    }

    // VEHICLE UNLOAD DELAY
    foreach(new v : Vehicles)
    {
        if(!VehicleInfo[v][vID] || VehicleInfo[v][vModel] == 0)
            continue;
        if(!Vehicle_IsOwnerConnected(v))
        {
            gVehicleDestroyTime[v]--;
            if(!gVehicleDestroyTime[v])
            {
                printf("Vehicle ID %d unloaded", v);
                Vehicle_Unload(v);
                Iter_SafeRemove(Vehicles, v, v);
            }
        }
    }
}