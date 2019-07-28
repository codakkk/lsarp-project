
stock Faction_IsValid(factionid)
{
	return 0 <= factionid < MAX_FACTIONS && FactionInfo[factionid][fID];
}

stock Faction_GetID(factionid)
{
	return FactionInfo[factionid][fID];
}

stock Faction_SetName(factionid, const name[])
{
	set(FactionInfo[factionid][fName], name);
}

stock String:Faction_GetNameStr(factionid)
{
	return str_new(FactionInfo[factionid][fName]);
}

stock Faction_SetShortName(factionid, const name[])
{
	set(FactionInfo[factionid][fShortName], name);
}

stock String:Faction_GetShortNameStr(factionid)
{
	return str_new(FactionInfo[factionid][fShortName]);
}

stock Faction_SetType(factionid, type)
{
	FactionInfo[factionid][fType] = type;
}

stock Faction_GetType(factionid)
{
	if(!Faction_IsValid(factionid))
		return -1;
	return FactionInfo[factionid][fType];
}

stock Faction_GetSpawnPos(factionid, &Float:x, &Float:y, &Float:z)
{
	if(!Faction_IsValid(factionid))
		return 0;
	x = FactionInfo[factionid][fSpawnX];
	y = FactionInfo[factionid][fSpawnY];
	z = FactionInfo[factionid][fSpawnZ];
	return 1;
}

stock Faction_SetSpawnPos(factionid, Float:x, Float:y, Float:z, interior, world)
{
	if(!Faction_IsValid(factionid))
		return 0;
	FactionInfo[factionid][fSpawnX] = x;
	FactionInfo[factionid][fSpawnY] = y;
	FactionInfo[factionid][fSpawnZ] = z;
	FactionInfo[factionid][fInterior] = interior;
	FactionInfo[factionid][fWorld] = world;
	return 1;
}

stock Faction_GetSpawnInterior(factionid)
{
	return FactionInfo[factionid][fInterior];
}

stock Faction_GetSpawnWorld(factionid)
{
	return FactionInfo[factionid][fWorld];
}

stock Faction_GetRankName(factionid, rank, name[])
{
	if(!Faction_IsValid(factionid))
		return 0;
	name = FactionRankInfo[factionid][rank][rankName];
	return 1;
}

stock String:Faction_GetRankNameStr(factionid, rank)
{
	if(rank == -1)
		return @("Indefinito");
	return str_new(FactionRankInfo[factionid][rank][rankName]);
}

stock String:Faction_SetRankName(factionid, rank, name[])
{
	set(FactionRankInfo[factionid][rank][rankName], name);
}

stock Faction_GetSkin(factionid, slotid)
{
	return FactionSkins[factionid][slotid];
}

stock Faction_SetSkin(factionid, slotid, skinid)
{
	FactionSkins[factionid][slotid] = skinid;
}

stock Faction_GetRankSalary(factionid, rank)
{
	return FactionRankInfo[factionid][rank][rankSalary];
}

stock Faction_SetRankSalary(factionid, rank, salary)
{
	FactionRankInfo[factionid][rank][rankSalary] = salary;
}