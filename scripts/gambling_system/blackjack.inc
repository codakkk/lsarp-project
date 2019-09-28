
#include "cards"

#include <YSI_Coding\y_hooks>

#define MAX_BLACKJACK_CARDS 			(15)

static 
    bool:IsPlayingBlackJack[MAX_PLAYERS char] = {false, ...}
;

static 
    PlayerTurn[MAX_PLAYERS],
    PlayerGuest[MAX_PLAYERS],
    PlayerStarter[MAX_PLAYERS],
    PlayerBet[MAX_PLAYERS],
    StarterStatus[MAX_PLAYERS],
    GuestStatus[MAX_PLAYERS],
    StarterCardCount[MAX_PLAYERS],
    GuestCardCount[MAX_PLAYERS]
;

static 
    Text:GlobalBJ_TD[7],
	PlayerText:PlayerBJ_TD[MAX_PLAYERS][2],
    PlayerText:StarterCards[MAX_PLAYERS][MAX_BLACKJACK_CARDS],
	PlayerText:GuestCards[MAX_PLAYERS][MAX_BLACKJACK_CARDS]
;

hook OnGameModeInit()
{
    BJ_CreateTD();
    return 1;
}

hook OnCharacterClearData(playerid)
{
    IsPlayingBlackJack{playerid} = false;
	PlayerTurn[playerid] = INVALID_PLAYER_ID;
	PlayerGuest[playerid] = INVALID_PLAYER_ID;
	PlayerStarter[playerid] = INVALID_PLAYER_ID;
	PlayerBet[playerid] = 0;
	StarterStatus[playerid] = 0;
	GuestStatus[playerid] = 0;
	StarterCardCount[playerid] = 0;
	GuestCardCount[playerid] = 0;
    return 1;
}

hook function Character_CanCasino(playerid)
{
	if(Character_CanCasino(playerid))
		return false;
    return continue(playerid);
}

flags:blackjack(CMD_ALIVE_USER);
CMD:blackjack(playerid, params[])
{
	new option[20], id, amount;
	new starter_id;

	if(sscanf(params, "s[20]U(-1)D(-1)", option, id, amount))return
		SendClientMessage(playerid, COLOR_GREY, "/blackjack <rifiuta - gioca - chiama - stai - esci>");

	if(!strcmp(option, "rifiuta", true))
	{
		if(!Request_IsPending(playerid, REQUEST_TYPE_BLACKJACK))
            return SendClientMessage(playerid, COLOR_ERROR, "Non hai richieste per giocare a blackjack.");

		SendClientMessage(PlayerStarter[playerid], COLOR_ERROR, "La tua richiesta per giocare a blackjack è stata rifiutata.");
		SendClientMessage(playerid, COLOR_ERROR, "Hai rifiutato la richiesta di giocare a blackjack.");

		starter_id = Request_GetSender(playerid, REQUEST_TYPE_BLACKJACK);
		SendClientMessage(playerid, COLOR_ERROR, "Hai rifiutato la partita di blackjack.");
		SendFormattedMessage(starter_id, COLOR_ERROR, "%s ha rifiutato la partita di blackjack.", Character_GetRolePlayName(playerid));
		BJ_Stop(playerid, starter_id);
	}
	else if(!strcmp(option, "gioca", true))
	{

		if(Character_CanCasino(playerid))
            return SendClientMessage(playerid, COLOR_ERROR, "Stai già giocando a blackjack.");

		if(id == -1 || amount < 1 || amount > 10000)
            return SendClientMessage(playerid, COLOR_GREY, "/blackjack <gioca> <id/nome> <scommessa(1 - 10000)>");

		if(!Character_IsLogged(id) || !IsPlayerInRangeOfPlayer(playerid, id, 2.0) || id == playerid)
            return SendClientMessage(playerid, COLOR_ERROR, "Questo giocatore non è collegato o non è vicino a te.");

		if(!Character_CanCasino(playerid))
            return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore è già impegnato in una partita.");

		if(Character_GetMoney(playerid) < amount)
            return SendFormattedMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi. ($%d)", amount);

		if(Character_GetMoney(id) < amount)
            return SendFormattedMessage(playerid, COLOR_ERROR, "Il giocatore non ha abbastanza soldi. ($%d)", amount);

        Character_SetRequest(playerid, id, REQUEST_TYPE_BLACKJACK, 0, amount);

		SendFormattedMessage(playerid, COLOR_GREEN, "Hai invitato a %s a giocare a blackjack. (Puntata: %d$ | Vincita Potenziale: %d$)", Character_GetRolePlayName(id), amount, amount * 2);
		SendFormattedMessage(id, COLOR_YELLOW, "%s (banco) ti ha inviato a giocare a blackjack. (Puntata: %d$ | Vincita Potenziale: %d$)", Character_GetRolePlayName(playerid), amount, amount * 2);
	}
	else if(!strcmp(option, "chiama", false))
	{
		if(!Character_CanCasino(playerid))return
			SendClientMessage(playerid, COLOR_ERROR, "Non stai giocando a blackjack.");

		BJ_GiveCard(playerid);
	}
	else if(!strcmp(option, "stai", false))
	{
		if(!Character_CanCasino(playerid))
            return SendClientMessage(playerid, COLOR_ERROR, "Non stai giocando a blackjack.");

		if(PlayerTurn[playerid] == playerid && PlayerStarter[playerid] == playerid)
            return SendClientMessage(playerid, COLOR_ERROR, "Non puoi stare essendo il banco.");

		if(GuestStatus[playerid] == 0)
            return SendClientMessage(playerid, COLOR_ERROR, "Non puoi stare se non hai chiamato nemmeno una carta.");

		if(PlayerTurn[playerid] == playerid && PlayerGuest[playerid] == playerid)
		{
			starter_id = PlayerStarter[playerid];
			PlayerTurn[playerid] = starter_id;
			PlayerTurn[starter_id] = starter_id;
			SendClientMessage(starter_id, COLOR_WHITE, "Il tuo avversario si sta, è il tuo turno.");
			SendClientMessage(playerid, COLOR_WHITE, "Ti stai, è il turno del tuo avversario.");
		}
		else return 1;
	}
	else if(!strcmp(option, "esci", false))
	{
		if(!Character_CanCasino(playerid))return
			SendClientMessage(playerid, COLOR_ERROR, "Non stai giocando a blackjack.");

		new bet = (Character_GetMoney(playerid) < PlayerBet[playerid]) ? Character_GetMoney(playerid) : PlayerBet[playerid];

		if(PlayerStarter[playerid] == playerid)
		{
			SendClientMessage(PlayerGuest[playerid], COLOR_GREEN, "Il tuo avversario ha abbandonato la partita, hai vinto.");
			SendClientMessage(playerid, COLOR_ERROR, "Hai abbandonato la partita, di conseguenza hai perso.");
			Character_GiveMoney(PlayerGuest[playerid], bet, "/blackjack");
			Character_GiveMoney(playerid, -bet, "/blackjack");
			BJ_Stop(playerid, PlayerGuest[playerid]);
		}
		if(PlayerGuest[playerid] == playerid)
		{
			SendClientMessage(PlayerStarter[playerid], COLOR_GREEN, "Il tuo avversario ha abbandonato la partita, hai vinto.");
			SendClientMessage(playerid, COLOR_ERROR, "Hai abbandonato la partita, di conseguenza hai perso.");
			Character_GiveMoney(PlayerStarter[playerid], bet, "/blackjack");
			Character_GiveMoney(playerid, -bet, "/blackjack");
			BJ_Stop(playerid, PlayerStarter[playerid]);
		}
		else return 1;
	}
	else 
        return SendClientMessage(playerid, COLOR_ERROR, "Parametro inesistente.");

	return 1;
}

stock BJ_ShowTDs(playerid)
{
	for(new i; i < 7; i++) TextDrawShowForPlayer(playerid, GlobalBJ_TD[i]);

	PlayerBJ_TD[playerid][0] = CreatePlayerTextDraw(playerid, 213.402343, 301.283599, "21");
	PlayerTextDrawLetterSize(playerid, PlayerBJ_TD[playerid][0], 0.255206, 1.199597);
	PlayerTextDrawAlignment(playerid, PlayerBJ_TD[playerid][0], 1);
	PlayerTextDrawColor(playerid, PlayerBJ_TD[playerid][0], -1);
	PlayerTextDrawSetShadow(playerid, PlayerBJ_TD[playerid][0], 0);
	PlayerTextDrawSetOutline(playerid, PlayerBJ_TD[playerid][0], -1);
	PlayerTextDrawBackgroundColor(playerid, PlayerBJ_TD[playerid][0], 255);
	PlayerTextDrawFont(playerid, PlayerBJ_TD[playerid][0], 2);
	PlayerTextDrawSetProportional(playerid, PlayerBJ_TD[playerid][0], 1);
	PlayerTextDrawSetShadow(playerid, PlayerBJ_TD[playerid][0], 0);

	PlayerBJ_TD[playerid][1] = CreatePlayerTextDraw(playerid, 216.452453, 366.167297, "21");
	PlayerTextDrawLetterSize(playerid, PlayerBJ_TD[playerid][1], 0.255206, 1.199597);
	PlayerTextDrawAlignment(playerid, PlayerBJ_TD[playerid][1], 1);
	PlayerTextDrawColor(playerid, PlayerBJ_TD[playerid][1], -1);
	PlayerTextDrawSetShadow(playerid, PlayerBJ_TD[playerid][1], 0);
	PlayerTextDrawSetOutline(playerid, PlayerBJ_TD[playerid][1], -1);
	PlayerTextDrawBackgroundColor(playerid, PlayerBJ_TD[playerid][1], 255);
	PlayerTextDrawFont(playerid, PlayerBJ_TD[playerid][1], 2);
	PlayerTextDrawSetProportional(playerid, PlayerBJ_TD[playerid][1], 1);
	PlayerTextDrawSetShadow(playerid, PlayerBJ_TD[playerid][1], 0);

	PlayerTextDrawSetString(playerid, PlayerBJ_TD[playerid][0], "0");
	PlayerTextDrawSetString(playerid, PlayerBJ_TD[playerid][1], "0");

	PlayerTextDrawShow(playerid, PlayerBJ_TD[playerid][0]);
	PlayerTextDrawShow(playerid, PlayerBJ_TD[playerid][1]); return 1;
}

stock BJ_HideTDs(playerid)
{
    for(new i; i < 7; i++)
	{
		TextDrawHideForPlayer(playerid, GlobalBJ_TD[i]);
	}

	PlayerTextDrawHide(playerid, PlayerBJ_TD[playerid][0]);
	PlayerTextDrawHide(playerid, PlayerBJ_TD[playerid][1]);

	for(new j; j < MAX_BLACKJACK_CARDS; j++)
	{
		PlayerTextDrawHide(playerid, StarterCards[playerid][j]);
		PlayerTextDrawHide(playerid, GuestCards[playerid][j]);
	}
    return 1;
}

stock BJ_GiveCard(playerid)
{
	new add_point = random(51);
	new total_point_str[8];
    new cardName_str[36];
    new card_id;
    
    new id = (StarterStatus[playerid] > GuestStatus[playerid]) ? PlayerGuest[playerid] : playerid;
    new bet = (Character_GetMoney(id) < PlayerBet[id]) ? Character_GetMoney(id) : PlayerBet[id];

	if(PlayerTurn[playerid] == playerid && PlayerStarter[playerid] == playerid)
	{
		new guest_id = PlayerGuest[playerid];

		StarterStatus[playerid] += Card_GetValue(add_point);
		StarterStatus[guest_id] = StarterStatus[playerid];

		format(total_point_str, sizeof(total_point_str), "%d", StarterStatus[playerid]);
		PlayerTextDrawSetString(playerid, PlayerBJ_TD[playerid][0], total_point_str);
		PlayerTextDrawSetString(guest_id, PlayerBJ_TD[guest_id][0], total_point_str);
        
        Card_GetName(add_point, cardName_str);

		if(StarterCardCount[playerid] < 1)
		{

			StarterCards[playerid][0] = CreatePlayerTextDraw(playerid, 177.775054, 250.000198, cardName_str);
			PlayerTextDrawLetterSize(playerid, StarterCards[playerid][0], 0.000000, 0.000000);
			PlayerTextDrawTextSize(playerid, StarterCards[playerid][0], 35.000000, 48.000000);
			PlayerTextDrawAlignment(playerid, StarterCards[playerid][0], 1);
			PlayerTextDrawColor(playerid, StarterCards[playerid][0], -1);
			PlayerTextDrawSetShadow(playerid, StarterCards[playerid][0], 0);
			PlayerTextDrawSetOutline(playerid, StarterCards[playerid][0], 0);
			PlayerTextDrawBackgroundColor(playerid, StarterCards[playerid][0], 255);
			PlayerTextDrawFont(playerid, StarterCards[playerid][0], 4);
			PlayerTextDrawSetProportional(playerid, StarterCards[playerid][0], 0);
			PlayerTextDrawSetShadow(playerid, StarterCards[playerid][0], 0);
			PlayerTextDrawShow(playerid, StarterCards[playerid][0]);
			StarterCardCount[playerid]++;

			StarterCards[guest_id][0] = CreatePlayerTextDraw(guest_id, 177.775054, 250.000198, cardName_str);
			PlayerTextDrawLetterSize(guest_id, StarterCards[guest_id][0], 0.000000, 0.000000);
			PlayerTextDrawTextSize(guest_id, StarterCards[guest_id][0], 35.000000, 48.000000);
			PlayerTextDrawAlignment(guest_id, StarterCards[guest_id][0], 1);
			PlayerTextDrawColor(guest_id, StarterCards[guest_id][0], -1);
			PlayerTextDrawSetShadow(guest_id, StarterCards[guest_id][0], 0);
			PlayerTextDrawSetOutline(guest_id, StarterCards[guest_id][0], 0);
			PlayerTextDrawBackgroundColor(guest_id, StarterCards[guest_id][0], 255);
			PlayerTextDrawFont(guest_id, StarterCards[guest_id][0], 4);
			PlayerTextDrawSetProportional(guest_id, StarterCards[guest_id][0], 0);
			PlayerTextDrawSetShadow(guest_id, StarterCards[guest_id][0], 0);
			PlayerTextDrawShow(guest_id, StarterCards[guest_id][0]);
		}
		else
		{
			card_id = StarterCardCount[playerid];

			StarterCards[playerid][card_id] = CreatePlayerTextDraw(playerid, 177.775054 + (card_id * 40), 250.000198, cardName_str);
			PlayerTextDrawLetterSize(playerid, StarterCards[playerid][card_id], 0.000000, 0.000000);
			PlayerTextDrawTextSize(playerid, StarterCards[playerid][card_id], 35.000000, 48.000000);
			PlayerTextDrawAlignment(playerid, StarterCards[playerid][card_id], 1);
			PlayerTextDrawColor(playerid, StarterCards[playerid][card_id], -1);
			PlayerTextDrawSetShadow(playerid, StarterCards[playerid][card_id], 0);
			PlayerTextDrawSetOutline(playerid, StarterCards[playerid][card_id], 0);
			PlayerTextDrawBackgroundColor(playerid, StarterCards[playerid][card_id], 255);
			PlayerTextDrawFont(playerid, StarterCards[playerid][card_id], 4);
			PlayerTextDrawSetProportional(playerid, StarterCards[playerid][card_id], 0);
			PlayerTextDrawSetShadow(playerid, StarterCards[playerid][card_id], 0);
			PlayerTextDrawShow(playerid, StarterCards[playerid][card_id]);
			StarterCardCount[playerid]++;

			StarterCards[guest_id][card_id] = CreatePlayerTextDraw(guest_id, 177.775054 + (card_id * 40), 250.000198, cardName_str);
			PlayerTextDrawLetterSize(guest_id, StarterCards[guest_id][card_id], 0.000000, 0.000000);
			PlayerTextDrawTextSize(guest_id, StarterCards[guest_id][card_id], 35.000000, 48.000000);
			PlayerTextDrawAlignment(guest_id, StarterCards[guest_id][card_id], 1);
			PlayerTextDrawColor(guest_id, StarterCards[guest_id][card_id], -1);
			PlayerTextDrawSetShadow(guest_id, StarterCards[guest_id][card_id], 0);
			PlayerTextDrawSetOutline(guest_id, StarterCards[guest_id][card_id], 0);
			PlayerTextDrawBackgroundColor(guest_id, StarterCards[guest_id][card_id], 255);
			PlayerTextDrawFont(guest_id, StarterCards[guest_id][card_id], 4);
			PlayerTextDrawSetProportional(guest_id, StarterCards[guest_id][card_id], 0);
			PlayerTextDrawSetShadow(guest_id, StarterCards[guest_id][card_id], 0);
			PlayerTextDrawShow(guest_id, StarterCards[guest_id][card_id]);
		}
		if(StarterStatus[playerid] > 21)
		{
			SendClientMessage(playerid, COLOR_ERROR, "Hai sballato, il tuo avversario ha vinto.");
			SendClientMessage(guest_id, COLOR_GREEN, "Il tuo avversario ha sballato, hai vinto.");
			Character_GiveMoney(guest_id, bet, "/blackjack");
			Character_GiveMoney(playerid, -bet, "/blackjack");
			return BJ_Stop(playerid, guest_id);
		}
		if(StarterStatus[playerid] == GuestStatus[playerid])
		{
			SendClientMessage(playerid, COLOR_ERROR, "Hai fatto lo stesso punteggio del tuo avversario, parità.");
			SendClientMessage(guest_id, COLOR_ERROR, "Il tuo avversario ha fatto il tuo stesso punteggio, parità.");
			return BJ_Stop(playerid, guest_id);
		}
		if(StarterStatus[playerid] > GuestStatus[playerid])
		{
			SendClientMessage(playerid, COLOR_GREEN, "Hai superato il punteggio del tuo avversario, hai vinto.");
			SendClientMessage(guest_id, COLOR_ERROR, "Il tuo avversario ha superato il tuo punteggio, hai perso.");
			Character_GiveMoney(guest_id, -bet, "/blackjack");
			Character_GiveMoney(playerid, bet, "/blackjack");
			return BJ_Stop(playerid, guest_id);
		}
		else return 1;
	}
	else if(PlayerTurn[playerid] == playerid && PlayerGuest[playerid] == playerid)
	{
		new starter_id = PlayerStarter[playerid];

		GuestStatus[playerid] += Card_GetValue(add_point);
		GuestStatus[starter_id] = GuestStatus[playerid];
		format(total_point_str, sizeof(total_point_str), "%d", GuestStatus[playerid]);
		PlayerTextDrawSetString(playerid, PlayerBJ_TD[playerid][1], total_point_str);
		PlayerTextDrawSetString(starter_id, PlayerBJ_TD[starter_id][1], total_point_str);

        Card_GetName(add_point, cardName_str);

		if(GuestCardCount[playerid] < 1)
		{

			GuestCards[playerid][0] = CreatePlayerTextDraw(playerid, 177.775054, 318.204345, cardName_str);
			PlayerTextDrawLetterSize(playerid, GuestCards[playerid][0], 0.000000, 0.000000);
			PlayerTextDrawTextSize(playerid, GuestCards[playerid][0], 35.000000, 48.000000);
			PlayerTextDrawAlignment(playerid, GuestCards[playerid][0], 1);
			PlayerTextDrawColor(playerid, GuestCards[playerid][0], -1);
			PlayerTextDrawSetShadow(playerid, GuestCards[playerid][0], 0);
			PlayerTextDrawSetOutline(playerid, GuestCards[playerid][0], 0);
			PlayerTextDrawBackgroundColor(playerid, GuestCards[playerid][0], 255);
			PlayerTextDrawFont(playerid, GuestCards[playerid][0], 4);
			PlayerTextDrawSetProportional(playerid, GuestCards[playerid][0], 0);
			PlayerTextDrawSetShadow(playerid, GuestCards[playerid][0], 0);
			PlayerTextDrawShow(playerid, GuestCards[playerid][0]);
			GuestCardCount[playerid]++;

			GuestCards[starter_id][0] = CreatePlayerTextDraw(starter_id, 177.775054, 318.204345, cardName_str);
			PlayerTextDrawLetterSize(starter_id, GuestCards[starter_id][0], 0.000000, 0.000000);
			PlayerTextDrawTextSize(starter_id, GuestCards[starter_id][0], 35.000000, 48.000000);
			PlayerTextDrawAlignment(starter_id, GuestCards[starter_id][0], 1);
			PlayerTextDrawColor(starter_id, GuestCards[starter_id][0], -1);
			PlayerTextDrawSetShadow(starter_id, GuestCards[starter_id][0], 0);
			PlayerTextDrawSetOutline(starter_id, GuestCards[starter_id][0], 0);
			PlayerTextDrawBackgroundColor(starter_id, GuestCards[starter_id][0], 255);
			PlayerTextDrawFont(starter_id, GuestCards[starter_id][0], 4);
			PlayerTextDrawSetProportional(starter_id, GuestCards[starter_id][0], 0);
			PlayerTextDrawSetShadow(starter_id, GuestCards[starter_id][0], 0);
			PlayerTextDrawShow(starter_id, GuestCards[starter_id][0]);
		}
		else
		{
			card_id = GuestCardCount[playerid];

			GuestCards[playerid][card_id] = CreatePlayerTextDraw(playerid, 177.775054 + (card_id * 40), 318.204345, cardName_str);
			PlayerTextDrawLetterSize(playerid, GuestCards[playerid][card_id], 0.000000, 0.000000);
			PlayerTextDrawTextSize(playerid, GuestCards[playerid][card_id], 35.000000, 48.000000);
			PlayerTextDrawAlignment(playerid, GuestCards[playerid][card_id], 1);
			PlayerTextDrawColor(playerid, GuestCards[playerid][card_id], -1);
			PlayerTextDrawSetShadow(playerid, GuestCards[playerid][card_id], 0);
			PlayerTextDrawSetOutline(playerid, GuestCards[playerid][card_id], 0);
			PlayerTextDrawBackgroundColor(playerid, GuestCards[playerid][card_id], 255);
			PlayerTextDrawFont(playerid, GuestCards[playerid][card_id], 4);
			PlayerTextDrawSetProportional(playerid, GuestCards[playerid][card_id], 0);
			PlayerTextDrawSetShadow(playerid, GuestCards[playerid][card_id], 0);
			PlayerTextDrawShow(playerid, GuestCards[playerid][card_id]);
			GuestCardCount[playerid]++;

			GuestCards[starter_id][card_id] = CreatePlayerTextDraw(starter_id, 177.775054 + (card_id * 40), 318.204345, cardName_str);
			PlayerTextDrawLetterSize(starter_id, GuestCards[starter_id][card_id], 0.000000, 0.000000);
			PlayerTextDrawTextSize(starter_id, GuestCards[starter_id][card_id], 35.000000, 48.000000);
			PlayerTextDrawAlignment(starter_id, GuestCards[starter_id][card_id], 1);
			PlayerTextDrawColor(starter_id, GuestCards[starter_id][card_id], -1);
			PlayerTextDrawSetShadow(starter_id, GuestCards[starter_id][card_id], 0);
			PlayerTextDrawSetOutline(starter_id, GuestCards[starter_id][card_id], 0);
			PlayerTextDrawBackgroundColor(starter_id, GuestCards[starter_id][card_id], 255);
			PlayerTextDrawFont(starter_id, GuestCards[starter_id][card_id], 4);
			PlayerTextDrawSetProportional(starter_id, GuestCards[starter_id][card_id], 0);
			PlayerTextDrawSetShadow(starter_id, GuestCards[starter_id][card_id], 0);
			PlayerTextDrawShow(starter_id, GuestCards[starter_id][card_id]);
		}
		if(GuestStatus[playerid] > 21)
		{
			SendClientMessage(playerid, COLOR_ERROR, "Hai sballato, il tuo avversario ha vinto.");
			SendClientMessage(starter_id, COLOR_GREEN, "Il tuo avversario ha fatto sballato, hai vinto!");
			Character_GiveMoney(starter_id, bet, "/blackjack");
			Character_GiveMoney(playerid, -bet, "/blackjack");
			BJ_Stop(playerid, starter_id);
		}
		else if(GuestStatus[playerid] == 21)
		{
			PlayerTurn[playerid] = starter_id;
			PlayerTurn[starter_id] = starter_id;
			SendClientMessage(playerid, COLOR_GREEN, "Hai fatto 21! Adesso è il turno del tuo avversario.");
			SendClientMessage(starter_id, COLOR_ERROR, "Il tuo avversario ha fatto 21! Adesso è il tuo turno.");
		}
		else return 1;
	}
	else SendClientMessage(playerid, COLOR_ERROR, "Non è il tuo turno.");

	return 1;
}

stock BJ_Stop(playerid, pid)
{
	Character_SetFreezed(playerid, false);
    Character_SetFreezed(pid, false);

    BJ_HideTDs(playerid);
    BJ_HideTDs(pid);

	PlayerStarter[playerid] = INVALID_PLAYER_ID;
    PlayerGuest[playerid] = INVALID_PLAYER_ID;
    PlayerTurn[playerid] = INVALID_PLAYER_ID;
    Request_Reset(playerid, REQUEST_TYPE_BLACKJACK);
	StarterStatus[playerid] = 0;
	GuestStatus[playerid] = 0;
	PlayerBet[playerid] = 0;
	IsPlayingBlackJack{playerid} = false;
    StarterCardCount[playerid] = 0;
    GuestCardCount[playerid] = 0;

	PlayerStarter[pid] = INVALID_PLAYER_ID;
    PlayerGuest[pid] = INVALID_PLAYER_ID;
    PlayerTurn[pid] = INVALID_PLAYER_ID;
    Request_Reset(pid, REQUEST_TYPE_BLACKJACK);
	StarterStatus[pid] = 0;
	GuestStatus[pid] = 0;
	PlayerBet[pid] = 0;
	IsPlayingBlackJack{pid} = false;
    StarterCardCount[pid] = 0;
    GuestCardCount[pid] = 0;

	return 1;
}

stock BJ_CreateTD()
{
	GlobalBJ_TD[0] = TextDrawCreate(175.125274, 240.750030, "_");
	TextDrawLetterSize(GlobalBJ_TD[0], 0.339374, 15.092494);
	TextDrawTextSize(GlobalBJ_TD[0], 467.500274, 0.000000);
	TextDrawAlignment(GlobalBJ_TD[0], 1);
	TextDrawColor(GlobalBJ_TD[0], -1);
	TextDrawUseBox(GlobalBJ_TD[0], 1);
	TextDrawBoxColor(GlobalBJ_TD[0], 120);
	TextDrawSetShadow(GlobalBJ_TD[0], 0);
	TextDrawSetOutline(GlobalBJ_TD[0], 0);
	TextDrawBackgroundColor(GlobalBJ_TD[0], 50);
	TextDrawFont(GlobalBJ_TD[0], 2);
	TextDrawSetProportional(GlobalBJ_TD[0], 1);
	TextDrawSetShadow(GlobalBJ_TD[0], 0);

	GlobalBJ_TD[1] = TextDrawCreate(173.689117, 225.562026, "Blackjack");
	TextDrawLetterSize(GlobalBJ_TD[1], 0.419999, 1.483332);
	TextDrawAlignment(GlobalBJ_TD[1], 1);
	TextDrawColor(GlobalBJ_TD[1], -1061109505);
	TextDrawSetShadow(GlobalBJ_TD[1], 0);
	TextDrawSetOutline(GlobalBJ_TD[1], 1);
	TextDrawBackgroundColor(GlobalBJ_TD[1], 255);
	TextDrawFont(GlobalBJ_TD[1], 3);
	TextDrawSetProportional(GlobalBJ_TD[1], 1);
	TextDrawSetShadow(GlobalBJ_TD[1], 0);

	GlobalBJ_TD[2] = TextDrawCreate(175.624847, 317.550445, "_");
	TextDrawLetterSize(GlobalBJ_TD[2], 0.340068, -0.580341);
	TextDrawTextSize(GlobalBJ_TD[2], 466.510498, 0.000000);
	TextDrawAlignment(GlobalBJ_TD[2], 1);
	TextDrawColor(GlobalBJ_TD[2], -2139094785);
	TextDrawUseBox(GlobalBJ_TD[2], 1);
	TextDrawBoxColor(GlobalBJ_TD[2], -1061109505);
	TextDrawSetShadow(GlobalBJ_TD[2], 0);
	TextDrawSetOutline(GlobalBJ_TD[2], 0);
	TextDrawBackgroundColor(GlobalBJ_TD[2], 255);
	TextDrawFont(GlobalBJ_TD[2], 1);
	TextDrawSetProportional(GlobalBJ_TD[2], 1);
	TextDrawSetShadow(GlobalBJ_TD[2], 0);

	GlobalBJ_TD[3] = TextDrawCreate(175.400024, 301.283599, "BANCO:");
	TextDrawLetterSize(GlobalBJ_TD[3], 0.255206, 1.199597);
	TextDrawAlignment(GlobalBJ_TD[3], 1);
	TextDrawColor(GlobalBJ_TD[3], -1);
	TextDrawSetShadow(GlobalBJ_TD[3], 0);
	TextDrawSetOutline(GlobalBJ_TD[3], -1);
	TextDrawBackgroundColor(GlobalBJ_TD[3], 255);
	TextDrawFont(GlobalBJ_TD[3], 2);
	TextDrawSetProportional(GlobalBJ_TD[3], 1);
	TextDrawSetShadow(GlobalBJ_TD[3], 0);

	GlobalBJ_TD[4] = TextDrawCreate(175.849975, 366.167297, "OSPITE:");
	TextDrawLetterSize(GlobalBJ_TD[4], 0.255206, 1.199597);
	TextDrawAlignment(GlobalBJ_TD[4], 1);
	TextDrawColor(GlobalBJ_TD[4], -1);
	TextDrawSetShadow(GlobalBJ_TD[4], 0);
	TextDrawSetOutline(GlobalBJ_TD[4], -1);
	TextDrawBackgroundColor(GlobalBJ_TD[4], 255);
	TextDrawFont(GlobalBJ_TD[4], 2);
	TextDrawSetProportional(GlobalBJ_TD[4], 1);
	TextDrawSetShadow(GlobalBJ_TD[4], 0);

	GlobalBJ_TD[5] = TextDrawCreate(175.499725, 241.867141, "_");
	TextDrawLetterSize(GlobalBJ_TD[5], 0.340068, -0.580341);
	TextDrawTextSize(GlobalBJ_TD[5], 466.940032, 0.000000);
	TextDrawAlignment(GlobalBJ_TD[5], 1);
	TextDrawColor(GlobalBJ_TD[5], -2139094785);
	TextDrawUseBox(GlobalBJ_TD[5], 1);
	TextDrawBoxColor(GlobalBJ_TD[5], -1061109505);
	TextDrawSetShadow(GlobalBJ_TD[5], 0);
	TextDrawSetOutline(GlobalBJ_TD[5], 0);
	TextDrawBackgroundColor(GlobalBJ_TD[5], 255);
	TextDrawFont(GlobalBJ_TD[5], 1);
	TextDrawSetProportional(GlobalBJ_TD[5], 1);
	TextDrawSetShadow(GlobalBJ_TD[5], 0);

	GlobalBJ_TD[6] = TextDrawCreate(175.224822, 382.067169, "_");
	TextDrawLetterSize(GlobalBJ_TD[6], 0.340068, -0.580341);
	TextDrawTextSize(GlobalBJ_TD[6], 466.360351, 0.000000);
	TextDrawAlignment(GlobalBJ_TD[6], 1);
	TextDrawColor(GlobalBJ_TD[6], -2139094785);
	TextDrawUseBox(GlobalBJ_TD[6], 1);
	TextDrawBoxColor(GlobalBJ_TD[6], -1061109505);
	TextDrawSetShadow(GlobalBJ_TD[6], 0);
	TextDrawSetOutline(GlobalBJ_TD[6], 0);
	TextDrawBackgroundColor(GlobalBJ_TD[6], 255);
	TextDrawFont(GlobalBJ_TD[6], 1);
	TextDrawSetProportional(GlobalBJ_TD[6], 1);
	TextDrawSetShadow(GlobalBJ_TD[6], 0);
}

stock Character_AcceptBlackJack(playerid)
{
    new senderid = Request_GetSender(playerid, REQUEST_TYPE_BLACKJACK);
		
    if(!Request_IsPending(playerid, REQUEST_TYPE_BLACKJACK) || senderid == playerid)
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai richieste per giocare a blackjack.");
    
    if(!IsPlayerInRangeOfPlayer(playerid, senderid, 2.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è vicino a te.");

    if(Character_CanCasino(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Stai già giocando a blackjack.");

    PlayerStarter[playerid] = senderid;
    PlayerGuest[playerid] = playerid;
    PlayerStarter[senderid] = senderid;
    PlayerGuest[senderid] = playerid;

    IsPlayingBlackJack{playerid} = true;
    IsPlayingBlackJack{senderid} = true;

    Character_SetFreezed(playerid, true);
    Character_SetFreezed(senderid, true);

    PlayerTurn[playerid] = playerid;
    PlayerTurn[senderid] = playerid;

    SendClientMessage(playerid, 0xCECECEFF, "Hai accettato di giocare a blackjack.");
    SendClientMessage(playerid, 0xCECECEFF, "È il tuo turno, utilizza \"/blackjack chiama\" e \"/blackjack stai\"...");
    SendClientMessage(playerid, 0xCECECEFF, "...per chiamare una carta o stare.");
    SendFormattedMessage(senderid, 0xCECECEFF, "%s ha accettato di giocare. Dovrà chiamare lui per primo.", Character_GetRolePlayName(playerid));

    BJ_ShowTDs(senderid);
    BJ_ShowTDs(playerid);

    Request_Reset(playerid, REQUEST_TYPE_BLACKJACK);
    Request_Reset(senderid, REQUEST_TYPE_BLACKJACK);
    return 1;
}