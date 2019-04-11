#include <YSI_Coding\y_hooks>

forward bool:Character_CollectDrop(playerid, dropid);

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    if(pLastPickup[playerid] != -1 && IsPlayerInRangeOfPickup(playerid, pLastPickup[playerid], 2.0) && PRESSED(KEY_CROUCH))
    {
        new eID, E_ELEMENT_TYPE:eType;
        new result = Pickup_GetInfo(pLastPickup[playerid], eID, eType);
        if(result && eType == ELEMENT_TYPE_DROP)
        {
            Character_CollectDrop(playerid, eID);
        }
    }
    return 1;
}

stock bool:Character_CollectDrop(playerid, dropid)
{
	new itemid = Drop_GetItem(dropid);
    if(Drop_IsValid(dropid) && itemid != 0)
    {
        new 
            String:title,
            String:content = @("Raccogli\nSposta");
        title = str_format("%s (%S: %d)", ServerItem_GetName(itemid), (ServerItem_IsWeapon(itemid) ? @("Munizioni") : @("Quantità")), (ServerItem_IsWeapon(itemid) ? Drop_GetItemExtra(dropid) : Drop_GetItemAmount(dropid)));
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
    new dropid = GetPVarInt(playerid, "Player_DropID"),
    	itemid, amount, extra;
    if(!response || !Drop_GetItemData(dropid, itemid, amount, extra))
    {
        DeletePVar(playerid, "Player_DropID");
        return 0;   
    }
    switch(listitem)
    {
        case 0:
        {
            if(!Character_HasSpaceForItem(playerid, itemid, amount))
                return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario!");
			Character_GiveItem(playerid, itemid, amount, extra);
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai raccolto %s (%d)", ServerItem_GetName(itemid), ServerItem_IsWeapon(itemid) ? extra : amount);
            Character_AMe(playerid, "raccoglie qualcosa");
            Drop_Destroy(dropid);
        }
        case 1:
        {
            SendClientMessage(playerid, COLOR_ERROR, "Non ancora disponibile");
        }
        case 2:
        {
            new weapon, ammo;
            GetPlayerWeaponData(playerid, Weapon_GetSlot(itemid), weapon, ammo);

            if(weapon != 0 && ammo > 0)
                return SendClientMessage(playerid, COLOR_ERROR, "Non puoi equipaggiare quest'arma!");

            if(Weapon_RequireAmmo(itemid) && extra <= 0)
                return SendClientMessage(playerid, COLOR_ERROR, "L'arma è scarica e non puo' essere equipaggiata!");

            SendFormattedMessage(playerid, COLOR_GREEN, "Hai raccolto %s (%d)", ServerItem_GetName(itemid), extra);
            Character_AMe(playerid, "raccoglie un'arma");
            AC_GivePlayerWeapon(playerid, itemid, extra);
            Drop_Destroy(dropid);
        }
    }
    return 1;
}
