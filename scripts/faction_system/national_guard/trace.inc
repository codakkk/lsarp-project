#include <house_system\core>
#include <building_system\core>
#include <inventory_system\core>
#include <YSI_Coding\y_hooks>

static 
    GangZone[MAX_PLAYERS],
    bool:Tracing[MAX_PLAYERS char]
;

hook OnPlayerClearData(playerid)
{
    GangZoneDestroy(GangZone[playerid]);
    Tracing{playerid} = false;
    return 1;
}
flags:traccia(CMD_POLICE);
CMD:traccia(playerid, params[])
{
    new number;
    if(sscanf(params, "d", number))
        return SendClientMessage(playerid, COLOR_ERROR, "/traccia <numero>");

    new Float:x, Float:y, Float:z;

    new Inventory:entityInv, entityId, E_ENTITY_TYPE:entityType, phoneSlotId;
    if(World_FindPhoneByNumber(number, entityInv, entityId, entityType, phoneSlotId))
    {
        if(Phone_IsOff(entityInv, phoneSlotId))
            return SendClientMessage(playerid, COLOR_ERROR, "[Trace] Il cellulare chiamato risulta spento.");
        
        if(entityType == ENTITY_TYPE_PLAYER)
        {
            Character_GetWorldPosition(entityId, x, y, z);
        }
        else if(entityType == ENTITY_TYPE_DROP)
        {
            Drop_GetWorldPosition(entityId, x, y, z);
        }
        
        GangZoneDestroy(GangZone[playerid]);
        
        new zone[32];
        Get2DZoneName(x, y, z, zone, sizeof(zone));

        if(entityType == ENTITY_TYPE_PLAYER && Character_IsInCall(entityId))
        {
            SetPlayerCheckpoint(playerid, x, y, z, 5.0);
            SendFormattedMessage(playerid, COLOR_YELLOW, "Il segnale è stato rilevato presso %s. E' stato creato un Checkpoint nella posizione precisa.", zone);
            SendClientMessage(playerid, COLOR_YELLOW, "Il checkpoint scomparirà tra 30 secondi.");
        }
        else
        {
            GangZone[playerid] = GangZoneCreate(x - 50.0, y - 50.0, x + 50.0, y + 50.0);
            GangZoneShowForPlayer(playerid, GangZone[playerid], COLOR_YELLOW);
            GangZoneFlashForPlayer(playerid, GangZone[playerid], COLOR_YELLOW);
            
            SendFormattedMessage(playerid, COLOR_YELLOW, "Il segnale è stato rilevato presso %s. E' stata creata una zona intorno al segnale.", zone);
            SendClientMessage(playerid, COLOR_YELLOW, "La zona scomparirà tra 30 secondi.");     
        }

        Tracing{playerid} = true;
        defer OnPlayerStopTracing(playerid);
    }
    else 
    {
        SendClientMessage(playerid, COLOR_ERROR, "Questo numero non esiste o non puo' essere tracciato al momento.");
    }
    return 1;
}
/*flags:traccia(CMD_POLICE);
CMD:traccia(playerid, params[])
{
    new number;
    if(sscanf(params, "d", number))
        return SendClientMessage(playerid, COLOR_ERROR, "/traccia <numero>");

    new Float:x, Float:y, Float:z, bool:isPlayer = false;

    new entity = Character_FindByPhoneNumber(number);

    if(entity != INVALID_PLAYER_ID)
    {
        Character_GetWorldPosition(entity, x, y, z);
        isPlayer = true;
    }
    else
    {
        isPlayer = false;
        for_pool(i : GetDropsPool())
        {
            new key = iter_get_key(i);
            if(Drop_GetItem(key) == gItem_Phone && Drop_GetItemExtra(key) == number)
            {
                entity = key;
                break;
            }
        }

        if(entity != -1)
        {
            Drop_GetWorldPosition(entity, x, y, z);
        }
    }

    if(entity != -1 && x != 0.0 && y != 0.0 && z != 0.0)
    {
        new zone[32];
        Get2DZoneName(x, y, z, zone, sizeof(zone));
        GangZoneDestroy(GangZone[playerid]);

        if(isPlayer)
        {
            if(Character_IsInCall(entity))
            {
                SetPlayerCheckpoint(playerid, x, y, z, 5.0);
                SendFormattedMessage(playerid, COLOR_YELLOW, "Il segnale è stato rilevato presso %s. E' stato creato un Checkpoint nella posizione precisa.", zone);
                SendClientMessage(playerid, COLOR_YELLOW, "Il checkpoint scomparirà tra 30 secondi.");
            }
            else
            {
                GangZone[playerid] = GangZoneCreate(x - 50.0, y - 50.0, x + 50.0, y + 50.0);
                GangZoneShowForPlayer(playerid, GangZone[playerid], COLOR_YELLOW);
		        GangZoneFlashForPlayer(playerid, GangZone[playerid], COLOR_YELLOW);
                
                SendFormattedMessage(playerid, COLOR_YELLOW, "Il segnale è stato rilevato presso %s. E' stata creata una zona intorno al segnale.", zone);
                SendClientMessage(playerid, COLOR_YELLOW, "La zona scomparirà tra 30 secondi.");
            }
        }
        else
        {
            GangZone[playerid] = GangZoneCreate(x - 50.0, y - 50.0, x + 50.0, y + 50.0);
            GangZoneShowForPlayer(playerid, GangZone[playerid], COLOR_YELLOW);
            GangZoneFlashForPlayer(playerid, GangZone[playerid], COLOR_YELLOW);
            
            SendFormattedMessage(playerid, COLOR_YELLOW, "Il segnale è stato rilevato presso %s. E' stata creata una zona intorno al segnale.", zone);
            SendClientMessage(playerid, COLOR_YELLOW, "La zona scomparirà tra 30 secondi.");
        }
        Tracing{playerid} = true;
        defer OnPlayerStopTracing(playerid);
    }
    else return SendClientMessage(playerid, COLOR_ERROR, "Questo numero non esiste o non puo' essere tracciato al momento.");
    return 1;
}*/

timer OnPlayerStopTracing[30000](playerid) 
{
    Tracing{playerid} = false;
	DisablePlayerCheckpoint(playerid);
	GangZoneDestroy(GangZone[playerid]);
	GangZone[playerid] = -1;

	SendClientMessage(playerid, COLOR_WHITE, "Hai perso il segnale.");
    return 1;
}

/*stock GetDynamicWorldPosition(virtualWorld, Float:x, Float:y, Float:z)
{

}*/

stock Character_GetWorldPosition(playerid, &Float:x, &Float:y, &Float:z)
{
    new world = Character_GetVirtualWorld(playerid);
    if(world < HOUSE_START_WORLD)
    {
        GetPlayerPos(playerid, x, y, z);
    }
    else if(world < BUILDING_START_WORLD)
    {
        new hid = Character_GetHouseID(playerid);
        
        if(!House_IsValid(hid))
            return 0;
        House_GetEnterPosition(hid, x, y, z);
    }
    else if(world >= BUILDING_START_WORLD)
    {
        new bid = Character_GetNearBuildingID(playerid);
        
        if(!Building_IsValid(bid))
            return 0;
        Building_GetEnterPos(bid, x, y, z);
    }
    return 1;
}

stock Drop_GetWorldPosition(dropid, &Float:x, &Float:y, &Float:z)
{
    new world = Drop_GetVirtualWorld(dropid);
    if(world < HOUSE_START_WORLD)
    {
        Drop_GetPosition(dropid, x, y, z);
    }
    else if(world < BUILDING_START_WORLD)
    {
        new hid = Drop_GetVirtualWorld(dropid) - HOUSE_START_WORLD;
        
        if(!House_IsValid(hid))
            return 0;
        
        House_GetEnterPosition(hid, x, y, z);
    }
    else if(world >= BUILDING_START_WORLD)
    {
        new bid = Drop_GetVirtualWorld(dropid) - BUILDING_START_WORLD;
        
        if(!Building_IsValid(bid))
            return 0;
        
        Building_GetEnterPos(bid, x, y, z);
    }
    return 1;
}