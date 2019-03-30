#include <YSI/y_hooks>

#define MAX_CHARACTERS      (5)

enum E_ACCOUNT_DATA
{
    aID,
    aName[MAX_PLAYER_NAME],
    aAdmin,
    aPremium,
    aPremiumExpiry,
    aFirstCharacter,
    aCharacters[MAX_CHARACTERS],
    aCharactersCount
};
new 
    AccountInfo[MAX_PLAYERS][E_ACCOUNT_DATA],
    gCharacterCreationStep[MAX_PLAYERS],
    gCharacterSelected[MAX_PLAYERS],
    gCharacterSelectedName[MAX_PLAYERS][MAX_PLAYER_NAME]
    ;

// Character Creation Steps:
// 0: Name & Surname
// 1: Age Selection
// 2: Sex Selection
// 3: Over