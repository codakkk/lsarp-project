#include <YSI\y_hooks>
#include <inventory/inventory_server.pwn>



hook OnGameModeInit()
{
    printf("---------- Setting Up Server Items ----------");

    ServerItem_ManualInitializeItem(0, "Vuoto", ITEM_TYPE:ITEM_TYPE_NONE);
    // Setup weapons
    for(new i = 1; i <= 46; ++i)
    {
        if(i == 19 || i == 20 || i == 21) // Invalid items id
            continue;
        ServerItem_ManualInitializeItem(i, Weapon_GetName(i), ITEM_TYPE:ITEM_TYPE_WEAPON, 1, 1);
    }
    ServerItem_InitializeItem("Hamburger Preconfezionato", ITEM_TYPE:ITEM_TYPE_FOOD, 0, 2, 0);
	ServerItem_InitializeItem("Fagioli in barattolo", ITEM_TYPE:ITEM_TYPE_FOOD, 2, 0);
	ServerItem_InitializeItem("Barrette Energetiche", ITEM_TYPE:ITEM_TYPE_FOOD, 2, 0);
	ServerItem_InitializeItem("Carne Essiccata", ITEM_TYPE:ITEM_TYPE_FOOD, 2, 0);
	ServerItem_InitializeItem("Burro di arachidi", ITEM_TYPE:ITEM_TYPE_FOOD, 2, 0);
	ServerItem_InitializeItem("Tonno in scatola", ITEM_TYPE:ITEM_TYPE_FOOD, 2, 0);
	ServerItem_InitializeItem("Carne cruda", ITEM_TYPE:ITEM_TYPE_FOOD, 2, 0);
	ServerItem_InitializeItem("Pesce crudo", ITEM_TYPE:ITEM_TYPE_FOOD, 2, 0);

	// Drinks
    ServerItem_InitializeItem("Bottiglia d'acqua (33oz)", ITEM_TYPE:ITEM_TYPE_DRINK, 2, 0);
    ServerItem_InitializeItem("Bottiglia d'acqua (16oz)", ITEM_TYPE:ITEM_TYPE_DRINK, 2, 0);
    ServerItem_InitializeItem("Latte in polvere", ITEM_TYPE:ITEM_TYPE_DRINK, 2, 0);
	ServerItem_InitializeItem("Barattolo d'acqua piovana", ITEM_TYPE:ITEM_TYPE_DRINK, 2, 0); 

	// Medical Objects
	gItem_RationK = ServerItem_InitializeItem("Razione K", ITEM_TYPE:ITEM_TYPE_MEDIK, 1, 1);
	ServerItem_InitializeItem("Siringa di Morfina", ITEM_TYPE:ITEM_TYPE_MEDIK, 1, 1);
	ServerItem_InitializeItem("Bende", ITEM_TYPE:ITEM_TYPE_MEDIK, 1, 1);
	ServerItem_InitializeItem("Kit Pronto Soccorso", ITEM_TYPE:ITEM_TYPE_MEDIK, 1, 1);

    ServerItem_InitializeItem("Zainetto", ITEM_TYPE:ITEM_TYPE_BAG, _, 1, 1, 5);
    ServerItem_InitializeItem("Zaino Grande", ITEM_TYPE:ITEM_TYPE_BAG, _, 1, 1, 2);
    

    ServerItem_ManualInitializeItem(90, "Munizioni", ITEM_TYPE:ITEM_TYPE_AMMO, 0, 9999999, 0);
    printf("Total items loaded: %d\n", Iter_Count(ServerItems));
    printf("---------- Server Items Created -------------");
    return 1;
}