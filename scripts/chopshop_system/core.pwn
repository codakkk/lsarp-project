#include <YSI_Coding\y_hooks>

#define CHOPSHOP_X						(2771.7817)
#define CHOPSHOP_Y						(-1606.4154)
#define CHOPSHOP_Z						(10.9219)

#define CHOPSHOP_PLAYER_TIME            (60 * 1) // Delay between each sold car to chopshop. (In minutes)
#define CHOPSHOP_VEHICLE_PERCENTAGE		(30)
hook OnGameModeInit()
{
    Pickup_Create(1239, 0, CHOPSHOP_X, CHOPSHOP_Y, CHOPSHOP_Z, ELEMENT_TYPE_CHOPSHOP, 0, 0);
    CreateDynamic3DTextLabel("ChopShop", COLOR_GREY, CHOPSHOP_X, CHOPSHOP_Y, CHOPSHOP_Z + 0.55, 20.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, 0, 0);
    return 1;
}

stock Vehicle_CanBeSoldToChopShop(vehicleid)
{
	return ( (gettime() - Vehicle_GetLastChopShopTime(vehicleid)) > 60 * CHOPSHOP_PLAYER_TIME );
}

stock Character_CanUseChopShop(playerid)
{
	return ( (gettime() - Character_GetLastChopShopTime(playerid)) > 60 * CHOPSHOP_PLAYER_TIME );
}

stock Vehicle_GetChopShopPrice(vehicleid)
{
	return ( (Dealership_GetModelPrice(GetVehicleModel(vehicleid)) / 100) * CHOPSHOP_VEHICLE_PERCENTAGE );
}

stock ChopShop_Sell(playerid, vehicleid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	
	if(!Vehicle_CanBeSoldToChopShop(vehicleid))
	{
		SendClientMessage(playerid, COLOR_ERROR, "Questo veicolo è stato già venduto recentemente.");
        return 0;
	}

    if(!Character_CanUseChopShop(playerid))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Puoi utilizzare questo comando una volta ogni 6 ore.");
        return 0;
    }

    if(GetPlayerVehicleID(playerid) != vehicleid)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Non sei più su questo veicolo.");
        return 0;
    }

    new 
        price = Vehicle_GetChopShopPrice(vehicleid);
    
	SendFormattedMessage(playerid, COLOR_GREEN, "Hai venduto questo veicolo (%s) per $%d.", Vehicle_GetName(vehicleid), price);

	Character_GiveMoney(playerid, price, "ChopShop");

    Character_SetLastChopShopTime(playerid, gettime());
	Vehicle_SetLastChopShopTime(vehicleid, gettime());

    Character_Save(playerid);
	Vehicle_Save(vehicleid); // Remember to update the code for pLastChopShopTime
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