
#include <YSI_Coding\y_hooks>

static 
	PlayerMoney[MAX_PLAYERS],
	PlayerBank[MAX_PLAYERS], 
	MoneyTimer[MAX_PLAYERS];

hook OnGameModeInit()
{
	mysql_tquery(gMySQL, "CREATE TABLE `bank_movement_records` (`id` int(11) AUTO_INCREMENT, \
    `character_id` int(11) NOT NULL, \
    `beneficiary_id` int(11) NOT NULL DEFAULT '0', \
    `money` int(11) NOT NULL, \
    `created_at` int(11) NOT NULL, \
    PRIMARY KEY (`id`)) ENGINE=MyISAM DEFAULT CHARSET=latin1");
	return 1;
}

hook OnCharacterLoad(playerid)
{
    await mysql_aquery_s(gMySQL, str_format("SELECT money, bank FROM `characters` WHERE id = '%d'", Character_GetID(playerid)));
    
    cache_get_value_name_int(0, "money", PlayerMoney[playerid]);
	cache_get_value_name_int(0, "bank", PlayerBank[playerid]);
	ResetPlayerMoney(playerid);
	GivePlayerMoney(playerid, PlayerMoney[playerid]);
    return 1;
}

stock Character_GetMoney(playerid)
{
	return PlayerMoney[playerid];
}

stock Character_SetMoney(playerid, money)
{
    PlayerMoney[playerid] = money;
    
	mysql_pquery_f(gMySQL, "UPDATE `characters` SET money = '%d' WHERE id = '%d'", Character_GetMoney(playerid), Character_GetID(playerid));

	ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, PlayerMoney[playerid]);
    return 1;
}

stock bool:Character_GiveMoney(playerid, money, reason[] = "")
{
	#pragma unused reason
	if(!Character_IsLogged(playerid))
		return false;

	new newMoney = Character_GetMoney(playerid) + money;

	if(newMoney < 0)
	{
		return false;
	}		
		
	Character_SetMoney(playerid, newMoney);

	new string[16];
	if(money < 0)
	{
		format(string, sizeof(string), "%d$", money);
		PlayerTextDrawHide(playerid, pMoneyTD[playerid]);
		PlayerTextDrawColor(playerid, pMoneyTD[playerid], 0xFF0000FF);
		PlayerTextDrawSetString(playerid, pMoneyTD[playerid], string);
		PlayerTextDrawShow(playerid, pMoneyTD[playerid]);
	}
	else if(money > 0)
	{
		format(string, sizeof(string), "+%d$", money);
		PlayerTextDrawHide(playerid, pMoneyTD[playerid]);
		PlayerTextDrawColor(playerid, pMoneyTD[playerid], 0x00FF00FF);
		PlayerTextDrawSetString(playerid, pMoneyTD[playerid], string);
		PlayerTextDrawShow(playerid, pMoneyTD[playerid]);
	}
	MoneyTimer[playerid] = SetTimerEx(#HideMoneyTD, 2000, false, "d", playerid);
	return true;
}

stock Character_GetBank(playerid)
{
	return PlayerBank[playerid];
}

stock Character_SetBank(playerid, money)
{
    PlayerBank[playerid] = money;
    mysql_pquery_f(gMySQL, "UPDATE `characters` SET bank = '%d' WHERE id = '%d'", Character_GetBank(playerid), Character_GetID(playerid));
    return 1;
}

stock Character_GiveBank(playerid, money, reason[] = "")
{
	#pragma unused reason
	if(!Character_IsLogged(playerid))
		return 0;
	
	Character_SetBank(playerid, Character_GetBank(playerid) + money);
	return 1;
}

forward HideMoneyTD(playerid);
public HideMoneyTD(playerid)
{
	PlayerTextDrawHide(playerid, pMoneyTD[playerid]);
	PlayerTextDrawColor(playerid, pMoneyTD[playerid], 0xFFFFFFFF);
	KillTimer(MoneyTimer[playerid]);
	return 1;
}

stock Character_ResetBank(playerid, reason[]="")
{
	if(!Character_IsLogged(playerid))
		return 0;
	PlayerMoney[playerid] = 0;

	mysql_pquery_f(gMySQL, "UPDATE `characters` SET money = '0' WHERE id = '%d'", Character_GetID(playerid));

	ResetPlayerMoney(playerid);
	return 1;
}

stock Character_AddBankRecord(playerid, amount)
{
	mysql_pquery_f(gMySQL, "INSERT INTO `bank_movement_records` (character_id, beneficiary_id, money, created_at) VALUES('%d', '0', '%d', '%d')", Character_GetID(playerid), amount, gettime());
}

stock Character_AddBankTransferRecord(characterid, beneficiaryid, money)
{
	mysql_pquery_f(gMySQL, "INSERT INTO `bank_movement_records` (character_id, beneficiary_id, money, created_at) \
	VALUES('%d', '%d', '%d', '%d')", characterid, beneficiaryid, money, gettime());
}