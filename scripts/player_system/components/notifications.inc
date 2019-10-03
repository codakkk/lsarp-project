
#include <YSI_Coding\y_hooks>

hook OnGameModeInit()
{
    mysql_tquery(gMySQL, "CREATE TABLE IF NOT EXISTS `notifications` ( \
                    `id` int(11) NOT NULL, \
                    `character_id` INT(11) NOT NULL, \
                    `message` VARCHAR(255) NOT NULL, \
                    PRIMARY KEY (`id`) \
                    ) ENGINE=InnoDB DEFAULT CHARSET=latin1");
    return 1;
}

hook OnCharacterLateLoad(playerid)
{
    inline OnLoad()
    {
        new rows = cache_num_rows();
        new msg[256];
        for(new i = 0; i < rows; ++i)
        {
            cache_get_value_name(i, "message", msg);
            strins(msg, "[NOTIFICA] ", 0);
            SendClientMessage(playerid, COLOR_ORANGE, msg);
        }
        mysql_tquery_f(gMySQL, "DELETE FROM `notifications` WHERE character_id = '%d'", Character_GetID(playerid));
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `notifications` WHERE character_id = '%d'", Character_GetID(playerid));
    return 1;
}

stock bool:SendNotification(characterid, const msg[], GLOBAL_TAG_TYPES:...)
{
    new s[256];
    format(s, sizeof(s), msg, ___(2));
    mysql_tquery_f(gMySQL, "INSERT INTO `notifications` (character_id, message) VALUES('%d', '%e')", characterid, s);
    return true;
}