
#include <YSI_Coding\y_hooks>

static
    Float:PlayerMaxHealth[MAX_PLAYERS],
    Float:PlayerMaxArmour[MAX_PLAYERS],
    UpgradePoints[MAX_PLAYERS]
;

hook OnCharacterClearData(playerid)
{
    PlayerMaxHealth[playerid] = 100.0;
    PlayerMaxArmour[playerid] = 100.0;
    UpgradePoints[playerid] = 0;
    return 1;
}

hook OnCharacterLoad(playerid)
{
    await mysql_aquery_s(gMySQL, str_format("SELECT max_health, max_armour, upgrade_points FROM `characters` WHERE id = '%d'", Character_GetID(playerid)), .parallel=true);
    
    cache_get_value_name_float(0, "max_health", PlayerMaxHealth[playerid]);
    cache_get_value_name_float(0, "max_armour", PlayerMaxArmour[playerid]);
    cache_get_value_name_int(0, "upgrade_points", UpgradePoints[playerid]);

    SetPlayerMaxArmour(playerid, PlayerMaxArmour[playerid]);
    SetPlayerMaxHealth(playerid, PlayerMaxHealth[playerid]);
    return 1;
}

CMD:upgrade(playerid, params[])
{
    if(isnull(params) || strlen(params) <= 0)
    {
        SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /upgrade [vita - armatura]");
        SendFormattedMessage(playerid, COLOR_COMMAND_ERROR, "Attualmente hai %d punti a disposizione.", UpgradePoints[playerid]);
        return 1;
    }
    
    if(UpgradePoints[playerid] <= 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza punti.");
    
    new hash = YHash(params, false);
    if(hash == _I<vita>)
    {
        if(Character_GetMaxHealth(playerid) >= 150.0)
            return SendClientMessage(playerid, COLOR_ERROR, "Hai raggiunto il limite di vita massima.");
        
        Character_AddUpgradePoints(playerid, -1);

        new Float:current = Character_GetMaxHealth(playerid);
        current += 2.0;

        if(current > 150.0)
            current = 150.0;

        Character_SetMaxHealth(playerid, current);

        SendFormattedMessage(playerid, COLOR_GREEN, "Hai aumentato i tuoi punti salute massimi. Punti upgrade rimanenti: %d.", UpgradePoints[playerid]);
    }
    else if(hash == _I<armatura>)
    {
        if(Character_GetMaxArmour(playerid) >= 150.0)
            return SendClientMessage(playerid, COLOR_ERROR, "Hai raggiunto il limite di armatura massima.");
        
        Character_AddUpgradePoints(playerid, -1);

        new Float:current = Character_GetMaxArmour(playerid);
        current += 2.0;

        if(current > 150.0)
            current = 150.0;

        Character_SetMaxArmour(playerid, current);

        SendFormattedMessage(playerid, COLOR_GREEN, "Hai aumentato i tuoi punti armatura massimi. Punti upgrade rimanenti: %d.", UpgradePoints[playerid]);
    }
    return 1;
}

stock bool:Character_SetMaxHealth(playerid, Float:max)
{
	if(max < 100.0)
	{
		return false;
	}
	PlayerMaxHealth[playerid] = max;
    SetPlayerMaxHealth(playerid, max);
	mysql_pquery_f(gMySQL, "UPDATE `characters` SET max_health = '%f' WHERE id = '%d'", max, Character_GetID(playerid));
	return true;
}

stock Float:Character_GetMaxHealth(playerid)
{
	return PlayerMaxHealth[playerid];
}

stock bool:Character_SetMaxArmour(playerid, Float:max)
{
	PlayerMaxArmour[playerid] = max;
    SetPlayerMaxArmour(playerid, max);
	mysql_pquery_f(gMySQL, "UPDATE `characters` SET max_armour = '%f' WHERE id = '%d'", max, Character_GetID(playerid));
	return true;
}

stock Float:Character_GetMaxArmour(playerid)
{
	return PlayerMaxArmour[playerid];
}

stock Character_AddUpgradePoints(playerid, points)
{
    UpgradePoints[playerid] += points;
    mysql_pquery_f(gMySQL, "UPDATE `characters` SET upgrade_points = '%d' WHERE id = '%d'", UpgradePoints[playerid], Character_GetID(playerid));
    return 1;
}