#include <YSI\y_hooks>

forward bool:Character_CollectDrop(playerid, dropid);

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    if(pLastPickup[playerid] != -1 && PRESSED(KEY_WALK))
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
    if(Drop_GetData(dropid, Drop) && Drop[dItem] != 0 && Drop[dItemAmount] > 0)
    {
        if(!Character_HasSpaceForItem(playerid, Drop[dItem], Drop[dItemAmount]) || (ServerItem_IsWeapon(Drop[dItem]) && !Character_HasSpaceForWeapon(playerid, Drop[dItem], Drop[dItemExtra])))
        {
            SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza spazio nell'inventario!");
            return false;
        }
        new itemid = Drop[dItem], amount = Drop[dItemAmount], extra = Drop[dItemExtra];
        Character_GiveItem(playerid, itemid, amount, extra);
        // Probably I shouldn't use extra for ammos. Instead, put ammos in inventory when /getta is used
        if(ServerItem_IsWeapon(itemid) && extra > 0)
        {
            Character_GiveItem(playerid, Weapon_GetAmmoType(itemid), extra);
        }
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai raccolto %s (%d)", ServerItem_GetName(Drop[dItem]), Drop[dItemAmount]);
        Character_AMe(playerid, "raccoglie qualcosa da terra");
        Drop_Destroy(dropid);
        return true;
    }
    return false;
}