#include <server\components\clock>
#include <server\components\weather>

#include <YSI_Coding\y_hooks>

static enum E_SERVER_INFO
{
	TimeAdd
}

static ServerInfo[E_SERVER_INFO];

stock Server_SetTimeAdd(timeToAdd)
{
	ServerInfo[TimeAdd] = timeToAdd;
}

stock Server_GetTimeAdd()
{
	return ServerInfo[TimeAdd];
}