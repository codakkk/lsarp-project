
#include <YSI_Coding\y_hooks>

new
	PlayerText:pMoneyTD[MAX_PLAYERS],
	PlayerText:JailTimePlayerText[MAX_PLAYERS],
	PlayerText:pDeathTextDraw[MAX_PLAYERS]
;
static 
	PlayerText:pVehicleFuelText[MAX_PLAYERS],
	PlayerText:PlayerInfoText[MAX_PLAYERS], bool:InfoBoxShown[MAX_PLAYERS char]
;

hook OnPlayerConnect(playerid)
{
	PlayerInfoText[playerid] = CreatePlayerTextDraw(playerid, 23.000000, 180.000000, "Testo");
	PlayerTextDrawUseBox(playerid, PlayerInfoText[playerid],1);
	PlayerTextDrawBoxColor(playerid, PlayerInfoText[playerid],0x00000033);
	PlayerTextDrawTextSize(playerid, PlayerInfoText[playerid],180.000000, 5.000000);
	PlayerTextDrawAlignment(playerid, PlayerInfoText[playerid],0);
	PlayerTextDrawBackgroundColor(playerid, PlayerInfoText[playerid],0x000000ff);
	PlayerTextDrawFont(playerid, PlayerInfoText[playerid],2);
	PlayerTextDrawLetterSize(playerid, PlayerInfoText[playerid],0.250000, 1.099999);
	PlayerTextDrawColor(playerid, PlayerInfoText[playerid],0xffffffff);
	PlayerTextDrawSetOutline(playerid, PlayerInfoText[playerid],1);
	PlayerTextDrawSetProportional(playerid, PlayerInfoText[playerid],1);
	PlayerTextDrawSetShadow(playerid, PlayerInfoText[playerid],1);

	pMoneyTD[playerid] = CreatePlayerTextDraw(playerid, 608.000000, 101.000000, " "); // Money Changes Textdraw
	PlayerTextDrawAlignment(playerid, pMoneyTD[playerid], 3);
	PlayerTextDrawFont(playerid, pMoneyTD[playerid], 3);
	PlayerTextDrawLetterSize(playerid, pMoneyTD[playerid], 0.519999, 2.100000);
	PlayerTextDrawColor(playerid, pMoneyTD[playerid], 0xFFFFFFFF);
	PlayerTextDrawSetOutline(playerid, pMoneyTD[playerid], 1);

	pVehicleFuelText[playerid] = CreatePlayerTextDraw(playerid, 578.000000, 392.000000, "~y~000%");
	PlayerTextDrawBackgroundColor(playerid, pVehicleFuelText[playerid], 255);
	PlayerTextDrawFont(playerid, pVehicleFuelText[playerid], 2);
	PlayerTextDrawLetterSize(playerid, pVehicleFuelText[playerid], 0.200000, 1.190000);
	PlayerTextDrawColor(playerid, pVehicleFuelText[playerid], 16711935);
	PlayerTextDrawSetOutline(playerid, pVehicleFuelText[playerid], 1);
	PlayerTextDrawSetProportional(playerid, pVehicleFuelText[playerid], 1);
	PlayerTextDrawSetSelectable(playerid, pVehicleFuelText[playerid], 0);

 	JailTimePlayerText[playerid] = CreatePlayerTextDraw(playerid, 499.000000, 101.000000, "~n~~n~~g~TEMPO RIMANENTE:~w~~n~ 0 secondi");
	PlayerTextDrawBackgroundColor(playerid, JailTimePlayerText[playerid], 255);
	PlayerTextDrawFont(playerid, JailTimePlayerText[playerid], 1);
	PlayerTextDrawLetterSize(playerid, JailTimePlayerText[playerid], 0.270000, 1.000000);
	PlayerTextDrawColor(playerid, JailTimePlayerText[playerid], -1);
	PlayerTextDrawSetOutline(playerid, JailTimePlayerText[playerid], 1);
	PlayerTextDrawSetProportional(playerid, JailTimePlayerText[playerid], 1);
	PlayerTextDrawSetSelectable(playerid, JailTimePlayerText[playerid], 0);

	pDeathTextDraw[playerid] = CreatePlayerTextDraw(playerid, 320.000000, 380.000000, "~n~~n~~g~TEMPO RIMANENTE:~w~~n~ 0 secondi");
	PlayerTextDrawBackgroundColor(playerid, pDeathTextDraw[playerid], 255);
	PlayerTextDrawAlignment(playerid, pDeathTextDraw[playerid], 2); // 2 Means centered.
	PlayerTextDrawFont(playerid, pDeathTextDraw[playerid], 1);
	PlayerTextDrawLetterSize(playerid, pDeathTextDraw[playerid], 0.400000, 1.600000);
	PlayerTextDrawColor(playerid, pDeathTextDraw[playerid], -1);
	PlayerTextDrawSetOutline(playerid, pDeathTextDraw[playerid], 1);
	PlayerTextDrawSetProportional(playerid, pDeathTextDraw[playerid], 1);
	PlayerTextDrawSetSelectable(playerid, pDeathTextDraw[playerid], 0);
	
	return 1;
}

stock TextDraw_ShowFuel(playerid, bool:show)
{
	if(show && Player_HasHUDEnabled(playerid))
		PlayerTextDrawShow(playerid, pVehicleFuelText[playerid]);
	else
		PlayerTextDrawHide(playerid, pVehicleFuelText[playerid]);
	return 1;
}

stock TextDraw_SetFuelText(playerid, text[])
{
	PlayerTextDrawSetString(playerid, pVehicleFuelText[playerid], text);
	return 1;
}

stock Player_Info(playerid, text[], bool:forced = false, time = 2500)
{
	if(InfoBoxShown{playerid} == true && !forced)
		return 1;

	static Timer:InfoTimer[MAX_PLAYERS] = {Timer:0, ...};

	if(InfoTimer[playerid] != Timer:0)
		stop InfoTimer[playerid];

	//PlayerTextDrawHide(playerid, PlayerInfoText[playerid]);
	PlayerTextDrawSetString(playerid, PlayerInfoText[playerid], text);
	PlayerTextDrawShow(playerid, PlayerInfoText[playerid]);

	InfoBoxShown{playerid} = true;

	InfoTimer[playerid] = defer DeleteInfoBox(playerid, time);
	return 1;
}

stock Player_InfoStr(playerid, String:string, bool:forced = false, time = 2500)
{
	/*new ptr[1][] = {{}}, size = str_len(string) + 1, Var:var = amx_alloc(size);
	amx_to_ref(var, ptr);
	str_get(string, ptr[0], .size=size);*/

	static buffer[256];
	str_get(string, buffer);
	new result = Player_Info(playerid, /*ptr[0]*/ buffer, forced, time);

	/*amx_free(var);
	amx_delete(var);*/
	return result;
}

timer DeleteInfoBox[time](playerid, time)
{
	#pragma unused time
	PlayerTextDrawHide(playerid, PlayerInfoText[playerid]);

	InfoBoxShown{playerid} = false;

	return 1;
}