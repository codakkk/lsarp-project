#include <YSI_Coding\y_hooks>

flags:dp(CMD_ALIVE_USER);
CMD:dp(playerid, params[])
{
	new slotid, text[128];
	new max_slots = (Account_GetPremiumLevel(playerid) > 1) ? MAX_DPS_PER_PLAYER : 3;
	if(!sscanf(params, "ds[128]", slotid, text) && slotid-- && 0 <= slotid < max_slots)
	{
		if(DPInfo[playerid][slotid][dpExists])
			return SendClientMessage(playerid, COLOR_ERROR, "Hai già utilizzato questo slot.");
		if(strlen(text) > 128)
		{
			strmid(text, text, 0, 128, 128);
		}
		new Float:x, Float:y, Float:z;
		GetPlayerPos(playerid, x, y, z);
		DP_Create(playerid, slotid, text, x, y, z);
		SendClientMessage(playerid, -1, "Hai creato il DP. Verrà rimosso automaticamente tra 60 minuti.");
		SendFormattedMessage(playerid, -1, "Altrimenti puoi utilizzare \"/dp %d\" per farlo in qualsiasi momento.", slotid+1);
	}
	else if(!sscanf(params, "d", slotid) && slotid-- && 0 <= slotid < max_slots)
	{
		if(!DPInfo[playerid][slotid][dpExists])
			return SendClientMessage(playerid, COLOR_ERROR, "Non hai utilizzato questo slot.");
		DP_Destroy(playerid, slotid);
		SendFormattedMessage(playerid, -1, "Hai rimosso il DP %d.", slotid+1);
	}
	else
		return SendFormattedMessage(playerid, COLOR_ERROR, "/dp <slotid 1 - %d> <facoltativo: testo> (usa \"|\" per andare a capo).", max_slots);
	return 1;
}

hook OnPlayerClearData(playerid)
{
	for(new i = 0; i < MAX_DPS_PER_PLAYER; ++i)
	{
		if(!DPInfo[playerid][i][dpExists])
			continue;
		DP_Destroy(playerid, i);
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

stock DP_Create(playerid, slotid, const text[], Float:x, Float:y, Float:z)
{
	if(slotid < 0 || slotid >= MAX_DPS_PER_PLAYER || DPInfo[playerid][slotid][dpExists])
		return 0;
	DPInfo[playerid][slotid][dpExists] = 1;
	DPInfo[playerid][slotid][dpX] = x;
	DPInfo[playerid][slotid][dpY] = y;
	DPInfo[playerid][slotid][dpZ] = z;

	new string[256], rowCounter = 0;
	format(string, sizeof(string), "%s\n{FFFFFF}#%07d-%d", ColouredText(text), Character_GetID(playerid), slotid+1);

	for(new i = 0, j = strlen(string); i < j; ++i)
	{
		if(rowCounter >= 5) 
			break;
		if(string[i] == '|')
		{
			string[i] = '\n';
			rowCounter++;
		}
	}

	DPInfo[playerid][slotid][dpText3D] = CreateDynamic3DTextLabel(string, -1, x, y, z, 30.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), -1);
	DPInfo[playerid][slotid][dpTimer] = defer Timer_DestroyDP(playerid, slotid);
	return 1;
}

stock DP_Destroy(playerid, slotid)
{
	DPInfo[playerid][slotid][dpExists] = 0;
	DPInfo[playerid][slotid][dpX] = 0.0;
	DPInfo[playerid][slotid][dpY] = 0.0;
	DPInfo[playerid][slotid][dpZ] = 0.0;

	stop DPInfo[playerid][slotid][dpTimer];
	DPInfo[playerid][slotid][dpTimer] = Timer:0;
	DestroyDynamic3DTextLabelEx(DPInfo[playerid][slotid][dpText3D]);
}

timer Timer_DestroyDP[6000000](playerid, slotid) 
{
	DP_Destroy(playerid, slotid);
	SendFormattedMessage(playerid, -1, "DP id %d cancellata.", slotid);
}

stock ColouredText(const text[])
{
    enum
        colorEnum
        {
            colorName[16],
            colorID[7]
        }
    ;
    static
        colorInfo[][colorEnum] =
        {
            { "BLUSCURO",       "000F6E" },
            { "BLU",            "1B1BE0" },
            { "ROSA",           "E81CC9" },
            { "GIALLO",         "FFFB00" },
            { "VERDECHIARO",    "8CED15" },
            { "VERDESCURO",     "089C00" },
            { "VERDEACQUA",     "00FFB3" },
            { "CELESTE",        "15D4ED" },
            { "ROSSO",          "FF0000" },
            { "GRIGIO",         "BABABA" },
            { "BIANCO",         "FFFFFF" },
            { "ARANCIONE",      "FFA200" },
            { "VERDE",          "37DB45" },
            { "VIOLA",          "7340DB" },
            { "FUCSIA",         "ED2884" },
            { "LIME",           "D4FF00" },
			{ "MARRONE",        "633224" },
			{ "NERO",           "000000" },
			{ "DIALOG",         "A9C4E4" }
        },
        string[(256 + 32)],
        tempString[16],
        pos = -1,
        x
    ;
    strmid(string, text, 0, 256, sizeof(string));

    for( ; x != sizeof(colorInfo); ++x)
    {
        format(tempString, sizeof(tempString), "#%s", colorInfo[x][colorName]);

        while((pos = strfind(string, tempString, true, (pos + 1))) != -1)
        {
            new
                tempLen = strlen(tempString),
                tempVar,
                i = pos
            ;
            format(tempString, sizeof(tempString), "{%s}", colorInfo[x][colorID]);

            if(tempLen < 8)
            {
                for(new j; j != (8 - tempLen); ++j)
                {
                    strins(string, " ", pos);
                }
            }
            for( ; ((string[i] != 0) && (tempVar != 8)) ; ++i, ++tempVar)
            {
                string[i] = tempString[tempVar];
            }
            if(tempLen > 8)
            {
                strdel(string, i, (i + (tempLen - 8)));
            }
            x = -1;
        }
    }
    return string;
}