#include <YSI_Coding\y_hooks>

static 
    FightStyle[MAX_PLAYERS];

hook OnCharacterLoad(playerid)
{
    inline OnLoad()
    {
        cache_get_value_index_int(0, 0, FightStyle[playerid]);
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT fight_style FROM `characters` WHERE id = '%d'", Character_GetID(playerid));
    return 1;
}

hook OnCharacterSpawn(playerid)
{
    if(Character_GetFightingStyle(playerid) == 0)
		Character_SetFightingStyle(playerid, FIGHT_STYLE_NORMAL);
	
	SetPlayerFightingStyle(playerid, Character_GetFightingStyle(playerid));
    return 1;
}

stock Internal_Save(playerid)
{
    mysql_tquery_f(gMySQL, "UPDATE `characters` SET fight_style = '%d' WHERE id = '%d'", Character_GetFightingStyle(playerid), Character_GetID(playerid));
}

stock Character_SetFightingStyle(playerid, style)
{
	if(style != FIGHT_STYLE_NORMAL && style != FIGHT_STYLE_BOXING && style != FIGHT_STYLE_KUNGFU && style != FIGHT_STYLE_KNEEHEAD && style != FIGHT_STYLE_GRABKICK)
		return 0;
	FightStyle[playerid] = style;
    Internal_Save(playerid);
	return SetPlayerFightingStyle(playerid, style);
}

stock Character_GetFightingStyle(playerid)
{
	return FightStyle[playerid];
}