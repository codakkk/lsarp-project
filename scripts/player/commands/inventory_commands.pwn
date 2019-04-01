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
        ammos = GetPlayerAmmo(playerid);
    if(itemid != 0)
    {
        if(!Character_HasSpaceForWeapon(playerid, itemid, ammos))
        {
            return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario!");
        }
        Character_GiveItem(playerid, itemid, 1);
        Character_GiveItem(playerid, Weapon_GetAmmoType(itemid), ammos);
        AC_ResetPlayerWeapons(playerid);
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai depositato la tua arma (%s). Munizioni: %d", ServerItem_GetName(itemid), ammos);
    }
    else
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando senza un'arma!");
    }
    return 1;
}

/*flags:getta(CMD_USER);
CMD:getta(playerid, params[])
{
    if(GetPlayerWeapon(playerid) == 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un'arma da gettare.");
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    Drop_Create(x, y, z, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), GetPlayerWeapon(playerid), 1, 0, Character_GetOOCNameStr(playerid));
    SendClientMessage(playerid, COLOR_GREEN, "Hai gettato l'arma.");
    return 1;
}*/