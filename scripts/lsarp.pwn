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
			{"Dyn", "Dynamic",    3, 7},
			{"TD",  "TextDraw",   2, 8},
			{"Upd", "Update",     3, 6},
			{"Obj", "Object",     3, 6},
			{"Cmd", "Command",    3, 7}
        };


        https://forum.sa-mp.com/showthread.php?t=659714 -> PawnPLUS Intersting
        https://forum.sa-mp.com/showthread.php?t=435525 -> Timerfix
        https://forum.sa-mp.com/showthread.php?t=120013 -> MapAndreas
        https://forum.sa-mp.com/showthread.php?t=326980 -> JIT
        https://forum.sa-mp.com/showthread.php?t=489897 -> Bit 

*/
// #pragma warning disable 208 // actually just a good way to prevent warning: "function with tag result used before definition, forcing reparse".

#include <a_samp>

//#define DEBUG 0

#undef MAX_PLAYERS
#define MAX_PLAYERS     (100)

#define INFINITY (Float:0x7F800000)

#define PRESSED(%0) \
	(((newkeys & (%0)) == (%0)) && ((oldkeys & (%0)) != (%0)))

// Includes
#include <sscanf2>
#include <a_mysql>
#include <a_mysql_yinline>
#include <YSI/y_timers>
#include <Pawn.CMD>
#include <whirlpool>
#include <streamer>
#include <strlib>

#define PP_SYNTAX 1
//#define PP_SYNTAX_GENERIC 1
#define PP_ADDITIONAL_TAGS E_ITEM_DATA

#include <PawnPlus>

#include <pp_wrappers.pwn>
#include <easyDialogs.pwn>

#define Inventory List@Inventory

#include <YSI\y_hooks>
DEFINE_HOOK_REPLACEMENT(ShowRoom, SR);
DEFINE_HOOK_REPLACEMENT(Element, Elm);


//#define AC_GivePlayerWeapon     GivePlayerWeapon
//#define AC_ResetPlayerWeapons    ResetPlayerWeapons

// Others
#include <forwarded_functions.pwn>
#include <globals.pwn>

#include <utils/colors.pwn>
#include <utils/utils.pwn>
#include <utils/weapons.pwn>

#include <database/database.pwn>

#include <utils/maths.pwn>
#include <sa_zones>

#include <callbacks.pwn>
#include <enums.pwn>
#include <systems.pwn>

//


#include <commands.pwn>

#include <utils/vehicles.pwn>
#include <global_timers.pwn>

native print_s(AmxString:string) = print;

main()
{
    printf("LSARP - By CodaKKK. Started: 26/02/2019.");

    PlayerInventory = map_new();
    VehicleInventory = map_new();

    new List:test = list_new();
    printf("%d", list_add(test, 1));
    printf("%d", list_add(test, 1));
    printf("%d", list_add(test, 1));
    printf("%d", list_add(test, 1));
    printf("%d", list_add(test, 1));
    printf("%d", list_add(test, 1));

    new Inventory:inv = Inventory_New(5);
    Inventory_AddItem(inv, 24, 1, 0);
    print_s(Inventory_ParseForDialog(inv));
}
    


public OnGameModeInit()
{
    SetGameModeText("ApoC1");
    
    ShowPlayerMarkers(PLAYER_MARKERS_MODE_OFF);
    SetNameTagDrawDistance(20.0);
    DisableInteriorEnterExits();
    ManualVehicleEngineAndLights(); 
    EnableStuntBonusForAll(false);
    return 1;
}

public OnPlayerRequestClass(playerid, classid) return 1;
public OnPlayerClearData(playerid) return 1;
public OnVehicleDeath(vehicleid, killerid) return 1;
public OnVehicleSpawn(vehicleid) return 1;
public OnPlayerExitVehicle(playerid, vehicleid) return 1;
public OnPlayerPickUpDynamicPickup(playerid, pickupid) return 1;
public OnPlayerSpawn(playerid) return 1;
public OnPlayerDeath(playerid, killerid, reason) return 0;
public OnPlayerRequestSpawn(playerid) return 1;
public OnPlayerKeyStateChange(playerid, newkeys, oldkeys) return 1;
public OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ) return 1;

public OnPlayerCommandReceived(playerid, cmd[], params[], flags)
{
    if(!gAccountLogged[playerid] || !gCharacterLogged[playerid])
        return 0;
    if(flags & CMD_SUPPORTER && AccountInfo[playerid][aAdmin] < 1)
    {
        SendClientMessage(playerid, COLOR_ERROR, "> Non sei un membro dello staff.");
        return 0;
    }
    else if(flags & CMD_JR_MODERATOR && AccountInfo[playerid][aAdmin] < 2)
    {
        SendClientMessage(playerid, COLOR_ERROR, "> Non sei un membro dello staff.");
        return 0;
    }
    else if(flags & CMD_MODERATOR && AccountInfo[playerid][aAdmin] < 3)
    {
        SendClientMessage(playerid, COLOR_ERROR, "> Non sei un membro dello staff.");
        return 0;
    }
    else if(flags & CMD_ADMIN && AccountInfo[playerid][aAdmin] < 4)
    {
        SendClientMessage(playerid, COLOR_ERROR, "> Non sei un membro dello staff.");
        return 0;
    }
    else if(flags & CMD_DEVELOPER && AccountInfo[playerid][aAdmin] < 5)
    {
        SendClientMessage(playerid, COLOR_ERROR, "> Non sei un membro dello staff.");
        return 0;
    }
    else if(flags & CMD_RCON && (AccountInfo[playerid][aAdmin] < 5 || !IsPlayerAdmin(playerid)))
    {
        SendClientMessage(playerid, COLOR_ERROR, "> Non sei un membro dello staff.");
        return 0;
    }
    return 1;
}

public OnPlayerCommandPerformed(playerid, cmd[], params[], result, flags)
{
    if(result == -1)
    {
        SendClientMessage(playerid, -1, "Il comando inserito non esiste. Digita /aiuto per una lista di comandi.");
        return 0;
    }
    return 1;
}

public OnPlayerText(playerid, text[])
{
    if(isnull(text))
        return 0;
    new string[256];
    if(pAdminDuty[playerid])
    {
        //format(string, sizeof(string), "{FFFFFF}(( {FF6347}%s{FFFFFF} [%d]: %s ))", AccountInfo[playerid][aName], playerid, text);
        //ProxDetector(playerid, 15.0, string, COLOR_FADE1, COLOR_FADE2, COLOR_FADE3, COLOR_FADE4, COLOR_FADE5);
        pc_cmd_b(playerid, text);
    }
    else
    {
        format(string, sizeof(string), "%s dice: %s", Character_GetOOCName(playerid), text);
        ProxDetector(playerid, 15.0, string, COLOR_FADE1, COLOR_FADE2, COLOR_FADE3, COLOR_FADE4, COLOR_FADE5);
    }
    return 0;
}

public OnPlayerDisconnect(playerid, reason)
{
    new const reasonName[3][16] = {"Crash", "Uscito", "Kick/Ban"};
    new 
        string[MAX_PLAYER_NAME + 64], name[MAX_PLAYER_NAME];

    GetPlayerName(playerid, name, sizeof(name));
    format(string, sizeof(string), "> %s è uscito dal server. [%s]", name, reasonName[reason]);
    SendClientMessageToAll(COLOR_GREY, string);

    if(!gAccountLogged[playerid] || !gCharacterLogged[playerid])
        return 0;
    
    CallLocalFunction(#OnCharacterDisconnected, "i", playerid);
    CallLocalFunction(#OnPlayerClearData, "i", playerid);
    return 1;
}

// This is the last callback called after the hooks.
public OnPlayerConnect(playerid)
{
    PreloadAnimations(playerid);
    SetPlayerColor(playerid, 0xFFFFFFFF);
    gAccountLogged[playerid] = 0;
    gCharacterLogged[playerid] = 0;
    gLoginAttempts[playerid] = 0;

    for(new i = 0; i < 30; ++i)
    {
        SendClientMessage(playerid, -1, " ");
    }
    SendClientMessage(playerid, COLOR_YELLOW, "________________________________________________________________");
    SendClientMessage(playerid, COLOR_YELLOW, "> Benvenuto su Los Santos Apocalypse Roleplay [www.lsarp.it]!");
    SendClientMessage(playerid, COLOR_YELLOW, "> Alpha v0.1");
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

    OnPlayerRequestClass(playerid, 0);
    return 1;
}

public OnQueryError(errorid, const error[], const callback[], const query[], MySQL:handle)
{
    printf("ERROR: %s", error);
    printf("QUERY ERROR: %s", query);
    return 1;
}

stock PreloadAnimations(playerid)
{
    PreloadAnimLib(playerid,"DANCING");
    PreloadAnimLib(playerid,"HEIST9");
    PreloadAnimLib(playerid,"BOMBER");
    PreloadAnimLib(playerid,"RAPPING");
    PreloadAnimLib(playerid,"SHOP");
    PreloadAnimLib(playerid,"BEACH");
    PreloadAnimLib(playerid,"SMOKING");
    PreloadAnimLib(playerid,"FOOD");
    PreloadAnimLib(playerid,"ON_LOOKERS");
    PreloadAnimLib(playerid,"DEALER");
    PreloadAnimLib(playerid,"CRACK");
    PreloadAnimLib(playerid,"CARRY");
    PreloadAnimLib(playerid,"COP_AMBIENT");
    PreloadAnimLib(playerid,"PARK");
    PreloadAnimLib(playerid,"INT_HOUSE");
    PreloadAnimLib(playerid,"FOOD" );
    PreloadAnimLib(playerid,"ped" );
    PreloadAnimLib(playerid,"MISC" );
    PreloadAnimLib(playerid,"POLICE" );
    PreloadAnimLib(playerid,"GRAVEYARD" );
    PreloadAnimLib(playerid,"WUZI" );
    PreloadAnimLib(playerid,"SUNBATHE" );
    PreloadAnimLib(playerid,"PLAYIDLES" );
    PreloadAnimLib(playerid,"CAMERA" );
    PreloadAnimLib(playerid,"RIOT" );
    PreloadAnimLib(playerid,"DAM_JUMP" );
    PreloadAnimLib(playerid,"JST_BUISNESS" );
    PreloadAnimLib(playerid,"KISSING" );
    PreloadAnimLib(playerid,"GANGS" );
    PreloadAnimLib(playerid,"GHANDS" );
    PreloadAnimLib(playerid,"BLOWJOBZ" );
    PreloadAnimLib(playerid,"SWEET" );
    return 1;
}

// Preload animations
// Hehe Figo
// Sono Forte
stock PreloadAnimLib(playerid, animlib[])
{
    ApplyAnimation(playerid, animlib, "null", 0.0, 0, 0, 0, 0, 0, 0);
}

stock Log(playerName[], giveplayerName[], text[], extravar = 0)
{
    new query[182];
    mysql_format(gMySQL, query, sizeof(query), "INSERT INTO `logs` (PlayerID, GivePlayerID, Text, ExtraVar, Time) \
                                                VALUES('%s', '%s', '%e', '%d', '%d')", 
                                                playerName, 
                                                giveplayerName, 
                                                text,
                                                extravar,
                                                gettime());
    mysql_tquery(gMySQL, query);
}
/**/