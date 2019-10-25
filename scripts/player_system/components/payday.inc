
#include <faction_system\core>

#include <YSI_Coding\y_hooks>

ptask OnPlayerPayDayMinute[60000](playerid) // 
{
	if(!Character_IsLogged(playerid))
		return 1;
	Account_AddPayDay(playerid, 1);
	Character_AddMinutesPlayed(playerid, 1);
	if(Account_GetPayDay(playerid) >= 60) // Pay Day Time in minutes
	{
		Account_AddPayDay(playerid, -60);
		Character_PayDay(playerid);
	}
	return 1;
}

stock Character_PayDay(playerid)
{
	new 
		startBankMoney = Character_GetBank(playerid),
		factionMoney = 0,
		premiumMoney = 0,
		businessMoney = 0,
		newPlayerMoney = 0, 
		interests = 0,
		paycheck = 0,
		experiencePerLevel = (Character_GetLevel(playerid)+1) * 2,
		currentPremiumLevel = Account_GetPremiumLevel(playerid),
		rentPrice = 0,
		bool:kickedFromHouse = false
	;

	new House:rentHouse = Character_GetRentHouse(playerid);
	if(House_IsValid(rentHouse))
	{
		rentPrice = Character_GetHouseRentPrice(playerid);
		if(rentPrice > Character_GetBank(playerid))
		{
			kickedFromHouse = true;
			rentPrice = 0;
			Character_SetRentHouse(playerid, INVALID_HOUSE);
			Character_SetHouseRentPrice(playerid, 0);
		}
		Character_GiveBank(playerid, -rentPrice, "rent_house_rent");
		House_GiveMoney(rentHouse, rentPrice);
	}

	if(currentPremiumLevel > 0)
	{
		static const premiumPaycheck[] = {1000, 2000, 3000};
		premiumMoney = premiumPaycheck[currentPremiumLevel-1];
		paycheck += premiumMoney;
		//SendFormattedMessage(playerid, COLOR_YELLOW, "Hai ricevuto $%d aggiuntivi come bonus Premium.", paycheck);
	}

	new factionid = Character_GetFaction(playerid);
	if(Faction_IsValid(factionid))
	{
		new salary = Faction_GetRankSalary(factionid, Character_GetRank(playerid));
		if(currentPremiumLevel > 1)
		{
			new percentage = currentPremiumLevel == 2 ? 25 : 50;
			new value = (salary * percentage) / 100;
			factionMoney += value;
		}

		factionMoney += salary;

		paycheck += factionMoney;
	}


	if(Character_GetLevel(playerid) <= 3)
	{
		static const payCheck[] = {2000, 1000, 500};
		new level = Character_GetLevel(playerid) - 1;
		newPlayerMoney = payCheck[level];
		paycheck += newPlayerMoney;
	}

	Character_AddExperience(playerid, 1);
	if(Character_GetExperience(playerid) > experiencePerLevel)
	{
		Character_IncreaseLevel(playerid, 1);
	   	Character_SetExperience(playerid, 0);
	}

	new List:refToList = Character_GetOwnedBuildingsList(playerid);
	if(list_valid(refToList))
	{
		for_list(i : refToList)
		{
			new Building:building = Building:iter_get(i);
			printf("Building: %d", Building_GetID(building));
			if(Building_IsValid(building))
			{
				new price = Building_GetPrice(building)/100;
				businessMoney = price;
				paycheck += price;
			}
		}
	}

	Character_AddPayCheck(playerid, paycheck);

	new h, m, s;
	gettime(h, m, s);

	SendFormattedMessage(playerid, -1, "SERVER TIME: %d:%d:%d", h, m, s);
    SendClientMessage(playerid, -1, "|__CONTO BANCARIO__|");
	SendFormattedMessage(playerid, 0xB4B5B7FF, "Bilancio: $%s", FormatNumber(startBankMoney));
	SendFormattedMessage(playerid, 0xB4B5B7FF, "Tasso di interesse: %f", 0.1);
	SendFormattedMessage(playerid, 0xB4B5B7FF, "Interessi guadagnati: $%s", FormatNumber(interests));
	SendFormattedMessage(playerid, 0xB4B5B7FF, "Tasse pagate: -$%d", 0);
	SendFormattedMessage(playerid, 0xB4B5B7FF, "Affitto: -$%s", FormatNumber(rentPrice));
	SendFormattedMessage(playerid, 0xB4B5B7FF, "Stipendio:  $%s", FormatNumber(paycheck));
	SendClientMessage(playerid, -1, "|__________________|");
	SendFormattedMessage(playerid, -1, "Nuovo Bilancio: $%s", FormatNumber(Character_GetBank(playerid)));
	if(factionMoney > 0)
	{
		new shortName[MAX_FACTION_SHORT_NAME];
		Faction_GetShortName(factionid, shortName);
		SendFormattedMessage(playerid, -1, "Hai ricevuto $%s per il tuo lavoro %s nel conto stipendio.", FormatNumber(factionMoney), shortName);
	}
	if(businessMoney > 0)
		SendFormattedMessage(playerid, -1, "Hai ricevuto $%s per il tuo business.", FormatNumber(businessMoney));
	if(premiumMoney > 0)
		SendFormattedMessage(playerid, -1, "Hai ricevuto $%s per il tuo premium.", FormatNumber(premiumMoney));
	if(newPlayerMoney > 0)
		SendFormattedMessage(playerid, -1, "Hai ricevuto $%s perché sei un nuovo arrivato.", FormatNumber(newPlayerMoney));

	if(kickedFromHouse)
		SendClientMessage(playerid, COLOR_ORANGE, "La casa di cui eri affittuario ti ha cacciato perché non hai pagato in tempo.");

	Character_AddUpgradePoints(playerid, 1);
	
    GameTextForPlayer(playerid, "~y~PayDay", 5000, 1);
	return 1;
}