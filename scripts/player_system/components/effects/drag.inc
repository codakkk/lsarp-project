
#include <YSI_Coding\y_hooks>

static 
	DraggedBy[MAX_PLAYERS],
	Timer:DragTimer[MAX_PLAYERS]
;

hook OnPlayerClearData(playerid)
{
	Character_SetDraggedBy(playerid, INVALID_PLAYER_ID);
	stop DragTimer[playerid];
    return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnCharacterSpawn(playerid)
{
	stop DragTimer[playerid];
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
	DraggedBy[playerid] = draggerid;
}

stock Character_GetDraggedBy(playerid)
{
	return DraggedBy[playerid];
}

stock Character_StartDragTimer(playerid, id)
{
	DragTimer[playerid] = repeat UpdateDragging(playerid, id);
}

stock Character_StopDragTimer(playerid)
{
	stop DragTimer[playerid];
	DragTimer[playerid] = Timer:0;
}