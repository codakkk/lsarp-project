#include <YSI_Coding\y_hooks>

flags:invmode(CMD_USER);
CMD:invmode(playerid, params[])
{
	Bit_Set(gPlayerBitArray[e_pInvMode], playerid, !Bit_Get(gPlayerBitArray[e_pInvMode], playerid));
    if(Bit_Get(gPlayerBitArray[e_pInvMode], playerid))
    {
        SendClientMessage(playerid, COLOR_GREEN, "Hai impostato l'inventario in chat mode.");
    }
    else
    {
        SendClientMessage(playerid, COLOR_GREEN, "Ora l'inventario è mostrato a dialogs!");
    }
	Dialog_Close(playerid);
	return 1;
}

flags:inventario(CMD_USER);
CMD:inventario(playerid, params[])
{
	if(Bit_Get(gPlayerBitArray[e_pInvMode], playerid))
	{
		new Inventory:playerInventory = Character_GetInventory(playerid);
		SendFormattedMessage(playerid, COLOR_GREEN, "_______________[Inventario (%d/%d)]_______________", Inventory_GetUsedSpace(playerInventory), Inventory_GetSpace(playerInventory));

		if(Inventory_IsEmpty(playerInventory))
			return SendClientMessage(playerid, COLOR_GREEN, "L'inventario è vuoto!");

		new tempItem[E_ITEM_DATA], slotid = -1;
		for_inventory(i : playerInventory)
		{
			slotid++;
			iter_get_arr(i, tempItem);
			if(iter_sizeof(i) == 0 || tempItem[gInvItem] == 0) // If no item
				continue;
			new itemid = tempItem[gInvItem],
				itemAmount = tempItem[gInvAmount],
				extra = tempItem[gInvExtra];
			
			new String:s = str_format("Slot {0080FF}%d{FFFFFF} - {FFFFFF}%s{FFFFFF} ({FFFFFF}%d{FFFFFF})", slotid, ServerItem_GetName(itemid), itemAmount);
			
			if(ServerItem_GetType(itemid) == ITEM_TYPE:ITEM_TYPE_WEAPON)
				s += @(" - ") + str_val(extra);
			
			SendClientMessageStr(playerid, -1, s);
		}
		SendClientMessage(playerid, COLOR_GREEN, "__________________________________________");
	}
    else Character_ShowInventory(playerid, playerid);
    return 1;
}
alias:inventario("inv");

flags:usa(CMD_USER);
CMD:usa(playerid, params[])
{
	new slotid;
	if(sscanf(params, "d", slotid))
		return SendClientMessage(playerid, COLOR_ERROR, "/usa <slotid>");
	new itemid = Character_GetSlotItem(playerid, slotid);
	if(itemid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Slot non valida!");
	Trigger_OnPlayerInvItemUse(playerid, slotid, itemid, ServerItem_GetType(itemid));
	return 1;
}

flags:deposita(CMD_USER);
CMD:deposita(playerid, params[])
{
	new itemid = GetPlayerWeapon(playerid);
	if(itemid == 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando senza un'arma");
	if(!Character_HasSpaceForItem(playerid, itemid, 1))
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario!");
	Character_GiveItem(playerid, itemid, 1, AC_GetPlayerAmmo(playerid));
	AC_RemovePlayerWeapon(playerid, itemid);
	SendClientMessage(playerid, COLOR_GREEN, "Hai depositato la tua arma nell'inventaario!");
	return 1;
}
alias:deposita("dep");

flags:disassembla(CMD_USER);
CMD:disassembla(playerid, params[])
{
    new 
        itemid = GetPlayerWeapon(playerid),
        ammos = AC_GetPlayerAmmo(playerid);
    if(itemid != 0)
    {
        if(!Character_HasSpaceForWeapon(playerid, itemid, ammos))
        {
            return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario!");
        }
        Character_GiveItem(playerid, itemid, 1);
        Character_GiveItem(playerid, Weapon_GetAmmoType(itemid), ammos);
        AC_RemovePlayerWeapon(playerid, itemid);
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai disassemblato la tua arma (%s). Munizioni: %d", ServerItem_GetName(itemid), ammos);
    }
    else
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando senza un'arma!");
    }
    return 1;
}
alias:disassembla("dis");

flags:gettaarma(CMD_USER);
CMD:gettaarma(playerid, params[])
{
    new 
        weaponid = GetPlayerWeapon(playerid), 
        ammo = GetPlayerAmmo(playerid);
    if(weaponid == 0 || ammo == 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un'arma da gettare.");
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    Drop_Create(x, y, z - 0.9, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), weaponid, 1, ammo, Character_GetOOCNameStr(playerid));
    RemovePlayerWeapon(playerid, weaponid);
    SendClientMessage(playerid, COLOR_GREEN, "Hai gettato l'arma.");
    ApplyAnimation(playerid, "GRENADE", "WEAPON_throwu", 3.0, 0, 0, 0, 0, 0);
    return 1;
}