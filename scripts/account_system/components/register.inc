#include <YSI_Coding\y_hooks>

Dialog:Dialog_Register(playerid, response, listitem, inputtext[])
{
	if(!response)
		return KickEx(playerid);
	if(strlen(inputtext) < 6 || strlen(inputtext) > 16)
		return Dialog_Show(playerid, Dialog_Register, DIALOG_STYLE_PASSWORD, "Account non registrato", "{FF0000}La password inserita è troppo lunga o troppo corta.{FFFFFF}\nInserisci una password per registrarti.", "Registra", "Annulla");
	
	
	inline OnInsert()
	{
		CallLocalFunction(#OnAccountCreated, "dd", playerid, cache_insert_id());
		CheckAccount(playerid);
	}

	new 
		psw[WHIRLPOOL_LEN],
		name[24];
	WP_Hash(psw, sizeof(psw), inputtext);

	GetPlayerName(playerid, name, sizeof(name));

	MySQL_TQueryInline(gMySQL, using inline OnInsert, "INSERT INTO `accounts` (name, password, admin_level, premium, premium_expiry, characters_counter, email) \
	VALUES('%e', '%e', '0', '0', '0', '0', '')", name, psw);
	return 1;
}