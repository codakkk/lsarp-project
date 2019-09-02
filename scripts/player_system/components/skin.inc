
#include <YSI_Coding\y_hooks>

static PlayerSkin[MAX_PLAYERS] = {0, ...};

hook OnCharacterSpawn(playerid)
{
    SetPlayerSkin(playerid, Character_GetSkin(playerid));
    return 1;
}

hook OnCharacterLoad(playerid)
{
    inline OnLoad()
    {
        cache_get_value_index_int(0, 0, PlayerSkin[playerid]);
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT skin FROM `characters` WHERE id = '%d'", Character_GetID(playerid));
    return 1;
}

static Internal_Save(playerid)
{
    if(!Character_IsLogged(playerid))
        return 0;
    mysql_tquery_f(gMySQL, "UPDATE `characters` SET skin = '%d' WHERE id = '%d'", Character_GetSkin(playerid), Character_GetID(playerid));
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