#include "cards"

#include <YSI_Coding\y_hooks>

#define MAX_PLAYER_POKER_UI				(47)
#define MAX_POKER_TABLE_SLOTS           (6)

static enum E_POKER_TABLE_DATA
{
    POKER_TABLE_ACTIVE,
    POKER_TABLE_ROT,
    POKER_TABLE_SLOTS[MAX_POKER_TABLE_SLOTS],
    POKER_TABLE_GUESTS,
    POKER_TABLE_PULSE_TIMER
};
static 
    PokerTableData[E_POKER_TABLE_DATA],
    PokerTableDataSize = E_POKER_TABLE_DATA;

static PlayerText:PlayerPokerUI[MAX_PLAYERS][MAX_PLAYER_POKER_UI];

static 
    PlayerTableID[MAX_PLAYERS],
    PlayerActionOptions[MAX_PLAYERS]
;

hook OnCharacterClearData(playerid)
{
    Poker_LeaveTable(playerid);
    return 1;
}

hook OnCharacterDeath(playerid, killerid, reason)
{
    Poker_LeaveTable(playerid);
    return 1;
}

hook OnPlayerClickPlayerTD(playerid, PlayerText:playertextid)
{
    if(PlayerTableID[playerid])
	{
		new tableid = PlayerTableID[playerid];

		if(playertextid == PlayerPokerUI[playerid][38]) 
        {
            switch(PlayerActionOptions[playerid])
            {
                case 1:
                {
                    PokerTableData[POKER_TABLE_ROT] = 0;
                    Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);
                    Poker_Raise(playerid);
                }
                case 2: Poker_Call(playerid);
                case 3:
                {
                    Poker_Check(playerid);
                    Poker_RotateGuests(tableid);
                }
                default: return 1;
            }
        }
		else if(playertextid == PlayerPokerUI[playerid][39])
		{
            switch(PlayerActionOptions[playerid])
            {
                case 1: // Check
                {
                    Poker_Check(playerid);
                    Poker_RotateGuests(tableid);
                }
                case 2: // Raise
                {
                    PokerTableData[POKER_TABLE_ROT] = 0;
                    Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);
                    Poker_Raise(playerid);
                }
                case 3: // Fold
                {
                    Poker_Fold(playerid);
                    Poker_RotateGuests(tableid);
                }
                default: return 1;
            }
		}
		else if(playertextid == PlayerPokerUI[playerid][40])
		{
            switch(PlayerActionOptions[playerid])
            {
                case 1: // Fold
                {
                    Poker_Fold(playerid);
                    Poker_RotateGuests(tableid);
                }
                case 2: // Fold
                {
                    Poker_Fold(playerid);
                    Poker_RotateGuests(tableid);
                }
                default: return 1;
            }
		}
		else if(playertextid == PlayerPokerUI[playerid][41] && PlayerTableID[playerid]) Poker_LeaveTable(playerid);
	}
}

stock Poker_WinnerHand(cards[5], hands[2], &p_value, &p_kicker)
{
	new best_value;
	new best_kicker;

	new value;
	new kicker;

	for(new i; i < 21; i++)
	{
		switch(i)
		{
			case 0: Poker_AnalyzeHand(cards[0], cards[1], cards[2], cards[3], cards[4], value, kicker);
			case 1: Poker_AnalyzeHand(cards[0], cards[1], cards[2], hands[0], hands[1], value, kicker);
			case 2: Poker_AnalyzeHand(cards[0], cards[1], cards[3], hands[0], hands[1], value, kicker);
			case 3: Poker_AnalyzeHand(cards[0], cards[1], cards[4], hands[0], hands[1], value, kicker);
			case 4: Poker_AnalyzeHand(cards[0], cards[2], cards[3], hands[0], hands[1], value, kicker);
			case 5: Poker_AnalyzeHand(cards[0], cards[2], cards[4], hands[0], hands[1], value, kicker);
			case 6: Poker_AnalyzeHand(cards[0], cards[3], cards[4], hands[0], hands[1], value, kicker);
			case 7: Poker_AnalyzeHand(cards[1], cards[2], cards[3], hands[0], hands[1], value, kicker);
			case 8: Poker_AnalyzeHand(cards[1], cards[2], cards[4], hands[0], hands[1], value, kicker);
			case 9: Poker_AnalyzeHand(cards[1], cards[3], cards[4], hands[0], hands[1], value, kicker);
			case 10: Poker_AnalyzeHand(cards[2], cards[3], cards[4], hands[0], hands[1], value, kicker);
			case 11: Poker_AnalyzeHand(cards[0], cards[1], cards[2], cards[3], hands[0], value, kicker);
			case 12: Poker_AnalyzeHand(cards[0], cards[1], cards[2], cards[4], hands[0], value, kicker);
			case 13: Poker_AnalyzeHand(cards[0], cards[1], cards[3], cards[4], hands[0], value, kicker);
			case 14: Poker_AnalyzeHand(cards[0], cards[2], cards[3], cards[4], hands[0], value, kicker);
			case 15: Poker_AnalyzeHand(cards[1], cards[2], cards[3], cards[4], hands[0], value, kicker);
			case 16: Poker_AnalyzeHand(cards[0], cards[1], cards[2], cards[3], hands[1], value, kicker);
			case 17: Poker_AnalyzeHand(cards[0], cards[1], cards[2], cards[4], hands[1], value, kicker);
			case 18: Poker_AnalyzeHand(cards[0], cards[1], cards[3], cards[4], hands[1], value, kicker);
			case 19: Poker_AnalyzeHand(cards[0], cards[2], cards[3], cards[4], hands[1], value, kicker);
			case 20: Poker_AnalyzeHand(cards[1], cards[2], cards[3], cards[4], hands[1], value, kicker);
		}

        if(value > best_value)
        {
			best_value = value;
			best_kicker = kicker;
        }
        else if(value == best_value && kicker > best_kicker) best_kicker = kicker;
	}

	p_value = best_value;
	p_kicker = best_kicker; return 1;
}

stock Poker_AnalyzeHand(card_1, card_2, card_3, card_4, card_5, &value, &kicker)
{
    new card[5];
	new card_suit[5];
	new card_value[5];

	card[0] = card_1;
	card[1] = card_2;
	card[2] = card_3;
	card[3] = card_4;
	card[4] = card_5;

	for(new i; i < 5; i++)
	{
	    new tmp_card = card[i];
	    
		card_suit[i] = PokerCards[tmp_card][cardChar];
		card_value[i] = PokerCards[tmp_card][cardValue];
	}

	ArraySort(card_value, 5);

	if(card_value[0] == card_value[1] - 1 && card_value[1] == card_value[2] - 1 && card_value[2] == card_value[3] - 1 && card_value[3] == card_value[4] - 1 &&
	card_suit[0] == card_suit[1] && card_suit[1] == card_suit[2] && card_suit[2] == card_suit[3] && card_suit[3] == card_suit[4])
	{
		value = (card_value[4] == Ace) ? RoyalFlush : StraightFlush;
		kicker = (card_value[4] == Ace) ? Ace : card_value[4];
	}
	else if(card_value[0] == Two && card_value[1] == Three && card_value[2] == Four && card_value[3] == Five && card_value[4] == Ace &&
	card_suit[0] == card_suit[1] && card_suit[1] == card_suit[2] && card_suit[2] == card_suit[3] && card_suit[3] == card_suit[4])
	{
		value = StraightFlush;
		kicker = Five;
	}
	else if(card_value[0] == card_value[1] && card_value[1] == card_value[2] && card_value[2] == card_value[3])
	{
		value = Poker;
		kicker = card_value[0] * 20 + card_value[4];
	}
	else if(card_value[1] == card_value[2] && card_value[2] == card_value[3] && card_value[3] == card_value[4])
	{
		value = Poker;
		kicker = card_value[1] * 20 + card_value[0];
	}
	else if(card_value[0] == card_value[1] && card_value[2] == card_value[3] && card_value[3] == card_value[4])
	{
		value = FullHouse;
		kicker = card_value[0] + card_value[2] * 20;
	}
	else if(card_value[0] == card_value[1] && card_value[1] == card_value[2] && card_value[3] == card_value[4])
	{
		value = FullHouse;
		kicker = card_value[0] * 20 + card_value[3];
	}
	else if(card_suit[0] == card_suit[1] && card_suit[1] == card_suit[2] && card_suit[2] == card_suit[3] && card_suit[3] == card_suit[4])
	{
		value = Flush;
		kicker = card_value[4] * 160000 + card_value[3] * 8000 + card_value[2] * 400 + card_value[1] * 20 + card_value[0];
	}
	else if(card_value[0] == card_value[1] - 1 && card_value[1] == card_value[2] - 1 && card_value[2] == card_value[3] - 1 && card_value[3] == card_value[4] - 1)
	{
		value = Straight;
		kicker = card_value[4];
	}
	else if(card_value[0] == Two && card_value[1] == Three && card_value[2] == Four && card_value[3] == Five && card_value[4] == Ace)
	{
		value = Straight;
	    kicker = Five;
	}
	else if(card_value[0] == card_value[1] && card_value[1] == card_value[2])
	{
		value = ThreeOfKind;
		kicker = card_value[0] * 400 + card_value[3] + card_value[4] * 20;
	}
	else if(card_value[1] == card_value[2] && card_value[2] == card_value[3])
	{
		value = ThreeOfKind;
		kicker = card_value[1] * 400 + card_value[0] + card_value[4] * 20;
	}
	else if(card_value[2] == card_value[3] && card_value[3] == card_value[4])
	{
		value = ThreeOfKind;
		kicker = card_value[1] * 400 + card_value[0] + card_value[1] * 20;
	}
	else if(card_value[0] == card_value[1] && card_value[2] == card_value[3])
	{
		value = TwoPairs;
		kicker = card_value[0] * 20 + card_value[2] * 400 + card_value[4];
	}
	else if(card_value[0] == card_value[1] && card_value[3] == card_value[4])
	{
		value = TwoPairs;
		kicker = card_value[0] * 20 + card_value[3] * 400 + card_value[2];
	}
	else if(card_value[1] == card_value[2] && card_value[3] == card_value[4])
	{
		value = TwoPairs;
		kicker = card_value[1] * 20 + card_value[3] * 400 + card_value[0];
	}
	else if(card_value[0] == card_value[1])
	{
		value = Pair;
		kicker = card_value[0] * 8000 + card_value[2] + card_value[3] * 20 + card_value[4] * 400;
	}
	else if(card_value[1] == card_value[2])
	{
		value = Pair;
		kicker = card_value[1] * 8000 + card_value[0] + card_value[3] * 20 + card_value[4] * 400;
	}
	else if(card_value[2] == card_value[3])
	{
		value = Pair;
		kicker = card_value[2] * 8000 + card_value[0] + card_value[1] * 20 + card_value[4] * 400;
	}
	else if(card_value[3] == card_value[4])
	{
		value = Pair;
		kicker = card_value[3] * 8000 + card_value[0] + card_value[1] * 20 + card_value[2] * 400;
	}
	else
	{
		value = HighCard;
		kicker = card_value[4] * 160000 + card_value[3] * 8000 + card_value[2] * 400 + card_value[1] * 20 + card_value[0];
	}

	return 1;
}

forward Poker_Timer(tableid); public Poker_Timer(tableid)
{
	Streamer_GetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);

	if(!PokerTableData[POKER_TABLE_ACTIVE])return KillTimer(PokerTableData[POKER_TABLE_PULSE_TIMER]);
	if(PokerTableData[POKER_TABLE_GUESTS] <= 0)return Reset_ResetTable(tableid);

	for(new i = 0; i < 6; i++)  if(PokerTableData[POKER_TABLE_SLOTS][i] != -1)
	{
		new playerid = PokerTableData[POKER_TABLE_SLOTS][i];
		new idleRandom = random(100);

		SetPlayerArmedWeapon(playerid, 0);

		if(idleRandom >= 90) 
		{
			SetPlayerPosObjectOffset(tableid, playerid, PokerTableMiscObjOffsets[i][0], PokerTableMiscObjOffsets[i][1], PokerTableMiscObjOffsets[i][2]);
			SetPlayerFacingAngle(playerid, PokerTableMiscObjOffsets[i][5] + 90.0);
			SetPlayerInterior(playerid, PokerTableData[co_table_int]);
			AC_SetPlayerVirtualWorld(playerid, PokerTableData[co_table_vw]);

			if(PlayerInfo[playerid][pActiveHand])  ApplyAnimation(playerid, "CASINO", "cards_loop", 4.1, 0, 1, 1, 1, 1, 1);
		}
		else continue;
	}

	if(PokerTableData[POKER_TABLE_ACTIVE_guests] >= 2 && PokerTableData[POKER_TABLE_ACTIVE] == 2)  for(new i = 0; i < 6; i++)
	{
		new playerid = PokerTableData[POKER_TABLE_SLOTS][i];

		if(playerid != -1 && PlayerInfo[playerid][pChips]) 
		{
			PokerTableData[POKER_TABLE_ACTIVE] = 3;
			PokerTableData[co_table_delay] = PokerTableData[co_table_setdelay]; break;
		}
		else continue;
	}

	Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);

	if(PokerTableData[POKER_TABLE_GUESTS] < 2) if(PokerTableData[POKER_TABLE_ACTIVE] == 3 || PokerTableData[POKER_TABLE_ACTIVE] == 4)  for(new i = 0; i < 6; i++)
	{
		new playerid = PokerTableData[POKER_TABLE_SLOTS][i];

		if(playerid != -1 && PlayerInfo[playerid][pChips]) 
		{
			PlayerInfo[playerid][pChips] += PokerTableData[co_table_pot];

			Poker_LeaveTable(playerid);
		}
		else continue;
	}

	if(PokerTableData[POKER_TABLE_ACTIVE] == 4)
	{
		if(PokerTableData[co_table_delay] == 20)  for(new i = 0; i < 6; i++) if(PokerTableData[POKER_TABLE_SLOTS][i] != -1)
		{
			PlayerPlaySound(PokerTableData[POKER_TABLE_SLOTS][i], 5826, 0.0, 0.0, 0.0);
			Poker_ShowOptions(PokerTableData[POKER_TABLE_SLOTS][i], 0);
		}

		if(PokerTableData[co_table_delay] > 0)
		{
			PokerTableData[co_table_delay]--;

			if(PokerTableData[co_table_delay] <= 5 && PokerTableData[co_table_delay] > 0) for(new i = 0; i < 6; i++) if(PokerTableData[POKER_TABLE_SLOTS][i] != -1) PlayerPlaySound(PokerTableData[POKER_TABLE_SLOTS][i], 1139, 0.0, 0.0, 0.0);
			if(PokerTableData[co_table_delay] == 0) return Poker_ResetTableRound(tableid);
		}

		if(PokerTableData[co_table_delay] == 19)
		{
			new cards[5];
			new hands[2];

			for(new i = 0; i < 6; i++) if(PokerTableData[POKER_TABLE_SLOTS][i] != -1)
			{
				new playerid = PokerTableData[POKER_TABLE_SLOTS][i];

				if(PlayerInfo[playerid][pActiveHand]) 
				{
					for(new c; c < 5; c++) 
						cards[c] = PokerTableData[co_table_cards][c];

					hands[0] = PlayerInfo[playerid][pFirstCard];
					hands[1] = PlayerInfo[playerid][pSecondCard];

					Poker_WinnerHand(cards, hands, PlayerInfo[playerid][pTableResult][0], PlayerInfo[playerid][pTableResult][1]);
				}
				else continue;
			}

			new best_value;
			new best_kicker;

			for(new i = 0; i < 6; i++) if(PokerTableData[POKER_TABLE_SLOTS][i] != -1)
			{
				new playerid = PokerTableData[POKER_TABLE_SLOTS][i];

				if(!PlayerInfo[playerid][pActiveHand])continue;

				if(PlayerInfo[playerid][pTableResult][0] > best_value)
    			{
    			    best_value = PlayerInfo[playerid][pTableResult][0];
    			    best_kicker = PlayerInfo[playerid][pTableResult][1];
				}
				else if(PlayerInfo[playerid][pTableResult][0] == best_value && PlayerInfo[playerid][pTableResult][1] > best_kicker) best_kicker = PlayerInfo[playerid][pTableResult][1];
			}

			for(new i = 0; i < 6; i++) if(PokerTableData[POKER_TABLE_SLOTS][i] != -1)
			{
			    new playerid = PokerTableData[POKER_TABLE_SLOTS][i];
			    
				if(PlayerInfo[playerid][pTableResult][0] == best_value && PlayerInfo[playerid][pTableResult][1] == best_kicker) PokerTableData[co_table_winners]++;
			}

			for(new i = 0; i < 6; i++) if(PokerTableData[POKER_TABLE_SLOTS][i] != -1)
			{
				new playerid = PokerTableData[POKER_TABLE_SLOTS][i];

				if(PlayerInfo[playerid][pTableResult][0] == best_value && PlayerInfo[playerid][pTableResult][1] == best_kicker)
				{
					switch(best_value)
					{
						case HighCard: format(PlayerInfo[playerid][pPokerResultString], 32, "Carta alta");
						case Pair: format(PlayerInfo[playerid][pPokerResultString], 32, "Coppia");
						case TwoPairs: format(PlayerInfo[playerid][pPokerResultString], 32, "Doppia coppia");
						case ThreeOfKind: format(PlayerInfo[playerid][pPokerResultString], 32, "Tris");
						case Straight: format(PlayerInfo[playerid][pPokerResultString], 32, "Scala");
						case Flush: format(PlayerInfo[playerid][pPokerResultString], 32, "Colore");
						case FullHouse: format(PlayerInfo[playerid][pPokerResultString], 32, "Full");
						case Poker: format(PlayerInfo[playerid][pPokerResultString], 32, "Poker");
						case StraightFlush: format(PlayerInfo[playerid][pPokerResultString], 32, "Scala colore");
						case RoyalFlush: format(PlayerInfo[playerid][pPokerResultString], 32, "Scala reale");
					}
					
					new splitPot = PokerTableData[co_table_pot] / PokerTableData[co_table_winners];

					if(PokerTableData[co_table_winners] < 2)
					{
						splitPot = PokerTableData[co_table_pot];
						PokerTableData[co_table_winner_id] = playerid;
						PlayerPlaySound(playerid, 5847, 0.0, 0.0, 0.0);
					}
					else PlayerPlaySound(playerid, 5821, 0.0, 0.0, 0.0);

					PlayerInfo[playerid][pTableWinner] = 1;
					PlayerInfo[playerid][pChips] += splitPot;
				} 
				else PlayerPlaySound(playerid, 31202, 0.0, 0.0, 0.0);
			}
			else continue;
		}

		Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);
	}

	if(PokerTableData[POKER_TABLE_ACTIVE] == 3)
	{
		if(PokerTableData[POKER_TABLE_ACTIVE_hands] == 1 && PokerTableData[co_table_round] == 1)
		{
			for(new i = 0; i < 6; i++) if(PokerTableData[POKER_TABLE_SLOTS][i] != -1)
			{
				new playerid = PokerTableData[POKER_TABLE_SLOTS][i];
						
				if(PlayerInfo[playerid][pActiveHand]) 
					PlayerInfo[playerid][pPokerHide] = 1;
			}

			PokerTableData[co_table_stage] = 0;
			PokerTableData[POKER_TABLE_ACTIVE] = 4;
			PokerTableData[co_table_delay] = 20 + 1;
		}

		if(PokerTableData[co_table_delay] > 0)
		{
			PokerTableData[co_table_delay]--;

			if(PokerTableData[co_table_delay] <= 5 && PokerTableData[co_table_delay] > 0) for(new i = 0; i < 6; i++) if(PokerTableData[POKER_TABLE_SLOTS][i] != -1) PlayerPlaySound(PokerTableData[POKER_TABLE_SLOTS][i], 1139, 0.0, 0.0, 0.0);
		}

		if(PokerTableData[co_table_round] == 0 && PokerTableData[co_table_delay] == 5)
		{
			for(new i = 0; i < 6; i++) if(PokerTableData[POKER_TABLE_SLOTS][i] != -1)
			{
				new playerid = PokerTableData[POKER_TABLE_SLOTS][i];
				PlayerInfo[playerid][pPokerStatus] = 1;					
			}

			Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);
			Poker_AssignBlinds(tableid);
		}

		if(PokerTableData[co_table_round] == 0 && PokerTableData[co_table_delay] == 0)
		{
			PokerTableData[co_table_round] = 1;

			for(new i = 0; i < 6; i++) if(PokerTableData[POKER_TABLE_SLOTS][i] != -1)
			{
				new playerid = PokerTableData[POKER_TABLE_SLOTS][i];
				PlayerInfo[playerid][pPokerStatusString] = '\0';
			}

			Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);
			Poker_ShuffleDeck(tableid);
			Poker_RotateGuests(tableid);
		}

		for(new i = 0; i < 6; i++) if(PokerTableData[POKER_TABLE_SLOTS][i] != -1)
		{
			new playerid = PokerTableData[POKER_TABLE_SLOTS][i];
		
			if(PlayerInfo[playerid][pActiveGuest]) 
			{
				PlayerInfo[playerid][pPokerTime]--;

				if(PlayerInfo[playerid][pPokerTime])continue;
				
				if(PlayerInfo[playerid][pActionChoice]) 
				{
					PlayerInfo[playerid][pActionChoice] = 0;
					Dialog_Close(playerid);
				}

				Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);
				Poker_Fold(playerid);
				Poker_RotateGuests(tableid);
			}
			else continue;
		}

		Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);
	}

	for(new i = 0; i < 6; i++)
	{
		new playerid = PokerTableData[POKER_TABLE_SLOTS][i];
		new tmpString[128];
		new tmp_value[6] = {0, 5, 10, 15, 20, 25};

		if(playerid != -1) 
		{
			new name[MAX_PLAYER_NAME];
			strmid(name, ReturnRoleplayName(playerid, true), 0, MAX_PLAYER_NAME);

			for(new td = 0; td < 6; td++) 
			{
				new pid = PokerTableData[POKER_TABLE_SLOTS][td];
				if(pid != -1) PlayerTextDrawSetString(pid, PlayerPokerUI[pid][tmp_value[i] + 0], name);
			}

			if(PlayerInfo[playerid][pChips] > 0) format(tmpString, sizeof(tmpString), "~y~$%d", PlayerInfo[playerid][pChips]);
			if(PlayerInfo[playerid][pChips] < 0) format(tmpString, sizeof(tmpString), "~r~$%d", PlayerInfo[playerid][pChips]);
			
			for(new td = 0; td < 6; td++) 
			{
				new pid = PokerTableData[POKER_TABLE_SLOTS][td];
				if(pid != -1) PlayerTextDrawSetString(pid, PlayerPokerUI[pid][tmp_value[i] + 1], tmpString);
			}

			for(new td = 0; td < 6; td++) if(PokerTableData[POKER_TABLE_SLOTS][td] != -1)
			{
				new pid = PokerTableData[POKER_TABLE_SLOTS][td];

				if(PlayerInfo[playerid][pActiveHand]) 
				{
					if(playerid != pid) 
					{
						if(PokerTableData[POKER_TABLE_ACTIVE] == 4 && PokerTableData[co_table_delay] <= 19 && PlayerInfo[playerid][pPokerHide] != 1)
						{
							strmid(tmpString, PokerCards[PlayerInfo[playerid][pFirstCard] + 0][cardName], 0, 32);
							PlayerTextDrawSetString(pid, PlayerPokerUI[pid][tmp_value[i] + 2], tmpString);

							strmid(tmpString, PokerCards[PlayerInfo[playerid][pSecondCard] + 0][cardName], 0, 32);
							PlayerTextDrawSetString(pid, PlayerPokerUI[pid][tmp_value[i] + 3], tmpString);
						}
						else 
						{
							PlayerTextDrawSetString(pid, PlayerPokerUI[pid][tmp_value[i] + 2], "LD_CARD:cdback");
							PlayerTextDrawSetString(pid, PlayerPokerUI[pid][tmp_value[i] + 3], "LD_CARD:cdback");
						}

						continue;
					} 
					
					strmid(tmpString, PokerCards[PlayerInfo[playerid][pFirstCard] + 0][cardName], 0, 32);
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][tmp_value[i] + 2], tmpString);

					strmid(tmpString, PokerCards[PlayerInfo[playerid][pSecondCard] + 0][cardName], 0, 32);
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][tmp_value[i] + 3], tmpString);
				} 
				else 
				{
					PlayerTextDrawSetString(pid, PlayerPokerUI[pid][tmp_value[i] + 2], " ");
					PlayerTextDrawSetString(pid, PlayerPokerUI[pid][tmp_value[i] + 3], " ");
				}

				continue;
			}

			if(PokerTableData[POKER_TABLE_ACTIVE] < 3)  format(tmpString, sizeof(tmpString), " ");
			else if(PlayerInfo[playerid][pActiveGuest] && PokerTableData[POKER_TABLE_ACTIVE] == 3) format(tmpString, sizeof(tmpString), "0:%d", PlayerInfo[playerid][pPokerTime]);
			else 
			{
				if(PokerTableData[POKER_TABLE_ACTIVE] == 3 && PokerTableData[co_table_delay] > 5)  PlayerInfo[playerid][pPokerStatusString] = '\0';
				if(PokerTableData[POKER_TABLE_ACTIVE] == 4 && PokerTableData[co_table_delay] == 19)
				{
					if(PokerTableData[co_table_winners] == 1)
					{
						if(PlayerInfo[playerid][pTableWinner])
						{
							format(tmpString, sizeof(tmpString), "+ $%d", PokerTableData[co_table_pot]);
							format(PlayerInfo[playerid][pPokerStatusString], 32, tmpString); 
						} 
						else format(tmpString, sizeof(tmpString), "- $%d", PlayerInfo[playerid][pCurrentBet]);
					} 
					else 
					{
						if(PokerTableData[co_table_winners] > 1)
						{
							new splitPot = PokerTableData[co_table_pot] / PokerTableData[co_table_winners];
							format(tmpString, sizeof(tmpString), "+ $%d", splitPot);
							format(PlayerInfo[playerid][pPokerStatusString], 32, tmpString);
						} 
						else format(tmpString, sizeof(tmpString), "- $%d", PlayerInfo[playerid][pCurrentBet]);
					}

					format(PlayerInfo[playerid][pPokerStatusString], 32, tmpString);	
				}

				if(PokerTableData[POKER_TABLE_ACTIVE] == 4 && PokerTableData[co_table_delay] == 19)
				{
					if(PlayerInfo[playerid][pTableWinner] && PlayerInfo[playerid][pActiveHand] && PlayerInfo[playerid][pPokerHide] != 1)
						format(PlayerInfo[playerid][pPokerStatusString], 32, PlayerInfo[playerid][pPokerResultString]);
				}

				if(PokerTableData[POKER_TABLE_ACTIVE] == 4 && PokerTableData[co_table_delay] == 10)
				{
					if(PokerTableData[co_table_winners] == 1)
					{
						if(PlayerInfo[playerid][pTableWinner])
						{
							format(tmpString, sizeof(tmpString), "+ $%d", PokerTableData[co_table_pot]);
							format(PlayerInfo[playerid][pPokerStatusString], 32, tmpString);
						} 
						else  format(tmpString, sizeof(tmpString), "- $%d", PlayerInfo[playerid][pCurrentBet]);
					} 
					else 
					{
						if(PlayerInfo[playerid][pTableWinner])
						{
							new splitPot = PokerTableData[co_table_pot]/PokerTableData[co_table_winners];
							format(tmpString, sizeof(tmpString), "+ $%d", splitPot);
							format(PlayerInfo[playerid][pPokerStatusString], 32, tmpString);
						} 
						else  format(tmpString, sizeof(tmpString), "- $%d", PlayerInfo[playerid][pCurrentBet]);
					}

					format(PlayerInfo[playerid][pPokerStatusString], 32, tmpString);
				}

				format(tmpString, 32, PlayerInfo[playerid][pPokerStatusString]);
			}

			for(new td = 0; td < 6; td++) 
			{
				new pid = PokerTableData[POKER_TABLE_SLOTS][td];
				if(pid != -1) PlayerTextDrawSetString(pid, PlayerPokerUI[pid][tmp_value[i] + 4], tmpString);
			}

			if(PokerTableData[POKER_TABLE_ACTIVE] == 3)
			{
				format(tmpString, sizeof(tmpString), "Piatto: ~y~$%d", PokerTableData[co_table_pot]);
				PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][37], tmpString);
			} 
			else if(PokerTableData[POKER_TABLE_ACTIVE] == 4 && PokerTableData[co_table_delay] < 19)
			{
				if(PokerTableData[co_table_winner_id] != -1)
					format(tmpString, sizeof(tmpString), "~y~%s ~w~ha vinto ~y~$%d", ReturnRoleplayName(PokerTableData[co_table_winner_id], true), PokerTableData[co_table_pot]);
				 
				else if(PokerTableData[co_table_winners] > 1)
				{
					new splitPot = PokerTableData[co_table_pot]/PokerTableData[co_table_winners];
					format(tmpString, sizeof(tmpString), "~y~%d ~w~giocatori hanno vinto ~y~$%d", PokerTableData[co_table_winners], splitPot);
				}

				PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][37], tmpString);
			} 
			else PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][37], "Texas Holdem Poker");

			if(PokerTableData[co_table_delay] > 0 && PokerTableData[POKER_TABLE_ACTIVE] == 3)
			{
				format(tmpString, sizeof(tmpString), "La mano comincia tra ~r~%d~w~...", PokerTableData[co_table_delay]);
				PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][46], tmpString);
			} 
			else if(PokerTableData[POKER_TABLE_ACTIVE] == 2)
			{
				format(tmpString, sizeof(tmpString), "In attesa di altri giocatori...", PokerTableData[co_table_pot]);
				PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][46], tmpString);
			} 
			else if(PokerTableData[POKER_TABLE_ACTIVE] == 3)
			{
				format(tmpString, sizeof(tmpString), "Puntata: ~y~$%d", PokerTableData[POKER_TABLE_ACTIVE_bet]);
				PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][46], tmpString);
			} 
			else if(PokerTableData[POKER_TABLE_ACTIVE] == 4)
			{
				format(tmpString, sizeof(tmpString), "La mano termina tra ~r~%d~w~...", PokerTableData[co_table_delay]);
				PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][46], tmpString);
			}
			else PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][46], "Texas Holdem Poker");

			switch(PokerTableData[co_table_stage])
			{
				case 0:
				{
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][31], "LD_CARD:cdback");
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][32], "LD_CARD:cdback");
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][33], "LD_CARD:cdback");
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][34], "LD_CARD:cdback");
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][35], "LD_CARD:cdback");
				}
				case 1: 
				{
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][31], PokerCards[PokerTableData[co_table_cards][0] + 0][cardName]);
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][32], PokerCards[PokerTableData[co_table_cards][1] + 0][cardName]);
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][33], PokerCards[PokerTableData[co_table_cards][2] + 0][cardName]);
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][34], "LD_CARD:cdback");
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][35], "LD_CARD:cdback");
				}
				case 2:
				{
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][31], PokerCards[PokerTableData[co_table_cards][0] + 0][cardName]);
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][32], PokerCards[PokerTableData[co_table_cards][1] + 0][cardName]);
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][33], PokerCards[PokerTableData[co_table_cards][2] + 0][cardName]);
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][34], PokerCards[PokerTableData[co_table_cards][3] + 0][cardName]);
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][35], "LD_CARD:cdback");
				}
				case 3, 4:
				{
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][31], PokerCards[PokerTableData[co_table_cards][0] + 0][cardName]);
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][32], PokerCards[PokerTableData[co_table_cards][1] + 0][cardName]);
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][33], PokerCards[PokerTableData[co_table_cards][2] + 0][cardName]);
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][34], PokerCards[PokerTableData[co_table_cards][3] + 0][cardName]);
					PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][35], PokerCards[PokerTableData[co_table_cards][4] + 0][cardName]);
				}
				default: continue;
			}
			
			Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);
		}

		else for(new td = 0; td < 6; td++) if(PokerTableData[POKER_TABLE_SLOTS][td] != -1)
		{
			new pid = PokerTableData[POKER_TABLE_SLOTS][td];

			PlayerTextDrawSetString(pid, PlayerPokerUI[pid][tmp_value[i] + 0], " ");
			PlayerTextDrawSetString(pid, PlayerPokerUI[pid][tmp_value[i] + 1], " ");
			PlayerTextDrawSetString(pid, PlayerPokerUI[pid][tmp_value[i] + 2], " ");
			PlayerTextDrawSetString(pid, PlayerPokerUI[pid][tmp_value[i] + 3], " ");
			PlayerTextDrawSetString(pid, PlayerPokerUI[pid][tmp_value[i] + 4], " ");
		}
		continue;
	}

	Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize); return 1;
}

stock Poker_AssignBlinds(tableid)
{
	Streamer_GetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);
	
	if(PokerTableData[co_table_pos] >= PokerTableData[POKER_TABLE_GUESTS]) PokerTableData[co_table_pos] = 0;

	new playerid;
	new tmpPos = PokerTableData[co_table_pos];

	PokerTableData[co_table_pos]++;
	Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);

	if(PokerTableData[POKER_TABLE_ACTIVE_guests] > 2)
	{
		playerid = Poker_FindGuestsOrder(tableid, tmpPos);

		if(playerid != -1)
		{
			PlayerInfo[playerid][pTableDealer] = 1;
			format(PlayerInfo[playerid][pPokerStatusString], 32, "Dealer");
		} 
		else return Poker_AssignBlinds(tableid);

		tmpPos++;
	}

	if(PokerTableData[POKER_TABLE_ACTIVE_guests] > 1)
	{
		//Small Blind

		if(tmpPos >= PokerTableData[POKER_TABLE_GUESTS]) tmpPos = 0;

		playerid = Poker_FindGuestsOrder(tableid, tmpPos);

		if(playerid != -1) 
		{
			format(PlayerInfo[playerid][pPokerStatusString], 32, "~r~SB -$%d", PokerTableData[co_table_blind] / 2);

			PokerTableData[co_table_pot] += (PokerTableData[co_table_blind] / 2 > PlayerInfo[playerid][pChips]) ? PlayerInfo[playerid][pChips] : PokerTableData[co_table_blind] / 2;
			PlayerInfo[playerid][pChips] = (PokerTableData[co_table_blind] / 2 > PlayerInfo[playerid][pChips]) ? 0 : PlayerInfo[playerid][pChips] - (PokerTableData[co_table_blind] / 2);

			PlayerInfo[playerid][pCurrentBet] = PokerTableData[co_table_blind] / 2;
			PokerTableData[POKER_TABLE_ACTIVE_bet] = PokerTableData[co_table_blind] / 2;

			Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize); 			
		}
		else return Poker_AssignBlinds(tableid);

		//Big Blind

		tmpPos++; if(tmpPos >= PokerTableData[POKER_TABLE_GUESTS]) tmpPos = 0;

		playerid = Poker_FindGuestsOrder(tableid, tmpPos);

		if(playerid != -1) 
		{
			format(PlayerInfo[playerid][pPokerStatusString], 32, "~r~BB -$%d", PokerTableData[co_table_blind]);

			PokerTableData[co_table_pot] += (PlayerInfo[playerid][pChips] < PokerTableData[co_table_blind]) ? PlayerInfo[playerid][pChips] : PokerTableData[co_table_blind];
			PlayerInfo[playerid][pChips] = (PlayerInfo[playerid][pChips] < PokerTableData[co_table_blind]) ? 0 : PlayerInfo[playerid][pChips]  - PokerTableData[co_table_blind];

			PlayerInfo[playerid][pCurrentBet] = PokerTableData[co_table_blind];
			PokerTableData[POKER_TABLE_ACTIVE_bet] = PokerTableData[co_table_blind];

			Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize); 			
		}
		else return Poker_AssignBlinds(tableid);
	}

	return 1;
}

stock Poker_ShuffleDeck(tableid)
{
	Streamer_GetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);

 	for(new i = 0; i < 6; i++) if(PokerTableData[POKER_TABLE_SLOTS][i] != -1) PlayerPlaySound(PokerTableData[POKER_TABLE_SLOTS][i], 5600, 0.0, 0.0, 0.0);
	for(new i = 0; i < 52; i++) PokerTableData[co_table_deck][i] = i;

	new rand;
	new tmp;
	new i;

	for(i = 52; i > 1; i--)
	{
		rand = random(52) % i;
		tmp = PokerTableData[co_table_deck][rand];
		PokerTableData[co_table_deck][rand] = PokerTableData[co_table_deck][i - 1];
		PokerTableData[co_table_deck][i - 1] = tmp;
	}

	Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);

	Poker_DealHands(tableid); return 1;
}

stock Poker_DealHands(tableid)
{
	Streamer_GetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);

	new tmp = 0;

	for(new i = 0; i < 6; i++) if(PokerTableData[POKER_TABLE_SLOTS][i] != -1)
	{
		new playerid = PokerTableData[POKER_TABLE_SLOTS][i];

		if(PlayerInfo[playerid][pPokerStatus] && PlayerInfo[playerid][pChips] > 0)
		{
			PlayerInfo[playerid][pFirstCard] = PokerTableData[co_table_deck][tmp];
			PlayerInfo[playerid][pSecondCard] = PokerTableData[co_table_deck][tmp + 1];

			PlayerInfo[playerid][pActiveHand] = 1;

			PokerTableData[POKER_TABLE_ACTIVE_hands]++;

			PlayerPlaySound(playerid, 5602, 0.0, 0.0, 0.0);
			ApplyAnimation(playerid, "CASINO", "cards_in", 4.1, 0, 1, 1, 1, 1, 1);

			tmp += 2;
		}
		else continue;
	}

	for(new i = 0; i < 5; i++)
	{
		PokerTableData[co_table_cards][i] = PokerTableData[co_table_deck][tmp];
		tmp++;
	}

	Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize); return 1;
}

stock Poker_FindGuestsOrder(tableid, index)
{
	new tmpIndex;

	Streamer_GetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);

	for(new i = 0; i < 6; i++) 
	{
		new playerid = PokerTableData[POKER_TABLE_SLOTS][i];

		if(playerid != -1 && tmpIndex == index) if(PlayerInfo[playerid][pPokerStatus] == 1)return playerid;

		tmpIndex++;
	}

	return -1;
}

stock Poker_RotateGuests(tableid)
{
	new next_active_id = -1;
	new last_ap_id = -1;
	new last_ap_slot = -1;

	Streamer_GetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);

	if(PokerTableData[POKER_TABLE_ACTIVE_guest] != -1)
	{
		last_ap_id = PokerTableData[POKER_TABLE_ACTIVE_guest];

		for(new i = 0; i < 6; i++) if(PokerTableData[POKER_TABLE_SLOTS][i] == last_ap_id) last_ap_slot = i;

		PlayerInfo[last_ap_id][pActiveGuest] = 0;
		PlayerInfo[last_ap_id][pPokerTime] = 0;

		Poker_ShowOptions(last_ap_id, 0);
	}

	if(PokerTableData[POKER_TABLE_ROT] == 0 && last_ap_id == -1 && last_ap_slot == -1) for(new i = 0; i < 6; i++)
	{
		new playerid = PokerTableData[POKER_TABLE_SLOTS][i];

		if(!PlayerInfo[playerid][pTableDealer])continue;
		
		next_active_id = playerid;
		PokerTableData[POKER_TABLE_ACTIVE_guest] = playerid;
		PokerTableData[POKER_TABLE_ACTIVE_guest_slot] = i;
		PokerTableData[POKER_TABLE_ROT]++;
		PokerTableData[POKER_TABLE_SLOTS_rot] = i;
	}
	else if(PokerTableData[POKER_TABLE_ROT] >= 6)
	{
		PokerTableData[POKER_TABLE_ROT] = 0;
		PokerTableData[co_table_stage]++;

		if(PokerTableData[co_table_stage] > 3)
		{
			PokerTableData[POKER_TABLE_ACTIVE] = 4;
			PokerTableData[co_table_delay] = 20 + 1;

			Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize); return 1;
		}

		PokerTableData[POKER_TABLE_SLOTS_rot]++;
		PokerTableData[POKER_TABLE_ROT]++;
		if(PokerTableData[POKER_TABLE_SLOTS_rot] >= 6)  PokerTableData[POKER_TABLE_SLOTS_rot] -= 6;

		Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);

		new playerid = Poker_FindGuestsOrder(tableid, PokerTableData[POKER_TABLE_SLOTS_rot]);

		if(playerid != -1) 
		{
			next_active_id = playerid;
			PokerTableData[POKER_TABLE_ACTIVE_guest] = playerid;
			PokerTableData[POKER_TABLE_ACTIVE_guest_slot] = PokerTableData[POKER_TABLE_SLOTS_rot];
			Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);
		} 
		else Poker_RotateGuests(tableid);
	}
	else
	{
		PokerTableData[POKER_TABLE_SLOTS_rot]++;
		PokerTableData[POKER_TABLE_ROT]++;
		if(PokerTableData[POKER_TABLE_SLOTS_rot] >= 6) PokerTableData[POKER_TABLE_SLOTS_rot] -= 6;

		Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);

		new playerid = Poker_FindGuestsOrder(tableid, PokerTableData[POKER_TABLE_SLOTS_rot]);

		if(playerid != -1) 
		{
			next_active_id = playerid;
			PokerTableData[POKER_TABLE_ACTIVE_guest] = playerid;
			PokerTableData[POKER_TABLE_ACTIVE_guest_slot] = PokerTableData[POKER_TABLE_SLOTS_rot];
			Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);
		} 
		else Poker_RotateGuests(tableid);
	}

	if(next_active_id != -1 && PlayerInfo[next_active_id][pActiveHand]) 
	{
		new currentBet = PlayerInfo[next_active_id][pCurrentBet];
		new activeBet = PokerTableData[POKER_TABLE_ACTIVE_bet];
		
		PlayerPlaySound(next_active_id, 5809, 0.0, 0.0, 0.0);

		if(PlayerInfo[next_active_id][pChips] < 1) Poker_ShowOptions(next_active_id, 3);
		else if(currentBet >= activeBet) Poker_ShowOptions(next_active_id, 1);
		else if(currentBet < activeBet) Poker_ShowOptions(next_active_id, 2);
		else Poker_ShowOptions(next_active_id, 0);

		PlayerInfo[next_active_id][pPokerTime] = 60 + 1;
		PlayerInfo[next_active_id][pActiveGuest] = 1;
	}

	Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize); return 1;
}

stock Poker_ShowMenu(playerid, option)
{
	new tableid = PlayerTableID[playerid];
	
	Streamer_GetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);
			
	switch(option)
	{
		case OPTION_CALL:
		{
		    PlayerInfo[playerid][pActionChoice] = 1;
		    
			if(PlayerInfo[playerid][pChips] <= 0)
			{
				PlayerPlaySound(playerid, 5823, 0.0, 0.0, 0.0);
				return SendClientMessageEx(playerid, COLOR_ERROR, "DEALER: Non hai abbastanza chips.");
			}

			new actualBet = PokerTableData[POKER_TABLE_ACTIVE_bet] - PlayerInfo[playerid][pCurrentBet];

			if(actualBet > PlayerInfo[playerid][pChips])return
				Dialog_Show(playerid, DialogPCall, DIALOG_STYLE_MSGBOX, "Texas Holdem Poker - (Call)", "Sei sicuro di voler chiamare $%d e cioè All-In?", "All-In", "Indietro", actualBet);

			Dialog_Show(playerid, DialogPCall, DIALOG_STYLE_MSGBOX, "Texas Holdem Poker - (Call)", "Sei sicuro di voler chiamate $%d?", "Call", "Indietro", actualBet);
		}
		case OPTION_RAISE:
		{
			PlayerInfo[playerid][pActionChoice] = 1;

			if(PlayerInfo[playerid][pCurrentBet] + PlayerInfo[playerid][pChips] > PokerTableData[POKER_TABLE_ACTIVE_bet] + PokerTableData[co_table_blind] / 2)return
				Dialog_Show(playerid, DialogPRaise, DIALOG_STYLE_INPUT, "Texas Holdem Poker - (Raise)", "Inserisci la cifra per il Raise ($%d a $%d):", "Raise", "Indietro", PokerTableData[POKER_TABLE_ACTIVE_bet] + PokerTableData[co_table_blind] / 2, PlayerInfo[playerid][pCurrentBet] + PlayerInfo[playerid][pChips]);

			else if(PlayerInfo[playerid][pCurrentBet] + PlayerInfo[playerid][pChips] == PokerTableData[POKER_TABLE_ACTIVE_bet] + PokerTableData[co_table_blind] / 2)return
				Dialog_Show(playerid, DialogPRaise, DIALOG_STYLE_INPUT, "Texas Holdem Poker - (Raise)", "Inserisci la cifra per il Raise (All-In):", "All-In", "Indietro");

			SendClientMessageEx(playerid, COLOR_ERROR, "DEALER: Non hai abbastanza chips.");
			PlayerPlaySound(playerid, 5823, 0.0, 0.0, 0.0);
		}
		case OPTION_BUY_IN: Dialog_Show(playerid, DialogPBuyIn, DIALOG_STYLE_INPUT, "Texas Holdem Poker - (BuyIn)", "Inserire il buy-in per entrare nel tavolo:\n\nSoldi a disposizione: $%d\nPoker Chips: $%d\nBuy-In da $%d a $%d", "Buy In", "Esci", PlayerInfo[playerid][pCash], PlayerInfo[playerid][pChips], PokerTableData[co_table_buy_in_min], PokerTableData[co_table_buy_in_max]);
		case OPTION_SETUP_GAME:
		{
			Dialog_Show(playerid, DialogPSetupGame, DIALOG_STYLE_LIST, "Texas Holdem Poker - (Setup Poker Room)", "Buy-In Max\t($%d)\nBuy-In Min\t($%d)\nBlind\t\t($%d / $%d)\nLimite\t\t(%d)\nPassword\t(%s)\nRound Delay\t(%d)\nSeat Price\t($%d)\n{FF6347}>> Avvia la partita", "Seleziona", "Esci",
			PokerTableData[co_table_buy_in_max],
			PokerTableData[co_table_buy_in_min],
			PokerTableData[co_table_blind],
			PokerTableData[co_table_blind] / 2,
			PokerTableData[POKER_TABLE_GUESTS_limit],
			PokerTableData[co_table_key],
			PokerTableData[co_table_setdelay],
			PokerTableData[co_table_seat_price]);
		}
		case OPTION_BUY_IN_MAX: Dialog_Show(playerid, DialogPBuyInMax, DIALOG_STYLE_INPUT, "Texas Holdem Poker - (Buy-In Max)", "Inserire il Buy-In Max:", "Modifica", "Indietro");
		case OPTION_BUY_IN_MIN: Dialog_Show(playerid, DialogPBuyInMin, DIALOG_STYLE_INPUT, "Texas Holdem Poker - (Buy-In Min)", "Inserire il Buy-In Min:", "Modifica", "Indietro");
		case OPTION_BLINDS: Dialog_Show(playerid, DialogPBlinds, DIALOG_STYLE_INPUT, "Texas Holdem Poker - (Blinds)", "Inserire i Blinds:\n\nNota: Lo Small blind viene calcolato automaticamente (Big Blind / 2).", "Modifica", "Indietro");
		case OPTION_PLAYERS_LIMIT: Dialog_Show(playerid, DialogPPlayersLimit, DIALOG_STYLE_INPUT, "Texas Holdem Poker - (Limite)", "Inserire il Limite di giocatori (2 - 6):", "Modifica", "Indietro");
		case OPTION_SET_KEY: Dialog_Show(playerid, DialogPKey, DIALOG_STYLE_INPUT, "Texas Holdem Poker - (Password)", "Inserire la Password:\n\nNota: Lascia il campo vuoto se vuoi rendere pubblico il tavolo.", "Modifica", "Indietro");
		case OPTION_ROUND_DELAY: Dialog_Show(playerid, DialogPDelay, DIALOG_STYLE_INPUT, "Texas Holdem Poker - (Round Delay)", "Inserire il Round Delay (15 - 120 secondi):", "Modifica", "Indietro");
		case OPTION_SEAT_PRICE: Dialog_Show(playerid, DialogPSeatPrice, DIALOG_STYLE_INPUT, "Texas Holdem Poker - (Seat Price)", "Inserisci il costo per entrare all'interno della partita ($0 - $200):", "Modifica", "Indietro");
	}
	
	return 1;
}

stock Poker_Call(playerid)return Poker_ShowMenu(playerid, OPTION_CALL);

stock Poker_Raise(playerid)return Poker_ShowMenu(playerid, OPTION_RAISE);

stock Poker_Check(playerid)
{
	if(PlayerInfo[playerid][pActiveHand])
		format(PlayerInfo[playerid][pPokerStatusString], 16, "Check");

	return ApplyAnimation(playerid, "CASINO", "cards_raise", 4.1, 0, 1, 1, 1, 1, 1);
}

stock Poker_Fold(playerid)
{
	if(PlayerInfo[playerid][pActiveHand] < 1) return 1;

	PlayerInfo[playerid][pFirstCard] = PlayerInfo[playerid][pSecondCard] = PlayerInfo[playerid][pActiveHand] = PlayerInfo[playerid][pPokerStatus] = 0;

	new tableid = PlayerTableID[playerid];

	Streamer_GetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);
	PokerTableData[POKER_TABLE_ACTIVE_hands]--;

	format(PlayerInfo[playerid][pPokerStatusString], 16, "Fold");

	for(new i = 0; i < 6; i++) if(PokerTableData[POKER_TABLE_SLOTS][i] != -1) PlayerPlaySound(PokerTableData[POKER_TABLE_SLOTS][i], 5602, 0.0, 0.0, 0.0);

	ApplyAnimation(playerid, "CASINO", "cards_out", 4.1, 0, 1, 1, 1, 1, 1);

	Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize); return 1;
}

stock Poker_InitTable(tableid)
{
	Reset_ResetTable(tableid);
	
	GetDynamicObjectPos(tableid, PokerTableData[co_table_x], PokerTableData[co_table_y], PokerTableData[co_table_z]);
	GetDynamicObjectRot(tableid, PokerTableData[co_table_rx], PokerTableData[co_table_ry], PokerTableData[co_table_rz]);
	
	PokerTableData[co_table_vw] = Streamer_GetIntData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_WORLD_ID);
	PokerTableData[co_table_int] = Streamer_GetIntData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_INTERIOR_ID);
    PokerTableData[co_table_placed] = 1;
    
	Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize); return 1;
}

stock Reset_ResetTable(tableid)
{
	for(new s = 0; s < 6; s++)
		PokerTableData[POKER_TABLE_SLOTS][s] = -1;

	PokerTableData[co_table_key] = EOS;
	PokerTableData[POKER_TABLE_ACTIVE] = 0;
	PokerTableData[POKER_TABLE_GUESTS_limit] = 6;
	PokerTableData[co_table_buy_in_max] = 1000;
	PokerTableData[co_table_buy_in_min] = 500;
	PokerTableData[co_table_seat_price] = 100;
	PokerTableData[co_table_blind] = 100;
	PokerTableData[co_table_pos] = 0;
	PokerTableData[co_table_round] = 0;
	PokerTableData[co_table_stage] = 0;
	PokerTableData[POKER_TABLE_ACTIVE_bet] = 0;
	PokerTableData[co_table_delay] = 0;
	PokerTableData[co_table_pot] = 0;
	PokerTableData[co_table_setdelay] = 15;
	PokerTableData[POKER_TABLE_ROT] = 0;
	PokerTableData[POKER_TABLE_SLOTS_rot] = 0;
	PokerTableData[co_table_winner_id] = -1;
	PokerTableData[co_table_winners] = 0;
	PokerTableData[POKER_TABLE_GUESTS] = 0;
	PokerTableData[POKER_TABLE_ACTIVE_guests] = 0;
	KillTimer(PokerTableData[POKER_TABLE_PULSE_TIMER]);

	Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize); return 1;
}

stock Poker_ResetTableRound(tableid)
{
	Streamer_GetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);

	PokerTableData[co_table_round] = 0;
	PokerTableData[co_table_stage] = 0;
	PokerTableData[POKER_TABLE_ACTIVE_bet] = 0;
	PokerTableData[POKER_TABLE_ACTIVE] = 2;
	PokerTableData[co_table_delay] = PokerTableData[co_table_setdelay];
	PokerTableData[co_table_pot] = 0;
	PokerTableData[POKER_TABLE_ROT] = 0;
	PokerTableData[POKER_TABLE_SLOTS_rot] = 0;
	PokerTableData[co_table_winner_id] = -1;
	PokerTableData[co_table_winners] = 0;
	PokerTableData[POKER_TABLE_ACTIVE_hands] = 0;

	Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);

	for(new i = 0; i < 6; i++)
	{
		new playerid = PokerTableData[POKER_TABLE_SLOTS][i];

		if(playerid != -1)
		{
			PlayerInfo[playerid][pTableWinner] = 0;
			PlayerInfo[playerid][pTableDealer] = 0;
			PlayerInfo[playerid][pFirstCard] = 0;
			PlayerInfo[playerid][pSecondCard] = 0;
			PlayerInfo[playerid][pActiveGuest] = 0;
			PlayerInfo[playerid][pPokerTime] = 0;
			PlayerInfo[playerid][pActiveHand] = 0;
			PlayerInfo[playerid][pCurrentBet] = 0;
			PlayerInfo[playerid][pPokerResultString] = '\0';
			PlayerInfo[playerid][pPokerHide] = 0;

			ApplyAnimation(playerid, "CASINO", "cards_out", 4.1, 0, 1, 1, 1, 1, 1);
		}
		else continue;
	}

	return 1;
}

stock Poker_CreateTD(playerid)
{
    PlayerPokerUI[playerid][0] = CreatePlayerTextDraw(playerid, 390.000000, 250.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][0], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][0], 100);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][0], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][0], 0.159998, 1.200001);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][0], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][0], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][0], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][0], 0);

	PlayerPokerUI[playerid][1] = CreatePlayerTextDraw(playerid, 390.000000, 260.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][1], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][1], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][1], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][1], 0.159998, 1.200001);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][1], 16711935);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][1], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][1], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][1], 0);

	PlayerPokerUI[playerid][2] = CreatePlayerTextDraw(playerid, 369.000000, 273.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][2], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][2], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][2], 4);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][2], 0.500000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][2], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][2], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][2], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][2], 1);
	PlayerTextDrawUseBox(playerid, PlayerPokerUI[playerid][2], 1);
	PlayerTextDrawBoxColor(playerid, PlayerPokerUI[playerid][2], 255);
	PlayerTextDrawTextSize(playerid, PlayerPokerUI[playerid][2], 20.000000, 33.000000);

	PlayerPokerUI[playerid][3] = CreatePlayerTextDraw(playerid, 392.000000, 273.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][3], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][3], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][3], 4);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][3], 0.500000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][3], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][3], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][3], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][3], 1);
	PlayerTextDrawUseBox(playerid, PlayerPokerUI[playerid][3], 1);
	PlayerTextDrawBoxColor(playerid, PlayerPokerUI[playerid][3], 255);
	PlayerTextDrawTextSize(playerid, PlayerPokerUI[playerid][3], 20.000000, 33.000000);

	PlayerPokerUI[playerid][4] = CreatePlayerTextDraw(playerid, 391.000000, 305.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][4], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][4], 100);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][4], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][4], 0.180000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][4], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][4], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][4], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][4], 0);

	PlayerPokerUI[playerid][5] = CreatePlayerTextDraw(playerid, 250.000000, 250.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][5], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][5], 100);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][5], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][5], 0.159999, 1.200001);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][5], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][5], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][5], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][5], 0);

	PlayerPokerUI[playerid][6] = CreatePlayerTextDraw(playerid, 250.000000, 260.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][6], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][6], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][6], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][6], 0.159999, 1.200001);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][6], 16711935);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][6], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][6], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][6], 0);

	PlayerPokerUI[playerid][7] = CreatePlayerTextDraw(playerid, 229.000000, 273.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][7], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][7], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][7], 4);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][7], 0.500000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][7], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][7], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][7], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][7], 1);
	PlayerTextDrawUseBox(playerid, PlayerPokerUI[playerid][7], 1);
	PlayerTextDrawBoxColor(playerid, PlayerPokerUI[playerid][7], 255);
	PlayerTextDrawTextSize(playerid, PlayerPokerUI[playerid][7], 20.000000, 33.000000);

	PlayerPokerUI[playerid][8] = CreatePlayerTextDraw(playerid, 252.000000, 273.000000, " ");
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][8], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][8], 4);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][8], 0.500000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][8], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][8], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][8], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][8], 1);
	PlayerTextDrawUseBox(playerid, PlayerPokerUI[playerid][8], 1);
	PlayerTextDrawBoxColor(playerid, PlayerPokerUI[playerid][8], 255);
	PlayerTextDrawTextSize(playerid, PlayerPokerUI[playerid][8], 20.000000, 33.000000);

	PlayerPokerUI[playerid][9] = CreatePlayerTextDraw(playerid, 250.000000, 305.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][9], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][9], 100);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][9], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][9], 0.180000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][9], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][9], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][9], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][9], 0);

	PlayerPokerUI[playerid][10] = CreatePlayerTextDraw(playerid, 199.000000, 190.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][10], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][10], 100);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][10], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][10], 0.159998, 1.200001);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][10], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][10], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][10], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][10], 0);

	PlayerPokerUI[playerid][11] = CreatePlayerTextDraw(playerid, 199.000000, 199.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][11], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][11], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][11], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][11], 0.159998, 1.200001);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][11], 16711935);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][11], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][11], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][11], 0);

	PlayerPokerUI[playerid][12] = CreatePlayerTextDraw(playerid, 179.000000, 212.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][12], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][12], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][12], 4);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][12], 0.500000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][12], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][12], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][12], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][12], 1);
	PlayerTextDrawUseBox(playerid, PlayerPokerUI[playerid][12], 1);
	PlayerTextDrawBoxColor(playerid, PlayerPokerUI[playerid][12], 255);
	PlayerTextDrawTextSize(playerid, PlayerPokerUI[playerid][12], 20.000000, 33.000000);

	PlayerPokerUI[playerid][13] = CreatePlayerTextDraw(playerid, 202.000000, 212.000000, " ");
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][13], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][13], 4);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][13], 0.500000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][13], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][13], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][13], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][13], 1);
	PlayerTextDrawUseBox(playerid, PlayerPokerUI[playerid][13], 1);
	PlayerTextDrawBoxColor(playerid, PlayerPokerUI[playerid][13], 255);
	PlayerTextDrawTextSize(playerid, PlayerPokerUI[playerid][13], 20.000000, 33.000000);

	PlayerPokerUI[playerid][14] = CreatePlayerTextDraw(playerid, 199.000000, 245.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][14], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][14], 100);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][14], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][14], 0.180000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][14], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][14], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][14], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][14], 0);

	PlayerPokerUI[playerid][15] = CreatePlayerTextDraw(playerid, 250.000000, 130.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][15], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][15], 100);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][15], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][15], 0.159998, 1.200001);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][15], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][15], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][15], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][15], 0);

	PlayerPokerUI[playerid][16] = CreatePlayerTextDraw(playerid, 250.000000, 140.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][16], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][16], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][16], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][16], 0.159998, 1.200001);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][16], 16711935);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][16], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][16], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][16], 0);

	PlayerPokerUI[playerid][17] = CreatePlayerTextDraw(playerid, 229.000000, 152.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][17], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][17], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][17], 4);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][17], 0.500000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][17], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][17], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][17], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][17], 1);
	PlayerTextDrawUseBox(playerid, PlayerPokerUI[playerid][17], 1);
	PlayerTextDrawBoxColor(playerid, PlayerPokerUI[playerid][17], 255);
	PlayerTextDrawTextSize(playerid, PlayerPokerUI[playerid][17], 20.000000, 33.000000);

	PlayerPokerUI[playerid][18] = CreatePlayerTextDraw(playerid, 252.000000, 152.000000, " ");
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][18], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][18], 4);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][18], 0.500000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][18], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][18], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][18], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][18], 1);
	PlayerTextDrawUseBox(playerid, PlayerPokerUI[playerid][18], 1);
	PlayerTextDrawBoxColor(playerid, PlayerPokerUI[playerid][18], 255);
	PlayerTextDrawTextSize(playerid, PlayerPokerUI[playerid][18], 20.000000, 33.000000);

	PlayerPokerUI[playerid][19] = CreatePlayerTextDraw(playerid, 250.000000, 190.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][19], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][19], 100);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][19], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][19], 0.180000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][19], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][19], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][19], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][19], 0);

	PlayerPokerUI[playerid][20] = CreatePlayerTextDraw(playerid, 390.000000, 130.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][20], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][20], 100);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][20], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][20], 0.159997, 1.200001);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][20], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][20], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][20], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][20], 0);

	PlayerPokerUI[playerid][21] = CreatePlayerTextDraw(playerid, 390.000000, 140.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][21], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][21], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][21], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][21], 0.159997, 1.200001);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][21], 16711935);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][21], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][21], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][21], 0);

	PlayerPokerUI[playerid][22] = CreatePlayerTextDraw(playerid, 369.000000, 152.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][22], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][22], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][22], 4);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][22], 0.500000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][22], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][22], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][22], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][22], 1);
	PlayerTextDrawUseBox(playerid, PlayerPokerUI[playerid][22], 1);
	PlayerTextDrawBoxColor(playerid, PlayerPokerUI[playerid][22], 255);
	PlayerTextDrawTextSize(playerid, PlayerPokerUI[playerid][22], 20.000000, 33.000000);

	PlayerPokerUI[playerid][23] = CreatePlayerTextDraw(playerid, 392.000000, 152.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][23], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][23], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][23], 4);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][23], 0.500000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][23], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][23], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][23], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][23], 1);
	PlayerTextDrawUseBox(playerid, PlayerPokerUI[playerid][23], 1);
	PlayerTextDrawBoxColor(playerid, PlayerPokerUI[playerid][23], 255);
	PlayerTextDrawTextSize(playerid, PlayerPokerUI[playerid][23], 20.000000, 33.000000);

	PlayerPokerUI[playerid][24] = CreatePlayerTextDraw(playerid, 391.000000, 190.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][24], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][24], 100);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][24], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][24], 0.180000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][24], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][24], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][24], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][24], 0);

	PlayerPokerUI[playerid][25] = CreatePlayerTextDraw(playerid, 443.000000, 190.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][25], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][25], 100);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][25], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][25], 0.159998, 1.200001);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][25], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][25], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][25], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][25], 0);

	PlayerPokerUI[playerid][26] = CreatePlayerTextDraw(playerid, 443.000000, 199.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][26], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][26], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][26], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][26], 0.159998, 1.200001);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][26], 16711935);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][26], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][26], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][26], 0);

	PlayerPokerUI[playerid][27] = CreatePlayerTextDraw(playerid, 422.000000, 212.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][27], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][27], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][27], 4);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][27], 0.500000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][27], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][27], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][27], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][27], 1);
	PlayerTextDrawUseBox(playerid, PlayerPokerUI[playerid][27], 1);
	PlayerTextDrawBoxColor(playerid, PlayerPokerUI[playerid][27], 255);
	PlayerTextDrawTextSize(playerid, PlayerPokerUI[playerid][27], 20.000000, 33.000000);

	PlayerPokerUI[playerid][28] = CreatePlayerTextDraw(playerid, 445.000000, 212.000000, " ");
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][28], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][28], 4);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][28], 0.500000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][28], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][28], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][28], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][28], 1);
	PlayerTextDrawUseBox(playerid, PlayerPokerUI[playerid][28], 1);
	PlayerTextDrawBoxColor(playerid, PlayerPokerUI[playerid][28], 255);
	PlayerTextDrawTextSize(playerid, PlayerPokerUI[playerid][28], 20.000000, 33.000000);

	PlayerPokerUI[playerid][29] = CreatePlayerTextDraw(playerid, 443.000000, 245.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][29], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][29], 100);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][29], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][29], 0.180000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][29], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][29], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][29], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][29], 0);

	PlayerPokerUI[playerid][31] = CreatePlayerTextDraw(playerid, 266.000000, 198.000000, "LD_CARD:cdback");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][31], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][31], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][31], 4);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][31], 0.500000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][31], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][31], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][31], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][31], 1);
	PlayerTextDrawUseBox(playerid, PlayerPokerUI[playerid][31], 1);
	PlayerTextDrawBoxColor(playerid, PlayerPokerUI[playerid][31], 255);
	PlayerTextDrawTextSize(playerid, PlayerPokerUI[playerid][31], 20.000000, 33.000000);

	PlayerPokerUI[playerid][32] = CreatePlayerTextDraw(playerid, 288.000000, 198.000000, "LD_CARD:cdback");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][32], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][32], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][32], 4);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][32], 0.500000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][32], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][32], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][32], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][32], 1);
	PlayerTextDrawUseBox(playerid, PlayerPokerUI[playerid][32], 1);
	PlayerTextDrawBoxColor(playerid, PlayerPokerUI[playerid][32], 255);
	PlayerTextDrawTextSize(playerid, PlayerPokerUI[playerid][32], 20.000000, 33.000000);

	PlayerPokerUI[playerid][33] = CreatePlayerTextDraw(playerid, 310.000000, 198.000000, "LD_CARD:cdback");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][33], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][33], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][33], 4);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][33], 0.500000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][33], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][33], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][33], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][33], 1);
	PlayerTextDrawUseBox(playerid, PlayerPokerUI[playerid][33], 1);
	PlayerTextDrawBoxColor(playerid, PlayerPokerUI[playerid][33], 255);
	PlayerTextDrawTextSize(playerid, PlayerPokerUI[playerid][33], 20.000000, 33.000000);

	PlayerPokerUI[playerid][34] = CreatePlayerTextDraw(playerid, 332.000000, 198.000000, "LD_CARD:cdback");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][34], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][34], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][34], 4);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][34], 0.500000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][34], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][34], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][34], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][34], 1);
	PlayerTextDrawUseBox(playerid, PlayerPokerUI[playerid][34], 1);
	PlayerTextDrawBoxColor(playerid, PlayerPokerUI[playerid][34], 255);
	PlayerTextDrawTextSize(playerid, PlayerPokerUI[playerid][34], 20.000000, 33.000000);

	PlayerPokerUI[playerid][35] = CreatePlayerTextDraw(playerid, 354.000000, 198.000000, "LD_CARD:cdback");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][35], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][35], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][35], 4);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][35], 0.500000, 1.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][35], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][35], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][35], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][35], 1);
	PlayerTextDrawUseBox(playerid, PlayerPokerUI[playerid][35], 1);
	PlayerTextDrawBoxColor(playerid, PlayerPokerUI[playerid][35], 255);
	PlayerTextDrawTextSize(playerid, PlayerPokerUI[playerid][35], 20.000000, 33.000000);

	PlayerPokerUI[playerid][37] = CreatePlayerTextDraw(playerid, 318.000000, 180.000000, "Texas Holdem Poker");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][37], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][37], -1);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][37], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][37], 0.199999, 1.200000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][37], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][37], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][37], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][37], 0);

	PlayerPokerUI[playerid][38] = CreatePlayerTextDraw(playerid, 321.000000, 253.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][38], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][38], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][38], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][38], 0.189999, 1.200000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][38], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][38], 1);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][38], 1);
	PlayerTextDrawUseBox(playerid, PlayerPokerUI[playerid][38], 1);
	PlayerTextDrawBoxColor(playerid, PlayerPokerUI[playerid][38], 45);
	PlayerTextDrawTextSize(playerid, PlayerPokerUI[playerid][38], 10.000000, 26.000000);
	PlayerTextDrawSetSelectable(playerid, PlayerPokerUI[playerid][38], 1);

	PlayerPokerUI[playerid][39] = CreatePlayerTextDraw(playerid, 321.000000, 270.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][39], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][39], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][39], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][39], 0.189999, 1.200000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][39], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][39], 1);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][39], 1);
	PlayerTextDrawUseBox(playerid, PlayerPokerUI[playerid][39], 1);
	PlayerTextDrawBoxColor(playerid, PlayerPokerUI[playerid][39], 45);
	PlayerTextDrawTextSize(playerid, PlayerPokerUI[playerid][39], 10.000000, 26.000000);
	PlayerTextDrawSetSelectable(playerid, PlayerPokerUI[playerid][39], 1);

	PlayerPokerUI[playerid][40] = CreatePlayerTextDraw(playerid, 321.000000, 286.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][40], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][40], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][40], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][40], 0.189999, 1.200000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][40], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][40], 1);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][40], 1);
	PlayerTextDrawUseBox(playerid, PlayerPokerUI[playerid][40], 1);
	PlayerTextDrawBoxColor(playerid, PlayerPokerUI[playerid][40], 45);
	PlayerTextDrawTextSize(playerid, PlayerPokerUI[playerid][40], 10.000000, 26.000000);
	PlayerTextDrawSetSelectable(playerid, PlayerPokerUI[playerid][40], 1);

	PlayerPokerUI[playerid][41] = CreatePlayerTextDraw(playerid, 318.000000, 148.000000, "ESCI");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][41], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][41], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][41], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][41], 0.189999, 1.200000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][41], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][41], 1);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][41], 1);
	PlayerTextDrawUseBox(playerid, PlayerPokerUI[playerid][41], 1);
	PlayerTextDrawBoxColor(playerid, PlayerPokerUI[playerid][41], 45);
	PlayerTextDrawTextSize(playerid, PlayerPokerUI[playerid][41], 10.000000, 36.000000);
	PlayerTextDrawSetSelectable(playerid, PlayerPokerUI[playerid][41], 1);

	PlayerPokerUI[playerid][42] = CreatePlayerTextDraw(playerid, 590.000000, 400.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][42], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][42], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][42], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][42], 0.500000, 2.000000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][42], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][42], 1);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][42], 1);

	PlayerPokerUI[playerid][43] = CreatePlayerTextDraw(playerid, 589.000000, 396.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][43], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][43], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][43], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][43], 0.180000, 0.800000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][43], 200);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][43], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][43], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][43], 0);

	PlayerPokerUI[playerid][44] = CreatePlayerTextDraw(playerid, 588.000000, 437.000000, " ");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][44], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][44], 255);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][44], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][44], 0.180000, 0.800000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][44], 200);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][44], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][44], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][44], 0);

	PlayerPokerUI[playerid][46] = CreatePlayerTextDraw(playerid, 318.000000, 235.000000, "In attesa di altri giocatori...");
	PlayerTextDrawAlignment(playerid, PlayerPokerUI[playerid][46], 2);
	PlayerTextDrawBackgroundColor(playerid, PlayerPokerUI[playerid][46], -1);
	PlayerTextDrawFont(playerid, PlayerPokerUI[playerid][46], 2);
	PlayerTextDrawLetterSize(playerid, PlayerPokerUI[playerid][46], 0.199999, 1.200000);
	PlayerTextDrawColor(playerid, PlayerPokerUI[playerid][46], -1);
	PlayerTextDrawSetOutline(playerid, PlayerPokerUI[playerid][46], 0);
	PlayerTextDrawSetProportional(playerid, PlayerPokerUI[playerid][46], 1);
	PlayerTextDrawSetShadow(playerid, PlayerPokerUI[playerid][46], 0); return 1;
}

stock Poker_DestroyTD(playerid)
{
	for(new i = 0; i < MAX_PLAYER_POKER_UI; i++) if(i != 30 && i != 36 && i != 45)
		PlayerTextDrawDestroy(playerid, PlayerPokerUI[playerid][i]);
	
	return 1;
}

stock Poker_ShowTD(playerid)
{
	for(new i = 0; i < MAX_PLAYER_POKER_UI; i++) if(i != 30 && i != 36 && i != 45)
		PlayerTextDrawShow(playerid, PlayerPokerUI[playerid][i]);
	
	return 1;
}

stock Poker_ShowOptions(playerid, option)
{
	switch(option)
	{
		case 0:
		{
			PlayerActionOptions[playerid] = 0;
			PlayerTextDrawHide(playerid, PlayerPokerUI[playerid][38]);
			PlayerTextDrawHide(playerid, PlayerPokerUI[playerid][39]);
			PlayerTextDrawHide(playerid, PlayerPokerUI[playerid][40]);
		}
		case 1: 
		{
			PlayerActionOptions[playerid] = 1;
			PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][38], "RAISE");
			PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][39], "CHECK");
			PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][40], "FOLD");

			PlayerTextDrawShow(playerid, PlayerPokerUI[playerid][38]);
			PlayerTextDrawShow(playerid, PlayerPokerUI[playerid][39]);
			PlayerTextDrawShow(playerid, PlayerPokerUI[playerid][40]);
		}
		case 2: 
		{
			PlayerActionOptions[playerid] = 2;
			PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][38], "CALL");
			PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][39], "RAISE");
			PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][40], "FOLD");

			PlayerTextDrawShow(playerid, PlayerPokerUI[playerid][38]);
			PlayerTextDrawShow(playerid, PlayerPokerUI[playerid][39]);
			PlayerTextDrawShow(playerid, PlayerPokerUI[playerid][40]);
		}
		case 3: 
		{
			PlayerActionOptions[playerid] = 3;

			PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][38], "CHECK");
			PlayerTextDrawSetString(playerid, PlayerPokerUI[playerid][39], "FOLD");

			PlayerTextDrawShow(playerid, PlayerPokerUI[playerid][38]);
			PlayerTextDrawShow(playerid, PlayerPokerUI[playerid][39]);
		}
		default: return 1;
	}
	
	return 1;
}

stock Poker_JoinTable(playerid, tableid)
{
    new buildingid = GetPlayerBuilding(playerid, true);
    
	Streamer_GetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);

	if(PokerTableData[POKER_TABLE_GUESTS] >= PokerTableData[POKER_TABLE_GUESTS_limit])return
		SendClientMessageEx(playerid, COLOR_ERROR, "Il limite di giocatori a questo tavolo è stato raggiunto.");

	if(PokerTableData[POKER_TABLE_ACTIVE] == 1) return
		SendClientMessageEx(playerid, COLOR_ERROR, "Qualcuno sta modificando le impostazioni del tavolo, riprova più tardi.");

	if(PlayerInfo[playerid][pCash] < PokerTableData[co_table_seat_price])return
	    SendFormattedMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi per entrare nel tavolo. ($%d)", PokerTableData[co_table_seat_price]);

	for(new s; s < 6; s++)  if(PokerTableData[POKER_TABLE_SLOTS][s] == -1)
	{
		AC_GivePlayerMoney(playerid, -PokerTableData[co_table_seat_price], "/poker entra");

	    Poker_CreateTD(playerid);
		Poker_ShowTD(playerid);
		Poker_ShowOptions(playerid, 0);

		PlayerTableID[playerid] = tableid;
		PlayerInfo[playerid][pTableSlot] = s;

		PokerTableData[POKER_TABLE_GUESTS]++;
		PokerTableData[POKER_TABLE_SLOTS][s] = playerid;

		Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);

		if(PokerTableData[POKER_TABLE_GUESTS] == 1)
		{
			PokerTableData[POKER_TABLE_ACTIVE] = 1;
			Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);

			Poker_ShowMenu(playerid, OPTION_SETUP_GAME);
		}
		else Poker_ShowMenu(playerid, OPTION_BUY_IN);

		CameraRadiusSetPos(playerid, PokerTableData[co_table_x], PokerTableData[co_table_y], PokerTableData[co_table_z], 90.0, 4.7, 0.1);
		GetPlayerPos(playerid, PlayerInfo[playerid][pTableX], PlayerInfo[playerid][pTableY], PlayerInfo[playerid][pTableZ]);
        SetPlayerInterior(playerid, PokerTableData[co_table_int]);
		AC_SetPlayerVirtualWorld(playerid, PokerTableData[co_table_vw]);

		ApplyAnimation(playerid, "CASINO", "cards_out", 4.1, 0, 1, 1, 1, 1, 1);
		Character_SetFreezed(playerid, true);
		SetPlayerPosObjectOffset(tableid, playerid, PokerTableMiscObjOffsets[s][0], PokerTableMiscObjOffsets[s][1], PokerTableMiscObjOffsets[s][2]);
		SetPlayerFacingAngle(playerid, PokerTableMiscObjOffsets[s][5] + 90.0);

		if(buildingid != -1) BuildingInfo[buildingid][bCash] += PokerTableData[co_table_seat_price]; break;
	}

	return 1;
}

stock Poker_LeaveTable(playerid)
{
	new tableid = PlayerTableID[playerid];
    
	if(!tableid)return 1;

	PlayerPlaySound(playerid, 5852, 0.0, 0.0, 0.0);
	AC_GivePlayerMoney(playerid, PlayerInfo[playerid][pChips], "/poker esci");

	Streamer_GetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);

	PokerTableData[POKER_TABLE_GUESTS]--;
	if(PlayerInfo[playerid][pPokerStatus]) PokerTableData[POKER_TABLE_ACTIVE_guests]--;

	new pk_slot = PlayerInfo[playerid][pTableSlot]; PokerTableData[POKER_TABLE_SLOTS][pk_slot] = -1;

	Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);

	if(!PokerTableData[co_table_round] && PokerTableData[co_table_delay] < 5)
		Poker_ResetTableRound(tableid);

	SetPlayerInterior(playerid, PokerTableData[co_table_int]);
	AC_SetPlayerVirtualWorld(playerid, PokerTableData[co_table_vw]);
	SetPlayerPos(playerid, PlayerInfo[playerid][pTableX], PlayerInfo[playerid][pTableY], PlayerInfo[playerid][pTableZ]+0.1);
	SetCameraBehindPlayer(playerid);
	Character_SetFreezed(playerid, false);
	Dialog_Close(playerid);

	if(PlayerInfo[playerid][pActiveHand])
		PokerTableData[POKER_TABLE_ACTIVE_hands]--;

	Streamer_SetArrayData(STREAMER_TYPE_OBJECT, tableid, E_STREAMER_EXTRA_ID, PokerTableData, PokerTableDataSize);

	if(PlayerInfo[playerid][pActiveGuest])
		Poker_RotateGuests(tableid);

	PlayerTableID[playerid] = 0;
	PlayerInfo[playerid][pTableSlot] = 0;
	PlayerInfo[playerid][pTableWinner] = 0;
	PlayerInfo[playerid][pCurrentBet] = 0;
	PlayerInfo[playerid][pChips] = 0;
	PlayerInfo[playerid][pTableX] = 0;
	PlayerInfo[playerid][pTableY] = 0;
	PlayerInfo[playerid][pTableZ] = 0;
	PlayerInfo[playerid][pPokerStatus] = 0;
	PlayerInfo[playerid][pTableDealer] = 0;
	PlayerInfo[playerid][pFirstCard] = 0;
	PlayerInfo[playerid][pSecondCard] = 0;
	PlayerInfo[playerid][pActiveGuest] = 0;
	PlayerInfo[playerid][pActiveHand] = 0;
	PlayerInfo[playerid][pPokerHide] = 0;

	Poker_DestroyTD(playerid);
	SetCameraBehindPlayer(playerid);
	Character_SetFreezed(playerid, false);
	ClearAnimations(playerid);
	CancelSelectTextDraw(playerid);

	if(!PokerTableData[POKER_TABLE_GUESTS])
		Reset_ResetTable(tableid);

	return 1;
}

hook function bool:Character_CanCasino(playerid)
{
	if(PlayerTableID[playerid] > 0)
		return false;
    return continue(playerid);
}