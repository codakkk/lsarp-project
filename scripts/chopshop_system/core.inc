#include <YSI_Coding\y_hooks>

/*
#define CHOPSHOP_X						(2771.7817)
#define CHOPSHOP_Y						(-1606.4154)
#define CHOPSHOP_Z						(10.9219)
*/

#define CHOPSHOP_PLAYER_TIME            (60 * 1) // Delay between each sold car to chopshop. (In minutes)
#define CHOPSHOP_VEHICLE_PERCENTAGE		(30)

static PlayerLastChopShopTime[MAX_PLAYERS];

hook OnGameModeInit()
{
    Pickup_Create(1239, 0, CHOPSHOP_X, CHOPSHOP_Y, CHOPSHOP_Z, ELEMENT_TYPE_CHOPSHOP, 0, 0);
    CreateDynamic3DTextLabel("ChopShop", COLOR_GREY, CHOPSHOP_X, CHOPSHOP_Y, CHOPSHOP_Z + 0.55, 20.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, 0, 0);
    return 1;
}

hook OnCharacterClearData(playerid)
{
	PlayerLastChopShopTime[playerid] = 0;
	return 1;
}

hook OnCharacterLoad(playerid)
{
	inline OnLoad()
	{
		cache_get_value_index_int(0, 0, PlayerLastChopShopTime[playerid]);
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT last_chop_shop_time FROM `characters` WHERE id = '%d'", Character_GetID(playerid));
	return 1;
}

stock Vehicle_CanBeSoldToChopShop(vehicleid)
{
	return ( (gettime() - Vehicle_GetLastChopShopTime(vehicleid)) > 60 * CHOPSHOP_PLAYER_TIME );
}

stock ChopShop_CanCharacterUse(playerid)
{
	return ( (gettime() - Character_GetLastChopShopTime(playerid)) > 60 * CHOPSHOP_PLAYER_TIME );
}

stock Vehicle_GetChopShopPrice(vehicleid)
{
	return ( (Vehicle_GetPrice(vehicleid) / 100) * CHOPSHOP_VEHICLE_PERCENTAGE );
}

stock ChopShop_Sell(playerid, vehicleid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	
	if(!Character_HasChopShopPrivileges(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai i permessi necessari per utilizzare questo comando.");

	if(!Vehicle_CanBeSoldToChopShop(vehicleid))
	{
		SendClientMessage(playerid, COLOR_ERROR, "Questo veicolo è stato già venduto recentemente.");
        return 0;
	}

    if(!ChopShop_CanCharacterUse(playerid))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Puoi utilizzare questo comando una volta ogni 6 ore.");
        return 0;
    }

    if(GetPlayerVehicleID(playerid) != vehicleid)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Non sei più su un veicolo.");
        return 0;
    }

    new 
        price = Vehicle_GetChopShopPrice(vehicleid);
    
	SendFormattedMessage(playerid, COLOR_GREEN, "Hai venduto questo veicolo (%s) per $%d.", Vehicle_GetName(vehicleid), price);

	Character_GiveMoney(playerid, price, "ChopShop");

    Character_SetLastChopShopTime(playerid, gettime());
	Vehicle_SetLastChopShopTime(vehicleid, gettime());

	new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	SetPlayerPos(playerid, x, y, z);
	//RemovePlayerFromVehicle(playerid);
	ChopShop_RespawnVehicle(vehicleid);
    return 1;
}

stock ChopShop_RespawnVehicle(vehicleid)
{
	Vehicle_SetDestroyed(vehicleid, true);
	SetVehicleToRespawn(vehicleid);
	return 1;
}

stock Character_GetLastChopShopTime(playerid)
{
	return PlayerLastChopShopTime[playerid];
}

stock Character_SetLastChopShopTime(playerid, time)
{
	if(!Character_IsLogged(playerid))
		return 0;
	PlayerLastChopShopTime[playerid] = time;
	mysql_tquery_f(gMySQL, "UPDATE `characters` SET last_chop_shop_time = '%d' WHERE id = '%d'", time, Character_GetID(playerid));
	return 1;
}