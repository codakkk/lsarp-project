
#include <YSI_Coding\y_hooks>

static PlayerMoney[MAX_PLAYERS];

hook OnCharacterLoad(playerid)
{
    inline OnLoad()
    {
        cache_get_value_index_int(0, 0, PlayerMoney[playerid]);
        ResetPlayerMoney(playerid);
        GivePlayerMoney(playerid, PlayerMoney[playerid]);
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT money FROM `characters` WHERE id = '%d'", Character_GetID(playerid));
    return 1;
}

stock Character_GetMoney(playerid)
{
	return PlayerMoney[playerid];
}

stock Character_SetMoney(playerid, money)
{
    PlayerMoney[playerid] = money;
    Internal_Save(playerid);

    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, PlayerMoney[playerid]);
    return 1;
}

stock Character_GiveMoney(playerid, money, reason[] = "")
{
	#pragma unused reason
	if(!Character_IsLogged(playerid))
		return 0;
	PlayerMoney[playerid] += money;
    Internal_Save(playerid);

	ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, PlayerMoney[playerid]);

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
	MoneyTimer[playerid] = SetTimerEx(#HidepMoneyTD, 2000, false, "d", playerid);
	return 1;
}

forward HidepMoneyTD(playerid);
public HidepMoneyTD(playerid)
{
	PlayerTextDrawHide(playerid, pMoneyTD[playerid]);
	PlayerTextDrawColor(playerid, pMoneyTD[playerid], 0xFFFFFFFF);
	KillTimer(MoneyTimer[playerid]);
	return 1;
}

stock Character_ResetMoney(playerid, reason[]="")
{
	if(!Character_IsLogged(playerid))
		return 0;
	PlayerMoney[playerid] = 0;
    Internal_Save(playerid);
	ResetPlayerMoney(playerid);
	return 1;
}

static Internal_Save(playerid)
{
    mysql_tquery_f(gMySQL, "UPDATE `characters` SET money = '%d' WHERE id = '%d'", Character_GetMoney(playerid), Character_GetID(playerid));
    return 1;
}