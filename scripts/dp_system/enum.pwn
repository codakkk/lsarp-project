
#define MAX_DPS_PER_PLAYER 		(5)

enum E_DP_DATA
{
	dpExists,
	Float:dpX,
	Float:dpY,
	Float:dpZ,
	Text3D:dpText3D,
	Timer:dpTimer
};
new DPInfo[MAX_PLAYERS][MAX_DPS_PER_PLAYER][E_DP_DATA];