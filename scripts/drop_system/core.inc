#include <drop_system\accessors>
#include <YSI_Coding\y_hooks>

hook OnPlayerClearData(playerid)
{
	if(pEditingDrop[playerid] != -1)
	{
		CancelEdit(pEditingDrop[playerid]);
		pEditingDrop[playerid] = -1;
	}
	return 1;
}

hook GlobalMinuteTimer(playerid)
{
    foreach(new i : DropsIterator)
    {
		//ITER_SAFE(DropsIterator, i)
		{
			if( GetTickCount() - Drop_GetCreatedTime(i) >= 1000 * 60 * MAX_DROP_TIME_AS_MIN)
			{
				new v = Drop_Destroy(i);
				if(v != -1) // used for Iter infinite loops.
					i = v;
			}

		}
	}
}

hook OnPlayerDeath(playerid, killerid, reason)
{
	if(pEditingDrop[playerid] != -1)
	{
		CancelEdit(pEditingDrop[playerid]);
		pEditingDrop[playerid] = -1;
	}
	return 1;
}

hook OnPlayerStateChange(playerid, newstate, oldstate)
{
	if(pEditingDrop[playerid] == -1) 
		return 0;
	if(oldstate == PLAYER_STATE_ONFOOT && newstate != PLAYER_STATE_ONFOOT)
	{
		CancelEdit(pEditingDrop[playerid]);
		pEditingDrop[playerid] = -1;
	}
	return 1;
}

hook OnPlayerEditDynObject(playerid, objectid, response, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz)
{
	if(pEditingDrop[playerid] != -1)
	{
		new dropid = pEditingDrop[playerid];
		if(response == EDIT_RESPONSE_CANCEL)
		{
			new Float:tx, Float:ty, Float:tz;
			Drop_GetPosition(dropid, tx, ty, tz);
			SetDynamicObjectPos(objectid, tx, ty, tz);
			Drop_GetRotation(dropid, tx, ty, tz);
			SetDynamicObjectRot(objectid, tx, ty, tz);
			pEditingDrop[playerid] = -1;
		}
		else if(response == EDIT_RESPONSE_FINAL)
		{
			DropInfo[dropid][dX] = x;
			DropInfo[dropid][dY] = y;
			DropInfo[dropid][dZ] = z;
			DropInfo[dropid][dRotX] = rx;
			DropInfo[dropid][dRotY] = ry;
			DropInfo[dropid][dRotZ] = rz;
			Drop_DestroyElements(dropid);
			Drop_CreateElements(dropid);
			pEditingDrop[playerid] = -1;
			Character_AMe(playerid, "sposta l'oggetto.");
			SendClientMessage(playerid, COLOR_GREEN, "Hai spostato l'oggetto.");
			Log(Character_GetOOCName(playerid), "", "Edit Drop", dropid);
		}
	}
	return 1;
}

stock Drop_Create(Float:x, Float:y, Float:z, world, interior, itemid, amount, extra, String:createdBy)
{
    if(itemid == 0 || amount == 0)
	   return -1;
	new id = Iter_Free(DropsIterator);
	if(id == -1)
		return -1;
	DropInfo[id][dCreated] = 1;
    DropInfo[id][dX] = x;
    DropInfo[id][dY] = y;
    DropInfo[id][dZ] = z;
	DropInfo[id][dRotX] = 90.0;
	DropInfo[id][dRotY] = 0.0;
	DropInfo[id][dRotZ] = 0.0;
    DropInfo[id][dWorld] = world;
    DropInfo[id][dInterior] = interior;
    DropInfo[id][dItem] = itemid;
    DropInfo[id][dItemAmount] = amount;
    DropInfo[id][dItemExtra] = extra;
    DropInfo[id][dCreatedBy] = createdBy;
    DropInfo[id][dCreatedTime] = GetTickCount();
    Drop_CreateElements(id);
	Iter_Add(DropsIterator, id);
    return id;
}

stock Drop_DestroyElements(dropid)
{
	if(Drop_IsValid(dropid))
	{
		DestroyDynamicObject(DropInfo[dropid][dObject]);
		Pickup_Destroy(DropInfo[dropid][dPickup]);
	}
}

stock Drop_CreateElements(dropid)
{
	if(Drop_IsValid(dropid))
	{
		DropInfo[dropid][dObject] = CreateDynamicObject(ServerItem_GetModelID(DropInfo[dropid][dItem]), DropInfo[dropid][dX], DropInfo[dropid][dY], DropInfo[dropid][dZ], DropInfo[dropid][dRotX], DropInfo[dropid][dRotY], DropInfo[dropid][dRotZ], DropInfo[dropid][dWorld], DropInfo[dropid][dInterior]);
    	DropInfo[dropid][dPickup] = Pickup_Create(1007, dropid, DropInfo[dropid][dX], DropInfo[dropid][dY], DropInfo[dropid][dZ], ELEMENT_TYPE_DROP, DropInfo[dropid][dWorld], DropInfo[dropid][dInterior]);
	}
}

stock Drop_Destroy(dropid)
{
    if(Drop_IsValid(dropid))
    {
		Drop_DestroyElements(dropid);
		DropInfo[dropid][dCreated] = 0;
		DropInfo[dropid][dX] = 0.0;
		DropInfo[dropid][dY] = 0.0;
		DropInfo[dropid][dZ] = 0.0;
		DropInfo[dropid][dWorld] = 0;
		DropInfo[dropid][dInterior] = 0;
		DropInfo[dropid][dItem] = 0;
		DropInfo[dropid][dItemAmount] = 0;
		DropInfo[dropid][dItemExtra] = 0;
		DropInfo[dropid][dCreatedBy] = @("");
		DropInfo[dropid][dCreatedTime] = 0;
		Iter_SafeRemove(DropsIterator, dropid, dropid);
		return dropid;
    }
    return -1;
}

stock Drop_IsPlayerInRangeOf(playerid, dropid, Float:range = 3.0)
{
	if(!Drop_IsValid(dropid))
		return 0;
	return GetPlayerInterior(playerid) == Drop_GetInterior(dropid) && GetPlayerVirtualWorld(playerid) == Drop_GetVirtualWorld(dropid) && IsPlayerInRangeOfPoint(playerid, range, DropInfo[dropid][dX], DropInfo[dropid][dY], DropInfo[dropid][dZ]);
}