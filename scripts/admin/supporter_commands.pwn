
flags:sduty(CMD_SUPPORTER);
CMD:sduty(playerid, params[])
{
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