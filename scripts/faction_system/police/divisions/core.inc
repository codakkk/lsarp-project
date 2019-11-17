

enum E_POLICE_DIVISION
{
    POLICE_DIVISION_NONE = 0,
    POLICE_DIVISION_SWAT = 1,
    POLICE_DIVISION_UPR = 2,
    POLICE_DIVISION_SI = 3
};

static DivisionName[E_POLICE_DIVISION][32] = {"", "S.W.A.T", "Urban Police Rifle", "Servizi Investigativi"};

#include "divisions\swat"
#include "divisions\si"
#include <YSI_Coding\y_hooks>


static E_POLICE_DIVISION:CharacterDivision[MAX_PLAYERS char] = {POLICE_DIVISION_NONE, ...};

hook OnCharacterClearData(playerid)
{
    CharacterDivision{playerid} = POLICE_DIVISION_NONE;
    return 1;
}

hook OnCharacterLoad(playerid)
{
    await mysql_aquery_s(gMySQL, str_format("SELECT division FROM `characters` WHERE id = '%d'", Character_GetID(playerid)));
    new i = 0;
    cache_get_value_name_int(0, "division", i);
    CharacterDivision{playerid} = E_POLICE_DIVISION:i;
    return 1;
}

flags:setdivision(CMD_POLICE);
CMD:setdivision(playerid, params[])
{
    if(Character_GetRank(playerid) > 2)
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei del rango giusto.");
    
    new id, inputDivision[32];
    if(sscanf(params, "k<u>s[32]", id, inputDivision))
    {
        SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /setdivision <playerid/partofname> <divisione>");
        SendClientMessage(playerid, COLOR_COMMAND_ERROR, "Divisioni: swat, upr, dsi");
        return 1;
    }

    new E_POLICE_DIVISION:division = GetDivisionByName(inputDivision);
    if(division == POLICE_DIVISION_NONE)
        return SendClientMessage(playerid, COLOR_ERROR, "La divisione inserita non è valida.");

    if(!Character_IsLogged(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");

    new divisionName[32];
    GetDivisionName(division, divisionName);

    if(Character_GetFaction(playerid) != Character_GetFaction(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non fa parte della fazione.");

    if(id != playerid && Character_GetRank(id) <= Character_GetRank(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi settare la divisione ad un membro di rank più alto.");

    if(Character_GetDivision(id) == division)
        return SendFormattedMessage(playerid, COLOR_ERROR, "Il giocatore fa già parte della divisione \"%s\".", divisionName);

    Character_SetDivision(id, division);

    new rankName[MAX_FACTION_RANK_NAME];
    Faction_GetRankName(Character_GetFaction(playerid), Character_GetRank(playerid), rankName);
    SendFormattedMessage(id, COLOR_ORANGE, "[%s] %s ti ha settato nella divisione \"%s\".", rankName, Character_GetOOCName(playerid), divisionName);
    SendFormattedMessage(playerid, COLOR_ORANGE, "Hai settato %s nella divisione \"%s\".", Character_GetOOCName(id), divisionName);
    return 1;
}

stock Character_SetDivision(playerid, E_POLICE_DIVISION:division)
{
    CharacterDivision{playerid} = division;
    mysql_pquery_f(gMySQL, "UPDATE `characters` SET division = '%d' WHERE id = '%d'", _:division, Character_GetID(playerid));
}

stock E_POLICE_DIVISION:Character_GetDivision(playerid)
{
    return CharacterDivision{playerid};
}

stock E_POLICE_DIVISION:GetDivisionByName(const name[])
{
    new hash = YHash(name);
    if(hash == _I<swat>) return POLICE_DIVISION_SWAT;
    else if(hash == _I<upr>) return POLICE_DIVISION_UPR;
    else if(hash == _I<dsi>) return POLICE_DIVISION_SI;
    return POLICE_DIVISION_NONE;
}

stock GetDivisionName(E_POLICE_DIVISION:division, name[])
{
    format(name, 32, DivisionName[division]);
}