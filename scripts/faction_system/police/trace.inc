#include <house_system\core>
#include <buildings\core>
#include <inventory_system\core>
#include <YSI_Coding\y_hooks>

static 
    GangZone[MAX_PLAYERS],
    bool:Tracing[MAX_PLAYERS char]
;

hook OnCharacterClearData(playerid)
{
    GangZoneDestroy(GangZone[playerid]);
    Tracing{playerid} = false;
    return 1;
}

//flags:traccia(CMD_POLICE);
CMD:traccia(playerid, params[])
{

    //if(Character_GetDivision(playerid) != POLICE_DIVISION_SWAT && Character_GetDivision(playerid) != POLICE_DIVISION_SI)
        //return SendClientMessage(playerid, COLOR_ERROR, "Non fai parte della divisione corretta.");

    new number;
    if(sscanf(params, "d", number))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /traccia <numero>");

    new Float:x, Float:y, Float:z;

    new Inventory:entityInv, entityId, EntityType:entityType, phoneSlotId;
    if(World_FindPhoneByNumber(number, entityInv, entityType, entityId, phoneSlotId))
    {
        
        if(Phone_IsOff(entityInv, phoneSlotId))
            return SendClientMessage(playerid, COLOR_ERROR, "[Trace] Il cellulare chiamato risulta spento.");
        
        Entity_GetWorldPosition(entityType, entityId, x, y, z);
        
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

timer OnPlayerStopTracing[30000](playerid) 
{
    Tracing{playerid} = false;
	DisablePlayerCheckpoint(playerid);
	GangZoneDestroy(GangZone[playerid]);
	GangZone[playerid] = -1;

	SendClientMessage(playerid, COLOR_WHITE, "Hai perso il segnale.");
    return 1;
}