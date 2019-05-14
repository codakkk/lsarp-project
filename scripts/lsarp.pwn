// https://packages.sampctl.com/
/*

	-- REMEMBER --
	y_hooks can replace those identifiers.
	https://github.com/Misiur/YSI-Includes/blob/f4d85a8d1c7552618c0546d3f93d5ef625ed59c5/YSI_Coding/y_hooks/impl.inc#L148-L160
	static stock
	YSI_g_sReplacements[][E_HOOK_NAME_REPLACEMENT_DATA] =
		{
			{"CP",  "Checkpoint", 2, 10},
			// For "SIF".
			{"Cnt", "Container",  3, 9},
			{"Inv", "Inventory",  3, 9},
			{"Dyn", "Dynamic",	3, 7},
			{"TD",  "TextDraw",	2, 8},
			{"Upd", "Update",	3, 6},
			{"Obj", "Object",	3, 6},
			{"Cmd", "Command",	3, 7}
		};


		https://forum.sa-mp.com/showthread.php?t=659714 -> PawnPLUS Intersting
		https://forum.sa-mp.com/showthread.php?t=435525 -> Timerfix
		https://forum.sa-mp.com/showthread.php?t=120013 -> MapAndreas
		https://forum.sa-mp.com/showthread.php?t=326980 -> JIT
		https://forum.sa-mp.com/showthread.php?t=489897 -> Bit 
		https://forum.sa-mp.com/showthread.php?t=655688 -> Loot Zones
		https://forum.sa-mp.com/showthread.php?t=558839 -> Walk anims etc
		https://www.burgershot.gg/showthread.php?tid=314 -> PawnPlus extension MySQL CHECK IT OUT
*/
#pragma warning disable 208 // actually just a good way to prevent warning: "function with tag result used before definition, forcing reparse".
#define ENABLE_MAPS
//#define LSARP_DEBUG

#if defined LSARP_DEBUG
	#warning LSARP_DEBUG is enabled. Care!!
#endif

#include <includes.pwn>
#include <defines.pwn>
#include <utils\iterators.pwn>
#include <forwarded_functions.pwn>
#include <miscellaneous\timestamp_to_date.pwn>

DEFINE_HOOK_REPLACEMENT(ShowRoom, SR);
DEFINE_HOOK_REPLACEMENT(Element, Elm);

#include <miscellaneous\globals.pwn>
// https://github.com/emmet-jones/New-SA-MP-callbacks/blob/master/README.md
// Exception. Must be on top of all others.
#include <pickup\enum.pwn>

#include <anticheat\enum.pwn>
#include <account_system\enum.pwn>
#include <drop_system\enum.pwn>
#include <player\enum.pwn>
#include <vehicles\enum.pwn>
#include <admin\enum.pwn>
#include <dealership\enum.pwn>
#include <inventory\enum.pwn>
#include <building\enum.pwn>
#include <weapon_system\enum.pwn>
#include <house_system\enum.pwn>
#include <faction_system\enum.pwn>
#include <dp_system\enum.pwn>

#include <database\core.pwn>

// ===== [ ANTI-CHEAT SYSTEM ] =====
#include <anticheat\core.pwn>

// ===== [ PICKUP SYSTEM ] =====
#include <pickup\core.pwn>

// ===== [ INVENTORY SYSTEM ] =====
#include <inventory\core.pwn>
#include <inventory\server.pwn>

// ===== [ WEAPON SYSTEM ] =====
#include <weapon_system\core.pwn>

// ===== [ DP SYSTEM ] =====
#include <dp_system\core.pwn>

#include <utils/utils.pwn>
#include <utils/maths.pwn>


#include <callbacks.pwn>

#include <miscellaneous\global_timers.pwn>

#include <YSI_Coding\y_hooks> // Place hooks after this. Everything included before this, is hooked first.

main()
{
	printf("LSARP - By CodaKKK. Started: 26/02/2019.");

	// Should I initialize them in a OnGameModeInit hook?
	PlayerInventory = map_new();
	VehicleInventory = map_new();
	HouseList = list_new();
	HouseInventory = map_new();

	printf("Valid: %d", list_valid(Inventory:0));
}

hook OnGameModeInit() 
{
	SetGameModeText("ApoC1");
	
	ShowPlayerMarkers(PLAYER_MARKERS_MODE_OFF);
	SetNameTagDrawDistance(20.0);
	DisableInteriorEnterExits();
	ManualVehicleEngineAndLights(); 
	EnableStuntBonusForAll(false);

	SetWorldTime(0);

	// /ritirastipendio
	Pickup_Create(1239, 0, 292.3752, 180.7307, 1007.1790, ELEMENT_TYPE_PAYCHECK, -1, 3);
	CreateDynamic3DTextLabel("/ritirastipendio", COLOR_BLUE, 292.3752, 180.7307, 1007.1790 + 0.55, 20.0, .worldid = -1, .interiorid = 3);
	// /lasciacarcere
	Pickup_Create(1239, 0, 2649.7790, -1948.9510, -58.7273, ELEMENT_TYPE_JAIL_EXIT, .worldid = -1, .interiorid = 0);
	CreateDynamic3DTextLabel("/lasciacarcere", COLOR_BLUE, 2649.7790, -1948.9510, -58.7273 + 0.55, 20.0, .worldid = -1, .interiorid = 0);

	printf("BUilding Load ALl");
	Building_LoadAll();
	House_LoadAll();

	Streamer_TickRate(30);
	//Streamer_SetVisibleItems(STREAMER_TYPE_OBJECT, 900);
	return 1;
}

hook OnPlayerSpawn(playerid)
{
	if(IsPlayerNPC(playerid))
		return Y_HOOKS_BREAK_RETURN_1;
	if(!Account_IsLogged(playerid) || !Character_IsLogged(playerid))
	{
		KickEx(playerid);
		return Y_HOOKS_BREAK_RETURN_1;
	}

	SetPlayerDrunkLevel(playerid, 0);

	SetPlayerSkillLevel(playerid, WEAPONSKILL_PISTOL, 1);
	SetPlayerSkillLevel(playerid, WEAPONSKILL_MICRO_UZI, 1);
	SetPlayerSkillLevel(playerid, WEAPONSKILL_SAWNOFF_SHOTGUN, 1);

	TextDrawShowForPlayer(playerid, Clock);

	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerUpdate(playerid)
{
	if(IsPlayerNPC(playerid)) return Y_HOOKS_BREAK_RETURN_1;
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerDeath(playerid, killerid, reason) 
{
	if(IsPlayerNPC(playerid)) return Y_HOOKS_BREAK_RETURN_1;

	if(killerid != INVALID_PLAYER_ID)
		Log(Character_GetOOCName(playerid), Character_GetOOCName(killerid), "OnPlayerDeath", reason);
	
	SetPlayerDrunkLevel(playerid, 0);
	if(pAnimLoop{playerid})
	{
		pAnimLoop{playerid} = false;
		TextDrawHideForPlayer(playerid, txtAnimHelper);
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerRequestSpawn(playerid)
{
    if(!Character_IsLogged(playerid))
	   return 0;
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	return Y_HOOKS_CONTINUE_RETURN_1;
}
hook OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ) 
{
	if(IsPlayerNPC(playerid))
		return Y_HOOKS_BREAK_RETURN_1;
	
	if( !( -1000.0 <= fX <= 1000.0 ) || !( -1000.0 <= fY <= 1000.0 ) || !( -1000.0 <= fZ <= 1000.0 ) )
    {
		#if defined DEBUG
			//printf("Not in range");
		#endif
		return 0;
	}
	if(hittype == BULLET_HIT_TYPE_PLAYER && hitid != INVALID_PLAYER_ID && pAdminDuty[hitid])
	{
		return Y_HOOKS_BREAK_RETURN_0;
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

// Not Hooking it makes the OnPlayerWeaponShot not called.
hook OnPlayerShootDynObject(playerid, weaponid, objectid, Float:x, Float:y, Float:z)
{
	return Y_HOOKS_CONTINUE_RETURN_1;
}


hook OnAntiCheatDetected(playerid, code)
{
	SendMessageToAdmins(true, COLOR_ERROR, "[ADMIN-ALERT]: %s (%d) è sospetto di hack. (%s)", Character_GetOOCName(playerid), playerid, AC_Name[code]);
	return 1;
}

hook OnVehicleMod(playerid, vehicleid, componentid)
{
	RemoveVehicleComponent(vehicleid, componentid);
	return 0;
}


public OnPlayerCommandReceived(playerid, cmd[], params[], flags)
{
	if(!Character_IsLogged(playerid))
		return 0;
	if(flags & CMD_ALIVE_USER)
	{
		if( (Character_IsJailed(playerid) && !Character_IsICJailed(playerid)) || !Character_IsAlive(playerid))
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando.");
			return 0;
		}
	}
	if(AccountInfo[playerid][aAdmin] <= 0)
	{
		if(flags & CMD_PREMIUM_BRONZE && AccountInfo[playerid][aPremium] < 1)
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando.");
			return 0;
		}
		else if(flags & CMD_PREMIUM_SILVER && AccountInfo[playerid][aPremium] < 2)
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando.");
			return 0;
		}
		else if(flags & CMD_PREMIUM_SILVER && AccountInfo[playerid][aPremium] < 3)
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando.");
			return 0;
		}
	}

	new factionid = Character_GetFaction(playerid);
	if(flags & CMD_POLICE)
	{
		if(factionid == INVALID_FACTION_ID || Faction_GetType(factionid) != FACTION_TYPE_POLICE || !Character_IsAlive(playerid))
		{
			SendClientMessage(playerid, COLOR_ERROR, "Devi essere un membro della Guardia Nazionale in servizio per utilizzare questo comando.");
			return 0;
		}
	}
	else if(flags & CMD_MEDICAL)
	{
		if(factionid == INVALID_FACTION_ID || Faction_GetType(factionid) != FACTION_TYPE_MEDICAL || !Character_IsAlive(playerid))
		{
			SendClientMessage(playerid, COLOR_ERROR, "Devi essere un medico in servizio per poter utilizzare questo comando.");
			return 0;
		}
	}
	else if(flags & CMD_GOVERNMENT)
	{
		if(factionid == INVALID_FACTION_ID || Faction_GetType(factionid) != FACTION_TYPE_GOVERNAMENT || !Character_IsAlive(playerid))
		{
			SendClientMessage(playerid, COLOR_ERROR, "Devi essere un membro del Governo in servizio per utilizzare questo comando.");
			return 0;
		}
	}
	else if(flags & CMD_ILLEGAL)
	{
		if(factionid == INVALID_FACTION_ID || (Faction_GetType(factionid) != FACTION_TYPE_IMPORT_WEAPONS && Faction_GetType(factionid) != FACTION_TYPE_IMPORT_DRUGS) || !Character_IsAlive(playerid))
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando.");
			return 0;
		}
	}

	if(flags & CMD_SUPPORTER && AccountInfo[playerid][aAdmin] < 1)
	{
		SendClientMessage(playerid, COLOR_ERROR, "Non sei un membro dello staff.");
		return 0;
	}
	else if(flags & CMD_JR_MODERATOR && AccountInfo[playerid][aAdmin] < 2)
	{
		SendClientMessage(playerid, COLOR_ERROR, "Non sei un membro dello staff.");
		return 0;
	}
	else if(flags & CMD_MODERATOR && AccountInfo[playerid][aAdmin] < 3)
	{
		SendClientMessage(playerid, COLOR_ERROR, "Non sei un membro dello staff.");
		return 0;
	}
	else if(flags & CMD_ADMIN && AccountInfo[playerid][aAdmin] < 4)
	{
		SendClientMessage(playerid, COLOR_ERROR, "Non sei un membro dello staff.");
		return 0;
	}
	else if(flags & CMD_DEVELOPER && AccountInfo[playerid][aAdmin] < 5)
	{
		SendClientMessage(playerid, COLOR_ERROR, "Non sei un membro dello staff.");
		return 0;
	}
	else if(flags & CMD_RCON && !IsPlayerAdmin(playerid))
	{
		SendClientMessage(playerid, COLOR_ERROR, "Non sei un membro dello staff.");
		return 0;
	}
	return 1;
}

public OnPlayerCommandPerformed(playerid, cmd[], params[], result, flags)
{
	if(result == -1)
	{
		SendClientMessage(playerid, -1, "Il comando inserito non esiste. Digita \"/aiuto\" per una lista di comandi.");
		return 0;
	}
	return 1;
}

hook OnPlayerText(playerid, text[])
{
	if(!Character_IsLogged(playerid) || isnull(text))
		return Y_HOOKS_BREAK_RETURN_0;
	return Y_HOOKS_CONTINUE_RETURN_0;
}

hook OnPlayerDisconnect(playerid, reason)
{
	new const reasonName[3][16] = {"Crash", "Quit", "Kick/Ban"};
	new String:string, name[MAX_PLAYER_NAME];

	GetPlayerName(playerid, name, sizeof(name));
	string = str_format("%s è uscito dal server. [%s]", name, reasonName[reason]);
	SendClientMessageToAllStr(COLOR_GREY, string);
	TextDrawHideForPlayer(playerid, Clock);
	if(Character_IsLogged(playerid))
	{
		CallLocalFunction(#OnCharacterDisconnected, "i", playerid);
		CallLocalFunction(#OnPlayerClearData, "i", playerid);
	}
	
	Account_SetLogged(playerid, false);
	Character_SetLogged(playerid, false);
	return 1;
}

// This is the last callback called after the hooks.
hook OnPlayerConnect(playerid)
{
	wait_ticks(1);
	
	SetPlayerColor(playerid, 0xFFFFFFFF);

	// Remove Vending Machines
	RemoveBuildingForPlayer(playerid, 1302, 0.0, 0.0, 0.0, 6000.0);
    RemoveBuildingForPlayer(playerid, 1209, 0.0, 0.0, 0.0, 6000.0);
    RemoveBuildingForPlayer(playerid, 955, 0.0, 0.0, 0.0, 6000.0);
    RemoveBuildingForPlayer(playerid, 956, 0.0, 0.0, 0.0, 6000.0);
    RemoveBuildingForPlayer(playerid, 1775, 0.0, 0.0, 0.0, 6000.0);
    RemoveBuildingForPlayer(playerid, 1776, 0.0, 0.0, 0.0, 6000.0);
    RemoveBuildingForPlayer(playerid, 1977, 0.0, 0.0, 0.0, 6000.0);
	
	Account_SetLogged(playerid, false);
	Character_SetLogged(playerid, false);
	gLoginAttempts[playerid] = 0;
	
	CallLocalFunction(#OnPlayerClearData, "d", playerid);

	for(new i = 0; i < 60; ++i)
	{
		SendClientMessage(playerid, -1, " ");
	}
	SendClientMessage(playerid, COLOR_YELLOW, "________________________________________________________________");
	SendClientMessage(playerid, COLOR_YELLOW, "Benvenuto su Los Santos Apocalypse Roleplay [www.lsarp.it].");
	SendClientMessage(playerid, COLOR_YELLOW, "Alpha v0.1 - Righe GameMode: 25159");
	SendClientMessage(playerid, COLOR_YELLOW, "________________________________________________________________");
	
	// Anti Bot Attack
	new
		cmd[32],
		ip[16],
		tmp[16],
		count;
	GetPlayerIp(playerid, ip, sizeof ip);

	foreach (new i : Player)
	{
		GetPlayerIp(i, tmp, sizeof tmp);
		if(!strcmp(ip, tmp))
			count ++;
	}

	if(count >= 4)
	{
		Ban(playerid);
		format(cmd, sizeof cmd, "banip %s", ip);
		SendRconCommand(cmd);
	}
	else if(count > 3)
	{
		KickEx(playerid);
		return 0;
	}
	LoadPlayerTextDraws(playerid);
	SendClientMessage(playerid, -1, "Hai 60 secondi per registrarti o effettuare il login prima di essere kickato.");
	//OnPlayerRequestClass(playerid, 0);
	return 1;
}

hook OnRconLoginAttempt(ip[], password[], success )
{
	new ip2[16];
	foreach(new i : Player)
	{
		GetPlayerIp(i, ip2, 16);
		if(!strcmp(ip, ip2, true))
		{
			if(success)
			{
				SendMessageToAdmins(true, COLOR_ADMIN, "%s (%d) è entrato in RCON.", Character_GetOOCName(i), i);
			}
			else
			{
				SendMessageToAdmins(true, COLOR_ADMIN, "%s (%d) ha tentato di entrare in RCON.", Character_GetOOCName(i), i);
			}
		}
	}
	return 1;
}

SSCANF:u(const string[])
{
	new userid;
	if(string[0] == '#')
	{
		for(new i = strlen(string) - 1, d = 1; i > 0; --i)
		{
			if('0' <= string[i] <= '9')
			{
				userid += d * (string[i] - '0');
				d *= 10;
			}
			else 
			{
				userid = 0;
				break;
			}
		}
		if(userid)
		{
			foreach(new i : Player)
			{
				if(Character_GetID(i) == userid)
					return i;
			}
		}
	}
	else
	{
		if(IsNumeric(string))
		{
			userid = strval(string);
			if(IsPlayerConnected(userid))
				return userid;
		}
		new name[MAX_PLAYER_NAME];
		foreach(new i : Player)
		{
			GetPlayerName(i, name, MAX_PLAYER_NAME);
			if(strfind(name, string, true) == 0)
		        return i;
		}
	}
	return INVALID_PLAYER_ID;
}

SSCANF:item(string[])
{
	// probably an ID
	if('0' <= string[0] <= '9')
	{
		new ret = strval(string);
		if(0 <= ret <= MAX_ITEMS_IN_SERVER)
		{
			return ret;
		}
	}
	else
	{
		foreach(new item : ServerItems)
		{
			if(!strcmp(ServerItem[item][sitemName], string, true) || strfind(ServerItem[item][sitemName], string, true) > -1)
				return item;
		}
	}
    return INVALID_ITEM_ID;
}

new stock shifthour;
stock FixHour(hour)
{
	hour = 2+hour;
	if (hour < 0)
	{
		hour = hour+24;
	}
	else if (hour > 23)
	{
		hour = hour-24;
	}
	shifthour = hour;
	return 1;
}

stock IsPlayerIDConnected(dbid)
{
	foreach(new i : Player)
	{
		if(Character_GetID(i) == dbid)
			return i;
	}
	return INVALID_PLAYER_ID;
}

#include <textdraws.pwn>

// ========== [ INCLUDES THAT DOESN'T CARE ABOUT HOOKING ORDER ] ==========
#include <mailer_system\core.pwn>
#include <log_system\core.pwn>

#include <account_system\core.pwn>
#include <animation_system\core.pwn>

// ===== [ PLAYER ] =====
#include <player\core.pwn>

// ===== [ VEHICLE SYSTEM ] =====
#include <vehicles\core.pwn>
#include <vehicles\inventory.pwn>

// ===== [ DEALERSHIP SYSTEM ] =====
#include <dealership\core.pwn>
#include <dealership\player.pwn>

// ===== [ HOUSE SYSTEM ] =====
#include <house_system\core.pwn>
#include <house_system\inventory.pwn>
// ===== [ BUILDING SYSTEM ] =====
#include <building\core.pwn>


// ===== [ ADMIN SYSTEM ] =====
#include <admin\core.pwn>

// ===== [ DROP SYSTEM ] =====
#include <drop_system\core.pwn>

// ===== [ FACTION SYSTEM ] =====
#include <faction_system\core.pwn>

// ===== [ WEATHER SYSTEM ] =====
#include <weather_system\core.pwn>

// ===== [ CHOPSHOP SYSTEM ] =====
#include <chopshop_system\core.pwn>
#include <chopshop_system\commands.pwn>

// ========== [ COMMANDS ] ==========
#include <commands.pwn>
#include <player\commands.pwn>
#include <building\commands\admin.pwn>
#include <house_system\commands.pwn>
#include <admin\commands.pwn>
#include <admin\supporter_commands.pwn>
#include <dealership\commands.pwn>
#include <faction_system\commands.pwn>
#include <animation_system\commands.pwn>
// ========== [ DIALOGS ] ==========
#include <player\dialogs.pwn>
#include <account_system\dialogs.pwn>
#include <faction_system\dialogs.pwn>

// ========== [ MISCELLANEOUS ] ==========
#if defined ENABLE_MAPS
	#include <server\maps\maps.pwn>
#endif