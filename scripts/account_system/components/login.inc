#include <samp_bcrypt>
#define BCRYPT_COST 10

#include <YSI_Coding\y_hooks>

#define MAX_LOGIN_ATTEMPTS	(3)


static 
	PlayerHash[MAX_PLAYERS][250],
	LoginAttempts[MAX_PLAYERS char]
    //AccountEMailCode[MAX_PLAYERS],
	//TempMail[MAX_PLAYERS][255]
;

//
static Timer:LoginTimer[MAX_PLAYERS];

hook OnPlayerDisconnect(playerid, reason)
{
	LoginTimer[playerid] = Timer:0;
	LoginAttempts{playerid} = 0;
	return 1;
}

timer ShowLogin[200](playerid)
{
	CheckAccount(playerid);
	return 1;
}

hook OnPlayerConnect(playerid)
{
	inline OnLoadHash()
	{
		if(cache_num_rows() > 0)
		{
			cache_get_value_name(0, "password", PlayerHash[playerid]);
			defer ShowLogin(playerid);
		}
	}
	new name[MAX_PLAYER_NAME];
	GetPlayerName(playerid, name, sizeof(name));
	MySQL_TQueryInline(gMySQL, using inline OnLoadHash, "SELECT password FROM `accounts` WHERE LOWER(name) = LOWER('%e')", name);
	return 1;
}

hook OnPlayerFinishedDwnling(playerid, virtualworld)
{
	if(!Account_IsLogged(playerid))
	{
		//SendClientMessage(playerid, -1, "Hai 60 secondi per registrarti o effettuare il login prima di essere kickato.");
		//LoginTimer[playerid] = defer LoginKickTimer(playerid);
	}
	return 1;
}

Dialog:Dialog_LoginOptions(playerid, response, listitem, inputtext[])
{
	if(!response)
	{
		Dialog_Show(playerid, Dialog_Login, DIALOG_STYLE_PASSWORD, "Login", "{FFFFFF}Inserisci la tua password per effettuare il login.\nTentativo: {FF0000}%d/%d{FFFFFF}", "Login", "Esci", LoginAttempts{playerid}, MAX_LOGIN_ATTEMPTS);
		return 1;
	}
	if(listitem == 0)
	{
	}
	else
		KickEx(playerid);
	return 1;
}


Dialog:Dialog_Login(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
	   //Dialog_Show(playerid, Dialog_LoginOptions, DIALOG_STYLE_LIST, "Altro", "Resetta Password\nEsci", "Seleziona", "Indietro"); 
	   return KickEx(playerid);
    }
	printf("Input: %s - Password: %s", inputtext, PlayerHash[playerid]);
	bcrypt_verify(playerid, #OnPasswordChecked, inputtext, PlayerHash[playerid]);
	return 1;
}

forward OnPasswordChecked(playerid, bool:success);
public OnPasswordChecked(playerid, bool:success)
{
	if(success)
	{
		inline OnCheck()
    	{
			new accountId, isBanned, banTime;
				
			cache_get_value_name_int(0, "id", accountId);
			cache_get_value_name_int(0, "banned", isBanned);
			cache_get_value_name_int(0, "ban_expiry", banTime);

			if(isBanned)
			{
				if(!banTime || gettime() < banTime)
				{
					Dialog_Show(playerid, DialogNull, DIALOG_STYLE_MSGBOX, "Account Bannato", "Questo account risulta bannato.\nSe pensi sia un errore, visita www.lsarp.it", "Ok", "");
					KickEx(playerid);
					return 0;
				}
				else
				{
					mysql_tquery_f(gMySQL, "UPDATE `accounts` SET banned = '0', ban_expiry = '0' WHERE id = '%d'", accountId);
				}
			}
			
			LoadAccountData(playerid);

			ExecuteLogin(playerid);
		}
		new name[MAX_PLAYER_NAME];
		GetPlayerName(playerid, name, sizeof(name));
		MySQL_TQueryInline(gMySQL, using inline OnCheck, "SELECT * FROM accounts WHERE LOWER(name) = LOWER('%e')", name);
	}
	else
	{
		LoginAttempts{playerid}++;
		if(LoginAttempts{playerid} > MAX_LOGIN_ATTEMPTS)
		{
			return KickEx(playerid);
		}
		Dialog_Show(playerid, Dialog_Login, DIALOG_STYLE_PASSWORD, "Login", "{FF0000}Password errata.\n{FFFFFF}Inserisci la tua password per effettuare il login.\nTentativo: {FF0000}%d/%d{FFFFFF}", "Login", "Esci", LoginAttempts{playerid}, MAX_LOGIN_ATTEMPTS);
	}
	return 1;
}

stock ExecuteLogin(playerid)
{
	Account_SetLogged(playerid, true);

	stop LoginTimer[playerid];
	LoginTimer[playerid] = Timer:0;

	CallLocalFunction(#OnAccountLogin, "d", playerid);
    
	/*inline OnLoad()
    {
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT id FROM characters WHERE account_id = '%d'", Account_GetID(playerid));*/
	return 1;
}