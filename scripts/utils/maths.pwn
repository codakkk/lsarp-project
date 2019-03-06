
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