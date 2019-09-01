#include <building\core>
#include <YSI_Coding\y_hooks>

static PlayerPayCheck[MAX_PLAYERS];

hook OnPlayerClearData(playerid)
{
    PlayerPayCheck[playerid] = 0;
    return 1;
}

hook OnCharacterLoad(playerid)
{
    inline OnLoad()
    {
        cache_get_value_index_int(0, 0, PlayerPayCheck[playerid]);
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT paycheck FROM `characters` WHERE id = '%d'", Character_GetID(playerid));
    return 1;
}

flags:ritirastipendio(CMD_ALIVE_USER);
CMD:ritirastipendio(playerid, params[])
{
	new buildingid = Character_GetNearBuildingID(playerid);
	if(!Building_IsValid(buildingid) || Building_GetType(buildingid) != BUILDING_TYPE_PAYCHECK)
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei nell'edificio esatto per ritirare lo stipendio.");
	new paycheck = Character_GetPayCheck(playerid);
	if(paycheck <= 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai uno stipendio da ritirare.");
	
	Character_GiveMoney(playerid, paycheck, "paycheck");
	Character_SetPayCheck(playerid, 0);
	SendFormattedMessage(playerid, -1, "Hai ritirato il tuo stipendio. Ammontare: {85bb65}$%d{FFFFFF}", paycheck);
	return 1;
}

stock Character_AddPayCheck(playerid, amount)
{
	PlayerPayCheck[playerid] += amount;
    Internal_Save(playerid);
}

stock Character_SetPayCheck(playerid, set)
{
	PlayerPayCheck[playerid] = set;
    Internal_Save(playerid);
}

stock Character_GetPayCheck(playerid)
{
	return PlayerPayCheck[playerid];
}

static Internal_Save(playerid)
{
    mysql_tquery_f(gMySQL, "UPDATE `characters` SET paycheck = '%d' WHERE id = '%d'", PlayerPayCheck[playerid], Character_GetID(playerid));
}