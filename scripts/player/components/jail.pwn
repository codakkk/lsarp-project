#include <YSI_Coding\y_hooks>

hook GlobalPlayerSecondTimer(playerid)
{
	if(Character_IsJailed(playerid))
	{
		PlayerInfo[playerid][pJailTime]--;
		PlayerTextDrawSetStringStr(playerid, pJailTimeText[playerid], str_format("~n~~n~~g~TEMPO RIMANENTE: ~w~~n~%d secondi", Character_GetJailTime(playerid)));
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
				Character_Spawn(playerid);
			}
			PlayerTextDrawHide(playerid, pJailTimeText[playerid]);
			Character_Save(playerid);
		}
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}