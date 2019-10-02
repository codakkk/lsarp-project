
#include <YSI_Coding\y_hooks>

enum E_CHECKPOINT_TYPE
{
    CHECKPOINT_TYPE_CHOPSHOP,
    CHECKPOINT_TYPE_HOUSE
}

static enum e_CHECKPOINT_DATA
{
    E_CHECKPOINT_TYPE:CHECKPOINT_TYPE,
    CHECKPOINT_ID
};

static PlayerCheckPoint[MAX_PLAYERS];

hook OnGameModeInit()
{
    //Checkpoint_Create(CHECKPOINT_TYPE_CHOPSHOP, 0, 2771.7817, -1606.4154, 10.9219, 2.0, -1, -1);
    return 1;
}

hook OnPlayerEnterDynamicCP(playerid, checkpointid)
{
    PlayerCheckPoint[playerid] = checkpointid;
    printf("Player %d enters checkpoint %d", playerid, checkpointid);
    return 1;
}

hook OnPlayerLeaveDynamicCP(playerid, checkpointid)
{
    PlayerCheckPoint[playerid] = -1;
    printf("Player %d leaving checkpoint %d", playerid, checkpointid);
    return 1;
}

stock Checkpoint_Create(E_CHECKPOINT_TYPE:type, GLOBAL_TAG_TYPES:elementid, Float:x, Float:y, Float:z, Float:size, worldid = -1, interiorid = -1, Float:renderDist = 1.0)
{
    new 
        checkpointid = CreateDynamicCP(x, y, z, size, worldid, interiorid, -1, renderDist),
        data[e_CHECKPOINT_DATA]
    ;
    data[CHECKPOINT_TYPE] = type;
    data[CHECKPOINT_ID] = _:elementid;
    Streamer_SetArrayData(STREAMER_TYPE_CP, checkpointid, E_STREAMER_EXTRA_ID, data);
    return 1;
}

stock bool:Checkpoint_GetInfoOfType(checkpointid, E_CHECKPOINT_TYPE:requestedType, &GLOBAL_TAG_TYPES:id)
{
    if(!IsValidDynamicCP(checkpointid))
        return false;
    new data[e_CHECKPOINT_DATA];
    Streamer_GetArrayData(STREAMER_TYPE_CP, checkpointid, E_STREAMER_EXTRA_ID, data);
    if(data[CHECKPOINT_TYPE] != requestedType)
        return false;
    id = data[CHECKPOINT_ID];
    return true;
}

stock Checkpoint_GetInfo(checkpointid, &id, &type)
{
    if(!IsValidDynamicCP(checkpointid))
        return 0;
    new data[e_CHECKPOINT_DATA];
    Streamer_GetArrayData(STREAMER_TYPE_CP, checkpointid, E_STREAMER_EXTRA_ID, data);
    id = data[CHECKPOINT_ID];
    type = data[CHECKPOINT_TYPE];
    return 1;
}

stock Character_GetCheckpoint(playerid)
{
    return PlayerCheckPoint[playerid];
}