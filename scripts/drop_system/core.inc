#include <YSI_Coding\y_hooks>

#define MAX_DROP_TIME_AS_MIN		60 //
#define MAX_DROPS					500

static enum _:E_DROP_INFO // _: for tag mismatch thing
{
    Float:X,
    Float:Y,
    Float:Z,
	Float:RotX,
	Float:RotY,
	Float:RotZ,
    /*Item,
    ItemAmount,
    ItemExtra[5],*/
    String:CreatedBy,
    CreatedTime,
    World,
    Interior,
    Object,
    Pickup
};
static
	Map:DropsMap
;

stock Map:GetDropsMap()
{
	return DropsMap;
}

hook OnGameModeInit()
{
	DropsMap = map_new();
	return 1;
}

task OnDropCheck[60000]()
{
    for_map(i : DropsMap)
    {
		new key = iter_get_key(i);
		if( GetTickCount() - Drop_GetCreatedTime(key) >= 1000 * 60 * MAX_DROP_TIME_AS_MIN)
		{
			Drop_Destroy(key, false);
			iter_erase(i);
		}	
	}
}

hook OnPlayerPickUpElmPickup(playerid, pickupid, elementid, E_ELEMENT_TYPE:type)
{
	if(type == ELEMENT_TYPE_DROP)
	{
		if(Player_HasHotKeysEnabled(playerid))
			Player_Info(playerid, "Premi ~y~~k~~PED_DUCK~~w~ per interagire", false);
		else
			Player_Info(playerid, "Digita ~y~/raccogli~w~ per interagire", false);
	}
	return 1;
}

hook OnPlayerEditDynObject(playerid, objectid, response, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz)
{
	if(EditObject_GetType(playerid) == EDIT_TYPE_DROP)
	{
		new dropid = EditObject_GetExtraID(playerid);
		if(response == EDIT_RESPONSE_CANCEL)
		{
			new Float:tx, Float:ty, Float:tz;
			
			Drop_GetPosition(dropid, tx, ty, tz);
			SetDynamicObjectPos(objectid, tx, ty, tz);

			Drop_GetRotation(dropid, tx, ty, tz);
			SetDynamicObjectRot(objectid, tx, ty, tz);

			EditObject_End(playerid);
		}
		else if(response == EDIT_RESPONSE_FINAL)
		{
			Drop_SetPosition(dropid, x, y, z);
			Drop_SetRotation(dropid, rx, ry, rz);

			Drop_DestroyElements(dropid);
			Drop_CreateElements(dropid);
			
			Character_AMe(playerid, "sposta l'oggetto.");
			SendClientMessage(playerid, COLOR_GREEN, "Hai spostato l'oggetto.");
			Log(Character_GetOOCName(playerid), "", "Edit Drop", dropid);

			EditObject_End(playerid);
		}
	}
	return 1;
}

stock Drop_Create(String:createdBy, Float:x, Float:y, Float:z, world, interior, itemid, amount, extra, Map:extraData = Map:0)
{
    if(itemid == 0 || amount == 0)
	   return 0;
	
	static id = 0;
	

	new data[E_DROP_INFO];
    data[X] = x;
    data[Y] = y;
    data[Z] = z;
	data[RotX] = 90.0;
	data[RotY] = 0.0;
	data[RotZ] = 0.0;
    data[World] = world;
    data[Interior] = interior;
    data[CreatedBy] = createdBy;
    data[CreatedTime] = GetTickCount();

	id++;
	map_add_arr(DropsMap, id, data);

	new Inventory:inventory = Inventory_New(ENTITY_TYPE_DROP, id, 1);
	Inventory_AddItem(inventory, itemid, amount, extra, extraData);
    Drop_CreateElements(id);
    return id;
}

stock Drop_DestroyElements(dropid)
{
	if(!Drop_IsValid(dropid))
		return 0;
	
	DestroyDynamicObject(Drop_GetObject(dropid));
	Pickup_Destroy(Drop_GetPickup(dropid));
	return 1;
}

stock Drop_CreateElements(dropid)
{
	if(!Drop_IsValid(dropid))
		return 0;
	new Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz;

	Drop_GetPosition(dropid, x, y, z);
	Drop_GetRotation(dropid, rx, ry, rz);
	new object = Server_CreateObject(ServerItem_GetModelID(Drop_GetItem(dropid)), x, y, z, rx, ry, rz, Drop_GetVirtualWorld(dropid), Drop_GetInterior(dropid));
	new pickupid = Pickup_Create(1007, dropid, x, y, z, ELEMENT_TYPE_DROP, Drop_GetVirtualWorld(dropid), Drop_GetInterior(dropid));
	
	map_set_cell(DropsMap, dropid, Object, object);
	map_set_cell(DropsMap, dropid, Pickup, pickupid);

	return 1;
}

stock Drop_Destroy(dropid, bool:remove = true)
{
    if(Drop_IsValid(dropid))
    {
		Drop_DestroyElements(dropid);
		Inventory_Delete(Drop_GetInventory(dropid));
		if(remove)
			map_remove(DropsMap, dropid);
		return dropid;
    }
    return 0;
}

stock Drop_IsPlayerInRangeOf(playerid, dropid, Float:range = 3.0)
{
	if(!Drop_IsValid(dropid))
		return 0;
	new Float:x, Float:y, Float:z;
	Drop_GetPosition(dropid, x, y, z);
	return GetPlayerInterior(playerid) == Drop_GetInterior(dropid) && Character_GetVirtualWorld(playerid) == Drop_GetVirtualWorld(dropid) && IsPlayerInRangeOfPoint(playerid, range, x, y, z);
}

stock Drop_IsValid(dropid)
{
	return map_has_key(DropsMap, dropid);
}

stock Drop_GetPosition(dropid, &Float:x, &Float:y, &Float:z)
{
    if(Drop_IsValid(dropid))
	{
		x = Float:map_get(DropsMap, dropid, X);
		y = Float:map_get(DropsMap, dropid, Y);
		z = Float:map_get(DropsMap, dropid, Z);
		return 1;
	}
	return 0;
}

stock Drop_SetPosition(dropid, Float:x, Float:y, Float:z)
{
	if(Drop_IsValid(dropid))
	{
		map_set_cell(DropsMap, dropid, X, x);
		map_set_cell(DropsMap, dropid, Y, y);
		map_set_cell(DropsMap, dropid, Z, z);
		return 1;
	}
	return 0;
}

stock Drop_SetRotation(dropid, Float:x, Float:y, Float:z)
{
	if(Drop_IsValid(dropid))
	{
		map_set_cell(DropsMap, dropid, RotX, x);
		map_set_cell(DropsMap, dropid, RotY, y);
		map_set_cell(DropsMap, dropid, RotZ, z);
		return 1;
	}
	return 0;
}

stock Drop_GetRotation(dropid, &Float:rx, &Float:ry, &Float:rz)
{
    if(Drop_IsValid(dropid))
	{
		rx = Float:map_get(DropsMap, dropid, RotX);
		ry = Float:map_get(DropsMap, dropid, RotY);
		rz = Float:map_get(DropsMap, dropid, RotZ);
		return 1;
	}
	return 0;
}

stock Inventory:Drop_GetInventory(dropid)
{
	if(!Drop_IsValid(dropid))
		return INVENTORY_NULL;
	return Inventory_Get(ENTITY_TYPE_DROP, dropid);
}

stock Drop_GetItemData(dropid, &itemid, &amount, &extra)
{
	if(Drop_IsValid(dropid))
	{
		new Inventory:inventory = Drop_GetInventory(dropid);
		itemid = Inventory_GetSlotItem(inventory, 0);
		amount = Inventory_GetSlotAmount(inventory, 0);
		extra = Inventory_GetSlotExtra(inventory, 0, 0);
		return 1;
	}
	return 0;
}

stock Drop_GetItem(dropid)
{
	if(!Drop_IsValid(dropid))
		return 0;
	return Inventory_GetSlotItem(Drop_GetInventory(dropid), 0);
}

stock Drop_GetItemAmount(dropid)
{
	if(!Drop_IsValid(dropid))
		return 0;
	return Inventory_GetSlotAmount(Drop_GetInventory(dropid), 0);
}

stock Drop_GetItemExtra(dropid, index = 0)
{
	if(!Drop_IsValid(dropid))
		return 0;
	return Inventory_GetSlotExtra(Drop_GetInventory(dropid), 0, index);
}

stock Drop_GetCreatedTime(dropid)
{
	if(!Drop_IsValid(dropid))
		return 0;
	return map_get(DropsMap, dropid, CreatedTime);
}

stock Drop_IsCreated(dropid)
{
	return Drop_IsValid(dropid);
}

stock Drop_GetInterior(dropid)
{
	if(!Drop_IsValid(dropid))
		return 0;
	return map_get(DropsMap, dropid, Interior);
}

stock Drop_GetVirtualWorld(dropid)
{
	if(!Drop_IsValid(dropid))
		return 0;
	return map_get(DropsMap, dropid, World);
}

stock Drop_GetObject(dropid)
{
	if(!Drop_IsValid(dropid))
		return 0;
	return map_get(DropsMap, dropid, Object);
}

stock Drop_GetPickup(dropid)
{
	if(!Drop_IsValid(dropid))
		return 0;
	return map_get(DropsMap, dropid, Pickup);
}

hook function bool:Entity_GetWorldPosition(EntityType:entityType, entityId, &Float:x, &Float:y, &Float:z)
{
	if(entityType != ENTITY_TYPE_DROP)
		return bool:continue(_:entityType, entityId, x, y, z);
	
	if(!Drop_IsValid(entityId))
		return false;

	x = y = z = 0.0;
	new world = Drop_GetVirtualWorld(entityId);
    if(world < HOUSE_START_WORLD)
    {
        Drop_GetPosition(entityId, x, y, z);
    }
    else if(world < BUILDING_START_WORLD)
    {
        new House:house = House_Find(Drop_GetVirtualWorld(entityId) - HOUSE_START_WORLD);
        if(House_IsValid(house))
		{
        	House_GetEnterPosition(house, x, y, z);
		}
    }
    else if(world >= BUILDING_START_WORLD)
    {
        new Building:building = Building_Find(Drop_GetVirtualWorld(entityId) - BUILDING_START_WORLD);
        if(Building_IsValid(building))
        {
        	Building_GetEnterPos(building, x, y, z);
		}
    }
	return x != 0.0 && y != 0.0 && z != 0.0;
}