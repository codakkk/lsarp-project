#include <YSI_Coding\y_hooks>

hook OnGameModeInit()
{
    //UpdateServerTime();
    return 1;
}

hook OnPlayerConnect(playerid)
{
    UpdatePlayerTime(playerid);
    return 1;
}

task OnServerTimeUpdate[1000]() 
{
    static 
        hour, minute, second;
    gettime(hour, minute, second);
	
	hour += Server_GetTimeAdd();

	// hour += TIME_ZONE; // TIME_ZONE = 0
	if(hour > 24) hour -= 24;
	if(hour < 0) hour += 24;
	//format(string,25,"%02d:%02d:%02d",hour, minute, second);
    //SetClockTextDrawString(string);
    SetWorldTime(hour);
    return 1;
}

ptask UpdatePlayerClockAndTime[1000 * 60](playerid) 
{
    UpdatePlayerTime(playerid);
}

static UpdatePlayerTime(playerid)
{
    new tmphour, tmpminute, tmpsecond;
    gettime(tmphour, tmpminute, tmpsecond);
    
    tmphour += Server_GetTimeAdd();
    
    FixHour(tmphour);

    SetPlayerTime(playerid, tmphour, tmpminute);
}

static stock UpdateServerTime(playerid)
{
    new tmphour, tmpminute, tmpsecond;
    gettime(tmphour, tmpminute, tmpsecond);
    
    tmphour += Server_GetTimeAdd();
    
    FixHour(tmphour);

    SetWorldTime(tmphour);

    printf("UpdateServerTime");
}