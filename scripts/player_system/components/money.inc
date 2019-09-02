
#include <YSI_Coding\y_hooks>

static 
	PlayerMoney[MAX_PLAYERS],
	PlayerBank[MAX_PLAYERS], 
	MoneyTimer[MAX_PLAYERS];

hook OnCharacterLoad(playerid)
{
    inline OnLoad()
    {
        cache_get_value_name_int(0, "money", PlayerMoney[playerid]);
		cache_get_value_name_int(0, "bank", PlayerMoney[playerid]);
        ResetPlayerMoney(playerid);
        GivePlayerMoney(playerid, PlayerMoney[playerid]);
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT money, bank FROM `characters` WHERE id = '%d'", Character_GetID(playerid));
    return 1;
}

stock Character_GetMoney(playerid)
{
	return PlayerMoney[playerid];
}

stock Character_SetMoney(playerid, money)
{
    PlayerMoney[playerid] = money;
    
	mysql_tquery_f(gMySQL, "UPDATE `characters` SET money = '%d' WHERE id = '%d'", Character_GetMoney(playerid), Character_GetID(playerid));

	ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, PlayerMoney[playerid]);
    return 1;
}

stock Character_GiveMoney(playerid, money, reason[] = "")
{
	#pragma unused reason
	if(!Character_IsLogged(playerid))
		return 0;
	Character_SetMoney(playerid, Character_GetMoney(playerid) + money);

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
	return 1;
}

stock Character_GetBank(playerid)
{
	return PlayerBank[playerid];
}

stock Character_SetBank(playerid, money)
{
    PlayerBank[playerid] = money;
    mysql_tquery_f(gMySQL, "UPDATE `characters` SET bank = '%d' WHERE id = '%d'", Character_GetBank(playerid), Character_GetID(playerid));
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

	mysql_tquery_f(gMySQL, "UPDATE `characters` SET money = '0' WHERE id = '%d'", Character_GetID(playerid));

	ResetPlayerMoney(playerid);
	return 1;
}