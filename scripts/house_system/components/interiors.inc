#include <YSI_Coding\y_hooks>


// Should I load allHouseInteriors in PawnPlus's maps at startup with <HOUSE_SIZE_TYPE, List:E_INTRERIORS_HOUSE> ?

enum 
{
    HOUSE_BUGGED = -1,
    HOUSE_VERY_LITTLE = 1,
    HOUSE_LITTLE = 2,
    HOUSE_MEDIUM = 3,
    HOUSE_BIG = 4,
    HOUSE_HUGE = 5,
	ROULOTTE_VERY_SMALL = 6,
	ROULOTTE_SMALL = 7,
	ROULOTTE_MEDIUM = 8,
	ROULOTTE_BIG = 9,

	HOUSE_LAST
}

enum _:E_INTERIORS_HOUSE
{
	iHouseInt,
	Float:iHouseX,
	Float:iHouseY,
	Float:iHouseZ,
	iType
};

static Map:HouseInteriors; // <HOUSE_SIZE_TYPE, List:E_INTRERIORS_HOUSE>

stock HouseInterior_IsValidType(type)
{
	return (HOUSE_VERY_LITTLE <= type < HOUSE_LAST);
}

stock HouseInterior_Get(interiorType, interiorid, data[E_INTERIORS_HOUSE])
{
	if(!map_has_key(HouseInteriors, interiorType))
		return 0;
	new List:list = List:map_get(HouseInteriors, interiorType);

	if(interiorid < 0 || interiorid >= list_size(list))
		return 0;

	list_get_arr(list, interiorid, data);
	return 1;
}

hook OnGameModeInit()
{
	HouseInteriors = map_new();
	new Float:allHouseInteriors[][E_INTERIORS_HOUSE] =
	{
		{1, 2513.4465, -1729.1830, 778.6371, ROULOTTE_SMALL},
		{1, 2333.2441, -1711.9185, -30.4113, ROULOTTE_VERY_SMALL},
		{1, 1955.7535, -1637.0811, -46.6805, ROULOTTE_MEDIUM},
		{1, 796.5201, -1497.1542, -63.5100, ROULOTTE_BIG},
		{1, 2479.6494, -1727.0028, -58.4227, ROULOTTE_BIG},
		{1, 2481.7354, -1728.3798, -30.4113, ROULOTTE_BIG},
		{10, 2269.8772, -1210.3240, 1047.5625, HOUSE_BUGGED},
		{2, 2468.2576, -1698.2361, 1013.5078, HOUSE_MEDIUM},
		{3, 2495.8711, -1693.0996, 1014.7422, HOUSE_BIG},
		{5, 2233.7888, -1114.2689, 1050.8828, HOUSE_LITTLE},
		{9, 2317.8015, -1026.2113, 1050.2178, HOUSE_BIG},
		{3, 235.2923, 1187.3684, 1080.2578, HOUSE_BIG},
		{2, 226.2912, 1240.0554, 1082.1406, HOUSE_LITTLE},
		{1, 223.1618, 1287.5175, 1082.1406, HOUSE_LITTLE},
		{5, 226.9281, 1114.2813, 1080.9962, HOUSE_MEDIUM},
		{15, 295.1485, 1473.1080, 1080.2578, HOUSE_MEDIUM},
		{10, 23.9584, 1340.6075, 1084.3750, HOUSE_BIG},
		{4, 222.0425, 1140.7565, 1082.6094, HOUSE_MEDIUM},
		{12, 2324.4209, -1148.9365, 1050.7101, HOUSE_BIG},
		{4, -261.1430, 1456.6396, 1084.3672, HOUSE_BIG},
		{5, 22.9676, 1403.8368,	1084.4297, HOUSE_MEDIUM},
		{5, 140.3088, 1366.8196, 1083.8594, HOUSE_BIG},
		{6, 234.3149, 1064.2772, 1084.2114, HOUSE_BIG},
		{7, 225.7000, 1022.0012, 1084.0161, HOUSE_BIG},
		{5, 1298.9750, -796.4567, 1084.0078, HOUSE_HUGE},
		{15, -283.8687, 1471.1174, 1084.3750, HOUSE_BIG},
		{1, 179.4169, 1764.4050, 5999.2876, HOUSE_MEDIUM},
		{10, 2259.38, -1135.77, 1050.64, HOUSE_LITTLE},
		{1, 243.72, 304.91, 999.15, HOUSE_VERY_LITTLE},
		{2, 266.50, 304.90, 999.15, HOUSE_VERY_LITTLE},

		// Added by Clou
		{6, 2481.7778,-2610.4031,-30.2500, HOUSE_VERY_LITTLE},
		{5,2513.5132, -1729.1434, 778.7484, HOUSE_VERY_LITTLE},
		{4, 796.7826, -1497.1791, -64.4517, HOUSE_VERY_LITTLE},
		{6, 2481.7778,-2610.4031,-30.2500, HOUSE_VERY_LITTLE},
		{7, 1535.8977,-6.6340,1014.2022, HOUSE_VERY_LITTLE},
		{8, 2333.2986,-1712.0065,-30.4113, HOUSE_VERY_LITTLE},
		{9, 2479.5930,-1726.8823,-58.4227, HOUSE_VERY_LITTLE},
		{10, 2481.7334,-1728.2928,-30.4113, HOUSE_VERY_LITTLE},
		{11 ,2481.7334,-1728.2928,-30.4113, HOUSE_VERY_LITTLE},
		{12, 1946.4091,-1642.4312,-61.9019, HOUSE_VERY_LITTLE},

		{1, 2218.40, -1076.18, 1050.48, HOUSE_MEDIUM},
		{8, -42.59, 1405.47, 1084.43, HOUSE_LITTLE},
		{15, 328.05, 1477.73, 1084.44, HOUSE_MEDIUM},
		{11, 2283.04, -1140.28, 1050.90, HOUSE_LITTLE},
		{9, 260.85, 1237.24, 1084.26, HOUSE_MEDIUM},
		{6, -68.81, 1351.21, 1080.21, HOUSE_MEDIUM},
		{4, 261.12, 1284.30, 1080.26, HOUSE_MEDIUM},
		{15, 295.04, 1472.26, 1080.26, HOUSE_MEDIUM},
		{2, 2237.59, -1081.64, 1049.02, HOUSE_MEDIUM},
		{8, 2365.31, -1135.60, 1050.88, HOUSE_MEDIUM},
		{15, 387.22, 1471.70, 1080.19, HOUSE_MEDIUM},
		{2, 446.99, 1397.07, 1084.30, HOUSE_MEDIUM},
		{15, 377.15, 1417.41, 1081.33, HOUSE_MEDIUM},
		{6, 2196.85, -1204.25, 1049.02, HOUSE_BUGGED},
		{9, 83.03, 	1322.28, 1083.87, HOUSE_MEDIUM},
		{12, 2324.53, -1149.54, 1050.71, HOUSE_HUGE},
		{7, 225.68, 1021.45, 1084.02, HOUSE_HUGE},
		{1, 2469.0000, -1747.5594, 1012.5000, HOUSE_MEDIUM},
		{1, 2806.9429, -1205.22, 1024.5625, HOUSE_BIG},
		{1, 2423.6149, -1149.3964, 1049.6875, HOUSE_BIG},
		{1, 2364.4937, -1108.4828, 1049.8672, HOUSE_MEDIUM},
		{1, 2282.1797, -1120.1151, 1049.8906, HOUSE_VERY_LITTLE},
		{1, 2259.5250, -1116.6406, 1049.6250, HOUSE_VERY_LITTLE},
		{1, 2166.5541, -1205.1250, 048.0156, HOUSE_MEDIUM},
		{1, 2236.7656, -1051.3905, 1048.0156, HOUSE_MEDIUM},
		{1, 2392.3125, -1077.1747, 1048.0156, HOUSE_MEDIUM},
		{1, 2525.3264, -1692.4098, 1013.7344, HOUSE_BIG},
		{1, -1292.6987,	2000.0592, 1099.9584, HOUSE_MEDIUM},
		{1, -1212.8787, 2000.3761, 1091.9584, HOUSE_MEDIUM},
		{1, -1252.8787, 2000.5, 1091.9584, HOUSE_MEDIUM},
		{4, 300.4356, 310.8415, 1003.3047, HOUSE_MEDIUM},
		{6, 2308.77, -1212.94, 1049.02, HOUSE_LITTLE},
		{9, 306.45, 313.70, 1049.99, HOUSE_MEDIUM},
		{3, 1531.28, -6.80, 1002.09, HOUSE_VERY_LITTLE},
		{1, 2527.654052, -1679.388305, 1015.498596, HOUSE_MEDIUM}
	};

	new List:tempList;
	for(new i = 0, j = sizeof(allHouseInteriors); i < j; ++i)
	{
		new interiorType = allHouseInteriors[i][iType];

		if(map_has_key(HouseInteriors, interiorType))
		{
			tempList = List:map_get(HouseInteriors, interiorType);
		}
		else
		{
			tempList = list_new();
			map_add(HouseInteriors, interiorType, tempList);
		}

		list_add_arr(tempList, allHouseInteriors[i]);
	}
	printf("House Interiors initialized");
	return 1;
}

stock List:HouseInterior_GetListByType(interiorType)
{
	if(!map_has_key(HouseInteriors, interiorType))
		return List:0;
	return List:map_get(HouseInteriors, interiorType);
}

stock GetInteriorTypeName(type)
{
	new l[32];
    switch(type)
    {
	   case HOUSE_VERY_LITTLE: l = "Casa molto piccola";
	   case HOUSE_LITTLE: l = "Casa piccola";
	   case HOUSE_MEDIUM: l = "Casa media";
	   case HOUSE_BIG: l = "Casa grande";
	   case HOUSE_HUGE: l = "Casa enorme";
	   case ROULOTTE_VERY_SMALL: l = "Roulotte molto piccola";
	   case ROULOTTE_SMALL: l = "Roulotte piccola";
	   case ROULOTTE_MEDIUM: l = "Roulotte media";
	   case ROULOTTE_BIG: l = "Roulotte grande";
	   default: l = "Sconosciuto";
    }
    return l;
}