
enum E_DATA_TYPE
{
    FLOAT = 0,
    INT = 1
};

enum _:E_KEY_TYPE
{
    KEY_STRING,
    KEY_OTHER
}

static enum _:E_STORE
{
    _:STORE_VALUE,
    E_DATA_TYPE:STORE_TYPE
};

stock Map_SetFloatData(Map:map, const key[], Float:value)
{
	if(map_has_str_key(map, key))
	{
        map_str_set_cell(map, key, STORE_VALUE, value);
        map_str_set_cell(map, key, STORE_TYPE, FLOAT);
		return true;
	}
    new data[E_STORE];
    data[STORE_VALUE] = value;
    data[STORE_TYPE] = FLOAT;
    map_str_add_arr(map, key, data);
	return true;
}

stock bool:Building_GetFloatData(Building:building, const key[], &Float:ret)
{
	new Map:data = var_get(Variant:building, Data);
	if(map_has_str_key(data, key))
	{
		ret = Float:map_get(map, key);
		return true;
	}
	return false;
}

stock bool:Building_SetFloatData(Building:building, const key[], Float:value)
{
	new Map:data = var_get(Variant:building, Data);
	if(map_has_str_key(data, key))
	{
		ret = map_set(map, key, value);
		Building_SaveData(building);
		return true;
	}
	return false;
}

stock bool:Building_SetIntData(Building:building, const key[], data)
{
	new Map:data = var_get(Variant:building, Data);
	if(map_has_str_key(data, key))
	{
		map_set(data, key, data);
		Building_SaveData(building);
		return true;
	}
	return false;
}

stock bool:Building_GetIntData(Building:building, const key[], &ret)
{
	new Map:data = var_get(Variant:building, Data);
	if(map_has_str_key(data, key))
	{
		ret = map_get(map, key);
		return true;
	}
	return false;
}

stock Map_Stringfy(Map:map, buf[], len = sizeof(buf))
{
    new Node:root = JsonArray(), Node:temp, n[32], v[E_STORE];
    for_map(i : map)
    {
		iter_get_key_str(i, n);
        iter_get_arr(i, v);
        
        if(v[STORE_TYPE] == FLOAT)
            temp = JsonObject("name", JsonString(n), "value", JsonFloat(v), "type", FLOAT);
        else if(v[STORE_TYPE] == INT)
            temp = JsonObject("name", JsonString(n), "value", JsonInt(v), "type", INT);
        root = JsonAppend(root, JsonArray(temp));
    }
    JsonStringify(root, buf, len);
}