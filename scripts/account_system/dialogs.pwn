#include <YSI_Coding\y_hooks>

Dialog:Dialog_Login(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
	   Kick(playerid);
	   return 1;
    }
    inline OnCheck()
    {
		// First check for email
		if(LoadAccountData(playerid))
		{
			#if defined E_MAIL_CHECK
			if(strlen(AccountInfo[playerid][aEMail]) <= 0 || !IsValidEMail(AccountInfo[playerid][aEMail]))
			{
				Dialog_Show(playerid, Dialog_EMail, DIALOG_STYLE_INPUT, "E-Mail", "{FF0000}Non hai ancora inserito una e-mail valida!{FFFFFF}\nPer poter giocare su questo server hai bisogno di una e-mail valida!\n{FFFFFF}Inserisci una mail per continuare.", "Continua", "Esci");
				stop gLoginTimer[playerid];
				gLoginTimer[playerid] = Timer:0;
			}
			else
			{
				OnAccountLogin(playerid);
			}
			#else
				OnAccountLogin(playerid);
			#endif
		}
		else
		{
			gLoginAttempts{playerid}++;

			if(gLoginAttempts{playerid} > MAX_LOGIN_ATTEMPTS)
			{
				return KickEx(playerid);
			}
			
			Dialog_Show(playerid, Dialog_Login, DIALOG_STYLE_PASSWORD, "Login", "{FF0000}Password errata.\n{FFFFFF}Inserisci la tua password per effettuare il login.\nTentativo: {FF0000}%d/%d{FFFFFF}", "Login", "Esci", gLoginAttempts{playerid}, MAX_LOGIN_ATTEMPTS);
		}
	}
	new 
		psw[WHIRLPOOL_LEN], 
		name[24];
	WP_Hash(psw, sizeof(psw), inputtext);

	GetPlayerName(playerid, name, sizeof(name));

	MySQL_TQueryInline(gMySQL, using inline OnCheck, "SELECT * FROM accounts WHERE LOWER(Name) = LOWER('%e') AND Password = '%e'", name, psw);
	return 1;
}

Dialog:Dialog_Register(playerid, response, listitem, inputtext[])
{
	if(!response)
		return KickEx(playerid);
	if(strlen(inputtext) < 6 || strlen(inputtext) > 16)
		return Dialog_Show(playerid, Dialog_Register, DIALOG_STYLE_PASSWORD, "Account non registrato", "{FF0000}La password inserita è troppo lunga o troppo corta.{FFFFFF}\nInserisci una password per registrarti.", "Registra", "Annulla");
	
	
	inline OnInsert()
	{
		OnAccountCreated(playerid);
		CheckAccount(playerid);
	}

	new 
		psw[WHIRLPOOL_LEN],
		name[24];
	WP_Hash(psw, sizeof(psw), inputtext);

	GetPlayerName(playerid, name, sizeof(name));

	MySQL_TQueryInline(gMySQL, using inline OnInsert, "INSERT INTO `accounts` (Name, Password, Admin, Premium, PremiumExpiry, CharactersCounter, EMail) \
	VALUES('%e', '%e', '0', '0', '0', '0', '')", name, psw);
	return 1;
}

Dialog:Dialog_EMail(playerid, response, listitem, inputtext[])
{
	if(!response)
		return Kick(playerid);
	
	if(!IsValidEMail(inputtext))
	{
		return Dialog_Show(playerid, Dialog_EMail, DIALOG_STYLE_INPUT, "Registrazione E-Mail", "{FF0000}L'email inserita non è valida!.{FFFFFF}\nPer poter giocare su questo server hai bisogno di una e-mail valida!\n{FFFFFF}Inserisci una mail per continuare.", "Registra", "Esci");
	}

	set(gAccountTempMail[playerid], inputtext);
	inline OnCheck()
	{
		new rows = cache_num_rows();
		if(rows > 0)
			return Dialog_Show(playerid, Dialog_EMail, DIALOG_STYLE_INPUT, "Registrazione E-Mail", "{FF0000}L'email inserita è già utilizzata!{FFFFFF}\nPer poter giocare su questo server hai bisogno di una e-mail valida!\n{FFFFFF}Inserisci una mail per continuare.", "Registra", "Esci");
		else
		{
			gAccountEmailCode[playerid] = 10000 + random(99999);

			new content[256];
			format(content, sizeof(content), "Di seguito il codice per poter verificare la tua e-mail\nCodice <b>%d</b>", gAccountEmailCode[playerid]);
			SendMail(gAccountTempMail[playerid], "Codice di Verifica", content);
			
			Dialog_Show(playerid, Dialog_EMailConfirm, DIALOG_STYLE_INPUT, "Conferma E-Mail", "{FFFFFF}E' stata inviata un e-mail all'indirizzo {00FF00}'%s'{FFFFFF} con il codice di verifica.\nInserisci il codice di verifica per attivare il tuo account.\n{FF0000}ATTENZIONE: Potrebbe volerci qualche minuto per ricevere la e-mail.", "Verifica", "Esci", gAccountTempMail[playerid]);
		}
	}
	MySQL_TQueryInline(gMySQL, using inline OnCheck, "SELECT ID FROM `accounts` WHERE EMail = '%e'", inputtext);
	return 1;
}

Dialog:Dialog_EMailConfirm(playerid, response, listitem, inputtext[])
{
	if(!response)
		return KickEx(playerid);
	new code = strval(inputtext);
	if(code != gAccountEmailCode[playerid])
	{
		return Dialog_Show(playerid, Dialog_EMailConfirm, DIALOG_STYLE_INPUT, "Conferma E-Mail", "Il codice inserito non è valido!\nInserisci il codice inviato all'email '%s' per verificare il tuo account\ned iniziare a giocare!'", "Verifica", "Esci", gAccountTempMail[playerid]);
	}

	inline OnUpdate()
	{
		set(AccountInfo[playerid][aEMail], gAccountTempMail[playerid]);
		SendClientMessage(playerid, COLOR_GREEN, "Hai verificato la tua e-mail.");
		OnAccountLogin(playerid);
	}
	MySQL_TQueryInline(gMySQL, using inline OnUpdate, "UPDATE `accounts` SET EMail = '%e' WHERE ID = '%d'", gAccountTempMail[playerid], AccountInfo[playerid][aID]);
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
	   gCharacterCreationStep{playerid} = 0;
	   Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Crea nuovo personaggio", "Inserisci il nome del nuovo personaggio!\nEsempio: Mario Rossi", "Crea", "Indietro");
    }
    else if(listitem == 1)
    {
	   new randcode = 10000 + random(99999);
	   SetPVarInt(playerid, "DeleteCharacter_Code", randcode);
	   return Dialog_Show(playerid, Dialog_CharacterDelete, DIALOG_STYLE_INPUT, "Cancella Personaggio", "Stai per cancellare il personaggio (%s) definitivamente!\nInserisci il codice sottostante per confermare.\nCodice: %d", "Cancella", "Annulla", 
		  gCharacterSelectedName[playerid], randcode);
    }
    return 1;
}

Dialog:Dialog_CharacterDelete(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
	   return Dialog_Show(playerid, Dialog_CharCreateOrDel, DIALOG_STYLE_LIST, "Personaggi", "Crea nuovo personaggio\nCancella personaggio", "Avanti", "Indietro");
    }
    if(strval(inputtext) == GetPVarInt(playerid, "DeleteCharacter_Code"))
    {
	   Character_Delete(playerid, gCharacterSelected{playerid}, gCharacterSelectedName[playerid]);
	   DeletePVar(playerid, "DeleteCharacter_Code");
	   Account_ShowCharactersList(playerid);
	   return 1;
    }
    return Dialog_Show(playerid, Dialog_CharacterDelete, DIALOG_STYLE_INPUT, "Cancella Personaggio", "{FF0000}Codice inserito non è corretto!\n{FFFFFF}Stai per cancellare il personaggio (%s) definitivamente!\nInserisci il codice sottostante per confermare.\nCodice: %d", "Cancella", "Annulla", 
		  gCharacterSelectedName[playerid], GetPVarInt(playerid, "DeleteCharacter_Code"));
}


Dialog:Dialog_CharacterSelect(playerid, response, listitem, inputtext[])
{
	gCharacterSelected{playerid} = AccountInfo[playerid][aCharacters][listitem];
	if(!response)
	{
		inline OnLoad()
		{
			cache_get_value_index(0, 0, gCharacterSelectedName[playerid]);
		}
		MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT Name FROM characters WHERE ID = '%d'", gCharacterSelected{playerid});
		return Dialog_Show(playerid, Dialog_CharCreateOrDel, DIALOG_STYLE_LIST, "Personaggi", "Crea nuovo personaggio\nCancella personaggio", "Avanti", "Indietro");
	}

	inline OnLoad()
	{
		OnCharacterLoad(playerid);
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM characters WHERE ID = '%d'", gCharacterSelected{playerid});
	return 1;
}

Dialog:Dialog_CreateNewChar(playerid, response, listitem, inputtext[])
{
	if(!Account_IsLogged(playerid))
		return KickEx(playerid);

	if(!response)
	{
		gCharacterCreationStep{playerid}--;
		if(gCharacterCreationStep{playerid} == -1)
			CharacterCreation_Reset(playerid);
		switch(gCharacterCreationStep{playerid})
		{
			case 0: return Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Crea nuovo personaggio", "Inserisci il nome del nuovo personaggio!\nEsempio: Mario Rossi", "Avanti", "Indietro");
			case 1: return Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Età Personaggio", "Adesso inserisci l'età del nuovo personaggio!\nN.B: 16 - 75 anni.", "Avanti", "Indietro");
			case 2: return Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_LIST, "Sesso Personaggio", "Maschio\nFemmina", "Crea", "Indietro");
			default: return Account_ShowCharactersList(playerid);
		}
		return 1;
	}

    switch(gCharacterCreationStep{playerid})
    {
		// Name & Surname
		case 0:
		{

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
				Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Crea nuovo personaggio", "{FF0000}Il nome del nuovo personaggio non è del formato giusto!\n{00FF00}Esempio: Mario Rossi", "Crea", "Indietro");
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
						/*if(AccountInfo[playerid][aCharactersCount] >= 3 && AccountInfo[playerid][aPremium] == 0)
						{
						return Dialog_Show(playerid, Dialog_ShowCharacters, DIALOG_STYLE_MSGBOX, "Personaggio Non Creato", "Non è stato possibile creare il personaggio!\nLimite superato.", "Continua", "", fixedCharName);
						}*/
						SetPVarString(playerid, "NewCharacter_Name", characterName);

						// Move to step 2
						gCharacterCreationStep{playerid} = 1;

						Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Età Personaggio", "Adesso inserisci l'età del nuovo personaggio!\nN.B: 16 - 75 anni.", "Avanti", "Indietro");
					}
				}
				MySQL_TQueryInline(gMySQL, using inline OnCheck, "SELECT ID FROM characters WHERE LOWER(Name) = LOWER('%e')", characterName);
				return 1;
			}
		}
	   case 1: // Age Selection
	   {
		  new val = strval(inputtext);
		  if(val < 16 || val > 75)
		  {
			 return Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_INPUT, "Età Personaggio", "Adesso inserisci l'età del nuovo personaggio!\nN.B: 16 - 75 anni.", "Avanti", "Indietro");
		  }
		  gCharacterCreationStep{playerid} = 2;
		  SetPVarInt(playerid, "NewCharacter_Age", val);
		  return Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_LIST, "Sesso Personaggio", "Maschio\nFemmina", "Crea", "Indietro");
	   }
	   case 2: // Sex Selection
	   {
		  gCharacterCreationStep{playerid} = 3;
		  new
				name[MAX_PLAYER_NAME], // Underscored Name Porca_Madonna
				fixedCharName[MAX_PLAYER_NAME], // Not underscored name. Porca Madonna
				age = GetPVarInt(playerid, "NewCharacter_Age")
			 ;
		  SetPVarInt(playerid, "NewCharacter_Sex", listitem);
		  GetPVarString(playerid, "NewCharacter_Name", name, sizeof(name));
		  
		  FixName(name, fixedCharName);

		  Dialog_Show(playerid, Dialog_CreateNewChar, DIALOG_STYLE_MSGBOX, "Riepilogo", "--- Informazioni ---\nNome: %s\nSesso: %s\nEtà: %d\nConfermi?", "Crea", "Annulla", fixedCharName, GetSexName(listitem), age);
		  return 1;
	   }
	   case 3: // Over
	   {
		  if(AccountInfo[playerid][aCharactersCount] >= MAX_CHARACTERS) // Should I prompt this before?
		  {
			 return Dialog_Show(playerid, Dialog_ShowCharacters, DIALOG_STYLE_MSGBOX, "Personaggio Non Creato", "Non è stato possibile creare il personaggio!\nLimite (5) superato.", "Continua", "");
		  }
		  new 
			 query[256],
			 age = GetPVarInt(playerid, "NewCharacter_Age"),
			 sex = GetPVarInt(playerid, "NewCharacter_Sex"),
			 characterName[MAX_PLAYER_NAME];
		  
		  GetPVarString(playerid, "NewCharacter_Name", characterName, sizeof(characterName));

		  mysql_format(gMySQL, query, sizeof(query), "INSERT INTO `characters` (Name, AccountID, Sex, Age, FirstSpawn) VALUES('%e', '%d', '%d', '%d', '1')", characterName, AccountInfo[playerid][aID], sex, age);
		  mysql_tquery(gMySQL, query);

		  Log(AccountInfo[playerid][aName], characterName, "CharacterCreation");
		  CharacterCreation_Reset(playerid);
		  
		  Dialog_Show(playerid, Dialog_ShowCharacters, DIALOG_STYLE_MSGBOX, "Personaggio Creato", "Il personaggio è stato creato con successo.", "Continua", "");
		  return 1;
	   }
    }
    return 1;
}
