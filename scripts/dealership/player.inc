#include <YSI_Coding\y_hooks>

static 
	CurrentShowRoom[MAX_PLAYERS char],
	VehicleColorNum1[MAX_PLAYERS char],
	VehicleColorNum2[MAX_PLAYERS char]

;

ptask OnPlayerDealerShipTimer[1000](playerid)
{
	if(!Character_IsLogged(playerid) || Character_IsBuyingVehicle(playerid))
		return 0;
    new keys, updown, leftright;
    GetPlayerKeys(playerid, keys, updown, leftright);
	if(leftright == KEY_LEFT)
	{
		VehicleColorNum1{playerid}--;
	}
	else if(leftright == KEY_RIGHT)
	{
		VehicleColorNum1{playerid}++;
	}
	else if(keys & KEY_FIRE)
	{
		VehicleColorNum2{playerid}--;
	}
	else if(keys & KEY_HANDBRAKE)
	{
		VehicleColorNum2{playerid}++;
	}
	new maxColor = sizeof(CarColors) - 1;
	if(VehicleColorNum1{playerid} < 0) VehicleColorNum1{playerid} = maxColor;
	if(VehicleColorNum1{playerid} > maxColor) VehicleColorNum1{playerid} = 0;
	if(VehicleColorNum2{playerid} < 0) VehicleColorNum2{playerid} = maxColor;
	if(VehicleColorNum2{playerid} > maxColor) VehicleColorNum2{playerid} = 0;
	ChangeVehicleColor(gBuyingVehicleID[playerid], CarColors[VehicleColorNum1{playerid}], CarColors[VehicleColorNum2{playerid}]);
    return 1;
}

hook OnCharacterPreSaveData(playerid, disconnect)
{
	if(disconnect)
	{
		if(Character_IsBuyingVehicle(playerid))
		{
			SetPlayerInterior(playerid, 0);
			Character_SetVirtualWorld(playerid, 0);
		}
	}
    return 1;
}

hook OnPlayerClearData(playerid)
{
    if(Character_IsBuyingVehicle(playerid) && gBuyingVehicleID[playerid])
    {
	   Vehicle_Destroy(gBuyingVehicleID[playerid]);
    }
    ClearBuyingVehicleData(playerid);
    return 1;
}

stock ClearBuyingVehicleData(playerid)
{
    Character_SetBuyingVehicle(playerid, false);
    CurrentShowRoom{playerid} = 0;
    gBuyingVehicleID[playerid] = 0;
    gBuyingVehiclePrice[playerid] = 0;
    VehicleColorNum1{playerid} = VehicleColorNum2{playerid} = 0;
    return 1;
}

//stock OnPlayerPickUpShowRoomPickup(playerid, dealership_id)
hook OnPlayerPickUpElmPickup(playerid, pickupid, elementId, E_ELEMENT_TYPE:type)
{
    if(type != ELEMENT_TYPE_DEALERSHIP)
	   return 1;
    new dealership_id = elementId;
    if(AccountInfo[playerid][aAdmin] > 1)
    {
	   new string[64];
	   format(string, sizeof(string), "~g~%s~w~ [%d]", DealershipInfo[dealership_id][dsName], dealership_id);
	   GameTextForPlayer(playerid, string, 1000, 3);
    }
    else
    {
	   new string[64];
	   format(string, sizeof(string), "~g~%s~w~", DealershipInfo[dealership_id][dsName]);
	   GameTextForPlayer(playerid, string, 1000, 3);
    }
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(!Character_IsAlive(playerid))
		return Y_HOOKS_CONTINUE_RETURN_0;
	if(Character_IsBuyingVehicle(playerid))
	{
		if(PRESSED(KEY_WALK))
		{
			Dealership_PlayerCancelBuy(playerid);
		}
		else if(PRESSED(KEY_SECONDARY_ATTACK))
		{
			Dealership_PlayerConfirmBuy(playerid);
		}
	}
	else
	{
		new
			pickupid = Character_GetLastPickup(playerid),
			eID,
			E_ELEMENT_TYPE:eType;
		if(PRESSED(KEY_WALK) && Pickup_GetInfo(pickupid, eID, eType) && eType == ELEMENT_TYPE_DEALERSHIP && IsPlayerInRangeOfPickup(playerid, pickupid, 2.0))
		{
			Dealership_ShowVehiclesToPlayer(eID, playerid);
		}
    }
    return Y_HOOKS_CONTINUE_RETURN_1;
}

stock Dealership_ShowVehiclesToPlayer(dealership_id, playerid)
{
    if(!DealershipInfo[dealership_id][dsID])
	   return 0;
    new
	   string[1024],
	   count = 0;
    
    for(new i = 0; i < MAX_VEHICLES_IN_SHOWROOM; ++i)
    {
	   if(!DealershipInfo[dealership_id][dsModels][i] || !DealershipInfo[dealership_id][dsPrices][i])
		  continue;
	   new 
		  modelid = DealershipInfo[dealership_id][dsModels][i],
		  price = DealershipInfo[dealership_id][dsPrices][i]
	   ;
	   format(string, sizeof(string), "%s%s\t$%d\n", string, Vehicle_GetNameFromModel(modelid), price);
	   gDealershipItemList[playerid][count] = i;
	   count++;
    }
    if(count == 0)
	   return 0;
    CurrentShowRoom{playerid} = dealership_id;
    Dialog_Show(playerid, Dialog_PlayerShowRoom, DIALOG_STYLE_TABLIST_HEADERS, "Concessionaria", "Modello\tPrezzo\n%s", "Acquista", "Chiudi", string);
    return 1;
}

Dialog:Dialog_PlayerShowRoom(playerid, response, listitem, inputtext[])
{
    if(!response)
	   return 0;
    new
	   item = gDealershipItemList[playerid][listitem],
	   dealership_id = CurrentShowRoom{playerid},
	   vehicle_price = DealershipInfo[dealership_id][dsPrices][item],
	   vehicle_model = DealershipInfo[dealership_id][dsModels][item]
    ;

    if(!IsPlayerInRangeOfPoint(playerid, 5.0, DealershipInfo[dealership_id][dsX], DealershipInfo[dealership_id][dsY], DealershipInfo[dealership_id][dsZ]))
	   return SendClientMessage(playerid, COLOR_ERROR, "Ti sei allontanato dalla concessionaria.");

    if(Character_GetMoney(playerid) < vehicle_price)
    {
	   SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi.");
	   Dealership_ShowVehiclesToPlayer(dealership_id, playerid);
	   return 1;
    }
    Character_SetBuyingVehicle(playerid, true);
    gBuyingVehiclePrice[playerid] = vehicle_price;
    gBuyingVehicleID[playerid] = Vehicle_Create(vehicle_model, DealershipInfo[dealership_id][dsVehX], DealershipInfo[dealership_id][dsVehY], DealershipInfo[dealership_id][dsVehZ], 0, 0, 0, 0, 0);
    
    SetPlayerPos(playerid, DealershipInfo[dealership_id][dsVehX], DealershipInfo[dealership_id][dsVehY], DealershipInfo[dealership_id][dsVehZ]);
    Character_SetVirtualWorld(playerid, playerid + 1);
    SetVehicleVirtualWorld(gBuyingVehicleID[playerid], playerid + 1);
    PutPlayerInVehicle(playerid, gBuyingVehicleID[playerid], 0);
    TogglePlayerControllable(playerid, 0);

    new Float:x, Float:y;
    GetXYInFrontOfPos(DealershipInfo[dealership_id][dsVehX], DealershipInfo[dealership_id][dsVehY], DealershipInfo[dealership_id][dsVehA] + 45, x, y, 10.0);
    SetPlayerCameraPos(playerid, x, y, DealershipInfo[dealership_id][dsVehZ] + 2.0);
    SetPlayerCameraLookAt(playerid, DealershipInfo[dealership_id][dsVehX], DealershipInfo[dealership_id][dsVehY], DealershipInfo[dealership_id][dsVehZ]);

    VehicleColorNum1{playerid} = 0;
    VehicleColorNum2{playerid} = 0;

    SendClientMessage(playerid, COLOR_GREEN, "Scegli il colore del veicolo. Usa le freccette o i tasti del mouse per scegliere (tieni premuto).");
    SendClientMessage(playerid, COLOR_GREEN, "Per acquistare il veicolo premi INVIO (oppure /compra).");
    SendClientMessage(playerid, COLOR_GREEN, "Per annullare l'acquisto premi ALT (oppure /annulla).");

    return 1;
}

stock Dealership_PlayerConfirmBuy(playerid)
{
    if(Character_IsBuyingVehicle(playerid))
		return 0;
    if(Character_GetMoney(playerid) < gBuyingVehiclePrice[playerid])
    {
		new dealershipid = CurrentShowRoom{playerid};
		SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi.");
		Vehicle_Destroy(gBuyingVehicleID[playerid]);
		Character_SetVirtualWorld(playerid, 0);
		SetPlayerPos(playerid, DealershipInfo[dealershipid][dsX], DealershipInfo[dealershipid][dsY], DealershipInfo[dealershipid][dsZ]);
		ClearBuyingVehicleData(playerid);
	}
	else
	{
		SendFormattedMessage(playerid, -1, "Congratulazioni! Hai acquistato questo veicolo ({00AA00}%s{FFFFFF}) per {00FF00}$%d{FFFFFF}.", Vehicle_GetName(gBuyingVehicleID[playerid]), gBuyingVehiclePrice[playerid]);
		Character_GiveMoney(playerid, -gBuyingVehiclePrice[playerid], "Concessionaria");
		SetVehicleVirtualWorld(gBuyingVehicleID[playerid], 0);
		Character_SetVirtualWorld(playerid, 0);
		PutPlayerInVehicle(playerid, gBuyingVehicleID[playerid], 0);
		
		Character_AddOwnedVehicle(playerid, gBuyingVehicleID[playerid]);
		Vehicle_SetEngineOff(gBuyingVehicleID[playerid]);
	}
	gBuyingVehicleID[playerid] = 0;
	gBuyingVehiclePrice[playerid] = 0;
	SetCameraBehindPlayer(playerid);
	ClearBuyingVehicleData(playerid);
	TogglePlayerControllable(playerid, 1);
	return 1;
}

stock Dealership_PlayerCancelBuy(playerid)
{
    new showroom = CurrentShowRoom{playerid};
    Vehicle_Destroy(gBuyingVehicleID[playerid]);
    SetPlayerPos(playerid, DealershipInfo[showroom][dsX], DealershipInfo[showroom][dsY], DealershipInfo[showroom][dsZ]);
    TogglePlayerControllable(playerid, 1);
    Character_SetVirtualWorld(playerid, 0);
    SetCameraBehindPlayer(playerid);
    ClearBuyingVehicleData(playerid);
    SendClientMessage(playerid, COLOR_ERROR, "Hai annullato l'acquisto.");
    return 1;
}