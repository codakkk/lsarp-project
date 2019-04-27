flags:blockpm(CMD_PREMIUM_SILVER);
CMD:blockpm(playerid, params[])
{
    if(!strcmp(params, "all", true))
    {
        Bit_Set(gPlayerBitArray[e_pTogglePMAll], playerid, !Bit_Get(gPlayerBitArray[e_pTogglePMAll], playerid));
        SendClientMessage(playerid, COLOR_GREEN, Bit_Get(gPlayerBitArray[e_pTogglePMAll], playerid) ? "Hai disabilitato i PM da e verso tutti." : "Hai riabilitato i PM da e verso tutti.");
        if(Bit_Get(gPlayerBitArray[e_pTogglePMAll], playerid))
            SendClientMessage(playerid, COLOR_GREEN, "Riutilizza '/blockpm all' per riattivarli.");
    }   
    else
    {
        new id;
        if(sscanf(params, "u", id))
            return SendClientMessage(playerid, COLOR_ERROR, "/blockpm <all o playerid/partofname>");
        if(id == playerid || !IsPlayerConnected(id) || !Character_IsLogged(id))
            return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");
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

flags:blockb(CMD_PREMIUM_BRONZE);
CMD:blockb(playerid, params[])
{
    if(!strcmp(params, "all", true))
    {
		Player_SetOOCEnabled(playerid, !Player_HasOOCEnabled(playerid));
        SendClientMessage(playerid, COLOR_GREEN, Player_HasOOCEnabled(playerid) ? "Hai riabilitato la chat OOC da e verso tutti." : "Hai disabilitato la chat OOC da e verso tutti.");
        if(!Player_HasOOCEnabled(playerid))
            SendClientMessage(playerid, COLOR_GREEN, "Riutilizza '/blockb all' per riattivarli.");
    }
    else
    {
        new id;
        if(sscanf(params, "u", id))
            return SendClientMessage(playerid, COLOR_ERROR, "/blockb <all o playerid/partofname>");
        if(id == playerid || !IsPlayerConnected(id) || !Character_IsLogged(id))
            return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido.");
        if(Iter_Contains(pToggleOOC[playerid], id))
        {
            SendFormattedMessage(playerid, COLOR_YELLOW, "Hai riabilitato la chat OOC da e verso %s.", Character_GetOOCName(id));
            Iter_Remove(pToggleOOC[playerid], id);
        }
        else
        {
            SendFormattedMessage(playerid, COLOR_YELLOW, "Hai disabilitato la chat OOC da e verso %s.", Character_GetOOCName(id));
            Iter_Add(pToggleOOC[playerid], id);
        }
    }
    return 1;
}