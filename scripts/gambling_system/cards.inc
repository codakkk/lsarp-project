

#if defined _included_CARDS
    #endinput
#endif 


static enum E_CARDS
{
	cardName[32],
	cardChar,
	cardValue
};
static PokerCards[52][E_CARDS] =
{
	{"ld_card:cd1c", 'c', 11},
	{"ld_card:cd2c", 'c', 2},
	{"ld_card:cd3c", 'c', 3},
	{"ld_card:cd4c", 'c', 4},
	{"ld_card:cd5c", 'c', 5},
	{"ld_card:cd6c", 'c', 6},
	{"ld_card:cd7c", 'c', 6},
	{"ld_card:cd8c", 'c', 7},
	{"ld_card:cd9c", 'c', 8},
	{"ld_card:cd10c", 'c', 9},
	{"ld_card:cd11c", 'c', 10},
	{"ld_card:cd12c", 'c', 10},
	{"ld_card:cd13c", 'c', 10},
	
	{"ld_card:cd1d", 'd', 1},
	{"ld_card:cd2d", 'd', 2},
	{"ld_card:cd3d", 'd', 3},
	{"ld_card:cd4d", 'd', 4},
	{"ld_card:cd5d", 'd', 5},
	{"ld_card:cd6d", 'd', 6},
	{"ld_card:cd7d", 'd', 7},
	{"ld_card:cd8d", 'd', 8},
	{"ld_card:cd9d", 'd', 9},
	{"ld_card:cd10d", 'd', 10},
	{"ld_card:cd11d", 'd', 10},
	{"ld_card:cd12d", 'd', 10},
	{"ld_card:cd13d", 'd', 10},
	
	{"ld_card:cd1h", 'h', 11},
	{"ld_card:cd2h", 'h', 2},
	{"ld_card:cd3h", 'h', 3},
	{"ld_card:cd4h", 'h', 4},
	{"ld_card:cd5h", 'h', 5},
	{"ld_card:cd6h", 'h', 6},
	{"ld_card:cd7h", 'h', 7},
	{"ld_card:cd8h", 'h', 8},
	{"ld_card:cd9h", 'h', 9},
	{"ld_card:cd10h", 'h', 10},
	{"ld_card:cd11h", 'h', 10},
	{"ld_card:cd12h", 'h', 10},
	{"ld_card:cd13h", 'h', 10},
	
	{"ld_card:cd1s", 's', 11},
	{"ld_card:cd2s", 's', 2},
	{"ld_card:cd3s", 's', 3},
	{"ld_card:cd4s", 's', 4},
	{"ld_card:cd5s", 's', 5},
	{"ld_card:cd6s", 's', 6},
	{"ld_card:cd7s", 's', 7},
	{"ld_card:cd8s", 's', 8},
	{"ld_card:cd9s", 's', 9},
	{"ld_card:cd10s", 's', 10},
	{"ld_card:cd11s", 's', 10},
	{"ld_card:cd12s", 's', 10},
	{"ld_card:cd13s", 's', 10}
};

stock bool:Card_GetName(cardid, name[])
{
    if(cardid < 0 || cardid >= 52)
        return false;
    set(name, PokerCards[cardid][cardName]);
    return true;
}

stock Card_GetChar(cardid)
{
    if(cardid < 0 || cardid >= 52)
        return '';
    return PokerCards[cardid][cardChar];
}

stock Card_GetValue(cardid)
{
    if(cardid < 0 || cardid >= 52)
        return 0;
    return PokerCards[cardid][cardValue];
}