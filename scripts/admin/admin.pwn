#include <YSI\y_hooks>

#include <admin/admin_commands.pwn>
#include <admin/supporter_commands.pwn>

hook OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ)
{
    if(AccountInfo[playerid][aAdmin] <= 0)
        return 0;
    SetPlayerPos(playerid, fX, fY, fZ);
    return 1;
}

hook OnPlayerExitVehicle(playerid, vehicleid)
{
    if(gAdminVehicle[playerid] == vehicleid)
    {
        DestroyVehicle(gAdminVehicle[playerid]);
        gAdminVehicle[playerid] = 0;
    }
}

stock GetAdminLevelName(level)
{
    new l[16];
    switch(level)
    {
        case 1: l = "Supporter";
        case 2: l = "Junior Mod.";
        case 3: l = "Moderatore";
        case 4: l = "Amministratore"; // del condominio
        case 5: l = "Sviluppatore";
        default: l = "Unknown";
    }
    return l; //unreachable
}

stock SendMessageToAdmins(forceAlert, color, str[], {Float,_}:...)
{
    static
	    args,
	    start,
	    end,
	    string[256];

	#emit LOAD.S.pri 8
	#emit STOR.pri args

	if(args > 8)
	{
		#emit ADDR.pri str
		#emit STOR.pri start

	    for(end = start + (args - 8); end > start; end -= 4)
		{
	        #emit LREF.pri end
	        #emit PUSH.pri
		}

		#emit PUSH.S str
		#emit PUSH.C 256
		#emit PUSH.C string

		#emit LOAD.S.pri 8
		#emit ADD.C 4
		#emit PUSH.pri

		#emit SYSREQ.C format
		#emit LCTRL 5
		#emit SCTRL 4

        foreach(new i : Player)
        {
            if(!gAccountLogged[i] || !gCharacterLogged[i] || AccountInfo[i][aAdmin] < 2 || (pDisableAdminAlerts[i] && !forceAlert))
                continue;
            SendClientMessage(i, color, string);
        }
        return 1;
    }
    
    foreach(new i : Player)
    {
        if(!gAccountLogged[i] || !gCharacterLogged[i] || AccountInfo[i][aAdmin] < 2 || (pDisableAdminAlerts[i] && !forceAlert))
            continue;
        SendClientMessage(i, color, str);
    }
    return 1;
}