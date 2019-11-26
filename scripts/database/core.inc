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
	//mysql_log(ERROR | WARNING);
	
    mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS accounts \
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
						PRIMARY KEY (`id`))");
					
	mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `characters` ( \
						`id` int(11) NOT NULL AUTO_INCREMENT, \
						`account_id` int(11) NOT NULL, \
						`name` varchar(32) NOT NULL, \
						`money` int(11) DEFAULT '0', \
						`health` float DEFAULT '0', \
						`armour` float DEFAULT '0', \
						`skin` int(11) DEFAULT '1', \
						`building_key` int(11) NOT NULL DEFAULT '0', \
						`house_rent` int(11) NOT NULL DEFAULT '0', \
						`house_rent_price` int(11) NOT NULL DEFAULT '0', \
						`faction` int(11) NOT NULL DEFAULT '0', \
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
						CONSTRAINT `Account-Characters` FOREIGN KEY (`account_id`) REFERENCES `accounts` (`id`) ON DELETE CASCADE \
						) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=latin1;");

	mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `character_privileges` (\
						`character_id` INT(11) NOT NULL, \
						`chop_shop` INT(11) NOT NULL DEFAULT 0, \
						PRIMARY KEY (`character_id`), \
						CONSTRAINT `CharacterPrivileges` FOREIGN KEY (`character_id`) REFERENCES `characters` (`id`) ON DELETE CASCADE \
						) ENGINE=InnoDB DEFAULT CHARSET=latin1;");


    mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS character_inventory \
						(`character_id` INT(11) NOT NULL, \
						`slot_id` INT(11) NOT NULL, \
						`item` INT(11) NOT NULL, \
						`amount` INT(11) NOT NULL, \
						`extra` INT(11) NOT NULL, \
						PRIMARY KEY (`character_id`))");

    mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS vehicle_inventory \
						(`vehicle_id` INT(11) NOT NULL, \
						`slot_id` INT(4) NULL, \
						`item` INT(6) NULL, \
						`amount` INT(6) NULL, \
						`extra` INT(6) NULL, \
						PRIMARY KEY (`vehicle_id`))");
    
    mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS houses \
						(`id` INT(11) NOT NULL AUTO_INCREMENT, \
						`owner_id` INT(11) NULL, \
						`enter_x` FLOAT NULL, \
						`enter_y` FLOAT NULL, \
						`enter_z` FLOAT NULL, \
						`enter_interior` INT(11) NULL, \
						`enter_world` INT(11) NULL, \
						`exit_x` FLOAT NULL, \
						`exit_y` FLOAT NULL, \
						`exit_z` FLOAT NULL, \
						`exit_interior` INT(11) NULL, \
						`price` INT(11) NULL, \
						`locked` INT(11) NULL, \
						`money` INT(11) NULL, \
						`interior` INT(11) NULL, \
						`created` INT(11) NULL, \
						PRIMARY KEY (`id`))");
	
	mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `character_drug_info` ( \
						`character_id` int(11) NOT NULL, \
						`drug_id` int(11) NOT NULL, \
						`last_usage_time` int(11) NOT NULL, \
						`usage_counter` int(11) NOT NULL, \
						`next_usage_time` int(11) NOT NULL, \
						PRIMARY KEY (`character_id`), \
						CONSTRAINT `character_drugs` FOREIGN KEY (`character_id`) REFERENCES `characters` (`id`) ON DELETE CASCADE \
						) ENGINE=InnoDB DEFAULT CHARSET=latin1");

	mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `character_custom_animations` ( \
						`id` int(11) NOT NULL AUTO_INCREMENT, \
						`character_id` int(11) NOT NULL, \
						`name` VARCHAR(32) NOT NULL, \
						`library` VARCHAR(32) NOT NULL, \
						`anim_name` VARCHAR(32) NOT NULL, \
						`is_loop` tinyint(4) NOT NULL, \
						CONSTRAINT IDCharacterID PRIMARY KEY (`id`, `character_id`), \
						CONSTRAINT `CharacterCustomAnimations` FOREIGN KEY (`character_id`) REFERENCES `characters` (`id`) ON DELETE CASCADE \
						) ENGINE=InnoDB DEFAULT CHARSET=latin1");

	mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS furnitures \
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
						PRIMARY KEY (`id`))");
						
	mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS loaded_furnitures \
						(`world` INT(11) )");

	mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `bans` ( \
							`ip` varchar(24) NOT NULL DEFAULT '', \
							`serial` varchar(44) NOT NULL DEFAULT '' \
							) ENGINE=MyISAM DEFAULT CHARSET=latin1");

	mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `connections` ( \
						`id` int(11) NOT NULL AUTO_INCREMENT, \
						`account_id` int(11) NOT NULL, \
						`ip` VARCHAR(24) NOT NULL, \
						`gpci` VARCHAR(44) NOT NULL, \
						PRIMARY KEY (`id`), \
						CONSTRAINT `connections_account` FOREIGN KEY (`account_id`) REFERENCES `accounts` (`id`) ON DELETE RESTRICT \
						) ENGINE=InnoDB DEFAULT CHARSET=latin1");

    printf("Database Connected.");
    return 1;
}

hook OnAccountLogin(playerid)
{
	new ip[16], serial[41];
	GetPlayerIp(playerid, ip, sizeof(ip));
	gpci(playerid, serial, sizeof(serial));

	mysql_pquery_f(gMySQL, "INSERT INTO `connections` (account_id, ip, gpci) VALUES('%d', '%e', '%e')", Account_GetID(playerid), ip, serial);
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

stock mysql_tquery_f_callback(MySQL:handle, const callback[] = "", const format[] = "", const query[], GLOBAL_TAG_TYPES:...)
{
	new ret = mysql_format(handle, YSI_UNSAFE_HUGE_STRING, YSI_UNSAFE_HUGE_LENGTH, query, ___(4));
	if(ret)
	{
		ret = mysql_tquery(handle, YSI_UNSAFE_HUGE_STRING, callback, format);
	}
	return ret;
}

stock Cache:mysql_query_f(MySQL:handle, bool:useCache, const query[], GLOBAL_TAG_TYPES:...)
{
	mysql_format(handle, YSI_UNSAFE_HUGE_STRING, YSI_UNSAFE_HUGE_LENGTH, query, ___(3));
	new Cache:cache = mysql_query(handle, YSI_UNSAFE_HUGE_STRING, useCache);
	return cache;
}

stock mysql_pquery_f(MySQL:handle, const query[], GLOBAL_TAG_TYPES:...)
{
	new ret = mysql_format(handle, YSI_UNSAFE_HUGE_STRING, YSI_UNSAFE_HUGE_LENGTH, query, ___(2));
	if(ret)
	{
		ret = mysql_pquery(handle, YSI_UNSAFE_HUGE_STRING);
	}
	return ret;
}

stock mysql_pquery_f_callback(MySQL:handle, const callback[] = "", const format[] = "", const query[], GLOBAL_TAG_TYPES:...)
{
	new ret = mysql_format(handle, YSI_UNSAFE_HUGE_STRING, YSI_UNSAFE_HUGE_LENGTH, query, ___(4));
	if(ret)
	{
		ret = mysql_pquery(handle, YSI_UNSAFE_HUGE_STRING, callback, format);
	}
	return ret;
}
