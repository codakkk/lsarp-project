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
	Banned,
	BanExpiry,
	Level,
	PayDay
};

static enum e_Bit1_AccountData 
{
	e_pAccountLogged,
    e_pTogglePMAll,
    e_pToggleOOCAll,
    e_pHotKeys,
	e_pInvMode, // 0: Dialog - 1: Chat
	e_pToggleHUD,
	e_pAdminDuty,
	e_pSupporterDuty
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
			p_name[24];
			
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
			#if ALLOW_NEW_USERS
				string = str_format("Benvenuto %s.\nQuesto account non risulta registrato.\nInserisci una password per registrarti!\nN.B: Minimo 6 caratteri. Massimo 16.", p_name);
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

stock Account_CreateCharacter(playerid, const characterName[MAX_PLAYER_NAME])
{
	inline OnCreate()
	{	
		CallLocalFunction(#OnCharacterCreated, "dd", playerid, cache_insert_id());
	}
	MySQL_TQueryInline(gMySQL, using inline OnCreate, "INSERT INTO `characters` (name, account_id) VALUES('%e', '%d')", characterName, Account_GetID(playerid));
	Log(Account_GetName(playerid), characterName, "CharacterCreation");
	return 1;
}

stock Account_Save(playerid)
{
    if(!Account_IsLogged(playerid))
	   return 0;
    new query[512];
    mysql_format(gMySQL, query, sizeof(query), "UPDATE `accounts` SET \
		admin_level = '%d', \
		premium = '%d', premium_expiry = '%d', \
		characters_counter = '%d', \
		email = '%e', \
		characters_slot = '%d', \
		banned = '%d', \
		ban_expiry = '%d', \
		level = '%d', \
		payday = '%d' \
		WHERE id = '%d'", 
		Account_GetAdminLevel(playerid), 
		Account_GetPremiumLevel(playerid), Account_GetPremiumExpiry(playerid),
		Account_GetCharactersCount(playerid), 
		Account_GetEMail(playerid),
		Account_GetCharactersSlot(playerid),
		Account_IsBanned(playerid),
		Account_GetBanExpiry(playerid),
		Account_GetLevel(playerid),
		Account_GetPayDay(playerid),
		Account_GetID(playerid));
    
    mysql_tquery(gMySQL, query);
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
		cache_get_value_index_int(0, 0, AccountInfo[playerid][ID]);
		cache_get_value_index(0, 1, AccountName[playerid]);
		cache_get_value_index_int(0, 3, AccountInfo[playerid][Admin]);
		cache_get_value_index_int(0, 4, AccountInfo[playerid][Premium]);
		cache_get_value_index_int(0, 5, AccountInfo[playerid][PremiumExpiry]);
		cache_get_value_index_int(0, 6, AccountInfo[playerid][CharactersCount]);
		cache_get_value_index(0, 7, AccountEMail[playerid]);
		cache_get_value_index_int(0, 8, AccountInfo[playerid][CharactersSlot]);
		cache_get_value_index_int(0, 9, AccountInfo[playerid][Banned]);
		cache_get_value_index_int(0, 10, AccountInfo[playerid][BanExpiry]);
		cache_get_value_index_int(0, 11, AccountInfo[playerid][Level]);
		cache_get_value_index_int(0, 12, AccountInfo[playerid][PayDay]);
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

stock Account_SetHUDEnabled(playerid, bool:enable)
{
	Bit_Set(gAccountBitState[e_pToggleHUD], playerid, !enable);
}

stock Account_HasHUDEnabled(playerid)
{
	return !Bit_Get(gAccountBitState[e_pToggleHUD], playerid);
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

stock Account_SetHotKeysEnabled(playerid, bool:enable)
{
	Bit_Set(gAccountBitState[e_pHotKeys], playerid, enable);
}

stock Account_HasHotKeysEnabled(playerid)
{
	return Bit_Get(gAccountBitState[e_pHotKeys], playerid);
}

stock Account_SetInvModeEnabled(playerid, bool:enable)
{
	Bit_Set(gAccountBitState[e_pInvMode], playerid, enable);
}

stock Account_HasInvModeEnabled(playerid)
{
	return Bit_Get(gAccountBitState[e_pInvMode], playerid);
}

stock Account_SetAdminDuty(playerid, bool:enable)
{
	Bit_Set(gAccountBitState[e_pAdminDuty], playerid, enable);
}

stock Account_IsAdminDuty(playerid)
{
	return Bit_Get(gAccountBitState[e_pAdminDuty], playerid);
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
}

stock Account_GetAdminLevel(playerid)
{
	return AccountInfo[playerid][Admin];
}

stock Account_SetPremiumLevel(playerid, level)
{
	AccountInfo[playerid][Premium] = level;
}

stock Account_GetPremiumLevel(playerid)
{
	return AccountInfo[playerid][Premium];
}

stock Account_SetPremiumExpiry(playerid, time)
{
	AccountInfo[playerid][PremiumExpiry] = time;
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
	AccountInfo[playerid][CharactersSlot] = setValue;
}

stock Account_IncreaseCharactersSlot(playerid, amount)
{
	AccountInfo[playerid][CharactersSlot] += amount;
}

stock Account_GetCharactersCount(playerid)
{
	return AccountInfo[playerid][CharactersCount];
}

stock Account_AddCharactersCount(playerid, add)
{
	AccountInfo[playerid][CharactersCount] += add;
}

stock Account_SetBanned(playerid, ban)
{
	AccountInfo[playerid][Banned] = ban;
}

stock Account_IsBanned(playerid)
{
	return AccountInfo[playerid][Banned];
}

stock Account_SetBanExpiry(playerid, time)
{
	AccountInfo[playerid][BanExpiry] = time;
}

stock Account_GetBanExpiry(playerid)
{
	return AccountInfo[playerid][BanExpiry];
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