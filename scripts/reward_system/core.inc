
#include <YSI_Coding\y_hooks>

forward OnCharacterChangeName(playerid, newCharacterName[]);

static enum e_RewardData
{
	rdPoints,
	rdName[32]
};
static const RewardInfo[][e_RewardData] =
{
	{250, "Cambio Nome"},
	{300, "Block PM"},
	{300, "Block OOC"},
	{500, "Premium Nerone (1 mese)"},
	{750, "Premium Caligola (1 mese)"},
	{1000, "Premium Giulio Cesare (1 mese)"},
	{500, "+1 Slot Personaggio"},
	{50, "+1 Livello"}
};

static enum E_PLAYER_REWARD_DATA
{
    ZPoints,
	ChangeName,
	BlockPM,
	BlockOOC
};
static 
    PlayerRewardData[MAX_PLAYERS][E_PLAYER_REWARD_DATA],
    CleanRewardData[E_PLAYER_REWARD_DATA];

hook OnCharacterClearData(playerid)
{
	PlayerRewardData[playerid] = CleanRewardData;
    return 1;
}

hook OnAccountLogin(playerid)
{
	Account_LoadRewards(playerid);
	return 1;
}

stock Account_LoadRewards(playerid)
{
	inline OnLoad()
	{
		if(cache_num_rows() > 0)
		{
            cache_get_value_name_int(0, "z_points", PlayerRewardData[playerid][ZPoints]);
			cache_get_value_name_int(0, "change_name", PlayerRewardData[playerid][ChangeName]);
			cache_get_value_name_int(0, "block_pm", PlayerRewardData[playerid][BlockPM]);
			cache_get_value_name_int(0, "block_ooc", PlayerRewardData[playerid][BlockOOC]);
		}
		else
		{
			mysql_tquery_f(gMySQL, "INSERT INTO `account_rewards` (account_id) VALUES('%d')", Account_GetID(playerid));
		}
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `account_rewards` WHERE account_id = '%d'", Account_GetID(playerid));
}

stock Account_SetZPoints(playerid, points)
{
	if(!Character_IsLogged(playerid))
		return 0;
	PlayerRewardData[playerid][ZPoints] = points;
	mysql_tquery_f(gMySQL, "UPDATE `account_rewards` SET z_points = '%d' WHERE account_id = '%d'", points, Account_GetID(playerid));
	return 1;
}

stock Account_AddPoints(playerid, points)
{
	return Account_SetZPoints(playerid, Account_GetZPoints(playerid) + points);
}

stock Account_GetZPoints(playerid)
{
	return PlayerRewardData[playerid][ZPoints];
}

stock Account_AddChangeName(playerid, amount)
{
    if(!Account_IsLogged(playerid))
        return 0;
    PlayerRewardData[playerid][ChangeName] += amount;
	mysql_tquery_f(gMySQL, "UPDATE `account_rewards` SET change_name = '%d' WHERE account_id = '%d'", Account_GetChangeNameCount(playerid), Account_GetID(playerid));
    return 1;
}

stock Account_HasChangeName(playerid)
{
    if(!Account_IsLogged(playerid))
        return 0;
    return PlayerRewardData[playerid][ChangeName] > 0;
}

stock Account_GetChangeNameCount(playerid)
{
    if(!Account_IsLogged(playerid))
        return 0;
    return PlayerRewardData[playerid][ChangeName];
}

stock Account_SetBlockPM(playerid, value)
{
    if(!Account_IsLogged(playerid))
        return 0;
    PlayerRewardData[playerid][BlockPM] = value;
	mysql_tquery_f(gMySQL, "UPDATE `account_rewards` SET block_pm = '%d' WHERE account_id = '%d'", Account_HasBlockPM(playerid), Account_GetID(playerid));
    return 1;
}

stock Account_HasBlockPM(playerid)
{
    if(!Account_IsLogged(playerid))
        return 0;
    return PlayerRewardData[playerid][BlockPM] > 0;
}

stock Account_HasBlockOOC(playerid)
{
    if(!Account_IsLogged(playerid))
        return 0;
    return PlayerRewardData[playerid][BlockOOC] > 0;
}

stock Account_SetBlockOOC(playerid, value)
{
    if(!Account_IsLogged(playerid))
        return 0;
    PlayerRewardData[playerid][BlockOOC] = value;
	mysql_tquery_f(gMySQL, "UPDATE `account_rewards` SET block_ooc = '%d' WHERE account_id = '%d'", Account_HasBlockOOC(playerid), Account_GetID(playerid));
    return 1;
}

stock RewardInfo_GetPoints(rewardid)
{
	return RewardInfo[rewardid][rdPoints];
}

stock RewardInfo_GetName(rewardid)
{
	return RewardInfo[rewardid][rdName];
}

flags:zpoints(CMD_USER);
CMD:zpoints(playerid, params[])
{
	Dialog_Show_s(playerid, Dialog_ZPoints, DIALOG_STYLE_LIST, str_format("Hai %d ZPoints", Account_GetZPoints(playerid)), @("Acquista\nUtilizza"), "Avanti", "chiudi");
	return 1;
}

Dialog:Dialog_ZPoints(playerid, response, listitem, inputtext[])
{
	if(!response)	
		return 0;
	if(listitem == 0)
	{
		new String:str = @("Premio\tZ-Points\n");
		for(new i = 0, j = sizeof(RewardInfo); i < j; ++i)
		{
			str += str_format("%s\t{FFFFFF}%dz{FFFFFF}\n", RewardInfo[i][rdName], RewardInfo_GetPoints(i));
		}
		Dialog_Show_s(playerid, Dialog_ZPointsBuyRewards, DIALOG_STYLE_TABLIST_HEADERS, str_format("Hai %d ZPoints", Account_GetZPoints(playerid)), str, "Acquista", "Annulla");
	}
	else if(listitem == 1)
	{
		Dialog_Show(playerid, Dialog_ZPointsUseReward, DIALOG_STYLE_LIST, "Utilizza Premi", "Cambio Nome", "Acquista", "Annulla");
	}
	return 1;
}

Dialog:Dialog_ZPointsUseReward(playerid, response, listitem, inputtext[])
{
	if(!response)
		return pc_cmd_zpoints(playerid, "");
	switch(listitem)
	{
		case 0:
		{
			if(!Account_HasChangeName(playerid))
				return SendClientMessage(playerid, COLOR_ERROR, "Non disponi di cambi nomi.");
			Dialog_Show(playerid, Dialog_ChangeName, DIALOG_STYLE_INPUT, "Cambio Nome", "Inserisci il nuovo nome per il personaggio.", "Cambia", "Annulla");
			return 1;
		}
	}
	return 1;
}

Dialog:Dialog_ChangeName(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;
	
	if(strlen(inputtext) > 22)
		return Dialog_Show(playerid, Dialog_ChangeName, DIALOG_STYLE_INPUT, "Cambio Nome", "{FF0000}Il nome è troppo lungo (22 caratteri max).{FFFFFF}\nInserisci il nuovo nome per il personaggio.", "Cambia", "Annulla");
	
	inputlength(inputtext, MAX_PLAYER_NAME);
	
	new 
		retFirst[MAX_PLAYER_NAME],
		retLast[MAX_PLAYER_NAME];

	// Must be static or breaks the inline functions
	static characterName[MAX_PLAYER_NAME];

	for(new i = 0, len = strlen(inputtext); i < len; ++i)
	{
		characterName[i] = (inputtext[i] == ' ') ? (characterName[i] = '_') : (characterName[i] = inputtext[i]); // () crashes compiler porca madonna
	}
	
	if(!IsValidRPName(characterName, retFirst, retLast))
		return Dialog_Show(playerid, Dialog_ChangeName, DIALOG_STYLE_INPUT, "Cambio Nome", "{FF0000}Il nome è inserito non rispetta i criteri minimi.{FFFFFF}\nInserisci il nuovo nome per il personaggio.", "Cambia", "Annulla");
	
	inline OnLoad()
	{
		
		if(cache_num_rows() > 0)
		{
			return Dialog_Show(playerid, Dialog_ChangeName, DIALOG_STYLE_INPUT, "Cambio Nome", "{FF0000}Esiste già un personaggio con questo nome.{FFFFFF}\nInserisci il nuovo nome per il personaggio.", "Cambia", "Annulla");
		}
		else
		{
			
			printf("Name: %s", characterName);

			Account_AddChangeName(playerid, -1);

			mysql_tquery_f(gMySQL, "UPDATE `characters` SET name = '%e' WHERE id = '%d'", characterName, Character_GetID(playerid));
			SendMessageToAdmins(0, COLOR_ORANGE, "%s (%d) ha cambiato il proprio nome. Nuovo nome: %s", Character_GetOOCName(playerid), playerid, characterName);
			
			Log(Character_GetOOCName(playerid), characterName, "change_name_zpoints");

			SendFormattedMessage(playerid, COLOR_ORANGE, "Hai cambiato il nome del tuo personaggio. Nuovo nome: %s", characterName);
			SendClientMessage(playerid, COLOR_ORANGE, "Rilogga per apportare le modifiche.");

			CallLocalFunction(#OnCharacterChangeName, "ds", playerid, characterName);

			KickEx(playerid);
		}
	}
	// here '%e' AS new_name is used to pass newCharacterName to Inline function
	
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT id FROM `characters` WHERE name = '%e'", characterName);
	return 1;
}


Dialog:Dialog_ZPointsBuyRewards(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;
	new price = RewardInfo_GetPoints(listitem);
	if(Account_GetZPoints(playerid) < price)
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza Z-Points.");
	switch(listitem)
	{
		case 0: // Change name
		{
			Account_AddChangeName(playerid, 1);
		}
		case 1: // Block PM
		{
			if(Account_HasBlockPM(playerid))
				return SendClientMessage(playerid, COLOR_ERROR, "Hai già acquistato il blocco PM.");
			Account_SetBlockPM(playerid, 1);
		}
		case 2: // Block OOC
		{
			if(Account_HasBlockOOC(playerid))
				return SendClientMessage(playerid, COLOR_ERROR, "Hai già acquistato il blocco OOC.");
			Account_SetBlockOOC(playerid, 1);
		}
		case 3: // Premium Bronze
		{
			if(Account_GetPremiumLevel(playerid) > 1)
				return SendClientMessage(playerid, COLOR_ERROR, "Possiedi già un livello di premium superiore.");
			//3600 seconds * 24 (hours) * 30 (days)
			if(Account_GetPremiumLevel(playerid) == 1 && Account_GetPremiumExpiry(playerid) > gettime())
			{
                Account_SetPremiumExpiry(playerid, Account_GetPremiumExpiry(playerid) + 3600 * 24 * 30);
			}
			else
			{
                Account_SetPremiumExpiry(playerid, 3600 * 24 * 30);
				Account_SetPremiumLevel(playerid, 1);
			}
		}
		case 4: // Premium Silver
		{
			if(Account_GetPremiumLevel(playerid) > 2)
				return SendClientMessage(playerid, COLOR_ERROR, "Possiedi già un livello di premium superiore.");

			if(Account_GetPremiumLevel(playerid) == 2 && Account_GetPremiumExpiry(playerid) > gettime())
			{
				Account_SetPremiumExpiry(playerid, Account_GetPremiumExpiry(playerid) + 3600 * 24 * 30);
			}
			else if(Account_GetPremiumLevel(playerid) == 1 && Account_GetPremiumExpiry(playerid) > gettime())
			{
				new bonus = (Account_GetPremiumExpiry(playerid) - gettime())/2;
                Account_SetPremiumExpiry(playerid, gettime() + bonus + 3600 * 24 * 30);
				Account_SetPremiumLevel(playerid, 2);
			}
			else
			{
                Account_SetPremiumExpiry(playerid, gettime() + 3600 * 24 * 30);
				Account_SetPremiumLevel(playerid, 2);
			}
		}
		case 5: // Premium Gold
		{
			if(Account_GetPremiumLevel(playerid) == 3 && Account_GetPremiumExpiry(playerid) > gettime())
			{
				Account_SetPremiumExpiry(playerid, Account_GetPremiumExpiry(playerid) + 3600 * 24 * 30);
			}
			else if(Account_GetPremiumLevel(playerid) == 2 && Account_GetPremiumExpiry(playerid) > gettime())
			{
				new bonus = ((Account_GetPremiumExpiry(playerid) - gettime())/3)*2;
                Account_SetPremiumExpiry(playerid, gettime() + bonus + 3600 * 24 * 30);
                Account_SetPremiumLevel(playerid, 3);
			}
			else if(Account_GetPremiumLevel(playerid) == 1 && Account_GetPremiumExpiry(playerid) > gettime())
			{
				new bonus = (Account_GetPremiumExpiry(playerid) - gettime())/3;
                Account_SetPremiumExpiry(playerid, gettime() + bonus + 3600 * 24 * 30);
				Account_SetPremiumLevel(playerid, 3);
			}
			else
			{
                Account_SetPremiumExpiry(playerid, gettime() + 3600 * 24 * 30);
				Account_SetPremiumLevel(playerid, 3);
			}
		}
		case 6: // +1 Slot Pg
		{
			Account_IncreaseCharactersSlot(playerid, 1);
		}
		case 7: // +1 Level
		{
			Character_IncreaseLevel(playerid, 1);
			SetPlayerScore(playerid, Character_GetLevel(playerid));
		}
	}
	SendFormattedMessage(playerid, -1, "Hai acquistato \"{00AA00}%s{FFFFFF}\" per {AA0000}%d{FFFFFF} Z-Points.", RewardInfo[listitem][rdName], price);
	Account_AddPoints(playerid, -price);
	return 1;
}
