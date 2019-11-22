
flags:admins(CMD_USER);
CMD:admins(playerid, params[]) 
{
    return ShowAdmins(playerid);
}
alias:admins("staff", "staffers");

CMD:testers(playerid, params[])
{
    new count = 0;
    new String:string = str_new("");
    SendClientMessage(playerid, COLOR_GREY, "Testers online:");
    foreach(new i : Player)
    {
        if(Account_GetAdminLevel(i) != 1)
            continue;
        str_clear(string);
        str_append_format(string, "[ID: %d] %s (%s) - In servizio: %s", i, Account_GetName(i), Character_GetOOCName(i), Player_IsSupporterDuty(i) ? ("si") : ("no"));
        SendClientMessageStr(playerid, Player_IsSupporterDuty(i) ? 0xE9DE98FF : COLOR_WHITE, string);
        count++;
    }
    if(count <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non ci sono testers online.");
    return 1;
}

static ShowAdmins(playerid)
{
    new List:list = list_new();

    foreach(new i : Player)
    {
        if(!Character_IsLogged(i) || Account_GetAdminLevel(i) < 2)
            continue;
        list_add(list, i);    
    }

    if(list_size(list) <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non ci sono amministratori online.");

    list_sort(list);

    new String:string = str_new("");
    
    SendClientMessage(playerid, COLOR_GREY, "Amministratori online:");

    for_list(i : list)
    {
        new id = iter_get(i);
        str_clear(string);
        str_append_format(string, "[ID: %d - Livello: %d] %s (%s) - In servizio: %s", id, Account_GetAdminLevel(id), Account_GetName(id), Character_GetOOCName(id), Player_IsAdminDuty(id) ? ("si") : ("no"));
        SendClientMessageStr(playerid, Player_IsAdminDuty(id) ? COLOR_ORANGE : COLOR_WHITE, string);
    }

    list_delete(list);
    return 1;
}