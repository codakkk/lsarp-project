#include <YSI_Coding\y_hooks>

ptask OnPlayerPayDayMinute[60000](playerid) // 
{
	if(!Character_IsLogged(playerid))
		return 1;
	PlayerInfo[playerid][pPayDay]++;
	if(PlayerInfo[playerid][pPayDay] >= 60) // Pay Day Time in minutes
	{
		PlayerInfo[playerid][pPayDay] -= 60;
		Character_PayDay(playerid);
	}
	return 1;
}

stock Character_PayDay(playerid)
{
    new expForNewLevel = (PlayerInfo[playerid][pLevel]+1)*2;
	Character_AddExp(playerid, 1);

    SendClientMessage(playerid, COLOR_YELLOW, "_______________________[PAYDAY]_____________________");
    //SendFormattedMessage(playerid, COLOR_YELLOW, );
    new paycheck = 0;

	if(AccountInfo[playerid][aPremium] > 0)
	{
		static const premiumPaycheck[] = {500, 1000, 1500};
		paycheck += premiumPaycheck[AccountInfo[playerid][aPremium]-1];
		SendFormattedMessage(playerid, COLOR_YELLOW, "Hai ricevuto $%d aggiuntivi come bonus Premium.", paycheck);
	}
	new factionid = Character_GetFaction(playerid);
	if(factionid != -1)
	{
		new salary = Faction_GetRankSalary(factionid, Character_GetRank(playerid)-1);
		if(salary > 0)
		{
			paycheck += salary;
			SendFormattedMessage(playerid, COLOR_YELLOW, "Hai ricevuto $%d aggiuntivi perché fai parte di una fazione.", salary);
		}
	}

	if(Character_GetLevel(playerid) < 4)
	{
		static const payCheck[] = {2000, 1000, 500};
		paycheck += payCheck[Character_GetLevel(playerid) - 1];
	}

	Character_AddPayCheck(playerid, paycheck);
	SendFormattedMessage(playerid, COLOR_YELLOW, "Stipendio: $%d.", Character_GetPayCheck(playerid));
	if(paycheck > 0)
		SendClientMessage(playerid, COLOR_YELLOW, "Dirigiti verso NOMEPOSTO per ritirare il tuo stipendio.");
    if(Character_GetExp(playerid) < expForNewLevel)
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
    }
    SendClientMessage(playerid, COLOR_YELLOW, "____________________________________________________");
	//Character_GiveMoney(playerid, paycheck, "paycheck");
    Character_Save(playerid);
    GameTextForPlayer(playerid, "~y~PayDay", 5000, 1);
}