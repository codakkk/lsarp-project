#include <YSI_Coding\y_hooks>

static gCurrentWeather, gNextWeather;

hook OnGameModeInit()
{
	gCurrentWeather = 0;
	gNextWeather = GenerateWeather();

	Server_UpdateWeather(); 
	return 1;
}

hook OnCharacterSpawn(playerid)
{
	SetPlayerWeather(playerid, gCurrentWeather);
	return 1;
}

hook OnCharacterDeath(playerid, killerid, reason)
{
	SetPlayerWeather(playerid, gCurrentWeather);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

task OnWeatherUpdateTimer[1000 * 60 * 60]()
{
	Server_UpdateWeather();
	foreach (new i : Player)
	{
		if(!Character_IsLogged(i))
			continue;
		//if(!DrugWeather[i])
		SetPlayerWeather(i, Server_GetCurrentWeather());
	}
}

stock GenerateWeather()
{
	new
	    weather,
		rand;
	switch(Server_GetCurrentWeather())
	{
		case 0..7, 10:
		{
			rand = random(11);
			switch(rand)
			{
				case 0, 1, 2, 9, 10:
				{
					rand = random(9);
					if(rand == 8) rand = 10;
					weather = rand;
				}
				case 3, 4:
				{
					rand = random(2);
					if(rand == 0) rand = 12;
					if(rand == 1) rand = 12;
					weather = rand;
				}
				case 5, 6:
				{
					rand = random(3);
					if(rand == 0) rand = 17;
					if(rand == 1) rand = 18;
					if(rand == 2) rand = 11;
					weather = rand;
				}				
			}
		}		
		case 12, 35:
		{
            rand = random(11);
			switch(rand)
			{
				case 0, 1, 2:
				{
					rand = random(2);
					if(rand == 0) rand = 12;
					if(rand == 1) rand = 12;
					weather = rand;
				}				
				case 7:
				{
					if(rand == 0) rand = 17;
					if(rand == 1) rand = 18;
					if(rand == 2) rand = 11;
					weather = rand;
				}
				case 8, 9, 10:
				{
					rand = random(9);
					if(rand == 8) rand = 10;
					weather = rand;
				}
			}
		}
		case 17..18, 11:
		{
            rand = random(15);
			switch(rand)
			{
				case 0, 1, 2, 4, 13, 14:
				{
					rand = random(3);
					if(rand == 0) rand = 17;
					if(rand == 1) rand = 18;
					if(rand == 2) rand = 11;
					weather = rand;
				}
				case 5, 6, 7, 11, 12:
				{
                    rand = random(9);
					if(rand == 8) rand = 10;
					weather = rand;
				}
				case 8:
				{
					rand = random(2);
					if(rand == 0) rand = 12;
					if(rand == 1) rand = 12;
					weather = rand;
				}			
			}
		}
	}
	return weather;
}

// Called to update weather.
stock Server_UpdateWeather()
{
	Server_SetCurrentWeather(Server_GetNextWeather());
	SetWeather(gCurrentWeather);
	foreach(new i : Player)
		SetPlayerWeather(i, gCurrentWeather);
	Server_SetNextWeather(GenerateWeather());
}

stock Server_SetCurrentWeather(weatherid)
{
	gCurrentWeather = weatherid;
}

stock Server_GetCurrentWeather()
{
	return gCurrentWeather;
}

stock Server_SetNextWeather(weatherid)
{
	gNextWeather = weatherid;
}

stock Server_GetNextWeather()
{
	return gNextWeather;
}

CMD:mercy(playerid, params[])
{
	SendClientMessage(playerid, COLOR_ERROR, "Ciao Mercy");
	return 1;
}