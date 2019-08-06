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
						(`id` INT(11) NOT NULL AUTO_INCREMENT, \
						`name` VARCHAR(24) NOT NULL, \
						`password` TEXT NOT NULL, \
						`admin_level` INT(11) DEFAULT '0', \
						`premium` INT(11) DEFAULT '0', \
						`premium_expiry` INT(11) DEFAULT '0', \
						`characters_counter` INT(11) DEFAULT '0', \
						`email` VARCHAR(255) DEFAULT '', \
						`characters_slot` tinyint(4) DEFAULT 0, \
						`banned` tinyint(4) DEFAULT 0, \
						`ban_expiry` INT(11) DEFAULT 0, \
						PRIMARY KEY (`id`))", false);
	
	mysql_query(gMySQL, "CREATE TABLE `account_options` ( \
						`account_id` int(11) NOT NULL, \
						`hotkeys` int(11) NOT NULL DEFAULT '0', \
						`inv_mode` int(11) NOT NULL DEFAULT '0', \
						PRIMARY KEY (`account_id`), \
						CONSTRAINT `AccountOptions` FOREIGN KEY (`account_id`) REFERENCES `accounts` (`id`) ON DELETE CASCADE \
						) ENGINE=InnoDB DEFAULT CHARSET=latin1;", false);
	mysql_query(gMySQL, "CREATE TABLE `characters` ( \
						`id` int(11) NOT NULL AUTO_INCREMENT, \
						`account_id` int(11) NOT NULL, \
						`name` varchar(32) NOT NULL, \
						`money` int(11) DEFAULT '0', \
						`health` float DEFAULT '0', \
						`armour` float DEFAULT '0', \
						`skin` int(11) DEFAULT '1', \
						`building_key` int(11) NOT NULL DEFAULT '-1', \
						`house_key` int(11) NOT NULL DEFAULT '-1', \
						`faction` int(11) NOT NULL DEFAULT '-1', \
						`faction_rank` int(11) NOT NULL DEFAULT '-1', \
						`jail_time` int(11) NOT NULL DEFAULT '0', \
						`jail_ic` int(11) NOT NULL DEFAULT '0', \
						`paycheck` int(11) NOT NULL DEFAULT '0', \
						`fight_style` int(11) NOT NULL DEFAULT '0', \
						`chat_style` int(11) NOT NULL DEFAULT '0', \
						`walk_style` tinyint(4) NOT NULL DEFAULT '0', \
						`banned` tinyint(4) NOT NULL DEFAULT '0', \
						`ban_expiry` int(11) NOT NULL DEFAULT '0', \
						`last_chop_shop_time` int(11) NOT NULL DEFAULT '0', \
						PRIMARY KEY (`id`), \
						KEY `Account-Characters` (`account_id`), \
						CONSTRAINT `Account-Characters` FOREIGN KEY (`AccountID`) REFERENCES `accounts` (`id`) ON DELETE CASCADE \
						) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=latin1;", false);

	mysql_query(gMySQL, "CREATE TABLE `character_privileges` (\
						`CharacterID` INT(11) NOT NULL, \
						`ChopShop` INT(11) NOT NULL DEFAULT 0, \
						PRIMARY KEY (`CharacterID`), \
						CONSTRAINT `CharacterPrivileges` FOREIGN KEY (`CharacterID`) REFERENCES `characters` (`id`) ON DELETE CASCADE \
						) ENGINE=InnoDB DEFAULT CHARSET=latin1;", false);

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

    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS character_inventory \
						(`CharacterID` INT(11) NOT NULL, \
						`Items` VARCHAR(255) NULL, \
						`ItemsAmount` VARCHAR(255) NULL, \
						`ItemsExtraData` VARCHAR(255) NULL, \
						`EquippedBag` INT(11) NULL,  \
						PRIMARY KEY (`CharacterID`))", false);

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
						CONSTRAINT `CharacterDrugs` FOREIGN KEY (`CharacterID`) REFERENCES `characters` (`id`) ON DELETE CASCADE \
						) ENGINE=InnoDB DEFAULT CHARSET=latin1", false);

	mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS `character_custom_animations` ( \
						`ID` int(11) NOT NULL AUTO_INCREMENT, \
						`CharacterID` int(11) NOT NULL, \
						`Name` VARCHAR(32) NOT NULL, \
						`Library` VARCHAR(32) NOT NULL, \
						`AnimName` VARCHAR(32) NOT NULL, \
						`Loop` tinyint(4) NOT NULL, \
						CONSTRAINT IDCharacterID PRIMARY KEY (`ID`, `CharacterID`), \
						CONSTRAINT `CharacterCustomAnimations` FOREIGN KEY (`CharacterID`) REFERENCES `characters` (`id`) ON DELETE CASCADE \
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
						`created_by_account` INT(11) DEFAULT '0', \
						`created_by_character` INT(11) DEFAULT '0', \
						`last_edit_by_account` INT(11) DEFAULT '0', \
						`last_edit_by_character` INT(11) DEFAULT '0', \
						PRIMARY KEY (`ID`))", false);
						
	mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS loaded_furnitures \
						(`world` INT(11) )", false);

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

stock Cache:mysql_query_f(MySQL:handle, bool:useCache, const query[], GLOBAL_TAG_TYPES:...)
{
	mysql_format(handle, YSI_UNSAFE_HUGE_STRING, YSI_UNSAFE_HUGE_LENGTH, query, ___(3));
	new Cache:cache = mysql_query(handle, YSI_UNSAFE_HUGE_STRING, useCache);
	return cache;
}