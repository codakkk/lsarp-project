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
	Discord_SendKickMessage("L'admin **%s** ha _kickato_ tutti. Motivo: __**%s**.", Account_GetName(playerid), params);
	return 1;
}

flags:update(CMD_DEVELOPER);
CMD:update(playerid, params[])
{
	SendRconCommand("password lsarp_updating_psw");
	callcmd::kickall(playerid, "Riavvio.");
	return 1;
}


flags:spawnbots(CMD_DEVELOPER);
CMD:spawnbots(playerid, params[])
{
	static bool:AreBotSpawned = false;
	static bool:IsCreatedBot[MAX_PLAYERS char] = {false, ...};
	if(AreBotSpawned)
	{
		for(new i = 0; i < MAX_PLAYERS; ++i)
		{
			if(IsPlayerNPC(playerid) && IsCreatedBot{i})
			{
				Kick(i);
				IsCreatedBot{i} = false;
			}
		}
	}
	else
	{

		for(new i = 0; i < 2; ++i)
		{
			ConnectNPC("test", "idle");
			IsCreatedBot{i} = true;
		}
	}
	return 1;
}