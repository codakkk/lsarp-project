#include <YSI\y_hooks>
#include <building/building_player.pwn>
#include <building/commands/building_admin.pwn>

hook OnGameModeInit()
{
    Building_LoadAll();
    printf("building.pwn/ongamemodeinit");
    return 1;
}

stock Building_Create(name[], Float:x, Float:y, Float:z, interior, world)
{
    new freeid = Iter_Free(Buildings);
    if(freeid == -1)
        return 0;
    inline OnInsert()
    {
        BuildingInfo[freeid][bID] = cache_insert_id();

        set(BuildingInfo[freeid][bName], name);
        set(BuildingInfo[freeid][bWelcomeText], "");
        set(BuildingInfo[freeid][bOwnerName], "");
        
        BuildingInfo[freeid][bEnterX] = x;
        BuildingInfo[freeid][bEnterY] = y;
        BuildingInfo[freeid][bEnterZ] = z;
        BuildingInfo[freeid][bEnterInterior] = interior;
        BuildingInfo[freeid][bEnterWorld] = world;
        BuildingInfo[freeid][bOwnable] = 0;
        BuildingInfo[freeid][bOwnerID] = 0;
        BuildingInfo[freeid][bPrice] = 0;
        BuildingInfo[freeid][bLocked] = 1;
        BuildingInfo[freeid][bExists] = 1;

        Building_CreateElements(freeid);

        Iter_Add(Buildings, freeid);
    }
    new query[512];
    mysql_format(gMySQL, query, sizeof(query), "INSERT INTO buildings (Name, OwnerName, WelcomeText, EnterX, EnterY, EnterZ, EnterInterior, EnterWorld, ExitX, ExitY, ExitZ, ExitInterior, Ownable, OwnerID, Price, Locked) \
                                 VALUES('%e', '', '', '%f', '%f', '%f', '%d', '%d', '0.0', '0.0', '0.0', '0', '0', '0', '0', '1')", 
                                                    name, x, y, z, interior, world);
    mysql_tquery_inline(gMySQL, query, using inline OnInsert);      
    return freeid;
}

stock Building_LoadAll()
{
    inline OnLoad()
    {
        new count = cache_num_rows();
        if(count > MAX_BUILDINGS)
            count = MAX_BUILDINGS;
        for(new i = 0; i < count; ++i)
        {
            cache_get_value_index_int(i, 0, BuildingInfo[i][bID]);
            cache_get_value_index(i, 1, BuildingInfo[i][bName]);
            cache_get_value_index(i, 2, BuildingInfo[i][bOwnerName]);
            cache_get_value_index(i, 3, BuildingInfo[i][bWelcomeText]);
            cache_get_value_index_float(i, 4, BuildingInfo[i][bEnterX]);
            cache_get_value_index_float(i, 5, BuildingInfo[i][bEnterY]);
            cache_get_value_index_float(i, 6, BuildingInfo[i][bEnterZ]);
            cache_get_value_index_int(i, 7, BuildingInfo[i][bEnterInterior]);
            cache_get_value_index_int(i, 8, BuildingInfo[i][bEnterWorld]);
            cache_get_value_index_float(i, 8, BuildingInfo[i][bExitX]);
            cache_get_value_index_float(i, 10, BuildingInfo[i][bExitY]);
            cache_get_value_index_float(i, 11, BuildingInfo[i][bExitZ]);
            cache_get_value_index_int(i, 12, BuildingInfo[i][bExitInterior]);
            cache_get_value_index_int(i, 13, BuildingInfo[i][bOwnable]);
            cache_get_value_index_int(i, 14, BuildingInfo[i][bOwnerID]);
            cache_get_value_index_int(i, 15, BuildingInfo[i][bPrice]);
            cache_get_value_index_int(i, 16, BuildingInfo[i][bLocked]);

            BuildingInfo[i][bExists] = 1;

            Building_CreateElements(i);
            
            Iter_Add(Buildings, i);
        }
    }
    mysql_tquery_inline(gMySQL, "SELECT * FROM `buildings` ORDER BY ID", using inline OnLoad);
}

stock Building_SetPosition(buildingid, Float:x, Float:y, Float:z, vw, int)
{
    if(!BuildingInfo[buildingid][bExists] || !BuildingInfo[buildingid][bID])
        return 0;
    BuildingInfo[buildingid][bEnterX] = x;
    BuildingInfo[buildingid][bEnterY] = y;
    BuildingInfo[buildingid][bEnterZ] = z;
    BuildingInfo[buildingid][bEnterWorld] = vw;
    BuildingInfo[buildingid][bEnterInterior] = int;
    Building_Save(buildingid);

    Building_DestroyElements(buildingid);
    Building_CreateElements(buildingid);
    return 1;
}

stock Building_SetName(buildingid, name[])
{
    if(!BuildingInfo[buildingid][bExists] || !BuildingInfo[buildingid][bID])
        return 0;
    set(BuildingInfo[buildingid][bName], name);
    Building_Save(buildingid);
    if(IsValidDynamic3DTextLabel(BuildingInfo[buildingid][bEnter3DText]))
        DestroyDynamic3DTextLabel(BuildingInfo[buildingid][bEnter3DText]);
    BuildingInfo[buildingid][bEnter3DText] = CreateDynamic3DTextLabel(BuildingInfo[buildingid][bName], COLOR_LIGHTBLUE/*2*/, BuildingInfo[buildingid][bEnterX], BuildingInfo[buildingid][bEnterY], BuildingInfo[buildingid][bEnterZ] + 0.55, 20, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, BuildingInfo[buildingid][bEnterWorld]);
    return 1;
}

stock Building_SetInterior(buildingid, Float:x, Float:y, Float:z, interiorid)
{
    if(!BuildingInfo[buildingid][bExists] || !BuildingInfo[buildingid][bID])
        return 0;
    BuildingInfo[buildingid][bExitInterior] = interiorid;
    BuildingInfo[buildingid][bExitX] = x;
    BuildingInfo[buildingid][bExitY] = y;
    BuildingInfo[buildingid][bExitZ] = z;
    Building_Save(buildingid);

    Pickup_Destroy(BuildingInfo[buildingid][bExitPickupID]);
    BuildingInfo[buildingid][bExitPickupID] = Pickup_Create(1007, buildingid, BuildingInfo[buildingid][bExitX], BuildingInfo[buildingid][bExitY], BuildingInfo[buildingid][bExitZ], ELEMENT_TYPE_BUILDING_EXIT, buildingid, BuildingInfo[buildingid][bExitInterior]);
    return 1;
}

stock Building_SetOwnable(buildingid, ownable)
{
    if(!BuildingInfo[buildingid][bExists] || !BuildingInfo[buildingid][bID])
        return 0;
    BuildingInfo[buildingid][bOwnable] = ownable;
    Building_Save(buildingid);
    return 1;
}

stock Building_SetPrice(buildingid, price)
{
    if(!BuildingInfo[buildingid][bExists] || !BuildingInfo[buildingid][bID])
        return 0;
    BuildingInfo[buildingid][bPrice] = price;
    Building_Save(buildingid);
}

stock Building_SetWelcomeText(buildingid, text[])
{
    set(BuildingInfo[buildingid])
}

stock Building_SetOwner(buildingid, playerid)
{
    if(!Building_IsValid(buildingid) || !Building_IsOwnable(buildingid) || !gCharacterLogged[playerid])
        return 0;
    BuildingInfo[buildingid][bOwnerID] = PlayerInfo[playerid][pID];
    set(BuildingInfo[buildingid][bOwnerName], Character_GetOOCName(playerid));
    Log(Character_GetOOCName(playerid), "", "Building_SetOwner", buildingid);
    return 1;
}

stock Building_ResetOwner(buildingid)
{
    if(!Building_IsValid(buildingid))
        return 0;
    BuildingInfo[buildingid][bOwnerID] = 0;
    set(BuildingInfo[buildingid][bOwnerName], "");
    Log("", "", "Building_ResetOwner", buildingid);
    return 1;
}

stock Building_Delete(buildingid)
{
    // UPDATE OWNERID -> BUILDING_ID TO 0.
    new reset[E_BUILDING_INFO];
    BuildingInfo[buildingid] = reset;
    new query[256];
    mysql_format(gMySQL, query, sizeof(query), "DELETE FROM `buildings` WHERE ID = '%d'",
    BuildingInfo[buildingid][bID]);
    mysql_tquery(gMySQL, query);
}

stock Building_Save(buildingid)
{
    if(!BuildingInfo[buildingid][bExists] || !BuildingInfo[buildingid][bID])
        return 0;

    new query[512];
    mysql_format(gMySQL, query, sizeof(query), "UPDATE `buildings` SET \
    Name = '%e', OwnerName = '%e', \
    WelcomeText = '%e', \
    EnterX = '%f', EnterY = '%f', EnterZ = '%f', \
    EnterInterior = '%d', EnterWorld = '%d', \
    ExitX = '%f', ExitY = '%f', ExitZ = '%f', \
    ExitInterior = '%d', \
    Ownable = '%d', OwnerID = '%d', \
    Price = '%d', Locked = '%d' \
    WHERE = ID = '%d'", 
    BuildingInfo[buildingid][bName], BuildingInfo[buildingid][bOwnerName], 
    BuildingInfo[buildingid][bWelcomeText],
    BuildingInfo[buildingid][bEnterX], BuildingInfo[buildingid][bEnterY], BuildingInfo[buildingid][bEnterZ],
    BuildingInfo[buildingid][bEnterInterior], BuildingInfo[buildingid][bEnterWorld],
    BuildingInfo[buildingid][bExitX], BuildingInfo[buildingid][bExitY], BuildingInfo[buildingid][bExitZ],
    BuildingInfo[buildingid][bExitInterior],
    BuildingInfo[buildingid][bOwnable], BuildingInfo[buildingid][bOwnerID],
    BuildingInfo[buildingid][bPrice], BuildingInfo[buildingid][bLocked],
    BuildingInfo[buildingid][bID]);
    return 1;
}

stock Building_CreateElements(buildingid)
{
    BuildingInfo[buildingid][bEnterPickupID] = Pickup_Create(1239, buildingid, BuildingInfo[buildingid][bEnterX], BuildingInfo[buildingid][bEnterY], BuildingInfo[buildingid][bEnterZ], ELEMENT_TYPE_BUILDING_ENTRANCE, BuildingInfo[buildingid][bEnterWorld], BuildingInfo[buildingid][bEnterInterior]);
    BuildingInfo[buildingid][bEnter3DText] = CreateDynamic3DTextLabel(BuildingInfo[buildingid][bName], COLOR_LIGHTBLUE/*2*/, BuildingInfo[buildingid][bEnterX], BuildingInfo[buildingid][bEnterY], BuildingInfo[buildingid][bEnterZ] + 0.55, 20, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, BuildingInfo[buildingid][bEnterWorld]);
    BuildingInfo[buildingid][bExitPickupID] = Pickup_Create(1007, buildingid, BuildingInfo[buildingid][bExitX], BuildingInfo[buildingid][bExitY], BuildingInfo[buildingid][bExitZ], ELEMENT_TYPE_BUILDING_EXIT, buildingid, BuildingInfo[buildingid][bExitInterior]);
}

stock Building_DestroyElements(buildingid)
{
    Pickup_Destroy(BuildingInfo[buildingid][bEnterPickupID]);
    Pickup_Destroy(BuildingInfo[buildingid][bExitPickupID]);
    DestroyDynamic3DTextLabel(BuildingInfo[buildingid][bEnter3DText]);
}

// GET/SET

stock Building_IsValid(a) return 0 <= a < MAX_BUILDINGS && BuildingInfo[a][bExists] && BuildingInfo[a][bID];

stock Building_GetName(buildingid, name[MAX_BUILDING_NAME])
{
    name = BuildingInfo[buildingid][bName];
    return 1;
}

stock Building_IsOwnable(a)
{
    return BuildingInfo[a][bOwnable];
}

stock Building_IsOwned(a)
{
    return BuildingInfo[a][bOwnable] && BuildingInfo[a][bOwnerID] != 0;
}

stock Building_GetOwnerID(a)
{
    return BuildingInfo[a][bOwnerID];
}

stock Building_GetOwnerName(a, name[])
{
    set(name, BuildingInfo[a][bOwnerName]);
    return 1;
}

stock Building_IsLocked(a)
{
    return BuildingInfo[a][bLocked];
}

stock Building_GetPrice(a)
{
    return BuildingInfo[a][bPrice];
}