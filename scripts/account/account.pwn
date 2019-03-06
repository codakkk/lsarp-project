#include <YSI/y_hooks>


stock OnAccountLogin(playerid)
{
    gAccountLogged[playerid] = 1;

    // Count player's characters

    inline OnLoad()
    {
        cache_get_row_count(AccountInfo[playerid][aCharactersCount]);
        ShowCharactersList(playerid);
    }
    new 
        query[128];
    
    mysql_format(gMySQL, query, sizeof(query), "SELECT ID FROM characters WHERE AccountID = '%d'", AccountInfo[playerid][aID]);
    mysql_tquery_inline(gMySQL, query, using inline OnLoad);
}

stock LoadAccountData(playerid)
{
    if(cache_num_rows() > 0)
    {
        cache_get_value_index_int(0, 0, AccountInfo[playerid][aID]);
        cache_get_value_index(0, 1, AccountInfo[playerid][aName]);
        cache_get_value_index_int(0, 3, AccountInfo[playerid][aAdmin]);
        cache_get_value_index_int(0, 4, AccountInfo[playerid][aPremium]);
        cache_get_value_index_int(0, 5, AccountInfo[playerid][aPremiumExpiry]);
        return 1;
    }
    return 0;
}

stock SaveAccountData(playerid)
{
    if(!gAccountLogged[playerid])
        return 0;
    new query[256];
    mysql_format(gMySQL, query, sizeof(query), "UPDATE `accounts` SET Admin = '%d', Premium = '%d' \
        WHERE ID = '%d'", 
        AccountInfo[playerid][aAdmin], AccountInfo[playerid][aPremium],
        AccountInfo[playerid][aID]);
    
    mysql_tquery(gMySQL, query);
    return 1;
}