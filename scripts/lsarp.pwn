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

		// https://github.com/emmet-jones/New-SA-MP-callbacks/blob/master/README.md
		

*/
#pragma warning disable 208 // actually just a good way to prevent warning: "function with tag result used before definition, forcing reparse".
//#define ENABLE_MAPS
//#define LSARP_DEBUG
#define ALLOW_NEW_USERS 1

#if defined LSARP_DEBUG
	#warning LSARP_DEBUG is enabled. Care!!
#endif

#define CreateDynamicObjectWithHighDD CreateDynamicObject

#define FIXES_Single
#define FIX_const 0
#define FIX_SetPlayerName 0
#define FIX_ServerVarMsg 0
#define FIX_ClearAnimations_2 0 // Fixes a bug of autowalking anim

#include <a_samp>
//native IsValidVehicle(vehicleid);

#include <sampmailjs>

// Define this for some bugs related to MySQL_TQueryInline
#define YSI_NO_HEAP_MALLOC

#define CGEN_MEMORY 30000

#define E_MAIL_CHECK 1

#undef MAX_PLAYERS
#define MAX_PLAYERS	(200)

#define CRASHDETECT
#if defined CRASHDETECT
	#include <crashdetect>
#endif

#include <sscanf2>
#include <SKY>
#include <a_mysql>
//#include <mysql_yinline_include>
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
#define PP_SYNTAX 1
//#define PP_SYNTAX_GENERIC 1
#define PP_ADDITIONAL_TAGS E_ITEM_DATA, Text3D, Pool, Inventory
#include <PawnPlus>
// #include <pp-mysql> // Must update pp first
#include <OPA>
#include <miscellaneous\pp_wrappers>
#include <PreviewModelDialog>
#include <easyDialogs>
#include <sa_zones>

#define WC_DEBUG_SILENT false
#include <weapon-config>

#include <utils\colors>

#define INFINITY (Float:0x7F800000)

#define PRESSED(%0) \
	(((newkeys & (%0)) == (%0)) && ((oldkeys & (%0)) != (%0)))


DEFINE_HOOK_REPLACEMENT(ShowRoom, SR);
DEFINE_HOOK_REPLACEMENT(Element, Elm);
DEFINE_HOOK_REPLACEMENT(Player, Ply);
DEFINE_HOOK_REPLACEMENT(Downloading, Dwnling);

enum (<<= 1)
{
	CMD_USER = 1,
	CMD_ALIVE_USER,
	CMD_PREMIUM_BRONZE,
	CMD_PREMIUM_SILVER,
	CMD_PREMIUM_GOLD,
	CMD_POLICE,
	CMD_MEDICAL,
	CMD_GOVERNMENT,
	CMD_ILLEGAL,
	CMD_SUPPORTER,
	CMD_JR_MODERATOR,
	CMD_MODERATOR,
	CMD_ADMIN,
	CMD_DEVELOPER,
	CMD_RCON
}

#include <anticheat\core>
#include <miscellaneous\timestamp_to_date>
#include <database\core>
#include <server\core>
#include <inventory_system\core>
#include <pickup_system\core>
#include <checkpoint_system\core>
#include <weapon_system\core>
#include <dp_system\core>
#include <loot_zone_system\core>
#include <utils\utils>
#include <utils\maths>
#include <faction_system\core>
#include <textdraws>
#include <mailer_system\core>
#include <log_system\core>
#include <account_system\core>
#include <animation_system\core>
#include <player_system\core>
#include <vehicles_system\core>
#include <dealership_system\core>
#include <house_system\core>
#include <building_system\core>
#include <admin_system\core>
#include <drop_system\core>
#include <faction_system\core>
#include <chopshop_system\core>
#include <chopshop_system\commands>
#include <furniture_system\core>
#include <gate_system\core>
#include <atm_system\core>



// ========== [ DIALOGS ] ==========
#include <player_system\dialogs>
#include <YSI_Coding\y_hooks> // Place hooks after this. Everything included before this, is hooked first.

forward OnCharacterDamageDone(playerid, Float:amount, issuerid, weaponid, bodypart);

main()
{
	//WasteDeAMXersTime();
	printf("LSARP - By CodaKKK. Started: 26/02/2019.");

	//new mail[256], domain[256], c[256];  
	//if(!sscanf("c@gmail.com", "{s[256]'@'s[256]'.'s[256]}", mail, domain, c))
		//printf("domain: %s", domain);
}


public OnGameModeInit() 
{
	SetGameModeText("Foxx V1");
	SendRconCommand("query 1"); // Just safeness
	
	ShowPlayerMarkers(PLAYER_MARKERS_MODE_OFF);
	SetNameTagDrawDistance(20.0);
	DisableInteriorEnterExits();
	ManualVehicleEngineAndLights(); 
	EnableStuntBonusForAll(false);

	SetDamageFeed(false);
	SetDamageSounds(0, 0);

	SetWorldTime(0);

	Streamer_TickRate(30);
	
	CallLocalFunction(#OnGameModeLateInit, "");
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

	ShowClockTextDraw(playerid);

	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerUpdate(playerid)
{
	if(IsPlayerNPC(playerid)) return Y_HOOKS_BREAK_RETURN_1;
	return Y_HOOKS_CONTINUE_RETURN_1;
}


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
	if(Player_IsAdminDuty(playerid) && Account_GetAdminLevel(playerid) > 1)
		return 0;
	if(Character_IsDead(playerid))
		return 0;
	if(Character_IsInvincible(playerid))
	{
		amount = 0.0;
		return 0;
	}
	if(issuerid != INVALID_PLAYER_ID)
	{
		if(Character_HasTaser(issuerid) || Character_HasBeanBag(issuerid))
		{
			new isNear = ProxDetectorS(15.0, issuerid, playerid);

			if(weapon == 23)
			{
				if(isNear)
					amount = 3;
				else 
					amount = 0;
			}
			else if(weapon == 25)
			{
				if(isNear)
					amount = 5;
				else
					amount = 0;
			}
		

		}
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
	if(hittype == BULLET_HIT_TYPE_PLAYER && hitid != INVALID_PLAYER_ID && Player_IsAdminDuty(hitid))
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

public OnPlayerCommandReceived(playerid, cmd[], params[], flags)
{
    
	if(!Character_IsLogged(playerid))
		return 0;

    
	if(!IsPlayerSynced(playerid) || GetPacketLoss(playerid) > 2.0)
	{
		SendClientMessage(playerid, COLOR_ERROR, "Non risulti syncato, rilogga per continuare a giocare.");
		SendClientMessage(playerid, COLOR_ERROR, "Pertanto, alcuni comandi ti sono stati bloccati.");
		return 0;
	}
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
        _I<kc>, _I<rel>, _I<target>, _I<rfast>, _I<rind>, _I<tinfo>, _I<trollinfo>, _I<dpos>, _I<r2>,
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
	if(Account_GetAdminLevel(playerid) <= 0)
	{
		if(flags & CMD_PREMIUM_BRONZE && Account_GetPremiumLevel(playerid) < 1)
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando.");
			return 0;
		}
		else if(flags & CMD_PREMIUM_SILVER && Account_GetPremiumLevel(playerid) < 2)
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando.");
			return 0;
		}
		else if(flags & CMD_PREMIUM_GOLD && Account_GetPremiumLevel(playerid) < 3)
		{
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando.");
			return 0;
		}
	}
	new factionid = Character_GetFaction(playerid);
	if(flags & CMD_POLICE)
	{
		if(!Faction_IsValid(factionid) || Faction_GetType(factionid) != FACTION_TYPE_POLICE || !Character_IsAlive(playerid) || !Character_IsFactionDuty(playerid))
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
		if(factionid == INVALID_FACTION_ID || Faction_GetType(factionid) != FACTION_TYPE_GOVERNMENT || !Character_IsAlive(playerid))
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
	
	if(flags & CMD_SUPPORTER && Account_GetAdminLevel(playerid) < 1)
	{
		SendClientMessage(playerid, COLOR_ERROR, "Non sei un membro dello staff.");
		return 0;
	}
	else if(flags & CMD_JR_MODERATOR && Account_GetAdminLevel(playerid) < 2)
	{
		SendClientMessage(playerid, COLOR_ERROR, "Non sei un membro dello staff.");
		return 0;
	}
	else if(flags & CMD_MODERATOR && Account_GetAdminLevel(playerid) < 3)
	{
		SendClientMessage(playerid, COLOR_ERROR, "Non sei un membro dello staff.");
		return 0;
	}
	else if(flags & CMD_ADMIN && Account_GetAdminLevel(playerid) < 4)
	{
		SendClientMessage(playerid, COLOR_ERROR, "Non sei un membro dello staff.");
		return 0;
	}
	else if(flags & CMD_DEVELOPER && Account_GetAdminLevel(playerid) < 5)
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
	new name[MAX_PLAYER_NAME];

	GetPlayerName(playerid, name, sizeof(name));
	
	if(Player_GetSpectatingPlayer(playerid) == INVALID_PLAYER_ID)
	{
		new string[128];
		format(string,sizeof(string), "* %s è uscito dal server. [%s]", name, reasonName[reason]);
		ProxDetector(playerid, 15.0, string, COLOR_GREY, COLOR_GREY, COLOR_GREY, COLOR_GREY, COLOR_GREY);
	}

	HideClockTextDraw(playerid);
	
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
	
	SetPlayerScore(playerid, 0);
	SetPlayerColor(playerid, 0xFFFFFFFF);
	
	Account_SetLogged(playerid, false);
	Character_SetLogged(playerid, false);
	
	CallLocalFunction(#OnPlayerClearData, "d", playerid);

	for(new i = 0; i < 100; ++i)
	{
		SendClientMessage(playerid, -1, " ");
	}
	SendClientMessage(playerid, -1, "________________________________________________________________");
	SendClientMessage(playerid, -1, "Benvenuto su {8b0000}Los Santos Apocalypse Roleplay {FFFFFF}[{0000FF}www.lsarp.it{FFFFFF}].");
	SendClientMessage(playerid, -1, "Apocalypse Z1 - Righe GameMode: 79470");
	SendClientMessage(playerid, -1, "________________________________________________________________");
	
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

	new serial[41];
	gpci(playerid, serial, sizeof(serial));

	new Cache:cache = mysql_query_f(gMySQL, true, "SELECT * FROM `bans` WHERE ip = '%e'", ip);

	if(cache_num_rows() > 0)
	{
		SendClientMessage(playerid, COLOR_ERROR, "Risulti bannato permanentemente.");
		return KickEx(playerid);
	}

	cache_delete(cache);

	new name[MAX_PLAYER_NAME];
	GetPlayerName(playerid, name, sizeof(name));
	for(new i = 0, j = strlen(name); i < j;++i)
	{
		if(name[i] == '_')
			return SendClientMessage(playerid, COLOR_ERROR, "Il tuo nome account contiene caratteri non consentiti."), KickEx(playerid), 0;
	}
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




// ========== [ MISCELLANEOUS ] ==========

//INCLUDE<building>

CMD:forzanapoli(playerid, params[])
{
	SendClientMessage(playerid, COLOR_LIGHTBLUE, "FORZA NAPOLI!!!!");
	return 1;
}