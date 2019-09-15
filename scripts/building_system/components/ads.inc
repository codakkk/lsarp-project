
#include <YSI_Coding\y_hooks>

static 
    LastServerAdv = 0,
    bool:NoAds[MAX_PLAYERS char];

hook OnPlayerClearData(playerid)
{
    NoAds{playerid} = false;
    return 1;
}

flags:pubblicita(CMD_ALIVE_USER);
CMD:pubblicita(playerid, params[])
{
    new buildingid = Character_GetNearBuildingID(playerid);
    if(!Building_IsValid(buildingid) || Building_GetType(buildingid) != BUILDING_TYPE_ADS)
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei all'interno di un ufficio pubblicitario.");
    
    new msg[160];
    if(sscanf(params, "s[160]", msg) || strlen(msg) < 10 || strlen(msg) > 160)
        return SendClientMessage(playerid, COLOR_ERROR, "(/pub)blicita <testo (min 10 caratteri - max 160 caratteri)>");
    
    new price = 6 * strlen(msg);

    if(Character_GetMoney(playerid) < price)
        return SendFormattedMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi per pubblicare l'annuncio (Costo: $%d).", price);

    if(GetTickCount() - LastServerAdv < 60000)
        return SendClientMessage(playerid, COLOR_ERROR, "Recentemente è stato pubblicato un annuncio. Attendi un attimo."); 

    foreach(new i : Player)
    {
        if(!Character_IsLogged(i) || NoAds{playerid})
            continue;
        SendFormattedMessage(i, 0x33AA33FF, Account_GetAdminLevel(i) > 1 ? ("[Annuncio] %s (Pubblicato da: %s)") : ("[Annuncio] %s"), msg, Character_GetOOCName(i));
    }

    Character_GiveMoney(playerid, -price, "adv");
    
    Building_GiveMoney(buildingid, price);

    SendFormattedMessage(playerid, COLOR_GREEN, "Annuncio pubblicato per $%d ($6 per carattere).", price);
    
    LastServerAdv = GetTickCount();
    return 1;
}
alias:pubblicita("pub");

flags:noads(CMD_USER);
CMD:noads(playerid, params[])
{
    NoAds{playerid} = !NoAds{playerid};
    if(NoAds{playerid})
        SendClientMessage(playerid, COLOR_GREEN, "Hai disabilitato le pubblicità.");
    else
        SendClientMessage(playerid, COLOR_GREEN, "Hai abilitato le pubblicità.");
    return 1;
}