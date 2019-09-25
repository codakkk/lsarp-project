#include <YSI_Coding\y_hooks> // Needed for NexAC

#define DEBUG 0

#include <nex-ac_it.lang>

#define AC_USE_VENDING_MACHINES		false
#define AC_USE_TUNING_GARAGES		false
#define AC_USE_PICKUP_WEAPONS		false
#define AC_USE_AMMUNATIONS			false
#define AC_USE_RESTAURANTS			false
#define AC_USE_PAYNSPRAY			true
#define AC_USE_CASINOS				false
#define AC_USE_NPC					false

#define AC_MAX_CONNECTS_FROM_IP     (3)

// WHEN UPDATING "ac.inc" REMEMBER TO MAKE ACInfo PUBLIC INSTEAD OF static.

#include "ac"

#define AC_GetPlayerHealth AntiCheatGetHealth
#define AC_SetPlayerHealth SetPlayerHealth

#define AC_GetPlayerArmour AntiCheatGetArmour
#define AC_SetPlayerArmour SetPlayerArmour

#define COLOR_ANTICHEAT 0xFF6347AA

static enum _:E_CHEAT_DATA
{
    Alerts,
    LastAlert,
};

static 
    Map:CheatData[MAX_PLAYERS];

hook OnPlayerClearData(playerid)
{
    if(map_valid(CheatData[playerid]))
        map_delete_deep(CheatData[playerid]);
    return 1;
}

ptask LVRP_OnCheatTimer[10000](playerid) 
{
    if(!map_valid(CheatData[playerid]))
        return 0;
    static alerts = 0;
    for_map(i : CheatData[playerid])
    {
        alerts = iter_get(i, Alerts);
        if(alerts > 0)
        {
            if(GetTickCount() - iter_get(i, LastAlert) > 60000)
            {
                iter_set_cell(i, Alerts, alerts - 1);
                iter_set_cell(i, LastAlert, GetTickCount());
            }
        }
    }
    return 1;
}

forward OnCheatDetected(playerid, ip_address[], type, code);
public OnCheatDetected(playerid, ip_address[], type, code) 
{
    if(!type)
    {
        if(!map_valid(CheatData[playerid]))
            CheatData[playerid] = map_new();

        new alerts = 0;

        if(map_has_key(CheatData[playerid], code))
        {
            alerts = map_get(CheatData[playerid], code, Alerts) + 1;
            map_set_cell(CheatData[playerid], code, Alerts, alerts);
            map_set_cell(CheatData[playerid], code, LastAlert, GetTickCount());
        }
        else
        {
            new data[E_CHEAT_DATA];
            data[Alerts] = alerts = 1;
            data[LastAlert] = GetTickCount();
            map_add_arr(CheatData[playerid], code, data);
        }

        if(alerts >= GetCheatMaxAlerts(code))
        {
            map_set_cell(CheatData[playerid], code, Alerts, 0);

            if(IsKickCheat(code))
            {
                SendFormattedMessage(playerid, COLOR_ANTICHEAT, "Sei stato kickato con il sospetto di %s.", GetCheatName(code));
                SendCheatAlertToAdmins(COLOR_ANTICHEAT, "AntiCheat: %s (%d) è stato kickato per sospetto di %s.", Character_GetOOCName(playerid), playerid, GetCheatName(code));
                AntiCheatKickWithDesync(playerid, code);
                return 1;
            }

            SendCheatAlertToAdmins(COLOR_ANTICHEAT, "AntiCheat: %s (%d) è sospetto di %s.", Character_GetOOCName(playerid), playerid, GetCheatName(code));

            if(Server_GetOnlineStaffCount() <= 0)
            {
                AntiCheatKickWithDesync(playerid, code);
                SendFormattedMessage(playerid, COLOR_ANTICHEAT, "AntiCheat: Sei stato kickato per sospetto di %s.", GetCheatName(code));
            }
        }
    }
    return 1;
}

static SendCheatAlertToAdmins(color, str[], {Float,_}:...)
{
    new s[128];
    format(s, sizeof(s), str, ___(2));
    foreach(new i : Player)
    {
        if(!Character_IsLogged(i) || Account_GetAdminLevel(i) < 2 || !Player_HasCheatAlertsEnabled(i))
            continue;
        SendClientMessage(i, color, s);
    }
    return 1;
}

static bool:IsKickCheat(code)
{
    return code == 0 || code == 1;
}

static GetCheatMaxAlerts(code)
{
    if(code == 15 || code == 16 || code == 17)
        return 1;
    return 3;
}

static GetCheatName(code)
{
    new name[32];
    switch(code)
    {
        case 0, 1: name = "Airbreak hack";
        case 2 .. 6: name = "Teleport hack";
        case 7: name = "Fly hack (a piedi)";
        case 8: name = "Fly hack (veicolo)";
        case 9: name = "Speed hack (a piedi)";
        case 10: name = "Speed hack (veicolo)";
        case 13: name = "Armour hack";
        case 15: name = "Weapon hack";
        case 16: name = "Ammo hack";
        case 17: name = "Infinite Ammo hack";
        case 18: name = "Special Actions hack";
        case 21: name = "Invisible hack";
        case 25: name = "Quick Turn hack";
        case 26: name = "Rapid Fire hack";
        case 29: name = "Pro Aim hack";
        case 30: name = "CJ Run hack";
        case 34: name = "AFK Ghost hack";
        case 35: name = "Full Aiming hack";
        case 50: name = "Seat Changing Flood hack";
        default: name = "Altro";
    }
    return name;
}

/*stock bool:ac_RemovePlayerWeapon(playerid, weaponid)
{
	if(!IsPlayerConnected(playerid))
		return false;
	
	ResetPlayerWeapons(playerid);

	for(new ac_i = 12; ac_i >= 0; --ac_i)
	{
		if(ACInfo[playerid][acWeapon][ac_i] == weaponid)
		{
			ACInfo[playerid][acWeapon][ac_i] = ACInfo[playerid][acAmmo][ac_i] = 0;
			ACInfo[playerid][acSetWeapon][ac_i] = -1;
			ACInfo[playerid][acGiveAmmo][ac_i] = -65535;
            continue;
		}
		GivePlayerWeapon(playerid, ACInfo[playerid][acWeapon][ac_i], ACInfo[playerid][acAmmo][ac_i]);
	}

	return true;
}*/

stock Server_GetOnlineStaffCount()
{
    new counter = 0;
    foreach(new i : Player)
    {
        if(!Character_IsLogged(i))
            continue;
        if(Account_GetAdminLevel(i) > 1)
            counter++;
    }
    return counter;
}