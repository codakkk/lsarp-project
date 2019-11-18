
#include <YSI_Coding\y_hooks>

static enum e_LOST_ITEMS
{
    E_ITEM_ID,
    E_QUANTITY,
    E_EXTRA
};

static List:LostItems[MAX_PLAYERS] = {List:0, ...};

hook OnCharacterClearData(playerid)
{
    if(list_valid(LostItems[playerid]))
        list_delete(LostItems[playerid]);
    LostItems[playerid] = INVALID_LIST;
    return 1;
}

hook OnCharacterDeath(playerid, killerid, reason)
{
    if(Character_IsAlive(playerid))
    {
        if(!list_valid(LostItems[playerid]))
        LostItems[playerid] = list_new();
    
        list_clear(LostItems[playerid]);

        new data[e_LOST_ITEMS], w, a;
        for(new i = 0; i < 13; ++i)
        {
            Character_GetWeaponData(playerid, i, w, a);
            if(w != 0 && a != 0)
            {
                data[E_ITEM_ID] = w;
                data[E_QUANTITY] = a;
                data[E_EXTRA] = 0;
                list_add_arr(LostItems[playerid], data);
            }
        }
    }
    return 1;
}

flags:lostitems(CMD_USER);
CMD:lostitems(playerid, params[])
{
    if(!list_valid(LostItems[playerid]) || list_size(LostItems[playerid]) == 0 )
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai perso oggetti.");
    
    new String:weapons, String:others;

    new data[e_LOST_ITEMS];
    for_list(i : LostItems[playerid])
    {
        iter_get_arr(i, data);

        if(!ServerItem_IsValid(data[E_ITEM_ID]))
            continue;

        if(ServerItem_IsWeapon(data[E_ITEM_ID]))
        {
            weapons += str_format("%s - %d (munizioni)", ServerItem_GetName(data[E_ITEM_ID]), data[E_QUANTITY]);
        }
        else
        {
            others += str_format("%s - %d (quantità)", ServerItem_GetName(data[E_ITEM_ID]), data[E_QUANTITY]);
        }
    }
    new String:total;

    if(str_len(weapons) > 0)
        total += str_format("{FF0000}Armi Perdute:{FFFFFF}\n%S\n\n", weapons);
    
    if(str_len(others))
        total += str_format("{FF0000}Oggetti Perduti:{FFFFFF}\n%S\n\n", others);
    
    Dialog_Show_s(playerid, DialogNull, DIALOG_STYLE_MSGBOX, @("Oggetti Perduti"), total, "Chiudi", "");
    return 1;
}