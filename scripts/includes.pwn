#include <a_samp>
native IsValidVehicle(vehicleid);

#define CGEN_MEMORY 20000

//#define DEBUG 0

#define E_MAIL_CHECK

#undef MAX_PLAYERS
#define MAX_PLAYERS	(200)

#if defined CRASHDETECT
	#include <crashdetect>
#endif

#define FIXES_Single
// #define FIX_OnDialogResponse 1
#define FIX_SetPlayerName 0
#define FIX_ServerVarMsg 0
#include <fixes>

// Includes
#include <sscanf2>
#include <a_mysql>
#include <YSI_Coding\y_timers>
#include <YSI_Coding\y_va>
#include <YSI_Coding\y_inline>
// For YSI and PawnPlus yield conflict.
#undef yield
#undef @@
#include <Pawn.CMD>
#include <whirlpool>
#include <streamer>
#include <strlib>
#include <YSI_Data\y_bit>

#define PP_SYNTAX 1
//#define PP_SYNTAX_GENERIC 1
#define PP_ADDITIONAL_TAGS E_ITEM_DATA

#include <PawnPlus>
// #include <pp-mysql> // Must update pp first
#include <OPA>

#include <miscellaneous\pp_wrappers.pwn>
#include <easyDialogs.pwn>

#include <sa_zones>


#include <YSI_Coding\y_hooks> // Needed for NexAC

#include <nex-ac_it.lang>
#include <nex-ac>

#define AC_GetPlayerHealth AntiCheatGetHealth
#define AC_SetPlayerHealth SetPlayerHealth

#define AC_GetPlayerArmour AntiCheatGetArmour
#define AC_SetPlayerArmour SetPlayerArmour

#define AC_GetPlayerWeapon AntiCheatGetWeapon
#define AC_GivePlayerWeapon GivePlayerWeapon
#define AC_GetPlayerAmmo GetPlayerAmmo

#define AC_GetPlayerWeaponData AntiCheatGetWeaponData
