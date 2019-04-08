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
    new Drop[E_DROP_INFO];
    if(Drop_GetData(dropid, Drop) && Drop[dItem] != 0)
    {
        new 
            String:title,
            String:content = @("Raccogli\nSposta");
        title = str_format("%s (%S: %d)", ServerItem_GetName(Drop[dItem]), (ServerItem_IsWeapon(Drop[dItem]) ? @("Munizioni") : @("Quantità")), (ServerItem_IsWeapon(Drop[dItem]) ? Drop[dItemExtra] : Drop[dItemAmount]));
        if(ServerItem_IsWeapon(Drop[dItem]))
            content += @("\n{FFFFFF}Equipaggia{FFFFFF}");
        SetPVarInt(playerid, "Player_DropID", dropid);
        Dialog_Show_s(playerid, Dialog_ItemDropAction, DIALOG_STYLE_LIST, title, content, "Continua", "Chiudi");
        return true;
    }
    return false;
}

Dialog:Dialog_ItemDropAction(playerid, response, listitem, inputtext[])
{
    new dropid = GetPVarInt(playerid, "Player_DropID"), Drop[E_DROP_INFO];
    if(!response || !Drop_GetData(dropid, Drop) || Drop[dItem] == 0)
    {
        DeletePVar(playerid, "Player_DropID");
        return 0;   
    }
    new itemid = Drop[dItem], amount = Drop[dItemAmount], extra = Drop[dItemExtra];
    switch(listitem)
    {
        case 0:
        {
            if( (ServerItem_IsWeapon(itemid) && !Character_HasSpaceForWeapon(playerid, itemid, extra)) || 
                !Character_HasSpaceForItem(playerid, itemid, amount))
                return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario!");
            Character_GiveItem(playerid, itemid, amount);
            Character_GiveItem(playerid, Weapon_GetAmmoType(itemid), extra);
            SendFormattedMessage(playerid, COLOR_GREEN, "Hai raccolto %s (%d)", ServerItem_GetName(Drop[dItem]), ServerItem_IsWeapon(itemid) ? Drop[dItemExtra] : Drop[dItemAmount]);
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

            if(extra <= 0)
                return SendClientMessage(playerid, COLOR_ERROR, "L'arma è scarica e non puo' essere equipaggiata!");

            SendFormattedMessage(playerid, COLOR_GREEN, "Hai raccolto %s (%d)", ServerItem_GetName(Drop[dItem]), Drop[dItemAmount]);
            Character_AMe(playerid, "raccoglie un'arma");
            AC_GivePlayerWeapon(playerid, itemid, extra);
            Drop_Destroy(dropid);
        }
    }
    return 1;
}
