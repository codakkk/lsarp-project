
#include <YSI_Coding\y_hooks>

#define ALERTS_BEFORE_DESYNC    10

static 
    PlayerUpdateTick[MAX_PLAYERS] = {0, ...},
    Ammo[MAX_PLAYERS char],
    bool:PlayerSynced[MAX_PLAYERS char] = {false, ...},
    Float:PacketLoss[MAX_PLAYERS],
    PlayerAlertCount[MAX_PLAYERS char] = {0, ...};

hook OnCharacterSpawn(playerid)
{
    PlayerSynced{playerid} = true;
    PlayerAlertCount{playerid} = 0;
    PacketLoss[playerid] = 0.0;
    Ammo{playerid} = 0;
    return 1;
}

hook OnCharacterClearData(playerid)
{
    PlayerUpdateTick[playerid] = 0;
    PlayerSynced{playerid} = true;
    PlayerAlertCount{playerid} = 0;
    PacketLoss[playerid] = 0.0;
    Ammo{playerid} = 0;
    return 1;
}

hook OnPlayerUpdate(playerid)
{
    if(!Character_IsLogged(playerid))
        return 1;
    if(gettime() > PlayerUpdateTick[playerid])
    {
        PlayerUpdateTick[playerid] = gettime() + 2;

        static currentWeapon, currentAmmo, bool:isSynced;

        GetPlayerWeaponData(playerid, 0, currentWeapon, currentAmmo);
        
        isSynced = (currentAmmo != Ammo{playerid});

        PlayerAlertCount{playerid} = (isSynced) ? 0 : PlayerAlertCount{playerid}++;

        if(PlayerAlertCount{playerid} >= ALERTS_BEFORE_DESYNC)
        {
            printf("Playerid %d desynced.");
            PlayerSynced{playerid} = false;//(currentAmmo != Ammo{playerid});
        }
        
        SetPlayerAmmo(playerid, currentWeapon, !currentAmmo);
        Ammo{playerid} = currentAmmo;

        // printf("Ammo: %d - Current: %d - Synced: %d", Ammo{playerid}, currentAmmo, IsPlayerSynced(playerid) ? 1 : 0);

        if(!IsPlayerSynced(playerid))
        {
            GameTextForPlayer(playerid, "IL SERVER HA RILEVATO CHE TI SEI DESYNCATO. NON MUOVERTI E RILOGGA.", 99999, 6);
            //Character_SetVirtualWorld(playerid, playerid);
        }
    }
    static
		Float:packet = 0.0;
    packet = NetStats_PacketLossPercent(playerid);
	if(floatcmp(packet, 0.0) != 0 && floatcmp(packet, PacketLoss{playerid}) != 0)
	{
		PacketLoss{playerid} = packet;
    }
    return 1;
}

stock bool:IsPlayerSynced(playerid)
{
    return PlayerSynced{playerid};
}

stock Float:GetPacketLoss(playerid)
{
    return PacketLoss{playerid};
}