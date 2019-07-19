
// Remove beanbag when exit national guard duty.

#include <YSI_Coding\y_hooks>

static 
    bool:BeanBag[MAX_PLAYERS char] = {false, ...},
    bool:Taser[MAX_PLAYERS char] = {false, ...},
    TempWeapon[MAX_PLAYERS][2 char],
    TempAmmo[MAX_PLAYERS][2]
;

hook OnPlayerClearData(playerid)
{
    BeanBag{playerid} = false;
    Taser{playerid} = false;
    TempWeapon[playerid]{0} = 0;
    TempWeapon[playerid]{1} = 0;
    TempAmmo[playerid][0] = 0;
    TempAmmo[playerid][1] = 0;
    return 1;
}

hook OnCharacterPreSaveData(playerid, disconnect)
{
    if(disconnect)
    {
        if(Character_HasBeanBag(playerid))
            pc_cmd_beanbag(playerid, "");
        if(Character_HasTaser(playerid))
            pc_cmd_taser(playerid, "");
    }
    return 1;
}

hook OnCharacterDutyChange(playerid, isOnDuty)
{
    if(!isOnDuty)
    {
        if(Character_HasBeanBag(playerid))
            pc_cmd_beanbag(playerid, "");
        
        if(Character_HasTaser(playerid))
            pc_cmd_taser(playerid, "");
    }
    return 1;
}

hook OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
    if(!Character_IsLogged(playerid))
        return 1;
    if(Character_HasTaser(playerid) && weaponid == 23 && Character_IsFactionDuty(playerid))
    {
        ApplyAnimation(playerid, "COLT45", "colt45_reload", 4.1, 0, 0, 0, 0, 0);
    }
    return 1;
}

hook OnCharacterDamageDone(playerid, Float:amount, issuerid, weaponid, bodypart)
{
    printf("1");
    if(issuerid == INVALID_PLAYER_ID)
        return 1;
    printf("2");
    if(!Character_HasBeanBag(issuerid) && !Character_HasTaser(issuerid))
        return 1;
    printf("3");
    if(Character_IsAlive(playerid))
    {
        printf("4");
        if(Faction_GetType(Character_GetFaction(issuerid)) == FACTION_TYPE_POLICE && Character_IsFactionDuty(issuerid))
        {
            printf("5");
            if(weaponid == 25 && ProxDetectorS(15.0, issuerid, playerid))
            {
                printf("6");
                SendFormattedMessage(playerid, COLOR_YELLOW, "Sei stato colpito dal BeanBag di %s (%d).", Character_GetRolePlayName(issuerid), issuerid);
                SendFormattedMessage(issuerid, COLOR_YELLOW, "Hai colpito %s (%d) con la tua BeanBag.", Character_GetRolePlayName(playerid), playerid);
                Character_SetStunned(playerid, 15);
            }
            else if(weaponid == 23 && ProxDetectorS(10.0, issuerid, playerid))
            {
                printf("7");
                SendFormattedMessage(playerid, COLOR_YELLOW, "Sei stato colpito dal taser di %s (%d).", Character_GetRolePlayName(issuerid), issuerid);
                SendFormattedMessage(issuerid, COLOR_YELLOW, "Hai colpito %s (%d) con il tuo taser.", Character_GetRolePlayName(playerid), playerid);
                Character_SetStunned(playerid, 10);
            }
            
            ApplyAnimation(playerid, "CRACK", "crckdeth4", 4.0, 0, 0, 0, 1, 0, 1);
        }
    }
    return 1;
}

flags:beanbag(CMD_POLICE);
CMD:beanbag(playerid, params[])
{
    if(!Character_IsFactionDuty(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Devi essere in servizio per poter utilizzare questo comando.");
    BeanBag{playerid} = !BeanBag{playerid};

    if(BeanBag{playerid})
    {
        Character_AMe(playerid, "estrae il fucile beanbag.");
        
        new w, a;
        
        Character_GetWeaponData(playerid, 3, w, a);
        
        TempWeapon[playerid]{0} = w;
        TempAmmo[playerid][0] = a;

        Character_RemoveWeapon(playerid, TempWeapon[playerid]{0});

        Character_GiveWeapon(playerid, 25, 1000);
    }
    else
    {
        Character_AMe(playerid, "ripone il fucile beanbag.");
        Character_RemoveWeapon(playerid, 25);
        Character_GiveWeapon(playerid, TempWeapon[playerid]{0}, TempAmmo[playerid][0]);
        SetPlayerArmedWeapon(playerid, TempWeapon[playerid]{0});

        TempWeapon[playerid]{0} = TempAmmo[playerid][0] = 0;
    }
    return 1;
}

flags:taser(CMD_POLICE);
CMD:taser(playerid, params[])
{
    if(!Character_IsFactionDuty(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Devi essere in servizio per poter utilizzare questo comando.");
    Taser{playerid} = !Taser{playerid};

    if(Taser{playerid})
    {
        Character_AMe(playerid, "estrae la pistola taser.");
        
        new w, a;
        
        Character_GetWeaponData(playerid, 2, w, a);
        
        TempWeapon[playerid]{1} = w;
        TempAmmo[playerid][1] = a;

        Character_RemoveWeapon(playerid, TempWeapon[playerid]{1});

        Character_GiveWeapon(playerid, 23, 1000);
    }
    else
    {
        Character_AMe(playerid, "ripone la pistola taser.");
        Character_RemoveWeapon(playerid, 23);
        Character_GiveWeapon(playerid, TempWeapon[playerid]{1}, TempAmmo[playerid][1]);
        SetPlayerArmedWeapon(playerid, TempWeapon[playerid]{1});

        TempWeapon[playerid]{1} = TempAmmo[playerid][1] = 0;
    }
    return 1;
}

stock Character_HasBeanBag(playerid)
{
    return BeanBag{playerid};
}

stock Character_HasTaser(playerid)
{
    return Taser{playerid};
}