
flags:dutynick(CMD_POLICE);
CMD:dutynick(playerid, params[])
{
    if(Character_GetDivision(playerid) != POLICE_DIVISION_SI)
        return SendClientMessage(playerid, COLOR_ERROR, "Non fai parte della divisione giusta.");
    
    if(!Character_IsFactionDuty(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Devi essere in servizio per poter utilizzare questo comando.");
    SetPlayerColor(playerid, 0xFFFFFFFF);
    SendClientMessage(playerid, COLOR_GREEN, "Adesso il tuo colore è bianco.");
    return 1;
}