#include <YSI\y_hooks>

flags:inventario(CMD_USER);
CMD:inventario(playerid, params[])
{
    Character_ShowInventory(playerid, playerid);
    return 1;
}
alias:inventario("inv");

flags:dep(CMD_USER);
CMD:dep(playerid, params[])
{
    new 
        itemid = GetPlayerWeapon(playerid),
        ammos = GetPlayerAmmo(playerid),
        ammoItemId = GetWeaponAmmoItemID(itemid);
    if(itemid != 0)
    {
        if(!Character_HasSpaceForItem(playerid, itemid, 1) || (ammos > 0 && !Character_HasSpaceForItem(playerid, ammoItemId, ammos)))
        {
            return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario!");
        }
        Character_GiveItem(playerid, itemid, 1);
        Character_GiveItem(playerid, ammoItemId, ammos);
        AC_ResetPlayerWeapons(playerid);
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai depositato la tua arma (%s). Munizioni: %d", ServerItem_GetName(itemid), ammos);
    }
    else
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando senza un'arma!");
    }
    return 1;
}