
#include <YSI_Coding\y_hooks>

static 
    EditMode_Type[MAX_PLAYERS char] = {0, ...},
    EditMode_ObjectID[MAX_PLAYERS] = {0, ...},
    EditMode_CustomID[MAX_PLAYERS] = {0, ...} // ID of the Type object (ex: Drop ID)
;

enum
{
    EDIT_TYPE_NONE = 0,
    EDIT_TYPE_DROP = 1,
    EDIT_TYPE_BUYING_FURNITURE,
    EDIT_TYPE_FURNITURE,
    EDIT_TYPE_PLAYER_OBJECT,
    EDIT_TYPE_GATE_START,
    EDIT_TYPE_GATE_END
}

hook OnPlayerClearData(playerid)
{
    CancelEdit(playerid);
    EditMode_Type{playerid} = 0;
    EditMode_ObjectID[playerid] = 0;
    EditMode_CustomID[playerid] = 0;
    return 1;
}

hook OnCharacterDeath(playerid, killerid, reason)
{
    EditObject_End(playerid);
    return 1;
}

hook OnPlayerStateChange(playerid, newstate, oldstate)
{
	if(!EditObject_IsEditing(playerid)) 
		return 0;
	if(oldstate == PLAYER_STATE_ONFOOT && newstate != PLAYER_STATE_ONFOOT)
	{
		EditObject_End(playerid);
	}
	return 1;
}

// Returns -1 if editing another object.
stock EditObject_Start(playerid, type, objectid, extra_id)
{
    if(EditObject_IsEditing(playerid))
        return -1;
    EditMode_Type{playerid} = type;
    EditMode_ObjectID[playerid] = objectid;
    EditMode_CustomID[playerid] = extra_id;
    if(type == EDIT_TYPE_PLAYER_OBJECT)
    {
        EditAttachedObject(playerid, objectid);
    }
    else
    {
        EditDynamicObject(playerid, objectid);
    }
    return 1;
}

stock EditObject_End(playerid)
{
    CancelEdit(playerid);
    EditMode_Type{playerid} = EDIT_TYPE_NONE;
    EditMode_ObjectID[playerid] = 0;
    EditMode_CustomID[playerid] = 0;
    return 1;
}

stock EditObject_IsEditing(playerid)
{
    return EditMode_Type{playerid} != EDIT_TYPE_NONE;
}

stock EditObject_GetType(playerid)
{
    return EditMode_Type{playerid};
}

stock EditObject_GetObjectID(playerid)
{
    return EditMode_ObjectID[playerid];
}

stock EditObject_GetExtraID(playerid)
{
    return EditMode_CustomID[playerid];
}