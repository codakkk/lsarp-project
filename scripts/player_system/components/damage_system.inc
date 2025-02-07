#include <YSI_Coding\y_hooks>

static enum e_CharacterDamageInfo
{
	Used,
	IssuerName[MAX_PLAYER_NAME],
	Weapon,
	Float:Damage,
	BodyPart,
};
static 
	CharacterDamageInfo[MAX_PLAYERS][MAX_DAMAGES_PER_PLAYER][e_CharacterDamageInfo];

hook OnCharacterClearData(playerid)
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

stock DamageSystem_Update(playerid, Float:amount, issuerid, weaponid, bodypart)
{
	if(Character_IsAlive(playerid) || Character_IsInjured(playerid)) // Equivalent to !Character_IsDead(playerid)
	{
		Character_AddDamage(playerid, (issuerid != INVALID_PLAYER_ID ? Character_GetRolePlayName(issuerid) : ("Nessuno")), weaponid, amount, bodypart);
	}
	return 1;
}

flags:danni(CMD_USER);
CMD:danni(playerid, params[])
{
	new id;
	if(sscanf(params, "k<u>", id))
	{
		new s[8];
		valstr(s, playerid);
		callcmd::danni(playerid, s);
		return 1;
		//return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /danni [playerid/partofname]");
	}
	
	if(!Character_IsLogged(id))
		return SendClientMessage(playerid, COLOR_ERROR, "ID invalido.");
	
	if(!IsPlayerInRangeOfPlayer(playerid, id, 5.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
	
	static const partName[7][20] = {"Torso", "Petto", "Braccio sinistro", "Braccio destro", "Gamba sinistra", "Gamba destra", "Testa"};

	new String:str = @("Utente\tArma\tDanni\tParte\n"), count = 0;
	for(new i = 0; i < MAX_DAMAGES_PER_PLAYER; ++i)
	{
		if(!CharacterDamageInfo[id][i][Used] || CharacterDamageInfo[id][i][Weapon] < 0)
			continue;
		new bodypart = CharacterDamageInfo[id][i][BodyPart]-3;
		if(bodypart < 0 || bodypart >= sizeof(partName))
			bodypart = sizeof(partName)-1;
		str_append_format(str, "%s\t%s\t%f\t%s\n", CharacterDamageInfo[id][i][IssuerName], Weapon_GetName(CharacterDamageInfo[id][i][Weapon]), CharacterDamageInfo[id][i][Damage], partName[bodypart]);
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
	CharacterDamageInfo[playerid][id][Used] = 1;
	set(CharacterDamageInfo[playerid][id][IssuerName], issuerName);
	CharacterDamageInfo[playerid][id][Weapon] = weaponid;
	CharacterDamageInfo[playerid][id][Damage] = amount;
	CharacterDamageInfo[playerid][id][BodyPart] = bodyPart;
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
		if(!CharacterDamageInfo[playerid][i][Used]) 
			return i;
	}
	return -1;
}