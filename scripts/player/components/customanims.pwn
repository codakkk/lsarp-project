#include <YSI_Coding\y_hooks>

new 
	pCustomAnimLibrary[MAX_PLAYERS][32],
	pCustomAnimName[MAX_PLAYERS][32],
	pCustomAnimCustomName[MAX_PLAYERS][32],
	pCustomAnimLoop[MAX_PLAYERS char]
	;

CMD:anim(playerid, params[])
{
	if(!AnimationCheck(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione ora.");
	
	new animName[32];
	if(sscanf(params, "s[32]", animName))
		return SendClientMessage(playerid, COLOR_ERROR, "/anim <custom anim name>");
	
	inline OnFind()
	{
		new rows = cache_num_rows();
		if(rows <= 0)
			return SendClientMessage(playerid, COLOR_ERROR, "Non esiste una custom anim con questo nome.");
		
		new tmpLib[32], tmpAnimName[32], isLoop;
		cache_get_value_index(0, 3, tmpLib);
		cache_get_value_index(0, 4, tmpAnimName);
		cache_get_value_index_int(0, 5, isLoop);
		if(AnimationCheck(playerid))
			ApplyAnimation(playerid, tmpLib, tmpAnimName, 4.1, isLoop, 1, 1, 1, 0);
	}
	MySQL_TQueryInline(gMySQL, using inline OnFind, "SELECT * FROM `character_custom_animations` WHERE CharacterID = '%d' AND Name = '%e';", Character_GetID(playerid), animName);
	return 1;
}
alias:anim("ca");

CMD:customanim(playerid, params[])
{
	if(isnull(params) || strlen(params) > 10)
		return SendClientMessage(playerid, COLOR_ERROR, "/customanim <crea - lista>");
	if(!strcmp(params, "crea", true) || !strcmp(params, "create", true) || !strcmp(params, "c", true))
	{
		Dialog_Show(playerid, Dialog_CreateCustomAnim, DIALOG_STYLE_INPUT, "Nuova Animazione Personalizzata", "Inserisci la libreria della nuova animazione.", "Continua", "Annulla");
	}
	else if(!strcmp(params, "lista", true) || !strcmp(params, "list", true) || !strcmp(params, "l", true))
	{
		inline OnFind()
		{
			new String:content = @("Nome\tLibreria\tNome Anim\tLoop\n");
			new rows = cache_num_rows();
			if(rows <= 0)
				return SendClientMessage(playerid, COLOR_ERROR, "Non hai creato nessuna custom anim.");
			new tmp[32], tmpInt;
			for(new i = 0; i < rows; ++i)
			{
				cache_get_value_index(i, 0, tmp);
				content += str_new(tmp) % @("\t");

				cache_get_value_index(i, 1, tmp);
				content += str_new(tmp) % @("\t");

				cache_get_value_index(i, 1, tmp);
				content += str_new(tmp) % @("\t");

				cache_get_value_index_int(i, 0, tmpInt);
				if(tmpInt)
					content += @("Si");
				else
					content += @("No");
				content += @("\n");
			}
			Dialog_Show_s(playerid, Dialog_CustomAnimDelete, DIALOG_STYLE_TABLIST_HEADERS, @("Custom Animations"), content, "Elimina", "Chiudi");
		}
		MySQL_TQueryInline(gMySQL, using inline OnFind, "SELECT Name, Library, AnimName, `Loop` FROM `character_custom_animations` WHERE CharacterID = '%d';", Character_GetID(playerid));
	}
	else return SendClientMessage(playerid, COLOR_ERROR, "/customanim <crea - lista>");
	return 1;
}

Dialog:Dialog_CustomAnimDelete(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;
	
	inline OnCheck()
	{
		new rows = cache_num_rows();
		if(rows <= 0)
			return SendClientMessage(playerid, COLOR_ERROR, "Animazione non trovata.");
		
		new tmpId, tmpName[32];
		
		cache_get_value_index_int(listitem, 0, tmpId);
		cache_get_value_index(listitem, 1, tmpName);

		mysql_tquery_f(gMySQL, "DELETE FROM `character_custom_animations` WHERE ID = '%d';", tmpId);

		SendFormattedMessage(playerid, COLOR_GREEN, "Custom Animation \"%s\" eliminata con successo.", tmpName);
	}
	MySQL_TQueryInline(gMySQL, using inline OnCheck, "SELECT ID, Name FROM `character_custom_animations` WHERE CharacterID = '%d';", Character_GetID(playerid));
	return 1;
}


Dialog:Dialog_CreateCustomAnim(playerid, response, listitem, inputtext[])
{
	if(!response) 
		return 0;
	if(!CustomAnimation_IsValidLibrary(inputtext) || isnull(inputtext) || strlen(inputtext) > 32)
		return Dialog_Show(playerid, Dialog_CreateCustomAnim, DIALOG_STYLE_INPUT, "Nuova Animazione Personalizzata", "{FF0000}La libreria inserita per la nuova animazione non è valida.{FFFFFF}\nInserisci la libreria della nuova animazione.", "Continua", "Annulla");
	set(pCustomAnimLibrary[playerid], inputtext);
	Dialog_Show(playerid, Dialog_CreateCustomAnimName, DIALOG_STYLE_INPUT, "Nome Animazione", "Libreria selezionata: %s\nInserisci il nome dell'animazione appartenente alla libreria.", "Continua", "Annulla", inputtext);
	return 1;
}

Dialog:Dialog_CreateCustomAnimName(playerid, response, listitem, inputtext[])
{
	if(!response) 
		return 0;
	set(pCustomAnimName[playerid], inputtext);
	Dialog_Show(playerid, Dialog_CreateCustomAnimLoop, DIALOG_STYLE_LIST, "Animazione Personalizzata: Loop", "Loop: No\nLoop: Si", "Crea", "Annulla");
	return 1;
}

Dialog:Dialog_CreateCustomAnimLoop(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;
	pCustomAnimLoop{playerid} = listitem;
	Dialog_Show(playerid, Dialog_CreateCAnimCName, DIALOG_STYLE_INPUT, "Nome", "Inserisci il nome della custom anim.", "Continua", "Annulla");
	return 1;
}

Dialog:Dialog_CreateCAnimCName(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;
	if(isnull(inputtext) || strlen(inputtext) > 30)
		return Dialog_Show(playerid, Dialog_CreateCAnimCName, DIALOG_STYLE_INPUT, "Nome", "{FF0000}Il nome inserito è troppo lungo o troppo corto.{FFFFFF}\nInserisci il nome della custom anim.", "Continua", "Annulla");
	set(pCustomAnimCustomName[playerid], inputtext);
	inline OnCheck()
	{
		new rows = cache_num_rows();
		if(rows > 0)
		{
			Dialog_Show(playerid, Dialog_CreateCAnimCName, DIALOG_STYLE_INPUT, "Nome", "{FF0000}Hai già una custom anim con questo nome.{FFFFFF}\nInserisci il nome della custom anim.", "Continua", "Annulla");
		}
		else
		{
			Dialog_Show(playerid, Dialog_CreateCustomAnimOk, DIALOG_STYLE_MSGBOX, "Conferma Animazione Personalizzata", "Nome: %s\nLibreria: %s\nNome Animazione: %s\nLoop: %s", "Crea", "Annulla", pCustomAnimCustomName[playerid], pCustomAnimLibrary[playerid], pCustomAnimName[playerid], pCustomAnimLoop{playerid} ? ("Si") : ("No"));
		}
	}
	MySQL_TQueryInline(gMySQL, using inline OnCheck, "SELECT ID FROM `character_custom_animations` WHERE CharacterID = '%d' AND LOWER(Name) = LOWER('%e');", Character_GetID(playerid), inputtext);
	return 1;
}

Dialog:Dialog_CreateCustomAnimOk(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;
	CustomAnimation_CreateForPlayer(playerid, pCustomAnimCustomName[playerid], pCustomAnimLibrary[playerid], pCustomAnimName[playerid], pCustomAnimLoop{playerid});
	SendClientMessage(playerid, COLOR_GREEN, "Custom Animation creata con successo.");
	return 1;
}

stock CustomAnimation_CreateForPlayer(playerid, const customName[], const library[], const animName[], isLoop)
{
	mysql_tquery_f(gMySQL, "INSERT INTO `character_custom_animations` (CharacterID, Name, Library, AnimName, `Loop`) VALUES('%d', '%e', '%e', '%e', '%d');", 
	Character_GetID(playerid), customName, library, animName, isLoop);
}

stock CustomAnimation_IsValidLibrary(const library[])
{
	static const libraries[] = 
	{
		"AIRPORT", "BAR", "BASEBALL", "BD_FIRE", "BEACH", 
		"benchpress", "BF_injection", "BIKED", "BIKEH", 
		"BIKELEAP", "BIKES", "BIKEV", "BIKE_DBZ", "BLOWJOBZ", 
		"BMX", "BOMBER", "BOX", "BSKTBALL", "BUDDY", "BUS", 
		"CAMERA", "CAR", "CARRY", "CAR_CHAT", "CASINO", 
		"CHAINSAW", "CHOPPA", "CLOTHES", "COACH", "COLT45", 
		"COP_AMBIENT", "COP_DVBYZ", "CRACK", "CRIB", "DAM_JUMP", 
		"DANCING", "DEALER", "DILDO", "DODGE", "DOZER", "DRIVEBYS", 
		"FAT", "FIGHT_B", "FIGHT_C", "FIGHT_D", "FIGHT_E", "FINALE", 
		"FINALE2", "FLAME", "Flowers", "FOOD", "Freeweights", "GANGS", 
		"GHANDS", "GHETTO_DB", "goggles", "GRAFFITI", "GRAVEYARD", "GRENADE", 
		"GYMNASIUM", "HAIRCUTS", "HEIST9", "INT_HOUSE", "INT_OFFICE", "INT_SHOP", 
		"JST_BUISNESS", "KART", "KISSING", "KNIFE", "LAPDAN1", "LAPDAN2", "LAPDAN3", 
		"LOWRIDER", "MD_CHASE", "MD_END", "MEDIC", "MISC", "MTB", "MUSCULAR", "NEVADA", 
		"ON_LOOKERS", "OTB", "PARACHUTE", "PARK", "PAULNMAC", "ped", "PLAYER_DVBYS", "PLAYIDLES", 
		"POLICE", "POOL", "POOR", "PYTHON", "QUAD", "QUAD_DBZ", "RAPPING", "RIFLE", "RIOT", "ROB_BANK", 
		"ROCKET", "RUSTLER", "RYDER", "SCRATCHING", "SHAMAL", "SHOP", "SHOTGUN", "SILENCED", "SKATE", 
		"SMOKING", "SNIPER", "SPRAYCAN", "STRIP", "SUNBATHE", "SWAT", "SWEET", "SWIM", "SWORD", "TANK", 
		"TATTOOS", "TEC", "TRAIN", "TRUCK", "UZI", "VAN", "VENDING", "VORTEX", "WAYFARER", "WEAPONS"
	};
	for(new i = 0, j = sizeof(libraries); i < j; ++i)
	{
		if(!strcmp(libraries[i], library, true))
			return 1;
	}
	return 0;
}