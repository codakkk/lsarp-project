
#include <YSI_Coding\y_hooks>

/*
	ANACONDA RELIGIONE'S ADVICES
	Appena logghi se hai usato tipo 6 droghe, 
	ti arrivano 6 allert di fila. E comunque se si usano 3-4 droghe in pochi attimi, 
	invece di 4 allerts di fila al massimo un'allert unico che dice 
	"La dipendenza di varie droghe si sta facendo sentire" quindi se si ha nel /drugstats 
	più di 4 dipendenze attive, fare un unico allert
	Inoltre, consiglio su 3 ore, 3 allert (1 all'ora)

*/

forward OnCharacterUseDrug(playerid, drugid);

#define MAX_PLAYER_DRUGS 	(11)
#define DRUG_ALERT_TIME 	(5)

static enum _:E_ADDICTION_STATE
{
	ADDICTION_STATE_NONE,
	ADDICTION_STATE_SOFT,
	ADDICTION_STATE_HEAVY
}

static enum e_PlayerDrugAddictionInfo
{
	Drug,
	LastUsageTime,
	NextUsageTime,
	EffectTime, // Next effect time as seconds.
	UsageCounter,
	LastAlertTime
};
static PlayerDrugAddictionInfo[MAX_PLAYERS][MAX_PLAYER_DRUGS][e_PlayerDrugAddictionInfo];

CMD:drugstats(playerid, params[])
{
	new id;
	if(Account_GetAdminLevel(playerid) > 1)
	{
		if(sscanf(params, "k<u>", id))
			id = playerid;
		if(!IsPlayerConnected(id) || !Character_IsLogged(id))
			return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");
	} else id = playerid;
	Character_ShowDrugStats(id, playerid);
	return 1;
}

flags:resetdrugstats(CMD_JR_MODERATOR);
CMD:resetdrugstats(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
		return SendClientMessage(playerid, COLOR_ERROR, "/resetdrugstats <playerid/partofname>");
	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "Giocatore non collegato.");
	SendMessageToAdmins(false, COLOR_ADMINCMD, "AdmAlert: %s ha resettato le stats della droga a %s.", Account_GetName(playerid), Character_GetOOCName(id));
	SendFormattedMessage(id, COLOR_GREEN, "L'admin %s ti ha resettato le stats della droga.", Account_GetName(playerid));
	new data[e_PlayerDrugAddictionInfo];
	for(new i = 0; i < MAX_PLAYER_DRUGS; ++i)
	{
		PlayerDrugAddictionInfo[id][i] = data;
	}
	Character_SaveDrugAddiction(id);
	return 1;
}

stock Character_ShowDrugStats(playerid, targetid)
{
	new count = 0, s[16];
	SendClientMessage(targetid, COLOR_ERROR, "__________[INFO DROGA]__________");
	for(new i = 0, j = MAX_PLAYER_DRUGS; i < j; ++i)
	{
		new drugid = Character_GetDrug(playerid, i);
		if(!ServerItem_IsDrug(drugid) || !DrugItem_CanGetDependent(drugid))//|| drugid == gItem_Marijuana || drugid == gItem_Hashish)
			continue;
		if(Character_GetAddictionState(playerid, drugid) == ADDICTION_STATE_NONE)
			set(s, "No");
		else if(Character_GetAddictionState(playerid, drugid) == ADDICTION_STATE_SOFT)
			set(s, "Lieve");
		else if(Character_GetAddictionState(playerid, drugid) == ADDICTION_STATE_HEAVY)
			set(s, "Pesante");
		SendFormattedMessage(targetid, COLOR_ERROR, "Droga: %s - Dipendente: %s - Ultimo Utilizzo: %d minuti fa.", ServerItem_GetName(drugid), s, PlayerDrugAddictionInfo[playerid][i][LastUsageTime]);
		count++;
	}
	if(count <= 0)
		SendClientMessage(targetid, COLOR_ERROR, "Niente da mostrare.");
	return 1;
}

hook OnPlayerClearData(playerid)
{
	new data[e_PlayerDrugAddictionInfo];
	for(new i = 0; i < MAX_PLAYER_DRUGS; ++i)
	{
		PlayerDrugAddictionInfo[playerid][i] = data;
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnCharacterLoad(playerid)
{
	Character_LoadDrugAddiction(playerid);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

ptask OnPlayerDrugTimer[60000](playerid)
{
	if(Player_IsAdminDuty(playerid) || !Character_IsLogged(playerid))
		return Y_HOOKS_CONTINUE_RETURN_1;
	new drugid = 0;
	for(new i = 0, j = MAX_PLAYER_DRUGS; i < j; ++i)
	{
		drugid = PlayerDrugAddictionInfo[playerid][i][Drug];
		if(!ServerItem_IsDrug(drugid))
			continue;
		PlayerDrugAddictionInfo[playerid][i][LastUsageTime]++;
		if(Character_IsAddictedBy(playerid, drugid))
		{
			if(PlayerDrugAddictionInfo[playerid][i][NextUsageTime] > 0)
			{
				PlayerDrugAddictionInfo[playerid][i][NextUsageTime]--;
			}
			else
			{
				PlayerDrugAddictionInfo[playerid][i][LastAlertTime]--;
			}
		}
		Character_SaveDrugAddiction(playerid);
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerAddictionTimer(playerid)
{
	if(!Character_IsLogged(playerid) || Player_IsAdminDuty(playerid))
		return 1;	
	new drugid = 0;
	for(new i = 0, j = MAX_PLAYER_DRUGS; i < j; ++i)
	{
		drugid = PlayerDrugAddictionInfo[playerid][i][Drug];
		if(!ServerItem_IsDrug(drugid))
			continue;
		if(PlayerDrugAddictionInfo[playerid][i][EffectTime] > 0 && PlayerDrugAddictionInfo[playerid][i][NextUsageTime] > 0)
		{
			PlayerDrugAddictionInfo[playerid][i][EffectTime]--;
			Character_HandleDrugEffect(playerid, drugid, false);
		}
		// Apply side effects
		else if(Character_IsAddictedBy(playerid, drugid) && PlayerDrugAddictionInfo[playerid][i][NextUsageTime] <= 0)
		{
			PlayerDrugAddictionInfo[playerid][i][EffectTime]--;
			if(PlayerDrugAddictionInfo[playerid][i][EffectTime] <= 0)
				Character_HandleDrugSideEffects(playerid, drugid);
			
			if(PlayerDrugAddictionInfo[playerid][i][LastAlertTime] <= 0)
			{
				SendFormattedMessage(playerid, COLOR_ERROR, "La dipendenza da %s comincia a farsi sentire. Assumi una dose o perderai hp.", ServerItem_GetName(drugid));
				PlayerDrugAddictionInfo[playerid][i][LastAlertTime] = DRUG_ALERT_TIME; // 30 minutes
			}
		}
	}
	return 1;
}

hook OnCharacterUseItem(playerid, slotid, itemid)
{
	if(!ServerItem_IsDrug(itemid))
		return Y_HOOKS_CONTINUE_RETURN_1;
	
	// Those are handled by player\components\effects\smoking.inc
	if(itemid == gItem_Marijuana || itemid == gItem_Hashish)
		return 1;

	SendFormattedMessage(playerid, -1, "Hai assunto una dose di %s.", ServerItem_GetName(itemid));

	Character_AMe(playerid, "assume una dose di %s.", ServerItem_GetName(itemid));

	Character_DecreaseSlotAmount(playerid, slotid, 1);
	
	if(DrugItem_CanGetDependent(itemid))
	{
		Character_IncreaseDrugUsage(playerid, itemid);
		if(Character_GetDrugUsageCounter(playerid, itemid) == DrugItem_GetAddictionAmount(itemid))
		{
			SendFormattedMessage(playerid, COLOR_ERROR, "ATTENZIONE: sei diventato dipendente dalla %s. Questo comporterà degli effetti negativi.", ServerItem_GetName(itemid));
		}
	}
	Character_HandleDrugEffect(playerid, itemid, true);
	return Y_HOOKS_BREAK_RETURN_1;
}

static stock Character_HandleDrugSideEffects(playerid, drugid)
{
	if(!ServerItem_IsDrug(drugid))
		return 0;
	//printf("Side effects");
	new drugIndex = GetDrugIndex(drugid),
		Float:currentHealth = 0.0, Float:nextHealth = 0.0, Float:minHealth = 20.0,
		effectTime = 3; // Each n seconds, decrease hp

	AC_GetPlayerHealth(playerid, currentHealth);

	nextHealth = currentHealth - GetDrugHealthLose(drugid);

	if(currentHealth <= minHealth)
		nextHealth = 0.0;
	else if(nextHealth < minHealth)
		nextHealth = minHealth;

	PlayerDrugAddictionInfo[playerid][drugIndex][EffectTime] = effectTime;

	if(nextHealth > 0.0)
		AC_SetPlayerHealth(playerid, nextHealth);
	return 1;
}

static stock Character_HandleDrugEffect(playerid, drugid, bool:firstUsage = false)
{
	if(!ServerItem_IsDrug(drugid))
		return 0;
	new drugIndex = GetDrugIndex(drugid),
		Float:currentHealth, 
		Float:nextHealth = 0,
		Float:maxDrugHealth, effectTime = 0;
	AC_GetPlayerHealth(playerid, currentHealth);

	maxDrugHealth = DrugItem_GetMaxHealth(drugid);
	nextHealth = currentHealth + DrugItem_GetHealthGain(drugid);
	effectTime = DrugItem_GetEffectTime(drugid);

	if(currentHealth >= maxDrugHealth)
		nextHealth = 0;
	else if(nextHealth > maxDrugHealth)
		nextHealth = maxDrugHealth;

	if(firstUsage)
	{
		PlayerDrugAddictionInfo[playerid][drugIndex][Drug] = drugid;
		PlayerDrugAddictionInfo[playerid][drugIndex][EffectTime] = effectTime;
		PlayerDrugAddictionInfo[playerid][drugIndex][LastUsageTime] = 0;
		PlayerDrugAddictionInfo[playerid][drugIndex][NextUsageTime] = GetDrugSideEffectTime(drugid);
		PlayerDrugAddictionInfo[playerid][drugIndex][LastAlertTime] = 0;
		CallLocalFunction(#OnCharacterUseDrug, "dd", playerid, drugid);
	}

	if(nextHealth > 0.0)
		AC_SetPlayerHealth(playerid, nextHealth);
	return 1;
}

stock Character_GetAddictionState(playerid, drugid)
{
	new usageCounter = Character_GetDrugUsageCounter(playerid, drugid),
		drugAddictionAmount = DrugItem_GetAddictionAmount(drugid);
	if(usageCounter >= drugAddictionAmount*2)
		return ADDICTION_STATE_HEAVY;
	else if(usageCounter >= drugAddictionAmount)
		return ADDICTION_STATE_SOFT;
	return ADDICTION_STATE_NONE;
}

stock Character_IsAddictedBy(playerid, drugid)
{
	return Character_GetAddictionState(playerid, drugid) != ADDICTION_STATE_NONE;
}

stock Character_GetDrug(playerid, slotid)
{
	return PlayerDrugAddictionInfo[playerid][slotid][Drug];
}

stock Character_IncreaseDrugUsage(playerid, drugid)
{
	PlayerDrugAddictionInfo[playerid][GetDrugIndex(drugid)][UsageCounter]++;
}

stock Character_SetDrugUsageCounter(playerid, drugid, usageCounter)
{
	PlayerDrugAddictionInfo[playerid][GetDrugIndex(drugid)][UsageCounter] = usageCounter;
}

stock Character_GetDrugUsageCounter(playerid, drugid)
{
	return PlayerDrugAddictionInfo[playerid][GetDrugIndex(drugid)][UsageCounter];
}

static stock GetDrugIndex(drugid)
{
	return drugid - gItem_PCP;
}

static stock GetDrugIDByIndex(index)
{
	return gItem_PCP + index;
}

// gets drug side effect time in minutes.
static stock GetDrugSideEffectTime(drugid)
{
	if(drugid == gItem_PCP)
		return 5;//60 * 3;
	return 5; //60 * 3;
}

static stock Float:GetDrugHealthLose(drugid)
{
	#pragma unused drugid
	return 1.0;
}

static stock Character_LoadDrugAddiction(playerid)
{
	inline OnLoadAddiction()
	{
		new rows = cache_num_rows();
		if(rows >= MAX_PLAYER_DRUGS)
			rows = MAX_PLAYER_DRUGS-1;
		new drugIndex, drugid;
		for(new i = 0; i < rows; ++i)
		{
			cache_get_value_name_int(i, "drug_id", drugid);
			drugIndex = GetDrugIndex(drugid);
			PlayerDrugAddictionInfo[playerid][drugIndex][Drug] = drugid;
			cache_get_value_name_int(i, "last_usage_time", PlayerDrugAddictionInfo[playerid][drugIndex][LastUsageTime]);
			cache_get_value_name_int(i, "usage_counter", PlayerDrugAddictionInfo[playerid][drugIndex][UsageCounter]);
			cache_get_value_name_int(i, "next_usage_time", PlayerDrugAddictionInfo[playerid][drugIndex][NextUsageTime]);
		}
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoadAddiction, "SELECT * FROM `character_drug_info` WHERE character_id = '%d';", Character_GetID(playerid));
}

static stock Character_SaveDrugAddiction(playerid)
{
	mysql_tquery(gMySQL, "START TRANSACTION;");
	new drugid = 0;
	for(new i = 0; i < MAX_PLAYER_DRUGS; ++i)
	{
		drugid = Character_GetDrug(playerid, i);
		if(drugid == 0)
		{
			mysql_tquery_f(gMySQL, "DELETE FROM `character_drug_info` WHERE character_id = '%d' AND drug_id = '%d';", Character_GetID(playerid), drugid);
			continue;
		}
		if(!ServerItem_IsDrug(drugid))
		{
			printf("Unable to save. Drug id invalid. ID: %d", drugid);
			continue;
		}
		mysql_tquery_f(gMySQL, "INSERT INTO `character_drug_info` (character_id, drug_id, last_usage_time, usage_counter, next_usage_time) VALUES('%d', '%d', '%d', '%d', '%d') \
		ON DUPLICATE KEY UPDATE \
		last_usage_time = VALUES(last_usage_time), \
		usage_counter = VALUES(usage_counter), \
		next_usage_time = VALUES(next_usage_time) \
		;", 
		Character_GetID(playerid),
		Character_GetDrug(playerid, i),
		PlayerDrugAddictionInfo[playerid][i][LastUsageTime],
		PlayerDrugAddictionInfo[playerid][i][UsageCounter],
		PlayerDrugAddictionInfo[playerid][i][NextUsageTime]);
	}
	mysql_tquery(gMySQL, "COMMIT;");
	return 1;
}