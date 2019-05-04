
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

stock GetXYInFrontOfPlayer(playerid, &Float:x, &Float:y, Float:distance)
{
    // Created by Y_Less

    new Float:a;

    GetPlayerPos(playerid, x, y, a);
    GetPlayerFacingAngle(playerid, a);

    if (GetPlayerVehicleID(playerid)) {
	   GetVehicleZAngle(GetPlayerVehicleID(playerid), a);
    }

    x += (distance * floatsin(-a, degrees));
    y += (distance * floatcos(-a, degrees));
}

stock IsPlayerInRangeOfPlayer(playerid, otherid, Float:radius)
{
	if(playerid == INVALID_PLAYER_ID || otherid == INVALID_PLAYER_ID || GetPlayerInterior(playerid) != GetPlayerInterior(otherid) || GetPlayerVirtualWorld(playerid) != GetPlayerVirtualWorld(otherid))
		return 0;
	if(otherid == playerid) 
		return 1;
    new 
	   Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    return IsPlayerInRangeOfPoint(otherid, radius, x, y, z); //GetPlayerDistanceFromPoint(playerid, x, y, z) <= radius;
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

stock GetPlayerSpeed(playerid,bool:kmh)
{
	new Float:Vx, Float:Vy, Float:Vz, Float:rtn;
	if(IsPlayerInAnyVehicle(playerid)) 
		GetVehicleVelocity(GetPlayerVehicleID(playerid),Vx,Vy,Vz); 
	else
		GetPlayerVelocity(playerid,Vx,Vy,Vz);
	rtn = floatsqroot(Vx*Vx + Vy*Vy + Vz*Vz);
	return kmh ? floatround(rtn * 100 * 1.61) : floatround(rtn * 100);
}
