stock Drop_IsValid(dropid)
{
	return (0 <= dropid < MAX_DROPS) && (DropInfo[dropid][dCreated]);
}

stock Drop_GetPosition(dropid, &Float:x, &Float:y, &Float:z)
{
    if(Drop_IsValid(dropid))
	{
		x = DropInfo[dropid][dX];
		y = DropInfo[dropid][dY];
		z = DropInfo[dropid][dZ];
		return 1;
	}
	return 0;
}

stock Drop_SetPosition(dropid, Float:x, Float:y, Float:z)
{
	if(Drop_IsValid(dropid))
	{
		DropInfo[dropid][dX] = x;
		DropInfo[dropid][dY] = y;
		DropInfo[dropid][dZ] = z;
	}
	return 0;
}

stock Drop_SetRotation(dropid, Float:x, Float:y, Float:z)
{
	if(Drop_IsValid(dropid))
	{
		DropInfo[dropid][dRotX] = x;
		DropInfo[dropid][dRotY] = y;
		DropInfo[dropid][dRotZ] = z;
	}
	return 0;
}

stock Drop_GetRotation(dropid, &Float:rx, &Float:ry, &Float:rz)
{
    if(Drop_IsValid(dropid))
	{
		rx = DropInfo[dropid][dRotX];
		ry = DropInfo[dropid][dRotY];
		rz = DropInfo[dropid][dRotZ];
		return 1;
	}
	return 0;
}

stock Drop_GetItemData(dropid, &itemid, &amount, &extra)
{
	if(Drop_IsValid(dropid))
	{
		itemid = DropInfo[dropid][dItem];
		amount = DropInfo[dropid][dItemAmount];
		extra = DropInfo[dropid][dItemExtra];
		return 1;
	}
	return 0;
}

stock Drop_GetItem(dropid)
{
    return DropInfo[dropid][dItem];
}

stock Drop_GetItemAmount(dropid)
{
    return DropInfo[dropid][dItemAmount];
}

stock Drop_GetItemExtra(dropid)
{
    return DropInfo[dropid][dItemExtra];
}

stock String:Drop_GetCreatedByStr(dropid)
{
    return DropInfo[dropid][dCreatedBy];
}

stock Drop_GetCreatedBy(dropid, name[])
{
    str_get(DropInfo[dropid][dCreatedBy], name); 
}

stock Drop_GetCreatedTime(dropid)
{
    return DropInfo[dropid][dCreatedTime];
}

stock Drop_IsCreated(dropid)
{
	return Drop_IsValid(dropid);
}

stock Drop_GetObject(dropid)
{
	return DropInfo[dropid][dObject];
}

stock Drop_GetInterior(dropid)
{
	return DropInfo[dropid][dInterior];
}

stock Drop_GetVirtualWorld(dropid)
{
	return DropInfo[dropid][dWorld];
}