stock Log(playerName[], giveplayerName[], text[], extravar = 0)
{
	mysql_tquery_f(gMySQL, "INSERT INTO `logs` \
						  (PlayerID, GivePlayerID, Text, ExtraVar, Time) \
						  VALUES('%s', '%s', '%e', '%d', '%d')", playerName, 
										giveplayerName, 
										text,
										extravar,
										gettime());
}