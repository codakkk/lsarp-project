#include <YSI_Coding\y_hooks>

stock Character_HandleFirstSpawn(playerid)
{
	PlayerRestore[playerid][pFirstSpawn] = 0;

	Character_SetSkin(playerid, 46);

	SetPlayerPos(playerid, 1748.1887, -1860.0414, 13.5792);

	if(AccountInfo[playerid][aCharactersCount] <= 2)
	{
		static const characterFirstLoginMoney[] = {30000, 20000, 10000};
		Character_GiveMoney(playerid, characterFirstLoginMoney[AccountInfo[playerid][aCharactersCount]], "FirstSpawn");
		SendFormattedMessage(playerid, -1, "(( Ti sono stati dati {85bb65}$%d{FFFFFF} per cominciare. ))", characterFirstLoginMoney[AccountInfo[playerid][aCharactersCount]]);
	}
	
	new query[128];
	mysql_format(gMySQL, query, sizeof(query), "UPDATE `characters` SET FirstSpawn = '0' WHERE ID = '%d'", PlayerInfo[playerid][pID]);
	mysql_tquery(gMySQL, query);

	AccountInfo[playerid][aCharactersCount]++;
	
	mysql_format(gMySQL, query, sizeof(query), "UPDATE `accounts` SET CharactersCounter = '%d' WHERE ID = '%d'", AccountInfo[playerid][aCharactersCount], AccountInfo[playerid][aID]);
	mysql_tquery(gMySQL, query);

	Character_Save(playerid);
	return 1;
}

stock Character_Spawn(playerid)
{
	PreloadAnimations(playerid);
	#if defined LSARP_DEBUG
		if(!pAdminDuty[playerid] && !strcmp(AccountInfo[playerid][aName], "Coda", false))
		{
			pc_cmd_aduty(playerid, "");
		}
	#endif

	AC_SetPlayerHealth(playerid, 100.0);
	AC_SetPlayerArmour(playerid, 0.0);

	Character_FreezeForTime(playerid, 2000);
	
	CallLocalFunction(#OnCharacterSpawn, "d", playerid);

	if(PlayerRestore[playerid][pFirstSpawn]) // First Login/Spawn
    {
		Character_HandleFirstSpawn(playerid);
		return 1;
    }

    if(PlayerRestore[playerid][pSpawned] && PlayerRestore[playerid][pLastX] != 0 && PlayerRestore[playerid][pLastY] != 0 && PlayerRestore[playerid][pLastZ] != 0)
    {
		PlayerRestore[playerid][pSpawned] = 0;
		SetPlayerPos(playerid, PlayerRestore[playerid][pLastX], PlayerRestore[playerid][pLastY], PlayerRestore[playerid][pLastZ]);
		SetPlayerFacingAngle(playerid, PlayerRestore[playerid][pLastAngle]);
		SetPlayerInterior(playerid, PlayerRestore[playerid][pLastInterior]);
		SetPlayerVirtualWorld(playerid, PlayerRestore[playerid][pLastVirtualWorld]);
		defer LoadCharacterDataAfterTime(playerid);
    }
    else
    {
		if(Character_GetFaction(playerid) == INVALID_FACTION_ID && !Character_HasHouseKey(playerid))
		{
			SetPlayerPos(playerid, 1723.3232, -1867.1775, 13.5705);
			SetPlayerInterior(playerid, 0);
			SetPlayerVirtualWorld(playerid, 0);
		}
		else if( Character_HasHouseKey(playerid))
		{
			new Float:x, Float:y, Float:z, housekey = Character_GetHouseKey(playerid);
			House_GetEnterPosition(housekey, x, y, z);
			SetPlayerPos(playerid, x, y, z);
			SetPlayerInterior(playerid, House_GetEnterInterior(housekey));
			SetPlayerVirtualWorld(playerid, House_GetEnterWorld(housekey));
		}
		else if( Character_GetFaction(playerid) != INVALID_FACTION_ID)
		{
			new Float:x, Float:y, Float:z, factionid = Character_GetFaction(playerid);
			Faction_GetSpawnPos(factionid, x, y, z);
			SetPlayerPos(playerid, x, y, z);
			SetPlayerInterior(playerid, Faction_GetSpawnInterior(factionid));
			SetPlayerVirtualWorld(playerid, Faction_GetSpawnWorld(factionid));
		}
		AC_SetPlayerHealth(playerid, 100.0);
		AC_SetPlayerArmour(playerid, 0.0);
    }

	if(Character_IsJailed(playerid))
	{
		Character_SetToJailPos(playerid);
		PlayerTextDrawShow(playerid, pJailTimeText[playerid]);
		return 1;
	}
	return 1;
}