#include <YSI_Coding\y_hooks>

static RadioFrequency[MAX_PLAYERS];

hook OnPlayerClearData(playerid) 
{
	RadioFrequency[playerid] = 0;
	return Y_HOOKS_CONTINUE_RETURN_1;
}

stock Character_SetRadioFrequency(playerid, frequency)
{
    if(frequency < 0 || frequency > 999)
        return 0;
	RadioFrequency[playerid] = frequency;
    return 1;
}

stock Character_GetRadioFrequency(playerid)
{
	return RadioFrequency[playerid];
}

flags:frequenza(CMD_ALIVE_USER);
CMD:frequenza(playerid, params[])
{
	if(!Character_HasItem(playerid, gItem_Radio))
    	return SendClientMessage(playerid, COLOR_ERROR, "Non possiedi una radio.");

    new inputFrequency;

    if(sscanf(params, "d", inputFrequency))
    	return SendClientMessage(playerid, COLOR_WHITE, "/frequenza <numero> (0: SPENTA - 1/999).");

    if(!Character_SetRadioFrequency(playerid, inputFrequency))
    	return SendClientMessage(playerid, COLOR_WHITE, "/frequenza <numero> (0: SPENTA - 1/999).");

    if(inputFrequency == 0)
    	SendClientMessage(playerid, COLOR_BLUE, "Hai spento la radio.");
    else
    	SendFormattedMessage(playerid, COLOR_BLUE, "Ti sei connesso alla frequenza %d.", Character_GetRadioFrequency(playerid));
	return 1;
}

flags:radio(CMD_ALIVE_USER);
CMD:radio(playerid, params[])
{
    if(!Character_HasItem(playerid, gItem_Radio, 1))
    	return SendClientMessage(playerid, COLOR_ERROR, "Non possiedi una radio.");

    if(Character_GetRadioFrequency(playerid) <= 0)
    	return SendClientMessage(playerid, COLOR_ERROR, "Non sei connesso ad una frequenza (/frequenza).");

    if(strlen(params) <= 0 || strlen(params) > 128)
    	return SendClientMessage(playerid, COLOR_WHITE, "/r <testo>.");

    new frequency = Character_GetRadioFrequency(playerid);

    ProxDetectorStr(playerid, 15.0, str_format("(Radio) %s dice: %s", Character_GetRolePlayName(playerid), params), COLOR_FADE1, COLOR_FADE2, COLOR_FADE3, COLOR_FADE4, COLOR_FADE5, false, false);
    
    SendMessageToRadio(frequency, COLOR_YELLOW, "** [CH: %d] %s: %s", frequency, Character_GetRolePlayName(playerid), params);	
    return 1;
}
alias:radio("r");

stock SendMessageToRadio(frequency, color, const msg[], GLOBAL_TAG_TYPES:...)
{
    foreach(new i : Player)
	{
        // We should avoid this Character_HasItem call here. It's a little bit expansive on the long road.
		if(!Character_HasItem(i, gItem_Radio) || Character_GetRadioFrequency(i) != frequency)
			continue;

		SendTwoLinesMessage(i, color, msg, ___2);
	}	
    return 1;
}