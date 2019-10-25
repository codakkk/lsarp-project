flags:acmds(CMD_JR_MODERATOR);
CMD:acmds(playerid, params[])
{
	
	/*if(Account_GetAdminLevel(playerid) >= 2)
		PrintPlayerCommandsByFlags(playerid, CMD_JR_MODERATOR, "MODERATORE JR");
	
	if(Account_GetAdminLevel(playerid) >= 3)
		PrintPlayerCommandsByFlags(playerid, CMD_MODERATOR, "MODERATORE");

	if(Account_GetAdminLevel(playerid) >= 4)
		PrintPlayerCommandsByFlags(playerid, CMD_ADMIN, "ADMIN");

	if(Account_GetAdminLevel(playerid) >= 5)
		PrintPlayerCommandsByFlags(playerid, CMD_DEVELOPER, "DEVELOPER");

	if(IsPlayerAdmin(playerid) && Account_GetAdminLevel(playerid) >= 5)
		PrintPlayerCommandsByFlags(playerid, CMD_RCON, "RCON");*/
	if(Account_GetAdminLevel(playerid) >= 2)
	{
		SendClientMessage(playerid, -1, "[MODERATORE JR]: /a - /aduty - /goto - /gethere - /jetpack - /staff");
		SendClientMessage(playerid, -1, "[MODERATORE JR]: /gotopos - /check - /invcheck - /freeze - /unfreeze");
		SendClientMessage(playerid, -1, "[MODERATORE JR]: /masks - /n - /setweather - /settimeadd");
		SendClientMessage(playerid, -1, "[MODERATORE JR]: /resetdrugstats - /drugstats - /setvw - /kick - /aslap");
		SendClientMessage(playerid, -1, "[MODERATORE JR]: /gotobuilding - /recon - /setinterior - /disablealerts");
		
	}
	if(Account_GetAdminLevel(playerid) >= 3)
	{
		SendClientMessage(playerid, -1, "[MODERATORE]: /setskin - /sethp - /setvhp - /setarmour - /ajail - /unjail - /setfaction");
		SendClientMessage(playerid, -1, "[MODERATORE]: /vcreate - /vdelete - (/fix)veh - /gotov - /getvhere - /avcolor - /vfuel");
		SendClientMessage(playerid, -1, "[MODERATORE]: /bancharacter - /unbancharacter - /apark - /avplate - /aafactionkick");
		SendClientMessage(playerid, -1, "[MODERATORE]: /aplacefurniture - /adeletefurniture - /atextureobject");
		SendClientMessage(playerid, -1, "[MODERATORE]: /teleportcreate - /teleportedit - /gototeleport");
		SendClientMessage(playerid, -1, "[MODERATORE]: /lzcreate - /lzedit - /alzrefreshall - /respawnfactionvehicles");
		SendClientMessage(playerid, -1, "[MODERATORE]: /sethunger - /afactioncmds - /abuildingcmds - /adealershipcmds");
		SendClientMessage(playerid, -1, "[MODERATORE]: /banaccount - /unbanaccount - /banofflineaccount - /resetinv");
		
	}
	if(Account_GetAdminLevel(playerid) >= 4)
	{
		SendClientMessage(playerid, -1, "[ADMIN]: /amm - /giveitem - /givevehicleitem  - /vowner - /vsave - /vdespawn");
		SendClientMessage(playerid, -1, "[ADMIN]: /vfaction - /avmaxhp - /atmcmds - /agpscmds - /settester");
		SendClientMessage(playerid, -1, "[ADMIN]: /banip - /unbanip - /gcreate - /gedit - /setchopshop - /ahousecmds");
	}
	if(Account_GetAdminLevel(playerid) >= 5)
	{
		SendClientMessage(playerid, -1, "[DEVELOPER]: /update - /kickall");
	}
	if(IsPlayerAdmin(playerid) && Account_GetAdminLevel(playerid) >= 5)
	{
		SendClientMessage(playerid, -1, "[RCON]: /setadmin - /givemoney - /payday");
	}
    return 1;
}

stock static PrintPlayerCommandsByFlags(playerid, flags = 0, const levelName[] = "")
{
	new
		String:temp = str_format("[%s]:", levelName),
		index = 0,
		dest[32],
		CmdArray: PC_array = PC_GetCommandArray();

	for (new i, j = PC_GetArraySize(PC_array); i != j; i++)
	{
		if (PC_GetFlags(dest) & flags)
		{
			PC_GetCommandName(PC_array, i, dest);
			str_append_format(temp, " %s", dest);
			index++;

			if(index == 10)
			{
				SendClientMessageStr(playerid, -1, temp);
				str_clear(temp);
				str_append_format(temp, "[%s]:", levelName);
				index = 0;
			}
		}
	}
	PC_FreeArray(PC_array);
}  