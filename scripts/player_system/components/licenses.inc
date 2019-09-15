
#include <YSI_Coding\y_hooks>

static enum _:E_LICENSE_TYPE
{
    LICENSE_TYPE_DRIVER,
    LICENSE_TYPE_BOAT,
    LICENSE_TYPE_FLY,
    LICENSE_TYPE_WEAPON
}

static enum E_DRIVE_LICENSE_I
{
    Float:X, Float:Y, Float:Z, Interior, World, Type, Price
}

static LicensesData[][E_DRIVE_LICENSE_I] =
{
    {1723.3232, -1867.1775, 13.5705, -1, -1, LICENSE_TYPE_DRIVER, 3000},
    {1723.3232, -1870.1775, 13.5705, -1, -1, LICENSE_TYPE_BOAT, 6000},
    {1723.3232, -1873.1775, 13.5705, -1, -1, LICENSE_TYPE_FLY, 12000},
    {1723.3232, -1876.1775, 13.5705, -1, -1, LICENSE_TYPE_WEAPON, 0}
};

static 
    bool:DriveLicense[MAX_PLAYERS char],
    bool:BoatLicense[MAX_PLAYERS char],
    bool:FlyLicense[MAX_PLAYERS char],
    bool:WeaponLicense[MAX_PLAYERS char]
;

hook OnGameModeInit()
{
    mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `character_licenses` ( \
                        `character_id` int(11) NOT NULL, \
                        `drive` int(1) NOT NULL DEFAULT '0', \
                        `boat` int(1) NOT NULL DEFAULT '0', \
                        `fly` int(1) NOT NULL DEFAULT '0', \
                        `weapon` int(1) NOT NULL DEFAULT '0', \
                        PRIMARY KEY (`character_id`), \
                        CONSTRAINT `character_licenses_c` FOREIGN KEY (`character_id`) REFERENCES `characters` (`id`) ON DELETE CASCADE \
                        ) ENGINE=InnoDB DEFAULT CHARSET=latin1");
    new name[64];
    for(new i = 0, j = sizeof(LicensesData); i < j; ++i)
    {
        Pickup_Create(1239, i, LicensesData[i][X], LicensesData[i][Y], LicensesData[i][Z], ELEMENT_TYPE_LICENSE, LicensesData[i][Interior], LicensesData[i][World]);
        format(name, sizeof(name), "%s (/compra)", GetLicenseName(LicensesData[i][Type]));
        CreateDynamic3DTextLabel(name, COLOR_BLUE, LicensesData[i][X], LicensesData[i][Y], LicensesData[i][Z], 15.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, LicensesData[i][World], LicensesData[i][Interior]);
    }
    return 1;
}

hook OnPlayerClearData(playerid)
{
    DriveLicense{playerid} = BoatLicense{playerid} = FlyLicense{playerid} = WeaponLicense{playerid} = false;
    return 1;
}

hook OnCharacterLoad(playerid)
{
    inline OnLoad()
    {
        if(cache_num_rows() <= 0)
        {
            mysql_tquery_f(gMySQL, "INSERT INTO `character_licenses` (character_id, drive, boat, fly, weapon) VALUES('%d', '0', '0', '0', '0')", Character_GetID(playerid));
            DriveLicense{playerid} = BoatLicense{playerid} = FlyLicense{playerid} = WeaponLicense{playerid} = false;
        }
        else
        {
            new tmp;
            cache_get_value_name_int(0, "drive", tmp);
            DriveLicense{playerid} = tmp ? true : false;

            cache_get_value_name_int(0, "boat", tmp);
            BoatLicense{playerid} = tmp ? true : false;

            cache_get_value_name_int(0, "fly", tmp);
            FlyLicense{playerid} = tmp ? true : false;

            cache_get_value_name_int(0, "weapon", tmp);
            WeaponLicense{playerid} = tmp ? true : false;
        }
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `character_licenses` WHERE character_id = '%d'", Character_GetID(playerid));
    return 1;
}

hook OnCharacterTryBuy(playerid)
{
    new element, E_ELEMENT_TYPE:t;
    if(!Character_GetNearestPickup(playerid, element, t) || t != ELEMENT_TYPE_LICENSE)
        return 0;
    new type = LicensesData[element][Type];
    new price = LicensesData[element][Price];
    new name[24];
    if(type == LICENSE_TYPE_DRIVER)
        name = "Patente di guida";
    else if(type == LICENSE_TYPE_BOAT)
        name = "Patente Nautica";
    else if(type == LICENSE_TYPE_FLY)
        name = "Patente di volo";
    else if(type == LICENSE_TYPE_WEAPON)
        name = "Porto d'armi";
    Dialog_Show(playerid, Dialog_LicenseBuy, DIALOG_STYLE_MSGBOX, name, "Sei sicuro di voler acquistare questa licenza (%s) per $%d?", "Acquista", "Chiudi", name, price);
    return 1;
}

flags:documenti(CMD_ALIVE_USER);
CMD:documenti(playerid, params[])
{
    Character_AMe(playerid, "guarda la sua carta d'identità.");

    SendClientMessage(playerid, COLOR_GREEN, "__________Las Venturas ID__________");
    SendFormattedMessage(playerid, -1, "Nome e Cognome: %s", Character_GetOOCName(playerid));
    SendFormattedMessage(playerid, -1, "- Patente di guida: %s", Character_HasDriveLicense(playerid) ? ("Si") : ("No"));
    SendFormattedMessage(playerid, -1, "- Patente nautica: %s", Character_HasBoatLicense(playerid) ? ("Si") : ("No"));
    SendFormattedMessage(playerid, -1, "- Patente di volo: %s", Character_HasFlyLicense(playerid) ? ("Si") : ("No"));
    SendFormattedMessage(playerid, -1, "- Porto d'armi: %s", Character_HasWeaponLicense(playerid) ? ("Si") : ("No"));
    SendClientMessage(playerid, COLOR_GREEN, "___________________________________");
    return 1;
}

flags:mostradocumenti(CMD_ALIVE_USER);
CMD:mostradocumenti(playerid, params[])
{
    new id;
    if(sscanf(params, "k<u>", id))
        return SendClientMessage(playerid, COLOR_ERROR, "/mostradocumenti (/md) <playerid/partofname/#code>");
    
    if(!Character_IsLogged(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore inserito non è collegato.");

    if(playerid == id)
        return pc_cmd_documenti(playerid, "");

    if(!IsPlayerInRangeOfPlayer(playerid, id, 5.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
    
    Character_CharacterMe(playerid, id, "mostra la sua carta d'identità a");

    SendClientMessage(id, COLOR_GREEN, "__________Las Venturas ID__________");
    SendFormattedMessage(id, -1, "Nome e Cognome: %s", Character_GetOOCName(playerid));
    SendFormattedMessage(id, -1, "- Patente di guida: %s", Character_HasDriveLicense(playerid) ? ("Si") : ("No"));
    SendFormattedMessage(id, -1, "- Patente nautica: %s", Character_HasBoatLicense(playerid) ? ("Si") : ("No"));
    SendFormattedMessage(id, -1, "- Patente di volo: %s", Character_HasFlyLicense(playerid) ? ("Si") : ("No"));
    SendFormattedMessage(id, -1, "- Porto d'armi: %s", Character_HasWeaponLicense(playerid) ? ("Si") : ("No"));
    SendClientMessage(playerid, COLOR_GREEN, "___________________________________");
    return 1;
}
alias:mostradocumenti("md");

flags:ritiralicenza(CMD_POLICE);
CMD:ritiralicenza(playerid, params[])
{
    new id, type[16];
    if(sscanf(params, "k<u>s[16]", id, type))
        return SendClientMessage(playerid, COLOR_ERROR, "/ritiralicenza <playerid/partofname/#code> <patente/nautica/volo/armi>");
    
    if(!Character_IsLogged(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore inserito non è collegato.");

    if(!IsPlayerInRangeOfPlayer(playerid, id, 5.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
    
    if(playerid == id)
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su te stesso.");
    
    if(strfind(type, "patente", true) != -1)
    {
        if(!Character_HasDriveLicense(id))
            return SendClientMessage(playerid, COLOR_ERROR, "Il cittadino non possiede la patente di guida.");
        
        Character_SetDriveLicense(id, false);
        SendFormattedMessage(id, COLOR_ORANGE, "%s ti ha ritirato la tua patente di guida.", Character_GetRolePlayName(playerid));
        SendFormattedMessage(playerid, COLOR_ORANGE, "Hai ritirato la patente di %s.", Character_GetRolePlayName(id));
    }
    else if(strfind(type, "nautica", true) != -1)
    {
        if(!Character_HasBoatLicense(id))
            return SendClientMessage(playerid, COLOR_ERROR, "Il cittadino non possiede la patente nautica.");
        
        Character_SetBoatLicense(id, false);
        SendFormattedMessage(id, COLOR_ORANGE, "%s ti ha ritirato la tua patente nautica.", Character_GetRolePlayName(playerid));
        SendFormattedMessage(playerid, COLOR_ORANGE, "Hai ritirato la patente nautica di %s.", Character_GetRolePlayName(id));
    }
    else if(strfind(type, "volo", true) != -1)
    {
        if(!Character_HasFlyLicense(id))
            return SendClientMessage(playerid, COLOR_ERROR, "Il cittadino non possiede la patente di volo.");
        
        Character_SetFlyLicense(id, false);
        SendFormattedMessage(id, COLOR_ORANGE, "%s ti ha ritirato la tua patente di volo.", Character_GetRolePlayName(playerid));
        SendFormattedMessage(playerid, COLOR_ORANGE, "Hai ritirato la patente di volo di %s.", Character_GetRolePlayName(id));
    }
    else if(strfind(type, "armi", true) != -1)
    {
        if(!Character_HasWeaponLicense(id))
            return SendClientMessage(playerid, COLOR_ERROR, "Il cittadino non possiede il porto d'armi.");
        
        Character_SetWeaponLicense(id, false);
        SendFormattedMessage(id, COLOR_ORANGE, "%s ti ha ritirato il tuo porto d'armi.", Character_GetRolePlayName(playerid));
        SendFormattedMessage(playerid, COLOR_ORANGE, "Hai ritirato il porto d'armi di %s.", Character_GetRolePlayName(id));
    }
    return 1;
}

flags:daiportodarmi(CMD_POLICE);
CMD:daiportodarmi(playerid, params[])
{
    if(Character_GetRank(playerid) > 3)
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei del rango giusto per poter utilizzare questo comando.");
    new id;
    if(sscanf(params, "k<u>", id))
        return SendClientMessage(playerid, COLOR_ERROR, "/daiportodarmi <playerid/partofname/#code>");
    
    if(!Character_IsLogged(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore inserito non è collegato.");

    if(!IsPlayerInRangeOfPlayer(playerid, id, 5.0))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
    
    if(playerid == id)
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su te stesso.");

    if(Character_HasWeaponLicense(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Il cittadino ha già un porto d'armi.");

    Character_SetWeaponLicense(id, true);

    SendFormattedMessage(playerid, COLOR_ORANGE, "Hai dato il porto d'armi a %s.", Character_GetRolePlayName(id));
    SendFormattedMessage(id, COLOR_ORANGE, "%s ti ha dato il porto d'armi.", Character_GetRolePlayName(playerid));
    SendMessageToAdmins(1, COLOR_ORANGE, "%s ha dato il porto d'armi a %s.", Character_GetOOCName(playerid), Character_GetOOCName(id));
    return 1;
}

Dialog:Dialog_LicenseBuy(playerid, response, listitem, inputtext[])
{
    if(!response)
        return 0;
    new element, E_ELEMENT_TYPE:t;
    if(!Character_GetNearestPickup(playerid, element, t) || t != ELEMENT_TYPE_LICENSE)
        return 0;
    
    new type = LicensesData[element][Type];

    new price = LicensesData[element][Price];
    
    if(Character_HasLicense(playerid, type))
        return SendClientMessage(playerid, COLOR_ERROR, "Possiedi già questa licenza.");
    if(Character_SetLicense(playerid, type, true))
    {
        SendFormattedMessage(playerid, COLOR_GREEN, "Hai acquistato la licenza (%s) per $%d.", GetLicenseName(type), price);
        Character_GiveMoney(playerid, -price, "license_buy");
    }
    return 1;
}

stock bool:Character_SetLicense(playerid, type, bool:set)
{
    new bool:ret;
    if(type == LICENSE_TYPE_DRIVER)
        ret = Character_SetDriveLicense(playerid, set);
    else if(type == LICENSE_TYPE_BOAT)
        ret = Character_SetBoatLicense(playerid, set);
    else if(type == LICENSE_TYPE_FLY)
        ret = Character_SetFlyLicense(playerid, set);
    else if(type == LICENSE_TYPE_WEAPON)
        ret = Character_SetWeaponLicense(playerid, set);
    return ret;
}

stock bool:Character_HasLicense(playerid, type)
{
    new bool:ret;
    if(type == LICENSE_TYPE_DRIVER)
        ret = Character_HasDriveLicense(playerid);
    else if(type == LICENSE_TYPE_BOAT)
        ret = Character_HasBoatLicense(playerid);
    else if(type == LICENSE_TYPE_FLY)
        ret = Character_HasFlyLicense(playerid);
    else if(type == LICENSE_TYPE_WEAPON)
        ret = Character_HasWeaponLicense(playerid);
    return ret;
}

stock bool:Character_SetDriveLicense(playerid, bool:set)
{
    if(!Character_IsLogged(playerid))
        return false;
    DriveLicense{playerid} = set;
    mysql_tquery_f(gMySQL, "UPDATE `character_licenses` SET drive = '%d' WHERE character_id = '%d'", set ? 1 : 0, Character_GetID(playerid));
    return true;
}

stock bool:Character_HasDriveLicense(playerid)
{
    return DriveLicense{playerid};
}

stock bool:Character_SetBoatLicense(playerid, bool:set)
{
    if(!Character_IsLogged(playerid))
        return false;
    BoatLicense{playerid} = set;
    mysql_tquery_f(gMySQL, "UPDATE `character_licenses` SET boat = '%d' WHERE character_id = '%d'", set ? 1 : 0, Character_GetID(playerid));
    return true;
}

stock bool:Character_HasBoatLicense(playerid)
{
    return BoatLicense{playerid};
}

stock bool:Character_SetFlyLicense(playerid, bool:set)
{
    if(!Character_IsLogged(playerid))
        return false;
    FlyLicense{playerid} = set;
    mysql_tquery_f(gMySQL, "UPDATE `character_licenses` SET fly = '%d' WHERE character_id = '%d'", set ? 1 : 0, Character_GetID(playerid));
    return true;
}

stock bool:Character_HasFlyLicense(playerid)
{
    return FlyLicense{playerid};
}

stock bool:Character_SetWeaponLicense(playerid, bool:set)
{
    if(!Character_IsLogged(playerid))
        return false;
    WeaponLicense{playerid} = set;
    mysql_tquery_f(gMySQL, "UPDATE `character_licenses` SET weapon = '%d' WHERE character_id = '%d'", set ? 1 : 0, Character_GetID(playerid));
    return true;
}

stock bool:Character_HasWeaponLicense(playerid)
{
    return WeaponLicense{playerid};
}

static GetLicenseName(type)
{
    new name[24];
    if(type == LICENSE_TYPE_DRIVER)
        name = "Patente di guida";
    else if(type == LICENSE_TYPE_BOAT)
        name = "Patente Nautica";
    else if(type == LICENSE_TYPE_FLY)
        name = "Patente di volo";
    else if(type == LICENSE_TYPE_WEAPON)
        name = "Porto d'armi";
    return name;
}