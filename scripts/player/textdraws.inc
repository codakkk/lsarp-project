stock LoadPlayerTextDraws(playerid)
{
	pInfoText[playerid] = CreatePlayerTextDraw(playerid, 23.000000, 180.000000, "Testo");
	PlayerTextDrawUseBox(playerid, pInfoText[playerid],1);
	PlayerTextDrawBoxColor(playerid, pInfoText[playerid],0x00000033);
	PlayerTextDrawTextSize(playerid, pInfoText[playerid],180.000000, 5.000000);
	PlayerTextDrawAlignment(playerid, pInfoText[playerid],0);
	PlayerTextDrawBackgroundColor(playerid, pInfoText[playerid],0x000000ff);
	PlayerTextDrawFont(playerid, pInfoText[playerid],2);
	PlayerTextDrawLetterSize(playerid, pInfoText[playerid],0.250000, 1.099999);
	PlayerTextDrawColor(playerid, pInfoText[playerid],0xffffffff);
	PlayerTextDrawSetOutline(playerid, pInfoText[playerid],1);
	PlayerTextDrawSetProportional(playerid, pInfoText[playerid],1);
	PlayerTextDrawSetShadow(playerid, pInfoText[playerid],1);

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

 	pJailTimeText[playerid] = CreatePlayerTextDraw(playerid, 499.000000, 101.000000, "~n~~n~~g~TEMPO RIMANENTE:~w~~n~ 0 secondi");
	PlayerTextDrawBackgroundColor(playerid, pJailTimeText[playerid], 255);
	PlayerTextDrawFont(playerid, pJailTimeText[playerid], 1);
	PlayerTextDrawLetterSize(playerid, pJailTimeText[playerid], 0.270000, 1.000000);
	PlayerTextDrawColor(playerid, pJailTimeText[playerid], -1);
	PlayerTextDrawSetOutline(playerid, pJailTimeText[playerid], 1);
	PlayerTextDrawSetProportional(playerid, pJailTimeText[playerid], 1);
	PlayerTextDrawSetSelectable(playerid, pJailTimeText[playerid], 0);

	pDeathTextDraw[playerid] = CreatePlayerTextDraw(playerid, 320.000000, 300.000000, "~n~~n~~g~TEMPO RIMANENTE:~w~~n~ 0 secondi");
	PlayerTextDrawBackgroundColor(playerid, pDeathTextDraw[playerid], 255);
	PlayerTextDrawAlignment(playerid, pDeathTextDraw[playerid], 2); // 2 Means centered.
	PlayerTextDrawFont(playerid, pDeathTextDraw[playerid], 1);
	PlayerTextDrawLetterSize(playerid, pDeathTextDraw[playerid], 0.400000, 1.600000);
	PlayerTextDrawColor(playerid, pDeathTextDraw[playerid], -1);
	PlayerTextDrawSetOutline(playerid, pDeathTextDraw[playerid], 1);
	PlayerTextDrawSetProportional(playerid, pDeathTextDraw[playerid], 1);
	PlayerTextDrawSetSelectable(playerid, pDeathTextDraw[playerid], 0);
}