
#include <YSI_Coding\y_hooks>

hook OnCharacterLoaded(playerid)
{
	if(Account_GetPremiumLevel(playerid) > 0)
	{
		if(gettime() > Account_GetPremiumExpiry(playerid))
		{
			Account_SetPremiumLevel(playerid, 0);
			Account_SetPremiumExpiry(playerid, 0);
			SendClientMessage(playerid, COLOR_ERROR, "Il premium ti � scaduto. Visita il forum per informazioni su come rinnovarlo.");
		}
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

flags:blockpm(CMD_USER);
CMD:blockpm(playerid, params[])
{
	if(Account_GetPremiumLevel(playerid) < 1 && !Account_HasBlockPM(playerid) && Account_GetAdminLevel(playerid) < 2)
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei un premium di almeno livello Bronzo o non hai acquistato il blocco PM.");
	
    if(isnull(params) || strlen(params) <= 0)
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /blockpm <all o playerid/partofname>");

    if(!strcmp(params, "all", true))
    {
        Account_SetPMAllEnabled(playerid, !Account_HasPMAllEnabled(playerid));
        SendClientMessage(playerid, COLOR_GREEN, Account_HasPMAllEnabled(playerid) ? ("Hai riabilitato i PM da e verso tutti.") : ("Hai disabilitato i PM da e verso tutti."));
        if(!Account_HasPMAllEnabled(playerid))
            SendClientMessage(playerid, COLOR_GREEN, "Riutilizza '/blockpm all' o \"/tog pm\" per riattivarli.");
    }   
    else
    {
        new id;
        if(sscanf(params, "k<u>", id))
            return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /blockpm <all o playerid/partofname>");
		if(!Account_HasPMAllEnabled(playerid))
			return SendClientMessage(playerid, COLOR_ERROR, "Hai gi� disabilitato i PM da e verso tutti.");
        if(id == playerid || !Character_IsLogged(id))
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

flags:blockb(CMD_USER);
CMD:blockb(playerid, params[])
{
	if(Account_GetPremiumLevel(playerid) < 1 && !Account_HasBlockOOC(playerid) && Account_GetAdminLevel(playerid) < 3)
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei un premium di almeno livello Bronzo o non hai acquistato il blocco PM.");
    if(!strcmp(params, "all", true))
    {
		Account_SetOOCEnabled(playerid, !Account_HasOOCEnabled(playerid));
        SendClientMessage(playerid, COLOR_GREEN, Account_HasOOCEnabled(playerid) ? "Hai riabilitato la chat OOC da e verso tutti." : "Hai disabilitato la chat OOC da e verso tutti.");
        if(!Account_HasOOCEnabled(playerid))
            SendClientMessage(playerid, COLOR_GREEN, "Riutilizza '/blockb all' o '/tog ooc' per riattivarli.");
    }
    else
    {
        new id;
        if(sscanf(params, "k<u>", id))
            return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /blockb <all o playerid/partofname>");
		if(!Account_HasOOCEnabled(playerid))
			return SendClientMessage(playerid, COLOR_ERROR, "Hai gi� disabilitato la chat OOC da e verso tutti.");
        if(id == playerid || !Character_IsLogged(id))
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

CMD:premiumchat(playerid, params[])
{
    if(!Player_HasPremiumAlertsEnabled(playerid))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "Non puoi utilizzare questo comando con gli alerts premium disattivati.");
    if(isnull(params))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: (/pc) /premiumchat [testo]");

    new String:string = str_new("PremiumChat: ");
    if(Account_GetAdminLevel(playerid) > 1)
        str_append_format(string, "%s (%s): %s", Account_GetName(playerid), Character_GetOOCName(playerid), params);
    else
        str_append_format(string, "%s: %s", Character_GetOOCName(playerid), params);
    foreach(new i : Player)
    {
        if(!Character_IsLogged(i) || Account_GetPremiumLevel(i) <= 0 || !Player_HasPremiumAlertsEnabled(i))
            continue;
        SendClientMessageStr(i, COLOR_ORANGE, string);
    }
    return 1;
}
alias:premiumchat("pc");