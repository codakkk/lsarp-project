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
new AccountInfo[MAX_PLAYERS][E_ACCOUNT_DATA];