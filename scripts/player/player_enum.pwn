#include <YSI/y_hooks>

enum E_PLAYER_DATA
{
    pID,
    pName[MAX_PLAYER_NAME],
    pMoney,
    pLevel,
    pExp,
    pAge,
    pSex,
    pSkin,
    pPayDay,
    Float:pHealth,
    Float:pArmour,
    pBuildingKey,
};
new 
    PlayerInfo[MAX_PLAYERS][E_PLAYER_DATA],
    pToggleOOCAll[MAX_PLAYERS],
    pTogglePMAll[MAX_PLAYERS],
    // probably Iterators aren't worth for this.
    Iterator:pTogglePM[MAX_PLAYERS]<MAX_PLAYERS>,
    Iterator:pToggleOOC[MAX_PLAYERS]<MAX_PLAYERS>
    ;



enum E_PLAYER_RESTORE_DATA
{
    pSpawned,
    pFirstSpawn,
    Float:pLastX,
    Float:pLastY,
    Float:pLastZ,
    Float:pLastAngle,
    Float:pLastHealth,
    Float:pLastArmour,
    pLastInterior,
    pLastVirtualWorld
};
new PlayerRestore[MAX_PLAYERS][E_PLAYER_RESTORE_DATA];