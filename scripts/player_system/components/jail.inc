#include <YSI_Coding\y_hooks>

static 
	PlayerJailTime[MAX_PLAYERS],
	bool:IsJailIC[MAX_PLAYERS char]
;

hook OnCharacterClearData(playerid)
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
		Character_SetJailTimeAsSeconds(playerid, Character_GetJailTime(playerid) - 1);
		PlayerTextDrawSetStringStr(playerid, JailTimePlayerText[playerid], str_format("~n~~n~~g~TEMPO RIMANENTE: ~w~~n~%d secondi", Character_GetJailTime(playerid)));
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
				SpawnPlayer(playerid);
			}
			PlayerTextDrawHide(playerid, JailTimePlayerText[playerid]);
		}	
	}
	return 1;
}

stock Character_GetJailTime(playerid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	return PlayerJailTime[playerid];
}

stock Character_SetJailTime(playerid, timeInMinutes)
{
	return Character_SetJailTimeAsSeconds(playerid, timeInMinutes * 60);
}

stock Character_SetJailTimeAsSeconds(playerid, timeAsSeconds)
{
	if(!Character_IsLogged(playerid))
		return 0;
	PlayerJailTime[playerid] = timeAsSeconds;
	mysql_tquery_f(gMySQL, "UPDATE `characters` SET jail_time = '%d' WHERE id = '%d'", Character_GetJailTime(playerid), Character_GetID(playerid));
	return 1;
}

stock Character_IsJailed(playerid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	return ( PlayerJailTime[playerid] > 0 );
}

stock bool:Character_IsICJailed(playerid)
{
	if(!Character_IsLogged(playerid))
		return false;
	return IsJailIC{playerid};
}

stock Character_SetICJailed(playerid, bool:value)
{
	if(!Character_IsLogged(playerid))
		return 0;
	IsJailIC{playerid} = value;
	mysql_tquery_f(gMySQL, "UPDATE `characters` SET jail_ic = '%d' WHERE id = '%d'", Character_IsICJailed(playerid), Character_GetID(playerid));
	return 1;
}