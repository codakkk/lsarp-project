

#include <faction_system\core>
#include <jobs\core>

#include <YSI_Coding\y_hooks>

static PayDay[MAX_PLAYERS char];

hook OnCharacterLoad(playerid)
{
	await mysql_aquery_s(gMySQL, str_format("SELECT payday FROM `characters` WHERE id = '%d'", Character_GetID(playerid)));
	new v = 0;
	cache_get_value_name_int(0, "payday", v);
	PayDay{playerid} = v;
	return 1;
}

ptask OnPlayerPayDayMinute[60000](playerid) // 
{
	if(!Character_IsLogged(playerid))
		return 1;
	Character_AddPayDay(playerid, 1);
	Character_AddMinutesPlayed(playerid, 1);
	if(Character_GetPayDay(playerid) >= 60) // Pay Day Time in minutes
	{
		Character_AddPayDay(playerid, -60);
		Character_PayDay(playerid);
	}
	return 1;
}

stock Character_PayDay(playerid)
{
	new experiencePerLevel = (Character_GetLevel(playerid)+1) * 2;
	Character_AddExperience(playerid, 1);

	if(Character_GetExperience(playerid) >= experiencePerLevel)
	{
		Character_IncreaseLevel(playerid, 1);
	   	Character_SetExperience(playerid, 0);
	}

	new 
		startBankMoney = Character_GetBank(playerid),
		factionMoney = 0,
		premiumMoney = 0,
		businessMoney = 0,
		newPlayerMoney = 0, 
		jobMoney = 0,
		interests = 0,
		paycheck = 0,
		rentPrice = 0,
		bool:kickedFromHouse = false
	;

	HandlePlayerHouseRent(playerid, rentPrice, kickedFromHouse);
	
	premiumMoney = GetPlayerMoneyByPremium(playerid);
	paycheck += premiumMoney;

	factionMoney = GetPlayerFactionSalary(playerid);
	paycheck += factionMoney;

	newPlayerMoney = GetPlayerMoneyForLevel(playerid);
	paycheck += newPlayerMoney;

	businessMoney = GetPlayerBuildingMoney(playerid);
	paycheck += businessMoney;

	jobMoney = GetPlayerMoneyByJob(playerid);
	paycheck += jobMoney;

	Character_AddPayCheck(playerid, paycheck);

	new h, m, s;
	gettime(h, m, s);

	SendFormattedMessage(playerid, -1, "SERVER TIME: %d:%d:%d", h, m, s);
    SendClientMessage(playerid, -1, "|__CONTO BANCARIO__|");
	SendFormattedMessage(playerid, 0xB4B5B7FF, "Bilancio: $%s", FormatNumber(startBankMoney));
	//SendFormattedMessage(playerid, 0xB4B5B7FF, "Tasso di interesse: %f", 0.1);
	//SendFormattedMessage(playerid, 0xB4B5B7FF, "Interessi guadagnati: $%s", FormatNumber(interests));
	SendFormattedMessage(playerid, 0xB4B5B7FF, "Tasse pagate: -$%d", 0);
	SendFormattedMessage(playerid, 0xB4B5B7FF, "Affitto: -$%s", FormatNumber(rentPrice));
	SendFormattedMessage(playerid, 0xB4B5B7FF, "Stipendio:  $%s", FormatNumber(paycheck));
	SendClientMessage(playerid, -1, "|__________________|");
	SendFormattedMessage(playerid, -1, "Nuovo Bilancio: $%s", FormatNumber(Character_GetBank(playerid)));
	
	if(factionMoney > 0)
	{
		new shortName[MAX_FACTION_SHORT_NAME];
		Faction_GetShortName(Character_GetFaction(playerid), shortName);
		SendFormattedMessage(playerid, -1, "Hai ricevuto $%s per il tuo lavoro %s nel conto stipendio.", FormatNumber(factionMoney), shortName);
	}

	if(businessMoney > 0)
		SendFormattedMessage(playerid, -1, "Hai ricevuto $%s per il tuo business.", FormatNumber(businessMoney));

	if(premiumMoney > 0)
		SendFormattedMessage(playerid, -1, "Hai ricevuto $%s per il tuo premium.", FormatNumber(premiumMoney));

	if(newPlayerMoney > 0)
		SendFormattedMessage(playerid, -1, "Hai ricevuto $%s perché sei un nuovo arrivato.", FormatNumber(newPlayerMoney));

	if(jobMoney > 0)
	{
		new jobName[32];
		Job_GetName(Character_GetJobType(playerid), jobName);
		SendFormattedMessage(playerid, -1, "Hai ricevuto $%s per il tuo lavoro da \"%s\".", FormatNumber(jobMoney), jobName);
	}

	if(kickedFromHouse)
		SendClientMessage(playerid, COLOR_ORANGE, "La casa di cui eri affittuario ti ha cacciato perché non hai pagato in tempo.");

	Character_AddUpgradePoints(playerid, 1);
	
    GameTextForPlayer(playerid, "~y~PayDay", 5000, 1);
	return 1;
}

static HandlePlayerHouseRent(playerid, &price, &bool:isKicked)
{
	isKicked = false;
	price = 0;
	new House:rentHouse = Character_GetRentHouse(playerid);
	if(House_IsValid(rentHouse))
	{
		price = Character_GetHouseRentPrice(playerid);
		if(price > Character_GetBank(playerid))
		{
			isKicked = true;
			price = 0;
			Character_SetRentHouse(playerid, INVALID_HOUSE);
			Character_SetHouseRentPrice(playerid, 0);
		}
		else
		{
			Character_GiveBank(playerid, -price, "rent_house_rent");
			House_GiveMoney(rentHouse, price);
		}
	}
}

static GetPlayerFactionSalary(playerid)
{
	new money = 0;
	new factionid = Character_GetFaction(playerid);
	if(Faction_IsValid(factionid))
	{
		new salary = Faction_GetRankSalary(factionid, Character_GetRank(playerid));
		/*if(currentPremiumLevel > 1)
		{
			new percentage = currentPremiumLevel == 2 ? 25 : 50;
			new value = (salary * percentage) / 100;
			money += value;
		}*/

		money += salary;
	}
	return money;
}

static GetPlayerMoneyForLevel(playerid)
{
	new money = 0;
	if(Character_GetLevel(playerid) <= 3)
	{
		static const payCheck[] = {4500, 3500, 2500};
		new level = Character_GetLevel(playerid) - 1;
		money = payCheck[level];
	}
	return money;
}

static GetPlayerBuildingMoney(playerid)
{
	if(Character_GetOwnedBuildingCount(playerid) <= 0)
		return 0;
	new money = 0;
	for_player_building(building : playerid)
	{
		if(Building_IsValid(building))
		{
			new price = Building_GetPrice(building)/100;
			money += price;
		}
	}
	return money;
}

static GetPlayerMoneyByPremium(playerid)
{
	new currentPremiumLevel = Account_GetPremiumLevel(playerid);
	new money = 0;
	if(currentPremiumLevel > 0)
	{
		static const premiumPaycheck[] = {3000, 5000, 8000};
		money = premiumPaycheck[currentPremiumLevel-1];
	}
	return money;
}

static GetPlayerMoneyByJob(playerid)
{
	if(Character_GetJobType(playerid) == JOB_TYPE_MECHANIC)
		return 1500;
	return 0;
}

stock Character_GetPayDay(playerid)
{
	return PayDay{playerid};
}

stock Character_AddPayDay(playerid, add)
{
	PayDay{playerid} += add;
	mysql_pquery_f(gMySQL, "UPDATE `characters` SET payday = '%d' WHERE id = '%d'", Character_GetPayDay(playerid), Character_GetID(playerid));
	return 1;
}