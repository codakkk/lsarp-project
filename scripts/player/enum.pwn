#include <YSI_Coding\y_hooks>

#define MAX_VEHICLES_PER_PLAYER     5

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
    pHouseKey,

    pLootZone, // MUST REMOVE FROM HERE SOON
};
new 
    PlayerInfo[MAX_PLAYERS][E_PLAYER_DATA],
    // probably Iterators aren't worth for this.
    Iterator:pTogglePM[MAX_PLAYERS]<MAX_PLAYERS>,
    Iterator:pToggleOOC[MAX_PLAYERS]<MAX_PLAYERS>,
    pVehicleListItem[MAX_PLAYERS][MAX_VEHICLES_PER_PLAYER],
    pSelectedVehicleListItem[MAX_PLAYERS]
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

enum e_Bit1_Data 
{
    e_pTogglePMAll,
    e_pToggleOOCAll,
    e_pHotKeys,
    e_pFreezed,
	e_pInvMode, // 0: Dialog - 1: Chat
};

new 
    BitArray:gPlayerBitArray[e_Bit1_Data]<MAX_PLAYERS>;