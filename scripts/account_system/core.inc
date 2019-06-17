#include <account_system\accessors>
#include <YSI_Coding\y_hooks>

hook OnPlayerClearData(playerid)
{
    CharacterCreation_Reset(playerid);
	gLoginTimer[playerid] = Timer:0;

	new Data[E_ACCOUNT_DATA];
	AccountInfo[playerid] = Data;

	new rData[E_PLAYER_REWARD_DATA];
	PlayerRewardData[playerid] = rData;

	gAccountTempMail[playerid] = "";
	gAccountEmailCode[playerid] = 0;

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

stock OnAccountCreated(playerid)
{
	#pragma unused playerid
}

stock CheckAccount(playerid)
{
	new 
	   String:string,
	   name[24];
    
    GetPlayerName(playerid, name, sizeof(name));

	foreach(new i : Player)
	{
		if(Account_IsLogged(i) && !strcmp(AccountInfo[i][aName], name, false))
		{
			KickEx(playerid);
			SendClientMessage(playerid, COLOR_ERROR, "Questo account risulta gi� collegato nel server.");
			return 0;
		}
	}

	inline AccountCheckCallback()
    {
		if(cache_num_rows() > 0)
		{
			new id;
			cache_get_value_index_int(0, 0, id);
			string = str_format("Ciao %s!\nIl tuo account risulta registrato.\nInserisci la password per effettuare il login.", name);
			Dialog_Show_s(playerid, Dialog_Login, DIALOG_STYLE_PASSWORD, @("Login"), string, "Login", "Esci");
		}
		else
		{
			string = str_format("Benvenuto %s.\nQuesto account non risulta registrato.\nInserisci una password per registrarti!\nN.B: Minimo 6 caratteri. Massimo 16.", name);
			Dialog_Show_s(playerid, Dialog_Register, DIALOG_STYLE_PASSWORD, @("Account non registrato"), string, "Ok", "");
		}
    }
    MySQL_TQueryInline(gMySQL, using inline AccountCheckCallback, "SELECT ID FROM accounts WHERE LOWER(Name) = LOWER('%e')", name);
	return 1;
}

hook OnPlayerConnect(playerid)
{
	gLoginTimer[playerid] = defer LoginKickTimer(playerid);
	return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
	stop gLoginTimer[playerid];
	gLoginTimer[playerid] = Timer:0;
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

stock Account_CreateCharacter(playerid)
{

}

stock OnAccountLogin(playerid)
{
	Account_SetLogged(playerid, true);
	stop gLoginTimer[playerid];
	gLoginTimer[playerid] = Timer:0;
    
	if(Account_IsBanned(playerid))
	{
		if(!Account_GetBanExpiry(playerid) || gettime() < Account_GetBanExpiry(playerid))
		{
			Dialog_Show(playerid, DialogNull, DIALOG_STYLE_MSGBOX, "Account Bannato", "Questo account risulta bannato.\nSe pensi sia un errore, visita www.lsarp.it", "Ok", "");
			KickEx(playerid);
			return 0;
		}
		else
		{
			Account_SetBanned(playerid, 0);
			Account_SetBanExpiry(playerid, 0);
			Account_Save(playerid);
		}
	}



	Account_LoadRewards(playerid);

    inline OnLoad()
    {
	   //cache_get_row_count(AccountInfo[playerid][aCharactersCount]);
	   Account_ShowCharactersList(playerid);
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT ID FROM characters WHERE AccountID = '%d'", AccountInfo[playerid][aID]);
	return 1;
}

stock LoadAccountData(playerid)
{
	if(cache_num_rows() > 0)
	{
		cache_get_value_index_int(0, 0, AccountInfo[playerid][aID]);
		cache_get_value_index(0, 1, AccountInfo[playerid][aName]);
		cache_get_value_index_int(0, 3, AccountInfo[playerid][aAdmin]);
		cache_get_value_index_int(0, 4, AccountInfo[playerid][aPremium]);
		cache_get_value_index_int(0, 5, AccountInfo[playerid][aPremiumExpiry]);
		cache_get_value_index_int(0, 6, AccountInfo[playerid][aCharactersCount]);
		cache_get_value_index(0, 7, AccountInfo[playerid][aEMail]);
		cache_get_value_index_int(0, 8, AccountInfo[playerid][aZPoints]);
		cache_get_value_index_int(0, 9, AccountInfo[playerid][aCharactersSlot]);
		cache_get_value_index_int(0, 10, AccountInfo[playerid][aBanned]);
		cache_get_value_index_int(0, 11, AccountInfo[playerid][aBanExpiry]);
		return 1;
	}
	return 0;
}

stock Account_Save(playerid)
{
    if(!Account_IsLogged(playerid))
	   return 0;
    new query[512];
    mysql_format(gMySQL, query, sizeof(query), "UPDATE `accounts` SET \
		Admin = '%d', \
		Premium = '%d', PremiumExpiry = '%d', \
		CharactersCounter = '%d', \
		EMail = '%e', \
		CharactersSlot = '%d', \
		Banned = '%d', \
		BanExpiry = '%d' \
		WHERE ID = '%d'", 
		Account_GetAdminLevel(playerid), 
		Account_GetPremiumLevel(playerid), Account_GetPremiumExpiry(playerid),
		Account_GetCharactersCount(playerid), 
		AccountInfo[playerid][aEMail],
		Account_GetCharactersSlot(playerid),
		Account_IsBanned(playerid),
		Account_GetBanExpiry(playerid),
		Account_GetID(playerid));
    
    mysql_tquery(gMySQL, query);
    return 1;
}

stock Account_SaveZPoints(playerid)
{
	new query[256];
	mysql_format(gMySQL, query, sizeof(query), "UPDATE `accounts` SET ZPoints = '%d' WHERE ID = '%d'", 
				Account_GetZPoints(playerid), Account_GetID(playerid));
    
    mysql_tquery(gMySQL, query);
}

stock Account_SaveRewards(playerid)
{
	new query[512];
	mysql_format(gMySQL, query, sizeof(query), "INSERT INTO `account_rewards` \
			(AccountID, ChangeName, BlockPM, BlockOOC) \
			VALUES('%d', '%d', '%d', '%d') \
			ON DUPLICATE KEY UPDATE \
			ChangeName = VALUES(ChangeName), \
			BlockPM = VALUES(BlockPM), \
			BlockOOC = VALUES(BlockOOC)",
			Account_GetID(playerid),
			PlayerRewardData[playerid][rChangeName],
			PlayerRewardData[playerid][rBlockPM],
			PlayerRewardData[playerid][rBlockOOC]);
    
    mysql_tquery(gMySQL, query);
}

stock Account_LoadRewards(playerid)
{
	inline OnLoad()
	{
		if(cache_num_rows() > 0)
		{
			cache_get_value_index_int(0, 1, PlayerRewardData[playerid][rChangeName]);
			cache_get_value_index_int(0, 2, PlayerRewardData[playerid][rBlockPM]);
			cache_get_value_index_int(0, 3, PlayerRewardData[playerid][rBlockOOC]);
		}
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `account_rewards` WHERE AccountID = '%d'", Account_GetID(playerid));
}

stock Account_ShowCharactersList(playerid)
{
    if(!Account_IsLogged(playerid))
	   return 0;

    inline OnLoad()
    {
		new count;
		cache_get_row_count(count);

		if(cache_num_rows() == 0)
		{
			gCharacterCreationStep{playerid} = 0;
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
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT ID, Name FROM characters WHERE AccountID = '%d'", AccountInfo[playerid][aID]);
    return 1;
}

// Resets all variables created for CharacterCreationSteps
stock CharacterCreation_Reset(playerid)
{
    gCharacterCreationStep{playerid} = -1;
    DeletePVar(playerid, "NewCharacter_Sex");
    DeletePVar(playerid, "NewCharacter_Age");
    DeletePVar(playerid, "NewCharacter_Name");
    DeletePVar(playerid, "DeleteCharacter_Code");
}

timer LoginKickTimer[1000 * 60](playerid) 
{
	if(Account_IsLogged(playerid))
		return 0;
	SendClientMessage(playerid, COLOR_ERROR, "Sei stato kickato per inattivit�.");
	KickEx(playerid);
	return 1;
}