#include <YSI_Coding\y_hooks>

enum e_CharacterDamageInfo
{
	damageUsed,
	damageIssuerName[MAX_PLAYER_NAME],
	damageWeapon,
	Float:damageAmount,
	damageBodyPart,
};
new 
	CharacterDamageInfo[MAX_PLAYERS][MAX_DAMAGES_PER_PLAYER][e_CharacterDamageInfo];

hook OnPlayerClearData(playerid)
{
	Character_ResetDamages(playerid);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnCharacterSpawn(playerid)
{
	Character_ResetDamages(playerid);
	Character_SetLegHit(playerid, false);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

/*hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(!Character_IsAlive(playerid))
		return Y_HOOKS_CONTINUE_RETURN_1;
	
	if(Character_IsLegHitted(playerid)) // || GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_CUFFED ||
	{
		if(newkeys & KEY_JUMP && !(oldkeys & KEY_JUMP))
			ApplyAnimation(playerid, "GYMNASIUM", "gym_jog_falloff", 4.1, 0, 1, 1, 0, 0);

		if(RELEASED(KEY_SPRINT) || HOLDING(KEY_SPRINT) || newkeys & KEY_SPRINT)
			ApplyAnimation(playerid, "PED", "FALL_collapse", 4.1, 0, 1, 1, 0, 0);
	}
	
	return Y_HOOKS_CONTINUE_RETURN_1;
}*/

hook OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart)
{
	//printf("Damage System");
	if(Character_IsAlive(playerid) || Character_IsInjured(playerid)) // Equivalent to !Character_IsDead(playerid)
	{
		new Float:customDamage = Weapon_GetDamage(weaponid);
		if(customDamage == 0)
			customDamage = amount;
		Character_AddDamage(playerid, (issuerid != INVALID_PLAYER_ID ? Character_GetRolePlayName(issuerid) : ("Nessuno")), weaponid, customDamage, bodypart);

		/*if(weaponid && (bodypart == 7 || bodypart == 8))
			Character_SetLegHit(playerid, true);*/
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

flags:danni(CMD_USER);
CMD:danni(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
	{
		new s[8];
		valstr(s, playerid);
		pc_cmd_danni(playerid, s);
		return 1;
		//return SendClientMessage(playerid, COLOR_ERROR, "/danni <playerid/partofname>");
	}
	
	if(!IsPlayerConnected(id) || !Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID invalido.");
	
	if(!IsPlayerInRangeOfPlayer(playerid, id, 5.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
	
	static const partName[7][20] = {"Torso", "Petto", "Braccio sinistro", "Braccio destro", "Gamba sinistra", "Gamba destra", "Testa"};

	new String:str = @("Utente\tArma\tDanni\tParte\n"), count = 0;
	for(new i = 0; i < MAX_DAMAGES_PER_PLAYER; ++i)
	{
		if(!CharacterDamageInfo[id][i][damageUsed])
			continue;
		printf("Weapon ID2: %d", CharacterDamageInfo[id][i][damageWeapon]);
		str += str_format("%s\t%s\t%f\t%s\n", CharacterDamageInfo[id][i][damageIssuerName], Weapon_GetName(CharacterDamageInfo[id][i][damageWeapon]), CharacterDamageInfo[id][i][damageAmount], partName[CharacterDamageInfo[id][i][damageBodyPart]-3]);
		count++;
	}

	if(count <= 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Nessun danno riscontrato sul giocatore.");
	Dialog_Show_s(playerid, DialogNull, DIALOG_STYLE_TABLIST_HEADERS, str_new(Character_GetRolePlayName(id)), str, "Chiudi", "");
	return 1;
}

stock Character_AddDamage(playerid, const issuerName[], weaponid, Float:amount, bodyPart)
{
	new id = Character_FreeDamageIndex(playerid);
	if(id == -1)
		return 0;
	CharacterDamageInfo[playerid][id][damageUsed] = 1;
	set(CharacterDamageInfo[playerid][id][damageIssuerName], issuerName);
	CharacterDamageInfo[playerid][id][damageWeapon] = weaponid;
	CharacterDamageInfo[playerid][id][damageAmount] = amount;
	CharacterDamageInfo[playerid][id][damageBodyPart] = bodyPart;
	return 1;
}

stock Character_ResetDamages(playerid)
{
	new data[e_CharacterDamageInfo];
	for(new i = 0; i < MAX_DAMAGES_PER_PLAYER; ++i)
		CharacterDamageInfo[playerid][i] = data;
	return 1;
}

stock Character_FreeDamageIndex(playerid)
{
	for(new i = 0; i < MAX_DAMAGES_PER_PLAYER; ++i)
	{
		if(!CharacterDamageInfo[playerid][i][damageUsed]) 
			return i;
	}
	return -1;
}