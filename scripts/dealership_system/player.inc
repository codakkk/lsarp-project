#include <YSI_Coding\y_hooks>

static 
	SelectedVehicle[MAX_PLAYERS],
	SelectedVehiclePrice[MAX_PLAYERS],
	CurrentDealership[MAX_PLAYERS char],
	VehicleColorNum1[MAX_PLAYERS char],
	VehicleColorNum2[MAX_PLAYERS char]
;

ptask OnPlayerDealershipTimer[500](playerid)
{
	if(!Character_IsLogged(playerid) || !Character_IsBuyingVehicle(playerid))
		return 0;
	if(!Player_HasHotKeysEnabled(playerid))
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
	Vehicle_SetColors(SelectedVehicle[playerid], Dealership_GetColorByIndex(VehicleColorNum1{playerid}), Dealership_GetColorByIndex(VehicleColorNum2{playerid}));
    return 1;
}

hook OnCharacterDisconnected(playerid, reason)
{
	if(Character_IsBuyingVehicle(playerid))
	{
		SetPlayerInterior(playerid, 0);
		Character_SetVirtualWorld(playerid, 0);
	}
	return 1;
}

hook OnCharacterClearData(playerid)
{
    if(Character_IsBuyingVehicle(playerid) && IsValidVehicle(SelectedVehicle[playerid]))
    {
	   Vehicle_Destroy(SelectedVehicle[playerid]);
    }
    ClearBuyingVehicleData(playerid);
    return 1;
}

stock ClearBuyingVehicleData(playerid)
{
    Character_SetBuyingVehicle(playerid, false);
    CurrentDealership{playerid} = 0;
    SelectedVehicle[playerid] = 0;
    SelectedVehiclePrice[playerid] = 0;
    VehicleColorNum1{playerid} = VehicleColorNum2{playerid} = 0;
    return 1;
}

//stock OnPlayerPickUpShowRoomPickup(playerid, dealership_id)
hook OnPlayerPickUpElmPickup(playerid, pickupid, elementId, E_ELEMENT_TYPE:type)
{
    if(type != ELEMENT_TYPE_DEALERSHIP)
	   return 1;
    new 
		dealership_id = elementId,
		String:str;
    
	if(Account_GetAdminLevel(playerid) > 1 && Player_IsAdminDuty(playerid))
    {
	   str += str_format("~r~ID: %d~w~~n~", dealership_id);
    }
    new name[32];
	Dealership_GetName(dealership_id, name);
	str += str_format("~g~%s~w~~n~", name);

	if(Player_HasHotKeysEnabled(playerid))
		str += @("Premi ~b~~k~~SNEAK_ABOUT~~w~ per il menu.");
	else
		str += @("Digita ~b~/compra~w~ per il menu.");
	
	GameTextForPlayerStr(playerid, str, 3000, 3);
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(!Character_IsAlive(playerid))
		return Y_HOOKS_CONTINUE_RETURN_0;
	if(!Player_HasHotKeysEnabled(playerid))
		return 0;
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
			eID,
			E_ELEMENT_TYPE:eType;
		if(Player_HasHotKeysEnabled(playerid) && PRESSED(KEY_WALK) && Character_GetNearestPickup(playerid, eID, eType) && eType == ELEMENT_TYPE_DEALERSHIP)
		{
			Dealership_ShowVehiclesToPlayer(eID, playerid);
		}
    }
    return Y_HOOKS_CONTINUE_RETURN_1;
}

stock Dealership_ShowVehiclesToPlayer(dealershipid, playerid)
{
    if(!Dealership_IsValid(dealershipid))
	   return 0;

	if(Dealership_IsOnlyPremium(dealershipid) && Account_GetPremiumLevel(playerid) <= 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Devi essere un membro premium per accedere a questa concessionaria.");
		
    new
	   String:string = @("Modello\tPrezzo\n"),
	   List:vehicles = Dealership_GetVehiclesList(dealershipid);

    if(list_size(vehicles) <= 0)
	   return SendClientMessage(playerid, COLOR_ERROR, "Non ci sono veicoli acquistabili.");
    
	for(new i = 0, j = list_size(vehicles); i < j; ++i)
    {
		string += str_format("%s\t$%d\n", Vehicle_GetNameFromModel(Dealership_GetModelAt(dealershipid, i)), Dealership_GetPriceAt(dealershipid, i));
	   //format(string, sizeof(string), "%s%d(0.0, 0.0, 0.0, 1.0, 3, 3)\t%s - ~g~~h~$%d\n", string, modelid, Vehicle_GetNameFromModel(modelid), price);
	   //string += str_format("%d(0.0, 0.0, 0.0, 1.0, 1.0)\t%s~n~~g~~h~$%d\n", modelid, Vehicle_GetNameFromModel(modelid), price);
    }
    CurrentDealership{playerid} = dealershipid;

	new name[32];
	Dealership_GetName(dealershipid, name);

   	Dialog_Show_s(playerid, Dialog_DealershipVehicles, DIALOG_STYLE_TABLIST_HEADERS, @(name), string, "Acquista", "Chiudi");
	//ShowPlayerDialog(playerid, Dialog_DealershipVehicles, DIALOG_STYLE_TABLIST_HEADERS, "Concessionaria", string, "Acquista", "Chiudi");
    //Dialog_ShowPreview(playerid, Dialog_DealershipVehicles, "Concessionaria", string, "Acquista", "Chiudi");
	return 1;
}

Dialog:Dialog_DealershipVehicles(playerid, response, listitem, inputtext[])
{
    if(!response)
	   return 0;
    new
	   	dealershipid = CurrentDealership{playerid},
		vehicleModel = Dealership_GetModelAt(dealershipid, listitem),
	   	vehiclePrice = Dealership_GetPriceAt(dealershipid, listitem)
    ;



    if(!Character_IsNearDealership(playerid, dealershipid))
	   return SendClientMessage(playerid, COLOR_ERROR, "Ti sei allontanato dalla concessionaria.");

	if(Character_GetOwnedVehicleCount(playerid) >= Account_GetOwnableVehicleCount(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Possiedi già il massimo numero di veicoli acquistabili. Acquista un pacchetto premium per aumentarlo.");

    if(Character_GetMoney(playerid) < vehiclePrice)
    {
		SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi.");
		Dealership_ShowVehiclesToPlayer(dealershipid, playerid);
		return 1;
    }

    Character_SetBuyingVehicle(playerid, true);
	Character_SetFreezed(playerid, true);
    
	SelectedVehiclePrice[playerid] = vehiclePrice;

	new Float:vx, Float:vy, Float:vz, Float:va;
	Dealership_GetVehicleSpawnPos(dealershipid, vx, vy, vz, va);

    SelectedVehicle[playerid] = Vehicle_Create(vehicleModel, vx, vy, vz, va, 0, 0, 0, 0);
    
    PutPlayerInVehicle(playerid, SelectedVehicle[playerid], 0);

	Character_SetVirtualWorld(playerid, playerid + 1);
    SetVehicleVirtualWorld(SelectedVehicle[playerid], playerid + 1);


	SetCameraBehindPlayer(playerid);

    VehicleColorNum1{playerid} = 0;
    VehicleColorNum2{playerid} = 0;

	if(Player_HasHotKeysEnabled(playerid))
	{
		SendClientMessage(playerid, COLOR_GREEN, "Scegli il colore del veicolo. Usa le freccette o i tasti del mouse per scegliere (tieni premuto).");
		SendClientMessage(playerid, COLOR_GREEN, "Per acquistare il veicolo premi INVIO (oppure /compra).");
		SendClientMessage(playerid, COLOR_GREEN, "Per annullare l'acquisto premi ALT (oppure /annulla).");
	}
	else
	{
		SendClientMessage(playerid, COLOR_GREEN, "Scegli il colore del veicolo. Usa /colore <colore 1> <colore 2>");
		SendClientMessage(playerid, COLOR_GREEN, "Per acquistare il veicolo digita /compra.");
		SendClientMessage(playerid, COLOR_GREEN, "Per annullare l'acquisto digita /annulla.");
	}
    return 1;
}

stock Dealership_PlayerConfirmBuy(playerid)
{
    if(!Character_IsBuyingVehicle(playerid))
		return 0;
    if(Character_GetMoney(playerid) < SelectedVehiclePrice[playerid])
    {
		new dealershipid = CurrentDealership{playerid};
		SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi.");
		Vehicle_Destroy(SelectedVehicle[playerid]);
		Character_SetVirtualWorld(playerid, 0);

		new Float:x, Float:y, Float:z;
		Dealership_GetPosition(dealershipid, x, y, z);

		SetPlayerPos(playerid, x, y, z);
		
		ClearBuyingVehicleData(playerid);
	}
	else
	{
		if(Character_AddOwnedVehicle(playerid, SelectedVehicle[playerid]))
		{
			SendFormattedMessage(playerid, -1, "Congratulazioni. Hai acquistato questo veicolo ({00AA00}%s{FFFFFF}) per {00FF00}$%d{FFFFFF}.", Vehicle_GetName(SelectedVehicle[playerid]), SelectedVehiclePrice[playerid]);
			Character_GiveMoney(playerid, -SelectedVehiclePrice[playerid], "Concessionaria");
			SetVehicleVirtualWorld(SelectedVehicle[playerid], 0);
			Character_SetVirtualWorld(playerid, 0);
			PutPlayerInVehicle(playerid, SelectedVehicle[playerid], 0);
			
			Vehicle_SetPrice(SelectedVehicle[playerid], SelectedVehiclePrice[playerid]);

			Vehicle_SetEngineStatus(SelectedVehicle[playerid], false);
		}
		else
			SendClientMessage(playerid, COLOR_ORANGE, "Hai raggiunto il limite massimo di veicoli acquistabili.");
	}
	SelectedVehicle[playerid] = 0;
	SelectedVehiclePrice[playerid] = 0;
	SetCameraBehindPlayer(playerid);
	ClearBuyingVehicleData(playerid);
	Character_SetFreezed(playerid, false);
	return 1;
}

stock Dealership_PlayerCancelBuy(playerid)
{
    new dealershipid = CurrentDealership{playerid};
    Vehicle_Destroy(SelectedVehicle[playerid]);

    new Float:x, Float:y, Float:z;
	Dealership_GetPosition(dealershipid, x, y, z);
	SetPlayerPos(playerid, x, y, z);

    Character_SetFreezed(playerid, false);
    Character_SetVirtualWorld(playerid, 0);
    SetCameraBehindPlayer(playerid);
    ClearBuyingVehicleData(playerid);
    SendClientMessage(playerid, COLOR_ERROR, "Hai annullato l'acquisto.");
    return 1;
}

static Character_IsNearDealership(playerid, dealershipid)
{
	if(!Dealership_IsValid(dealershipid) || Character_GetVirtualWorld(playerid) != Dealership_GetVirtualWorld(dealershipid))
		return 0;
	new Float:x, Float:y, Float:z;
	Dealership_GetPosition(dealershipid, x, y, z);
	return IsPlayerInRangeOfPoint(playerid, 5.0, x, y, z);
}