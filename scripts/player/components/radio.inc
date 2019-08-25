#include <YSI_Coding\y_hooks>

static 
	RadioFrequency[MAX_PLAYERS],
	RadioTraceFrequency[MAX_PLAYERS],
	RadioMessageLastTick[MAX_PLAYERS],
	Iterator:RadioGangZone[MAX_PLAYERS]<MAX_PLAYERS>
;

hook OnGameModeInit() 
{
	Iter_Init(RadioGangZone);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

static Character_CreateRadioGZ(playerid, Float:range, frequency) 
{
	new 
		Float:posX,
		Float:posY,
		Float:posZ,
		zoneid
	;

	foreach(new i : Player)
	{
		if(Character_IsConnectedToFreq(i, frequency) == 0 || (GetTickCount() - Radio_GetMessageLastTick(i)) > 10000)
			continue;

		GetPlayerPos(i, posX, posY, posZ);

		zoneid = GangZoneCreate(posX - range, posY - range, posX + range, posY + range);
		GangZoneShowForPlayer(playerid, zoneid, COLOR_RECEIVEPM);
		Iter_Add(RadioGangZone[playerid], zoneid);
	}
	return 1;
}

stock Character_DestroyRadioGZ(playerid) 
{
	if(Iter_Count(RadioGangZone[playerid]) != 0)
	{
		foreach(new zoneid : RadioGangZone[playerid])
		{
			GangZoneHideForPlayer(playerid, zoneid);
			GangZoneDestroy(zoneid);
		}
		
		Iter_Clear(RadioGangZone[playerid]);
	}
	return 1;
}

hook OnPlayerClearData(playerid) 
{
	RadioFrequency[playerid] = 0;
	RadioTraceFrequency[playerid] = 0;
	RadioMessageLastTick[playerid] = 10000;
	Character_DestroyRadioGZ(playerid);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

stock Character_SetRadioFrequency(playerid, frequency)
{
    if(!Radio_IsValidFrequency(frequency))
        return 0;
	RadioFrequency[playerid] = frequency;
    return 1;
}

stock Character_GetRadioFrequency(playerid)
{
	return RadioFrequency[playerid];
}

stock Character_SetTraceFrequency(playerid, frequency)
{
    if(!Radio_IsValidFrequency(frequency))
        return 0;
	RadioTraceFrequency[playerid] = frequency;
    return 1;
}

stock Character_GetTraceFrequency(playerid)
{
	return RadioTraceFrequency[playerid];
}

stock Radio_SetMessageLastTick(playerid, tick)
{
	RadioMessageLastTick[playerid] = tick;
    return 1;
}

stock Radio_GetMessageLastTick(playerid)
{
	return RadioMessageLastTick[playerid];
}

stock Character_IsConnectedToFreq(playerid, frequency)
{
	return (!Character_HasItem(playerid, gItem_Radio) || Character_GetRadioFrequency(playerid) != frequency) ? 0 : 1;
}

stock Radio_IsValidFrequency(frequency) 
{
	return (frequency < 0 || frequency > 999) ? 0 : 1;
}

stock Radio_SendTraceTrigger(frequency)
{
	foreach(new i : Player) 
	{
		if(Character_GetTraceFrequency(i) != frequency)
			continue;

    	Character_DestroyRadioGZ(i);
	    Character_CreateRadioGZ(i, 75.0, frequency);
	}
    return 1;
}

flags:frequenza(CMD_ALIVE_USER);
CMD:frequenza(playerid, params[])
{
	if(!Character_HasItem(playerid, gItem_Radio))
    	return SendClientMessage(playerid, COLOR_ERROR, "Non possiedi una radio.");

    new inputFrequency;

    if(sscanf(params, "d", inputFrequency))
    	return SendClientMessage(playerid, COLOR_ERROR, "/(freq)uenza <numero> (0: SPENTA - 1/999).");

    if(!Character_SetRadioFrequency(playerid, inputFrequency))
    	return SendClientMessage(playerid, COLOR_ERROR, "/(freq)uenza <numero> (0: SPENTA - 1/999).");

    if(inputFrequency == 0)
    	SendClientMessage(playerid, COLOR_BLUE, "Hai spento la radio.");
    else
    	SendFormattedMessage(playerid, COLOR_BLUE, "Ti sei connesso alla frequenza %d.", Character_GetRadioFrequency(playerid));
	return 1;
}
alias:frequenza("freq");

flags:radio(CMD_ALIVE_USER);
CMD:radio(playerid, params[])
{
    if(!Character_HasItem(playerid, gItem_Radio, 1))
    	return SendClientMessage(playerid, COLOR_ERROR, "Non possiedi una radio.");

    if(Character_GetRadioFrequency(playerid) <= 0)
    	return SendClientMessage(playerid, COLOR_ERROR, "Non sei connesso ad una frequenza (/(freq)uenza).");

    if(strlen(params) <= 0 || strlen(params) > 128)
    	return SendClientMessage(playerid, COLOR_ERROR, "/r <testo>.");

    new frequency = Character_GetRadioFrequency(playerid);

    ProxDetectorStr(playerid, 15.0, str_format("(Radio) %s dice: %s", Character_GetRolePlayName(playerid), params), COLOR_FADE1, COLOR_FADE2, COLOR_FADE3, COLOR_FADE4, COLOR_FADE5, false, false);
    
    SendMessageToRadio(playerid, frequency, COLOR_YELLOW, "** [CH: %d] %s: %s", frequency, Character_GetRolePlayName(playerid), params);	
    return 1;
}
alias:radio("r");

flags:radiolow(CMD_ALIVE_USER);
CMD:radiolow(playerid, params[])
{
    if(!Character_HasItem(playerid, gItem_Radio, 1))
    	return SendClientMessage(playerid, COLOR_ERROR, "Non possiedi una radio.");

    if(Character_GetRadioFrequency(playerid) <= 0)
    	return SendClientMessage(playerid, COLOR_ERROR, "Non sei connesso ad una frequenza (/(freq)uenza).");

    if(strlen(params) <= 0 || strlen(params) > 128)
    	return SendClientMessage(playerid, COLOR_ERROR, "/rlow <testo>.");

    new frequency = Character_GetRadioFrequency(playerid);

    ProxDetectorStr(playerid, 5.0, str_format("(Radio) %s dice: %s", Character_GetRolePlayName(playerid), params), COLOR_FADE1, COLOR_FADE2, COLOR_FADE3, COLOR_FADE4, COLOR_FADE5, false, false);
    
    SendMessageToRadio(playerid, frequency, COLOR_YELLOW, "** [CH: %d] %s: %s", frequency, Character_GetRolePlayName(playerid), params);	
    return 1;
}
alias:radiolow("rlow");

flags:rtrace(CMD_POLICE);
CMD:rtrace(playerid, params[])
{
    if(!Character_HasItem(playerid, gItem_Radio, 1))
    	return SendClientMessage(playerid, COLOR_ERROR, "Non possiedi una radio.");

    new inputFrequency;

    if(sscanf(params, "d", inputFrequency))
    	return SendClientMessage(playerid, COLOR_ERROR, "/rtrace <frequenza> (0: DISATTIVA - 1/999).");

    if(!Character_SetTraceFrequency(playerid, inputFrequency))
    	return SendClientMessage(playerid, COLOR_ERROR, "/rtrace <frequenza> (0: DISATTIVA - 1/999).");

    Character_DestroyRadioGZ(playerid);

    if(inputFrequency == 0)
     	SendClientMessage(playerid, COLOR_SLATEBLUE, "Hai disattivato il Radio Trace.");
    else
     	SendFormattedMessage(playerid, COLOR_SLATEBLUE, "Hai attivato il Radio Trace nella frequenza %d.", inputFrequency);
    return 1;
}

stock SendMessageToRadio(issuerid, frequency, color, const msg[], GLOBAL_TAG_TYPES:...)
{
    foreach(new i : Player)
	{
        // We should avoid this Character_HasItem call here. It's a little bit expansive on the long road.
		if(Character_IsConnectedToFreq(i, frequency) == 0)
			continue;

		SendTwoLinesMessage(i, color, msg, ___4);
	}	
	Radio_SetMessageLastTick(issuerid, GetTickCount());
	Radio_SendTraceTrigger(frequency);
    return 1;
}