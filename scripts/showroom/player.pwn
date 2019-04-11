#include <YSI_Coding\y_hooks>

hook GlobalPlayerSecondTimer(playerid)
{
    new keys, updown, leftright;
    GetPlayerKeys(playerid, keys, updown, leftright);
    if(gBuyingVehicle[playerid])
    {
        if(leftright == KEY_LEFT)
        {
            VehicleColorNum1[playerid]--;
        }
        else if(leftright == KEY_RIGHT)
        {
            VehicleColorNum1[playerid]++;
        }
        else if(keys & KEY_FIRE)
        {
            VehicleColorNum2[playerid]--;
        }
        else if(keys & KEY_HANDBRAKE)
        {
            VehicleColorNum2[playerid]++;
        }
        new maxColor = sizeof(CarColors) - 1;
        if(VehicleColorNum1[playerid] < 0) VehicleColorNum1[playerid] = maxColor;
        if(VehicleColorNum1[playerid] > maxColor) VehicleColorNum1[playerid] = 0;
        if(VehicleColorNum2[playerid] < 0) VehicleColorNum2[playerid] = maxColor;
        if(VehicleColorNum2[playerid] > maxColor) VehicleColorNum2[playerid] = 0;
        ChangeVehicleColor(gBuyingVehicleID[playerid], CarColors[VehicleColorNum1[playerid]], CarColors[VehicleColorNum2[playerid]]);
    }
    return 1;
}

hook OnCharacterPreSaveData(playerid, disconnect)
{
    if(disconnect)
    {
        if(gBuyingVehicle[playerid])
        {
            SetPlayerInterior(playerid, 0);
            SetPlayerVirtualWorld(playerid, 0);
            printf("%d PreDataSave", playerid);
        }
    }
    return 1;
}

hook OnPlayerClearData(playerid)
{
    if(gBuyingVehicle[playerid] && gBuyingVehicleID[playerid])
    {
        Vehicle_Destroy(gBuyingVehicleID[playerid]);
    }
    ClearBuyingVehicleData(playerid);
    return 1;
}

stock ClearBuyingVehicleData(playerid)
{
    gBuyingVehicle[playerid] = 0;
    gCurrentShowRoom[playerid] = 0;
    gBuyingVehicleID[playerid] = 0;
    gBuyingVehiclePrice[playerid] = 0;
    VehicleColorNum1[playerid] = VehicleColorNum2[playerid] = 0;
    return 1;
}

//stock OnPlayerPickUpShowRoomPickup(playerid, showroom_id)
hook OnPlayerPickUpElmPickup(playerid, pickupid, elementId, E_ELEMENT_TYPE:type)
{
    if(type != ELEMENT_TYPE_DEALERSHIP)
        return 1;
    new showroom_id = elementId;
    if(AccountInfo[playerid][aAdmin] > 1)
    {
        new string[64];
        format(string, sizeof(string), "~g~%s~w~ [%d]", ShowRoomInfo[showroom_id][srName], showroom_id);
        GameTextForPlayer(playerid, string, 1000, 3);
    }
    else
    {
        new string[64];
        format(string, sizeof(string), "~g~%s~w~", ShowRoomInfo[showroom_id][srName]);
        GameTextForPlayer(playerid, string, 1000, 3);
    }
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    if(gBuyingVehicle[playerid])
    {
        if(PRESSED(KEY_WALK))
        {
            ShowRoom_PlayerCancelBuy(playerid);
        }
        else if(PRESSED(KEY_SECONDARY_ATTACK))
        {
            ShowRoom_PlayerConfirmBuy(playerid);
        }
    }
    else if(pLastPickup[playerid] != -1 && PRESSED(KEY_WALK))
    {
        new
            eID,
            E_ELEMENT_TYPE:eType,
            Float:x, Float:y, Float:z;

        Pickup_GetInfo(pLastPickup[playerid], eID, eType);
        Pickup_GetPosition(pLastPickup[playerid], x, y, z);

        if(eType == ELEMENT_TYPE_DEALERSHIP && IsPlayerInRangeOfPoint(playerid, 2.5, x, y, z))
        {
            ShowRoom_ShowVehiclesToPlayer(eID, playerid);
        }
        // Show Dialog
    }
    return 1;
}

ShowRoom_ShowVehiclesToPlayer(showroom_id, playerid)
{
    if(!ShowRoomInfo[showroom_id][srID])
        return 0;
    new
        string[1024],
        count = 0;
    
    for(new i = 0; i < MAX_VEHICLES_IN_SHOWROOM; ++i)
    {
        if(!ShowRoomInfo[showroom_id][srModels][i] || !ShowRoomInfo[showroom_id][srPrices][i])
            continue;
        new 
            modelid = ShowRoomInfo[showroom_id][srModels][i],
            price = ShowRoomInfo[showroom_id][srPrices][i]
        ;
        format(string, sizeof(string), "%s%s\t$%d\n", string, GetVehicleNameFromModel(modelid), price);
        gShowRoomItemList[playerid][count] = i;
        count++;
    }
    if(count == 0)
        return 0;
    gCurrentShowRoom[playerid] = showroom_id;
    Dialog_Show(playerid, Dialog_PlayerShowRoom, DIALOG_STYLE_TABLIST_HEADERS, "Concessionaria", "Modello\tPrezzo\n%s", "Acquista", "Chiudi", string);
    return 1;
}

Dialog:Dialog_PlayerShowRoom(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    new
        item = gShowRoomItemList[playerid][listitem],
        showroom_id = gCurrentShowRoom[playerid],
        vehicle_price = ShowRoomInfo[showroom_id][srPrices][item],
        vehicle_model = ShowRoomInfo[showroom_id][srModels][item]
    ;

    if(!IsPlayerInRangeOfPoint(playerid, 5.0, ShowRoomInfo[showroom_id][srX], ShowRoomInfo[showroom_id][srY], ShowRoomInfo[showroom_id][srZ]))
        return SendClientMessage(playerid, COLOR_ERROR, "Ti sei allontanato dalla concessionaria!");

    if(AC_GetPlayerMoney(playerid) < vehicle_price)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi!");
        ShowRoom_ShowVehiclesToPlayer(showroom_id, playerid);
        return 1;
    }
    gBuyingVehicle[playerid] = 1;
    gBuyingVehiclePrice[playerid] = vehicle_price;
    gBuyingVehicleID[playerid] = Vehicle_Create(vehicle_model, ShowRoomInfo[showroom_id][srVehX], ShowRoomInfo[showroom_id][srVehY], ShowRoomInfo[showroom_id][srVehZ], 0, 0, 0, 0, 0);
    
    SetPlayerPos(playerid, ShowRoomInfo[showroom_id][srVehX], ShowRoomInfo[showroom_id][srVehY], ShowRoomInfo[showroom_id][srVehZ]);
    SetPlayerVirtualWorld(playerid, playerid + 1);
    SetVehicleVirtualWorld(gBuyingVehicleID[playerid], playerid + 1);
    PutPlayerInVehicle(playerid, gBuyingVehicleID[playerid], 0);
    TogglePlayerControllable(playerid, 0);

    new Float:x, Float:y;
    GetXYInFrontOfPos(ShowRoomInfo[showroom_id][srVehX], ShowRoomInfo[showroom_id][srVehY], ShowRoomInfo[showroom_id][srVehA] + 45, x, y, 10.0);
    SetPlayerCameraPos(playerid, x, y, ShowRoomInfo[showroom_id][srVehZ] + 2.0);
    SetPlayerCameraLookAt(playerid, ShowRoomInfo[showroom_id][srVehX], ShowRoomInfo[showroom_id][srVehY], ShowRoomInfo[showroom_id][srVehZ]);

    VehicleColorNum1[playerid] = 0;
    VehicleColorNum2[playerid] = 0;

    SendClientMessage(playerid, COLOR_GREEN, "Scegli il colore del veicolo. Usa le freccette o i tasti del mouse per scegliere (tieni premuto).");
    SendClientMessage(playerid, COLOR_GREEN, "Per acquistare il veicolo premi INVIO (oppure /compra).");
    SendClientMessage(playerid, COLOR_GREEN, "Per annullare l'acquisto premi ALT (oppure /annulla).");

    return 1;
}

stock ShowRoom_PlayerConfirmBuy(playerid)
{
    if(!gBuyingVehicle[playerid])
        return 0;
    if(AC_GetPlayerMoney(playerid) < gBuyingVehiclePrice[playerid])
    {
        new showroomid = gCurrentShowRoom[playerid];
        SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi!");
        Vehicle_Destroy(gBuyingVehicleID[playerid]);
        SetPlayerVirtualWorld(playerid, 0);
        SetPlayerPos(playerid, ShowRoomInfo[showroomid][srX], ShowRoomInfo[showroomid][srY], ShowRoomInfo[showroomid][srZ]);
        ClearBuyingVehicleData(playerid);
    }
    else
    {
        SendFormattedMessage(playerid, -1, "Congratulazioni! Hai acquistato questo veicolo ({00AA00}%s{FFFFFF}) per {00FF00}$%d{FFFFFF}!", GetVehicleName(gBuyingVehicleID[playerid]), gBuyingVehiclePrice[playerid]);
        AC_GivePlayerMoney(playerid, -gBuyingVehiclePrice[playerid], "Concessionaria");
        SetVehicleVirtualWorld(gBuyingVehicleID[playerid], 0);
        SetPlayerVirtualWorld(playerid, 0);
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

stock ShowRoom_PlayerCancelBuy(playerid)
{
    new showroom = gCurrentShowRoom[playerid];
    Vehicle_Destroy(gBuyingVehicleID[playerid]);
    SetPlayerPos(playerid, ShowRoomInfo[showroom][srX], ShowRoomInfo[showroom][srY], ShowRoomInfo[showroom][srZ]);
    TogglePlayerControllable(playerid, 1);
    SetPlayerVirtualWorld(playerid, 0);
    SetCameraBehindPlayer(playerid);
    ClearBuyingVehicleData(playerid);
    SendClientMessage(playerid, COLOR_ERROR, "Hai annullato l'acquisto!");
    return 1;
}

