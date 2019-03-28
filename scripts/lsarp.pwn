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
#include <easyDialogs>
#include <whirlpool>
#include <streamer>
#include <strlib>

#define PP_SYNTAX 1
//#define PP_SYNTAX_GENERIC 1
#define PP_ADDITIONAL_TAGS E_INVENTORY_DATA

#include <PawnPlus>


#include <YSI\y_hooks>
DEFINE_HOOK_REPLACEMENT(ShowRoom, SR);
DEFINE_HOOK_REPLACEMENT(Element, Elm);


#define AC_GivePlayerWeapon     GivePlayerWeapon
#define AC_ResetPlayerWeapons    ResetPlayerWeapons

// Others
#include <utils/colors.pwn>
#include <database/database.pwn>

#include <globals.pwn>
#include <utils/maths.pwn>
#include <sa_zones>

// Callback(s)
#include <global_callbacks.pwn>
#include <inventory/inventory_callbacks.pwn>


// Exception.
#include <pickup/pickup_enum.pwn>

// Include enums on top, to prevents problems like "Undefined ENUM_NAME"
#include <account/account_enum.pwn>
#include <player/player_enum.pwn>
#include <vehicles/vehicles_enum.pwn>
#include <admin/admin_enum.pwn>
#include <showroom/showroom_enum.pwn>
#include <inventory/inventory_enum.pwn>
#include <building/building_enum.pwn>

// Here includes that must hook before everything
#include <player/player.pwn>
#include <pickup/pickup.pwn>
#include <building/building.pwn>

// Others (2)
#include <account/account.pwn>
#include <vehicles/vehicles.pwn>
#include <showroom/showroom.pwn>
#include <admin/admin.pwn>
#include <inventory/inventory.pwn>

//
#include <utils/utils.pwn>
#include <utils/weapon_utils.pwn>
#include <anticheat/anticheat.pwn>
#include <global_timers.pwn>
// Character Creation Steps:
// 0: Name & Surname
// 1: Age Selection
// 2: Sex Selection
// 3: Over
#include <inventory\general\inventory_general.pwn>
main()
{
    printf("LSARP - By CodaKKK. Started: 26/02/2019.");

    /*VehicleInventory = map_new();
    Vehicle_InitializeInventory(5);
    Vehicle_AddItem(0, 5, 1);
    Vehicle_AddItem(5, 5, 1);
    //Vehicle_AddItem(5, 10, 5);
    //Vehicle_AddItem(5, 11, 1);
    //Vehicle_AddItem(5, 12, 6);
    //Vehicle_AddItem(5, 13, 1);
    for_map(i : VehicleInventory)
    {
        new k, List:items;
        iter_get_key_safe(i, k);
        iter_get_value_safe(i, items);
        for(new a = 0; a < list_size(items); a++)
        {
            new item[E_INVENTORY_DATA];
            list_get_arr_safe(items, a, item);
            printf("%s - %d - %d", ServerItem[item[gInvItem]][sitemName], item[gInvAmount], item[gInvExtra]);
        }
    }*/

    for(new i = 0; i < 5; ++i) print("\n");

    print("========== < INVENTORY UNIT TEST > ==========");

    new Inventory:inv = Inventory_New(10);
    printf("Inventory Space: %d", Inventory_GetSpace(inv));

    Inventory_AddItem(inv, gItem_Hamburger, 999, 0);

    Inventory_Print(inv);

    /*for(new i = 1; i <= 20; ++i)
    {
        printf("Has space for %d UNIQUE: %d", i, Inventory_HasSpaceForItem(inv, gItem_RationK, i));  
    }

    for(new i = 1; i <= 21; ++i)
    {
        printf("Has space for %d NON UNIQUE: %d", i, Inventory_HasSpaceForItem(inv, gItem_Hamburger, i));    
    }*/
    Inventory_Delete(inv);

    print("========== < INVENTORY UNIT TEST > ==========");
    for(new i = 0; i < 5; ++i) print("\n");
}

new
    gCharacterCreationStep[MAX_PLAYERS],
    gCharacterSelected[MAX_PLAYERS],
    gCharacterSelectedName[MAX_PLAYERS][MAX_PLAYER_NAME]
    ;
    


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

public OnPlayerClearData(playerid)
{
    CharacterCreation_Reset(playerid);
    return 1;
}

public OnVehicleDeath(vehicleid, killerid)
{
    return 1;
}

public OnVehicleSpawn(vehicleid)
{
    return 1;
}

public OnPlayerExitVehicle(playerid, vehicleid)
{
    return 1;
}

public OnPlayerPickUpDynamicPickup(playerid, pickupid)
{
    return 1;
}

public OnPlayerSpawn(playerid)
{
    return 1;
}

public OnPlayerDeath(playerid, killerid, reason)
{
    return 0;
}

public OnPlayerRequestSpawn(playerid)
{
    return 1;
}

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    return 1;
}

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
    
    CallLocalFunction("OnCharacterSaveData", "i", playerid);
    CallLocalFunction("OnPlayerClearData", "i", playerid);
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
    gCharacterCreationStep[playerid] = -1;

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

public OnPlayerRequestClass(playerid, classid)
{
    if(IsPlayerNPC(playerid))
        return 1;

    if(gCharacterLogged[playerid])
    {
        SpawnPlayer(playerid);
        return 1;
    }

    if(gAccountLogged[playerid] && !gCharacterLogged[playerid])
    {
        ShowCharactersList(playerid);
        return 1;
    }

    new 
        name[24],
        query[128];
    
    GetPlayerName(playerid, name, sizeof(name));

    inline AccountCheckCallback()
    {
        if(cache_num_rows() > 0)
        {
            new id;
            cache_get_value_index_int(0, 0, id);
            Dialog_Show(playerid, Dialog_Login, DIALOG_STYLE_PASSWORD, "Login", "Ciao %s!\nIl tuo account risulta registrato.\nInserisci la password per effettuare il login.", "Login", "Esci", name);
        }
        else
        {
            Dialog_Show(playerid, DialogNull, DIALOG_STYLE_MSGBOX, "Account non registrato", "Questo account non risulta registrato.\nVisita www.lsarp.it per registrare un account.", "Ok", "", name);
        }
    }

    mysql_format(gMySQL, query, sizeof(query), "SELECT ID FROM accounts WHERE LOWER(Name) = LOWER('%e')", name);
    mysql_tquery_inline(gMySQL, query, using inline AccountCheckCallback);
    return 1;
}

Dialog:Dialog_Login(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
        Kick(playerid);
        return 1;
    }

    new query[256];
    inline OnCheck()
    {
        if(LoadAccountData(playerid))
        {
            OnAccountLogin(playerid);
        }
        else
        {
            Dialog_Show(playerid, Dialog_Login, DIALOG_STYLE_PASSWORD, "Login", "{FF0000}Password errata.\n{FFFFFF}Inserisci la tua password per effettuare il login.", "Login", "Esci");

            gLoginAttempts[playerid]++;
            if(gLoginAttempts[playerid] == 3)
            {
                return KickEx(playerid);
            }
        }
    }
    new 
        psw[WHIRLPOOL_LEN], 
        name[24];
    WP_Hash(psw, sizeof(psw), inputtext);    

    GetPlayerName(playerid, name, sizeof(name));

    mysql_format(gMySQL, query, sizeof(query), "SELECT * FROM accounts WHERE LOWER(Name) = LOWER('%e') AND Password = '%e'", name, psw);
    mysql_tquery_inline(gMySQL, query, using inline OnCheck);
    return 1;
}

stock ShowCharactersList(playerid)
{
    if(!gAccountLogged[playerid])
        return 0;

    inline OnLoad()
    {
        new count;
        cache_get_row_count(count);

        if(cache_num_rows() == 0)
        {
            gCharacterCreationStep[playerid] = 0;
            Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Crea nuovo personaggio", "Attualmente non risultano personaggi creati!\nPer poter giocare, devi disporre di un personaggio.\nInserisci il nome del nuovo personaggio.\nEsempio: Mario Rossi", "Ok", "");
            return 1;
        }

        new
            list[24 * MAX_CHARACTERS] = "",
            nameTemp[24];

        if(count > MAX_CHARACTERS)
            count = MAX_CHARACTERS;

        for(new i = 0; i < count; ++i)
        {
            cache_get_value_index_int(i, 0, AccountInfo[playerid][aCharacters][i]);
            cache_get_value_index(i, 1, nameTemp, sizeof(nameTemp));
            format(list, sizeof(list), "%s%s\n", list, nameTemp);
        }
        return Dialog_Show(playerid, Dialog_CharacterSelect, DIALOG_STYLE_LIST, "Gestione Personaggi", "%s", "Utilizza", "Altro", list);
    }

    new 
        query[128];
    
    mysql_format(gMySQL, query, sizeof(query), "SELECT ID, Name FROM characters WHERE AccountID = '%d'", AccountInfo[playerid][aID]);
    mysql_tquery_inline(gMySQL, query, using inline OnLoad);
    return 1;
}

Dialog:Dialog_ShowCharacters(playerid, response, listitem, inputtext[])
{
    if(!gAccountLogged[playerid])
        return KickEx(playerid);

    ShowCharactersList(playerid);
    return 1;
}

Dialog:Dialog_CharCreateOrDel(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
        return ShowCharactersList(playerid);
    }
    if(listitem == 0)
    {
        gCharacterCreationStep[playerid] = 0;
        Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Crea nuovo personaggio", "Inserisci il nome del nuovo personaggio!\nEsempio: Mario Rossi", "Crea", "Indietro");
    }
    else if(listitem == 1)
    {
        new randcode = 10000 + random(99999);
        SetPVarInt(playerid, "DeleteCharacter_Code", randcode);
        return Dialog_Show(playerid, Dialog_CharacterDelete, DIALOG_STYLE_INPUT, "Cancella Personaggio", "Stai per cancellare il personaggio (%s) definitivamente!\nInserisci il codice sottostante per confermare.\nCodice: %d", "Cancella", "Annulla", 
            gCharacterSelectedName[playerid], randcode);
    }
    return 1;
}

Dialog:Dialog_CharacterDelete(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
        return Dialog_Show(playerid, Dialog_CharCreateOrDel, DIALOG_STYLE_LIST, "Personaggi", "Crea nuovo personaggio\nCancella personaggio", "Avanti", "Indietro");
    }
    if(strval(inputtext) == GetPVarInt(playerid, "DeleteCharacter_Code"))
    {
        Character_Delete(playerid, gCharacterSelected[playerid], gCharacterSelectedName[playerid]);
        DeletePVar(playerid, "DeleteCharacter_Code");
        ShowCharactersList(playerid);
        return 1;
    }
    return Dialog_Show(playerid, Dialog_CharacterDelete, DIALOG_STYLE_INPUT, "Cancella Personaggio", "{FF0000}Codice inserito non è corretto!\n{FFFFFF}Stai per cancellare il personaggio (%s) definitivamente!\nInserisci il codice sottostante per confermare.\nCodice: %d", "Cancella", "Annulla", 
            gCharacterSelectedName[playerid], GetPVarInt(playerid, "DeleteCharacter_Code"));
}


Dialog:Dialog_CharacterSelect(playerid, response, listitem, inputtext[])
{
    gCharacterSelected[playerid] = AccountInfo[playerid][aCharacters][listitem];
    if(!response)
    {
        inline OnLoad()
        {
            cache_get_value_index(0, 0, gCharacterSelectedName[playerid]);
        }
        new query[128];
        mysql_format(gMySQL, query, sizeof(query), "SELECT Name FROM characters WHERE ID = '%d'", gCharacterSelected[playerid]);
        mysql_tquery_inline(gMySQL, query, using inline OnLoad);
        return Dialog_Show(playerid, Dialog_CharCreateOrDel, DIALOG_STYLE_LIST, "Personaggi", "Crea nuovo personaggio\nCancella personaggio", "Avanti", "Indietro");
    }

    inline OnLoad()
    {
        OnCharacterLoad(playerid);
    }
    new query[128];
    mysql_format(gMySQL, query, sizeof(query), "SELECT * FROM characters WHERE ID = '%d'", gCharacterSelected[playerid]);
    mysql_tquery_inline(gMySQL, query, using inline OnLoad);
    return 1;
}

Dialog:Dialog_CreateNewChar(playerid, response, listitem, inputtext[])
{
    if(!gAccountLogged[playerid])
        return KickEx(playerid);
    
    if(!response)
    {
        gCharacterCreationStep[playerid]--;
        if(gCharacterCreationStep[playerid] == -1)
            CharacterCreation_Reset(playerid);
        switch(gCharacterCreationStep[playerid])
        {
            case 0: return Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Crea nuovo personaggio", "Inserisci il nome del nuovo personaggio!\nEsempio: Mario Rossi", "Avanti", "Indietro");
            case 1: return Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Età Personaggio", "Adesso inserisci l'età del nuovo personaggio!\nN.B: 16 - 75 anni.", "Avanti", "Indietro");
            case 2: return Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_LIST, "Sesso Personaggio", "Maschio\nFemmina\nLorack", "Crea", "Indietro");
            default: return ShowCharactersList(playerid);
        }
        return 1;
    }

    switch(gCharacterCreationStep[playerid])
    {
        // Name & Surname
        case 0:
        {

            new 
                len = strlen(inputtext),
                retFirst[MAX_PLAYER_NAME],
                retLast[MAX_PLAYER_NAME],
                characterName[MAX_PLAYER_NAME];

            if(len > 22)
            {
                return Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Crea nuovo personaggio", "{FF0000}La lunghezza massima del nome è 22!\n{FFFFFF}Inserisci il nome del nuovo personaggio.\n{00FF00}Esempio: Mario Rossi", "Crea", "Indietro");
            }

            inputlength(inputtext, MAX_PLAYER_NAME);

            for(new i = 0; i < len; ++i)
            {
                characterName[i] = (inputtext[i] == ' ') ? (characterName[i] = '_') : (characterName[i] = inputtext[i]); // () crashes compiler porca madonna
            }

            if(!IsValidRPName(characterName, retFirst, retLast))
            {
                Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Crea nuovo personaggio", "{FF0000}Il nome del nuovo personaggio non è del formato giusto!\n{00FF00}Esempio: Mario Rossi", "Crea", "Indietro");
            }
            else
            {
                inline OnCheck()
                {
                    if(cache_num_rows() > 0)
                    {
                        Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Crea nuovo personaggio", "{FF0000}Esiste già un personaggio con questo nome!\n{FFFFFF}Inserisci il nome del nuovo personaggio.\n{00FF00}Esempio: Mario Rossi", "Crea", "Indietro");
                    }
                    else
                    {
                        /*if(AccountInfo[playerid][aCharactersCount] >= 3 && AccountInfo[playerid][aPremium] == 0)
                        {
                            return Dialog_Show(playerid, Dialog_ShowCharacters, DIALOG_STYLE_MSGBOX, "Personaggio Non Creato", "Non è stato possibile creare il personaggio!\nLimite superato.", "Continua", "", fixedCharName);
                        }*/
                        SetPVarString(playerid, "NewCharacter_Name", characterName);

                        // Move to step 2
                        gCharacterCreationStep[playerid] = 1;

                        Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Età Personaggio", "Adesso inserisci l'età del nuovo personaggio!\nN.B: 16 - 75 anni.", "Avanti", "Indietro");
                    }
                }
                new 
                    query[128];
                mysql_format(gMySQL, query, sizeof(query), "SELECT ID FROM characters WHERE LOWER(Name) = LOWER('%e')", characterName);
                mysql_tquery_inline(gMySQL, query, using inline OnCheck);
                return 1;
            }
        }
        case 1: // Age Selection
        {
            new val = strval(inputtext);
            if(val < 16 || val > 75)
            {
                return Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Età Personaggio", "Adesso inserisci l'età del nuovo personaggio!\nN.B: 16 - 75 anni.", "Avanti", "Indietro");
            }
            gCharacterCreationStep[playerid] = 2;
            SetPVarInt(playerid, "NewCharacter_Age", val);
            return Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_LIST, "Sesso Personaggio", "Maschio\nFemmina\nLorack", "Crea", "Indietro");
        }
        case 2: // Sex Selection
        {
            gCharacterCreationStep[playerid] = 3;
            new
                name[MAX_PLAYER_NAME], // Underscored Name Porca_Madonna
                fixedCharName[MAX_PLAYER_NAME], // Not underscored name. Porca Madonna
                age = GetPVarInt(playerid, "NewCharacter_Age")
                ;
            SetPVarInt(playerid, "NewCharacter_Sex", listitem);
            GetPVarString(playerid, "NewCharacter_Name", name, sizeof(name));
            
            FixName(name, fixedCharName);

            Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_MSGBOX, "Riepilogo", "--- Informazioni ---\nNome: %s\nSesso: %s\nEtà: %d\nConfermi?", "Crea", "Annulla", fixedCharName, GetSexName(listitem), age);
            return 1;
        }
        case 3: // Over
        {
            if(AccountInfo[playerid][aCharactersCount] >= MAX_CHARACTERS) // Should I prompt this before?
            {
                return Dialog_Show(playerid, Dialog_ShowCharacters, DIALOG_STYLE_MSGBOX, "Personaggio Non Creato", "Non è stato possibile creare il personaggio!\nLimite (5) superato.", "Continua", "");
            }
            new 
                query[256],
                age = GetPVarInt(playerid, "NewCharacter_Age"),
                sex = GetPVarInt(playerid, "NewCharacter_Sex"),
                characterName[MAX_PLAYER_NAME];
            
            GetPVarString(playerid, "NewCharacter_Name", characterName, sizeof(characterName));

            mysql_format(gMySQL, query, sizeof(query), "INSERT INTO `characters` (Name, AccountID, Sex, Age, FirstSpawn) VALUES('%e', '%d', '%d', '%d', '1')", characterName, AccountInfo[playerid][aID], sex, age);
            mysql_tquery(gMySQL, query);

            AccountInfo[playerid][aCharactersCount]++;
            Log(AccountInfo[playerid][aName], characterName, "CharacterCreation");
            CharacterCreation_Reset(playerid);
            
            Dialog_Show(playerid, Dialog_ShowCharacters, DIALOG_STYLE_MSGBOX, "Personaggio Creato", "Il personaggio è stato creato con successo!", "Continua", "");
            return 1;
        }
    }
    return 1;
}

// Resets all variables created for CharacterCreationSteps
stock CharacterCreation_Reset(playerid)
{
    gCharacterCreationStep[playerid] = -1;
    DeletePVar(playerid, "NewCharacter_Sex");
    DeletePVar(playerid, "NewCharacter_Age");
    DeletePVar(playerid, "NewCharacter_Name");
    DeletePVar(playerid, "DeleteCharacter_Code");
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