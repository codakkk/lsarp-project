
flags:fruga(CMD_ALIVE_USER);
CMD:fruga(playerid, params[])
{
	if(IsPlayerInAnyVehicle(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su un veicolo.");

	if(Character_GetLastPickup(playerid) == -1)
		return SendClientMessage(playerid, COLOR_ERROR, "Non c'è nulla da frugare qui.");

	new id, E_ELEMENT_TYPE:type;
	Pickup_GetInfo(playerid, id, type);
	if(type != ELEMENT_TYPE_LOOTZONE) 
		return SendClientMessage(playerid, COLOR_ERROR, "Non c'è nulla da frugare qui.");
	LootZone_ShowItemsToPlayer(playerid, id);
	return 1;
}

flags:createlootzone(CMD_DEVELOPER);
CMD:createlootzone(playerid, params[])
{
	new Float:x,
		Float:y,
		Float:z,
		string[128],
		zone_id;
	GetPlayerPos(playerid, x, y, z);
	zone_id = LootZone_Create(x, y, z, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));
	if(zone_id == -1)
		return SendClientMessage(playerid, COLOR_ERROR, "Non è stato possibile creare una Loot Zone. Contatta lo sviluppatore.");
	format(string, sizeof(string), "Hai creato una loot zone [ID: %d].", zone_id);
	SendClientMessage(playerid, COLOR_ADMIN, string);

	//Log(playerid, "LootZone_Create", zone_id);
	return 1;
}

flags:lootzonemenu(CMD_DEVELOPER);
CMD:lootzonemenu(playerid, params[])
{
	if(Iter_Count(LootZoneIterator) <= 0)
		return SendClientMessage(playerid, -1, "Non esistono Loot Zone.");
	
	LootZone_AdminShowMenu(playerid, 0);
	return 1;
}

flags:refreshlootzone(CMD_DEVELOPER);
CMD:refreshlootzone(playerid, params[])
{
	new
		zone_id;
	if(sscanf(params, "d", zone_id)) 
	{
		SendClientMessage(playerid, COLOR_GREY, "[UTILIZZO] /refreshlootzone <zone_id>");
		return 1;
	}

	if(!LootZone_IsValid(zone_id))
		return SendClientMessage(playerid, COLOR_ERROR, "[LOOT_ZONE] Loot Zone non valida.");

	LootZone_Refresh(zone_id);
	SendMessageToAdmins(false, COLOR_ADMIN, "[ADM-CMD] %s ha refreshato la loot zone ID %d.", AccountInfo[playerid][aName], zone_id);
	SendFormattedMessage(playerid, COLOR_ADMIN, "[LOOT-ZONE] Hai refreshato la Loot Zone ID %d.", zone_id);
	//Log(playerid, "LootZone_Refresh", zone_id);
	return 1;
}

flags:refreshlootzoneall(CMD_DEVELOPER);
CMD:refreshlootzoneall(playerid, params[])
{
	LootZone_RefreshAll();
	SendMessageToAdmins(false, COLOR_ADMIN, "[ADM-CMD] %s ha refreshato tutte le loot zone.", AccountInfo[playerid][aName]);
	SendFormattedMessage(playerid, COLOR_ADMIN, "[LOOT-ZONE] Hai refreshato tutte le lootzone.");
	// Log(playerid, "LootZone_RefreshAll", -1);
	return 1;
}

flags:removealllootzone(CMD_DEVELOPER);
CMD:removealllootzone(playerid, params[])
{
	if(!IsPlayerAdmin(playerid))
		return 1;
	foreach(new zone_id : LootZoneIterator)
	{
		ITER_SAFE(LootZoneIterator, zone_id)
		{
			LootZone_Delete(zone_id);
		}
	}
	// Log(playerid, "Remove All Loot zone", -1);
	SendMessageToAdmins(false, COLOR_ADMIN, "[ADM-CMD] %s ha refreshato tutte le loot zone.", AccountInfo[playerid][aName]);
	SendFormattedMessage(playerid, COLOR_ADMIN, "[LOOT-ZONE] Hai refreshato tutte le lootzone.");
	return 1;
}