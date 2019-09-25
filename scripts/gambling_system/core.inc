
#include "blackjack"

#include <YSI_Coding\y_hooks>

new Float:PokerTableMiscObjOffsets[6][6] =
{
	{-1.25, -0.470, 0.1, 0.0, 0.0, 180.0}, // (Slot 2)
	{-1.25, 0.470, 0.1, 0.0, 0.0, 180.0}, // (Slot 1)
	{0.01, 1.85, 0.1, 0.0, 0.0, 90.0},  // (Slot 6)
	{1.25, 0.470, 0.1, 0.0, 0.0, 0.0}, // (Slot 5)
	{1.25, -0.470, 0.1, 0.0, 0.0, 0.0}, // (Slot 4)
	{-0.01, -1.85, 0.1, 0.0, 0.0, -90.0} // (Slot 3)
};

enum
{
	Two = 1,
	Three,
	Four,
	Five,
	Six,
	Seven,
	Eight,
	Nine,
	Ten,
	Jack,
	Queen,
	King,
	Ace
}

enum
{
	HighCard = 1,
	Pair,
	TwoPairs,
	ThreeOfKind,
	Straight,
	Flush,
	FullHouse,
	Poker,
	StraightFlush,
	RoyalFlush
}

stock bool:Character_CanCasino(playerid)
{
    return true;
}