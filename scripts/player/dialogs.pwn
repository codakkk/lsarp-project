Dialog:Dialog_InvSelectAmmo(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    new ammo = strval(inputtext), 
        itemid = GetPVarInt(playerid, "InventorySelect_WeaponItem"),
        ammoType = Weapon_GetAmmoType(itemid);
    if(ammo <= 0 || ammo > Inventory_GetItemAmount(Character_GetInventory(playerid), ammoType))
        return Dialog_Show(playerid, Dialog_InvSelectAmmo, DIALOG_STYLE_INPUT, "{FF0000}Munizioni non valide!\n{FFFFFF}Inserisci le munizioni", "Inserisci le munizioni che vuoi inserire nell'arma.\nDisponibili: %d", "Usa", "Annulla", 
                    Inventory_GetItemAmount(Character_GetInventory(playerid), ammoType));
    AC_GivePlayerWeapon(playerid, itemid, ammo);
    Character_DecreaseItemAmount(playerid, itemid, 1);
    Character_DecreaseItemAmount(playerid, ammoType, ammo);
    return 1;
}
