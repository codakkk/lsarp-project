#define MAX_LOOT_ZONES 			(1000)
#define MAX_ITEMS_PER_LOOT 		(10)
#define MAX_LOOT_ZONES_PER_PAGE (15)

enum E_LOOT_ZONE
{
	lzID,
	Float:lzX,
	Float:lzY,
	Float:lzZ,
	lzInterior,
	lzVirtualWorld,
	lzLootItem[MAX_ITEMS_PER_LOOT],
	lzLootAmount[MAX_ITEMS_PER_LOOT],
	lzLootRarity[MAX_ITEMS_PER_LOOT],

	lzObject,
	lzPickup,
	Text3D:lz3DText,

	lzPlayerID, // Are we showing LootZone items to someone? 
	lzPlayerEditing, // Is someone editing this Loot Zone ? (admin) It represents the player id.
}
new LootZoneInfo[MAX_LOOT_ZONES][E_LOOT_ZONE],
	Iterator:LootZoneIterator<MAX_LOOT_ZONES>,
	LootZoneListItem[MAX_PLAYERS][MAX_LOOT_ZONES]; // LootZoneListItem used for both admin and players. (Admin for loot zone editing, players for looting)

new 
    List:LootZoneList,
    List:LootZoneInventoryList; // List<Inventory:LZINV>