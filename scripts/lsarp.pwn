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
#define ALLOW_NEW_USERS 0

#if defined LSARP_DEBUG
	#warning LSARP_DEBUG is enabled. Care!!
#endif

//#define FAKE_LOGIN 1
// CAR_DEAD_LHS
#include <a_samp>
native IsValidVehicle(vehicleid);

#define FIXES_Single
#define FIX_const 0
// #define FIX_OnDialogResponse 1
#define FIX_SetPlayerName 0
#define FIX_ServerVarMsg 0
#define FIX_ClearAnimations_2 0 // Fixes a bug of autowalking anim

//#include <fixes>
#include <sscanf2>
#include <SKY>


#define CGEN_MEMORY 20000

//#define DEBUG 0

#define E_MAIL_CHECK

#undef MAX_PLAYERS
#define MAX_PLAYERS	(200)


#define CRASHDETECT
#if defined CRASHDETECT
	#include <crashdetect>
#endif

// Includes

#include <a_mysql>

//#include <timerfix>

#include <YSI_Coding\y_timers>
#include <YSI_Coding\y_va>
#include <YSI_Coding\y_inline>
#include <YSI_Data\y_bit>
#include <YSI_Coding\y_stringhash>
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

#define WC_DEBUG_SILENT false
#include <weapon-config>

#include <YSI_Coding\y_hooks> // Needed for NexAC

#include <nex-ac_it.lang>

#define AC_USE_VENDING_MACHINES		false
#define AC_USE_TUNING_GARAGES		false
#define AC_USE_PICKUP_WEAPONS		false
#define AC_USE_AMMUNATIONS			false
#define AC_USE_RESTAURANTS			false
#define AC_USE_PAYNSPRAY			true
#define AC_USE_CASINOS				false
#define AC_USE_NPC					false
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

#include <server\core>
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

// ===== [ CHECKPOINT SYSTEM ] =====
#include <checkpoint_system\core>

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


main()
{
	//WasteDeAMXersTime();
	printf("LSARP - By CodaKKK. Started: 26/02/2019.");
	// Should I initialize them in a OnGameModeInit hook?
	PlayerInventory = map_new();
	VehicleInventory = map_new();
	HouseInventory = map_new();
}


hook OnGameModeInit() 
{
	SetGameModeText("ApocalypseV1");
	SendRconCommand("query 1"); // Just safeness
	ShowPlayerMarkers(PLAYER_MARKERS_MODE_OFF);
	SetNameTagDrawDistance(20.0);
	DisableInteriorEnterExits();
	ManualVehicleEngineAndLights(); 
	EnableStuntBonusForAll(false);

	SetDamageFeed(false);
	SetDamageSounds(0, 0);

	SetWorldTime(0);

	// /ritirastipendio
	Pickup_Create(1239, 0, 292.3752, 180.7307, 1007.1790, ELEMENT_TYPE_PAYCHECK, -1, 3);
	CreateDynamic3DTextLabel("/ritirastipendio", COLOR_BLUE, 292.3752, 180.7307, 1007.1790 + 0.55, 20.0, .worldid = -1, .interiorid = 3);
	// /lasciacarcere
	Pickup_Create(1239, 0, 2649.7790, -1948.9510, -58.7273, ELEMENT_TYPE_JAIL_EXIT, .worldid = -1, .interiorid = 0);
	CreateDynamic3DTextLabel("/lasciacarcere", COLOR_BLUE, 2649.7790, -1948.9510, -58.7273 + 0.55, 20.0, .worldid = -1, .interiorid = 0);

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

public OnPlayerDeath(playerid, killerid, reason)
{
	printf("PlayerDeath");
	if(IsPlayerNPC(playerid))
		return 1;
	
	if(killerid != INVALID_PLAYER_ID)
		Log(Character_GetOOCName(playerid), Character_GetOOCName(killerid), "OnPlayerDeath", reason);
	
	SetPlayerDrunkLevel(playerid, 0);
	if(Character_IsAnimLoop(playerid))
	{
		Character_SetAnimLoop(playerid, false);
		TextDrawHideForPlayer(playerid, txtAnimHelper);
	}

	// Just a workaround for unable to hook OnPlayerDeath with weaponconfig
	CallLocalFunction(#OnCharacterDeath, "ddd", playerid, killerid, reason);
	printf("lsarp.pwn\\OnPlayerDeath");
	//CallLocalFunction(#OnCharacterDeath, "ddd", playerid, killerid, reason);
	return 1;
}

forward OnCharacterDamageDone(playerid, Float:amount, issuerid, weaponid, bodypart);


public OnPlayerDamageDone(playerid, Float:amount, issuerid, weapon, bodypart)
{
	DamageSystem_Update(playerid, amount, issuerid, weapon, bodypart);
	CallLocalFunction(#OnCharacterDamageDone, "dfddd", playerid, amount, issuerid, weapon, bodypart);
	return 1;
}

public OnPlayerPrepareDeath(playerid, animlib[32], animname[32], &anim_lock, &respawn_time)
{
	return 1;
}

public OnPlayerDamage(&playerid, &Float:amount, &issuerid, &weapon, &bodypart)
{
	if(Account_IsAdminDuty(playerid) && AccountInfo[playerid][aAdmin] > 1)
		return 0;
	if(Character_IsDead(playerid))
		return 0;
	if(issuerid != INVALID_PLAYER_ID)
	{
		if(weapon == 23 && Character_HasTaser(issuerid))
			amount = 3;
		else if(weapon == 25 && Character_HasBeanBag(issuerid))
			amount = 5;
	}
	return 1;
}

public OnPlayerDeathFinished(playerid, bool:cancelable)
{
	CallLocalFunction(#OnCharDeathFinished, "d", playerid);
	if(Character_IsInjured(playerid) || Character_IsDead(playerid))
		return 0;
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
	if(hittype == BULLET_HIT_TYPE_PLAYER && hitid != INVALID_PLAYER_ID && Account_IsAdminDuty(hitid))
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
	//printf("Packet Loss: %f", NetStats_PacketLossPercent(playerid));
	//if(NetStats_PacketLossPercent(playerid) > 2.0)
	if(!IsPlayerSynced(playerid) || GetPacketLoss(playerid) > 2.0)
	{
		SendClientMessage(playerid, COLOR_ERROR, "Non risulti syncato, rilogga per continuare a giocare.");
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
	/*switch(YHash(cmdtext, false))
	{
        case _I(.,s,p,a,m):
        case _I(r,2,i,n,f,o):
        case _I(p,o,s):
        case _I(p,l,a,c,e,b,o,m,b):
        case _I(d,e,t,o,n,a,t,e):
        case _I(f,a,k,e,n,i,c,k):
        case _I(f,a,k,e,s,k,i,n):
        case _I(r,o):
        case _I(f,o,l,l,o,w):
        case _I(s,a,f,k):
        case _I(b,c,i,n,f,o):
        case _I(t,p,c):
        case _I(s,k,r):
        case _I(b,e,_,c,h,a,t):
        case _I(b,e,_,a,n,o,n):
        case _I(b,e,_,i,d):
        case _I(b,e,_,n,a,m,e):
        case _I(n,a,m,e):
        case _I(c,o,n,n,e,c,t):
        case _I(s,e,r,v,e,r,s):
        case _I(c,t,.,s,l,a,p):
        case _I(c,t,.,u,p):
        case _I(c,t,.,d,o,w,n):
        case _I(c,t,.,r,a,y):
        case _I(c,t,.,g,h):
        case _I(v,e,r,t):
        case _I(r,e,a,p,e,r):
        case _I(t,r,a,i,n,i,n,g):
        case _I(t,r,o,f,f):
        case _I(t,r):
        case _I(f,r):
        case _I(.,s,h,o,t,r,e,p,e,a,t,e,r):
        case _I(.,p,o,s):
        case _I(.,f,l,i,p):
        case _I(e,c,o,l):
        case _I(t,b,.,c,m,d,s):
        case _I(c,f,a,k,e):
        case _I(u,s,m):
        case _I(g,j,p):
        case _I(t,c):
        case _I(f,i,n,d):
        case _I(c,b,u,g):
        case _I(t,b,.,k,i,l,l,a,l,l):
        case _I(t,b,.,s,l,a,g):
        case _I(t,b,.,f,i,r,e):
        case _I(t,b,.,p,a,n,i,c):
        case _I(t,b,.,f,l,i,p):
        case _I(t,b,.,f,l,y):
        case _I(t,b,.,l,o,a,d):
        case _I(t,b,.,p,o,p):
        case _I(t,b,.,c,o,l,o,r):
        case _I(t,b,.,o,c,e,a,n):
        case _I(t,b,.,j,a,c,k):
        case _I(t,b,.,t,p,t,o):
        case _I(t,b,.,h,e,a,v,e,n):
        case _I(t,b,.,k,i,c,k):
        case _I(t,b,.,g,r,o,u,n,d):
        case _I(t,b,.,k,i,l,l):
        case _I(t,b,.,m,a,g,n,e,t):
        case _I(t,b,.,r,a,n,d,o,m):
        case _I(c,c):
        case _I(s,t,o,p,f,i,n,d):
        case _I(m,a,s,t,e,r):
        case _I(h,m,o):
        case _I(s,e,t,a,m,p,l):
        case _I(k,i,l,l):
        case _I(t,o,n):
        case _I(o,f,f):
        case _I(n,o,n):
        case _I(n,o,f,f):
        case _I(s,p,y):
        case _I(i,n,c,a,r):
        case _I(w,e,a,p):
        case _I(s,c):
        case _I(s,o):
        case _I(r,e,a,c,t):
        case _I(f,d):
        case _I(f,b):
        case _I(g,o):
        case _I(c,o,o,r,d,s):
        case _I(n,o,t,e):
        case _I(l,i,n,e):
        case _I(g,e,t):
        case _I(s,e,t):
        case _I(t,o,p):
        case _I(d,g,u,n):
        case _I(.,s,l,p):
        case _I(.,s,l,p,v):
        case _I(.,t,e,l):
        case _I(s,l,a,p,a,l,l):
        case _I(e,j,e,c,t):
        case _I(s,t):
        case _I(k,c):
        case _I(r,e,l):
        case _I(t,a,r,g,e,t):
        case _I(r,f,a,s,t):
        case _I(r,i,n,d):
        case _I(a,i,m):
        case _I(.,a,d,d,f,r,i,e,n,d):
        case _I(.,d,e,l,f,r,i,e,n,d):
        case _I(.,f,r,i,e,n,d,s):
        case _I(t,i,n,f,o):
        case _I(t,r,o,l,l,i,n,f,o):
        case _I(d,p,o,s):
        case _I(r,2):
        case _I(f,w):
        case _I(c,c,a,r,s):
        case _I(h,p):
        case _I(h,p,u):
        case _I(t,p):
        case _I(s,m):
        case _I(s,h,o,w,i,d):
        case _I(s,h,o,w,c,a,r,i,d):
        case _I(s,1):
        case _I(s,s,w,i,t,c,h):
        case _I(l,a,g):
        case _I(c,o,l):
        case _I(f,w,h):
        case _I(b,m):
        case _I(r,e,u):
        case _I(t,p,a,l,l):
        case _I(h,p,a,l,l):
        case _I(t,h,r):
        case _I(s,m,a,l,l):
        case _I(s,m,a,l,l,c):
        case _I(s,a,f,e):
        case _I(o,h,e,l,p):
        case _I(o,y):
        case _I(o,r):
        case _I(o,b):
        case _I(o,v):
        case _I(o,g):
        case _I(o,w):
        case _I(o,r,e,s,e,t):
        case _I(o,p,o,s):
        case _I(o,s,i,z,e):
        case _I(o,a,l,i,g,n):
        case _I(o,s,h,o,w,i,d):
        case _I(.,r,e,c):
        case _I(.,s,t,o,p,r,e,c):
        case _I(.,p,l,a,y):
        case _I(.,s,t,o,p):
        case _I(f,s,t,o,p):
        case _I(s,d):
        case _I(s,d,p,o,s):
        case _I(s,d,s,i,z,e):
        case _I(s,d,a,l,i,g,n):
        case _I(s,d,f,l,a,g,s):
        case _I(s,d,z,o,n,e):
        case _I(s,d,p,i,n,g):
        case _I(s,d,f,p,s):
        case _I(a,o,f,f):
        case _I(a,a,f,k):
        case _I(.,s,i,l,e,n,t,a,i,m):
        case _I(c,o,p,d,e,t,e,c,t):
        case _I(v,h,f,i,n,d,e,r):
        case _I(v,b):
        case _I(v,e,h,i,c,l,e,b,u,g,g,e,r):
        case _I(a,k,b):
        case _I(f,d,e,a,g,l,e):
        case _I(w,p,i,n,f,o):
        case _I(s,p,e,c):
        case _I(c,a,r,d,a,n,c,e):
        case _I(v,h,i,n,f,o):
        case _I(s,p,i,n,f,o):
        case _I(s,k,i,n,_,h,a,c,k):
        case _I(t,s,.,t,r,o,l,l):
        case _I(t,s,.,m,e):
        case _I(t,s,.,p,i,n):
        case _I(i,n,v,i,s):
        case _I(r,p,c):
        case _I(p,k,t):
        case _I(p,p,o,i,n,t):
        case _I(p,t,r,a,c,e,r,s):
        case _I(s,c,h,a,t):
        case _I(c,l,e,a,r,c):
        case _I(p,s,k,i,n):
        case _I(c,s,k):
        case _I(w,c):
        case _I(s,e,t,_,c,h,a,t):
        case _I(s,e,t,_,t,i,m,e):
        case _I(v,e,r,s):
        case _I(.,e):
        case _I(v,i,s,i,o,n,h,e,l,p):
        case _I(v,i,s,i,o,n,1):
        case _I(v,i,s,i,o,n,2):
        case _I(.,c,l):
        case _I(i,n,v,d,o,n):
        case _I(.,b,o,t,_,s,a,y):
        case _I(.,b,o,t,_,a,n,i,m):
        case _I(.,b,o,t,_,f,a,n,i,m):
        case _I(g,b,c):
        case _I(a,k):
        case _I(.,r,a):
        case _I(.,r,a,n,g,e):
        case _I(.,a,l,l,o,w,w,e,p):
        case _I(m,y,f,i,n,d):
        case _I(m,a,t,r,u,n,b,o,t):
        case _I(m,a,t,r,u,n,b,o,t,h,e,l,p):
        case _I(r,e,f,r,e,s,h):
        case _I(v,e,h,i,c,l,e,r,e,c,o,r,d,i,n,g,l,o,o,p):
        case _I(r,p,a,y):
        case _I(k,c,m,d,s):
        case _I(h,m,e):
        case _I(k,m,e):
        case _I(u,f,m,e):
        case _I(s,p,m,e):
        case _I(t,e,a,m):
        case _I(a,c,t):
        case _I(u,p):
        case _I(d,o,w,n):
        case _I(i,r,u,n):
        case _I(f,x):
        case _I(f,p):
        case _I(m,y,n,r,g):
        case _I(a,n,t,i,b,f):
        case _I(c,c,o,l,o,r):
        case _I(s,c,a,r,s):
        case _I(p,c,a,r,s):
        case _I(c,w,a,t,e,r):
        case _I(s,p,a,c,k):
        case _I(f,p,a,c,k):
        case _I(p,p,a,c,k):
        case _I(g,e,t,w):
        case _I(a,m,m,o):
        case _I(r,w):
        case _I(r,w,a,l,l):
        case _I(.,l,s):
        case _I(.,s,f):
        case _I(.,l,v):
        case _I(m,a,r,k,e,t):
        case _I(s,l):
        case _I(l,p):
        case _I(c,a,s,h):
        case _I(p,i,n,f,o):
        case _I(n,o,i,n,t):
        case _I(f,d,e,a,t,h):
        case _I(f,d,a,m,a,g,e):
        case _I(.,t,i,m,e):
        case _I(.,w,e,a,t,h,e,r):
        case _I(r,e,j,o,i,n):
        case _I(b,e):
        case _I(n,a,m,e,t,a,g):
        case _I(s,p,l,a,y,e,r,s):
        case _I(.,c,c):
        case _I(n,t,p):
        case _I(d,n,t):
        case _I(d,h,p):
        case _I(a,w,r):
        case _I(t,r,o,l,l,s,k,i,n):
        case _I(w,h,a,c,k):
        case _I(.,w,1):
        case _I(s,p):
        case _I(c,a,r,c):
        case _I(c,a,r,c,o,n,t,r,o,l):
        case _I(g,c,c):
        case _I(c,b,o,o,m):
        case _I(j,s):
        case _I(s,f,u,c,k,e,r):
        case _I(w,h):
        case _I(a,f,i,x):
        case _I(m,i,n,i):
        case _I(c,h,e,c,k,c,o,n):
        case _I(c,p):
        case _I(s,e,t,c,a,r,h,p):
        case _I(s,i):
        case _I(s,i,p,o,s):
        case _I(s,i,s,i,z,e):
        case _I(s,i,m,a,x):
        case _I(u,c):
        case _I(c,g):
        case _I(c,g,p,o,s):
        case _I(c,g,s,i,z,e):
        case _I(c,g,l,i,n,e):
        case _I(v,a,p,e):
        case _I(b,c,m):
        case _I(c,h,p):
        case _I(s,p,c):
        case _I(c,o,o,r,d,z):
        case _I(c,h,e,l,p):
        case _I(c,f,i,n,d):
        case _I(c,f,i,n,d,2):
        case _I(c,r,e,s,e,t):
        case _I(c,p,o,s):
        case _I(c,s,i,z,e):
        case _I(c,a,l,i,g,n):
        case _I(t,o,g,p,t,b):
        case _I(n,o,d,r,i,v,e):
        case _I(n,o,d,r,i,v,e,_,l,o,o,p,l,e,n,g,t,h):
        case _I(c,n):
        case _I(c,n,t):
        case _I(c,n,d):
        case _I(c,s,l,o,a,d):
        case _I(.,c,o,m,a,n,d,o,s):
        case _I(.,m,a,r,k):
        case _I(.,t,p):
        case _I(.,j,a,c,k):
        case _I(.,u,p):
        case _I(.,d,o,w,n):
        case _I(.,h,m,e):
        case _I(.,a,m,e):
        case _I(.,g,i,n,t):
        case _I(.,s,i,n,t):
        case _I(.,f,i,x):
        case _I(.,g,e,t,w):
        case _I(l,o,c,k,s,t,a,t,u,s):
        case _I(i,r,e,l,o,a,d):
        case _I(i,n,f,o,b,a,r):
        case _I(i,s,e,t):
        case _I(k):
        case _I(a,i,m,k,a):
        case _I(d,f):
        case _I(w,i,t,c,h):
        case _I(m,s):
        case _I(q,q):
        case _I(t,c,p):
        case _I(g,g,u,n):
        case _I(t,t,p):
        case _I(s,e,t,c,l,a,s,s):
        case _I(s,c,a,r):
        case _I(s,s,k,i,n):
        case _I(f,k,i,l,l):
        case _I(n,i,c,k):
        case _I(b,s,_,f,u,n,c):
        case _I(b,s,_,c,m,d):
        case _I(a,k,i,l,l):
        case _I(b,r):
        case _I(f,s,k,i,n):
        case _I(w,a,r,m):
        case _I(w,h,e,l,p):
        case _I(s,e,t,w,e,p):
        case _I(s,l,p):
        case _I(l,a,g,o,n):
        case _I(l,a,g,o,f,f):
        case _I(a,d,e,a,t,h):
        case _I(d,s,t,o,p):
        case _I(p,l,i,n,e,s):
        case _I(b,c):
        case _I(b,c,c):
        case _I(c,t,p):
        case _I(c,r,p):
        case _I(m,r,p):
        case _I(c,i,n,f,o):
        case _I(p,i,d):
        case _I(c,i,d):
        case _I(k,i,c,k,e,r):
        case _I(a,t,t,a,c,k):
        case _I(c,h,u,d,p,o,s):
        case _I(p,z,_,r,a,d,i,u,s):
        case _I(p,z):
        case _I(g,r,a,v,o,n):
        case _I(g,r,a,v,o,f,f):
        case _I(c,g,r,a,v):
        case _I(d,g,r,a,v):
        case _I(g,r,a,v,h,e,l,p):
        case _I(h,u,d):
        case _I(s,k):
        case _I(t,a,k,e):
        case _I(t,r,o,l,l):
        case _I(w,r,i,t,e):
        case _I(c,o):
        case _I(.,s,p):
        case _I(.,w):
        case _I(r,s):
        case _I(.,4,2,0):
        case _I(.,f,z):
        case _I(g,k,i,c,k):
        case _I(n,o,s,p,r,e,a,d):
        case _I(c,h,e,c,k,c,o,d,e):
        case _I(g,e,t,m,a,t,s):
        case _I(o,n,c,e,m,a,t,s):
        case _I(m,a,s,s,m,a,t,s,o,n):
        case _I(m,a,s,s,m,a,t,s,o,f,f):
        case _I(g,o,b,a,c,k):
        case _I(m,a,t,s,m,a,s,t,e,r,h,e,l,p):
        case _I(.,g,o,t,o):
        case _I(c,k):
        case _I(m,o,w):
        case _I(k,p):
        case _I(4,f,i,n,d):
        case _I(c,a,r,h,e,l,p):
        case _I(v,r):
        case _I(a,h):
        case _I(c,h,a,s,e):
        case _I(g,u,n):
        case _I(p,h):
        case _I(v,h):
        case _I(g,c):
        case _I(c,b):
        case _I(j,a,c,k):
        case _I(p,j,a,c,k):
        case _I(m,o,n,d,i,e):
        case _I(x):
        case _I(a,u,t,o,s,a,y):
        case _I(a,u,t,o,s,a,y,s,t,o,p):
        case _I(a,u,t,o,s,a,y,r,e,s,e,t):
        case _I(s,e,t,d,e,l,a,y):
        case _I(s,e,t,p,o,i,n,t):
        case _I(a,n,t,i,a,f,k):
        case _I(d,k):
        case _I(g,h):
        case _I(s,o,b):
        case _I(r,e,c,o,n,n,e,c,t):
        case _I(s,p,a,w,n,c,a,r,s):
        case _I(f,a,s,t,s,p,a,w,n):
        case _I(f,u,c,k,c,a,r,s):
        case _I(f,t,e,x,t):
        case _I(b,u,b,b,l,e):
        case _I(.,s,e,t,c,o,l,o,r):
        case _I(c,o,n,t,r,o,l,c,a,r):
        case _I(r,a,i,n,i,n,f,o):
        case _I(r,a,i,n):
        case _I(r,a,i,n,f,e,w):
        case _I(c,o,u,n,t):
        case _I(r,a,i,n,p,o,s):
        case _I(c,a,r,p,o,s):
        case _I(d,e,l,p,o,s):
        case _I(.,g,o,d,m,o,d,e,o,n):
        case _I(.,g,o,d,m,o,d,e,o,f,f):
        case _I(c,l,e,a,r,c,h,a,t,e):
        case _I(e,d,i,t,p,i):
        case _I(e,d,i,t,p,i,c):
        case _I(p,i,m,o,d):
        case _I(.,s,p,e,c):
        case _I(c,o,m,m,a,n,d,e,r):
        case _I(g,e,t,p,l,a,y,e,r,i,d):
        case _I(g,e,t,c,a,r,i,d):
        case _I(g,e,t,c,o,o,r,d):
        case _I(a,u,t,o,s,h,o,t):
        case _I(f,r,i,e,z,e):
        case _I(s,t,a,n):
        case _I(l,o,a,d):
        case _I(t,f):
        case _I(m,p,3,o,n):
        case _I(s,g,m):
        case _I(c,a,r,t,p):
        case _I(c,a,r,c,l,e,a,r):
        case _I(r,e,l,o,g):
        case _I(f,c,o,r,d):
        case _I(n,c,m,d):
        case _I(c,1,s,a,v,e):
        case _I(c,2,s,a,v,e):
        case _I(c,3,s,a,v,e):
        case _I(c,4,s,a,v,e):
        case _I(c,5,s,a,v,e):
        case _I(c,1,l,o,a,d):
        case _I(c,2,l,o,a,d):
        case _I(c,3,l,o,a,d):
        case _I(c,4,l,o,a,d):
        case _I(c,5,l,o,a,d):
        case _I(c,r,a,s,h,e,r):
        case _I(m,q):
        case _I(p,i,c,k,u,p):
        case _I(r,e,m):
        case _I(m,a,p,r,e,c,o,r,d):
        case _I(v,e,h,i,c,l,e,r,e,c,o,r,d,i,n,g):
        case _I(g,p,i,n,g):
        case _I(c,.,s,h,o,w,c,m,d,s):
        case _I(c,.,c,h,a,n,g,e,s,e,r,v,e,r):
        case _I(c,.,c,u,r,r,e,n,t,s,e,r,v,e,r):
        case _I(c,.,p,i,c,k,u,p):
        case _I(c,.,s,k,i,n):
        case _I(c,.,n,a,m,e):
        case _I(c,.,w,a,r,p):
        case _I(c,.,f,a,k,e,k,i,l,l):
        case _I(c,.,i,n,f,o,t,r):
        case _I(c,.,p,o,s):
        case _I(c,.,r,s):
        case _I(c,.,g,s,k,i,n):
        case _I(c,.,u,n,c,a,r):
        case _I(.,c,m,d,s):
        case _I(.,f,r,i,e,n,d):
        case _I(.,d,e,l,f,r,i,e,n,d,s):
        case _I(.,i,k):
        case _I(.,i,b):
        case _I(.,c,r):
        case _I(.,c):
        case _I(.,t,r,o,l,l):
        case _I(.,p,m,a,l,l):
        case _I(.,f,p,s):
        case _I(.,s,d):
        case _I(.,s,d,2):
        case _I(.,c,r,a,s,h):
        case _I(.,b,o,t,_,p,i,n,g):
        case _I(.,f,a,k,e,_,f,p,s):
        case _I(.,o,l,_,c,m,d,s):
        case _I(.,f,i,n,d,_,c,h,a,t):
        case _I(.,s,t,o,p,_,f,i,n,d):
        case _I(.,b,o,t,_,n,i,c,k):
        case _I(.,b,o,t,_,s,t,e,a,l,_,n,i,c,k):
        case _I(.,a,d,d,_,f,r,i,e,n,d):
        case _I(.,s,t,i,c,k):
        case _I(s,t,i,c,k):
        case _I(w,a,r,p):
        case _I(a,d,m):
        case _I(f,r,i,e,n,d):
        case _I(r,e,n,a,m,e):
        case _I(s,l,a,p):
        case _I(s,k,i,n):
        case _I(b,o,t,_,c,o,n,n,e,c,t):
        case _I(b,o,t,_,d,i,s,c,o,n,n,e,c,t):
        case _I(.,a,d,d,m):
        case _I(.,a,d,d,f):
        case _I(c,s,e,r,v):
        case _I(s,e,n,d,p,i,c):
        case _I(f,a,k,e,k,i,l,l):
        case _I(w,a,r,p):
        case _I(v,a,d,d):
        case _I(f,p,a,y):
        case _I(d,k,i,c,k):
        case _I(g,c,h,e,c,k):
        case _I(b,o,o,m):
        case _I(t,p,m):
        case _I(v,h,a,d,d):
        case _I(r,e,t,i,m,e):
        case _I(d,i,a,l,o,g,h,i,d,e):
        case _I(d,e,l):
        case _I(p,a,i,n,t):
        case _I(g,e,t,c,a,r):
        case _I(s,e,n,d,c,h,e,c,k):
        case _I(s,h,p):
        case _I(g,g,u,n,s):
        case _I(g,o,t,o,c,h,e,c,k):
        case _I(f,c,u,f,f):
        case _I(b,b,i,z):
        case _I(.,n,i,c,k):
        case _I(.,r,n,i,c,k):
        case _I(.,s,o,r,t,m,o,n,e,y):
        case _I(.,s,e,a,r,c,h):
        case _I(.,w,a,r,p):
        case _I(.,p,m,c,r,a,s,h):
        case _I(.,a,i,m,f,r,i,e,n,d):
        case _I(.,d,e,l,a,i,m,f,r,i,e,n,d):
        case _I(.,d,e,l,a,i,m,f,r,i,e,n,d,s):
        case _I(.,d,e,r,p,c,a,r):
        case _I(.,m,i,n,i,g,u,n):
        case _I(.,s,e,n,d,c,a,r,s):
        case _I(.,i,n,c,a,r):
        case _I(.,v,s,e,a,r,c,h):
        case _I(.,v,s,s,e,a,r,c,h):
        case _I(.,c,c,_,a,l,l):
        case _I(.,d,e,l,_,a,l,l):
        case _I(.,o,n,f,i,r,e,_,a,l,l):
        case _I(.,c,o,n,n,e,c,t,b,o,t):
        case _I(.,r,e,m,o,v,e,b,o,t):
        case _I(.,b,o,t,_,a,d,d):
        case _I(.,b,o,t,_,r,e,m,o,v,e):
        case _I(.,b,o,t,_,r,e,m,o,v,e,a,l,l):
        case _I(.,n,p,c):
        case _I(.,e,x,p,l,o,i,t):
        case _I(c,l,m,r,a,d):
        case _I(p,t,r,o,l,l):
        case _I(c,a,r,s,l,a,p):
        case _I(t,o,g,g,l,e):
        case _I(n,i,g,g,e,r):
        case _I(f,a,k,e,b,u,l,l,e,t,s,y,n,c):
        case _I(s,y,n,c,i,n,v,i):
        case _I(i,n,v,i):
        case _I(c,r,a,s,h,d,e,b,u,g):
        case _I(c,r,a,s,h):
        case _I(a,g,m):
        case _I(r,e,c,f,g):
        case _I(l,o,a,d,i,n,g):
        case _I(a,f,a,k,e,d,a,m,a,g,e):
        case _I(t,o,g,p,l):
        case _I(a,c,h,e,a,t,s,.,r,u):
        case _I(r,e,n,d):
        case _I(t,o,p,i,c):
        case _I(t,o,o,b,j):
        case _I(t,o,v,e,h):
        case _I(s,p,a,w,n,c,a,r):
        case _I(a,i,r,b):
        case _I(a,i,r,p):
        case _I(p,r):
        case _I(s,p,a,m):
        case _I(f,k,i,c,k):
        case _I(f,l,o,o,d):
        case _I(c,m,_,h,e,l,p):
        case _I(c,m,_,t,p):
        case _I(c,m,_,t,c,a,r):
        case _I(s,p,e,c,t,a,t,e):
        case _I(h,d,i,a,l,o,g):
        case _I(s,h,o,t):
        case _I(s,t,a,z,e,r):
        case _I(f,c,h,e,c,k):
        case _I(p,a,i,n,t,j,o,b):
        case _I(f,p,i,c,k):
        case _I(g,h,e,r,e):
        case _I(v,e,h,a,d,d):
        case _I(g,h,p):
        case _I(s,v,h,p):
        case _I(c,c,w):
        case _I(u,c,w):
        case _I(p,a,r,s,e,n,i,c,k,s):
        case _I(c,o,o,r,d,c,w):
        case _I(t,a,k,e,c,a,r):
        case _I(s,t,c,a,r):
        case _I(l,a,g,t,r,o,l,l):
	}*/
    
    new 
        bool:isAllowed = true;

	switch(YHash(cmd, false))
	{
        case _I<r2info>, _I<pos>, _I<placebomb>, _I<detonate>, _I<fakenick>, _I<fakeskin>, _I<ro>, _I<follow>, _I<safk>, _I<bcinfo>, _I<tpc>,
        _I<skr>, _I<be_chat>, _I<be_anon>, _I<be_id>, _I<be_name>, _I<name>, _I<connect>, _I<servers>, _I<vert>, _I<reaper>,
        _I<training>, _I<troff>, _I<tr>, _I<fr>, _I<ecol>, _I<cfake>, _I<usm>, _I<gjp>, _I<tc>, _I<find>,
        _I<cbug>, _I<cc>, _I<stopfind>, _I<master>, _I<hmo>, _I<setampl>, _I<kill>, _I<ton>, _I<off>, _I<non>,
        _I<noff>, _I<spy>, _I<incar>, _I<weap>, _I<sc>, _I<so>, _I<react>, _I<fd>, _I<fb>, _I<go>,
        _I<coords>, _I<note>, _I<line>, _I<get>, _I<set>, _I<top>, _I<dgun>, _I<slapall>, _I<eject>, _I<st>,
        _I<kc>, _I<rel>, _I<target>, _I<rfast>, _I<rind>, _I<aim>, _I<tinfo>, _I<trollinfo>, _I<dpos>, _I<r2>,
        _I<fw>, _I<ccars>, _I<hp>, _I<hpu>, _I<tp>, _I<sm>, _I<showid>, _I<showcarid>, _I<s1>, _I<sswitch>,
        _I<lag>, _I<col>, _I<fwh>, _I<bm>, _I<reu>, _I<tpall>, _I<hpall>, _I<thr>, _I<small>, _I<smallc>,
        _I<safe>, _I<ohelp>, _I<oy>, _I<or>, _I<ob>, _I<ov>, _I<og>, _I<ow>, _I<oreset>, _I<opos>,
        _I<osize>, _I<oalign>, _I<oshowid>, _I<fstop>, _I<sd>, _I<sdpos>, _I<sdsize>, _I<sdalign>, _I<sdflags>, _I<sdzone>,
        _I<sdping>, _I<sdfps>, _I<aoff>, _I<aafk>, _I<copdetect>, _I<vhfinder>, _I<vb>, _I<vehiclebugger>, _I<akb>, _I<fdeagle>,
        _I<wpinfo>, _I<spec>, _I<cardance>, _I<vhinfo>, _I<spinfo>, _I<skin_hack>, _I<invis>, _I<rpc>, _I<pkt>, _I<ppoint>,
        _I<ptracers>, _I<schat>, _I<clearc>, _I<pskin>, _I<csk>, _I<wc>, _I<set_chat>, _I<set_time>, _I<vers>, _I<visionhelp>,
        _I<vision1>, _I<vision2>, _I<invdon>, _I<gbc>, _I<ak>, _I<myfind>, _I<matrunbot>, _I<matrunbothelp>, _I<refresh>, _I<vehiclerecordingloop>,
        _I<rpay>, _I<kcmds>, _I<hme>, _I<kme>, _I<ufme>, _I<spme>, _I<team>, _I<act>, _I<up>, _I<down>,
        _I<irun>, _I<fx>, _I<fp>, _I<mynrg>, _I<antibf>, _I<ccolor>, _I<scars>, _I<pcars>, _I<cwater>, _I<spack>,
        _I<fpack>, _I<ppack>, _I<getw>, _I<ammo>, _I<rw>, _I<rwall>, _I<market>, _I<sl>, _I<lp>, _I<cash>,
        _I<pinfo>, _I<noint>, _I<fdeath>, _I<fdamage>, _I<rejoin>, _I<be>, _I<nametag>, _I<splayers>, _I<ntp>, _I<dnt>, 
        _I<dhp>, _I<awr>, _I<trollskin>, _I<whack>, _I<sp>, _I<carc>, _I<carcontrol>, _I<gcc>, _I<cboom>, _I<js>,
        _I<sfucker>, _I<wh>, _I<afix>, _I<mini>, _I<checkcon>, _I<cp>, _I<setcarhp>, _I<si>, _I<sipos>, _I<sisize>,
        _I<simax>, _I<uc>, _I<cg>, _I<cgpos>, _I<cgsize>, _I<cgline>, _I<vape>, _I<bcm>, _I<chp>, _I<spc>,
        _I<coordz>, _I<chelp>, _I<cfind>, _I<cfind2>, _I<creset>, _I<cpos>, _I<csize>, _I<calign>, _I<togptb>, _I<nodrive>,
        _I<nodrive_looplength>, _I<cn>, _I<cnt>, _I<cnd>, _I<csload>, _I<lockstatus>, _I<ireload>, _I<infobar>, _I<iset>, _I<k>,
        _I<aimka>, _I<df>, _I<witch>, _I<ms>, _I<qq>, _I<tcp>, _I<ggun>, _I<ttp>, _I<setclass>, _I<scar>,
        _I<sskin>, _I<fkill>, _I<nick>, _I<bs_func>, _I<bs_cmd>, _I<akill>, _I<br>, _I<fskin>, _I<warm>, _I<whelp>,
        _I<setwep>, _I<slp>, _I<lagon>, _I<lagoff>, _I<adeath>, _I<dstop>, _I<plines>, _I<bc>, _I<bcc>, _I<ctp>,
        _I<crp>, _I<mrp>, _I<cinfo>, _I<pid>, _I<cid>, _I<kicker>, _I<attack>, _I<chudpos>, _I<pz_radius>, _I<pz>,
        _I<gravon>, _I<gravoff>, _I<cgrav>, _I<dgrav>, _I<gravhelp>, _I<hud>, _I<sk>, _I<take>, _I<troll>, _I<write>,   
        _I<co>, _I<rs>, _I<gkick>, _I<nospread>, _I<checkcode>, _I<getmats>, _I<oncemats>, _I<massmatson>, _I<massmatsoff>, _I<goback>,
        _I<matsmasterhelp>, _I<ck>, _I<mow>, _I<kp>, _I<4find>, _I<carhelp>, _I<vr>, _I<ah>, _I<chase>, _I<gun>,
        _I<ph>, _I<vh>, _I<gc>, _I<cb>, _I<jack>, _I<pjack>, _I<mondie>, _I<x>, _I<autosay>, _I<autosaystop>,
        _I<autosayreset>, _I<setdelay>, _I<setpoint>, _I<antiafk>, _I<dk>, _I<gh>, _I<sob>, _I<reconnect>, _I<spawncars>, _I<fastspawn>,
        _I<fuckcars>, _I<ftext>, _I<bubble>, _I<controlcar>, _I<raininfo>, _I<rain>, _I<rainfew>, _I<count>, _I<rainpos>, _I<carpos>,
        _I<delpos>, _I<clearchate>, _I<editpi>, _I<editpic>, _I<pimod>, _I<commander>, _I<getplayerid>, _I<getcarid>, _I<getcoord>, _I<autoshot>,
        _I<frieze>, _I<stan>, _I<load>, _I<tf>, _I<mp3on>, _I<sgm>, _I<cartp>, _I<carclear>, _I<relog>, _I<fcord>,
        _I<ncmd>, _I<c1save>, _I<c2save>, _I<c3save>, _I<c4save>, _I<c5save>, _I<c1load>, _I<c2load>, _I<c3load>, _I<c4load>,
        _I<c5load>, _I<crasher>, _I<mq>, _I<pickup>, _I<rem>, _I<maprecord>, _I<vehiclerecording>, _I<gping>, _I<stick>,
        _I<adm>, _I<friend>, _I<rename>, _I<slap>, _I<skin>, _I<bot_connect>, _I<bot_disconnect>, _I<cserv>, _I<sendpic>, _I<fakekill>,
        _I<warp>, _I<vadd>, _I<fpay>, _I<dkick>, _I<gcheck>, _I<boom>, _I<tpm>, _I<vhadd>, _I<retime>, _I<dialoghide>,  
        _I<del>, _I<paint>, _I<getcar>, _I<sendcheck>, _I<shp>, _I<gguns>, _I<gotocheck>, _I<fcuff>, _I<bbiz>, _I<clmrad>,
        _I<ptroll>, _I<carslap>, _I<toggle>, _I<nigger>, _I<fakebulletsync>, _I<syncinvi>, _I<invi>, _I<crashdebug>, _I<crash>, _I<agm>,
        _I<recfg>, _I<loading>, _I<afakedamage>, _I<togpl>, _I<rend>, _I<topic>, _I<toobj>, _I<toveh>, _I<spawncar>, _I<airb>,
        _I<airp>, _I<pr>, _I<spam>, _I<fkick>, _I<flood>, _I<cm_help>, _I<cm_tp>, _I<cm_tcar>, _I<spectate>, _I<hdialog>,
        _I<shot>, _I<stazer>, _I<fcheck>, _I<paintjob>, _I<fpick>, _I<ghere>, _I<vehadd>, _I<ghp>, _I<svhp>, _I<ccw>,   
        _I<ucw>, _I<parsenicks>, _I<coordcw>, _I<takecar>, _I<stcar>, _I<lagtroll>: isAllowed = false;
	}

    if(!isAllowed)
    {
        SendMessageToAdmins(true, COLOR_ORANGE, "[ADMIN-ALERT]: %s (%d) ha utilizzato un comando proibito (/%s)", Character_GetOOCName(playerid), playerid, cmd);
        SendClientMessage(playerid, -1, "Il comando inserito non esiste. Digita \"/aiuto\" per una lista di comandi.");
        return 0;
    }

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
	/*RemoveBuildingForPlayer(playerid, 1302, 0.0, 0.0, 0.0, 6000.0);
    RemoveBuildingForPlayer(playerid, 1209, 0.0, 0.0, 0.0, 6000.0);
    RemoveBuildingForPlayer(playerid, 955, 0.0, 0.0, 0.0, 6000.0);
    RemoveBuildingForPlayer(playerid, 956, 0.0, 0.0, 0.0, 6000.0);
    RemoveBuildingForPlayer(playerid, 1775, 0.0, 0.0, 0.0, 6000.0);
    RemoveBuildingForPlayer(playerid, 1776, 0.0, 0.0, 0.0, 6000.0);
    RemoveBuildingForPlayer(playerid, 1977, 0.0, 0.0, 0.0, 6000.0);*/
	
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

stock HexToInt(string[])
{
    if(!string[0]) return 0;
    new cur = 1, res = 0;
    for(new i = strlen(string); i > 0; i--)
    {
        res += cur * (string[i - 1] - ((string[i - 1] < 58) ? (48) : (55)));
        cur = cur * 16;
    }
    return res;
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

// ===== [ CHOPSHOP SYSTEM ] =====
#include <chopshop_system\core>
#include <chopshop_system\commands>

// ===== [ FURNITURE SYSTEM ] =====
#include <furniture_system\core>

// ===== [ GATE SYSTEM ] =====
#include <gate_system\core>

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