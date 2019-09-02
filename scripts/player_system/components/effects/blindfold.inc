
#include <YSI_Coding\y_hooks>

static 
    BlindFold[MAX_PLAYERS char] = {false, ...},
    Text:BlindFoldText;

hook OnGameModeInit()
{
    BlindFoldText = TextDrawCreate(0.0, 0.0, "_");
    TextDrawFont(BlindFoldText, 1);
    TextDrawLetterSize(BlindFoldText, 0.0, 50.0);
    TextDrawUseBox(BlindFoldText, true);
    TextDrawColor(BlindFoldText, 0);
    TextDrawBoxColor(BlindFoldText, 0x000000FF);
    return 1;
}

hook OnPlayerClearData(playerid)
{
    BlindFold{playerid} = false;
    TextDrawHideForPlayer(playerid, BlindFoldText);
    return 1;
}

flags:blindfold(CMD_USER);
CMD:blindfold(playerid, params[])
{
    BlindFold{playerid} = !BlindFold{playerid};
    if(BlindFold{playerid})
        TextDrawShowForPlayer(playerid, BlindFoldText);
    else
        TextDrawHideForPlayer(playerid, BlindFoldText);
    return 1;
}