#include <YSI_Coding\y_hooks>

static 
	Text:txtAnimHelper, 
	Text:Clock, 
	Text:LoginScreenText
;

hook OnGameModeInit()
{
	LoadTextDraws();
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnGameModeExit()
{
	TextDrawDestroy(txtAnimHelper);
	TextDrawDestroy(Clock);
	return Y_HOOKS_CONTINUE_RETURN_1;
}
new PlayerText:HealthAsNumericTextDraw[MAX_PLAYERS];
hook OnPlayerConnect(playerid)
{
	HealthAsNumericTextDraw[playerid] = CreatePlayerTextDraw(playerid, 578.333312, 68.029632, "100.0");
	PlayerTextDrawLetterSize(playerid, HealthAsNumericTextDraw[playerid], 0.206999, 0.594962);
	PlayerTextDrawTextSize(playerid, HealthAsNumericTextDraw[playerid], -103.666648, 36.918521);
	PlayerTextDrawAlignment(playerid, HealthAsNumericTextDraw[playerid], 2);
	PlayerTextDrawColor(playerid, HealthAsNumericTextDraw[playerid], -1);
	PlayerTextDrawSetShadow(playerid, HealthAsNumericTextDraw[playerid], 0);
	PlayerTextDrawSetOutline(playerid, HealthAsNumericTextDraw[playerid], 1);
	PlayerTextDrawBackgroundColor(playerid, HealthAsNumericTextDraw[playerid], 51);
	PlayerTextDrawFont(playerid, HealthAsNumericTextDraw[playerid], 1);
	PlayerTextDrawSetProportional(playerid, HealthAsNumericTextDraw[playerid], 1);

	TextDrawShowForPlayer(playerid, LoginScreenText);
	return 1;
}

hook OnCharacterLoad(playerid)
{
	TextDrawHideForPlayer(playerid, LoginScreenText);
	return 1;
}

stock LoadTextDraws()
{
	txtAnimHelper = TextDrawCreate(610.0, 400.0, "~w~Usa ~b~~k~~CONVERSATION_NO~ ~w~per fermare l'animazione");
    TextDrawUseBox(txtAnimHelper, 0);
    TextDrawFont(txtAnimHelper, 2);
    TextDrawSetShadow(txtAnimHelper,0);
	TextDrawSetOutline(txtAnimHelper,1);
	TextDrawBackgroundColor(txtAnimHelper,0x000000FF);
	TextDrawColor(txtAnimHelper,0xFFFFFFFF);
	TextDrawAlignment(txtAnimHelper,3);

	Clock = TextDrawCreate(546.000000, 22.000000, "00:00:00");
	TextDrawBackgroundColor(Clock, 255);
	TextDrawFont(Clock, 3);
	TextDrawLetterSize(Clock, 0.370000, 2.000000);
	TextDrawColor(Clock, -17);
	TextDrawSetOutline(Clock, 1);
	TextDrawSetProportional(Clock, 0);

	LoginScreenText = TextDrawCreate(0.0, 0.0, "mdl-2001:login_image");
	TextDrawAlignment(LoginScreenText, 2);
	TextDrawBackgroundColor(LoginScreenText, 255);
    TextDrawFont(LoginScreenText, 4);
	TextDrawLetterSize(LoginScreenText, 0.500000, 1.000000);
	TextDrawColor(LoginScreenText, -1);
	TextDrawSetOutline(LoginScreenText, 0);
	TextDrawSetProportional(LoginScreenText, 1);
	TextDrawSetShadow(LoginScreenText, 1);
    TextDrawUseBox(LoginScreenText, 1);
    TextDrawBoxColor(LoginScreenText, 255);
	TextDrawTextSize(LoginScreenText, 640.0, 448.0);
}

stock ShowTextAnimationHelper(playerid)
{
	return TextDrawShowForPlayer(playerid, txtAnimHelper);
}

stock HideTextAnimationHelper(playerid)
{
	return TextDrawHideForPlayer(playerid, txtAnimHelper);
}

stock ShowClockTextDraw(playerid)
{
	return TextDrawShowForPlayer(playerid, Clock);
}

stock HideClockTextDraw(playerid)
{
	return TextDrawHideForPlayer(playerid, Clock);
}

stock SetClockTextDrawString(string[])
{
	return TextDrawSetString(Clock, string);
}

hook function SetHealthBarVisible(playerid, bool:toggle)
{
    if(toggle)
        PlayerTextDrawShow(playerid, HealthAsNumericTextDraw[playerid]);
    else
        PlayerTextDrawHide(playerid, HealthAsNumericTextDraw[playerid]);
    return 1;
}

hook function UpdateHealthBar(playerid, bool:force = true)
{
    PlayerTextDrawSetStringStr(playerid, HealthAsNumericTextDraw[playerid], str_format("%0.2f", Character_GetHealth(playerid)));
	return continue(playerid, force);
}