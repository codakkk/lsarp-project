#include <YSI_Coding\y_hooks>

#define MIN_CHARACTERS		(3)
#define MAX_CHARACTERS	 	(10)
#define MAX_LOGIN_ATTEMPTS	(3)

enum E_ACCOUNT_DATA
{
	aID,
	aName[MAX_PLAYER_NAME],
	aEMail[255],
	aAdmin,
	aZPoints,
	aPremium,
	aPremiumExpiry,
	aFirstCharacter,
	aCharacters[MAX_CHARACTERS],
	aCharactersSlot,
	aCharactersCount,
	aBanned,
	aBanExpiry
};
new 
	AccountInfo[MAX_PLAYERS][E_ACCOUNT_DATA],
	gAccountTempMail[MAX_PLAYERS][255],
	gAccountEmailCode[MAX_PLAYERS],
	gCharacterCreationStep[MAX_PLAYERS char],
	gCharacterSelected[MAX_PLAYERS char],
	gCharacterSelectedName[MAX_PLAYERS][MAX_PLAYER_NAME]
;

enum e_RewardData
{
	rdPoints,
	rdName[32]
};
new const RewardInfo[][e_RewardData] =
{
	{250, "Cambio Nome"},
	{300, "Block PM"},
	{300, "Block OOC"},
	{500, "Premium Bronze (1 mese)"},
	{750, "Premium Silver (1 mese)"},
	{1000, "Premium Gold (1 mese)"},
	{500, "+1 Slot Personaggio"},
	{200, "+1 Livello"}
};

enum E_PLAYER_REWARD_DATA
{
	rChangeName,
	rBlockPM,
	rBlockOOC
};
new PlayerRewardData[MAX_PLAYERS][E_PLAYER_REWARD_DATA];


// Character Creation Steps:
// 0: Name & Surname
// 1: Age Selection
// 2: Sex Selection
// 3: Over

enum e_Bit1_AccountData 
{
	e_pAccountLogged,
    e_pTogglePMAll,
    e_pToggleOOCAll,
    e_pHotKeys,
	e_pInvMode, // 0: Dialog - 1: Chat
	e_pToggleHUD,
	e_pAdminDuty,
	e_pSupporterDuty
};

new 
    BitArray:gAccountBitState[e_Bit1_AccountData]<MAX_PLAYERS>;