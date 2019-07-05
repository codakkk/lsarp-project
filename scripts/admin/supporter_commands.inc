
flags:sduty(CMD_SUPPORTER);
CMD:sduty(playerid, params[])
{
	if(Account_IsAdminDuty(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se sei in servizio admin.");
	Account_SetSupporterDuty(playerid, !Account_IsSupporterDuty(playerid));
    if(Account_IsSupporterDuty(playerid))
	{
		SetPlayerColor(playerid, 0xa52a2aff);
	}
	else
	{
		SetPlayerColor(playerid, 0xFFFFFFFF);
	}
    return 1;
}