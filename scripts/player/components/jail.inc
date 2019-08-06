#include <YSI_Coding\y_hooks>

static 
	PlayerJailTime[MAX_PLAYERS],
	bool:IsJailIC[MAX_PLAYERS char]
;

hook OnPlayerClearData(playerid)
{
	PlayerJailTime[playerid] = 0;
	IsJailIC{playerid} = false;
	return 1;
}

hook OnCharacterLoad(playerid)
{
	inline OnLoad()
	{
		cache_get_value_index_int(0, 0, PlayerJailTime[playerid]);
		new tmp;
		cache_get_value_index_int(0, 1, tmp);
		IsJailIC{playerid} = tmp? true : false;
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT jail_time, jail_ic FROM `characters` WHERE id = '%d'", Character_GetID(playerid));
	return 1;
}



ptask OnJailTimeUpdate[1000](playerid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	if(Character_IsJailed(playerid))
	{
		PlayerJailTime[playerid]--;
		PlayerTextDrawSetStringStr(playerid, pJailTimeText[playerid], str_format("~n~~n~~g~TEMPO RIMANENTE: ~w~~n~%d secondi", Character_GetJailTime(playerid)));
		if(!Character_IsJailed(playerid))
		{
			if(Character_IsICJailed(playerid))
			{
				SendClientMessage(playerid, COLOR_GREEN, "Hai scontato la tua pena. Digita /lasciacarcere al punto per uscire.");
				Player_Info(playerid, "Digita ~y~/lasciacarcere~w~ per usicre.", true, 3000);
			}
			else
			{
				SendClientMessage(playerid, COLOR_GREEN, "(( Hai scontato la tua pena. ))");
				Character_Spawn(playerid);
			}
			PlayerTextDrawHide(playerid, pJailTimeText[playerid]);
			Internal_Save(playerid);	
		}
	}
	return 1;
}

static Internal_Save(playerid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	mysql_tquery_f(gMySQL, "UPDATE `characters` SET jail_time = '%d', jail_ic = '%d' WHERE id = '%d'", Character_GetJailTime(playerid), Character_IsICJailed(playerid), Character_GetID(playerid));
	return 1;
}

stock Character_GetJailTime(playerid)
{
	return PlayerJailTime[playerid];
}

stock Character_SetJailTime(playerid, timeInMinutes)
{
	Character_SetJailTimeAsSeconds(playerid, timeInMinutes * 60);
}

stock Character_SetJailTimeAsSeconds(playerid, timeAsSeconds)
{
	PlayerJailTime[playerid] = timeAsSeconds;
	Internal_Save(playerid);
}

stock Character_IsJailed(playerid)
{
	return ( PlayerJailTime[playerid] > 0 );
}

stock bool:Character_IsICJailed(playerid)
{
	return IsJailIC{playerid};
}

stock Character_SetICJailed(playerid, bool:value)
{
	IsJailIC{playerid} = value;
}