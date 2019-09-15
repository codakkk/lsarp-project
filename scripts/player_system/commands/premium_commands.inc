
#include <YSI_Coding\y_hooks>
hook OnCharacterLoad(playerid)
{
	if(Account_GetPremiumLevel(playerid) > 0)
	{
		if(gettime() > Account_GetPremiumExpiry(playerid))
		{
			Account_SetPremiumLevel(playerid, 0);
			Account_SetPremiumExpiry(playerid, 0);
			SendClientMessage(playerid, COLOR_ERROR, "Il premium ti è scaduto. Visita il forum per informazioni su come rinnovarlo.");
		}
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

flags:givezpoints(CMD_RCON);
CMD:givezpoints(playerid, params[])
{
	new id, points;
	if(sscanf(params, "k<u>d", id, points))	
		return SendClientMessage(playerid, COLOR_ERROR, "/givezpoints <playerid/partofname> <points>");
	
	Account_AddPoints(id, points);

	SendMessageToAdmins(true, COLOR_ADMINCMD, "[ADMIN-ALERT] %s (%d) ha dato %d Z-Points a %s (%d).", 
	Account_GetName(playerid), playerid, points, Account_GetName(id), id);

	return 1;
}

flags:blockpm(CMD_USER);
CMD:blockpm(playerid, params[])
{
	if(Account_GetPremiumLevel(playerid) < 1 && !Account_HasBlockPM(playerid) && Account_GetAdminLevel(playerid) < 2)
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei un premium di almeno livello Bronzo o non hai acquistato il blocco PM.");
	
    if(isnull(params) || strlen(params) <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "/blockpm <all o playerid/partofname>");

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
            return SendClientMessage(playerid, COLOR_ERROR, "/blockpm <all o playerid/partofname>");
		if(!Account_HasPMAllEnabled(playerid))
			return SendClientMessage(playerid, COLOR_ERROR, "Hai già disabilitato i PM da e verso tutti.");
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
            return SendClientMessage(playerid, COLOR_ERROR, "/blockb <all o playerid/partofname>");
		if(!Account_HasOOCEnabled(playerid))
			return SendClientMessage(playerid, COLOR_ERROR, "Hai già disabilitato la chat OOC da e verso tutti.");
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

flags:setpremium(CMD_RCON);
CMD:setpremium(playerid, params[])
{
	new id, premiumid, time;
	if(sscanf(params, "k<u>dd", id, premiumid, time))
		return SendClientMessage(playerid, COLOR_ERROR, "/setpremium <playerid/partofname> <premium (0 - 3)> <giorni>");
	if(!IsPlayerConnected(id) || IsPlayerNPC(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido");
	if(premiumid < 0 || premiumid > 3)
		return SendClientMessage(playerid, COLOR_ERROR, "Premium ID non valido");
	if(time < 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Giorni < 0");
	
	Account_SetPremiumLevel(id, premiumid);
	Account_SetPremiumExpiry(id, time ? (gettime() + 3600 * 24 * time) : 0);

	SendMessageToAdmins(true, COLOR_ADMINCMD, "[ADMIN-ALERT]: %s (%d) ha settato %s (%d) premium livello %d.", 
	Account_GetName(playerid), playerid, Account_GetName(id), id, premiumid);
	return 1;
}