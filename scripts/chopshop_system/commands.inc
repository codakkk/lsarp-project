flags:chopshop(CMD_USER);
CMD:chopshop(playerid, params[])
{
	if(!IsPlayerInRangeOfPoint(playerid, 10.0, CHOPSHOP_X, CHOPSHOP_Y, CHOPSHOP_Z))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei nei pressi di un Chopshop.");

	if(!Character_HasChopShopPrivileges(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai i permessi necessari per poter utilizzare questo comando.");

	new vehicle_id = GetPlayerVehicleID(playerid);
	if(vehicle_id == 0 || vehicle_id == INVALID_VEHICLE_ID)
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei a bordo di un veicolo.");
	
	//if(VehicleInfo[vehicle_id][vFaction] != INVALID_VEHICLE_ID)
		//return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su un veicolo di fazione.");

	if(Vehicle_GetOwnerID(vehicle_id) == Character_GetID(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su un tuo veicolo.");

	new Float:hp;
	GetVehicleHealth(vehicle_id, hp);
	if(hp < 500.0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su un veicolo con meno di 500HP.");


	if(!Vehicle_CanBeSoldToChopShop(vehicle_id))
	{
		SendClientMessage(playerid, COLOR_ERROR, "Questo veicolo è stato già venduto recentemente.");
        return 0;
	}

	if(!ChopShop_CanCharacterUse(playerid))
	{
		new time = gettime() - Character_GetLastChopShopTime(playerid);
		SendFormattedMessage(playerid, COLOR_ERROR, "Puoi riutilizzare questo comando tra %d minuti.", (CHOPSHOP_PLAYER_TIME*60 - time)/60);
		return 0;
	}

	SetPVarInt(playerid, "ChopShop_VehicleID", vehicle_id);

	new 
		price = Vehicle_GetChopShopPrice(vehicle_id);

	Dialog_Show(playerid, Dialog_ChopShopSell, DIALOG_STYLE_MSGBOX, "Chop Shop", "Sei sicuro di voler vendere questa %s per $%d?", "Vendi", "Annulla", Vehicle_GetName(vehicle_id), price);
	return 1;
}

Dialog:Dialog_ChopShopSell(playerid, response, listitem, inputtext[])
{
	if(!response)
	{
		DeletePVar(playerid, "ChopShop_VehicleID");
		return 1;
	}
	
	new 
		vehicle_id = GetPVarInt(playerid, "ChopShop_VehicleID");

	ChopShop_Sell(playerid, vehicle_id);
	DeletePVar(playerid, "ChopShop_VehicleID");
	return 1;
}