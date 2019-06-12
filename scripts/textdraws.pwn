#include <YSI_Coding\y_hooks>

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
}