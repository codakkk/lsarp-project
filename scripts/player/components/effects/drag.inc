
#include <YSI_Coding\y_hooks>

static 
	pDraggedBy[MAX_PLAYERS],
	Timer:pDragTimer[MAX_PLAYERS]
;

hook OnPlayerClearData(playerid)
{
	Character_SetDraggedBy(playerid, INVALID_PLAYER_ID);
	stop pDragTimer[playerid];
    return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnCharacterSpawn(playerid)
{
	stop pDragTimer[playerid];
	Character_SetDragged(playerid, false);
	Character_SetDraggedBy(playerid, INVALID_PLAYER_ID);
	Character_SetDragging(playerid, false);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

timer UpdateDragging[250](playerid, targetid) 
{
	if(Character_IsDragging(playerid))
	{
		new id = Character_GetDraggedBy(targetid);
		if(Character_IsDragged(targetid) && playerid == id)
		{
			new Float:x, Float:y, Float:z, Float:angle;
			GetPlayerPos(playerid, x, y, z);
			GetPlayerFacingAngle(playerid, angle);

			SetPlayerPos(targetid, x+0.2, y+0.2, z);
			SetPlayerInterior(targetid, GetPlayerInterior(playerid));
			Character_SetVirtualWorld(targetid, Character_GetVirtualWorld(playerid));
			SetPlayerFacingAngle(targetid, angle);
		}
	}
}

stock Character_SetDraggedBy(playerid, draggerid)
{
	pDraggedBy[playerid] = draggerid;
}

stock Character_GetDraggedBy(playerid)
{
	return pDraggedBy[playerid];
}

stock Character_SetDragged(playerid, bool:dragged)
{
	Bit_Set(gCharacterBitState[e_pDragged], playerid, dragged);
}

stock Character_IsDragged(playerid)
{
	return Bit_Get(gCharacterBitState[e_pDragged], playerid);
}

stock Character_SetDragging(playerid, bool:dragging)
{
	Bit_Set(gCharacterBitState[e_pDragging], playerid, dragging);
}

stock Character_IsDragging(playerid)
{
	return Bit_Get(gCharacterBitState[e_pDragging], playerid);
}

stock Character_StartDragTimer(playerid, id)
{
	pDragTimer[playerid] = repeat UpdateDragging(playerid, id);
}

stock Character_StopDragTimer(playerid)
{
	stop pDragTimer[playerid];
	pDragTimer[playerid] = Timer:0;
}