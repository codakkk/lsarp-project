#include <a_samp>
#include <memory.inc>

new 
    PlayerText:CharacterModelTextdraw[MAX_PLAYERS][9],
    PlayerText:CharacterNameTextdraw[MAX_PLAYERS][9],
    PlayerText:CharacterLevelTextdraw[MAX_PLAYERS][9];

// Should I use a Map instead of preloaded arrays?

enum _:E_TEST
{
    A,
    B,
    C,
    D
}

main() 
{
    new Pointer:test = MEM_new(E_TEST);
    MEM_set_val(test, A, 1);
    MEM_set_val(test, B, 2);
    MEM_set_val(test, C, 3);
    MEM_set_val(test, D, 4);
    printf("V: %d", E_TEST);
    printf("V: %d", MEM_get_val(test, A));
    printf("V: %d", MEM_get_val(test, B));
    printf("V: %d", MEM_get_val(test, C));
    printf("V: %d", MEM_get_val(test, D));
}

public OnGameModeInit()
{
    return 1;
}

forward Timer(playerid);
public Timer(playerid)
{
    SetPlayerCameraPos(playerid, 2115.4441,2131.3276,32.3443);
    SetPlayerCameraLookAt(playerid, 2174.6531,2141.4639,16.6471);
    return 1;
}

public OnPlayerConnect(playerid)
{
    TogglePlayerSpectating(playerid, true);
    SetTimerEx(#Timer, 100, false, "d", playerid);
    new 
        i = 0,
        Float:startX = 150.0000, 
        Float:startY = 80.0000, 
        Float:gapX = 105.0, 
        Float:gapY = 105.0, 
        Float:textStartX = startX + 2.5,
        Float:textStartY = startY + 2.5;
    for(new y = 0; y < 3; ++y)
    {
        for(new x = 0; x < 3; ++x)
        {
            i = x + y * 3;
            CharacterModelTextdraw[playerid][i] = CreatePlayerTextDraw(playerid, startX + x * gapX, startY + y * gapY, "MODEL");
            PlayerTextDrawFont(playerid, CharacterModelTextdraw[playerid][i], TEXT_DRAW_FONT_MODEL_PREVIEW );
            PlayerTextDrawLetterSize(playerid, CharacterModelTextdraw[playerid][i], 0.5000, 1.0000);
            PlayerTextDrawColor(playerid, CharacterModelTextdraw[playerid][i], -1);
            PlayerTextDrawSetShadow(playerid, CharacterModelTextdraw[playerid][i], 0);
            PlayerTextDrawSetOutline(playerid, CharacterModelTextdraw[playerid][i], 0);
            PlayerTextDrawBackgroundColor(playerid, CharacterModelTextdraw[playerid][i], 0x000000AA);
            PlayerTextDrawSetProportional(playerid, CharacterModelTextdraw[playerid][i], 1);
            PlayerTextDrawTextSize(playerid, CharacterModelTextdraw[playerid][i], 100.0000, 100.0000);
            PlayerTextDrawSetPreviewModel(playerid, CharacterModelTextdraw[playerid][i], 46);
            PlayerTextDrawSetPreviewRot(playerid, CharacterModelTextdraw[playerid][i], 0.0000, 0.0000, 0.0000, 1.0000);
            PlayerTextDrawSetSelectable(playerid, CharacterModelTextdraw[playerid][i], 1);

            CharacterNameTextdraw[playerid][i] = CreatePlayerTextDraw(playerid, textStartX + x * gapX, textStartY + y * gapY, "Peppe Musicco");
            PlayerTextDrawFont(playerid, CharacterNameTextdraw[playerid][i], 1);
            PlayerTextDrawLetterSize(playerid, CharacterNameTextdraw[playerid][i], 0.1399, 0.8999);
            PlayerTextDrawColor(playerid, CharacterNameTextdraw[playerid][i], -1);
            PlayerTextDrawSetShadow(playerid, CharacterNameTextdraw[playerid][i], 0);
            PlayerTextDrawSetOutline(playerid, CharacterNameTextdraw[playerid][i], 0);
            PlayerTextDrawBackgroundColor(playerid, CharacterNameTextdraw[playerid][i], 255);
            PlayerTextDrawSetProportional(playerid, CharacterNameTextdraw[playerid][i], 1);
            PlayerTextDrawUseBox(playerid, CharacterNameTextdraw[playerid][i], 0);
            PlayerTextDrawBoxColor(playerid, CharacterNameTextdraw[playerid][i], 0);
            PlayerTextDrawTextSize(playerid, CharacterNameTextdraw[playerid][i], (201.5000 + (i * gapX)), 0.0000);

            CharacterLevelTextdraw[playerid][i] = CreatePlayerTextDraw(playerid, textStartX + x * gapX, textStartY + y * gapY + 10.0, "Livello 100");
            PlayerTextDrawFont(playerid, CharacterLevelTextdraw[playerid][i], 1);
            PlayerTextDrawLetterSize(playerid, CharacterLevelTextdraw[playerid][i], 0.1399, 0.8999);
            PlayerTextDrawColor(playerid, CharacterLevelTextdraw[playerid][i], -1);
            PlayerTextDrawSetShadow(playerid, CharacterLevelTextdraw[playerid][i], 0);
            PlayerTextDrawSetOutline(playerid, CharacterLevelTextdraw[playerid][i], 0);
            PlayerTextDrawBackgroundColor(playerid, CharacterLevelTextdraw[playerid][i], 255);
            PlayerTextDrawSetProportional(playerid, CharacterLevelTextdraw[playerid][i], 1);
            PlayerTextDrawUseBox(playerid, CharacterLevelTextdraw[playerid][i], 0);
            PlayerTextDrawBoxColor(playerid, CharacterLevelTextdraw[playerid][i], 0);
            PlayerTextDrawTextSize(playerid, CharacterLevelTextdraw[playerid][i], (201.5000 + (i * gapX)), 0.0000);
        }
    }
    return 1;
}
 
public OnPlayerClickPlayerTextDraw(playerid, PlayerText:playertextid)
{
    for(new i = 0; i < 9; ++i)
    {
        if(playertextid == CharacterModelTextdraw[playerid][i])
        {
            printf("You clicked on Peppe %d", i);
            CancelSelectTextDraw(playerid);
            return 1;
        }
    }
    return 0;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
    if(!strcmp(cmdtext, "/s"))
    {
        SelectTextDraw(playerid, 0xFFFFFFAA);
    }
    if(!strcmp(cmdtext, "/a"))
    {
        PlayerTextDrawShow(playerid, CharacterNameTextdraw[playerid][0]);
        PlayerTextDrawShow(playerid, CharacterModelTextdraw[playerid][0]);
        PlayerTextDrawShow(playerid, CharacterLevelTextdraw[playerid][0]);
    }
    if(!strcmp(cmdtext, "/a2"))
    {
        PlayerTextDrawShow(playerid, CharacterNameTextdraw[playerid][1]);
        PlayerTextDrawShow(playerid, CharacterModelTextdraw[playerid][1]);
        PlayerTextDrawShow(playerid, CharacterLevelTextdraw[playerid][1]);
    }
    if(!strcmp(cmdtext, "/a3"))
    {
        PlayerTextDrawShow(playerid, CharacterNameTextdraw[playerid][2]);
        PlayerTextDrawShow(playerid, CharacterModelTextdraw[playerid][2]);
        PlayerTextDrawShow(playerid, CharacterLevelTextdraw[playerid][2]);
    }
    if(!strcmp(cmdtext, "/a4"))
    {
        PlayerTextDrawShow(playerid, CharacterNameTextdraw[playerid][3]);
        PlayerTextDrawShow(playerid, CharacterModelTextdraw[playerid][3]);
        PlayerTextDrawShow(playerid, CharacterLevelTextdraw[playerid][3]);
    }
    if(!strcmp(cmdtext, "/a5"))
    {
        PlayerTextDrawShow(playerid, CharacterNameTextdraw[playerid][4]);
        PlayerTextDrawShow(playerid, CharacterModelTextdraw[playerid][4]);
        PlayerTextDrawShow(playerid, CharacterLevelTextdraw[playerid][4]);
    }
    if(!strcmp(cmdtext, "/a6"))
    {
        PlayerTextDrawShow(playerid, CharacterNameTextdraw[playerid][5]);
        PlayerTextDrawShow(playerid, CharacterModelTextdraw[playerid][5]);
        PlayerTextDrawShow(playerid, CharacterLevelTextdraw[playerid][5]);
    }
    if(!strcmp(cmdtext, "/a7"))
    {
        PlayerTextDrawShow(playerid, CharacterNameTextdraw[playerid][6]);
        PlayerTextDrawShow(playerid, CharacterModelTextdraw[playerid][6]);
        PlayerTextDrawShow(playerid, CharacterLevelTextdraw[playerid][6]);
    }
    if(!strcmp(cmdtext, "/a8"))
    {
        PlayerTextDrawShow(playerid, CharacterNameTextdraw[playerid][7]);
        PlayerTextDrawShow(playerid, CharacterModelTextdraw[playerid][7]);
        PlayerTextDrawShow(playerid, CharacterLevelTextdraw[playerid][7]);
    }
    if(!strcmp(cmdtext, "/a9"))
    {
        PlayerTextDrawShow(playerid, CharacterNameTextdraw[playerid][8]);
        PlayerTextDrawShow(playerid, CharacterModelTextdraw[playerid][8]);
        PlayerTextDrawShow(playerid, CharacterLevelTextdraw[playerid][8]);
    }
    return 1;
}