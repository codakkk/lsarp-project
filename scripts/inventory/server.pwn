hook OnGameModeInit()
{
    printf("Loading items 2...");

    ServerItem_ManualInitializeItem(0, "Vuoto", ITEM_TYPE:ITEM_TYPE_NONE);
    // Setup weapons
    for(new i = 1; i <= 46; ++i)
    {
        if(i == 19 || i == 20 || i == 21) // Invalid items id
            continue;
        ServerItem_ManualInitializeItem(i, Weapon_GetName(i), ITEM_TYPE:ITEM_TYPE_WEAPON, Weapon_GetObjectModel(i), 1, 1);
    }
    gItem_Hamburger = ServerItem_InitializeItem("Hamburger Preconfezionato", ITEM_TYPE:ITEM_TYPE_FOOD, 0, 2, 0);
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
    

    gItem_LightAmmo = ServerItem_InitializeItem("Munizioni leggere", ITEM_TYPE:ITEM_TYPE_AMMO, 3016, 1000, 0);
    gItem_BuckShotAmmo = ServerItem_InitializeItem("Munizioni a pallettoni", ITEM_TYPE:ITEM_TYPE_AMMO, 2041, 1000, 0);
    gItem_HeavyAmmo = ServerItem_InitializeItem("Munizioni pesanti", ITEM_TYPE:ITEM_TYPE_AMMO, 3013, 1000, 0);
    gItem_RifleAmmo = ServerItem_InitializeItem("Munizioni fucile", ITEM_TYPE:ITEM_TYPE_AMMO, 2041, 1000, 0);
    printf("Total items loaded: %d\n", Iter_Count(ServerItems));
    return 1;
}

// Useful for setting item_ids manually (example: weapons)
stock ServerItem_ManualInitializeItem(item_id, name[], ITEM_TYPE:type, modelId = 0, maxStack = 0, isUnique = 0, ...)
{
    if(item_id < 0 || item_id >= MAX_ITEMS_IN_SERVER || Iter_Contains(ServerItems, item_id))
        return -1;
    foreach(new item : ServerItems)
    {
        if(!strcmp(ServerItem[item][sitemName], name, true))
        {
            printf("Error initializing item %s. Already exists!", ServerItem[item][sitemName]);
            return -1;
        }
    }
    ServerItem[item_id][sitemID] = item_id; // Redundant?
    //set(ServerItem[item_id][sitemName], name);
    
    format(ServerItem[item_id][sitemName], MAX_ITEM_NAME, "%s", name);
    ServerItem[item_id][sitemType] = type;
    ServerItem[item_id][sitemModelID] = modelId;
    
    if(maxStack == 0 || maxStack == 1 || isUnique == 1)
    {
        maxStack = 1;
        isUnique = 1;
    }
    ServerItem[item_id][sitemUnique] = isUnique;
    ServerItem[item_id][sitemMaxStack] = maxStack;

    new num_args = numargs(),
        start_args = 6;
    if(num_args > start_args)
    {
        if(num_args > start_args+5)
        {
            printf("ID: %d - More than 5 extra arguments provided.", item_id);
            num_args = start_args+5;
        }
        for(new i = start_args; i < num_args; ++i)
        {
            ServerItem[item_id][sitemExtraData][i-start_args] = getarg(i);
            //printf("Extra %d: %d", i-start_args, getarg(i));
        }
    }

    Iter_Add(ServerItems, item_id);

    //DEBUG
#if DEBUG
        //printf("ID: %d - Item: %s - Type: %s - Unique: %s - Max Stack: %d", item_id, ServerItem_GetName(item_id), ServerItem_GetTypeName(item_id), isUnique ? ("Yes") : ("No"), maxStack);
#endif
    return item_id;
}

// Useful for automatic items id
stock ServerItem_InitializeItem(name[], ITEM_TYPE:type, modelId = 0, maxStack = 0, isUnique = 0, ...)
{
    return ServerItem_ManualInitializeItem(Iter_Free(ServerItems), name, type, modelId, maxStack, isUnique, ___5);
}

stock ITEM_TYPE:ServerItem_GetType(itemid) return ServerItem[itemid][sitemType];
stock ServerItem_IsWeapon(itemid) return ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_WEAPON;
stock ServerItem_IsAmmo(itemid) return ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_AMMO;
stock ServerItem_IsFood(itemid) return ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_FOOD;
stock ServerItem_IsDrink(itemid) return ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_DRINK;
stock ServerItem_IsMedik(itemid) return ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_MEDIK;
stock ServerItem_IsBag(itemid) return ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_BAG;

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