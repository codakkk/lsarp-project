hook OnGameModeInit()
{
	printf("2");
    printf("Loading items.");

    ServerItem_ManualInitializeItem(0, "Vuoto", ITEM_TYPE:ITEM_TYPE_NONE, 0, 1);
    // Setup weapons
    for(new i = 1; i <= 46; ++i)
    {
	   if(i == 19 || i == 20 || i == 21 || Weapon_IsGrenade(i)) // Invalid items id
		  continue;
	   ServerItem_ManualInitializeItem(i, Weapon_GetName(i), ITEM_TYPE:ITEM_TYPE_WEAPON, Weapon_GetObjectModel(i), 1, 1);
    }

	ServerItem_ManualInitializeItem(16, Weapon_GetName(16), ITEM_TYPE_WEAPON, Weapon_GetObjectModel(16), 2); // Grenades
	ServerItem_ManualInitializeItem(17, Weapon_GetName(17), ITEM_TYPE_WEAPON, Weapon_GetObjectModel(17), 2); // Teargas
	ServerItem_ManualInitializeItem(18, Weapon_GetName(18), ITEM_TYPE_WEAPON, Weapon_GetObjectModel(18), 2); // Molotov
	ServerItem_ManualInitializeItem(39, Weapon_GetName(39), ITEM_TYPE_WEAPON, Weapon_GetObjectModel(39), 2); // Satchel

    gItem_Hamburger = ServerItem_ManualInitializeItem(47, "Hamburger Preconfezionato", ITEM_TYPE:ITEM_TYPE_FOOD, .modelId = 2804, .maxStack = 2);
	gItem_Beans = ServerItem_ManualInitializeItem(48, "Fagioli in barattolo", ITEM_TYPE:ITEM_TYPE_FOOD, 19568, 	2, 10);
	gItem_EnergyBar = ServerItem_ManualInitializeItem(49, "Barretta Energetica", ITEM_TYPE:ITEM_TYPE_FOOD, 2, 		2, 10);
	gItem_DriedMeat = ServerItem_ManualInitializeItem(50, "Carne Essiccata", ITEM_TYPE:ITEM_TYPE_FOOD, 		2, 		2, 10);
	gItem_PeanutButter = ServerItem_ManualInitializeItem(51, "Burro di arachidi", ITEM_TYPE:ITEM_TYPE_FOOD, 	2,		2, 10);
	gItem_CannedTuna = ServerItem_ManualInitializeItem(52, "Tonno in scatola", ITEM_TYPE:ITEM_TYPE_FOOD, 	2, 		2, 10);
	gItem_RawMeat = ServerItem_ManualInitializeItem(53, "Carne cruda", ITEM_TYPE:ITEM_TYPE_FOOD, 			2804,	2, 10);
	gItem_RawFish = ServerItem_ManualInitializeItem(54, "Pesce crudo", ITEM_TYPE:ITEM_TYPE_FOOD, 			19630, 	2, 10);

	// Drinks
    gItem_BottleOfWater = ServerItem_ManualInitializeItem(55, "Bottiglia d'acqua (33oz)", ITEM_TYPE:ITEM_TYPE_DRINK, 		.modelId = 1486, .maxStack = 1);
    gItem_SmallBottleOfWater = ServerItem_ManualInitializeItem(56, "Bottiglia d'acqua (16oz)", ITEM_TYPE:ITEM_TYPE_DRINK, 	.modelId = 1486, .maxStack = 1);
    gItem_Milk = ServerItem_ManualInitializeItem(57, "Latte in polvere", ITEM_TYPE:ITEM_TYPE_DRINK, 						.modelId = 19569, .maxStack = 1);
	gItem_RainWater = ServerItem_ManualInitializeItem(58, "Barattolo d'acqua piovana", ITEM_TYPE:ITEM_TYPE_DRINK, 			.modelId = 2, .maxStack = 1); 

	// Medical Objects
	gItem_RationK = ServerItem_ManualInitializeItem(59, "Razione K", ITEM_TYPE:ITEM_TYPE_MEDIK, 11748, 1);
	gItem_Morfine = ServerItem_ManualInitializeItem(60, "Siringa di Morfina", ITEM_TYPE:ITEM_TYPE_MEDIK, 1, 1);
	gItem_Bandages = ServerItem_ManualInitializeItem(61, "Bende", ITEM_TYPE:ITEM_TYPE_MEDIK, 11747, 1);
	gItem_Medikit = ServerItem_ManualInitializeItem(62, "Kit Pronto Soccorso", ITEM_TYPE:ITEM_TYPE_MEDIK, 11738, 1);

	gItem_SmallBackPack = ServerItem_ManualInitializeItem(63, "Zainetto", ITEM_TYPE:ITEM_TYPE_BAG, 		3026, 1, 1, 5);
    gItem_BigBackPack = ServerItem_ManualInitializeItem(64, "Zaino Grande", ITEM_TYPE:ITEM_TYPE_BAG, 	19559, 1, 1, 2);
    

    gItem_LightAmmo = ServerItem_ManualInitializeItem(65, "Munizioni leggere", ITEM_TYPE:ITEM_TYPE_AMMO, 			.modelId = 3016, .maxStack = 1000);
    gItem_BuckShotAmmo = ServerItem_ManualInitializeItem(66, "Munizioni a pallettoni", ITEM_TYPE:ITEM_TYPE_AMMO, 	.modelId = 2041, .maxStack = 1000);
    gItem_HeavyAmmo = ServerItem_ManualInitializeItem(67, "Munizioni pesanti", ITEM_TYPE:ITEM_TYPE_AMMO, 			.modelId = 3013, .maxStack = 1000);
    gItem_RifleAmmo = ServerItem_ManualInitializeItem(68, "Munizioni fucile", ITEM_TYPE:ITEM_TYPE_AMMO, 			.modelId = 2041, .maxStack = 1000);

	gItem_SmallFuelTank = ServerItem_ManualInitializeItem(69, 	"Tanica Piccola", ITEM_TYPE:ITEM_TYPE_FUEL, 	1650, 1, 35);
	gItem_MediumFuelTank = ServerItem_ManualInitializeItem(70, 	"Tanica Media", ITEM_TYPE:ITEM_TYPE_FUEL, 	1650, 1, 70);
	gItem_BigFuelTank = ServerItem_ManualInitializeItem(71, 	"Tanica Grande", ITEM_TYPE:ITEM_TYPE_FUEL, 		1650, 1, 100);

    printf("Items loaded. N: %d\n", Iter_Count(ServerItems));
    return 1;
}

// Useful for automatic items id
stock ServerItem_InitializeItem(name[], ITEM_TYPE:type, modelId = 0, maxStack = 1, ...)
{
    return ServerItem_ManualInitializeItem(Iter_Free(ServerItems), name, type, modelId, maxStack, ___5);
}

// Useful for setting item_ids manually (example: weapons)
stock ServerItem_ManualInitializeItem(itemid, name[], ITEM_TYPE:type, modelId = 0, maxStack = 1, ...)
{
    if(itemid < 0 || itemid >= MAX_ITEMS_IN_SERVER || Iter_Contains(ServerItems, itemid))
	   return -1;
	
	if(maxStack <= 0)
	{
		printf("Failed initializing item %d. maxStack = %d.", itemid, maxStack);
		return -1;	
	}

    foreach(new item : ServerItems)
    {
	   if(!strcmp(ServerItem[item][sitemName], name, true))
	   {
		  printf("Error initializing item %s. Already exists!", ServerItem[item][sitemName]);
		  return -1;
	   }
    }
    ServerItem[itemid][sitemID] = itemid; // Redundant?
    format(ServerItem[itemid][sitemName], MAX_ITEM_NAME, "%s", name);
    ServerItem[itemid][sitemType] = type;
    ServerItem[itemid][sitemModelID] = modelId;

    ServerItem[itemid][sitemUnique] = maxStack == 1;
    ServerItem[itemid][sitemMaxStack] = maxStack;

    new num_args = numargs(),
	   start_args = 5;
    if(num_args > start_args)
    {
	   if(num_args > start_args+5)
	   {
		  printf("ID: %d - More than 5 extra arguments provided.", itemid);
		  num_args = start_args+5;
	   }
	   for(new i = start_args; i < num_args; ++i)
	   {
		  ServerItem[itemid][sitemExtraData][i-start_args] = getarg(i);
		  //printf("Extra %d: %d", i-start_args, getarg(i));
	   }
    }

    Iter_Add(ServerItems, itemid);
    return itemid;
}

stock ITEM_TYPE:ServerItem_GetType(itemid) return ServerItem[itemid][sitemType];
stock ServerItem_IsWeapon(itemid) return ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_WEAPON;
stock ServerItem_IsAmmo(itemid) return ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_AMMO;
stock ServerItem_IsFood(itemid) return ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_FOOD;
stock ServerItem_IsDrink(itemid) return ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_DRINK;
stock ServerItem_IsMedik(itemid) return ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_MEDIK;
stock ServerItem_IsBag(itemid) return ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_BAG;
stock ServerItem_IsDrug(itemid) return ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_DRUG;

stock ServerItem_GetMaxStack(itemid) return ServerItem[itemid][sitemMaxStack];

stock ServerItem_IsValid(itemid)
{
    return !(itemid < 0 || itemid >= MAX_ITEMS_IN_SERVER) && Iter_Contains(ServerItems, itemid);
}

stock ServerItem_IsUnique(itemid)
{
    return ServerItem[itemid][sitemUnique];
}

stock ServerItem_GetExtra(itemid, EXTRA_TYPE_ID:extraid)
{
    return ServerItem[item][sitemExtraData][extraid];
}

stock ServerItem_GetTypeName(item_id)
{
    new string[MAX_ITEM_NAME] = "Invalid";
    
    if(! (item_id < 0 || item_id >= MAX_ITEMS_IN_SERVER) && Iter_Contains(ServerItems, item_id))
    {
	   switch(ServerItem[item_id][sitemType])
	   {
		  case ITEM_TYPE_NONE: string = "Niente";
		  case ITEM_TYPE_MATERIAL: string = "Niente";
		  case ITEM_TYPE_WEAPON: string = "Arma";
		  case ITEM_TYPE_AMMO: string = "Munizioni";
		  case ITEM_TYPE_FOOD: string = "Cibo";
		  case ITEM_TYPE_DRINK: string = "Bevanda";
		  case ITEM_TYPE_MEDIK: string = "Medico";
		  case ITEM_TYPE_BAG: string = "Zaino";
	   }
    }
    return string;
}

stock ServerItem_GetModelID(itemid)
{
    return ServerItem[itemid][sitemModelID];
}

// Common functions for faster extra 
stock ServerItem_GetBagSize(bagid)
{
    if(!ServerItem_IsValid(bagid) || !ServerItem_IsBag(bagid))
	   return 0;
    return ServerItem[bagid][sitemExtraData][0];
}

stock ServerItem_GetTankCapacity(itemid)
{
	return ServerItem[itemid][sitemExtraData][0];
}

stock ServerItem_GetFoodValue(itemid)
{
	return ServerItem[itemid][sitemExtraData][0];
}