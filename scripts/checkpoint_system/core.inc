
#include <YSI_Coding\y_hooks>

enum 
{
    CHECKPOINT_TYPE_CHOPSHOP,
    CHECKPOINT_TYPE_HOUSE
}

static enum e_CHECKPOINT_DATA
{
    E_CHECKPOINT_TYPE,
    E_CHECKPOINT_ID
};

hook OnGameModeInit()
{
    //Checkpoint_Create(CHECKPOINT_TYPE_CHOPSHOP, 0, 2771.7817, -1606.4154, 10.9219, 2.0, -1, -1);
    return 1;
}

hook OnPlayerEnterDynamicCP(playerid, checkpointid)
{
    printf("Player %d enters checkpoint %d", playerid, checkpointid);
    return 1;
}

hook OnPlayerLeaveDynamicCP(playerid, checkpointid)
{
    printf("Player %d leaving checkpoint %d", playerid, checkpointid);
    return 1;
}

stock Checkpoint_Create(type, elementid, Float:x, Float:y, Float:z, Float:size, worldid = -1, interiorid = -1)
{
    new 
        checkpointid = CreateDynamicCP(x, y, z, size, worldid, interiorid, -1, 1.5),
        data[e_CHECKPOINT_DATA]
    ;
    data[E_CHECKPOINT_TYPE] = type;
    data[E_CHECKPOINT_ID] = elementid;
    Streamer_SetArrayData(STREAMER_TYPE_CP, checkpointid, E_STREAMER_EXTRA_ID, data);
    return 1;
}

stock Checkpoint_GetInfo(checkpointid, &id, &type)
{
    if(!IsValidDynamicCP(checkpointid))
        return 0;
    new data[e_CHECKPOINT_DATA];
    Streamer_GetArrayData(STREAMER_TYPE_CP, checkpointid, E_STREAMER_EXTRA_ID, data);
    id = data[E_CHECKPOINT_ID];
    type = data[E_CHECKPOINT_TYPE];
    return 1;
}