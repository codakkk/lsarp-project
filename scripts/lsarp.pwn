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
// #pragma warning disable 208 // actually just a good way to prevent warning: "function with tag result used before definition, forcing reparse".

#include <a_samp>
native IsValidVehicle(vehicleid);

//#define Function:%0()			forward %0();public %0()
//#define Function:%0(%1) 		forward %0(%1);public %0(%1)

#define DestroyDynamic3DTextLabelEx DestroyDynamic3DTextLabel // Useful for the future
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

#define INFINITY (Float:0x7F800000)

#define PRESSED(%0) \
	(((newkeys & (%0)) == (%0)) && ((oldkeys & (%0)) != (%0)))

// Includes
#include <sscanf2>
#include <a_mysql>
#include <YSI_Coding\y_timers>
#include <YSI_Coding\y_va>
#include <YSI_Coding\y_inline>
// For YSI and PawnPlus yield conflict.
#undef yield
#undef @@
#include <Pawn.CMD>
#include <whirlpool>
#include <streamer>
#include <strlib>
#include <YSI_Data\y_bit>
#define PP_SYNTAX 1
//#define PP_SYNTAX_GENERIC 1
#define PP_ADDITIONAL_TAGS E_ITEM_DATA

#include <PawnPlus>
//native print_s(AmxString:string) = print;
#include <OPA>
#include <miscellaneous\pp_wrappers.pwn>
#include <easyDialogs.pwn>

#define MAILER_URL "185.25.204.170/server_mailer/emailconfig.php" // This has to be defined BEFORE you include mailer.


#define Inventory List@Inventory

#include <YSI_Coding\y_hooks>
DEFINE_HOOK_REPLACEMENT(ShowRoom, SR);
DEFINE_HOOK_REPLACEMENT(Element, Elm);

enum (<<= 1)
{
	CMD_USER = 1,
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


#define IC_JAIL_X						(2638.6926)
#define IC_JAIL_Y						(-1995.0134)
#define IC_JAIL_Z 						(-59.7283)
#define IC_JAIL_INT						(0)

#define OOC_JAIL_X						(197.1209)
#define OOC_JAIL_Y						(176.3204)
#define OOC_JAIL_Z 						(1003.0234)
#define OOC_JAIL_INT					(3)

//
// Others
#include <forwarded_functions.pwn>
#include <miscellaneous\globals.pwn>

#include <utils/colors.pwn>
#include <utils/utils.pwn>

#include <database/database.pwn>

#include <utils/maths.pwn>
#include <sa_zones>

#include <callbacks.pwn>
#include <enums.pwn>
#include <systems.pwn>

#include <utils/weapons.pwn> 
//


#include <commands.pwn>

#include <utils/vehicles.pwn>
#include <miscellaneous\global_timers.pwn>

#include <miscellaneous\anims.pwn>
#include <miscellaneous\maps.pwn>

main()
{
	printf("LSARP - By CodaKKK. Started: 26/02/2019.");

	// Should I initialize them in a OnGameModeInit hook?
	PlayerInventory = map_new();
	VehicleInventory = map_new();
	HouseList = list_new();
}

public OnGameModeInit() 
{
	SetGameModeText("ApoC1");
	
	ShowPlayerMarkers(PLAYER_MARKERS_MODE_OFF);
	SetNameTagDrawDistance(20.0);
	DisableInteriorEnterExits();
	ManualVehicleEngineAndLights(); 
	EnableStuntBonusForAll(false);

	SetWorldTime(0);

	// /ritirastipendio
	Pickup_Create(1239, 0, 292.3752, 180.7307, 1007.1790, E_ELEMENT_TYPE:ELEMENT_TYPE_PAYCHECK, -1, 3);
	CreateDynamic3DTextLabel("/ritirastipendio", COLOR_BLUE, 292.3752, 180.7307, 1007.1790 + 0.55, 20.0, .worldid = -1, .interiorid = 3);
	// /ritirastipendio
	Pickup_Create(1239, 0, 2649.7790, -1948.9510, -58.7273, ELEMENT_TYPE_JAIL_EXIT, .worldid = -1, .interiorid = 0);
	CreateDynamic3DTextLabel("/lasciacarcere", COLOR_BLUE, 2649.7790, -1948.9510, -58.7273 + 0.55, 20.0, .worldid = -1, .interiorid = 0);

	Building_LoadAll();
	House_LoadAll();

	LoadTextDraws();
	return 1;
}

public OnGameModeExit()
{
	TextDrawDestroy(txtAnimHelper);
	TextDrawDestroy(Clock);
	return 1;
}

public OnPlayerUpdate(playerid)
{
	if(IsPlayerNPC(playerid)) return Y_HOOKS_BREAK_RETURN_1;
	return Y_HOOKS_CONTINUE_RETURN_1;
}

public OnPlayerRequestClass(playerid, classid) return 1;
public OnPlayerClearData(playerid) return 1;
public OnVehicleDeath(vehicleid, killerid) return 1;
public OnVehicleSpawn(vehicleid) return 1;
public OnPlayerExitVehicle(playerid, vehicleid) return 1;
public OnPlayerPickUpDynamicPickup(playerid, pickupid) return 1;
public OnPlayerSpawn(playerid) return 1;
public OnPlayerDeath(playerid, killerid, reason) 
{
	if(pAnimLoop{playerid})
	{
		pAnimLoop{playerid} = false;
		TextDrawHideForPlayer(playerid, txtAnimHelper);
	}
	return 1;
}
public OnPlayerRequestSpawn(playerid) return 1;
public OnPlayerKeyStateChange(playerid, newkeys, oldkeys) return 1;
public OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ) 
{
	if(IsPlayerNPC(playerid))
		return Y_HOOKS_BREAK_RETURN_1;
	printf("lsarp.pwn/OnPlayerWeaponShot");
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

public OnVehicleMod(playerid, vehicleid, componentid)
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

public OnPlayerText(playerid, text[])
{
	if(isnull(text))
		return 0;
	if(pAdminDuty[playerid])
	{
		//format(string, sizeof(string), "{FFFFFF}(( {FF6347}%s{FFFFFF} [%d]: %s ))", AccountInfo[playerid][aName], playerid, text);
		//ProxDetector(playerid, 15.0, string, COLOR_FADE1, COLOR_FADE2, COLOR_FADE3, COLOR_FADE4, COLOR_FADE5);
		pc_cmd_b(playerid, text);
	}
	else
	{
		if(Character_IsDead(playerid))
			return 0;
		new String:string = str_format("%s dice: %s", Character_GetOOCName(playerid), text);
		ProxDetectorStr(playerid, 15.0, string, COLOR_FADE1, COLOR_FADE2, COLOR_FADE3, COLOR_FADE4, COLOR_FADE5);
	}
	return 0;
}

public OnPlayerDisconnect(playerid, reason)
{
	new const reasonName[3][16] = {"Crash", "Uscito", "Kick/Ban"};
	new String:string, name[MAX_PLAYER_NAME];

	GetPlayerName(playerid, name, sizeof(name));
	string = str_format("%s è uscito dal server. [%s]", name, reasonName[reason]);
	SendClientMessageToAllStr(COLOR_GREY, string);
	TextDrawHideForPlayer(playerid, Clock);
	if(!gAccountLogged[playerid] || !Character_IsLogged(playerid))
		return 0;
	
	CallLocalFunction(#OnCharacterDisconnected, "i", playerid);
	CallLocalFunction(#OnPlayerClearData, "i", playerid);
	gAccountLogged[playerid] = 0;
	gCharacterLogged[playerid] = 0;
	return 1;
}

// This is the last callback called after the hooks.
public OnPlayerConnect(playerid)
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
	mysql_tquery_f(gMySQL, "INSERT INTO `logs` \
						  (PlayerID, GivePlayerID, Text, ExtraVar, Time) \
						  VALUES('%s', '%s', '%e', '%d', '%d')", playerName, 
										giveplayerName, 
										text,
										extravar,
										gettime());
}

stock mysql_tquery_f(MySQL:handle, const query[], GLOBAL_TAG_TYPES:...)
{
	new ret = mysql_format(handle, YSI_UNSAFE_HUGE_STRING, YSI_UNSAFE_HUGE_LENGTH, query, ___(2));
	if(ret)
	{
		ret = mysql_tquery(handle, YSI_UNSAFE_HUGE_STRING);
	}
	return ret;
}
/**/

stock GetPlayerSpeed(playerid,bool:kmh)
{
	new Float:Vx, Float:Vy, Float:Vz, Float:rtn;
	if(IsPlayerInAnyVehicle(playerid)) 
		GetVehicleVelocity(GetPlayerVehicleID(playerid),Vx,Vy,Vz); 
	else
		GetPlayerVelocity(playerid,Vx,Vy,Vz);
	rtn = floatsqroot(Vx*Vx + Vy*Vy + Vz*Vz);
	return kmh ? floatround(rtn * 100 * 1.61) : floatround(rtn * 100);
}

stock LoadTextDraws()
{
	txtAnimHelper = TextDrawCreate(610.0, 400.0, "~w~Usa ~b~~k~~CONVERSATION_NO~ ~w~per fermare l'animazione");
    TextDrawUseBox(txtAnimHelper, 0);
    TextDrawFont(txtAnimHelper, 2);
    TextDrawSetShadow(txtAnimHelper,0);
	TextDrawSetOutline(txtAnimHelper,1);
	TextDrawBackgroundColor(txtAnimHelper,0x000000FF);
	TextDrawColor(txtAnimHelper,0xFFFFFFFF);
	TextDrawAlignment(txtAnimHelper,3);

	Clock = TextDrawCreate(546.000000, 22.000000, "00:00:00");
	TextDrawBackgroundColor(Clock, 255);
	TextDrawFont(Clock, 3);
	TextDrawLetterSize(Clock, 0.370000, 2.000000);
	TextDrawColor(Clock, -17);
	TextDrawSetOutline(Clock, 1);
	TextDrawSetProportional(Clock, 0);
}

stock LoadPlayerTextDraws(playerid)
{
	pInfoText[playerid] = CreatePlayerTextDraw(playerid, 23.000000, 180.000000, "Testo");
	PlayerTextDrawUseBox(playerid, pInfoText[playerid],1);
	PlayerTextDrawBoxColor(playerid, pInfoText[playerid],0x00000033);
	PlayerTextDrawTextSize(playerid, pInfoText[playerid],180.000000, 5.000000);
	PlayerTextDrawAlignment(playerid, pInfoText[playerid],0);
	PlayerTextDrawBackgroundColor(playerid, pInfoText[playerid],0x000000ff);
	PlayerTextDrawFont(playerid, pInfoText[playerid],2);
	PlayerTextDrawLetterSize(playerid, pInfoText[playerid],0.250000, 1.099999);
	PlayerTextDrawColor(playerid, pInfoText[playerid],0xffffffff);
	PlayerTextDrawSetOutline(playerid, pInfoText[playerid],1);
	PlayerTextDrawSetProportional(playerid, pInfoText[playerid],1);
	PlayerTextDrawSetShadow(playerid, pInfoText[playerid],1);

	pMoneyTD[playerid] = CreatePlayerTextDraw(playerid, 608.000000, 101.000000, " "); // Money Changes Textdraw
	PlayerTextDrawAlignment(playerid, pMoneyTD[playerid], 3);
	PlayerTextDrawFont(playerid, pMoneyTD[playerid], 3);
	PlayerTextDrawLetterSize(playerid, pMoneyTD[playerid], 0.519999, 2.100000);
	PlayerTextDrawColor(playerid, pMoneyTD[playerid], 0xFFFFFFFF);
	PlayerTextDrawSetOutline(playerid, pMoneyTD[playerid], 1);

	pVehicleFuelText[playerid] = CreatePlayerTextDraw(playerid, 578.000000, 392.000000, "~y~000%");
	PlayerTextDrawBackgroundColor(playerid, pVehicleFuelText[playerid], 255);
	PlayerTextDrawFont(playerid, pVehicleFuelText[playerid], 2);
	PlayerTextDrawLetterSize(playerid, pVehicleFuelText[playerid], 0.200000, 1.190000);
	PlayerTextDrawColor(playerid, pVehicleFuelText[playerid], 16711935);
	PlayerTextDrawSetOutline(playerid, pVehicleFuelText[playerid], 1);
	PlayerTextDrawSetProportional(playerid, pVehicleFuelText[playerid], 1);
	PlayerTextDrawSetSelectable(playerid, pVehicleFuelText[playerid], 0);

 	pJailTimeText[playerid] = CreatePlayerTextDraw(playerid, 499.000000, 101.000000, "~n~~n~~g~TEMPO RIMANENTE:~w~~n~ 0 secondi");
	PlayerTextDrawBackgroundColor(playerid, pJailTimeText[playerid], 255);
	PlayerTextDrawFont(playerid, pJailTimeText[playerid], 1);
	PlayerTextDrawLetterSize(playerid, pJailTimeText[playerid], 0.270000, 1.000000);
	PlayerTextDrawColor(playerid, pJailTimeText[playerid], -1);
	PlayerTextDrawSetOutline(playerid, pJailTimeText[playerid], 1);
	PlayerTextDrawSetProportional(playerid, pJailTimeText[playerid], 1);
	PlayerTextDrawSetSelectable(playerid, pJailTimeText[playerid], 0);
}

stock Player_Info(playerid, text[], bool:forced = false, time = 2500)
{
	if(pInfoBoxShown[playerid] == true && !forced)
		return 1;

	PlayerTextDrawHide(playerid, pInfoText[playerid]);
	PlayerTextDrawSetString(playerid, pInfoText[playerid], text);
	PlayerTextDrawShow(playerid, pInfoText[playerid]);

	SetTimerEx("DeleteInfoBox", time, false, "d", playerid);
	pInfoBoxShown[playerid] = true;

	return 1;
}

stock Player_InfoStr(playerid, String:string, bool:forced = false, time = 2500)
{
	new ptr[1][] = {{}}, size = str_len(string) + 1, Var:var = amx_alloc(size);
	amx_to_ref(var, ptr);
	str_get(string, ptr[0], .size=size);

	new result = Player_Info(playerid, ptr[0], forced, time);

	amx_free(var);
	amx_delete(var);
	return result;
}

forward DeleteInfoBox(playerid); 
public DeleteInfoBox(playerid)
{
	PlayerTextDrawHide(playerid, pInfoText[playerid]);

	pInfoBoxShown[playerid] = false;

	return 1;
}

stock SendMail( const szReceiver[ ], const szSubject[ ], const szMessage[ ] )
{
	new rec[255], subject[255], mess[255];
	set(rec, szReceiver);
	set(subject, szSubject);
	set(mess, szMessage);

	StringURLEncode(rec);
	StringURLEncode(subject);
	StringURLEncode(mess);

	new str[256];
	format(str, sizeof(str), "%s?sendto=%s&subject=%s&body=%s",
								MAILER_URL, rec, subject, mess);
	printf("%s", str);
	HTTP( 0xD00D, HTTP_GET , str, "", "OnMailScriptResponse" );
}

forward OnMailScriptResponse( iIndex, iResponseCode, const szData[ ] );
public  OnMailScriptResponse( iIndex, iResponseCode, const szData[ ] )
{
	if ( szData[ 0 ] )
		printf( "Mailer script says: %s", szData );
}
stock StringURLEncode( szString[ ], iSize = sizeof( szString ) )
{
	for ( new i = 0, l = strlen( szString ); i < l; i++ )
	{
		switch ( szString[ i ] )
		{
			case '!', '(', ')', '\'', '*',
			     '0' .. '9',
			     'A' .. 'Z',
			     'a' .. 'z':
			{
				continue;
			}
			
			case ' ':
			{
				szString[ i ] = '+';
				
				continue;
			}
		}
		
		new
			s_szHex[ 8 ]
		;
		
		if ( i + 3 >= iSize )
		{
			szString[ i ] = EOS;
			
			break;
		}
		
		if ( l + 3 >= iSize )
			szString[ iSize - 3 ] = EOS;
		
		format( s_szHex, sizeof( s_szHex ), "%02h", szString[ i ] );
		
		szString[ i ] = '%';
		
		strins( szString, s_szHex, i + 1, iSize );
		
		l += 2;
		i += 2;
		
		if ( l > iSize - 1 )
			l = iSize - 1;
	}
}