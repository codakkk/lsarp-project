
flags:gov(CMD_GOVERNMENT);
CMD:gov(playerid, params[])
{
	if(Character_GetRank(playerid) > 3) return
		SendClientMessage(playerid, COLOR_ERROR, "Non hai il grado adatto per utilizzare questo comando.");

	if(isnull(params) || strlen(params) > 256)return
		SendClientMessage(playerid, COLOR_GREY, "/gov <testo>");

	foreach(new j : Player)
	{
		if(!Character_IsLogged(j))continue;
		SendFormattedMessage(j, COLOR_BLUE, "[Annuncio dal Governo] %s dice: %s", Character_GetOOCName(playerid), params);
	}

	return 1;
}