

#include "commands\cmenu"
#include "commands\hedit"
#include "commands\hcreate"

flags:gotohouse(CMD_JR_MODERATOR);
CMD:gotohouse(playerid, params[])
{
	new houseid;
    if(sscanf(params, "d", houseid))
	   return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /gotohouse <houseid>");
	new House:house = House_Find(houseid);
	if(!House_IsValid(house))
		return SendClientMessage(playerid, COLOR_ERROR, "La casa inserita non esiste.");
	new Float:x, Float:y, Float:z;
	House_GetEnterPosition(house, x, y, z);
	SetPlayerPos(playerid, x, y, z);
	SetPlayerInterior(playerid, House_GetEnterInterior(house));
	Character_SetVirtualWorld(playerid, House_GetEnterWorld(house));
	SendFormattedMessage(playerid, COLOR_GREEN, "Ti sei gotato alla casa id %d", House_GetID(house));
	return 1;
}

flags:hinttypes(CMD_JR_MODERATOR);
CMD:hinttypes(playerid, params[])
{
	new type;
	if(sscanf(params, "d", type) || !HouseInterior_IsValidType(type))
	{
		SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /hinttypes <tipo interno>");
		SendClientMessage(playerid, COLOR_GREY, "1: Casa molto piccola - 2: Casa piccola - 3: Casa media - 4: Casa grande");
		SendClientMessage(playerid, COLOR_GREY, "5: Casa molto grande - 6: Roulotte molto piccola - 7: Roulotte piccola");
		SendClientMessage(playerid, COLOR_GREY, "8: Roulotte media - 9: Roulotte grande");
		return 1;
	}
	new 
		count = 0,
		List:interiors = HouseInterior_GetListByType(type);
	
	SendFormattedMessage(playerid, COLOR_GREEN, "INTERIOR ID DISPONIBILI PER: %s", GetInteriorTypeName(type));
	for_list(i : interiors)
	{
		SendFormattedMessage(playerid, COLOR_GREEN, "Interior %d", count);
		++count;
	}
	return 1;
}

flags:ahousecmds(CMD_ADMIN);
CMD:ahousecmds(playerid, params[])
{
    SendClientMessage(playerid, COLOR_WHITE, "[HOUSES]: /hcreate - /hedit");
    return 1;
}

CMD:casacmds(playerid, params[])
{
	SendClientMessage(playerid, -1, "[Casa] /casa o /cmenu");
	return 1;
}

flags:bussa(CMD_ALIVE_USER);
CMD:bussa(playerid, params[])
{
	new House:house;
	new Building:building;

	if(Character_TryGetBuilding(playerid, building) && Character_IsNearBuildingEnter(playerid, building))
	{
		Character_AMe(playerid, "bussa alla porta.");
		foreach(new p : Player)
		{
			if(Character_IsLogged(p) && Character_GetVirtualWorld(p) == Building_GetExitWorld(building))
				SendClientMessage(p, COLOR_PURPLE, "** Qualcuno sta bussando alla porta di casa. **");
		}
	}
	else if(Character_TryGetHouse(playerid, house) && Character_IsNearHouseEnter(playerid, house))
	{
		Character_AMe(playerid, "bussa alla porta.");
		foreach(new p : Player)
		{
			if(Character_IsLogged(p) && Character_GetVirtualWorld(p) == House_GetExitWorld(house))
				SendClientMessage(p, COLOR_PURPLE, "** Qualcuno sta bussando alla porta di casa. **");
		}
	}
	return 1;
}

flags:dshout(CMD_ALIVE_USER);
CMD:dshout(playerid, params[])
{
	if(isnull(params) || strlen(params) <= 0)
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: (/ds)hout [testo]");
	
	new House:house, Building:building, 
		Float:x, Float:y, Float:z, 
		bool:isInside, world, interior;

	if(!GetHouseOrBuildingData(playerid, building, house, isInside, x, y, z, world, interior))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata o all'uscita di un edificio.");	

	
	callcmd::shout(playerid, params);

	if(isInside)
	{
		SendRangedMessageStr(20.0, x, y, z, world, interior, str_format("[Porta] %s grida: %s", Character_GetRolePlayName(playerid), params), COLOR_FADE1, COLOR_FADE2, COLOR_FADE3, COLOR_FADE4, COLOR_FADE5, false);
	}
	else
	{
		new String:str = str_format("[Porta] %s grida: %s!", Character_GetRolePlayName(playerid), params);
		foreach(new p : Player)
		{
			if(Character_GetVirtualWorld(p) == world)
				SendClientMessageStr(p, -1, str);
		}
	}
	return 1;
}
alias:dshout("ds");

flags:ddo(CMD_ALIVE_USER);
CMD:ddo(playerid, params[])
{
	if(isnull(params) || strlen(params) <= 0)
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /ddo [testo]");
	
	new House:house, Building:building, 
		Float:x, Float:y, Float:z, 
		bool:isInside, world, interior;

	if(!GetHouseOrBuildingData(playerid, building, house, isInside, x, y, z, world, interior))
		return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'entrata o all'uscita di un edificio.");	

	
	callcmd::do(playerid, params);

	if(isInside)
	{
		SendRangedMessageStr(20.0, x, y, z, world, interior, str_format("* [Porta] %s (( %s ))", params, Character_GetRolePlayName(playerid)), COLOR_INTERACTION, COLOR_INTERACTION, COLOR_INTERACTION, COLOR_INTERACTION, COLOR_INTERACTION, false);
	}
	else
	{
		new String:str = str_format("* [Porta] %s (( %s ))", params, Character_GetRolePlayName(playerid));
		foreach(new p : Player)
		{
			if(Character_GetVirtualWorld(p) == world)
				SendClientMessageStr(p, -1, str);
		}
	}
	return 1;
}

static bool:GetHouseOrBuildingData(playerid, &Building:building, &House:house, &bool:isInside, &Float:x, &Float:y, &Float:z, &world, &interior)
{
	if(Character_TryGetHouse(playerid, house))
	{
		if(Character_IsInsideHouse(playerid, house))
		{
			if(Character_IsNearHouseExit(playerid, house))
			{
				isInside = true;
				world = House_GetEnterWorld(house);
				interior = House_GetEnterInterior(house);
				House_GetEnterPosition(house, x, y, z);
				return true;
			}
		}
		else
		{
			isInside = false;
			world = House_GetExitWorld(house);
			interior = House_GetExitInterior(house);
			return true;
		}
	}
	else if(Character_TryGetBuilding(playerid, building))
	{
		if(Character_IsInsideBuilding(playerid, building))
		{
			if(Character_IsNearBuildingEnter(playerid, building))
			{
				isInside = true;
				world = Building_GetEnterWorld(building);
				interior = Building_GetEnterInterior(building);
				Building_GetEnterPos(building, x, y, z);
				return true;
			}
		}
		else
		{
			isInside = false;
			world = Building_GetExitWorld(building);
			interior = Building_GetExitInterior(building);
			return true;
		}
	}
	return false;
}