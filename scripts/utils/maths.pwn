
stock GetXYInFrontOfPos(Float:xx,Float:yy,Float:a, &Float:x2, &Float:y2, Float:distance)
{
    if(a > 360)
	{
        a = a-360;
    }
    xx += (distance * floatsin(-a, degrees));
    yy += (distance * floatcos(-a, degrees));
    x2 = xx;
    y2 = yy;
}

stock IsPlayerInRangeOfPlayer(playerid, otherid, Float:radius)
{
    new 
        //Float:tx, Float:ty, Float:tz,
        Float:t2x, Float:t2y, Float:t2z;
    //GetPlayerPos(playerid, tx, ty, tz);
    GetPlayerPos(otherid, t2x, t2y, t2z);
    return GetPlayerDistanceFromPoint(playerid, t2x, t2y, t2z) <= radius;
}

stock IsPlayerInRangeOfVehicle(playerid, vehicleid, Float:radius)
{
    if(vehicleid <= 0)
        return 0;
    new Float:x, Float:y, Float:z;
    GetVehiclePos(vehicleid, x, y, z);
    return GetPlayerDistanceFromPoint(playerid, x, y, z) <= radius;
}

stock IsPlayerInRangeOfPickup(playerid, pickupid, Float:radius)
{
    if(pickupid < 0)
        return 0;
    new 
        Float:x, Float:y, Float:z;
    Pickup_GetPosition(pickupid, x, y, z);
    return GetPlayerDistanceFromPoint(playerid, x, y, z) <= radius;
}

stock IsNumeric(const string[])
{
    for (new i = 0, j = strlen(string); i < j; i++)
    {
        if (string[i] > '9' || string[i] < '0') return 0;
    }
    return 1;
}