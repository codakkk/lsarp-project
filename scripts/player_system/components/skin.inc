
#include <YSI_Coding\y_hooks>

static 
    PlayerSkin[MAX_PLAYERS] = {0, ...},
    PlayerLastSkin[MAX_PLAYERS] = {0, ...};
hook OnCharacterClearData(playerid)

{
    PlayerSkin[playerid] = PlayerLastSkin[playerid] = 0;
    return 1;
}

hook OnCharacterDisconnected(playerid, reason)
{
    mysql_pquery_f(gMySQL, "UPDATE `characters` SET last_skin = '%d' WHERE id = '%d'", GetPlayerSkin(playerid), Character_GetID(playerid));
    return 1;
}

hook OnCharacterSpawned(playerid)
{
    new skinid = PlayerLastSkin[playerid] > 0 ? PlayerLastSkin[playerid] : Character_GetSkin(playerid);
    if(skinid == 0)
    {
        skinid = 1;
        printf("No skin found for player id %d. Set skin id 1.");
    }
    mysql_pquery_f(gMySQL, "UPDATE `characters` SET last_skin = '%d' WHERE id = '%d'", 0, Character_GetID(playerid));
    SetPlayerSkin(playerid, skinid);
    return 1;
}

hook OnCharacterLoad(playerid)
{
    inline OnLoad()
    {
        cache_get_value_name_int(0, "skin", PlayerSkin[playerid]);
        cache_get_value_name_int(0, "last_skin", PlayerLastSkin[playerid]);
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT skin, last_skin FROM `characters` WHERE id = '%d'", Character_GetID(playerid));
    return 1;
}

static Internal_Save(playerid)
{
    if(!Character_IsLogged(playerid))
        return 0;
    mysql_pquery_f(gMySQL, "UPDATE `characters` SET skin = '%d' WHERE id = '%d'", Character_GetSkin(playerid), Character_GetID(playerid));
    return 1;
}

stock Character_SetSkin(playerid, skinid)
{
    if(skinid == 0)
        return 0;
	PlayerSkin[playerid] = skinid;
    Internal_Save(playerid);
	return SetPlayerSkin(playerid, skinid);
}

stock Character_GetSkin(playerid)
{
    return PlayerSkin[playerid];
}