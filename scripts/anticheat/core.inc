#include <YSI_Coding\y_hooks> // Needed for NexAC

#define DEBUG 0

#include <nex-ac_it.lang>

#define AC_USE_VENDING_MACHINES		false
#define AC_USE_TUNING_GARAGES		false
#define AC_USE_PICKUP_WEAPONS		false
#define AC_USE_AMMUNATIONS			false
#define AC_USE_RESTAURANTS			false
#define AC_USE_PAYNSPRAY			true
#define AC_USE_CASINOS				false
#define AC_USE_NPC					false
#include <nex-ac>

#define AC_GetPlayerHealth AntiCheatGetHealth
#define AC_SetPlayerHealth SetPlayerHealth

#define AC_GetPlayerArmour AntiCheatGetArmour
#define AC_SetPlayerArmour SetPlayerArmour