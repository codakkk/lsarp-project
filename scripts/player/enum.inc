#include <YSI_Coding\y_hooks>

#define MAX_VEHICLES_PER_PLAYER			20
#define MAX_DAMAGES_PER_PLAYER			(24)
#define PENDING_REQUEST_TIME			1

new
	// probably Iterators aren't worth for this.
	Iterator:pTogglePM[MAX_PLAYERS]<MAX_PLAYERS>,
	Iterator:pToggleOOC[MAX_PLAYERS]<MAX_PLAYERS>
;