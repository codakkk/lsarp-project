#include <YSI_Coding\y_hooks>

task OnServerTimeUpdate[1000]() 
{
    static 
        hour, minute, second, string[64];
    gettime(hour, minute, second);
	
	hour += Server_GetTimeAdd();

	// hour += TIME_ZONE; // TIME_ZONE = 0
	if(hour > 24) hour -= 24;
	if(hour < 0) hour += 24;
	format(string,25,"%02d:%02d:%02d",hour, minute, second);
    TextDrawSetString(Clock, string);
    return 1;
}

ptask UpdatePlayerClockAndTime[1000 * 60](playerid) 
{
    new tmphour, tmpminute, tmpsecond;
    gettime(tmphour, tmpminute, tmpsecond);
    
    tmphour += Server_GetTimeAdd();
    
    FixHour(tmphour);
    SetPlayerTime(playerid, tmphour, tmpminute);
}