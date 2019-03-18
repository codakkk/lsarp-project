
CMD:blockpm(playerid, params[])
{
    if(!strcmp(params, "all", true))
    {
        pTogglePMAll[playerid] = !pTogglePMAll[playerid];
        SendClientMessage(playerid, COLOR_GREEN, pTogglePMAll[playerid] ? "Hai disabilitato i PM da e verso tutti." : "Hai riabilitato i PM da e verso tutti.");
        if(pTogglePMAll[playerid])
            SendClientMessage(playerid, COLOR_GREEN, "Riutilizza '/blockpm all' per riattivarli.");
    }   
    else
    {
        new id;
        if(sscanf(params, "u", id))
            return SendClientMessage(playerid, COLOR_ERROR, "/blockpm <all o playerid/partofname>");
        if(!IsPlayerConnected(id) || !gCharacterLogged[id])
            return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è connesso!");
        if(Iter_Contains(pTogglePM[playerid], id))
        {
            SendFormattedMessage(playerid, COLOR_YELLOW, "Hai riabilitato i PM da e verso %s.", Character_GetOOCName(id));
            Iter_Remove(pTogglePM[playerid], id);
        }
        else
        {
            SendFormattedMessage(playerid, COLOR_YELLOW, "Hai disabilitato i PM da e verso %s.", Character_GetOOCName(id));
            Iter_Add(pTogglePM[playerid], id);
        }
    } 
    return 1;
}

CMD:blockb(playerid, params[])
{
    if(!strcmp(params, "all", true))
    {
        pToggleOOCAll[playerid] = !pToggleOOCAll[playerid];
        SendClientMessage(playerid, COLOR_GREEN, pToggleOOCAll[playerid] ? "Hai disabilitato i PM da e verso tutti." : "Hai riabilitato i PM da e verso tutti.");
        if(pToggleOOCAll[playerid])
            SendClientMessage(playerid, COLOR_GREEN, "Riutilizza '/blockpm all' per riattivarli.");
    }
    else
    {
        new id;
        if(sscanf(params, "u", id))
            return SendClientMessage(playerid, COLOR_ERROR, "/blockb <all o playerid/partofname>");
        if(!IsPlayerConnected(id) || !gCharacterLogged[id])
            return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è connesso!");
        if(Iter_Contains(pToggleOOC[playerid], id))
        {
            SendFormattedMessage(playerid, COLOR_YELLOW, "Hai riabilitato i PM da e verso %s.", Character_GetOOCName(id));
            Iter_Remove(pToggleOOC[playerid], id);
        }
        else
        {
            SendFormattedMessage(playerid, COLOR_YELLOW, "Hai disabilitato i PM da e verso %s.", Character_GetOOCName(id));
            Iter_Add(pToggleOOC[playerid], id);
        }
    }
    return 1;
}