#include <YSI_Coding\y_hooks>
ptask SavePlayerTimer[5000](playerid)
{
    if(!gAccountLogged[playerid] || !Character_IsLogged(playerid))
	   return 0;
    Character_Save(playerid);
    return 1;
}