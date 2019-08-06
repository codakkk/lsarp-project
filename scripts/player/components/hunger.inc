
#include <YSI_Coding\y_hooks>

forward OnCharacterHungerChange(playerid);

static 
	PlayerHunger[MAX_PLAYERS char],
	PlayerHungerTime[MAX_PLAYERS char],
	pLastHungerAlert[MAX_PLAYERS];

static 
	const 
	HUNGER_DECREASE_TIME = 2, // As minutes
	HUNGER_ALERT_TIME = 30; // As minutes

hook OnPlayerClearData(playerid)
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
		cache_get_value_index_int(0, 0, PlayerHunger[playerid]);
		pLastHungerAlert[playerid] = GetTickCount();
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT hunger FROM `characters` WHERE id = '%d'", Character_GetID(playerid));
	return 1;
}

ptask OnPlayerHungerTimer[60000](playerid)
{
	if(!Character_IsLogged(playerid) || !Character_IsAlive(playerid))
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
			SendClientMessage(playerid, -1, "La fame comincia a farsi sentire.");
		}
	}
	return 1;
}

hook OnCharacterUseItem(playerid, slotid, itemid)
{
	if(!ServerItem_IsFood(itemid))
		return Y_HOOKS_CONTINUE_RETURN_1;
	
	new 
		foodValue = FoodItem_GetFoodValue(itemid),
		healthValue = FoodItem_GetHealthValue(itemid)
	;

	Character_AddHunger(playerid, foodValue, true);
	
	if(healthValue > 0)
	{
		new Float:health;
		AC_GetPlayerHealth(playerid, health);

		// Prevents removing health-cap (example: drugs assumptions).
		if(health < 100.0)
		{
			health = health + healthValue;
			if(health > 100.0)
				health = 100.0;
			AC_SetPlayerHealth(playerid, health);
		}

		SendFormattedMessage(playerid, COLOR_GREEN, "Hai mangiato %s. Hai recuperato %d punti fame e %d punti salute.", ServerItem_GetName(itemid), foodValue, healthValue);
	}
	else
	{
		SendFormattedMessage(playerid, COLOR_GREEN, "Hai mangiato %s. Hai recuperato %d punti fame.", ServerItem_GetName(itemid), foodValue);
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