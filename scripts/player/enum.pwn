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
	pVehicleListItem[MAX_PLAYERS][MAX_VEHICLES_PER_PLAYER],
	pSelectedVehicleListItem[MAX_PLAYERS],
	pAmmoSync[MAX_PLAYERS char],
	pTempSkin[MAX_PLAYERS],
	pSelectedUniformSlot[MAX_PLAYERS char],
	pDraggedBy[MAX_PLAYERS],
	Timer:pDragTimer[MAX_PLAYERS],
	Timer:pChatTimer[MAX_PLAYERS],
	Timer:pWalkTimer[MAX_PLAYERS],
	pLastPMTime[MAX_PLAYERS],
	pCare[MAX_PLAYERS],
	pInCare[MAX_PLAYERS char],
	pCareTime[MAX_PLAYERS],
	pAFKTime[MAX_PLAYERS]
;

// Text3D
new stock
	Text3D:pMaskText3D[MAX_PLAYERS]
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
	e_pMasked
};

new 
    BitArray:gCharacterBitState[e_Bit1_Data]<MAX_PLAYERS>;

enum _:e_PendingType
{
	REQUEST_TYPE_NONE,
	REQUEST_TYPE_WEAPON,
	REQUEST_TYPE_ITEM
}

enum e_RequestData
{
	rdPending,
	rdByPlayer,
	rdToPlayer,
	rdTime,
	rdItem,
	rdAmount,
	rdExtra,
	rdType,
	rdSlot,
};

new 
	PendingRequestInfo[MAX_PLAYERS][e_RequestData]
	;


stock ResetPendingRequest(playerid)
{
	PendingRequestInfo[playerid][rdPending] = 0;
	PendingRequestInfo[playerid][rdByPlayer] = -1;
	PendingRequestInfo[playerid][rdTime] = 0;
	PendingRequestInfo[playerid][rdItem] = 0;
	PendingRequestInfo[playerid][rdAmount] = 0;
	PendingRequestInfo[playerid][rdExtra] = 0;
	PendingRequestInfo[playerid][rdType] = REQUEST_TYPE_NONE;
	PendingRequestInfo[playerid][rdSlot] = 0;
}

enum // e_DeathStates
{
	DEATH_STATE_NONE = 0,
	DEATH_STATE_INJURED = 1,
	DEATH_STATE_DEAD = 2
}

enum e_DeathState
{
	pDeathTime,
	Float:pDeathX,
	Float:pDeathY,
	Float:pDeathZ,
	Float:pDeathA,
	pDeathInt,
	pDeathWorld,
	pDeathKiller,
	Text3D:pDeathText
};

new 
	PlayerDeathState[MAX_PLAYERS][e_DeathState],
	pDeathState[MAX_PLAYERS char] // 0 = Not dead, 1 = Waiting for healing, 2 = Death
;

enum e_CharacterDamageInfo
{
	damageUsed,
	damageIssuerName[MAX_PLAYER_NAME],
	damageWeapon,
	Float:damageAmount,
	damageBodyPart,
};
new 
	CharacterDamageInfo[MAX_PLAYERS][MAX_DAMAGES_PER_PLAYER][e_CharacterDamageInfo];