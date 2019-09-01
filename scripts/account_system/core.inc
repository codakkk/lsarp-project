#include <account_system\components\login>
#include <account_system\components\register>
#include <account_system\components\char_select>
#include <reward_system\core>

#include <YSI_Coding\y_hooks>

forward OnAccountLogin(playerid);
forward OnAccountCreated(playerid, account_db_id);
forward OnCharacterCreated(playerid, character_db_id);

static enum E_ACCOUNT_DATA
{
	ID,
	Admin,
	ZPoints,
	Premium,
	PremiumExpiry,
	FirstCharacter,
	CharactersSlot,
	CharactersCount,
	Level,
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

hook OnPlayerClearData(playerid)
{

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

stock CheckAccount(playerid)
{
	printf("CheckAccount called");
	new 
	   name[24];
    
    GetPlayerName(playerid, name, sizeof(name));

	foreach(new i : Player)
	{
		if(Account_IsLogged(i) && !strcmp(Account_GetName(i), name, false))
		{
			KickEx(playerid);
			SendClientMessage(playerid, COLOR_ERROR, "Questo account risulta già collegato nel server.");
			return 0;
		}
	}

	inline AccountCheckCallback()
    {
		new 
			String:string,
			p_name[MAX_PLAYER_NAME];
			
		GetPlayerName(playerid, p_name, sizeof(p_name));
		printf("AccountCheckCallback");
		if(cache_num_rows() > 0)
		{
			new id;
			cache_get_value_index_int(0, 0, id);
			string = str_format("Ciao %s!\nIl tuo account risulta registrato.\nInserisci la password per effettuare il login.", p_name);
			Dialog_Show_s(playerid, Dialog_Login, DIALOG_STYLE_PASSWORD, @("Login"), string, "Login", "Esci");
			printf("Dialog_Login");
		}
		else
		{
			new bool:isValid = true;
			for(new i = 0, j = strlen(p_name); i < j; ++i)
			{
				if(p_name[i] == '_')
				{
					isValid = false;
					break;
				}
			}
			if(!isValid)
			{
				Dialog_Show(playerid, DialogNull, DIALOG_STYLE_MSGBOX, "Account non valido", "Il tuo nome contiene caratteri non validi (_).\nRilogga con un account valido.", "Ok", "");
				KickEx(playerid);
				return 1;
			}
			#if ALLOW_NEW_USERS
				string = str_format("Benvenuto %s.\nQuesto account non risulta registrato.\nInserisci una password per registrarti!\nN.B: Minimo 6 caratteri. Massimo 16.\n\nRicorda che la password deve contenere almeno:\n- 8 caratteri.\n- Almeno una lettera maiuscola.\n- Almeno un numero.\n- Almeno un carattere speciale.\nLa password non puo':\n- Cominciare o finire con uno spazio\n- Contenere come caratteri speciali: %%, \\, ', `, \".", p_name);
				Dialog_Show_s(playerid, Dialog_Register, DIALOG_STYLE_PASSWORD, @("Account non registrato"), string, "Ok", "");
			#else
				Dialog_Show_s(playerid, Dialog_Register, DIALOG_STYLE_PASSWORD, @("Account non registrato"), @("Benvenuto.\nQuesto account non risulta registrato.\nAttualmente non è possibile effettuare l'accesso.\nSe fai parte del team dei tester, contatta Coda.\nBYE"), "Oh shit", "");
				KickEx(playerid);
			#endif
			printf("Dialog_Register ");
		}
    }
    MySQL_TQueryInline(gMySQL, using inline AccountCheckCallback, "SELECT id FROM accounts WHERE LOWER(name) = LOWER('%e')", name);
	return 1;
}

hook OnPlayerRequestClass(playerid, classid)
{
	if(IsPlayerNPC(playerid))
		return 1;

	if(Character_IsLogged(playerid))
	{
		SpawnPlayer(playerid);
		return 1;
	}

	if(Account_IsLogged(playerid) && !Character_IsLogged(playerid))
	{
		Account_ShowCharactersList(playerid);
		return 1;
	}

	CheckAccount(playerid);
	return 1;
}

stock Account_CreateCharacter(playerid, const characterName[MAX_PLAYER_NAME], skinid = 46)
{
	inline OnCreate()
	{	
		CallLocalFunction(#OnCharacterCreated, "dd", playerid, cache_insert_id());
	}
	MySQL_TQueryInline(gMySQL, using inline OnCreate, "INSERT INTO `characters` (name, account_id, skin) VALUES('%e', '%d', '%d')", characterName, Account_GetID(playerid), skinid);
	Log(Account_GetName(playerid), characterName, "CharacterCreation");
	return 1;
}

timer LoginKickTimer[1000 * 60](playerid) 
{
	if(Account_IsLogged(playerid))
		return 0;
	SendClientMessage(playerid, COLOR_ERROR, "Sei stato kickato per inattività.");
	KickEx(playerid);
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

stock LoadAccountData(playerid)
{
	if(cache_num_rows() > 0)
	{
		cache_get_value_name_int(0, "id", AccountInfo[playerid][ID]);
		cache_get_value_name(0, "name", AccountName[playerid]);
		cache_get_value_name_int(0, "admin_level", AccountInfo[playerid][Admin]);
		cache_get_value_name_int(0, "premium", AccountInfo[playerid][Premium]);
		cache_get_value_name_int(0, "premium_expiry", AccountInfo[playerid][PremiumExpiry]);
		cache_get_value_name_int(0, "characters_counter", AccountInfo[playerid][CharactersCount]);
		cache_get_value_name(0, "email", AccountEMail[playerid]);
		cache_get_value_name_int(0, "characters_slot", AccountInfo[playerid][CharactersSlot]);
		cache_get_value_name_int(0, "level", AccountInfo[playerid][Level]);
		cache_get_value_name_int(0, "payday", AccountInfo[playerid][PayDay]);

		SetPlayerScore(playerid, AccountInfo[playerid][Level]);
		return 1;
	}
	return 0;
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

stock Account_GetPayDay(playerid)
{
	return AccountInfo[playerid][PayDay];
}

stock Account_AddPayDay(playerid, add)
{
	AccountInfo[playerid][PayDay] += add;
	mysql_tquery_f(gMySQL, "UPDATE `accounts` SET payday = '%d' WHERE id = '%d'", Account_GetPayDay(playerid), Account_GetID(playerid));
	return 1;
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

stock Account_SetSupporterDuty(playerid, bool:enable)
{
	Bit_Set(gAccountBitState[e_pSupporterDuty], playerid, enable);
}

stock Account_IsSupporterDuty(playerid)
{
	return Bit_Get(gAccountBitState[e_pSupporterDuty], playerid);
}

stock Account_GetID(playerid)
{
	return AccountInfo[playerid][ID];
}

stock Account_SetAdminLevel(playerid, level)
{
	AccountInfo[playerid][Admin] = level;
	mysql_tquery_f(gMySQL, "UPDATE `accounts` SET admin_level = '%d' WHERE id = '%d'", level, Account_GetID(playerid));
	return 1;
}

stock Account_GetAdminLevel(playerid)
{
	return AccountInfo[playerid][Admin];
}

stock Account_SetPremiumLevel(playerid, level)
{
	AccountInfo[playerid][Premium] = level;
	mysql_tquery_f(gMySQL, "UPDATE `accounts` SET premium = '%d' WHERE id = '%d'", level, Account_GetID(playerid));
	return 1;
}

stock Account_GetPremiumLevel(playerid)
{
	return AccountInfo[playerid][Premium];
}

stock Account_SetPremiumExpiry(playerid, time)
{
	AccountInfo[playerid][PremiumExpiry] = time;
	mysql_tquery_f(gMySQL, "UPDATE `accounts` SET premium_expiry = '%d' WHERE id = '%d'", time, Account_GetID(playerid));
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
	mysql_tquery_f(gMySQL, "UPDATE `accounts` SET characters_slot = '%d' WHERE id = '%d'", Account_GetCharactersSlot(playerid), Account_GetID(playerid));
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
	mysql_tquery_f(gMySQL, "UPDATE `accounts` SET characters_counter = '%d' WHERE id = '%d'", Account_GetCharactersCount(playerid), Account_GetID(playerid));
}

stock Account_SetLevel(playerid, level)
{
	AccountInfo[playerid][Level] = level;
	SetPlayerScore(playerid, level);
	mysql_tquery_f(gMySQL, "UPDATE `accounts` SET level = '%d' WHERE id = '%d'", level, Account_GetID(playerid));
}

stock Account_IncreaseLevel(playerid, amount)
{
	Account_SetLevel(playerid, Account_GetLevel(playerid) + amount);
}

stock Account_GetLevel(playerid)
{
	return AccountInfo[playerid][Level];
}

stock Account_GetOwnableVehicleCount(playerid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	static const ownable[] = {3, 8, 10, 12};
	return ownable[Account_GetPremiumLevel(playerid)];
}