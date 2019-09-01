
#include <YSI_Coding\y_hooks>

static Call[MAX_PLAYERS] = {INVALID_PLAYER_ID, ...};

hook OnGameModeInit()
{
    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS `phone_data` ( \
						`id` int(11) NOT NULL AUTO_INCREMENT, \
						`character_id` int(11) NOT NULL DEFAULT '0', \
                        `created_at` int(11) NOT NULL DEFAULT '0', \
                        `number` int(11) NOT NULL DEFAULT '0', \
                        `building` int(11) NOT NULL DEFAULT '0', \
						PRIMARY KEY (`id`), \
						CONSTRAINT `phone_characters` FOREIGN KEY (`character_id`) REFERENCES `characters` (`id`) ON DELETE CASCADE \
						) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=latin1", false);
    return 1;
}

hook OnPlayerClearData(playerid)
{
    if(Character_IsInCall(playerid))
    {
        new other = Character_GetCallPlayer(playerid);
        if(Character_IsLogged(other))
        {
            SendClientMessage(playerid, COLOR_ORANGE, "Il giocatore con cui eri al telefono si è disconnesso.");
            Call[other] = INVALID_PLAYER_ID;
        }
    }
    Call[playerid] = INVALID_PLAYER_ID;
    return 1;
}

hook OnCharacterFirstSpawn(playerid)
{
    new number = Server_GeneratePhoneNumber();
    Character_GiveItem(playerid, gItem_Phone, 1, number);

    mysql_tquery_f(gMySQL, "INSERT INTO (character_id, created_at, number) VALUES('%d', '%d', '%d')", Character_GetID(playerid), gettime(), number);

    SendClientMessage(playerid, COLOR_GREEN, "Ti è stato dato un cellulare. Usa /cellcmds per i comandi o (/inv)entario per gestirlo.");
    return 1;
}

flags:cellcmds(CMD_USER);
CMD:cellcmds(playerid, params[])
{
    SendClientMessage(playerid, -1, "[TELEFONE]: /chiama - /sms");    
    return 1;
}

flags:chiama(CMD_ALIVE_USER);
CMD:chiama(playerid, params[])
{
    if(!Character_HasItem(playerid, gItem_Phone, 1))
        return SendClientMessage(playerid, COLOR_ERROR, "Devi possedere un cellulare per poter utilizzarlo.");
    
    new slot, number[32];
    if(sscanf(params, "ds[32]", slot, number))
        return SendClientMessage(playerid, COLOR_ERROR, "/chiama <slot> <numero>");
    
    
    return 1;
}

stock Server_GeneratePhoneNumber()
{
    new number = 30000000 + random(9999999);
    return number;
}

stock Character_IsInCall(playerid)
{
    return Call[playerid] != INVALID_PLAYER_ID;
}

stock Character_GetCallPlayer(playerid)
{
    return Call[playerid];
}