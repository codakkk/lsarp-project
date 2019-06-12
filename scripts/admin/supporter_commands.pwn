
flags:sduty(CMD_SUPPORTER);
CMD:sduty(playerid, params[])
{
	if(pAdminDuty[playerid])
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se sei in servizio admin.");
    pSupporterDuty[playerid] = !pSupporterDuty[playerid];
    if(pSupporterDuty[playerid])
	{
		SetPlayerColor(playerid, 0xa52a2aff);
	}
	else
	{
		SetPlayerColor(playerid, 0xFFFFFFFF);
	}
    return 1;
}