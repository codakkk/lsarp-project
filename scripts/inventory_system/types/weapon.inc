#include <YSI_Coding\y_hooks>

static SelectedSlot[MAX_PLAYERS];

hook OnCharacterUseItem(playerid, slotid, itemid)
{
    if(!ServerItem_IsWeapon(itemid))
        return 0;
    new 
        amount = Character_GetSlotAmount(playerid, slotid),
        extra = Character_GetSlotExtra(playerid, slotid);
    new 
        weaponSlot = Weapon_GetSlot(itemid), 
        weapon, 
        ammo;
    GetPlayerWeaponData(playerid, weaponSlot, weapon, ammo);
    if(Character_HasWeaponInSlot(playerid, weaponSlot) && !Weapon_IsGrenade(weapon))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Hai già un'arma equipaggiata in questo slot.");	
        return 0;
    }
    if(Weapon_RequireAmmo(itemid))
    {
        if(extra > 0)
        {
            Character_GiveWeapon(playerid, itemid, extra);
            //Character_DecreaseItemAmount(playerid, itemid, 1);
            Character_DecreaseSlotAmount(playerid, slotid, 1);
            Player_InfoStr(playerid, str_format("Hai equipaggiato: ~g~%s~w~~n~con ~g~%d~w~ proiettili.", Weapon_GetName(itemid), extra), true);
            //Character_AMe(playerid, "prende un'arma dall'inventario.");
        }
        else
        {
            if(!Character_HasItem(playerid, Weapon_GetAmmoType(itemid), 1))
                SendClientMessage(playerid, COLOR_ERROR, "Non hai le munizioni necessarie.");
            else
            {
                SetPVarInt(playerid, "InventorySelect_WeaponItem", itemid);
                SelectedSlot[playerid] = slotid;
                new ammos = Character_GetItemAmount(playerid, Weapon_GetAmmoType(itemid));
                Dialog_Show(playerid, Dialog_InvSelectAmmo, DIALOG_STYLE_INPUT, "Inserisci le munizioni", "Immetti la quantità di munizioni che vuoi inserire nell'arma.\nQuantità: {00FF00}%d{FFFFFF}.", "Usa", "Annulla", ammos);
            }
        }
    }
    else
    {
        Character_GiveWeapon(playerid, itemid, amount);
        Player_InfoStr(playerid, str_format("Hai equipaggiato: ~g~%s~w~", Weapon_GetName(itemid)), true);
        //Character_AMe(playerid, "prende un'arma dall'inventario.");
        Character_DecreaseSlotAmount(playerid, slotid, amount);
        Trigger_OnPlayerInvItemUse(playerid, itemid, amount, ITEM_TYPE_WEAPON);
    }   
    return 1;
}

Dialog:Dialog_InvSelectAmmo(playerid, response, listitem, inputtext[])
{
    if(!response)
	   return 0;
    new slotid = SelectedSlot[playerid],
        itemid = Character_GetSlotItem(playerid, slotid);
    
    if(!ServerItem_IsWeapon(itemid))
        return 0;
    
    new ammo = strval(inputtext), 
	   ammoType = Weapon_GetAmmoType(itemid),
       avaiableAmount = Character_GetItemAmount(playerid, ammoType);
    if(ammo <= 0 || ammo > avaiableAmount)
        return Dialog_Show(playerid, Dialog_InvSelectAmmo, DIALOG_STYLE_INPUT, "Inserisci le munizioni", "{FF0000}Munizioni non valide!{FFFFFF}\nImmetti la quantità di munizioni che vuoi inserire nell'arma.\nQuantità: {00FF00}%d{FFFFFF}", "Usa", "Annulla", avaiableAmount);
    
	Character_GiveWeapon(playerid, itemid, ammo);
    
    Character_DecreaseSlotAmount(playerid, slotid, 1);
    Character_DecreaseItemAmount(playerid, ammoType, ammo);

	Player_InfoStr(playerid, str_format("Hai equipaggiato: ~g~%s~w~", Weapon_GetName(itemid)), true);
	//Character_AMe(playerid, "prende un'arma dall'inventario.");
	Trigger_OnPlayerInvItemUse(playerid, itemid, 1, ITEM_TYPE_WEAPON);
	Trigger_OnPlayerInvItemUse(playerid, ammoType, ammo, ITEM_TYPE_AMMO);
    return 1;
}

hook function String:GetItemExtraStr(itemid, quantity, extra)
{
    if(ServerItem_IsWeapon(itemid))
    {
        if(Weapon_RequireAmmo(itemid) || Weapon_CanBeDisassembled(itemid))
        {
            if(extra == 0)
                return @("Vuota");
            else
                return str_val(extra);
        }
    }
    return String:continue(itemid, quantity, extra);
}