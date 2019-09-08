
flags:gov(CMD_GOVERNMENT);
CMD:gov(playerid, params[])
{
	if(Character_GetRank(playerid) > 3) 
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai il grado adatto per utilizzare questo comando.");

	if(isnull(params) || strlen(params) > 256)return
		SendClientMessage(playerid, COLOR_GREY, "/gov <testo>");

	foreach(new j : Player)
	{
		if(!Character_IsLogged(j))continue;
		SendFormattedMessage(j, COLOR_BLUE, "[Annuncio dal Governo] %s dice: %s", Character_GetOOCName(playerid), params);
	}

	return 1;
}

flags:tasse(CMD_GOVERNMENT);
CMD:tasse(playerid, params[])
{
	if(Character_GetRank(playerid) != 1) 
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai il grado adatto per utilizzare questo comando.");
	new 
		String:title = @("Tasse - Las Venturas"),
		String:info = @("Nome\tValore Attuale(%%)\n");
	info += str_format("Accise Benzina\t%d\n", Server_GetExcisesOnFuel());
	info += str_format("Tasse sulle Pubblicità\t%d\n", Server_GetTaxOnAds());
	info += str_format("Tasse sui veicoli\t%d\n", Server_GetTaxOnVehicles());

	Dialog_Show_s(playerid, Dialog_GovernmentTaxes, DIALOG_STYLE_TABLIST_HEADERS, title, info, "Modifica", "Chiudi");
	return 1;
}