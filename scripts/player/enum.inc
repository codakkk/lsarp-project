#include <YSI_Coding\y_hooks>

#define MAX_VEHICLES_PER_PLAYER			20
#define MAX_DAMAGES_PER_PLAYER			(24)
#define PENDING_REQUEST_TIME			1

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

	pFaction,
	pRank,

	pPayCheck,
	pJailTime,
	pJailIC,

	pFightStyle,
	pChatStyle,
	pWalkingStyle,
	
	pBanned,
	pBanExpiry,

	pLastChopShopTime,

    pLootZone, // MUST REMOVE FROM HERE SOON
};
new stock
	PlayerInfo[MAX_PLAYERS][E_PLAYER_DATA],
	// probably Iterators aren't worth for this.
	Iterator:pTogglePM[MAX_PLAYERS]<MAX_PLAYERS>,
	Iterator:pToggleOOC[MAX_PLAYERS]<MAX_PLAYERS>,
	pTempSkin[MAX_PLAYERS],
	pSelectedUniformSlot[MAX_PLAYERS char],
	pLastPMTime[MAX_PLAYERS], 
	pTempWeapons[MAX_PLAYERS][13],
	pTempAmmo[MAX_PLAYERS][13]
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
    pLastVirtualWorld,
};
new PlayerRestore[MAX_PLAYERS][E_PLAYER_RESTORE_DATA];

enum e_Bit1_Data 
{
	e_pCharacterLogged,
    e_pFreezed,
	e_pToggleFactionOOC,
	e_pFactionDuty,
	e_pSelectingUniform,
	e_pCuffed,
	e_pDragged,
	e_pDragging,
	e_pMasked,
	e_pLegHit
};

new 
    BitArray:gCharacterBitState[e_Bit1_Data]<MAX_PLAYERS>;

enum // e_DeathStates
{
	DEATH_STATE_NONE = 0,
	DEATH_STATE_INJURED = 1,
	DEATH_STATE_DEAD = 2
}

enum _:e_PendingType
{
	REQUEST_TYPE_NONE,
	REQUEST_TYPE_WEAPON,
	REQUEST_TYPE_ITEM
}