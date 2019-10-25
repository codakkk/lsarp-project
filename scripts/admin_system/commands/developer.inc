flags:kickall(CMD_DEVELOPER);
CMD:kickall(playerid, params[])
{
	if(strlen(params) <= 0 || strlen(params) > 90)
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /kickall <motivo>");
	
	SendMessageToAdmins(true, COLOR_ADMINCMD, "L'admin %s ha kickato tutti. Motivo: %s", Account_GetName(playerid), params);

	foreach(new i : Player)
	{
		if(Account_GetAdminLevel(i) >= 5)
			continue;
		wait_ticks(1);
		KickEx(i);
	}
	Discord_SendBanMessage("L'admin **%s** ha _kickato_ tutti. Motivo: __**%s**.", Account_GetName(playerid), params);
	return 1;
}

flags:update(CMD_DEVELOPER);
CMD:update(playerid, params[])
{
	SendRconCommand("password lsarp_updating_psw");
	callcmd::kickall(playerid, "Riavvio.");
	return 1;
}
