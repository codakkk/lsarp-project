#include <admin_system\components\recon>

#include <admin_system\commands>
#include <admin_system\supporter_commands>

#include <YSI_Coding\y_hooks>


hook OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ)
{
    if(Account_GetAdminLevel(playerid) <= 0 || !Player_IsAdminDuty(playerid))
	   return 0;
    Character_SetSafePosition(playerid, fX, fY, fZ);
    return 1;
}

stock GetAdminLevelName(level)
{
    new l[16];
    switch(level)
    {
	   case 1: l = "Tester";
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
	/*new ptr[1][] = {{}}, size = str_len(string) + 1, Var:var = amx_alloc(size);
    amx_to_ref(var, ptr);
    str_get(string, ptr[0], .size=size);*/

	static str[256];
	str_get(string, str);

    new result = SendMessageToAdmins(forceAlert, color, str);

    /*amx_free(var);
    amx_delete(var);*/
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
		  if(!Character_IsLogged(i) || Account_GetAdminLevel(i) < 2 || (Player_IsDisabledAdminAlerts(i) && !forceAlert))
			 continue;
		  SendTwoLinesMessage(i, color, string);
	   }
	   return 1;
    }
    
    foreach(new i : Player)
    {
	   if(!Character_IsLogged(i) || Account_GetAdminLevel(i) < 2 || (Player_IsDisabledAdminAlerts(i) && !forceAlert))
		  continue;
	   SendTwoLinesMessage(i, color, str);
    }
    return 1;
}

stock SendMessageToTesters(forceAlert, color, str[], {Float,_}:...)
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
		  if(!Character_IsLogged(i) || Account_GetAdminLevel(i) < 1 || (Player_IsDisabledAdminAlerts(i) && !forceAlert))
			 continue;
		  SendTwoLinesMessage(i, color, string);
	   }
	   return 1;
    }
    
    foreach(new i : Player)
    {
	   if(!Character_IsLogged(i) || Account_GetAdminLevel(i) < 1 || (Player_IsDisabledAdminAlerts(i) && !forceAlert))
		  continue;
	   SendTwoLinesMessage(i, color, str);
    }
    return 1;
}

stock SendMessageToTestersStr(forceAlert, color, String:string)
{
	/*new ptr[1][] = {{}}, size = str_len(string) + 1, Var:var = amx_alloc(size);
    amx_to_ref(var, ptr);
    str_get(string, ptr[0], .size=size);*/

	static str[256];
	str_get(string, str);
    new result = SendMessageToTesters(forceAlert, color, str);

    /*amx_free(var);
    amx_delete(var);*/
	return result;
}