
#include <YSI_Coding\y_hooks>

static stock
    SelectedCharacter[MAX_PLAYERS],
    TempCharacterName[MAX_PLAYERS][MAX_PLAYER_NAME],
    List:Characters[MAX_PLAYERS],
    DeleteCharacterCode[MAX_PLAYERS] = {0, ...};

static
	PlayerText:CharacterModelTextdraw[MAX_PLAYERS][9],
    PlayerText:CharacterNameTextdraw[MAX_PLAYERS][9],
    PlayerText:CharacterLevelTextdraw[MAX_PLAYERS][9],
	PlayerText:CharacterStateTextdraw[MAX_PLAYERS][9]
;

hook OnAccountLogin(playerid)
{
    Characters[playerid] = list_new();
    Account_ShowCharactersList(playerid);
    return 1;
}

hook OnPlayerRequestClass(playerid, classid)
{
	if(!Character_IsLogged(playerid))
	{
		TogglePlayerSpectating(playerid, true);
		defer PositionCamera(playerid);
	}
	return 1;
}

hook OnPlayerClickPlayerTD(playerid, PlayerText:playertextid)
{
    for(new i = 0; i < 9; ++i)
    {
        if(playertextid == CharacterModelTextdraw[playerid][i])
        {
			new characterid = list_get(Characters[playerid], i);
            Account_LoadCharacter(playerid, characterid);
			for(new x = 0; x < 9; ++x)
			{
				PlayerTextDrawHide(playerid, CharacterModelTextdraw[playerid][x]);
				PlayerTextDrawHide(playerid, CharacterNameTextdraw[playerid][x]);
				PlayerTextDrawHide(playerid, CharacterLevelTextdraw[playerid][x]);
				PlayerTextDrawHide(playerid, CharacterStateTextdraw[playerid][x]);
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
            PlayerTextDrawSetSelectable(playerid, CharacterModelTextdraw[playerid][i], 0);

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

			CharacterStateTextdraw[playerid][i] = CreatePlayerTextDraw(playerid, textStartX + x * gapX, textStartY + y * gapY + 20.0, "");
            PlayerTextDrawFont(playerid, CharacterStateTextdraw[playerid][i], 1);
            PlayerTextDrawLetterSize(playerid, CharacterStateTextdraw[playerid][i], 0.1399, 0.8999);
            PlayerTextDrawColor(playerid, CharacterStateTextdraw[playerid][i], -1);
            PlayerTextDrawSetShadow(playerid, CharacterStateTextdraw[playerid][i], 0);
            PlayerTextDrawSetOutline(playerid, CharacterStateTextdraw[playerid][i], 0);
            PlayerTextDrawBackgroundColor(playerid, CharacterStateTextdraw[playerid][i], 255);
            PlayerTextDrawSetProportional(playerid, CharacterStateTextdraw[playerid][i], 1);
            PlayerTextDrawUseBox(playerid, CharacterStateTextdraw[playerid][i], 0);
            PlayerTextDrawBoxColor(playerid, CharacterStateTextdraw[playerid][i], 0);
            PlayerTextDrawTextSize(playerid, CharacterStateTextdraw[playerid][i], (201.5000 + (i * gapX)), 0.0000);
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
	   return KickEx(playerid);

	await mysql_aquery_s(gMySQL, str_format("SELECT id, name, skin, level, banned, ban_expiry, validated_at FROM `characters` WHERE account_id = '%d'", Account_GetID(playerid)), true);
	
	new count = cache_num_rows();
	if(count <= 0)
	{
		//Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Crea nuovo personaggio", "Attualmente non risultano personaggi creati.\nPer poter giocare, devi disporre di un personaggio.\nInserisci il nome del nuovo personaggio.\nEsempio: Mario Rossi", "Ok", "");
		SendClientMessage(playerid, COLOR_ORANGE, "SERVER: Il tuo account non risulta completato e/o convalidato.");
		SendClientMessage(playerid, COLOR_ORANGE, "SERVER: Effettua il log-in nel tuo account su ucp.lv-rp.it/ e riprova.");
		return KickEx(playerid), 0;
	}
	
	list_clear(Characters[playerid]);

	new 
		String:content = str_new(""), nameTemp[24], character_id = 0, skinid = 0, level = 1,
		bool:isValidated = false, isBanned, banExpiry;

	if(count > 9)
		count = 9;

	for(new i = 0; i < count; ++i)
	{
		cache_get_value_name_int(i, "id", character_id);
		cache_get_value_name(i, "name", nameTemp, sizeof(nameTemp));
		cache_get_value_name_int(i, "skin", skinid);
		cache_get_value_name_int(i, "level", level);
		cache_is_value_name_null(i, "validated_at", isValidated);
		cache_get_value_name_int(i, "banned", isBanned);
		cache_get_value_name_int(i, "ban_expiry", banExpiry);
		
		isValidated = !isValidated;
		str_append_format(content, "%s\n", nameTemp);

		PlayerTextDrawSetPreviewModel(playerid, CharacterModelTextdraw[playerid][i], skinid);
		PlayerTextDrawSetString(playerid, CharacterNameTextdraw[playerid][i], nameTemp);

		format(nameTemp, sizeof(nameTemp), "Livello %d", level);
		PlayerTextDrawSetString(playerid, CharacterLevelTextdraw[playerid][i], nameTemp);
	
		
		
		if(!isValidated)
		{
			PlayerTextDrawColor(playerid, CharacterModelTextdraw[playerid][i], 0x55555599);
			PlayerTextDrawSetString(playerid, CharacterStateTextdraw[playerid][i], "~h~~h~~h~~p~Non Convalidato~w~");
			PlayerTextDrawSetSelectable(playerid, CharacterModelTextdraw[playerid][i], 0);
		}
		else if(isBanned == 1 && gettime() < banExpiry)
		{
			PlayerTextDrawColor(playerid, CharacterModelTextdraw[playerid][i], 0x55555599);
			PlayerTextDrawSetStringStr(playerid, CharacterStateTextdraw[playerid][i], str_format("~r~Bannato~w~~n~Fine: %s", date(banExpiry)));
			PlayerTextDrawSetSelectable(playerid, CharacterModelTextdraw[playerid][i], 0);
		}
		else if(isBanned == 2)
		{
			PlayerTextDrawColor(playerid, CharacterModelTextdraw[playerid][i], 0x55555599);
			PlayerTextDrawSetString(playerid, CharacterStateTextdraw[playerid][i], "~p~Freezato~w~");
			PlayerTextDrawSetSelectable(playerid, CharacterModelTextdraw[playerid][i], 0);
		}
		else
		{
			PlayerTextDrawColor(playerid, CharacterModelTextdraw[playerid][i], -1);
			PlayerTextDrawSetString(playerid, CharacterStateTextdraw[playerid][i], "~g~Attivo~w~");
			PlayerTextDrawSetSelectable(playerid, CharacterModelTextdraw[playerid][i], 1);
		}

		PlayerTextDrawShow(playerid, CharacterModelTextdraw[playerid][i]);
		PlayerTextDrawShow(playerid, CharacterNameTextdraw[playerid][i]);
		PlayerTextDrawShow(playerid, CharacterLevelTextdraw[playerid][i]);
		PlayerTextDrawShow(playerid, CharacterStateTextdraw[playerid][i]);

		list_add(Characters[playerid], character_id);
	}
	SelectTextDraw(playerid, 0xFFFFFFAA);
    return 1;
}

stock Account_LoadCharacter(playerid, characterid)
{
	if(characterid <= 0)
		return KickEx(playerid);
	inline OnLoadCharacter()
	{
		if(cache_num_rows() <= 0)
			return KickEx(playerid);
	
		Character_LoadData(playerid);

		SendFormattedMessage(playerid, -1, "SERVER: Benvenuto %s", Character_GetOOCName(playerid));
		GameTextForPlayerStr(playerid, str_format("Benvenuto~n~~y~~h~%s", Character_GetOOCName(playerid)), 2000, 1);
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoadCharacter, "SELECT * FROM `characters` WHERE id = '%d'", characterid);
	return 1;
}