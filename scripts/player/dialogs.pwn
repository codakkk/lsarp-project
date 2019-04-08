Dialog:Dialog_InvSelectAmmo(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    new ammo = strval(inputtext), 
        itemid = GetPVarInt(playerid, "InventorySelect_WeaponItem"),
        ammoType = Weapon_GetAmmoType(itemid);
    if(ammo <= 0 || ammo > Inventory_GetItemAmount(Character_GetInventory(playerid), ammoType))
        return Dialog_Show(playerid, Dialog_InvSelectAmmo, DIALOG_STYLE_INPUT, "{FF0000}Munizioni non valide!\n{FFFFFF}Inserisci le munizioni", "Immetti la quantità di munizioni che vuoi inserire nell'arma.\n{00FF00}Quantità: %d{FFFFFF}", "Usa", "Annulla", 
                    Inventory_GetItemAmount(Character_GetInventory(playerid), ammoType));
    AC_GivePlayerWeapon(playerid, itemid, ammo);
    Character_DecreaseItemAmount(playerid, itemid, 1);
    Character_DecreaseItemAmount(playerid, ammoType, ammo);
    return 1;
}

Dialog:Dialog_House(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    new pickupid = pLastPickup[playerid];
    if(pickupid == -1 || !IsPlayerInRangeOfPickup(playerid, pickupid, 2.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata o all'uscita della casa.");
    new 
        elementId, 
        E_ELEMENT_TYPE:type;
    Pickup_GetInfo(pickupid, elementId, type);
    if(type != ELEMENT_TYPE_HOUSE_ENTRANCE && type != ELEMENT_TYPE_HOUSE_EXIT)
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata o all'uscita della casa.");
    new houseid = elementId;
    switch(listitem)
    {
        case 0: // Apri/Chiudi Porta
        {
            House_SetLocked(houseid, !House_IsLocked(houseid));
            if(House_IsLocked(houseid))
                SendClientMessage(playerid, COLOR_GREEN, "Hai chiuso la porta della casa!");
            else
                SendClientMessage(playerid, COLOR_GREEN, "Hai aperto la porta della casa!");
            House_Save(houseid);
        }
        case 1: // Show Inv
        {
            House_ShowInventory(houseid, playerid);
        }
        case 2: // Deposita Soldi
        {
            return Dialog_Show(playerid, Dialog_HouseDeposit, DIALOG_STYLE_INPUT, "Deposita soldi", "Inserisci l'ammontare di soldi che vuoi depositare in casa.", "Deposita", "Annulla");
        }
        case 3: // Ritira Soldi
        {
            return Dialog_Show(playerid, Dialog_HouseWithdraw, DIALOG_STYLE_INPUT, "Ritira soldi", "Inserisci l'ammontare di soldi che vuoi ritirare dalla casa.\n{00FF00}Cassa: $%d{FFFFFF}", "Ritira", "Annulla", House_GetMoney(houseid));
        }
        case 4: // Vendi 
        {
            return Dialog_Show(playerid, Dialog_HouseSell, DIALOG_STYLE_MSGBOX, "Vendi", "Sei sicuro di voler vendere la tua casa per {00FF00}$%d{FFFFFF}", "Vendi", "Annulla", House_GetPrice(houseid));
        }
        case 5: // Vendi a giocatore
        {
            SendClientMessage(playerid, COLOR_ERROR, "Non ancora disponibile!");
            return 1;
        }
        case 6: // Cambia interior
        {
            new String:string;
            for(new i = 0, sz = sizeof(allHouseInteriors); i < sz; ++i)
            {
                string += str_format("Interno %d\n", i);
            }
            return Dialog_Show_s(playerid, Dialog_HouseInterior, DIALOG_STYLE_MSGBOX, @("Cambio Interior"), string, "Vendi", "Annulla");
        }
    }
    return 1;
}

Dialog:Dialog_HouseDeposit(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    new amount = strval(inputtext);
    if(amount <= 0 || amount > AC_GetPlayerMoney(playerid))
        return Dialog_Show(playerid, Dialog_HouseDeposit, DIALOG_STYLE_INPUT, "Deposita soldi", "{FF0000}Ammontare non valido!\n{FFFFFF}Inserisci l'ammontare di soldi che vuoi depositare in casa.", "Deposita", "Annulla");
    new houseid = Character_GetHouseKey(playerid);
    House_GiveMoney(houseid, amount);
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai depositato $%d all'interno della tua casa.", amount);
    SendFormattedMessage(playerid, COLOR_GREEN, "Attuale: $%d", House_GetMoney(houseid));
    return 1;
}

Dialog:Dialog_HouseWithdraw(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    new amount = strval(inputtext),
        houseid = Character_GetHouseKey(playerid);
    if(amount <= 0 || amount > House_GetMoney(houseid))
        return Dialog_Show(playerid, Dialog_HouseWithdraw, DIALOG_STYLE_INPUT, "Ritira soldi", "{FF0000}Ammontare non valido!\n{FFFFFF}Inserisci l'ammontare di soldi che vuoi ritirare dalla casa.\n{00FF00}Cassa: $%d{FFFFFF}", "Ritira", "Annulla", House_GetMoney(houseid));
    House_GiveMoney(houseid, -amount);
    AC_GivePlayerMoney(playerid, amount, "Dialog_HouseWithdraw");
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai ritirato $%d dalla tua casa.", amount);
    SendFormattedMessage(playerid, COLOR_GREEN, "Attuale: $%d", House_GetMoney(houseid));
    return 1;
}

Dialog:Dialog_HouseSell(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    new houseid = Character_GetHouseKey(playerid);
    if(houseid == 0)
        return 0; // Bug?
    new sellPrice = House_GetPrice(houseid) / 2;
    AC_GivePlayerMoney(playerid, sellPrice, "Dialog_HouseSell");
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai venduto la tua casa per $%d.", sellPrice);

    House_ResetOwner(houseid);
    House_Save(houseid);
    return 1;
}

Dialog:Dialog_HouseInterior(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    new houseid = Character_GetHouseKey(playerid);
    SendFormattedMessage(playerid, COLOR_GREEN, "Hai cambiato l'interno della tua casa. Nuovo interno: %d.", listitem);
    House_SetInterior(houseid, listitem);
    House_Save(houseid);
    return 1;
}
