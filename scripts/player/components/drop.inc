#include <YSI_Coding\y_hooks>

forward bool:Character_CollectDrop(playerid, dropid);

CMD:raccogli(playerid, params[])
{
	new pickupid = Character_GetLastPickup(playerid),
			eID, E_ELEMENT_TYPE:eType;
	if( Pickup_GetInfo(pickupid, eID, eType) && eType == ELEMENT_TYPE_DROP && Drop_IsPlayerInRangeOf(playerid, eID, 2.5))
	{
		Character_CollectDrop(playerid, eID);
	}
	else SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino ad un oggetto da raccogliere.");
	return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(!Character_IsAlive(playerid))
		return Y_HOOKS_CONTINUE_RETURN_1;
	if(PRESSED(KEY_CROUCH))
	{
		new pickupid = Character_GetLastPickup(playerid),
			eID, E_ELEMENT_TYPE:eType;
		if( Pickup_GetInfo(pickupid, eID, eType) && eType == ELEMENT_TYPE_DROP && Drop_IsPlayerInRangeOf(playerid, eID, 2.5))
		{
			Character_CollectDrop(playerid, eID);
		}
	}
    return 1;
}

stock bool:Character_CollectDrop(playerid, dropid)
{
	if(!Character_IsAlive(playerid))
		return false;
	new itemid = Drop_GetItem(dropid);
    if(Drop_IsValid(dropid) && itemid != 0)
    {
	   new 
		  String:title,
		  String:content = @("Raccogli\nSposta");
	   title = str_format("%s (%S: %d)", ServerItem_GetName(itemid), (ServerItem_IsWeapon(itemid) ? @("Munizioni") : @("Quantitï¿½")), (ServerItem_IsWeapon(itemid) ? Drop_GetItemExtra(dropid) : Drop_GetItemAmount(dropid)));
	   if(ServerItem_IsWeapon(itemid))
		  content += @("\n{FFFFFF}Equipaggia{FFFFFF}");
	   SetPVarInt(playerid, "Player_DropID", dropid);
	   Dialog_Show_s(playerid, Dialog_ItemDropAction, DIALOG_STYLE_LIST, title, content, "Continua", "Chiudi");
	   return true;
    }
    return false;
}

Dialog:Dialog_ItemDropAction(playerid, response, listitem, inputtext[])
{
    new 
		dropid = GetPVarInt(playerid, "Player_DropID"),
    	itemid, amount, extra;
    if(!response || !Drop_GetItemData(dropid, itemid, amount, extra) || !Drop_IsPlayerInRangeOf(playerid, dropid, 2.5))
    {
	   DeletePVar(playerid, "Player_DropID");
	   return 0;   
    }
    switch(listitem)
    {
		case 0:
		{
			if(!Character_HasSpaceForItem(playerid, itemid, amount))
				return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario.");
			Character_GiveItem(playerid, itemid, amount, extra);
			SendFormattedMessage(playerid, COLOR_GREEN, "Hai raccolto %s (%d).", ServerItem_GetName(itemid), ServerItem_IsWeapon(itemid) ? extra : amount);
			Character_AMe(playerid, "raccoglie qualcosa.");
			Drop_Destroy(dropid);
		}
		case 1:
		{
			EditObject_Start(playerid, EDIT_TYPE_DROP, Drop_GetObject(dropid), dropid);
			SendClientMessage(playerid, -1, "Stai spostando l'oggetto. Quando hai finito, clicca il {00FF00}Floppy{FFFFFF} per salvare.");
		}
		case 2:
		{
			if(Character_HasWeaponInSlot(playerid, Weapon_GetSlot(itemid)))
				return SendClientMessage(playerid, COLOR_ERROR, "Hai già un'arma equipaggiata.");

			new ammoType = Weapon_GetAmmoType(itemid),
				invAmmo = Inventory_GetItemAmount(Character_GetInventory(playerid), ammoType);
			if(Weapon_RequireAmmo(itemid) && extra <= 0 && invAmmo <= 0)
			{
				return SendClientMessage(playerid, COLOR_ERROR, "L'arma è scarica e non puo' essere equipaggiata.");
				//Dialog_Show(playerid, Dialog_EquipWeaponDrop, DIALOG_STYLE_INPUT, "Munizioni", "{FFFFFF}Inserisci la quantità di munizioni che vuoi disporre all'interno dell'arma.\n{00FF00}Munizioni disponibili: {FFFFFF}%d", "Inserisci", "Annulla", invAmmo);
			}
			if(extra > 0)
			{
				SendFormattedMessage(playerid, COLOR_GREEN, "Hai raccolto %s (%d).", ServerItem_GetName(itemid), extra);
				Character_AMe(playerid, "raccoglie un'arma.");
				AC_GivePlayerWeapon(playerid, itemid, extra);
				Drop_Destroy(dropid);
			}
			else 
			{
				Dialog_Show(playerid, Dialog_ItemDropEquipWeapon, DIALOG_STYLE_INPUT, "Equipaggia Arma", "Inserisci le munizioni che vuoi inserire nell'arma.\nQuantitï¿½: {00FF00}%d{FFFFFF}", "Equipaggia", "Annulla", invAmmo);
			}
		}
	}
	return 1;
}

Dialog:Dialog_ItemDropEquipWeapon(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;
	new dropid = GetPVarInt(playerid, "Player_DropID"), itemid, amount, extra;
	if(!Drop_GetItemData(dropid, itemid, amount, extra) || !ServerItem_IsWeapon(itemid)) // Should never happen, but better check.
		return 0;
	if(Character_HasWeaponInSlot(playerid, Weapon_GetSlot(itemid)))
		return SendClientMessage(playerid, COLOR_ERROR, "Hai già un'arma equipaggiata in questo slot.");
	new ammo = strval(inputtext), 
		ammoType = Weapon_GetAmmoType(itemid),
		invAmmo = Inventory_GetItemAmount(Character_GetInventory(playerid), ammoType);
	if(ammo <= 0 || ammo > invAmmo)
		return Dialog_Show(playerid, Dialog_ItemDropEquipWeapon, DIALOG_STYLE_INPUT, "Equipaggia Arma", "{FF0000}Non possiedi la quantitï¿½ di munizioni inserita.{FFFFFF}\nInserisci le munizioni che vuoi inserire nell'arma.\n{00FF00}%d{FFFFFF}", "Equipaggia", "Annulla", invAmmo);
	Player_InfoStr(playerid, str_format("Hai equipaggiato: ~g~%s~w~~n~con ~g~%d~w~ proiettili.", ServerItem_GetName(itemid), extra + ammo), true);
	Character_AMe(playerid, "raccoglie un'arma.");
	AC_GivePlayerWeapon(playerid, itemid, extra + ammo);
	Character_DecreaseItemAmount(playerid, ammoType, ammo);
	Drop_Destroy(dropid);
	Character_Save(playerid);
	return 1;
}
