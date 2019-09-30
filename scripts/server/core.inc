#include <server\components\clock>
#include <server\components\weather>
#include <server\maps\maps>
#include <YSI_Coding\y_hooks>

static enum E_SERVER_INFO
{
	TimeAdd,
	GovernamentBank,
	ExcisesOnFuel,
	TaxOnAds,
	TaxOnVehicles
}

static ServerInfo[E_SERVER_INFO];

hook OnGameModeInit()
{
	Server_SetExcisesOnFuel(5);
	Server_SetTaxOnAds(1);
	Server_SetTaxOnVehicles(1);
	Server_SetGovernamentBank(0);
	return 1;
}

stock Server_SetTimeAdd(timeToAdd)
{
	ServerInfo[TimeAdd] = timeToAdd;
}

stock Server_GetTimeAdd()
{
	return ServerInfo[TimeAdd];
}

stock Server_GiveGovernamentBank(money)
{
	ServerInfo[GovernamentBank] += money;
}

stock Server_SetGovernamentBank(money)
{
	ServerInfo[GovernamentBank] = money;
}

stock Server_GetGovernamentBank(money)
{
	return ServerInfo[GovernamentBank];
}

stock Server_GetExcisesOnFuel()
{
	return ServerInfo[ExcisesOnFuel];
}

stock Server_SetExcisesOnFuel(value)
{
	ServerInfo[ExcisesOnFuel] = value;
}

stock Server_GetTaxOnAds()
{
	return ServerInfo[TaxOnAds];
}

stock Server_SetTaxOnAds(value)
{
	ServerInfo[TaxOnAds] = value;
}

stock Server_GetTaxOnVehicles()
{
	return ServerInfo[TaxOnVehicles];
}

stock Server_SetTaxOnVehicles(value)
{
	ServerInfo[TaxOnVehicles] = value;
}