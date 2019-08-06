
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