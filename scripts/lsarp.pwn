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

*/
#pragma warning disable 208 // actually just a good way to prevent warning: "function with tag result used before definition, forcing reparse".
#include <includes.pwn>
#include <defines.pwn>
#include <forwarded_functions.pwn>

DEFINE_HOOK_REPLACEMENT(ShowRoom, SR);
DEFINE_HOOK_REPLACEMENT(Element, Elm);

#include <miscellaneous\globals.pwn>

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
#include <weather_system\definitions.pwn>


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

	Building_LoadAll();
	House_LoadAll();

	
	return 1;
}

hook OnPlayerUpdate(playerid)
{
	if(IsPlayerNPC(playerid)) return Y_HOOKS_BREAK_RETURN_1;
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerDeath(playerid, killerid, reason) 
{
	if(pAnimLoop{playerid})
	{
		pAnimLoop{playerid} = false;
		TextDrawHideForPlayer(playerid, txtAnimHelper);
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}
hook OnPlayerRequestSpawn(playerid) return 1;
hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) return 1;
hook OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ) 
{
	if(IsPlayerNPC(playerid))
		return Y_HOOKS_BREAK_RETURN_1;
	//if( !( -1000.0 <= fX <= 1000.0 ) || !( -1000.0 <= fY <= 1000.0 ) || !( -1000.0 <= fZ <= 1000.0 ) )
		//return 0;
	// If the server isn't performing well, updates to this callback will be
	// delayed and could stack up resulting in a sudden mass-call of this
	// callback which can cause false positives.
	// More research needed into this though as player lag can also cause this,
	// possibly a ping check or packet loss check would work.
	
	
	return 1;
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
	if(!gAccountLogged[playerid] || !Character_IsLogged(playerid))
		return 0;
	if(flags & CMD_USER)
	{
		if( (Character_IsJailed(playerid) && !Character_IsICJailed(playerid)) || Character_IsDead(playerid))
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando.");
			return 0;
		}
	}
	if(AccountInfo[playerid][aAdmin] <= 0)
	{
		if(flags & CMD_PREMIUM_BRONZE && AccountInfo[playerid][aPremium] < 1)
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando!");
			return 0;
		}
		else if(flags & CMD_PREMIUM_SILVER && AccountInfo[playerid][aPremium] < 2)
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando!");
			return 0;
		}
		else if(flags & CMD_PREMIUM_SILVER && AccountInfo[playerid][aPremium] < 3)
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando!");
			return 0;
		}
	}

	new factionid = Character_GetFaction(playerid);
	if(flags & CMD_POLICE)
	{
		if(factionid == -1 || Faction_GetType(factionid) != FACTION_TYPE_POLICE || Character_IsDead(playerid))
		{
			SendClientMessage(playerid, COLOR_ERROR, "Devi essere un membro della Guardia Nazionale in servizio per utilizzare questo comando.");
			return 0;
		}
	}
	else if(flags & CMD_MEDICAL)
	{
		if(factionid == -1 || Faction_GetType(factionid) != FACTION_TYPE_MEDICAL || Character_IsDead(playerid))
		{
			SendClientMessage(playerid, COLOR_ERROR, "Devi essere un medico in servizio per poter utilizzare questo comando!");
			return 0;
		}
	}
	else if(flags & CMD_GOVERNMENT)
	{
		if(factionid == -1 || Faction_GetType(factionid) != FACTION_TYPE_GOVERNAMENT || Character_IsDead(playerid))
		{
			SendClientMessage(playerid, COLOR_ERROR, "Devi essere un membro del Governo in servizio per utilizzare questo comando.");
			return 0;
		}
	}
	else if(flags & CMD_ILLEGAL)
	{
		if(factionid == -1 || (Faction_GetType(factionid) != FACTION_TYPE_IMPORT_WEAPONS && Faction_GetType(factionid) != FACTION_TYPE_IMPORT_DRUGS) || Character_IsDead(playerid))
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando!");
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
		SendClientMessage(playerid, -1, "Il comando inserito non esiste. Digita \"{00FF00}/aiuto{FFFFFF}\" per una lista di comandi.");
		return 0;
	}
	return 1;
}

hook OnPlayerText(playerid, text[])
{
	if(!Character_IsLogged(playerid) || isnull(text))
		return Y_HOOKS_BREAK_RETURN_0;
	if(pAdminDuty[playerid])
	{
		pc_cmd_b(playerid, text);
	}
	else
	{
		if(Character_IsDead(playerid))
			return 0;
		new String:string = str_format("%s dice: %s", Character_GetOOCName(playerid), text);
		ProxDetectorStr(playerid, 15.0, string, COLOR_FADE1, COLOR_FADE2, COLOR_FADE3, COLOR_FADE4, COLOR_FADE5);
	}
	return Y_HOOKS_CONTINUE_RETURN_0;
}

hook OnPlayerDisconnect(playerid, reason)
{
	new const reasonName[3][16] = {"Crash", "Uscito", "Kick/Ban"};
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
	
	gAccountLogged[playerid] = 0;
	gCharacterLogged[playerid] = 0;
	return 1;
}

// This is the last callback called after the hooks.
hook OnPlayerConnect(playerid)
{
	wait_ticks(1);
	
	SetPlayerColor(playerid, 0xFFFFFFFF);

	gAccountLogged[playerid] = 0;
	gCharacterLogged[playerid] = 0;
	gLoginAttempts[playerid] = 0;

	for(new i = 0; i < 30; ++i)
	{
		SendClientMessage(playerid, -1, " ");
	}
	SendClientMessage(playerid, COLOR_YELLOW, "________________________________________________________________");
	SendClientMessage(playerid, COLOR_YELLOW, "Benvenuto su Los Santos Apocalypse Roleplay [www.lsarp.it]!");
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

	if(count > 10)
	{
		Ban(playerid);
		format(cmd, sizeof cmd, "banip %s", ip);
		SendRconCommand(cmd);
	}
	LoadPlayerTextDraws(playerid);
	SendClientMessage(playerid, -1, "Hai 60 secondi per effettuare registrarti o effettuare il login prima di essere kickato.");
	//OnPlayerRequestClass(playerid, 0);
	return 1;
}

#include <textdraws.pwn>

// ========== [ INCLUDES THAT DOESN'T CARE ABOUT HOOKING ORDER ] ==========
#include <mailer_system\core.pwn>
#include <log_system\core.pwn>

#include <account_system\core.pwn>
#include <animation_system\core.pwn>

// ===== [ PLAYER ] =====
#include <player\core.pwn>
#include <player\inventory.pwn>
#include <player\drop.pwn>
#include <player\textdraws.pwn>


// ===== [ VEHICLE SYSTEM ] =====
#include <vehicles\core.pwn>
#include <vehicles\inventory.pwn>

// ===== [ DEALERSHIP SYSTEM ] =====
#include <dealership\core.pwn>
#include <dealership\player.pwn>

// ===== [ HOUSE SYSTEM ] =====
#include <house_system\core.pwn>

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
// #include <miscellaneous\maps.pwn>