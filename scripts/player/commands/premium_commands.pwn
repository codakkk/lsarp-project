
#include <YSI_Coding\y_hooks>
hook OnPlayerCharacterLoad(playerid)
{
	if(Account_GetPremiumLevel(playerid) > 0)
	{
		if(gettime() > Account_GetPremiumExpiry(playerid))
		{
			Account_SetPremiumLevel(playerid, 0);
			Account_SetPremiumExpiry(playerid, 0);
			Account_Save(playerid);
			SendClientMessage(playerid, COLOR_ERROR, "Il premium ti è scaduto. Visita il forum per informazioni su come rinnovarlo.");
		}
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

flags:givezpoints(CMD_RCON);
CMD:givezpoints(playerid, params[])
{
	new id, points;
	if(sscanf(params, "ud", id, points))	
		return SendClientMessage(playerid, COLOR_ERROR, "/givezpoints <playerid/partofname> <points>");
	
	Account_AddPoints(id, points);
	Account_SaveZPoints(id);
	SendMessageToAdmins(true, COLOR_ADMIN, "[ADMIN-ALERT] %s (%d) ha dato %d Z-Points a %s (%d).", 
	AccountInfo[playerid][aName], playerid, points, AccountInfo[id][aName], id);

	return 1;
}

flags:zpoints(CMD_USER);
CMD:zpoints(playerid, params[])
{
	Dialog_Show_s(playerid, Dialog_ZPoints, DIALOG_STYLE_LIST, str_format("Hai %d ZPoints", Account_GetZPoints(playerid)), @("Acquista\nUtilizza"), "Avanti", "chiudi");
	return 1;
}

Dialog:Dialog_ZPoints(playerid, response, listitem, inputtext[])
{
	if(!response)	
		return 0;
	if(listitem == 0)
	{
		new String:str = @("Premio\tZ-Points\n");
		for(new i = 0, j = sizeof(RewardInfo); i < j; ++i)
		{
			str += str_format("%s\t{FFFFFF}%dz{FFFFFF}\n", RewardInfo[i][rdName], RewardInfo_GetPoints(i));
		}
		Dialog_Show_s(playerid, Dialog_ZPointsBuyRewards, DIALOG_STYLE_TABLIST_HEADERS, str_format("Hai %d ZPoints", Account_GetZPoints(playerid)), str, "Acquista", "Annulla");
	}
	else if(listitem == 1)
	{
		Dialog_Show(playerid, Dialog_ZPointsUseReward, DIALOG_STYLE_LIST, "Utilizza Premi", "Cambio Nome", "Acquista", "Annulla");
	}
	return 1;
}

Dialog:Dialog_ZPointsUseReward(playerid, response, listitem, inputtext[])
{
	if(!response)
		return pc_cmd_zpoints(playerid, "");
	switch(listitem)
	{
		case 0:
		{
			if(PlayerRewardData[playerid][rChangeName] <= 0)
				return SendClientMessage(playerid, COLOR_ERROR, "Non disponi di cambi nomi.");
			
			return 1;
		}
	}
	return 1;
}

Dialog:Dialog_ZPointsBuyRewards(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;
	new price = RewardInfo_GetPoints(listitem);
	if(Account_GetZPoints(playerid) < price)
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza Z-Points.");
	switch(listitem)
	{
		case 0: // Change name
		{
			PlayerRewardData[playerid][rChangeName]++;
		}
		case 1: // Block PM
		{
			if(PlayerRewardData[playerid][rBlockPM])
				return SendClientMessage(playerid, COLOR_ERROR, "Hai già acquistato il blocco PM.");
			PlayerRewardData[playerid][rBlockPM] = 1;
		}
		case 2: // Block OOC
		{
			if(PlayerRewardData[playerid][rBlockOOC])
				return SendClientMessage(playerid, COLOR_ERROR, "Hai già acquistato il blocco OOC.");
			PlayerRewardData[playerid][rBlockOOC] = 1;
		}
		case 3: // Premium Bronze
		{
			if(Account_GetPremiumLevel(playerid) > 1)
				return SendClientMessage(playerid, COLOR_ERROR, "Possiedi già un livello di premium superiore.");
			//3600 seconds * 24 (hours) * 30 (days)
			if(Account_GetPremiumLevel(playerid) == 1 && Account_GetPremiumExpiry(playerid) > gettime())
			{
				AccountInfo[playerid][aPremiumExpiry] += 3600 * 24 * 30;
			}
			else
			{
				AccountInfo[playerid][aPremiumExpiry] = gettime() + 3600 * 24 * 30;
				AccountInfo[playerid][aPremium] = 1;
			}
		}
		case 4: // Premium Silver
		{
			if(Account_GetPremiumLevel(playerid) > 2)
				return SendClientMessage(playerid, COLOR_ERROR, "Possiedi già un livello di premium superiore.");

			if(Account_GetPremiumLevel(playerid) == 2 && Account_GetPremiumExpiry(playerid) > gettime())
			{
				AccountInfo[playerid][aPremiumExpiry] += 3600 * 24 * 30;
			}
			else if(Account_GetPremiumLevel(playerid) == 1 && Account_GetPremiumExpiry(playerid) > gettime())
			{
				new bonus = (Account_GetPremiumExpiry(playerid) - gettime())/2;
				AccountInfo[playerid][aPremiumExpiry] = gettime() + bonus + 3600 * 24 * 30;
				AccountInfo[playerid][aPremium] = 2;
			}
			else
			{
				AccountInfo[playerid][aPremiumExpiry] = gettime() + 3600 * 24 * 30;
				AccountInfo[playerid][aPremium] = 2;
			}
		}
		case 5: // Premium Gold
		{
			if(Account_GetPremiumLevel(playerid) == 3 && Account_GetPremiumExpiry(playerid) > gettime())
			{
				AccountInfo[playerid][aPremiumExpiry] += 3600 * 24 * 30;
			}
			else if(Account_GetPremiumLevel(playerid) == 2 && Account_GetPremiumExpiry(playerid) > gettime())
			{
				new bonus = ((Account_GetPremiumExpiry(playerid) - gettime())/3)*2;
				AccountInfo[playerid][aPremiumExpiry] = gettime() + bonus + 3600 * 24 * 30;
				AccountInfo[playerid][aPremium] = 3;
			}
			else if(Account_GetPremiumLevel(playerid) == 1 && Account_GetPremiumExpiry(playerid) > gettime())
			{
				new bonus = (Account_GetPremiumExpiry(playerid) - gettime())/3;
				AccountInfo[playerid][aPremiumExpiry] = gettime() + bonus + 3600 * 24 * 30;
				AccountInfo[playerid][aPremium] = 3;
			}
			else
			{
				AccountInfo[playerid][aPremiumExpiry] = gettime() + 3600 * 24 * 30;
				AccountInfo[playerid][aPremium] = 3;
			}
		}
		case 6: // +1 Slot Pg
		{
			if(Account_GetCharactersSlot(playerid)+1 >= MAX_CHARACTERS)
				return SendClientMessage(playerid, COLOR_ERROR, "Hai già acquistato il massimo di slot possibili.");
			Account_IncreaseCharactersSlot(playerid, 1);
		}
		case 7: // +1 Level
		{
			Character_IncreaseLevel(playerid, 1);
			SetPlayerScore(playerid, Character_GetLevel(playerid));
		}
	}
	SendFormattedMessage(playerid, -1, "Hai acquistato \"{00AA00}%s{FFFFFF}\" per {AA0000}%d{FFFFFF} Z-Points.", RewardInfo[listitem][rdName], price);
	Account_AddPoints(playerid, -price);
	Account_SaveZPoints(playerid);
	Account_SaveRewards(playerid);
	Account_Save(playerid);
	Character_Save(playerid);
	return 1;
}


flags:blockpm(CMD_USER);
CMD:blockpm(playerid, params[])
{
	if(Account_GetPremiumLevel(playerid) < 1 && !PlayerRewardData[playerid][rBlockPM] && Account_GetAdminLevel(playerid) < 2)
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei un premium di almeno livello Bronzo o non hai acquistato il blocco PM.");
	
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

flags:blockb(CMD_USER);
CMD:blockb(playerid, params[])
{
	if(Account_GetPremiumLevel(playerid) < 1 && !PlayerRewardData[playerid][rBlockOOC] && Account_GetAdminLevel(playerid) < 2)
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei un premium di almeno livello Bronzo o non hai acquistato il blocco PM.");
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

flags:setpremium(CMD_RCON);
CMD:setpremium(playerid, params[])
{
	new id, premiumid, time;
	if(sscanf(params, "udd", id, premiumid, time))
		return SendClientMessage(playerid, COLOR_ERROR, "/setpremium <playerid/partofname> <premium (0 - 3)> <giorni>");
	if(!IsPlayerConnected(id) || IsPlayerNPC(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID Invalido");
	if(premiumid < 0 || premiumid > 3)
		return SendClientMessage(playerid, COLOR_ERROR, "Premium ID non valido");
	if(time < 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Giorni < 0");
	AccountInfo[id][aPremium] = premiumid;
	AccountInfo[id][aPremiumExpiry] = time ? (gettime() + 3600 * 24 * time) : 0;//cellmax;
	Account_Save(id);
	SendMessageToAdmins(true, COLOR_ADMIN, "[ADMIN-ALERT]: %s (%d) ha settato %s (%d) premium livello %d.", 
	AccountInfo[playerid][aName], playerid, AccountInfo[id][aName], id, premiumid);
	return 1;
}