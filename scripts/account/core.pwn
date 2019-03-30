#include <YSI/y_hooks>

hook OnPlayerClearData(playerid)
{
    CharacterCreation_Reset(playerid);
    return 1;
}

hook OnPlayerRequestClass(playerid, classid)
{
    if(IsPlayerNPC(playerid))
        return 1;

    if(gCharacterLogged[playerid])
    {
        SpawnPlayer(playerid);
        return 1;
    }

    if(gAccountLogged[playerid] && !gCharacterLogged[playerid])
    {
        Account_ShowCharactersList(playerid);
        return 1;
    }

    new 
        name[24],
        query[128];
    
    GetPlayerName(playerid, name, sizeof(name));

    inline AccountCheckCallback()
    {
        if(cache_num_rows() > 0)
        {
            new id;
            cache_get_value_index_int(0, 0, id);
            Dialog_Show(playerid, Dialog_Login, DIALOG_STYLE_PASSWORD, "Login", "Ciao %s!\nIl tuo account risulta registrato.\nInserisci la password per effettuare il login.", "Login", "Esci", name);
        }
        else
        {
            Dialog_Show(playerid, DialogNull, DIALOG_STYLE_MSGBOX, "Account non registrato", "Questo account non risulta registrato.\nVisita www.lsarp.it per registrare un account.", "Ok", "", name);
        }
    }

    mysql_format(gMySQL, query, sizeof(query), "SELECT ID FROM accounts WHERE LOWER(Name) = LOWER('%e')", name);
    mysql_tquery_inline(gMySQL, query, using inline AccountCheckCallback);
    return 1;
}

stock Account_CreateCharacter(playerid)
{
    
    
}

stock OnAccountLogin(playerid)
{
    gAccountLogged[playerid] = 1;

    // Count player's characters

    inline OnLoad()
    {
        cache_get_row_count(AccountInfo[playerid][aCharactersCount]);
        Account_ShowCharactersList(playerid);
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

stock Account_ShowCharactersList(playerid)
{
    if(!gAccountLogged[playerid])
        return 0;

    inline OnLoad()
    {
        new count;
        cache_get_row_count(count);

        if(cache_num_rows() == 0)
        {
            gCharacterCreationStep[playerid] = 0;
            Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Crea nuovo personaggio", "Attualmente non risultano personaggi creati!\nPer poter giocare, devi disporre di un personaggio.\nInserisci il nome del nuovo personaggio.\nEsempio: Mario Rossi", "Ok", "");
            return 1;
        }

        new
            list[24 * MAX_CHARACTERS] = "",
            nameTemp[24];

        if(count > MAX_CHARACTERS)
            count = MAX_CHARACTERS;

        for(new i = 0; i < count; ++i)
        {
            cache_get_value_index_int(i, 0, AccountInfo[playerid][aCharacters][i]);
            cache_get_value_index(i, 1, nameTemp, sizeof(nameTemp));
            format(list, sizeof(list), "%s%s\n", list, nameTemp);
        }
        return Dialog_Show(playerid, Dialog_CharacterSelect, DIALOG_STYLE_LIST, "Gestione Personaggi", "%s", "Utilizza", "Altro", list);
    }

    new 
        query[128];
    
    mysql_format(gMySQL, query, sizeof(query), "SELECT ID, Name FROM characters WHERE AccountID = '%d'", AccountInfo[playerid][aID]);
    mysql_tquery_inline(gMySQL, query, using inline OnLoad);
    return 1;
}

// Resets all variables created for CharacterCreationSteps
stock CharacterCreation_Reset(playerid)
{
    gCharacterCreationStep[playerid] = -1;
    DeletePVar(playerid, "NewCharacter_Sex");
    DeletePVar(playerid, "NewCharacter_Age");
    DeletePVar(playerid, "NewCharacter_Name");
    DeletePVar(playerid, "DeleteCharacter_Code");
}