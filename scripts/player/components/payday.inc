#include <YSI_Coding\y_hooks>

ptask OnPlayerPayDayMinute[60000](playerid) // 
{
	if(!Character_IsLogged(playerid))
		return 1;
	Account_AddPayDay(playerid, 1);
	
	if(Account_GetPayDay(playerid) >= 60) // Pay Day Time in minutes
	{
		Account_AddPayDay(playerid, -60);
		Character_PayDay(playerid);
	}
	return 1;
}

stock Character_PayDay(playerid)
{

    SendClientMessage(playerid, COLOR_YELLOW, "_______________________[PAYDAY]_____________________");
    
	new 
		paycheck = 0,
		currentPremiumLevel = Account_GetPremiumLevel(playerid)
	;

	if(currentPremiumLevel > 0)
	{
		static const premiumPaycheck[] = {1000, 2000, 3000};
		paycheck += premiumPaycheck[currentPremiumLevel-1];
		SendFormattedMessage(playerid, COLOR_YELLOW, "Hai ricevuto $%d aggiuntivi come bonus Premium.", paycheck);
	}

	new factionid = Character_GetFaction(playerid);
	if(factionid != INVALID_FACTION_ID)
	{
		new salary = Faction_GetRankSalary(factionid, Character_GetRank(playerid));
		if(salary > 0)
		{
			
			if(currentPremiumLevel > 1)
			{
				new percentage = currentPremiumLevel == 2 ? 25 : 50;
				new value = (salary * percentage) / 100;
				salary += value;

				SendFormattedMessage(playerid, COLOR_YELLOW, "Hai ricevuto $%d aggiuntivi perché fai parte di una fazione.", salary);
				SendFormattedMessage(playerid, COLOR_YELLOW, "Inoltre, hai ricevuto un bonus del %d percento poiché Premium.", percentage);
			}
			else
			{
				SendFormattedMessage(playerid, COLOR_YELLOW, "Hai ricevuto $%d aggiuntivi perché fai parte di una fazione.", salary);
			}

			paycheck += salary;
		}
	}

	if(Account_GetLevel(playerid) <= 18)
	{
		static const payCheck[] = {2000, 1000, 500};
		new level = Account_GetLevel(playerid);
		if(level <= 4)
			paycheck += payCheck[0];
		else if(level <= 10)
			paycheck += payCheck[1];
		else if(level <= 18)
			paycheck += payCheck[2];
	}

	Character_AddPayCheck(playerid, paycheck);
	SendFormattedMessage(playerid, COLOR_YELLOW, "Stipendio: $%d.", Character_GetPayCheck(playerid));

	if(paycheck > 0)
		SendClientMessage(playerid, COLOR_YELLOW, "Dirigiti verso NOMEPOSTO per ritirare il tuo stipendio.");
    
	static const levels[] = {1, 1, 2, 3};
	new level = levels[Account_GetPremiumLevel(playerid)];
	Account_IncreaseLevel(playerid, level);
	
	if(level > 1)
		SendFormattedMessage(playerid, COLOR_YELLOW, "Livello incrementato di %d punti. (Bonus Premium)", level);
	else
		SendClientMessage(playerid, COLOR_YELLOW, "Livello incrementato di 1 punto.");

	/*if(Character_GetExp(playerid) < expForNewLevel)
    {
	   SendFormattedMessage(playerid, COLOR_YELLOW, "Livello attuale: %d. Attualmente hai %d/%d punti esperienza.", Character_GetLevel(playerid), Character_GetExp(playerid), expForNewLevel);
    }
    else
    {
	   Character_IncreaseLevel(playerid, 1);
	   Character_SetExp(playerid, 0);
	   SetPlayerScore(playerid, Character_GetLevel(playerid));
	   expForNewLevel = (Character_GetLevel(playerid)+1) * 2;
	   SendFormattedMessage(playerid, COLOR_YELLOW, "Congratulazioni! Hai raggiunto il livello %d. Per il prossimo livello hai bisogno di %d punti esperienza.", Character_GetLevel(playerid), expForNewLevel);
    }*/

	if(Character_HasBuildingKey(playerid))
	{
		new id = Character_GetBuildingKey(playerid);
		if(Building_IsValid(id))
		{
			new price = Building_GetPrice(id)/100;
			Building_GiveMoney(id, price);
			SendFormattedMessage(playerid, COLOR_YELLOW, "Il tuo business ti ha fruttato $%d. Puoi trovarli all'interno della cassa.", price);
		}
	}
    SendClientMessage(playerid, COLOR_YELLOW, "____________________________________________________");

    GameTextForPlayer(playerid, "~y~PayDay", 5000, 1);
	return 1;
}