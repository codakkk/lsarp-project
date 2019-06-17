
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

	//Clock System
    new hour, minute, second, string[64];
    gettime(hour, minute, second);
	// hour += TIME_ZONE; // TIME_ZONE = 0
	if(hour > 24) hour -= 24;
	if(hour < 0) hour += 24;
	format(string,25,"%02d:%02d:%02d",hour, minute, second);
    TextDrawSetString(Clock, string);
	return 1;
}

ptask BigTimerForPlayer[15000+1](playerid)
{
	new vehicleid = GetPlayerVehicleID(playerid);
	if(vehicleid > 0)
	{
		if(Vehicle_GetFuel(vehicleid) > 0.0)
		{
			//!IsAHelicopter(vehicleid) && !IsAPlane(vehicleid) && !IsABoat(vehicleid)
			if(Vehicle_IsEngineOn(vehicleid) && !IsABike(vehicleid))
			{
				Vehicle_GiveFuel(vehicleid, -0.5);
			}
		}
		else
		{
			Player_Info(playerid, "Benzina ~r~finita~w~.", true, 4000);
			Vehicle_SetEngineOff(vehicleid);
		}
		new Float:health = Vehicle_GetHealth(vehicleid);
		if(health < 350.0 && Vehicle_IsEngineOn(vehicleid))
		{
			Vehicle_SetEngineOff(vehicleid);
			SendClientMessage(playerid, COLOR_ERROR, "Il motore del veicolo si è spento poiché è danneggiato.");
		}
	}
	return 1;
}

task GlobalMinuteTimer[60000]() // 
{
	// PAY DAY
	foreach(new p : Player)
	{
		if(!Character_IsLogged(p))
			continue;
		PlayerInfo[p][pPayDay]++;
		if(PlayerInfo[p][pPayDay] >= 60) // Pay Day Time in minutes
		{
			PlayerInfo[p][pPayDay] -= 60;
			Character_PayDay(p);
		}
	}
}

ptask CharacterMinuteTimer[60000](playerid)
{
	if(!Character_IsLogged(playerid))
		return Y_HOOKS_BREAK_RETURN_0;
	return Y_HOOKS_CONTINUE_RETURN_1;
}

ptask CharacterSmallTimer[250](playerid)
{
	return Y_HOOKS_CONTINUE_RETURN_1;
}

task HourTimer[1000 * 60 * 60]() 
{
	return Y_HOOKS_CONTINUE_RETURN_1;
}