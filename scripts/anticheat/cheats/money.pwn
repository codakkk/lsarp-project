#include <YSI_Coding\y_hooks>

stock Character_GetMoney(playerid)
{
	return PlayerInfo[playerid][pMoney];
}

stock Character_GiveMoney(playerid, money, reason[] = "")
{
	#pragma unused reason
	if(!Character_IsLogged(playerid))
		return 0;
	PlayerInfo[playerid][pMoney] += money;
	ResetPlayerMoney(playerid);
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
	return GivePlayerMoney(playerid, PlayerInfo[playerid][pMoney]);
}

forward HidepMoneyTD(playerid);
public HidepMoneyTD(playerid)
{
	PlayerTextDrawHide(playerid, pMoneyTD[playerid]);
	PlayerTextDrawColor(playerid, pMoneyTD[playerid], 0xFFFFFFFF);
	KillTimer(MoneyTimer[playerid]);
	return 1;
}

stock AC_ResetPlayerMoney(playerid, reason[]="")
{
	if(!Character_IsLogged(playerid))
		return 0;
	PlayerInfo[playerid][pMoney] = 0;
	ResetPlayerMoney(playerid);
	return 1;
}