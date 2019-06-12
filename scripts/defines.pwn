#include <utils\colors.pwn>

#define Inventory List@Inventory

#define INFINITY (Float:0x7F800000)

#define PRESSED(%0) \
	(((newkeys & (%0)) == (%0)) && ((oldkeys & (%0)) != (%0)))

enum (<<= 1)
{
	CMD_USER = 1,
	CMD_ALIVE_USER,
	CMD_PREMIUM_BRONZE,
	CMD_PREMIUM_SILVER,
	CMD_PREMIUM_GOLD,
	CMD_POLICE,
	CMD_MEDICAL,
	CMD_GOVERNMENT,
	CMD_ILLEGAL,
	CMD_SUPPORTER,
	CMD_JR_MODERATOR,
	CMD_MODERATOR,
	CMD_ADMIN,
	CMD_DEVELOPER,
	CMD_RCON
}

#define IC_JAIL_X						(2638.6926)
#define IC_JAIL_Y						(-1995.0134)
#define IC_JAIL_Z 						(-59.7283)
#define IC_JAIL_INT						(0)

#define OOC_JAIL_X						(197.1209)
#define OOC_JAIL_Y						(176.3204)
#define OOC_JAIL_Z 						(1003.0234)
#define OOC_JAIL_INT					(3)
