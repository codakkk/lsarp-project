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
    Float:pArmour 
};
new PlayerInfo[MAX_PLAYERS][E_PLAYER_DATA];

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