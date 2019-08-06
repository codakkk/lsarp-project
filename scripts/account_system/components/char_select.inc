
#include <YSI_Coding\y_hooks>

static 
    SelectedCharacter[MAX_PLAYERS char],
    TempCharacterName[MAX_PLAYERS][MAX_PLAYER_NAME],
    List:Characters[MAX_PLAYERS],
    DeleteCharacterCode[MAX_PLAYERS] = {0, ...};

hook OnAccountLogin(playerid)
{
    Characters[playerid] = list_new();
    Account_ShowCharactersList(playerid);
    return 1;
}

hook OnPlayerClearData(playerid)
{
    if(list_valid(Characters[playerid]))
	{
		list_delete_deep(Characters[playerid]);
	}
    return 1;
}

stock Account_ShowCharactersList(playerid)
{
    if(!Account_IsLogged(playerid))
	   return 0;

    inline OnLoad()
    {
		new count;
		cache_get_row_count(count);
		printf("COunt: %d", count);
		if(cache_num_rows() == 0)
		{
			Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Crea nuovo personaggio", "Attualmente non risultano personaggi creati.\nPer poter giocare, devi disporre di un personaggio.\nInserisci il nome del nuovo personaggio.\nEsempio: Mario Rossi", "Ok", "");
			return 1;
		}
		
		list_clear(Characters[playerid]);

		new
			String:content,
			nameTemp[24], 
			character_id = 0;

		for(new i = 0; i < count; ++i)
		{
			cache_get_value_index_int(i, 0, character_id);
			cache_get_value_index(i, 1, nameTemp, sizeof(nameTemp));
			
			content += str_format("%s\n", nameTemp);
			
			printf("%s", nameTemp);
			list_add(Characters[playerid], character_id);
	   	}
	   	return Dialog_Show_s(playerid, Dialog_CharacterSelect, DIALOG_STYLE_LIST, @("Gestione Personaggi"), content, "Seleziona", "Altro");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT id, name FROM `characters` WHERE account_id = '%d'", Account_GetID(playerid));
    return 1;
}

Dialog:Dialog_CharacterSelect(playerid, response, listitem, inputtext[])
{
	SelectedCharacter{playerid} = list_get(Characters[playerid], listitem);
	if(!response)
	{
		inline OnLoad()
		{
			cache_get_value_index(0, 0, TempCharacterName[playerid]);
		}
		MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT name FROM `characters` WHERE id = '%d'", SelectedCharacter{playerid});
		return Dialog_Show(playerid, Dialog_CharCreateOrDel, DIALOG_STYLE_LIST, "Personaggi", "Crea nuovo personaggio\nCancella personaggio", "Avanti", "Indietro");
	}

	inline OnLoad()
	{
		Character_LoadData(playerid);
        //CallLocalFunction(#OnCharacterSelected, "d", playerid);
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `characters` WHERE id = '%d'", SelectedCharacter{playerid});
	return 1;
}

Dialog:Dialog_ShowCharacters(playerid, response, listitem, inputtext[])
{
    if(!Account_IsLogged(playerid))
	   return KickEx(playerid);

    Account_ShowCharactersList(playerid);
    return 1;
}

Dialog:Dialog_CharCreateOrDel(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
	   return Account_ShowCharactersList(playerid);
    }
    if(listitem == 0)
    {
	   Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Crea nuovo personaggio", "Inserisci il nome del nuovo personaggio!\nEsempio: Mario Rossi", "Crea", "Indietro");
    }
    else if(listitem == 1)
    {
	   new randcode = 10000 + random(99999);
	   DeleteCharacterCode[playerid] = randcode;
	   return Dialog_Show(playerid, Dialog_CharacterDelete, DIALOG_STYLE_INPUT, "Cancella Personaggio", "Stai per cancellare il personaggio (%s) definitivamente!\nInserisci il codice sottostante per confermare.\nCodice: %d", "Cancella", "Annulla", 
		  TempCharacterName[playerid], randcode);
    }
    return 1;
}

Dialog:Dialog_CharacterDelete(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
	   return Dialog_Show(playerid, Dialog_CharCreateOrDel, DIALOG_STYLE_LIST, "Personaggi", "Crea nuovo personaggio\nCancella personaggio", "Avanti", "Indietro");
    }
    if(strval(inputtext) == DeleteCharacterCode[playerid])
    {
	   Character_Delete(playerid, SelectedCharacter{playerid}, TempCharacterName[playerid]);
	   DeleteCharacterCode[playerid] = 0;
	   Account_ShowCharactersList(playerid);
	   return 1;
    }
    return Dialog_Show(playerid, Dialog_CharacterDelete, DIALOG_STYLE_INPUT, "Cancella Personaggio", "{FF0000}Codice inserito non è corretto!\n{FFFFFF}Stai per cancellare il personaggio (%s) definitivamente!\nInserisci il codice sottostante per confermare.\nCodice: %d", "Cancella", "Annulla", 
		  TempCharacterName[playerid], DeleteCharacterCode[playerid]);
}


Dialog:Dialog_CreateNewChar(playerid, response, listitem, inputtext[])
{
	if(!Account_IsLogged(playerid))
		return KickEx(playerid);

	if(!response)
		return Account_ShowCharactersList(playerid);

    new 
		len = strlen(inputtext),
		retFirst[MAX_PLAYER_NAME],
		retLast[MAX_PLAYER_NAME],
		characterName[MAX_PLAYER_NAME];

	if(len > 22)
	{
		return Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Crea nuovo personaggio", "{FF0000}La lunghezza massima del nome è 22!\n{FFFFFF}Inserisci il nome del nuovo personaggio.\n{00FF00}Esempio: Mario Rossi", "Crea", "Indietro");
	}

	inputlength(inputtext, MAX_PLAYER_NAME);

	for(new i = 0; i < len; ++i)
	{
		characterName[i] = (inputtext[i] == ' ') ? (characterName[i] = '_') : (characterName[i] = inputtext[i]); // () crashes compiler porca madonna
	}

	if(!IsValidRPName(characterName, retFirst, retLast))
	{
		Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Crea nuovo personaggio", "{FF0000}Il nome del nuovo personaggio non è del formato giusto.\n{00FF00}Esempio: Mario Rossi", "Crea", "Indietro");
	}
	else
	{
		inline OnCheck()
		{
			if(cache_num_rows() > 0)
			{
				Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Crea nuovo personaggio", "{FF0000}Esiste già un personaggio con questo nome!\n{FFFFFF}Inserisci il nome del nuovo personaggio.\n{00FF00}Esempio: Mario Rossi", "Crea", "Indietro");
			}
			else
			{
				set(TempCharacterName[playerid], characterName);
				Dialog_Show(playerid, Dialog_ConfirmNewCharacter, DIALOG_STYLE_MSGBOX, "Crea nuovo personaggio", "Sei sicuro di voler creare questo personaggio?\nNome: %s.", "Crea", "Indietro", TempCharacterName[playerid]);
			}
		}
		MySQL_TQueryInline(gMySQL, using inline OnCheck, "SELECT id FROM `characters` WHERE LOWER(name) = LOWER('%e')", characterName);
	}
	return 1;
}

Dialog:Dialog_ConfirmNewCharacter(playerid, response, listitem, inputtext[])
{
	if(!response)
	{
		return Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Crea nuovo personaggio", "Inserisci il nome del nuovo personaggio!\nEsempio: Mario Rossi", "Crea", "Indietro");
	}
	Account_CreateCharacter(playerid, TempCharacterName[playerid]);
	Dialog_Show(playerid, Dialog_ShowCharacters, DIALOG_STYLE_MSGBOX, "Personaggio Creato", "Il personaggio è stato creato con successo.", "Continua", "");
	return 1;
}
