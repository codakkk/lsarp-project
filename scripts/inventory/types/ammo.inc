#include <YSI_Coding\y_hooks>

hook OnCharacterUseItem(playerid, slotid, itemid)
{
    if(!ServerItem_IsAmmo(itemid))
        return 0;
    new currentWeapon = Character_GetWeapon(playerid);
    if(currentWeapon == 0)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Non hai armi equipaggiate.");
        return 0;
    } 
    if(!Weapon_RequireAmmo(currentWeapon) || Weapon_GetAmmoType(currentWeapon) != itemid)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare queste munizioni su quest'arma.");
        return 0;
    }
    new ammos = Inventory_GetItemAmount(Character_GetInventory(playerid), itemid);
    SetPVarInt(playerid, "InventorySelect_CurrentWeaponItem", currentWeapon);
    Dialog_Show(playerid, Dialog_InvSelectAddAmmo, DIALOG_STYLE_INPUT, "Inserisci le munizioni", "Immetti la quantità di munizioni che vuoi inserire nell'arma.\nQuantità: {00FF00}%d{FFFFFF}.", "Usa", "Annulla", ammos);
    return 1;
}

Dialog:Dialog_InvSelectAddAmmo(playerid, response, listitem, inputtext[])
{
    if(!response)
	{
		DeletePVar(playerid, "InventorySelect_CurrentWeaponItem");
		return 0;
	}
    new ammo = strval(inputtext), 
		weaponid = GetPVarInt(playerid, "InventorySelect_CurrentWeaponItem"),
		ammoType = Weapon_GetAmmoType(weaponid),
		amount = Inventory_GetItemAmount(Character_GetInventory(playerid), ammoType)
	;
    if(ammo <= 0 || ammo > amount)
	   return Dialog_Show(playerid, Dialog_InvSelectAddAmmo, DIALOG_STYLE_INPUT, "Inserisci le munizioni", 
					"{FF0000}Munizioni non valide!{FFFFFF}\nImmetti la quantità di munizioni che vuoi inserire nell'arma.\nQuantità: {00FF00}%d{FFFFFF}", "Usa", "Annulla", 
					amount);
    Character_GiveWeapon(playerid, weaponid, ammo);
    Character_DecreaseItemAmount(playerid, ammoType, ammo);
	
	Character_AMe(playerid, "prende delle munizioni e le inserisce nell'arma.");
	
	Player_InfoStr(playerid, str_format("Hai equipaggiato: ~g~%s~w~~n~con ~g~%d~w~ proiettili.", Weapon_GetName(weaponid), ammo), true);

	Trigger_OnPlayerInvItemUse(playerid, weaponid, 1, ITEM_TYPE_WEAPON);
	Trigger_OnPlayerInvItemUse(playerid, ammoType, ammo, ITEM_TYPE_AMMO);
	
	DeletePVar(playerid, "InventorySelect_CurrentWeaponItem");
    return 1;
}