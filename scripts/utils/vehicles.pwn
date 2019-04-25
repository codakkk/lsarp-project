

stock GetClosestVehicle(playerid, Float:radius)
{
    if(IsPlayerInAnyVehicle(playerid))
	   return GetPlayerVehicleID(playerid);
    
    new 
	   Float:x, Float:y, Float:z;
    foreach(new i : Vehicles)
    {
	   GetVehiclePos(i, x, y, z);
	   if(IsPlayerInRangeOfPoint(playerid, radius, x, y, z))
	   {
		  return i;
	   }
    }
    return 0;
}