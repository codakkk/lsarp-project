
#include <YSI_Coding\y_hooks>

forward OnCharacterHungerChange(playerid);

static bool:IsFoodEnabled = true;

static 
	PlayerHunger[MAX_PLAYERS char],
	PlayerHungerTime[MAX_PLAYERS char],
	pLastHungerAlert[MAX_PLAYERS];

static 
	const 
	HUNGER_DECREASE_TIME = 2, // As minutes
	HUNGER_ALERT_TIME = 30; // As minutes

flags:enablehunger(CMD_ADMIN);
CMD:enablehunger(playerid, params[])
{
	IsFoodEnabled = !IsFoodEnabled;
	if(IsFoodEnabled)
		SendMessageToAdmins(1, COLOR_ORANGE, "AdmCmd: %s (%d) ha abilitato il sistema fame.", Account_GetName(playerid), playerid);
	else
		SendMessageToAdmins(1, COLOR_ORANGE, "AdmCmd: %s (%d) ha disabilitato il sistema fame.", Account_GetName(playerid), playerid);
	return 1;
}

flags:sethunger(CMD_JR_MODERATOR);
CMD:sethunger(playerid, params[])
{
	new id, hunger;
	if(sscanf(params, "k<u>d", id, hunger))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /sethunger <playerid/partofname/####> <valore 0 - 100>");

	if(hunger < 0 || hunger > 100)
		return SendClientMessage(playerid, COLOR_ERROR, "Valore minimo: 0 - Valore Massimo: 100");

	Character_SetHunger(id, hunger, true);
	
	SendMessageToAdmins(0, COLOR_ORANGE, "%s (%d) ha settato la fame di %s (%d) a %d.", Account_GetName(playerid), playerid, Character_GetOOCName(id), id, hunger);
	return 1;
}

hook OnCharacterClearData(playerid)
{
	pLastHungerAlert[playerid] = 0;
	PlayerHungerTime{playerid} = 0;
	Character_SetHunger(playerid, 0, false);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnCharacterLoad(playerid)
{
	inline OnLoad()
	{
		new val;
		cache_get_value_index_int(0, 0, val);
		PlayerHunger{playerid} = val;
		pLastHungerAlert[playerid] = GetTickCount();
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT hunger FROM `characters` WHERE id = '%d'", Character_GetID(playerid));
	return 1;
}

ptask OnPlayerHungerTimer[60000](playerid)
{
	if(!Character_IsLogged(playerid) || !Character_IsAlive(playerid))
		return 0;
	if(!IsFoodEnabled)
		return 0;
	PlayerHungerTime{playerid}++;
	if(PlayerHungerTime{playerid} >= HUNGER_DECREASE_TIME)
	{
		PlayerHungerTime{playerid} = 0;
		Character_AddHunger(playerid, -1, true);
	}
	if(Character_GetHunger(playerid) < 20)
	{
		if(GetTickCount() - pLastHungerAlert[playerid] >= 1000 * 60 * HUNGER_ALERT_TIME) // An alert each 30 minutes
		{
			pLastHungerAlert[playerid] = GetTickCount();
			SendClientMessage(playerid, -1, "Il bisogno di alimentazione comincia a farsi sentire.");
		}
		new Float:hp;
		GetPlayerHealth(playerid, hp);
		if(hp >= 30.0)
			SetPlayerHealth(playerid, hp - 1.0);
	}
	return 1;
}

hook OnCharacterUseItem(playerid, slotid, itemid)
{
	if(!ServerItem_IsFood(itemid) && !ServerItem_IsDrink(itemid))
		return Y_HOOKS_CONTINUE_RETURN_1;
	
	new 
		foodValue = FoodItem_GetFoodValue(itemid),
		healthValue = FoodItem_GetHealthValue(itemid)
	;

	Character_AddHunger(playerid, foodValue, true);
	
	if(healthValue > 0)
	{
		new Float:health = Character_GetHealth(playerid);

		// Prevents removing health-cap (example: drugs assumptions).
		if(health < 100.0)
		{
			health = health + healthValue;
			if(health > 100.0)
				health = 100.0;
			Character_SetHealth(playerid, health);
		}

		SendFormattedMessage(playerid, COLOR_GREEN, "Hai %s %s. Hai recuperato %d punti alimentazione e %d punti salute.", ServerItem_IsFood(itemid) ? ("mangiato") : ("bevuto"), ServerItem_GetName(itemid), foodValue, healthValue);
	}
	else
	{
		SendFormattedMessage(playerid, COLOR_GREEN, "Hai %s %s. Hai recuperato %d punti alimentazione.", ServerItem_IsFood(itemid) ? ("mangiato") : ("bevuto"), ServerItem_GetName(itemid), foodValue);
	}

	Character_AMe(playerid, "comincia a mangiare qualcosa.");
	Character_DecreaseSlotAmount(playerid, slotid, 1);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

stock Character_SetHunger(playerid, Hunger, bool:triggerCallback = false)
{
	if(Hunger > 100)
		Hunger = 100;
	else if(Hunger < 0)
		Hunger = 0;
	PlayerHunger{playerid} = Hunger;
	
	mysql_tquery_f(gMySQL, "UPDATE `characters` SET hunger = '%d' WHERE id = '%d'", Character_GetHunger(playerid), Character_GetID(playerid));

	if(triggerCallback)
		CallLocalFunction(#OnCharacterHungerChange, "d", playerid);
}

stock Character_AddHunger(playerid, amount, bool:triggerCallback = false)
{
	Character_SetHunger(playerid, PlayerHunger{playerid} + amount, triggerCallback);
}

stock Character_GetHunger(playerid)
{
	return PlayerHunger{playerid};
}

stock bool:Character_IsRunning(playerid)
{
	new keys, ud, lr;
	GetPlayerKeys(playerid, keys, ud, lr);
	if(keys & KEY_WALK || (ud == 0 && lr == 0) || !(keys & KEY_SPRINT))
		return false;
	return true;
}