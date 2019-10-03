

#if defined _included_HOUSE
    #endinput
#endif 

#define _included_HOUSE 

#define INVALID_HOUSE_ID	(0)
#define INVALID_HOUSE	    (House:0)
#define HOUSE_START_WORLD 	(1000)

#define for_house(%0) for(new Iter:iter=map_iter(GetHousesMap()), House:%0=House:iter_get_value(iter);iter_inside(iter);%0=(iter_move_next(iter) != ITER_NULL ? (House:iter_get_value(iter)) : (House:0)))

#include <building_system\core>

#include <house_system\components\inventory>
#include <house_system\components\interiors>

#include <house_system\commands>


#include <YSI_Coding\y_hooks>


static enum _:E_HOUSE_DATA
{
    ID,

    OwnerID,
    OwnerName[MAX_PLAYER_NAME],
    Price,
    bool:IsLocked,
	Money,

    Float:EnterX,
    Float:EnterY,
    Float:EnterZ,
    EnterInterior,
    EnterWorld,

    Float:ExitX,
    Float:ExitY,
    Float:ExitZ,
    ExitInterior,
    ExitWorld,
	
    bool:IsRentable,
    RentPrice,

    InteriorType,
	InteriorID,

    EnterCheckpoint,
    ExitCheckpoint,

    Text3D:h3DText,
};

static 
    Map:Houses;

static SelectedPage[MAX_PLAYERS char], List:PlayerRenters[MAX_PLAYERS], SelectedRenter[MAX_PLAYERS];

static const RENTERS_PER_PAGE = 10;

stock Map:GetHousesMap()
{
    return Houses;
}

hook OnGameModeInit()
{
    Houses = map_new();
    House_LoadAll();
    return 1;
}

stock bool:Character_CanUseHouseMenu(playerid, House:house)
{
	if(!House_IsValid(house))
		return false;
	if(Player_IsAdminDuty(playerid) || Character_DoesOwnHouse(playerid, house))
		return true;
	return false;
}

hook function Character_Enter(playerid)
{
    if(IsPlayerInAnyVehicle(playerid))
        return continue(playerid);

    new House:house = Character_GetHouse(playerid, false);
    if(!House_IsValid(house))
        house = Character_GetHouse(playerid, true);
    
    if(House_IsValid(house))
    {
        
        new 
            Float:x = 0.0,
            Float:y = 0.0,
            Float:z = 0.0,
            Float:enterX = 0.0, 
            Float:enterY = 0.0, 
            Float:enterZ = 0.0,
            Float:exitX = 0.0, 
            Float:exitY = 0.0, 
            Float:exitZ = 0.0,
            bool:isEnter = false,
            interiorId = 0,
            world = 0;
        
        House_GetEnterPosition(house, enterX, enterY, enterZ);
        House_GetExitPosition(house, exitX, exitY, exitZ);

        if(IsPlayerInRangeOfPoint(playerid, 2.0, enterX, enterY, enterZ))
        {
            x = exitX;
            y = exitY;
            z = exitZ;
            interiorId = House_GetExitInterior(house);
            world = House_GetExitWorld(house);
            isEnter = true;
        }
        else if(IsPlayerInRangeOfPoint(playerid, 2.0, exitX, exitY, exitZ))
        {
            x = enterX;
            y = enterY;
            z = enterZ;
            interiorId = House_GetEnterInterior(house);
            world = House_GetEnterWorld(house);
            isEnter = false;
        }
        else
        {
            return continue(playerid);
        }
        
        if(House_IsLocked(house))
            return GameTextForPlayer(playerid, "~r~Porta Chiusa", 5000, 1);
        
        Character_AMe(playerid, isEnter ? ("apre la porta ed entra.") : ("apre la porta ed esce."));
        
        Streamer_UpdateEx(playerid, x, y, z, world, interiorId);
        Character_SetSafePosition(playerid, x, y, z, interiorId, world);
        return 1;
    }
    return continue(playerid);
}

hook OnCharacterClearData(playerid)
{
    SelectedPage{playerid} = 0;
    SelectedRenter[playerid] = 0;
    if(list_valid(PlayerRenters[playerid]))
        list_delete_deep(PlayerRenters[playerid]);
    return 1;
}

hook OnCharacterChangeName(playerid, newCharacterName[])
{
	for_house(house)
	{
		if(House_GetOwnerID(house) == Character_GetID(playerid))
		{
            var_set_cells(Variant:house, OwnerName, newCharacterName, MAX_PLAYER_NAME);
		}
	}
	return 1;
}

hook OnPlayerEnterDynamicCP(playerid, checkpointid)
{
    new House:house;
    if(Checkpoint_GetInfoOfType(checkpointid, CHECKPOINT_TYPE_HOUSE, house) && House_IsValid(house))
    {
        if(!House_IsOwned(house))
        {
            SendClientMessage(playerid, 0x33AA33FF, "Vuoi comprare questo immobile?");
            SendFormattedMessage(playerid, 0x33AA33FF, "Prezzo: $%s", FormatNumber(House_GetPrice(house)));
            SendClientMessage(playerid, -1, "/compra");
        }
        SendClientMessage(playerid, 0x33AA33FF, House_GetAddress(house));
        SendClientMessage(playerid, -1, "Comandi disponibili: /entra, /ds(hout), /ddo, /bussa");
    }
    return 1;
}

stock House:House_Find(houseid)
{
    if(!map_has_key(Houses, houseid))
        return INVALID_HOUSE;
    return House:map_get(Houses, houseid);
}

stock String:House_GetAddressStr(House:house)
{
    return str_new(House_GetAddress(house));
}

stock House_GetAddress(House:house)
{   
    new zone[64], Float:x, Float:y, Float:z;
        
    if(House_GetEnterWorld(house) > BUILDING_START_WORLD)
    {
        new Building:building = Building_Find(House_GetEnterWorld(house) - BUILDING_START_WORLD);
        if(Building_IsValid(building) && Building_GetType(building) == BUILDING_TYPE_COMPLEX)
        {
            Building_GetEnterPos(building, x, y, z);
        }
    }
    else
    {
        House_GetEnterPosition(house, x, y, z);
    }
    Get2DZoneName(x, y, z, zone, 32);
    format(zone, sizeof(zone), "%d %s, Las Venturas, San Andreas", House_GetID(house), zone);
    return zone;
}


stock House_Create(Float:x, Float:y, Float:z, int, world, interiorType)
{
    new Cache:result = mysql_query_f(gMySQL, true, "INSERT INTO `houses` \
    (owner_id, enter_x, enter_y, enter_z, enter_interior, enter_world, price, locked) \
    VALUES(0, %f, %f, %f, %d, %d, 0, 1)", x, y, z, int, world);

    new 
        id = cache_insert_id()
    ;
    new House:house = House:var_new_buf(E_HOUSE_DATA);
    var_acquire(Variant:house);
    var_set_cell(Variant:house, ID, id);
    var_set_cell(Variant:house, OwnerID, 0);
    var_set_cells(Variant:house, OwnerName, "");
    var_set_cell(Variant:house, Price, 0);
    var_set_cell(Variant:house, EnterX, _:x);
    var_set_cell(Variant:house, EnterY, _:y);
    var_set_cell(Variant:house, EnterZ, _:z);
    var_set_cell(Variant:house, EnterInterior, int);
    var_set_cell(Variant:house, EnterWorld, world);
    var_set_cell(Variant:house, ExitX, 0.0);
    var_set_cell(Variant:house, ExitY, 0.0);
    var_set_cell(Variant:house, ExitZ, 0.0);
    var_set_cell(Variant:house, ExitInterior, 0);
    var_set_cell(Variant:house, ExitWorld, HOUSE_START_WORLD + id);
    var_set_cell(Variant:house, IsLocked, true);
    var_set_cell(Variant:house, IsRentable, false);
    var_set_cell(Variant:house, RentPrice, 0);
    map_add(Houses, id, _:house);

    House_SetInterior(house, interiorType, 0);
    House_CreateObjects(house); // Already called by House_SetInterior
    CallLocalFunction(#OnHouseLoaded, "d", _:house);
    cache_delete(result);
    return id;
}

stock House_Delete(House:house)
{
    if(!House_IsValid(house))
        return 0;

    new query[64];
    mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `houses` WHERE id = '%d'", House_GetID(house));
    mysql_tquery(gMySQL, query);

    House_DestroyObjects(house);

    var_release(Variant:house);

    map_remove_deep(Houses, House_GetID(house));
    return 1;
}

stock bool:House_CreateObjects(House:house)
{
    if(!House_IsValid(house))
	   return false;

    House_DestroyObjects(house);
    new 
	   Float:x, Float:y, Float:z,
	   Float:ex, Float:ey, Float:ez,
	   interior = House_GetEnterInterior(house),
	   einterior = House_GetExitInterior(house),
	   enterWorld = House_GetEnterWorld(house),
       exitWorld = House_GetExitWorld(house);
    
    House_GetEnterPosition(house, x, y, z);
    House_GetExitPosition(house, ex, ey, ez);

    new text[128];
    
    if(!House_IsOwned(house))
    {
        format(text, sizeof(text), "Appartamento[%d] In Vendita:\nPrezzo: $%s\nInterior: %d", House_GetID(house), FormatNumber(House_GetPrice(house)), House_GetInteriorID(house));
	    new Text3D:label = CreateDynamic3DTextLabel(text, 0xC38A39FF, x, y, z + 0.55, 5.0, .worldid = enterWorld, .interiorid = interior, .testlos = 1);
        var_set_cell(Variant:house, h3DText, _:label);
    }

    new 
       enterCheckpoint = Checkpoint_Create(CHECKPOINT_TYPE_HOUSE, house, x, y, z - 0.3, 2.5, enterWorld, interior, 5.0),
       exitCheckpoint = Checkpoint_Create(CHECKPOINT_TYPE_HOUSE, house, ex, ey, ez - 0.3, 2.2, exitWorld, einterior, 5.0)
    ;

    var_set_cell(Variant:house, EnterCheckpoint, _:enterCheckpoint);
    var_set_cell(Variant:house, ExitCheckpoint, _:exitCheckpoint);
    return true;
}

stock bool:House_DestroyObjects(House:house)
{
    if(!House_IsValid(house))
	   return false;
    
    new
        enterCheckpoint = var_get(Variant:house, EnterCheckpoint),
        exitCheckpoint = var_get(Variant:house, ExitCheckpoint),
	    Text3D:label = House_Get3DText(house);
    
    if(IsValidDynamicCP(enterCheckpoint))
	   DestroyDynamicCP(enterCheckpoint);
    if(IsValidDynamicCP(exitCheckpoint))
	   DestroyDynamicCP(exitCheckpoint);
    if(IsValidDynamic3DTextLabel(label))
	   DestroyDynamic3DTextLabelEx(label);
    return true;
}

stock House_IsValid(House:house)
{
    return var_valid(Variant:house);
}

stock House_GetID(House:house)
{
    return var_get(Variant:house, ID);
}

stock bool:House_SetOwner(House:house, playerid)
{
    if(!IsPlayerConnected(playerid) || !Character_IsLogged(playerid) || Character_HasRentHouse(playerid))
	   return false;
    
    House_ResetOwner(house);

    var_set_cell(Variant:house, OwnerID, Character_GetID(playerid));
    var_set_cells(Variant:house, OwnerName, Character_GetOOCName(playerid));

    Character_AddOwnedHouse(playerid, house);

    House_CreateObjects(house);

    mysql_tquery_f(gMySQL, "UPDATE `houses` SET owner_id = '%d' WHERE id = '%d'", Character_GetID(playerid), House_GetID(house));
    return true;
}

stock House_ResetOwner(House:house)
{
    var_set_cell(Variant:house, OwnerID, 0);
    var_set_cells(Variant:house, OwnerName, "");

    foreach(new i : Player)
	{
		if(!Character_DoesOwnHouse(i, house))
			continue;
		Character_RemoveOwnedHouse(i, house);
	}

    House_CreateObjects(house);

    mysql_tquery_f(gMySQL, "UPDATE `houses` SET owner_id = '0' WHERE id = '%d'", House_GetID(house));
    
    return 1;
}

stock House_GetOwnerID(House:house)
{
    return var_get(Variant:house, OwnerID);
}

stock House_IsOwned(House:house)
{
    return House_GetOwnerID(house) > 0;
}

stock bool:House_GetOwnerName(House:house, name[MAX_PLAYER_NAME])
{
    var_get_md_arr(Variant:house, {OwnerName}, name);
    return true;
}

stock String:House_GetOwnerNameStr(House:house)
{
    new name[MAX_PLAYER_NAME];
    House_GetOwnerName(house, name);
    return str_new(name);
}

stock House_SetEnterPosition(House:house, Float:x, Float:y, Float:z)
{
    var_set_cell(Variant:house, EnterX, x);
    var_set_cell(Variant:house, EnterY, y);
    var_set_cell(Variant:house, EnterZ, z);

    mysql_tquery_f(gMySQL, "UPDATE `houses` SET enter_x = '%f', enter_y = '%f', enter_z = '%f' WHERE id = '%d'", x, y, z, House_GetID(house));
    return 1;
}

stock House_GetEnterPosition(House:house, &Float:X, &Float:Y, &Float:Z)
{
    X = Float:var_get(Variant:house, EnterX);
    Y = Float:var_get(Variant:house, EnterY);
    Z = Float:var_get(Variant:house, EnterZ);
    return 1;
}

stock House_SetEnterInterior(House:house, interior)
{
    var_set_cell(Variant:house, EnterInterior, interior);

    mysql_tquery_f(gMySQL, "UPDATE `houses` SET enter_interior = '%d' WHERE id = '%d'", interior, House_GetID(house));
    return 1;
}

stock House_GetEnterInterior(House:house)
{
    return var_get(Variant:house, EnterInterior);
}

stock House_SetEnterWorld(House:house, world)
{
    var_set_cell(Variant:house, EnterWorld, world);

    mysql_tquery_f(gMySQL, "UPDATE `houses` SET enter_world = '%d' WHERE id = '%d'", world, House_GetID(house));
    return 1;
}

stock House_GetEnterWorld(House:house)
{
    return var_get(Variant:house, EnterWorld);
}

stock House_GetExitWorld(House:house)
{
    return var_get(Variant:house, ExitWorld);
}

stock House_SetExitPosition(House:house, Float:x, Float:y, Float:z)
{
    var_set_cell(Variant:house, ExitX, x);
    var_set_cell(Variant:house, ExitY, y);
    var_set_cell(Variant:house, ExitZ, z);

    mysql_tquery_f(gMySQL, "UPDATE `houses` SET exit_x = '%f', exit_y = '%f', exit_z = '%f' WHERE id = '%d'", x, y, z, House_GetID(house));
    return 1;
}

stock House_GetExitPosition(House:house, &Float:X, &Float:Y, &Float:Z)
{
    X = Float:var_get(Variant:house, ExitX);
    Y = Float:var_get(Variant:house, ExitY);
    Z = Float:var_get(Variant:house, ExitZ);
    return 1;
}

stock House_SetExitInterior(House:house, interior)
{
    var_set_cell(Variant:house, ExitInterior, interior);
    mysql_tquery_f(gMySQL, "UPDATE `houses` SET exit_interior = '%d' WHERE id = '%d'", interior, House_GetID(house));
    return 1;
}

stock House_GetExitInterior(House:house)
{
    return var_get(Variant:house, ExitInterior);
}

stock bool:House_SetInterior(House:house, interiorType, interiorid)
{
    if(!HouseInterior_IsValidType(interiorType))
	   return false;

    new data[E_INTERIORS_HOUSE];
    if(!HouseInterior_Get(interiorType, interiorid, data))
        return false;

    var_set_cell(Variant:house, InteriorType, interiorType);
    var_set_cell(Variant:house, InteriorID, interiorid);

    mysql_tquery_f(gMySQL, "UPDATE `houses` SET interior_id = '%d', interior_type = '%d' WHERE id = '%d'", interiorid, interiorType, House_GetID(house));
    
    House_SetExitPosition(house, data[iHouseX], data[iHouseY], data[iHouseZ]);
    House_SetExitInterior(house, data[iHouseInt]);
    House_CreateObjects(house);
    return true;
}

stock House_GetInteriorType(House:house)
{
    return var_get(Variant:house, InteriorType);
}

stock House_GetInteriorID(House:house)
{
    return var_get(Variant:house, InteriorID);
}

stock House_SetPrice(House:house, price)
{
    var_set_cell(Variant:house, Price, price);

    House_CreateObjects(house);

    mysql_tquery_f(gMySQL, "UPDATE `houses` SET price = '%d' WHERE id = '%d'", price, House_GetID(house));
    return 1;
}

stock House_GetPrice(House:house)
{
    return var_get(Variant:house, Price);
}

stock House_SetLocked(House:house, bool:lock)
{
    var_set_cell(Variant:house, IsLocked, lock);
    mysql_tquery_f(gMySQL, "UPDATE `houses` SET locked = '%d' WHERE id = '%d'", lock?1:0, House_GetID(house));
    return 1;
}

stock bool:House_IsLocked(House:house)
{
    return bool:var_get(Variant:house, IsLocked);
}

stock House_GetMoney(House:house)
{
    return var_get(Variant:house, Money);
}

stock House_GiveMoney(House:house, money)
{
    House_SetMoney(house, House_GetMoney(house) + money);
    return 1;
}

stock House_SetMoney(House:house, amount)
{
    var_set_cell(Variant:house, Money, amount);
    mysql_tquery_f(gMySQL, "UPDATE `houses` SET money = '%d' WHERE id = '%d'", amount, House_GetID(house));
    return 1;
}

stock Text3D:House_Get3DText(House:house)
{
    return Text3D:var_get(Variant:house, h3DText);
}

stock bool:House_SetRentable(House:house, bool:rentable)
{
    var_set_cell(Variant:house, IsRentable, rentable);
    mysql_tquery_f(gMySQL, "UPDATE `houses` SET is_rentable = '%d' WHERE id = '%d'", rentable?1:0, House_GetID(house));
    return true;
}

stock bool:House_IsRentable(House:house)
{
    return bool:var_get(Variant:house, IsRentable);
}

stock bool:House_SetRentPrice(House:house, price)
{
    var_set_cell(Variant:house, RentPrice, price);
    mysql_tquery_f(gMySQL, "UPDATE `houses` SET rent_price = '%d' WHERE id = '%d'", price, House_GetID(house));
    return true;
}

stock House_GetRentPrice(House:house)
{
    return var_get(Variant:house, RentPrice);
}

stock House_LoadAll()
{
    inline OnLoad()
    {
        new count = cache_num_rows(), data[E_HOUSE_DATA];
        for(new i = 0; i < count; ++i)
        {
            cache_get_value_name_int(i, "id", data[ID]);
            cache_get_value_name_int(i, "owner_id", data[OwnerID]);

            cache_get_value_name_float(i, "enter_x", data[EnterX]);
            cache_get_value_name_float(i, "enter_y", data[EnterY]);
            cache_get_value_name_float(i, "enter_z", data[EnterZ]);
            cache_get_value_name_int(i, "enter_interior", data[EnterInterior]);
            cache_get_value_name_int(i, "enter_world", data[EnterWorld]);

            cache_get_value_name_float(i, "exit_x", data[ExitX]);
            cache_get_value_name_float(i, "exit_y", data[ExitY]);
            cache_get_value_name_float(i, "exit_z", data[ExitZ]);
            cache_get_value_name_int(i, "exit_interior", data[ExitInterior]);

            cache_get_value_name_int(i, "price", data[Price]);
            cache_get_value_name_bool(i, "locked", data[IsLocked]);
            cache_get_value_name_int(i, "money", data[Money]);

            cache_get_value_name_bool(i, "is_rentable", data[IsRentable]);
            cache_get_value_name_int(i, "rent_price", data[RentPrice]);

            cache_get_value_name_int(i, "interior_id", data[InteriorID]);
            cache_get_value_name_int(i, "interior_type", data[InteriorType]);

            data[ExitWorld] = HOUSE_START_WORLD + data[ID];

            new House:house = House:var_new_buf(E_HOUSE_DATA);

            var_acquire(Variant:house);
            var_set_cell(Variant:house, ID, data[ID]);
            var_set_cell(Variant:house, OwnerID, data[OwnerID]);
            var_set_cells(Variant:house, OwnerName, data[OwnerName]);
            var_set_cell(Variant:house, Price, data[Price]);

            var_set_cell(Variant:house, EnterX, _:data[EnterX]);
            var_set_cell(Variant:house, EnterY, _:data[EnterY]);
            var_set_cell(Variant:house, EnterZ, _:data[EnterZ]);
            var_set_cell(Variant:house, EnterInterior, data[EnterInterior]);
            var_set_cell(Variant:house, EnterWorld, data[EnterWorld]);

            var_set_cell(Variant:house, ExitX, data[ExitX]);
            var_set_cell(Variant:house, ExitY, data[ExitY]);
            var_set_cell(Variant:house, ExitZ, data[ExitZ]);
            var_set_cell(Variant:house, ExitInterior, data[ExitInterior]);
            var_set_cell(Variant:house, ExitWorld, data[ExitWorld]);

            var_set_cell(Variant:house, IsLocked, data[IsLocked]);
            var_set_cell(Variant:house, Money, data[Money]);
            var_set_cell(Variant:house, IsRentable, data[IsRentable]);
            var_set_cell(Variant:house, RentPrice, data[RentPrice]);

            var_set_cell(Variant:house, InteriorID, data[InteriorID]);
            var_set_cell(Variant:house, InteriorType, data[InteriorType]);

            map_add(Houses, data[ID], _:house);
            House_CreateObjects(house);
            
            CallLocalFunction(#OnHouseLoaded, "d", _:house);
        }
        

        for_house(house)
        {
            if(House_GetOwnerID(house) > 0)
            {
                new Cache:cache = mysql_query_f(gMySQL, true, "SELECT name FROM `characters` WHERE id = '%d'", House_GetID(house));
                if(cache_num_rows() > 0)
                {
                    new name[MAX_PLAYER_NAME];
                    cache_get_value_name(0, "name", name);
                    var_set_cells(Variant:house, OwnerName, name);
                }
                cache_delete(cache);
            }

        }
    }
    MySQL_PQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `houses` ORDER BY id");
}

stock House_ShowRentersList(House:house, playerid, page = 0)
{
    if(page < 0)
        return 0;
    inline OnLoad()
    {
        if(cache_num_rows() <= 0)
            return SendClientMessage(playerid, COLOR_ERROR, "Non ci sono affittuari in questa casa.");
        
        if(!list_valid(PlayerRenters[playerid]))
            PlayerRenters[playerid] = list_new();

        list_clear(PlayerRenters[playerid]);

        SelectedPage{playerid} = page;

        new 
            rows = cache_num_rows(), String:content = @("Nome\tPrezzo\n"),
            id,
            name[MAX_PLAYER_NAME],
            price;

        if(page > 0)
            content += @("Pagina\tPrecedente\n");
        
        if(rows > RENTERS_PER_PAGE)
            content += @("Pagina\tSuccessiva\n");
        //rows--; // This is because I added 1 to RENTERS_PER_PAGE, just to check if there atleast one entry
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name_int(i, "id", id);
            cache_get_value_name(i, "name", name);
            cache_get_value_name_int(i, "house_rent_price", price);
            
            list_add(PlayerRenters[playerid], id);

            if(i <= RENTERS_PER_PAGE)
                content += str_format("{FFFFFF}%s\t{8FBC8F}$%s{FFFFFF}\n", name, FormatNumber(price));
        }
        Dialog_Show_s(playerid, Dialog_HouseRentersList, DIALOG_STYLE_TABLIST_HEADERS, House_GetAddressStr(house), content, "Modifica", "Indietro");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT id, name, house_rent_price FROM `characters` WHERE house_rent = '%d' LIMIT %d, %d", House_GetID(house), page * RENTERS_PER_PAGE, RENTERS_PER_PAGE + 1);
    return 1;
}

Dialog:Dialog_HouseRentersList(playerid, response, listitem, inputtext[])
{
    new House:house = Character_GetHouseMenu(playerid);
	if(!House_IsValid(house))
		return 0;
    
    if(!response)
        return Character_ShowHouseDialog(playerid);
    if(SelectedPage{playerid} > 0)
    {
        if(listitem == 0)
        {
            House_ShowRentersList(house, playerid, SelectedPage{playerid}-1);
            return 1;
        }
        listitem--;
    }
    if(list_size(PlayerRenters[playerid]) > RENTERS_PER_PAGE)
    {
        if(listitem == 0)
        {
            House_ShowRentersList(house, playerid, SelectedPage{playerid} + 1);
            return 1;
        }
        listitem--;
    }
    SelectedRenter[playerid] = list_get(PlayerRenters[playerid], listitem);
    Character_ShowRenterAction(playerid, SelectedRenter[playerid]);
    return 1;
}

stock Character_ShowRenterAction(playerid, renterid)
{
    inline OnLoad()
    {
        if(cache_num_rows() <= 0)
            return 0;
        new name[MAX_PLAYER_NAME], price;
        cache_get_value_name(0, "name", name);
        cache_get_value_name_int(0, "house_rent_price", price);
        Dialog_Show_s(playerid, Dialog_HouseRenterAction, DIALOG_STYLE_LIST, str_format("%s - $%s", name, FormatNumber(price)), @("Caccia"), "Seleziona", "Indietro");
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT name, house_rent_price FROM `characters` WHERE id = '%d'", renterid);
    return 1;
}

Dialog:Dialog_HouseRenterAction(playerid, response, listitem, inputtext[])
{
    new House:house = Character_GetHouseMenu(playerid);
	if(!House_IsValid(house))
		return 0;
    
    if(!response)
        return House_ShowRentersList(house, playerid, SelectedPage{playerid});
    
    if(listitem == 0)
    {
        new bool:notified = false;
        foreach(new i : Player)
        {
            if(SelectedRenter[playerid] == Character_GetID(i))
            {
                SendClientMessage(playerid, COLOR_ORANGE, "Il proprietario della casa di cui eri affittuario ti ha cacciato.");
                Character_SetRentHouse(playerid, INVALID_HOUSE);
                notified = true;
                break;
            }
        }
        mysql_tquery_f(gMySQL, "UPDATE `characters` SET house_rent = '%d', house_rent_price = '0' WHERE id = '%d'", INVALID_HOUSE_ID, SelectedRenter[playerid]);
        SendClientMessage(playerid, COLOR_ORANGE, "Hai cacciato l'utente da casa tua.");
        House_ShowRentersList(house, playerid, SelectedPage{playerid});

        if(notified == false)
        {
            SendNotification(SelectedRenter[playerid], "Il proprietario della casa di cui eri affittuario ti ha cacciato.");
        }
    }
    return 1;
}
