#include <YSI\y_va>

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
    ServerItem[item_id][sitemUnique] = isUnique;
    ServerItem[item_id][sitemModelID] = modelId;
    
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
            printf("Extra %d: %d", i-start_args, getarg(i));
        }
    }

    if(maxStack == 0 || maxStack == 1 || isUnique)
    {
        maxStack = 1;
        isUnique = 1;
    }
    ServerItem[item_id][sitemMaxStack] = maxStack;
    Iter_Add(ServerItems, item_id);

    //DEBUG
    new uniqueStr[4];
    if(isUnique)
        uniqueStr = "Yes";
    else
        uniqueStr = "No";
#if DEBUG
        //printf("ID: %d - Item: %s - Type: %s - Unique: %s - Max Stack: %d", item_id, ServerItem_GetName(item_id), ServerItem_GetTypeName(item_id), uniqueStr, maxStack);
#endif
    return item_id;
}

// Useful for automatic items id
stock ServerItem_InitializeItem(name[], ITEM_TYPE:type, modelId = 0, maxStack = 0, isUnique = 0, va_args<>)
{
    return ServerItem_ManualInitializeItem(Iter_Free(ServerItems), name, type, modelId, maxStack, isUnique, ___5);
}

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
            case ITEM_TYPE_AMMO: string = "Zaino";
            case ITEM_TYPE_FOOD: string = "Cibo";
            case ITEM_TYPE_DRINK: string = "Bevanda";
            case ITEM_TYPE_MEDIK: string = "Medico";
            case ITEM_TYPE_BAG: string = "Zaino";
        }
    }
    return string;
}