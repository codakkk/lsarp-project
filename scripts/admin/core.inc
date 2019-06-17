#include <YSI_Coding\y_hooks>

hook OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ)
{
    if(AccountInfo[playerid][aAdmin] <= 0 || !pAdminDuty[playerid])
	   return 0;
    SetPlayerPos(playerid, fX, fY, fZ);
    return 1;
}

stock GetAdminLevelName(level)
{
    new l[16];
    switch(level)
    {
	   case 1: l = "Supporter";
	   case 2: l = "Junior Mod";
	   case 3: l = "Moderatore";
	   case 4: l = "Amministratore"; // del condominio
	   case 5: l = "Sviluppatore";
	   default: l = "Unknown";
    }
    return l; //unreachable
}

stock SendMessageToAdminsStr(forceAlert, color, String:string)
{
	new ptr[1][] = {{}}, size = str_len(string) + 1, Var:var = amx_alloc(size);
    amx_to_ref(var, ptr);
    str_get(string, ptr[0], .size=size);

    new result = SendMessageToAdmins(forceAlert, color, ptr[0]);

    amx_free(var);
    amx_delete(var);
	return result;
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
		  if(!Character_IsLogged(i) || AccountInfo[i][aAdmin] < 2 || (pDisableAdminAlerts[i] && !forceAlert))
			 continue;
		  SendTwoLinesMessage(i, color, string);
	   }
	   return 1;
    }
    
    foreach(new i : Player)
    {
	   if(!Character_IsLogged(i) || AccountInfo[i][aAdmin] < 2 || (pDisableAdminAlerts[i] && !forceAlert))
		  continue;
	   SendTwoLinesMessage(i, color, str);
    }
    return 1;
}