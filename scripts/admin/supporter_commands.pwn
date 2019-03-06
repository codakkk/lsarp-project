
flags:sduty(CMD_SUPPORTER);
CMD:sduty(playerid, params[])
{
    pSupporterDuty[playerid] = !pSupporterDuty[playerid];
    
    return 1;
}