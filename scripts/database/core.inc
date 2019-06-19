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
		  printf("Impossibile collegarsi al Database");
    }
	mysql_log(ERROR | WARNING);
	
    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS accounts \
						(`ID` INT(11) NOT NULL AUTO_INCREMENT, \
						`Name` VARCHAR(24) NOT NULL, \
						`Password` TEXT NOT NULL, \
						`Admin` INT(11) DEFAULT '0', \
						`Premium` INT(11) DEFAULT '0', \
						`PremiumExpiry` INT(11) DEFAULT '0', \
						`CharactersCounter` INT(11) DEFAULT '0', \
						`EMail` VARCHAR(255) DEFAULT '', \
						`ZPoints` tinyint(4) DEFAULT '', \
						`CharactersSlot` tinyint(4) DEFAULT 0, \
						`Banned` tinyint(4) DEFAULT 0, \
						`BanExpiry` INT(11) DEFAULT 0, \
						PRIMARY KEY (`ID`))", false);
	
	mysql_query(gMySQL, "CREATE TABLE `account_options` ( \
						`AccountID` int(11) NOT NULL, \
						`HotKeys` int(11) NOT NULL DEFAULT '0', \
						`InvMode` int(11) NOT NULL DEFAULT '0', \
						PRIMARY KEY (`AccountID`), \
						CONSTRAINT `AccountOptions` FOREIGN KEY (`AccountID`) REFERENCES `accounts` (`ID`) ON DELETE CASCADE \
						) ENGINE=InnoDB DEFAULT CHARSET=latin1;", false);
	mysql_query(gMySQL, "CREATE TABLE `characters` ( \
						`ID` int(11) NOT NULL AUTO_INCREMENT, \
						`AccountID` int(11) NOT NULL, \
						`Name` varchar(32) NOT NULL, \
						`Money` int(11) DEFAULT '0', \
						`Level` int(11) NOT NULL DEFAULT '1', \
						`Age` int(11) DEFAULT '0', \
						`Sex` int(11) DEFAULT '0', \
						`LastX` float DEFAULT '0', \
						`LastY` float DEFAULT '0', \
						`LastZ` float DEFAULT '0', \
						`LastInterior` int(11) DEFAULT '0', \
						`LastVirtualWorld` int(11) DEFAULT '0', \
						`FirstSpawn` int(11) DEFAULT '1', \
						`Health` float DEFAULT '0', \
						`Armour` float DEFAULT '0', \
						`Skin` int(11) DEFAULT '1', \
						`LastAngle` float DEFAULT '0', \
						`Spawned` int(11) NOT NULL DEFAULT '0', \
						`PayDay` int(11) NOT NULL DEFAULT '0', \
						`Exp` int(11) NOT NULL DEFAULT '0', \
						`BuildingKey` int(11) NOT NULL DEFAULT '-1', \
						`HouseKey` int(11) NOT NULL DEFAULT '-1', \
						`Faction` int(11) NOT NULL DEFAULT '-1', \
						`FactionRank` int(11) NOT NULL DEFAULT '-1', \
						`JailTime` int(11) NOT NULL DEFAULT '0', \
						`JailIC` int(11) NOT NULL DEFAULT '0', \
						`PayCheck` int(11) NOT NULL DEFAULT '0', \
						`FightStyle` int(11) NOT NULL DEFAULT '0', \
						`ChatStyle` int(11) NOT NULL DEFAULT '0', \
						`WalkStyle` tinyint(4) NOT NULL DEFAULT '0', \
						`Banned` tinyint(4) NOT NULL DEFAULT '0', \
						`BanExpiry` int(11) NOT NULL DEFAULT '0', \
						`LastChopShopTime` int(11) NOT NULL DEFAULT '0', \
						PRIMARY KEY (`ID`), \
						KEY `Account-Characters` (`AccountID`), \
						CONSTRAINT `Account-Characters` FOREIGN KEY (`AccountID`) REFERENCES `accounts` (`ID`) ON DELETE CASCADE \
						) ENGINE=InnoDB AUTO_INCREMENT=22 DEFAULT CHARSET=latin1;", false);

    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS dealerships \
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
						PRIMARY KEY (`ID`))", false);

    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS vehicles \
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
						PRIMARY KEY (`ID`))", false);

    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS logs \
						(`ID` INT(11) NOT NULL AUTO_INCREMENT, \
						`PlayerID` VARCHAR(255) NOT NULL, \
						`GivePlayerID` VARCHAR(255) NOT NULL, \
						`Text` VARCHAR(255) NOT NULL, \
						`ExtraVar` INT(11) NOT NULL, \
						`Time` INT(11) NOT NULL, \
						PRIMARY KEY (`ID`))", false);

    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS character_inventory \
						(`CharacterID` INT(11) NOT NULL, \
						`Items` VARCHAR(255) NULL, \
						`ItemsAmount` VARCHAR(255) NULL, \
						`ItemsExtraData` VARCHAR(255) NULL, \
						`EquippedBag` INT(11) NULL,  \
						PRIMARY KEY (`CharacterID`))", false);

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
						PRIMARY KEY (`ID`))", false);

    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS vehicle_inventory \
						(`VehicleID` INT(11) NOT NULL, \
						`Items` VARCHAR(255) NULL, \
						`ItemsAmount` VARCHAR(255) NULL, \
						`ItemsExtraData` VARCHAR(255) NULL, \
						PRIMARY KEY (`VehicleID`))", false);
    
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
						PRIMARY KEY (`ID`))", false);

    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS house_inventory \
						(`HouseID` INT(11) NOT NULL, \
						`Items` VARCHAR(255) NULL, \
						`ItemsAmount` VARCHAR(255) NULL, \
						`ItemsExtraData` VARCHAR(255) NULL, \
						PRIMARY KEY (`HouseID`))", false);

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
						PRIMARY KEY (`ID`))", false);

	mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS factions \
						(`ID` INT(11) NOT NULL AUTO_INCREMENT, \
						`Name` VARCHAR(32) NULL, \
						`ShortName` VARCHAR(8) NULL, \
						`Type` INT(11) NULL, \
						`SpawnX` FLOAT NULL, \
						`SpawnY` FLOAT NULL, \
						`SpawnZ` FLOAT NULL, \
						`SpawnInterior` INT(11) NULL, \
						`SpawnWorld` INT(11) NULL, \
						PRIMARY KEY (`ID`))", false);

	mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS faction_ranks \
						(`ID` INT(11) NOT NULL AUTO_INCREMENT, \
						`FactionID` INT(11) NOT NULL, \
						`Rank` INT(11) DEFAULT '0', \
						`Name` VARCHAR(16) NOT NULL, \
						`Salary` INT(11) DEFAULT '0', \
						PRIMARY KEY (`ID`))", false);

	mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS faction_skins \
						(`ID` INT(11) NOT NULL AUTO_INCREMENT, \
						`FactionID` INT(11) NOT NULL, \
						`SkinID` INT(11) NOT NULL DEFAULT '0', \
						`SlotID` INT(11) NOT NULL, \
						PRIMARY KEY (`ID`))", false);
	
	mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS character_drug_info \
						(`CharacterID` INT(11) NOT NULL, \
						`SkinID` INT(11) NOT NULL DEFAULT '0', \
						`SlotID` INT(11) NOT NULL, \
						PRIMARY KEY (`ID`))", false);
	
	mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS `character_drug_info` ( \
						`CharacterID` int(11) NOT NULL, \
						`LastUsedDrug` smallint(6) NOT NULL DEFAULT '0', \
						`LastUsedDrugTime` int(11) NOT NULL DEFAULT '0', \
						`LastUsedDrugCount` tinyint(4) NOT NULL DEFAULT '0', \
						PRIMARY KEY (`CharacterID`), \
						CONSTRAINT `CharacterDrugs` FOREIGN KEY (`CharacterID`) REFERENCES `characters` (`ID`) ON DELETE CASCADE \
						) ENGINE=InnoDB DEFAULT CHARSET=latin1", false);

	mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS `character_custom_animations` ( \
						`ID` int(11) NOT NULL AUTO_INCREMENT, \
						`CharacterID` int(11) NOT NULL, \
						`Name` VARCHAR(32) NOT NULL, \
						`Library` VARCHAR(32) NOT NULL, \
						`AnimName` VARCHAR(32) NOT NULL, \
						`Loop` tinyint(4) NOT NULL, \
						CONSTRAINT IDCharacterID PRIMARY KEY (`ID`, `CharacterID`), \
						CONSTRAINT `CharacterCustomAnimations` FOREIGN KEY (`CharacterID`) REFERENCES `characters` (`ID`) ON DELETE CASCADE \
						) ENGINE=InnoDB DEFAULT CHARSET=latin1", false);

	mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS furnitures \
						(`id` INT(11) NOT NULL AUTO_INCREMENT,\
						`object_id` INT(11) DEFAULT '0', \
						`world` INT(11) DEFAULT '0', \
						`interior` INT(11) DEFAULT '0', \
						`model` INT(11) DEFAULT '0', \
						`price` INT(11) DEFAULT '0', \
						`pos_x` FLOAT DEFAULT 0.0, \
						`pos_y` FLOAT DEFAULT 0.0, \
						`pos_z` FLOAT DEFAULT 0.0, \
						`rot_x` FLOAT DEFAULT 0.0, \
						`rot_y` FLOAT DEFAULT 0.0, \
						`rot_z` FLOAT DEFAULT 0.0, \
						`name` VARCHAR(64) DEFAULT '', \
						PRIMARY KEY (`ID`))", false);
    printf("Database Connected.");
    return 1;
}

public OnQueryError(errorid, const error[], const callback[], const query[], MySQL:handle)
{
	printf("ERROR: %s", error);
	printf("QUERY ERROR: %s", query);
	return 1;
}

stock mysql_tquery_f(MySQL:handle, const query[], GLOBAL_TAG_TYPES:...)
{
	new ret = mysql_format(handle, YSI_UNSAFE_HUGE_STRING, YSI_UNSAFE_HUGE_LENGTH, query, ___(2));
	if(ret)
	{
		ret = mysql_tquery(handle, YSI_UNSAFE_HUGE_STRING);
	}
	return ret;
}
