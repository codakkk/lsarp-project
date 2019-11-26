#include <account_system\components\char_select>
#include <reward_system\core>
#include <samp_bcrypt>
#define BCRYPT_COST 10

#include <YSI_Coding\y_hooks>

#define MAX_LOGIN_ATTEMPTS	(3)

forward OnAccountLogin(playerid);

static
	PlayerHash[MAX_PLAYERS][250],
	LoginAttempts[MAX_PLAYERS char] = {0, ...}
;

enum
{
	ADMIN_ROLE_NONE = 0,
	ADMIN_ROLE_PROPERTY = 1,
	ADMIN_ROLE_FACTION = 2
}


static enum E_ACCOUNT_DATA
{
	ID,
	Admin,
	AdminRole,
	ZPoints,
	Premium,
	PremiumExpiry,
	FirstCharacter,
	CharactersSlot,
	CharactersCount,
	PayDay
};

// Test

static enum e_Bit1_AccountData 
{
	e_pAccountLogged,
    e_pTogglePMAll,
    e_pToggleOOCAll,
	e_pSupporterDuty,
};

static 
	AccountInfo[MAX_PLAYERS][E_ACCOUNT_DATA],
	BitArray:gAccountBitState[e_Bit1_AccountData]<MAX_PLAYERS>,

	AccountName[MAX_PLAYERS][MAX_PLAYER_NAME],
	AccountEMail[MAX_PLAYERS][255],
	CleanAccountData[E_ACCOUNT_DATA]
;

hook OnPlayerRequestClass(playerid, classid)
{
	if(IsPlayerNPC(playerid))
		return 1;

	if(Character_IsLogged(playerid))
	{
		defer SpawnTest(playerid);
		return 1;
	}

	if(Account_IsLogged(playerid) && !Character_IsLogged(playerid))
	{
		Account_ShowCharactersList(playerid);
		return 1;
	}

	//CheckAccount(playerid);
	return 1;
}

timer SpawnTest[10](playerid)
{
	SpawnPlayer(playerid);
}

hook OnPlayerDisconnect(playerid)
{

	LoginAttempts{playerid} = 0;
	AccountName[playerid] = "";
	AccountEMail[playerid] = "";
	AccountInfo[playerid] = CleanAccountData;

	// before everything, make sure to reset ToggleOOC and PM from players to this playerid
    foreach(new i : Player)
    {
	   if(Iter_Contains(pTogglePM[i], playerid))
		  Iter_Remove(pTogglePM[i], playerid);
	   if(Iter_Contains(pToggleOOC[i], playerid))
		  Iter_Remove(pToggleOOC[i], playerid);
    }
    Iter_Clear(pTogglePM[playerid]);
    Iter_Clear(pToggleOOC[playerid]);

	Account_ResetBitState(playerid);
	
    return 1;
}

hook OnPlayerConnected(playerid)
{
	new name[MAX_PLAYER_NAME], bool:found = false;
    
    GetPlayerName(playerid, name, sizeof(name));

	foreach(new i : Player)
	{
		if(Account_IsLogged(i) && !strcmp(Account_GetName(i), name, false))
		{
			found = true;
			break;
		}
	}

	if(found)
	{
		SendClientMessage(playerid, COLOR_ERROR, "Questo account risulta già collegato nel server.");
		KickEx(playerid);
		return 0;
	}
	return 1;
}

public OnPlayerRequestClass(playerid, classid)
{
	if(IsPlayerNPC(playerid))
		return 1;
	new name[MAX_PLAYER_NAME];
	GetPlayerName(playerid, name, sizeof(name));
	yield 1;
	await mysql_aquery_s(gMySQL, str_format("SELECT password FROM `accounts` WHERE LOWER(name) = LOWER('%e')", name), true);

	if(cache_num_rows() > 0)
	{
		cache_get_value_name(0, "password", PlayerHash[playerid]);
		ShowLoginDialog(playerid);
	}
	else
	{
		SendClientMessage(playerid, COLOR_YELLOW, "SERVER: Devi avere un account registrato per poter giocare su questo server!");
		SendClientMessage(playerid, COLOR_YELLOW, "Puoi registrarne uno qui: ucp.lv-rp.it");
		KickEx(playerid);
	}
	return 1;
}

static ShowLoginDialog(playerid)
{
	new name[MAX_PLAYER_NAME];
	GetPlayerName(playerid, name, sizeof(name));
	new String:string = str_format("Ciao %s!\nIl tuo account risulta registrato.\nInserisci la password per effettuare il login.\nTentativo %d/%d", name, LoginAttempts{playerid}+1, MAX_LOGIN_ATTEMPTS);
	Dialog_Show_s(playerid, Dialog_Login, DIALOG_STYLE_PASSWORD, @("Login"), string, "Login", "Esci");
}

static CheckForAccountStatus(playerid)
{
	new accountId, isBanned, banTime;
	
	cache_get_value_name_int(0, "id", accountId);
	cache_get_value_name_int(0, "banned", isBanned);
	cache_get_value_name_int(0, "ban_expiry", banTime);

	if(isBanned)
	{
		if(!banTime || gettime() < banTime)
		{
			Dialog_Show(playerid, DialogNull, DIALOG_STYLE_MSGBOX, "Account Bannato", "Questo account risulta bannato.\nSe pensi sia un errore, visita www.lsarp.it", "Ok", "");
			KickEx(playerid);
			return 0;
		}
		else
		{
			mysql_pquery_f(gMySQL, "UPDATE `accounts` SET banned = '0', ban_expiry = '0' WHERE id = '%d'", accountId);
		}
	}
	return 1;
}

Dialog:Dialog_Login(playerid, response, listitem, inputtext[])
{
	if(!response) return KickEx(playerid);
	bcrypt_verify(playerid, #OnPasswordCheck, inputtext, PlayerHash[playerid]);
	return 1;
}

stock LoadAccountData(playerid)
{
	cache_get_value_name_int(0, "id", AccountInfo[playerid][ID]);
	cache_get_value_name(0, "name", AccountName[playerid]);
	cache_get_value_name_int(0, "admin_level", AccountInfo[playerid][Admin]);
	cache_get_value_name_int(0, "admin_role", AccountInfo[playerid][AdminRole]);
	cache_get_value_name_int(0, "premium", AccountInfo[playerid][Premium]);
	cache_get_value_name_int(0, "premium_expiry", AccountInfo[playerid][PremiumExpiry]);
	cache_get_value_name_int(0, "characters_counter", AccountInfo[playerid][CharactersCount]);
	cache_get_value_name(0, "email", AccountEMail[playerid]);
	cache_get_value_name_int(0, "characters_slot", AccountInfo[playerid][CharactersSlot]);
	cache_get_value_name_int(0, "payday", AccountInfo[playerid][PayDay]);

	if(AccountInfo[playerid][Admin] <= 1)
		AccountInfo[playerid][AdminRole] = 0;
}

forward OnPasswordCheck(playerid, bool:success);
public OnPasswordCheck(playerid, bool:success)
{
	if(success)
	{
		new name[MAX_PLAYER_NAME];
		GetPlayerName(playerid, name, sizeof(name));
		

		inline OnLoad()
		{
			//await mysql_aquery_s(gMySQL, str_format("SELECT * FROM `accounts` WHERE LOWER(name) = LOWER('%e')", name));
			//printf("Success after query");
			CheckForAccountStatus(playerid);
			LoadAccountData(playerid);

			Account_SetLogged(playerid, true);

			CallLocalFunction(#OnAccountLogin, "d", playerid);
		}
		MySQL_PQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `accounts` WHERE LOWER(name) = LOWER('%e')", name);

	}
	else
	{
		LoginAttempts{playerid}++;
		if(LoginAttempts{playerid} >= MAX_LOGIN_ATTEMPTS)
		{
			KickEx(playerid);
			return 0;
		}
		ShowLoginDialog(playerid);
	}
	return 1;
}

stock Account_GetNameFromDatabase(accountdbid)
{
	new tmpName[MAX_PLAYER_NAME] = "Nessuno";
	if(accountdbid > 0)
	{
		new query[76];
		mysql_format(gMySQL, query, sizeof(query), "SELECT name FROM `accounts` WHERE id = '%d';", accountdbid);
		new Cache:cache = mysql_query(gMySQL, query, true);
		if(cache_num_rows() > 0)
		{
			cache_get_value_index(0, 0, tmpName, sizeof(tmpName));
		}
		cache_delete(cache);
	}
	return tmpName;
}


// ======================= ACCESSORS ========================================

stock Account_ResetBitState(playerid)
{
	for(new i = 0; e_Bit1_AccountData:i < e_Bit1_AccountData; i++)
	{
		Bit_Set(gAccountBitState[e_Bit1_AccountData:i], playerid, false);
	}
}

stock Account_GetName(playerid)
{
	return AccountName[playerid];
}

stock Account_SetEMail(playerid, email[])
{
	format(AccountEMail[playerid], 255, "%s", email);
	return 1;
}

stock Account_GetEMail(playerid)
{
	return AccountEMail[playerid];
}

stock Account_SetLogged(playerid, bool:logged)
{
	Bit_Set(gAccountBitState[e_pAccountLogged], playerid, logged);
}

stock Account_IsLogged(playerid)
{
	return Bit_Get(gAccountBitState[e_pAccountLogged], playerid);
}

stock Account_SetPMAllEnabled(playerid, bool:enable)
{
	Bit_Set(gAccountBitState[e_pTogglePMAll], playerid, !enable);
}

stock Account_HasPMAllEnabled(playerid)
{
	return !Bit_Get(gAccountBitState[e_pTogglePMAll], playerid);
}

stock Account_SetOOCEnabled(playerid, bool:enable)
{
	Bit_Set(gAccountBitState[e_pToggleOOCAll], playerid, !enable);
}

stock Account_HasOOCEnabled(playerid)
{
	return !Bit_Get(gAccountBitState[e_pToggleOOCAll], playerid);
}

stock Account_HasOOCEnabledForPlayer(playerid, targetid)
{
	return !Iter_Contains(pToggleOOC[playerid], targetid);
}

stock Account_GetID(playerid)
{
	return AccountInfo[playerid][ID];
}

stock Account_SetAdminLevel(playerid, level)
{
	AccountInfo[playerid][Admin] = level;
	mysql_pquery_f(gMySQL, "UPDATE `accounts` SET admin_level = '%d' WHERE id = '%d'", level, Account_GetID(playerid));
	return 1;
}

stock Account_GetAdminLevel(playerid)
{
	return AccountInfo[playerid][Admin];
}

stock Account_SetAdminRole(playerid, role)
{
	AccountInfo[playerid][AdminRole] = role;
	mysql_pquery_f(gMySQL, "UPDATE `accounts` SET admin_role = '%d' WHERE id = '%d'", role, Account_GetID(playerid));
	return 1;
}

stock Account_GetAdminRole(playerid)
{
	return AccountInfo[playerid][AdminRole];
}

stock Account_SetPremiumLevel(playerid, level)
{
	AccountInfo[playerid][Premium] = level;
	mysql_pquery_f(gMySQL, "UPDATE `accounts` SET premium = '%d' WHERE id = '%d'", level, Account_GetID(playerid));
	return 1;
}

stock Account_GetPremiumLevel(playerid)
{
	return AccountInfo[playerid][Premium];
}

stock Account_SetPremiumExpiry(playerid, time)
{
	AccountInfo[playerid][PremiumExpiry] = time;
	mysql_pquery_f(gMySQL, "UPDATE `accounts` SET premium_expiry = '%d' WHERE id = '%d'", time, Account_GetID(playerid));
	return 1;
}

stock Account_GetPremiumExpiry(playerid)
{
	return AccountInfo[playerid][PremiumExpiry];
}

stock Account_GetCharactersSlot(playerid)
{
	return AccountInfo[playerid][CharactersSlot];
}

stock Account_SetCharactersSlot(playerid, setValue)
{
	if(!Account_IsLogged(playerid))
		return 0;
	AccountInfo[playerid][CharactersSlot] = setValue;
	mysql_pquery_f(gMySQL, "UPDATE `accounts` SET characters_slot = '%d' WHERE id = '%d'", Account_GetCharactersSlot(playerid), Account_GetID(playerid));
	return 1;
}

stock Account_IncreaseCharactersSlot(playerid, amount)
{
	return Account_SetCharactersSlot(playerid, Account_GetCharactersSlot(playerid) + amount);
}

stock Account_GetCharactersCount(playerid)
{
	return AccountInfo[playerid][CharactersCount];
}

stock Account_AddCharactersCount(playerid, add)
{
	AccountInfo[playerid][CharactersCount] += add;
	mysql_pquery_f(gMySQL, "UPDATE `accounts` SET characters_counter = '%d' WHERE id = '%d'", Account_GetCharactersCount(playerid), Account_GetID(playerid));
}

stock Account_GetOwnableVehicleCount(playerid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	static const ownable[] = {3, 8, 10, 12};
	return ownable[Account_GetPremiumLevel(playerid)];
}