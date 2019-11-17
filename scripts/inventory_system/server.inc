
#if defined _included_INVENTORY_SERVER
	#endinput
#endif

#define _included_INVENTORY_SERVER

#include <inventory_system\items>

#include <YSI_Coding\y_hooks>

#define MAX_ITEM_NAME			    (32)
#define MAX_ITEMS_IN_SERVER		   (150)
#define MAX_ITEMS_PER_PLAYER		  (50)
#define MAX_ITEMS_PER_VEHICLE		 (50)
#define INVALID_ITEM_ID			  (-1)
#define PLAYER_INVENTORY_START_SIZE	(10)

#define INVENTORY_NULL			(Inventory:0)


enum ITEM_TYPE
{
	ITEM_TYPE_NONE = 0,
	ITEM_TYPE_MATERIAL,
	ITEM_TYPE_WEAPON,
	ITEM_TYPE_AMMO,
	ITEM_TYPE_FOOD,
	ITEM_TYPE_DRINK,
	ITEM_TYPE_MEDIC,
	ITEM_TYPE_BAG,
	ITEM_TYPE_FUEL,
	ITEM_TYPE_DRUG,
	ITEM_TYPE_MECHANICAL_COMPONENTS,
	ITEM_TYPE_MONEY,
	ITEM_TYPE_GENERAL
};

enum EXTRA_TYPE_ID
{
	EXTRA_BAG_SIZE = 0
};

enum E_SERVER_ITEM_DATA
{
	sitemInitialized,
	sitemID,
	sitemName[MAX_ITEM_NAME],
	ITEM_TYPE:sitemType,
	sitemUnique,
	sitemMaxStack,
	sitemExtraData[5],
	sitemModelID
};
static
	ServerItem[MAX_ITEMS_IN_SERVER][E_SERVER_ITEM_DATA],
	ItemName[MAX_ITEMS_IN_SERVER][MAX_ITEM_NAME]
;

static Iterator:ServerItems<MAX_ITEMS_IN_SERVER>;

hook OnGameModeInit()
{
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

    gItem_Hamburger = ServerItem_InitializeFoodItem(47, "Hamburger Preconfezionato", 2804, 10);
	gItem_Beans = ServerItem_InitializeFoodItem(48, "Fagioli in barattolo", 19567,	10);
	gItem_EnergyBar = ServerItem_InitializeFoodItem(49, "Barretta Energetica", 11748, 10);
	gItem_DriedMeat = ServerItem_InitializeFoodItem(50, "Carne Essiccata", 19568, 10);
	gItem_PeanutButter = ServerItem_InitializeFoodItem(51, "Burro di arachidi", 1455, 10);
	gItem_CannedTuna = ServerItem_InitializeFoodItem(52, "Tonno in scatola", 19567, 10);
	gItem_RawMeat = ServerItem_InitializeFoodItem(53, "Carne cruda", 2804,	10);
	gItem_RawFish = ServerItem_InitializeFoodItem(54, "Pesce crudo", 19630, 10);

	// Drinks
    gItem_BottleOfWater = ServerItem_InitializeDrinkItem(55, "Bottiglia d'acqua (33oz)", 19570, 10);
    gItem_SmallBottleOfWater = ServerItem_InitializeDrinkItem(56, "Bottiglia d'acqua (16oz)", 19570, 10);
    gItem_Milk = ServerItem_InitializeDrinkItem(57, "Latte in polvere", 19569, 10);
	gItem_RainWater = ServerItem_InitializeDrinkItem(58, "Barattolo d'acqua piovana", 19570, 10); 

	gItem_RationK = ServerItem_InitializeFoodItem(59, "Razione K", 1575, 100, 100);

	// Medical Objects
	gItem_Morfine = ServerItem_InitializeMedicItem(60, "Siringa di Morfina", 2902, 5, 30);
	gItem_Bandages = ServerItem_InitializeMedicItem(61, "Bende", 11747, 5, 15);
	gItem_Medikit = ServerItem_InitializeMedicItem(62, "Kit Pronto Soccorso", 11738, 2, 50);

	gItem_SmallBackPack = ServerItem_ManualInitializeItem(63, "Zainetto", ITEM_TYPE:ITEM_TYPE_BAG, 		3026, 1, 1, 5);
    gItem_BigBackPack = ServerItem_ManualInitializeItem(64, "Zaino Grande", ITEM_TYPE:ITEM_TYPE_BAG, 	19559, 1, 1, 2);
    

    gItem_LightAmmo = ServerItem_ManualInitializeItem(65, "Munizioni leggere", ITEM_TYPE:ITEM_TYPE_AMMO, 			.modelId = 3016, .maxStack = 1000);
    gItem_BuckShotAmmo = ServerItem_ManualInitializeItem(66, "Munizioni a pallettoni", ITEM_TYPE:ITEM_TYPE_AMMO, 	.modelId = 2041, .maxStack = 1000);
    gItem_HeavyAmmo = ServerItem_ManualInitializeItem(67, "Munizioni pesanti", ITEM_TYPE:ITEM_TYPE_AMMO, 			.modelId = 3013, .maxStack = 1000);
    gItem_RifleAmmo = ServerItem_ManualInitializeItem(68, "Munizioni fucile", ITEM_TYPE:ITEM_TYPE_AMMO, 			.modelId = 2041, .maxStack = 1000);

	gItem_SmallFuelTank = ServerItem_ManualInitializeItem(69, 	"Tanica Piccola", ITEM_TYPE:ITEM_TYPE_FUEL, 	1650, 1, 35);
	gItem_MediumFuelTank = ServerItem_ManualInitializeItem(70, 	"Tanica Media", ITEM_TYPE:ITEM_TYPE_FUEL, 	1650, 1, 70);
	gItem_BigFuelTank = ServerItem_ManualInitializeItem(71, 	"Tanica Grande", ITEM_TYPE:ITEM_TYPE_FUEL, 		1650, 1, 100);

	// Remember to change MAX_PLAYER_DRUGS inside player/components/drugs.pwn when added/removed drugs.
	gItem_PCP = ServerItem_InitializeDrugItem(72, "PCP", 1650, 3, 100, 30, 0);
	gItem_Xanax = ServerItem_InitializeDrugItem(73, "Xanax",1650, 3, 120, 5, 4);
	gItem_MDMA = ServerItem_InitializeDrugItem(74, "MDMA", 1650, 3, 100, 20, 0);
	gItem_Crack = ServerItem_InitializeDrugItem(75, "Crack", 1650, 3, 100, 40, 0);
	gItem_Oxycodone = ServerItem_InitializeDrugItem(76, "Ossicodone", 1650, 3, 100, 20, 0);
	gItem_Marijuana = ServerItem_InitializeDrugItem(77, "Marijuana", 1650, 0, 100, 5, 0);
	gItem_Hashish = ServerItem_InitializeDrugItem(78, "Hashish", 1650, 0, 100, 5, 0);
	gItem_Eroine = ServerItem_InitializeDrugItem(79, "Eroina", 1650, 3, 100, 50, 0);
	gItem_Coke = ServerItem_InitializeDrugItem(80, "Cocaina", 1650, 3, 100, 40, 0);
	gItem_Meth = ServerItem_InitializeDrugItem(81, "Metanfetamina", 1650, 3, 100, 50, 0);
	gItem_LSD = ServerItem_InitializeDrugItem(82, "LSD", 1650, 3, 100, 30, 0);

	gItem_Bread = ServerItem_InitializeFoodItem(83, "Pane", 19579, 20);
	gItem_BreadPiece = ServerItem_InitializeFoodItem(84, "Fetta di pane", 19883, 10);
	gItem_PackedMeat = ServerItem_InitializeFoodItem(85, "Carne Confezionata", 19560, 10);
	gItem_Cerelas = ServerItem_InitializeFoodItem(86, "Scatola di cereali", 19562, 10);
	gItem_IceCream = ServerItem_InitializeFoodItem(87, "Vaschetta di gelato", 19567, 10);

	gItem_Tomato = ServerItem_InitializeFoodItem(88, "Pomodoro", 19577, 10);
	gItem_RedApple = ServerItem_InitializeFoodItem(89, "Mela Rossa", 19575, 10);
	gItem_GreenApple = ServerItem_InitializeFoodItem(90, "Mela Verde", 19576, 10);
	gItem_Orange = ServerItem_InitializeFoodItem(91, "Arancia", 19574, 10);
	gItem_Banana = ServerItem_InitializeFoodItem(92, "Banana", 19578, 10);

	gItem_OrangeJuice = ServerItem_InitializeDrinkItem(93, "Succo d'Arancia", 19563, 10);
	gItem_AppleJuice = ServerItem_InitializeDrinkItem(94, "Succo di mela", 19564, 10);

	gItem_MechanicalComponents = ServerItem_ManualInitializeItem(95, "Componenti Meccanici", ITEM_TYPE_MECHANICAL_COMPONENTS, 19577, 64);

	gItem_Money = ServerItem_ManualInitializeItem(96, "Denaro", ITEM_TYPE_MONEY, 1212, cellmax-1);

	gItem_Radio = ServerItem_ManualInitializeItem(97, "Radio", ITEM_TYPE_GENERAL, 1212, 1);

	gItem_Cigarette = ServerItem_ManualInitializeItem(98, "Sigarette", ITEM_TYPE_GENERAL, 1212, 10);

	gItem_Dress = ServerItem_ManualInitializeItem(99, "Vestito", ITEM_TYPE_GENERAL, 2846, 1);

	gItem_Phone = ServerItem_ManualInitializeItem(100, "Cellulare", ITEM_TYPE_GENERAL, 330, 1);

    printf("Items loaded. N: %d\n", Iter_Count(ServerItems));
    return 1;
}

// Useful for automatic items id
stock ServerItem_InitializeItem(const name[], ITEM_TYPE:type, modelId = 0, maxStack = 1, ...)
{
    return ServerItem_ManualInitializeItem(Iter_Free(ServerItems), name, type, modelId, maxStack, ___4);
}

stock ServerItem_InitializeDrugItem(id, const name[], modelid = 0, amountForAddiction = 0, maxHealth = 100, healthGain = 0, effectTime = 0)
{
	return ServerItem_ManualInitializeItem(id, name, ITEM_TYPE_DRUG, modelid, 100, amountForAddiction, maxHealth, healthGain, effectTime);
}

stock ServerItem_InitializeFoodItem(id, const name[], modelId, foodAmount, health = 0)
{
	return ServerItem_ManualInitializeItem(id, name, ITEM_TYPE_FOOD, modelId, 15, foodAmount, health);
}

stock ServerItem_InitializeMedicItem(id, const name[], modelId, maxStacks, healthAmount)
{
	return ServerItem_ManualInitializeItem(id, name, ITEM_TYPE:ITEM_TYPE_MEDIC, modelId, maxStacks, healthAmount);
}

stock ServerItem_InitializeDrinkItem(id, const name[], modelId, foodAmount)
{
	return ServerItem_ManualInitializeItem(id, name, ITEM_TYPE_DRINK, modelId, 2, foodAmount);
}

// Useful for setting item_ids manually (example: weapons)
stock ServerItem_ManualInitializeItem(itemid, const name[], ITEM_TYPE:type, modelId = 0, maxStack = 1, GLOBAL_TAG_TYPES:...)
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
	   if(!strcmp(ItemName[item], name, true))
	   {
		  printf("Error initializing item %s. Already exists.", ItemName[item]);
		  return -1;
	   }
    }
	ServerItem[itemid][sitemInitialized] = 1;
    ServerItem[itemid][sitemID] = itemid; // Redundant?
    format(ItemName[itemid], MAX_ITEM_NAME, "%s", name);
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

stock ServerItem_GetName(itemid)
{
	return ItemName[itemid];
}

stock ITEM_TYPE:ServerItem_GetType(itemid) return ServerItem[itemid][sitemType];
stock ServerItem_IsMaterial(itemid) return ServerItem_IsValid(itemid) && ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_MATERIAL;
stock ServerItem_IsWeapon(itemid) return ServerItem_IsValid(itemid) && ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_WEAPON;
stock ServerItem_IsAmmo(itemid) return ServerItem_IsValid(itemid) && ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_AMMO;
stock ServerItem_IsFood(itemid) return ServerItem_IsValid(itemid) && ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_FOOD;
stock ServerItem_IsDrink(itemid) return ServerItem_IsValid(itemid) && ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_DRINK;
stock ServerItem_IsMedic(itemid) return ServerItem_IsValid(itemid) && ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_MEDIC;
stock ServerItem_IsBag(itemid) return ServerItem_IsValid(itemid) && ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_BAG;
stock ServerItem_IsFuel(itemid) return ServerItem_IsValid(itemid) && ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_FUEL;
stock ServerItem_IsDrug(itemid) return ServerItem_IsValid(itemid) && ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_DRUG;
stock ServerItem_IsMechanicalComp(itemid) return ServerItem_IsValid(itemid) && ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_MECHANICAL_COMPONENTS;
stock ServerItem_IsMoney(itemid) return ServerItem_IsValid(itemid) && ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_MONEY;
stock ServerItem_IsGeneral(itemid) return ServerItem_IsValid(itemid) && ServerItem[itemid][sitemType] == ITEM_TYPE:ITEM_TYPE_GENERAL;

stock ServerItem_GetMaxStack(itemid) return ServerItem[itemid][sitemMaxStack];

stock ServerItem_IsValid(itemid)
{
    return (0 <= itemid < MAX_ITEMS_IN_SERVER) && ServerItem[itemid][sitemInitialized] && Iter_Contains(ServerItems, itemid);
}

stock ServerItem_IsUnique(itemid)
{
    return ServerItem[itemid][sitemUnique];
}

stock ServerItem_GetExtra(itemid, extraid)
{
    return ServerItem[itemid][sitemExtraData][extraid];
}

stock ServerItem_GetTypeName(itemid)
{
    new string[MAX_ITEM_NAME] = "Sconosciuto";
    
    if(0 <= itemid < MAX_ITEMS_IN_SERVER && Iter_Contains(ServerItems, itemid))
    {
	   switch(ServerItem[itemid][sitemType])
	   {
		  case ITEM_TYPE_NONE: string = "Niente";
		  case ITEM_TYPE_MATERIAL: string = "Materiali";
		  case ITEM_TYPE_WEAPON: string = "Arma";
		  case ITEM_TYPE_AMMO: string = "Munizioni";
		  case ITEM_TYPE_FOOD: string = "Cibo";
		  case ITEM_TYPE_DRINK: string = "Bevanda";
		  case ITEM_TYPE_MEDIC: string = "Medico";
		  case ITEM_TYPE_BAG: string = "Zaino";
		  case ITEM_TYPE_FUEL: string = "Carburante";
		  case ITEM_TYPE_DRUG: string = "Droga";
		  case ITEM_TYPE_MECHANICAL_COMPONENTS: string = "Componenti";
		  case ITEM_TYPE_MONEY: string = "Denaro";
		  case ITEM_TYPE_GENERAL: string = "Altro";
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

stock MedicItem_GetHealthGain(itemid)
{
	return ServerItem_GetExtra(itemid, 0);
}

stock FoodItem_GetFoodValue(itemid)
{
	return ServerItem[itemid][sitemExtraData][0];
}

stock FoodItem_GetHealthValue(itemid)
{
	return ServerItem[itemid][sitemExtraData][1];
}

stock DrugItem_CanGetDependent(itemid)
{
	return DrugItem_GetAddictionAmount(itemid) > 0;
}

stock DrugItem_GetAddictionAmount(itemid)
{
	return ServerItem_GetExtra(itemid, 0) > 0;
}

stock DrugItem_GetMaxHealth(itemid)
{
	return ServerItem_GetExtra(itemid, 1);
}

stock DrugItem_GetHealthGain(itemid)
{
	return ServerItem_GetExtra(itemid, 2);
}

stock DrugItem_GetEffectTime(itemid)
{
	return ServerItem_GetExtra(itemid, 3);
}

stock Float:FuelItem_GetAmount(itemid)
{
	return float(ServerItem_GetExtra(itemid, 0));
}

SSCANF:slotid(string[])
{
	new value = strval(string);
	value--;
	return value;
}

SSCANF:item(string[])
{
	// probably an ID
	if('0' <= string[0] <= '9')
	{
		new ret = strval(string);
		if(0 <= ret <= MAX_ITEMS_IN_SERVER)
		{
			return ret;
		}
	}
	else
	{
		new len = strlen(string);
		for(new i = 0; i < len; ++i)
		{
			if(string[i] == '_')
				string[i] = ' ';
		}
		foreach(new item : ServerItems)
		{
			if(!strcmp(ItemName[item], string, true) || strfind(ItemName[item], string, true) > -1)
				return item;
		}
	}
    return INVALID_ITEM_ID;
}