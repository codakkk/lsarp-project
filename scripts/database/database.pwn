#include <YSI_Coding\y_hooks>
#include <file-import>
new 
    MySQL:gMySQL;

hook OnGameModeInit()
{
    import File("mysql.ini", host[16], user[24], database[12], password[32])
    {
        gMySQL = mysql_connect(host, user, password, database);
        if(mysql_errno(gMySQL) != 0) 
            printf("Non collegato dal database.");
    }
	mysql_log(ERROR | WARNING);
    //gMySQL = mysql_connect("localhost", "root", ""/*"Mrrobot123?"*/, "lsarp");

    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS accounts (`ID` INT(11) NOT NULL AUTO_INCREMENT, PRIMARY KEY (`ID`))");
    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS showrooms \
            (`ID` INT(11) NOT NULL AUTO_INCREMENT, \
            `Name` VARCHAR(24) NULL, \
            `Models` VARCHAR(255) NULL, \
            `Prices` VARCHAR(255) NULL, \
            `X` FLOAT NULL, \
            `Y` FLOAT NULL, \
            `Z` FLOAT NULL, \
            `VehicleX` FLOAT NULL, \
            `VehicleY` FLOAT NULL, \
            `VehicleZ` FLOAT NULL, \
            PRIMARY KEY (`ID`))");
    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS player_vehicles \
            (`ID` INT(11) NOT NULL AUTO_INCREMENT, \
            `OwnerID` INT(11) NOT NULL, \
            `Model` INT(11) NOT NULL, \
            `Color1` INT(11) NOT NULL, \
            `Color2` INT(11) NOT NULL, \
            `X` FLOAT NULL, \
            `Y` FLOAT NULL, \
            `Z` FLOAT NULL, \
            `Angle` FLOAT NULL, \
            `Locked` INT NULL, \
            `LastX` FLOAT NULL, \
            `LastY` FLOAT NULL, \
            `LastZ` FLOAT NULL, \
            `LastA` FLOAT NULL, \
            `LastHealth` FLOAT NULL, \
            `Spawned` INT NULL, \
            `Engine` INT NULL, \
            PRIMARY KEY (`ID`))");
    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS logs \
        (`ID` INT(11) NOT NULL AUTO_INCREMENT, \
        `PlayerID` VARCHAR(255) NOT NULL, \
        `GivePlayerID` VARCHAR(255) NOT NULL, \
        `Text` VARCHAR(255) NOT NULL, \
        `ExtraVar` INT(11) NOT NULL, \
        `Time` INT(11) NOT NULL, \
        PRIMARY KEY (`ID`))");
    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS character_inventory \
        (`CharacterID` INT(11) NOT NULL, \
        `Items` VARCHAR(255) NULL, \
        `ItemsAmount` VARCHAR(255) NULL, \
        `ItemsExtraData` VARCHAR(255) NULL, \
        `EquippedBag` INT(11) NULL,  \
        PRIMARY KEY (`CharacterID`))");
    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS buildings \
        (`ID` INT(11) NOT NULL AUTO_INCREMENT, \
        `Name` VARCHAR(64) NULL, \
        `OwnerName` VARCHAR(24) NULL, \
        `WelcomeText` VARCHAR(120) NULL, \
        `EnterX` FLOAT NULL, \
        `EnterY` FLOAT NULL, \
        `EnterZ` FLOAT NULL, \
        `EnterInterior` INT(11) NULL, \
        `EnterWorld` INT(11) NULL, \
        `ExitX` FLOAT NULL, \
        `ExitY` FLOAT NULL, \
        `ExitZ` FLOAT NULL, \
        `ExitInterior` INT(11) NULL, \
        `Ownable` INT(11) NULL, \
        `OwnerID` INT(11) NULL, \
        `Price` INT(11) NULL, \
        `Locked` INT(11) NULL, \
        PRIMARY KEY (`ID`))");
    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS vehicle_inventory \
        (`VehicleID` INT(11) NOT NULL, \
        `Items` VARCHAR(255) NULL, \
        `ItemsAmount` VARCHAR(255) NULL, \
        `ItemsExtraData` VARCHAR(255) NULL, \
        PRIMARY KEY (`VehicleID`))");
    
    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS houses \
        (`ID` INT(11) NOT NULL AUTO_INCREMENT, \
        `OwnerID` INT(11) NULL, \
        `OwnerName` VARCHAR(24) NULL, \
        `EnterX` FLOAT NULL, \
        `EnterY` FLOAT NULL, \
        `EnterZ` FLOAT NULL, \
        `EnterInterior` INT(11) NULL, \
        `EnterWorld` INT(11) NULL, \
        `ExitX` FLOAT NULL, \
        `ExitY` FLOAT NULL, \
        `ExitZ` FLOAT NULL, \
        `ExitInterior` INT(11) NULL, \
        `Price` INT(11) NULL, \
        `Locked` INT(11) NULL, \
        PRIMARY KEY (`ID`))");
    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS house_inventory \
        (`HouseID` INT(11) NOT NULL, \
        `Items` VARCHAR(255) NULL, \
        `ItemsAmount` VARCHAR(255) NULL, \
        `ItemsExtraData` VARCHAR(255) NULL, \
        PRIMARY KEY (`HouseID`))");
    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS loot_zones \
        (`ID` INT(11) NOT NULL AUTO_INCREMENT, \
        `lzX` FLOAT NULL, \
        `lzY` FLOAT NULL, \
        `lzZ` FLOAT NULL, \
        `lz_interior` INT(11) NULL, \
        `lz_virtual_world` INT(11) NULL, \
        `lz_possible_items` VARCHAR(255) NULL, \
        `lz_possible_items_amount` VARCHAR(255) NULL, \
        `lz_items_rarity` VARCHAR(255) NULL, \
        PRIMARY KEY (`ID`))");
    printf("Database Connected!");
    return 1;
}