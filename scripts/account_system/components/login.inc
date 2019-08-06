
#include <YSI_Coding\y_hooks>

#define MAX_LOGIN_ATTEMPTS	(3)


static 
    AccountEMailCode[MAX_PLAYERS],
	TempMail[MAX_PLAYERS][255];

//
static Timer:LoginTimer[MAX_PLAYERS];

hook OnPlayerDisconnect(playerid, reason)
{
	LoginTimer[playerid] = Timer:0;
	return 1;
}

hook OnPlayerFinishedDwnling(playerid, virtualworld)
{
	if(!Account_IsLogged(playerid))
	{
		SendClientMessage(playerid, -1, "Hai 60 secondi per registrarti o effettuare il login prima di essere kickato.");
		LoginTimer[playerid] = defer LoginKickTimer(playerid);
	}
	return 1;
}

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
			if(strlen(Account_GetEMail(playerid)) <= 0 || !IsValidEMail(Account_GetEMail(playerid)))
			{
				Dialog_Show(playerid, Dialog_EMail, DIALOG_STYLE_INPUT, "E-Mail", "{FF0000}Non hai ancora inserito una e-mail valida!{FFFFFF}\nPer poter giocare su questo server hai bisogno di una e-mail valida!\n{FFFFFF}Inserisci una mail per continuare.", "Continua", "Esci");
				stop LoginTimer[playerid];
				LoginTimer[playerid] = Timer:0;
			}
			else
			{
				ExecuteLogin(playerid);
			}
			#else
				ExecuteLogin(playerid);
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

	MySQL_TQueryInline(gMySQL, using inline OnCheck, "SELECT * FROM accounts WHERE LOWER(name) = LOWER('%e') AND password = '%e'", name, psw);
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

	set(TempMail[playerid], inputtext);
	inline OnCheck()
	{
		new rows = cache_num_rows();
		if(rows > 0)
			return Dialog_Show(playerid, Dialog_EMail, DIALOG_STYLE_INPUT, "Registrazione E-Mail", "{FF0000}L'email inserita è già utilizzata!{FFFFFF}\nPer poter giocare su questo server hai bisogno di una e-mail valida!\n{FFFFFF}Inserisci una mail per continuare.", "Registra", "Esci");
		else
		{
			AccountEMailCode[playerid] = 10000 + random(99999);

			new content[256];
			format(content, sizeof(content), "Di seguito il codice per poter verificare la tua e-mail\nCodice <b>%d</b>", AccountEMailCode[playerid]);
			SendMail(TempMail[playerid], "Codice di Verifica", content);
			
			Dialog_Show(playerid, Dialog_EMailConfirm, DIALOG_STYLE_INPUT, "Conferma E-Mail", "{FFFFFF}E' stata inviata un e-mail all'indirizzo {00FF00}'%s'{FFFFFF} con il codice di verifica.\nInserisci il codice di verifica per attivare il tuo account.\n{FF0000}ATTENZIONE: Potrebbe volerci qualche minuto per ricevere la e-mail.", "Verifica", "Esci", TempMail[playerid]);
		}
	}
	MySQL_TQueryInline(gMySQL, using inline OnCheck, "SELECT ID FROM `accounts` WHERE email = '%e'", inputtext);
	return 1;
}

Dialog:Dialog_EMailConfirm(playerid, response, listitem, inputtext[])
{
	if(!response)
		return KickEx(playerid);
	new code = strval(inputtext);
	if(code != AccountEMailCode[playerid])
	{
		return Dialog_Show(playerid, Dialog_EMailConfirm, DIALOG_STYLE_INPUT, "Conferma E-Mail", "Il codice inserito non è valido!\nInserisci il codice inviato all'email '%s' per verificare il tuo account\ned iniziare a giocare!'", "Verifica", "Esci", TempMail[playerid]);
	}

	inline OnUpdate()
	{
		Account_SetEMail(playerid, TempMail[playerid]);
		SendClientMessage(playerid, COLOR_GREEN, "Hai verificato la tua e-mail.");
		ExecuteLogin(playerid);
	}
	MySQL_TQueryInline(gMySQL, using inline OnUpdate, "UPDATE `accounts` SET email = '%e' WHERE id = '%d'", TempMail[playerid], Account_GetID(playerid));
	return 1;
}


stock ExecuteLogin(playerid)
{
	Account_SetLogged(playerid, true);

	stop LoginTimer[playerid];
	LoginTimer[playerid] = Timer:0;
    
	if(Account_IsBanned(playerid))
	{
		if(!Account_GetBanExpiry(playerid) || gettime() < Account_GetBanExpiry(playerid))
		{
			Dialog_Show(playerid, DialogNull, DIALOG_STYLE_MSGBOX, "Account Bannato", "Questo account risulta bannato.\nSe pensi sia un errore, visita www.lsarp.it", "Ok", "");
			KickEx(playerid);
			return 0;
		}
		else
		{
			Account_SetBanned(playerid, 0);
			Account_SetBanExpiry(playerid, 0);
			Account_Save(playerid);
		}
	}

	Account_LoadRewards(playerid);

    inline OnLoad()
    {
       CallLocalFunction(#OnAccountLogin, "d", playerid);
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT id FROM characters WHERE account_id = '%d'", Account_GetID(playerid));
	return 1;
}