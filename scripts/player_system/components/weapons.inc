#include <YSI_Coding\y_hooks>

static
	Weapon[MAX_PLAYERS][13 char] = {0, ...},
	Ammo[MAX_PLAYERS][13] = {0, ...},
	bool:HasLoadedWeapons[MAX_PLAYERS char] = {false, ...};

hook OnCharacterClearData(playerid)
{
	ResetPlayerWeapons(playerid);
	for(new i = 0; i < 13; ++i)
		Weapon[playerid]{i} = Ammo[playerid][i] = 0;
	HasLoadedWeapons{playerid} = false;
	return 1;
}

hook OnCharacterSpawn(playerid)
{
	ResetPlayerWeapons(playerid);
	for(new i = 0; i < 13; ++i)
		Weapon[playerid]{i} = Ammo[playerid][i] = 0;
	return 1;
}

hook OnCharacterDisconnected(playerid, reason)
{
	Character_SaveWeapons(playerid);
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnCharacterLateLoad(playerid)
{
	Character_LoadWeapons(playerid);
	return 1;
}

hook OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
	new slotid = Weapon_GetSlot(weaponid);
	if(Weapon[playerid]{slotid} != 0 && Ammo[playerid][slotid] != 0)
	{
		Ammo[playerid][slotid]--;
		if(Ammo[playerid][slotid] <= 0)
			Character_RemoveWeapon(playerid, weaponid);
		//printf("Weapon ID: %d - Ammo: %d - AC Ammo: %d", weaponid, GetPlayerAmmo(playerid), Ammo[playerid][slotid]);
	}
	return 1;
}

//hook OnPlayerUpdate(playerid)
ptask UpdatePlayerWeaponAntiCheat[50](playerid) 
{
	if(!Character_IsLogged(playerid))
		return 1;
	
	static clientAmmo, clientWeapon, weaponSlot;
	static serverWeapon, serverAmmo;
	clientWeapon = GetPlayerWeapon(playerid);
	if(GetPlayerState(playerid) != PLAYER_STATE_SPECTATING && clientWeapon != -1 && clientWeapon != 0)
	{
		weaponSlot = Weapon_GetSlot(clientWeapon);
		clientAmmo = GetPlayerAmmo(playerid);
		serverWeapon = Weapon[playerid]{weaponSlot};
		serverAmmo = Ammo[playerid][weaponSlot];
		
		if(weaponSlot == 7 && Account_GetAdminLevel(playerid) < 2)
		{
			Character_RemoveWeapon(playerid, clientWeapon);
			ClearAnimations(playerid, 1);
			KickEx(playerid);
			SendMessageToAdmins(1, COLOR_ORANGE, "[ATTENZIONE] %s si è probabilmente givato un'arma tra Minigun, Flamethrower e lanciarazzi. (Kick automatico)", Character_GetOOCName(playerid));
			return 0;
		}

		if(weaponSlot == 8 && clientAmmo < serverAmmo)
		{
			Ammo[playerid][weaponSlot] = clientAmmo;
		}

		if(clientAmmo > serverAmmo + 6) // Six is just a threshold allowable of bullets desync
		{
			SetPlayerAmmo(playerid, clientWeapon, Ammo[playerid][weaponSlot]);
			SetPlayerArmedWeapon(playerid, 0); // just to prevent weapon infinite ammo without being detected.

			//printf("AC Timer: Weapon ID: %d - Ammo: %d - AC Ammo: %d", clientWeapon, clientAmmo, Ammo[playerid][weaponSlot]);
		}
		if(serverWeapon != clientWeapon)
		{
			Character_RemoveWeapon(playerid, clientWeapon);
			GivePlayerWeapon(playerid, Weapon[playerid]{weaponSlot}, Ammo[playerid][weaponSlot]);
			SetPlayerArmedWeapon(playerid, 0); // just to prevent weapon infinite ammo without being detected.
			
			//printf("AC Timer: Weapon ID: %d - AC Weapon ID: %d", clientWeapon, Weapon[playerid]{weaponSlot});
		}
	}
	return 1;
}

ptask SaveWeaponsEachTot[5000](playerid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	Character_SaveWeapons(playerid);
	return 1;
}

stock Character_SaveWeapons(playerid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	new weaponid, ammo;
	for(new i = 0; i < 13; ++i)
	{
		weaponid = Weapon[playerid]{i};
		ammo = Ammo[playerid][i];
		
		if(weaponid <= 0)
			continue;
		
		if(i == 2 && Character_HasTaser(playerid))
		{
			Character_GetCachedWeaponTaser(playerid, weaponid, ammo);
		}
		else if(i == 3 && Character_HasBeanBag(playerid))
		{
			Character_GetCachedWeaponBean(playerid, weaponid, ammo);
		}
		
		if(i > 1 && i != 10 && (ammo <= 0 || weaponid == 0))
		{
			mysql_pquery_f(gMySQL, "DELETE FROM `character_weapons` WHERE character_id = '%d' AND slot_id = '%d';", Character_GetID(playerid), i);
		}
		else
		{
			mysql_pquery_f(gMySQL, "INSERT INTO `character_weapons` (character_id, slot_id, weapon_id, ammo) VALUES('%d', '%d', '%d', '%d') ON DUPLICATE KEY UPDATE weapon_id = VALUES(weapon_id), ammo = VALUES(ammo);", Character_GetID(playerid), i, weaponid, ammo);
		}
	}
	return 1;
}

stock Character_LoadWeapons(playerid)
{
	if(HasLoadedWeapons{playerid})
		return;
	HasLoadedWeapons{playerid} = true;
	inline OnLoad()
	{
		new rows = cache_num_rows(), cid;
		if(rows <= 0)
			return;
		
		new w, a, slotid;

		for(new i = 0; i < rows; ++i)
		{
			cache_get_value_name_int(i, "character_id", cid);
			
			if(cid != Character_GetID(playerid))
				continue;
			
			cache_get_value_name_int(i, "weapon_id", w);
			cache_get_value_name_int(i, "ammo", a);

			if(! (0 <= w <= 46))
			{
				printf("Character_LoadWeapons(%d) failed. invalid weapon id (id %d)", playerid, w);
				continue;
			}

			slotid = Weapon_GetSlot(w);
			
			Weapon[playerid]{slotid} = w;
			Ammo[playerid][slotid] = a;

			GivePlayerWeapon(playerid, w, a);
		}
		printf("%d weapons loaded for player %d (DBID: %d).", rows, playerid, Character_GetID(playerid));
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT character_id, weapon_id, ammo FROM `character_weapons` WHERE character_id = '%d'", Character_GetID(playerid));
}

stock Character_HasWeaponInSlot(playerid, slotid)
{
	return Weapon[playerid]{slotid} != 0 && Ammo[playerid][slotid] > 0;
}

stock Character_GetWeaponInSlot(playerid, slotid)
{
	new weapon = Weapon[playerid]{slotid}, ammo = Ammo[playerid][slotid];
	if(weapon > 0 && ammo > 0)
		return weapon;
	return 0;
}

stock Character_RemoveWeapon(playerid, weaponid)
{

	for(new slot = 0; slot != 12; slot++)
	{
		if(Weapon[playerid]{slot} == weaponid)
		{
			Weapon[playerid]{slot} = Ammo[playerid][slot] = 0;
		}
	}

	ResetPlayerWeapons(playerid);

	for(new slot = 0; slot != 12; slot++)
	{
		GivePlayerWeapon(playerid, Weapon[playerid]{slot}, Ammo[playerid][slot]);
	}
	
	mysql_pquery_f(gMySQL, "DELETE FROM `character_weapons` WHERE weapon_id = '%d' AND character_id = '%d';", weaponid, Character_GetID(playerid));
	return 1;
}



stock Character_ResetWeapons(playerid)
{
	if(!IsPlayerConnected(playerid) || !Character_IsLogged(playerid) || !ResetPlayerWeapons(playerid))
		return 0;
	
	mysql_pquery_f(gMySQL, "DELETE FROM `character_weapons` WHERE character_id = '%d'", Character_GetID(playerid));

	for(new i = 0; i < 13; ++i)
	{
		Weapon[playerid]{i} = Ammo[playerid][i] = 0;
	}
	return 0;
}

stock Character_GiveWeapon(playerid, weaponid, ammo)
{
	if(!Character_IsLogged(playerid))
		return 0;
	new slotid = Weapon_GetSlot(weaponid);
	new currentWeapon = Weapon[playerid]{slotid};
	if(0 <= weaponid <= 18 || 22 <= weaponid <= 46)
	{
		Ammo[playerid][slotid] = (3 <= slotid <= 5 || currentWeapon == weaponid) ? (Ammo[playerid][slotid] + ammo) : ammo;
		if(Ammo[playerid][slotid] < -32768) Ammo[playerid][slotid] = -32768;
		else if(Ammo[playerid][slotid] > 32768) Ammo[playerid][slotid] = 32768;
		Weapon[playerid]{slotid} = weaponid;
	}
	else
	{
		Weapon[playerid]{slotid} = weaponid;
		Ammo[playerid][slotid] = 1;
	}
	Character_SaveWeapons(playerid);
	return GivePlayerWeapon(playerid, weaponid, ammo);
}

stock Character_GetWeapon(playerid)
{
	new slotid = Weapon_GetSlot(GetPlayerWeapon(playerid));
	return Weapon[playerid]{slotid};
	//return AntiCheatGetWeapon(playerid);
}

stock Character_GetCurrentWeaponData(playerid, &weaponid, &ammo)
{
	return Character_GetWeaponData(playerid, Weapon_GetSlot(GetPlayerWeapon(playerid)), weaponid, ammo);
}

stock Character_HasWeapon(playerid, weaponid)
{
	new slotid = Weapon_GetSlot(weaponid), acW, acA;
	return Character_GetWeaponData(playerid, slotid, acW, acA) && weaponid == acW;
}

stock Character_GetAmmo(playerid)
{
	new slot = Weapon_GetSlot(GetPlayerWeapon(playerid));
	return Ammo[playerid][slot];//GetPlayerAmmo(playerid);
}

stock Character_SetWeaponAmmo(playerid, weaponid, ammo)
{
	if(!Character_IsLogged(playerid))
		return 0;
	
	if(ammo < -32768) ammo = -32768;
	else if(ammo > 32767) ammo = 32767;

	if(16 <= weaponid <= 43)
	{
		new slotid = Weapon_GetSlot(weaponid);
		if(Weapon[playerid]{slotid} > 0)
		{
			Ammo[playerid][slotid] = ammo;
			mysql_pquery_f(gMySQL, "UPDATE `character_weapons` SET ammo = '%d' WHERE weapon_id = '%d' AND character_id = '%d'", ammo, Weapon[playerid]{slotid}, Character_GetID(playerid));
		}
	}
	return SetPlayerAmmo(playerid, weaponid, ammo);
}

stock Character_GetWeaponData(playerid, slotid, &weaponid, &ammo)
{
	if(!Character_IsLogged(playerid))
		return 0;
	weaponid = Weapon[playerid]{slotid};
	ammo = Ammo[playerid][slotid];
	return 1;
}

hook OnCharacterTryAccept(playerid, type)
{
	if(type != REQUEST_TYPE_WEAPON)
		return 0;

	if(!IsPlayerInRangeOfPlayer(playerid, Request_GetSender(playerid, REQUEST_TYPE_WEAPON), 5.0))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei vicino al giocatore.");
	
	new slotid = Request_GetSlot(playerid, REQUEST_TYPE_WEAPON),
		weaponid = Request_GetItem(playerid, REQUEST_TYPE_WEAPON),
		tempWeapon = 0,
		ammo = 0,
		requestSender = Request_GetSender(playerid, REQUEST_TYPE_WEAPON);
	GetPlayerWeaponData(requestSender, slotid, tempWeapon, ammo);
	if(weaponid != tempWeapon || ammo == 0)
	{
		Request_Reset(playerid, REQUEST_TYPE_WEAPON);
		Request_Reset(requestSender, REQUEST_TYPE_WEAPON);
		SendClientMessage(playerid, COLOR_ERROR, "Non possiedi più l'arma. La richiesta è stata annullata.");
		return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non possiede più l'arma, pertanto non puoi più accettare.");	
	}
	new w, a;
	Character_GetWeaponData(playerid, Weapon_GetSlot(weaponid), w, a);
	if(w == 0 || a == 0)
	{
		Character_RemoveWeapon(requestSender, weaponid);
		Character_GiveWeapon(playerid, weaponid, ammo);
		
		SendFormattedMessage(playerid, COLOR_ACCEPT_REQUEST, "Hai accettato l'arma (%s) con %d proiettili da %s.", Weapon_GetName(weaponid), ammo, Character_GetRolePlayName(requestSender));
		SendFormattedMessage(requestSender, COLOR_ACCEPT_REQUEST, "%s (%d) ha accettato la tua arma.", Character_GetRolePlayName(playerid), playerid);
		
		Request_Reset(playerid, REQUEST_TYPE_WEAPON);
		Request_Reset(requestSender, REQUEST_TYPE_WEAPON);
	}
	else if(Character_HasSpaceForItem(playerid, weaponid, ammo))
	{
		Character_RemoveWeapon(requestSender, weaponid);
		Character_GiveItem(playerid, weaponid, 1, ammo);

		SendFormattedMessage(playerid, COLOR_ACCEPT_REQUEST, "Hai accettato l'arma (%s) con %d proiettili da %s.", Weapon_GetName(weaponid), ammo, Character_GetRolePlayName(requestSender));
		SendClientMessage(playerid, COLOR_ACCEPT_REQUEST, "L'arma è stata messa nell'inventario poiché lo slot è occupato.");

		SendFormattedMessage(requestSender, COLOR_ACCEPT_REQUEST, "%s (%d) ha accettato la tua arma.", Character_GetRolePlayName(playerid), playerid);

		Request_Reset(playerid, REQUEST_TYPE_WEAPON);
		Request_Reset(requestSender, REQUEST_TYPE_WEAPON);
	}
	else
		SendClientMessage(playerid, COLOR_ERROR, "Hai già un'arma e non hai abbastanza spazio nell'inventario.");
	return 1;
}