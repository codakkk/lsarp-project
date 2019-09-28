
#include <YSI_Coding\y_hooks>

static 
    SelectedCharacter[MAX_PLAYERS],
    TempCharacterName[MAX_PLAYERS][MAX_PLAYER_NAME],
    List:Characters[MAX_PLAYERS],
    DeleteCharacterCode[MAX_PLAYERS] = {0, ...};

static
	PlayerText:CharacterModelTextdraw[MAX_PLAYERS][9],
    PlayerText:CharacterNameTextdraw[MAX_PLAYERS][9],
    PlayerText:CharacterLevelTextdraw[MAX_PLAYERS][9]
;

hook OnAccountLogin(playerid)
{
    Characters[playerid] = list_new();
    Account_ShowCharactersList(playerid);
    return 1;
}

hook OnPlayerRequestClass(playerid, classid)
{
	TogglePlayerSpectating(playerid, true);
	defer PositionCamera(playerid);
	return 1;
}

hook OnPlayerClickPlayerTD(playerid, PlayerText:playertextid)
{
    for(new i = 0; i < 9; ++i)
    {
        if(playertextid == CharacterModelTextdraw[playerid][i])
        {
			new characterid = list_get(Characters[playerid], i);
			printf("Slot: %d - Character ID: %d", i, characterid);
            Account_LoadCharacter(playerid, characterid);
			for(new x = 0; x < 9; ++x)
			{
				PlayerTextDrawHide(playerid, CharacterModelTextdraw[playerid][x]);
				PlayerTextDrawHide(playerid, CharacterNameTextdraw[playerid][x]);
				PlayerTextDrawHide(playerid, CharacterLevelTextdraw[playerid][x]);
			}
            CancelSelectTextDraw(playerid);
            return 1;
        }
    }
    return 0;
}

timer PositionCamera[100](playerid) 
{
	SetPlayerCameraPos(playerid, 2115.4441,2131.3276,32.3443);
    SetPlayerCameraLookAt(playerid, 2174.6531,2141.4639,16.6471);
	CheckAccount(playerid);
	return 1;
}

hook OnPlayerConnect(playerid)
{
	//CheckAccount(playerid);

	//TogglePlayerSpectating(playerid, true);
	
	

    new 
        i = 0,
        Float:startX = 150.0000, 
        Float:startY = 80.0000, 
        Float:gapX = 105.0, 
        Float:gapY = 105.0, 
        Float:textStartX = startX + 2.5,
        Float:textStartY = startY + 2.5;
    for(new y = 0; y < 3; ++y)
    {
        for(new x = 0; x < 3; ++x)
        {
            i = x + y * 3;
            CharacterModelTextdraw[playerid][i] = CreatePlayerTextDraw(playerid, startX + x * gapX, startY + y * gapY, "MODEL");
            PlayerTextDrawFont(playerid, CharacterModelTextdraw[playerid][i], TEXT_DRAW_FONT_MODEL_PREVIEW );
            PlayerTextDrawLetterSize(playerid, CharacterModelTextdraw[playerid][i], 0.5000, 1.0000);
            PlayerTextDrawColor(playerid, CharacterModelTextdraw[playerid][i], -1);
            PlayerTextDrawSetShadow(playerid, CharacterModelTextdraw[playerid][i], 0);
            PlayerTextDrawSetOutline(playerid, CharacterModelTextdraw[playerid][i], 0);
            PlayerTextDrawBackgroundColor(playerid, CharacterModelTextdraw[playerid][i], 0x000000AA);
            PlayerTextDrawSetProportional(playerid, CharacterModelTextdraw[playerid][i], 1);
            PlayerTextDrawTextSize(playerid, CharacterModelTextdraw[playerid][i], 100.0000, 100.0000);
            PlayerTextDrawSetPreviewModel(playerid, CharacterModelTextdraw[playerid][i], 46);
            PlayerTextDrawSetPreviewRot(playerid, CharacterModelTextdraw[playerid][i], 0.0000, 0.0000, 0.0000, 1.0000);
            PlayerTextDrawSetSelectable(playerid, CharacterModelTextdraw[playerid][i], 1);

            CharacterNameTextdraw[playerid][i] = CreatePlayerTextDraw(playerid, textStartX + x * gapX, textStartY + y * gapY, "Peppe Musicco");
            PlayerTextDrawFont(playerid, CharacterNameTextdraw[playerid][i], 1);
            PlayerTextDrawLetterSize(playerid, CharacterNameTextdraw[playerid][i], 0.1399, 0.8999);
            PlayerTextDrawColor(playerid, CharacterNameTextdraw[playerid][i], -1);
            PlayerTextDrawSetShadow(playerid, CharacterNameTextdraw[playerid][i], 0);
            PlayerTextDrawSetOutline(playerid, CharacterNameTextdraw[playerid][i], 0);
            PlayerTextDrawBackgroundColor(playerid, CharacterNameTextdraw[playerid][i], 255);
            PlayerTextDrawSetProportional(playerid, CharacterNameTextdraw[playerid][i], 1);
            PlayerTextDrawUseBox(playerid, CharacterNameTextdraw[playerid][i], 0);
            PlayerTextDrawBoxColor(playerid, CharacterNameTextdraw[playerid][i], 0);
            PlayerTextDrawTextSize(playerid, CharacterNameTextdraw[playerid][i], (201.5000 + (i * gapX)), 0.0000);

            CharacterLevelTextdraw[playerid][i] = CreatePlayerTextDraw(playerid, textStartX + x * gapX, textStartY + y * gapY + 10.0, "Livello 100");
            PlayerTextDrawFont(playerid, CharacterLevelTextdraw[playerid][i], 1);
            PlayerTextDrawLetterSize(playerid, CharacterLevelTextdraw[playerid][i], 0.1399, 0.8999);
            PlayerTextDrawColor(playerid, CharacterLevelTextdraw[playerid][i], -1);
            PlayerTextDrawSetShadow(playerid, CharacterLevelTextdraw[playerid][i], 0);
            PlayerTextDrawSetOutline(playerid, CharacterLevelTextdraw[playerid][i], 0);
            PlayerTextDrawBackgroundColor(playerid, CharacterLevelTextdraw[playerid][i], 255);
            PlayerTextDrawSetProportional(playerid, CharacterLevelTextdraw[playerid][i], 1);
            PlayerTextDrawUseBox(playerid, CharacterLevelTextdraw[playerid][i], 0);
            PlayerTextDrawBoxColor(playerid, CharacterLevelTextdraw[playerid][i], 0);
            PlayerTextDrawTextSize(playerid, CharacterLevelTextdraw[playerid][i], (201.5000 + (i * gapX)), 0.0000);
        }
    }
	return 1;
}

hook OnCharacterClearData(playerid)
{
    if(list_valid(Characters[playerid]))
	{
		list_delete_deep(Characters[playerid]);
	}
    return 1;
}

hook OnPlayerUpdate(playerid)
{
	if(Character_IsLogged(playerid))
		return 1;
	SelectTextDraw(playerid, 0xFFFFFFAA);
	return 1;
}

stock Account_ShowCharactersList(playerid)
{
    if(!Account_IsLogged(playerid))
	   return 0;
	new f[128];
	mysql_format(gMySQL, f, sizeof(f), "SELECT id, name, skin FROM `characters` WHERE account_id = '%d'", Account_GetID(playerid));
	mysql_tquery(gMySQL, f, #OnLoadCharactersList, "d", playerid);
    return 1;
}

stock Account_LoadCharacter(playerid, characterid)
{
	if(characterid <= 0)
		return KickEx(playerid);

	new f[128];
	mysql_format(gMySQL, f, sizeof(f), "SELECT * FROM `characters` WHERE id = '%d'", characterid);
	mysql_tquery(gMySQL, f, #OnLoadCharacter, "d", playerid);
	return 1;
}

forward OnLoadCharacter(playerid);
public OnLoadCharacter(playerid)
{
	if(cache_num_rows() > 0)
	{
		Character_LoadData(playerid);

		SendFormattedMessage(playerid, -1, "SERVER: Benvenuto %s", Character_GetOOCName(playerid));
		GameTextForPlayerStr(playerid, str_format("Benvenuto~n~~y~~h~%s", Character_GetOOCName(playerid)), 2500, 1);
	}
	else KickEx(playerid);
	return 1;
}

forward OnLoadCharactersList(playerid);
public OnLoadCharactersList(playerid)
{
	new count = cache_num_rows();
	if(count == 0)
	{
		Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Crea nuovo personaggio", "Attualmente non risultano personaggi creati.\nPer poter giocare, devi disporre di un personaggio.\nInserisci il nome del nuovo personaggio.\nEsempio: Mario Rossi", "Ok", "");
		return 1;
	}
	
	list_clear(Characters[playerid]);

	new
		String:content,
		nameTemp[24], 
		character_id = 0,
		skinid = 0;
	if(count > 9)
		count = 9;

	for(new i = 0; i < count; ++i)
	{
		cache_get_value_name_int(i, "id", character_id);
		cache_get_value_name(i, "name", nameTemp, sizeof(nameTemp));
		cache_get_value_name_int(i, "skin", skinid);
		
		content += str_format("%s\n", nameTemp);

		PlayerTextDrawSetPreviewModel(playerid, CharacterModelTextdraw[playerid][i], skinid);
		PlayerTextDrawSetString(playerid, CharacterNameTextdraw[playerid][i], nameTemp);
		PlayerTextDrawSetString(playerid, CharacterLevelTextdraw[playerid][i], "Livello 1");
	
		PlayerTextDrawShow(playerid, CharacterModelTextdraw[playerid][i]);
		PlayerTextDrawShow(playerid, CharacterNameTextdraw[playerid][i]);
		PlayerTextDrawShow(playerid, CharacterLevelTextdraw[playerid][i]);

		printf("Character id: %d", character_id);
		list_add(Characters[playerid], character_id);
	}
	SelectTextDraw(playerid, 0xFFFFFFAA);
	return 1;//Dialog_Show_s(playerid, Dialog_CharacterSelect, DIALOG_STYLE_LIST, @("Gestione Personaggi"), content, "Seleziona", "Altro");
}

Dialog:Dialog_CharacterSelect(playerid, response, listitem, inputtext[])
{
	SelectedCharacter[playerid] = list_get(Characters[playerid], listitem);
	
	if(SelectedCharacter[playerid] <= 0)
		return KickEx(playerid);
	
	if(!response)
	{
		TempCharacterName[playerid] = Character_GetNameFromDatabase(SelectedCharacter[playerid], false);
		return Dialog_Show(playerid, Dialog_CharCreateOrDel, DIALOG_STYLE_LIST, "Personaggi", "Crea nuovo personaggio\nCancella personaggio", "Avanti", "Indietro");
	}
	printf("Character id: %d", SelectedCharacter[playerid]);
	Account_LoadCharacter(playerid, SelectedCharacter[playerid]);
	return 1;
}

Dialog:Dialog_ShowCharacters(playerid, response, listitem, inputtext[])
{
    if(!Account_IsLogged(playerid))
	   return KickEx(playerid);

    Account_ShowCharactersList(playerid);
    return 1;
}

Dialog:Dialog_MaxCharsNotify(playerid, response, listitem, inputtext[])
{
	if(!Account_IsLogged(playerid))
		return KickEx(playerid), 0;
	return Account_ShowCharactersList(playerid);
}


Dialog:Dialog_CharCreateOrDel(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
	   return Account_ShowCharactersList(playerid);
    }
    if(listitem == 0)
    {
		inline OnLoad()
		{
			new rows = 0;
			cache_get_value_index_int(0, 0, rows);
			
			if(rows >= 3 + Account_GetCharactersSlot(playerid))
			{
				Dialog_Show(playerid, Dialog_MaxCharsNotify, DIALOG_STYLE_MSGBOX, "Limite Raggiunto", "Hai raggiunto il limite di personaggi creabili.\nAcquista il Premium o degli slot personaggio tramite /zpoints.\nPer maggiori informazioni visita www.lsarp.it", "Ok", "");
			}
			else
			{
				Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Crea nuovo personaggio", "Inserisci il nome del nuovo personaggio!\nEsempio: Mario Rossi", "Crea", "Indietro");
			}
		}
		MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT COUNT(*) FROM `characters` WHERE account_id = '%d'", Account_GetID(playerid));
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
	   Character_Delete(playerid, SelectedCharacter[playerid], TempCharacterName[playerid]);
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
	Dialog_Show(playerid, Dialog_ChooseSkin, DIALOG_STYLE_INPUT, "Creazione Personaggio", "Adesso inserisci la skin con cui vuoi iniziare\nDefault: 46.", "Continua", "");
	return 1;
}

Dialog:Dialog_ChooseSkin(playerid, response, listitem, inputtext[])
{
	new val = strval(inputtext);
	if(isnull(inputtext) || strlen(inputtext) <= 0)
		val = 46;
	if(!IsValidSkin(val))
		return Dialog_Show(playerid, Dialog_ChooseSkin, DIALOG_STYLE_INPUT, "Personaggio Creato", "La skin id inserita non è valida.\nIl personaggio è stato creato con successo.", "Continua", "");
	Account_CreateCharacter(playerid, TempCharacterName[playerid], val);
	Dialog_Show(playerid, Dialog_ShowCharacters, DIALOG_STYLE_MSGBOX, "Personaggio Creato", "Il personaggio è stato creato con successo.", "Continua", "");
	return 1;
}

