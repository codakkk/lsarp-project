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

//#define FAKE_LOGIN 1

#include <a_samp>
native IsValidVehicle(vehicleid);

#define CGEN_MEMORY 20000

//#define DEBUG 0

#define E_MAIL_CHECK

#undef MAX_PLAYERS
#define MAX_PLAYERS	(200)

#if defined CRASHDETECT
	#include <crashdetect>
#endif

#define FIXES_Single
// #define FIX_OnDialogResponse 1
#define FIX_SetPlayerName 0
#define FIX_ServerVarMsg 0
#include <fixes>

// Includes
#include <sscanf2>
#include <a_mysql>

#include <timerfix>

#include <YSI_Coding\y_timers>
#include <YSI_Coding\y_va>
#include <YSI_Coding\y_inline>
#include <YSI_Data\y_bit>

// For YSI and PawnPlus yield conflict.
#undef yield
#undef @@
#include <Pawn.CMD>
#include <whirlpool>
#include <streamer>
//#include <strlib>

#define PP_SYNTAX 1
//#define PP_SYNTAX_GENERIC 1
#define PP_ADDITIONAL_TAGS E_ITEM_DATA, Text3D, Pool
#include <PawnPlus>
// #include <pp-mysql> // Must update pp first
#include <OPA>

#include <miscellaneous\pp_wrappers>
#include <easyDialogs>

#include <sa_zones>

DEFINE_HOOK_REPLACEMENT(ShowRoom, SR);
DEFINE_HOOK_REPLACEMENT(Element, Elm);
DEFINE_HOOK_REPLACEMENT(Player, Ply);
DEFINE_HOOK_REPLACEMENT(Downloading, Dwnling);

#include <YSI_Coding\y_hooks> // Needed for NexAC

#include <nex-ac_it.lang>
#include <nex-ac>

#define AC_GetPlayerHealth AntiCheatGetHealth
#define AC_SetPlayerHealth SetPlayerHealth

#define AC_GetPlayerArmour AntiCheatGetArmour
#define AC_SetPlayerArmour SetPlayerArmour

#include <defines>
#include <utils\iterators>
#include <forwarded_functions>
#include <miscellaneous\timestamp_to_date>

#include <miscellaneous\globals>
// https://github.com/emmet-jones/New-SA-MP-callbacks/blob/master/README.md
// Exception. Must be on top of all others.
#include <pickup\enum>

//#include <anticheat\enum>
#include <account_system\enum>
#include <drop_system\enum>
#include <player\enum>
#include <vehicles\enum>
#include <admin\enum>
#include <dealership\enum>
#include <inventory\enum>
#include <building\enum>
#include <weapon_system\enum>
#include <house_system\enum>
#include <faction_system\enum>
#include <dp_system\enum>

#include <database\core>

// ===== [ ANTI-CHEAT SYSTEM ] =====
//#include <anticheat\core>

// ===== [ PICKUP SYSTEM ] =====
#include <pickup\core>

// ===== [ INVENTORY SYSTEM ] =====
#include <inventory\core>
#include <inventory\server>

// ===== [ WEAPON SYSTEM ] =====
#include <weapon_system\core>

// ===== [ DP SYSTEM ] =====
#include <dp_system\core>

#include <utils\utils>
#include <utils\maths>


#include <callbacks>

//#include <anticheat\cheats\triggerbot>

#include <YSI_Coding\y_hooks> // Place hooks after this. Everything included before this, is hooked first.

CMD:resetwe(playerid, params[])
{
	ResetPlayerWeapons(playerid);
	return 1;
}

WasteDeAMXersTime()
{
    new b;
    #emit load.pri b
    #emit stor.pri b
}

main()
{
	WasteDeAMXersTime();
	printf("LSARP - By CodaKKK. Started: 26/02/2019.");

	// Should I initialize them in a OnGameModeInit hook?
	PlayerInventory = map_new();
	VehicleInventory = map_new();
	HouseList = list_new();
	HouseInventory = map_new();
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
	if(IsPlayerNPC(playerid))
		return 1;
	
	if(killerid != INVALID_PLAYER_ID)
		Log(Character_GetOOCName(playerid), Character_GetOOCName(killerid), "OnPlayerDeath", reason);
	
	SetPlayerDrunkLevel(playerid, 0);
	if(pAnimLoop{playerid})
	{
		pAnimLoop{playerid} = false;
		TextDrawHideForPlayer(playerid, txtAnimHelper);
	}
	printf("OnPlayerDeath");
	//CallLocalFunction(#OnCharacterDeath, "ddd", playerid, killerid, reason);
	return 1;
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


/*hook OnAntiCheatDetected(playerid, code)
{
	SendMessageToAdmins(true, COLOR_ERROR, "[ADMIN-ALERT]: %s (%d) è sospetto di hack. (%s)", Character_GetOOCName(playerid), playerid, AC_Name[code]);
	return 1;
}
*/
hook OnVehicleMod(playerid, vehicleid, componentid)
{
	RemoveVehicleComponent(vehicleid, componentid);
	return 0;
}

hook OnPlayerExitVehicle(playerid, vehicleid)
{
	return Y_HOOKS_CONTINUE_RETURN_1;
}

public OnPlayerCommandReceived(playerid, cmd[], params[], flags)
{
	if(!Character_IsLogged(playerid))
		return 0;
	printf("Packet Loss: %f", NetStats_PacketLossPercent(playerid));
	if(NetStats_PacketLossPercent(playerid) > 2.0)
	{
		SendClientMessage(playerid, COLOR_ERROR, "Il tuo livello di pacchetti persi risulta elevato.");
		SendClientMessage(playerid, COLOR_ERROR, "Pertanto, alcuni comandi ti sono stati bloccati.");
		return 0;
	}
	/*printf("%s", cmd);
	static const banned_commands[] = {
		"/.spam","/r2info","/pos","/placebomb","/detonate","/fakenick", "/fakeskin","/ro","/follow","/safk","/bcinfo","/tpc","/skr","/be_chat","/be_anon","/be_id","/be_name","/name","/connect",
		"/servers","/ct.slap","/ct.up","/ct.down","/ct.ray","/ct.gh","/vert","/reaper","/training","/troff","/tr","/fr","/.shotrepeater","/.pos","/.flip","/ecol","/tb.cmds","/cfake","/usm","/gjp",
		"/tc","/find","/cbug","/tb.killall","/tb.slag","/tb.fire","/tb.panic","/tb.flip","/tb.fly","/tb.load","/tb.pop","/tb.color","/tb.ocean","/tb.jack","/tb.tpto","/tb.heaven","/tb.kick",
		"/tb.ground","/tb.kill","/tb.magnet","/tb.random","/cc","/stopfind","/master","/hmo","/setampl","/kill","/ton","/off","/non","/noff","/spy","/incar","/weap","/sc","/so","/react","/fd","/fb",
		"/go","/coords","/note","/line","/get","/set","/top","/dgun","/.slp","/.slpv","/.tel","/slapall","/eject","/st","/kc","/rel","/target","/rfast","/rind","/aim","/.addfriend","/.delfriend","/.friends","/tinfo","/trollinfo","/dpos","/r2","/fw","/ccars","/hp","/hpu","/tp","/sm","/showid","/showcarid","/s1","/sswitch","/lag","/col","/fwh","/bm","/reu","/tpall","/hpall","/thr","/small","/smallc","/safe","/ohelp","/oy","/or","/ob","/ov","/og","/ow","/oreset","/opos","/osize","/oalign","/oshowid","/.rec","/.stoprec","/.play","/.stop","/fstop","/sd","/sdpos","/sdsize","/sdalign","/sdflags","/sdzone","/sdping","/sdfps","/aoff","/aafk","/.silentaim","/copdetect","/vhfinder","/vb","/vehiclebugger","/akb","/fdeagle","/wpinfo","/spec","/cardance","/vhinfo","/spinfo","/skin_hack","/ts.troll","/ts.me","/ts.pin","/invis","/rpc","/pkt","/ppoint","/ptracers","/schat","/clearc","/pskin","/csk","/wc","/set_chat","/set_time","/vers","/.e","/visionhelp","/vision1","/vision2","/.cl","/invdon","/.bot_say","/.bot_anim","/.bot_fanim","/gbc","/ak","/.ra","/.range","/.allowwep","/myfind","/matrunbot",
		"/matrunbothelp","/refresh","/vehiclerecordingloop","/rpay","/kcmds","/hme","/kme","/ufme","/spme","/team","/act","/up","/down","/irun","/fx","/fp","/mynrg","/antibf","/ccolor","/scars","/pcars","/cwater","/spack","/fpack","/ppack","/getw","/ammo","/rw","/rwall","/.ls","/.sf","/.lv","/market","/sl","/lp","/cash","/pinfo","/noint","/fdeath","/fdamage","/.time","/.weather","/rejoin","/be","/nametag","/splayers","/.cc","/ntp","/dnt","/dhp","/awr","/trollskin","/whack","/.w1","/sp","/carc","/carcontrol","/gcc","/cboom","/js","/sfucker","/wh","/afix","/mini","/checkcon","/cp","/setcarhp","/si","/sipos","/sisize","/simax","/uc","/cg","/cgpos","/cgsize","/cgline","/vape","/bcm","/chp","/spc","/coordz","/chelp","/cfind","/cfind2","/creset","/cpos","/csize","/calign","/togptb","/nodrive","/nodrive_looplength","/cn","/cnt","/cnd","/csload","/.comandos","/.mark","/.tp","/.jack","/.up","/.down","/.hme","/.ame","/.gint","/.sint","/.fix","/.getw","/lockstatus","/ireload","/infobar","/iset","/k","/aimka","/df","/witch","/ms","/qq","/tcp","/ggun","/ttp","/setclass","/scar","/sskin","/fkill","/nick","/bs_func","/bs_cmd","/akill","/br","/fskin","/warm","/whelp","/setwep","/slp","/lagon","/lagoff","/adeath","/dstop","/plines","/bc","/bcc","/ctp","/crp","/mrp","/cinfo","/pid",
		"/cid","/kicker","/attack","/chudpos","/pz_radius","/pz","/gravon","/gravoff","/cgrav","/dgrav","/gravhelp","/hud","/sk","/take","/troll","/write","/co","/.sp","/.w","/rs","/.420","/.fz","/gkick","/nospread","/checkcode","/getmats","/oncemats","/massmatson","/massmatsoff","/goback","/matsmasterhelp","/.goto","/ck","/mow","/kp","/4find","/carhelp","/vr","/ah","/chase","/gun","/ph","/vh","/gc","/cb","/jack","/pjack","/mondie","/x","/autosay","/autosaystop","/autosayreset","/setdelay","/setpoint","/antiafk","/dk","/gh","/sob","/reconnect","/spawncars","/fastspawn","/fuckcars","/ftext","/bubble","/.setcolor","/controlcar","/raininfo","/rain","/rainfew","/count","/rainpos","/carpos","/delpos","/.godmodeon","/.godmodeoff","/clearchate","/editpi","/editpic","/pimod","/.spec","/commander","/getplayerid","/getcarid","/getcoord","/autoshot","/frieze","/stan","/load","/tf","/mp3on","/sgm","/cartp","/carclear","/relog","/fcord","/ncmd","/c1save","/c2save","/c3save","/c4save","/c5save","/c1load","/c2load","/c3load","/c4load","/c5load","/crasher","/mq","/pickup","/rem","/maprecord","/vehiclerecording","/gping",
		"/c.showcmds","/c.changeserver","/c.currentserver","/c.pickup","/c.skin","/c.name","/c.warp","/c.fakekill","/c.infotr","/c.pos","/c.rs","/c.gskin","/c.uncar","/.cmds","/.friend","/.delfriends","/.ik","/.ib","/.cr","/.c","/.troll","/.pmall","/.fps","/.sd","/.sd2","/.crash","/.bot_ping","/.fake_fps","/.ol_cmds","/.find_chat","/.stop_find","/.bot_nick","/.bot_steal_nick","/.add_friend","/.stick","//stick","//warp","//adm","//friend","//rename","//slap","//skin","//bot_connect","//bot_disconnect","/.addm","/.addf","/cserv","/sendpic","/fakekill","/warp","/vadd","/fpay","/dkick","/gcheck","/boom","/tpm","/vhadd","/retime","/dialoghide","/del","/paint","/getcar","/sendcheck","/shp","/gguns","/gotocheck","/fcuff","/bbiz","/.nick","/.rnick","/.sortmoney","/.search","/.warp","/.pmcrash","/.aimfriend","/.delaimfriend","/.delaimfriends","/.derpcar","/.minigun","/.sendcars","/.incar","/.vsearch","/.vssearch","/.cc_all","/.del_all","/.onfire_all","/.connectbot","/.removebot","/.bot_add","/.bot_remove","/.bot_removeall","/.npc","/.exploit","/clmrad","/ptroll","/carslap","/toggle","/nigger","/fakebulletsync","/syncinvi","/invi","/crashdebug","/crash","/agm","/recfg","/loading","/afakedamage","/togpl","/acheats.ru","/rend","/topic","/toobj","/toveh","/spawncar","/airb","/airp","/pr","/spam","/fkick","/flood","/cm_help","/cm_tp","/cm_tcar","/spectate","/hdialog","/shot","/stazer","/fcheck","/paintjob","/fpick","/ghere","/vehadd","/ghp","/svhp","/ccw","/ucw","/parsenicks","/coordcw","/takecar","/stcar","/lagtroll"
		};
	if(strlen(cmd) >= 3)
	{
		new f_cmd[32];
		format(f_cmd, sizeof(f_cmd), "/%s", cmd);
		for(new i = 0, j = sizeof(banned_commands); i < j; ++i)
		{
			if(!strcmp(f_cmd, banned_commands[i]))
			{
				SendMessageToAdmins(true, COLOR_ADMIN, "[ADMIN-ALERT] Il giocatore %s (%d) ha utilizzato un comando proibito. Comando: %s.", Character_GetOOCName(playerid), playerid, cmd);
				return 0;
			}
		}
	}*/
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
	gLoginAttempts{playerid} = 0;
	
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

	#if defined FAKE_LOGIN
		Account_SetLogged(playerid, true);
		Character_SetLogged(playerid, true);
		SpawnPlayer(playerid);
	#endif
	//OnPlayerRequestClass(playerid, 0);
	return 1;
}

public OnPlayerFinishedDownloading(playerid, virtualworld)
{
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

#include <textdraws>

// ========== [ INCLUDES THAT DOESN'T CARE ABOUT HOOKING ORDER ] ==========
#include <mailer_system\core>
#include <log_system\core>

#include <account_system\core>
#include <animation_system\core>

// ===== [ PLAYER ] =====
#include <player\core>
#include <anticheat\cheats\money>

// ===== [ VEHICLE SYSTEM ] =====
#include <vehicles\core>
#include <vehicles\inventory>

// ===== [ DEALERSHIP SYSTEM ] =====
#include <dealership\core>
#include <dealership\player>

// ===== [ HOUSE SYSTEM ] =====
#include <house_system\core>
#include <house_system\inventory>
// ===== [ BUILDING SYSTEM ] =====
#include <building\core>

// ===== [ ADMIN SYSTEM ] =====
#include <admin\core>

// ===== [ DROP SYSTEM ] =====
#include <drop_system\core>

// ===== [ FACTION SYSTEM ] =====
#include <faction_system\core>

// ===== [ WEATHER SYSTEM ] =====
#include <weather_system\core>

// ===== [ CHOPSHOP SYSTEM ] =====
#include <chopshop_system\core>
#include <chopshop_system\commands>

// ===== [ FURNITURE SYSTEM ] =====
#include <furniture_system\core_map>

// ========== [ COMMANDS ] ==========
#include <commands>
#include <player\commands>
#include <building\commands\admin>
#include <house_system\commands>
#include <admin\commands>
#include <admin\supporter_commands>
#include <dealership\commands>
#include <animation_system\commands>
// ========== [ DIALOGS ] ==========
#include <player\dialogs>
#include <account_system\dialogs>
#include <faction_system\dialogs>

// ========== [ MISCELLANEOUS ] ==========
#if defined ENABLE_MAPS
	#include <server\maps\maps>
#endif



stock AC_ResetPlayerWeapons(playerid, bool:deleteFromDatabase = false)
{
	ResetPlayerWeapons(playerid);
	if(deleteFromDatabase)
		Character_DeleteAllWeapons(playerid);
	return 1;
}