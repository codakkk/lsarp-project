#include <YSI/y_hooks>
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
    printf("Database Connected!");
    return 1;
}