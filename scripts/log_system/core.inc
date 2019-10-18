
#if defined _included_LOG_MANAGER
    #endinput
#endif 

#define _included_LOG_MANAGER

enum e_LOG_TYPE
{
	LOG_TYPE_BAN = 0,
	LOG_TYPE_KICK = 1,
	LOG_TYPE_JAIL = 2,
	LOG_TYPE_UNJAIL = 3,
	LOG_TYPE_GIVE_MONEY = 4,
	LOG_TYPE_RECEIVE_MONEY = 5,
	LOG_TYPE_BUY_HOUSE = 6,
	LOG_TYPE_SELL_HOUSE = 7,
	LOG_TYPE_BUY_BUILDING = 8,
	LOG_TYPE_SELL_BUILDING = 9,
	LOG_TYPE_PRIVILEGE = 10
};

enum e_LOG_ENTITY_TYPE
{
	LOG_ENTITY_TYPE_HOUSE = 0,
	LOG_ENTITY_TYPE_BUSINESS = 1,
	LOG_ENTITY_TYPE_VEHICLE = 2,
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit()
{
	mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS logs \
						(`ID` INT(11) NOT NULL AUTO_INCREMENT, \
						`PlayerID` VARCHAR(255) NOT NULL, \
						`GivePlayerID` VARCHAR(255) NOT NULL, \
						`Text` VARCHAR(255) NOT NULL, \
						`ExtraVar` INT(11) NOT NULL, \
						`Time` INT(11) NOT NULL, \
						PRIMARY KEY (`ID`))", false);
	mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS character_logs \
						(`id` INT(11) NOT NULL AUTO_INCREMENT, \
						`account_id` INT(11) NOT NULL, \
						`character_id` INT(11) NOT NULL, \
						`issuer_character_id` INT(11) NOT NULL, \
						`issuer_account_id` INT(11) NOT NULL, \
						`type` INT(11) NOT NULL, \
						`reason` VARCHAR(255) NOT NULL, \
						`extra` INT(11) NOT NULL, \
						`created_at` INT(11) NOT NULL, \
						PRIMARY KEY (`ID`))", false);
	return 1;
}

stock Log(const playerName[], const giveplayerName[], const text[], extravar = 0)
{
	mysql_tquery_f(gMySQL, "INSERT INTO `logs` \
						  (PlayerID, GivePlayerID, Text, ExtraVar, Time) \
						  VALUES('%s', '%s', '%e', '%d', '%d')", playerName, 
										giveplayerName, 
										text,
										extravar,
										gettime());
}

stock LogCharacter(playerid, issuerid, e_LOG_TYPE:type, const reason[] = "", extra = 0)
{
	mysql_tquery_f(gMySQL, "INSERT INTO `character_logs` \
						  (account_id, character_id, issuer_account_id, issuer_character_id, type, reason, extra, created_at) \
						  VALUES('%d', '%d', '%d', '%d', '%d', '%e', '%d', '%d')", 
						  Account_GetID(playerid), Character_GetID(playerid), Account_GetID(issuerid), Character_GetID(issuerid), type, reason, extra, gettime());
	return 1;
}

