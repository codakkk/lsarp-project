
// Thanks to Dave

/*#include <YSI_Coding\y_hooks>

static
    IsPlayerSpectating[MAX_PLAYERS] = -1;
 
 
 
flags:recon(CMD_JR_MODERATOR);
CMD:recon(playerid, params[])
{
    if(IsPlayerSpectating[playerid] != -1)
        return SendClientMessage(playerid, COLOR_ADMIN, "Stai già reconnando qualcuno, Usa il Tasto sinistro e destro del mouse per cambiare player.");
 
    new otherid;
    if(sscanf(params, "u", otherid)) 
        return SendClientMessage(playerid, COLOR_ADMIN, "/recon <playerid/partofname/######>");
 
    TogglePlayerSpectating(playerid, 1);
    PlayerSpectatePlayer(playerid, otherid);
 
    //SendAdminMessage....
 
    IsPlayerSpectating[playerid] = otherid;
    return 1;    
}
 
hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
   
    if(PRESSED(KEY_FIRE))
    {
        if(IsPlayerSpectating[playerid] != -1)
        {
            if(Character_IsLogged(IsPlayerSpectating[playerid]+1))
            {
                IsPlayerSpectating[playerid]++;
                PlayerSpectatePlayer(playerid, IsPlayerSpectating[playerid]);
                //SendAdminMessage
            }
            return 1;
        }
    }
    if(PRESSED(KEY_HANDBRAKE))
    {
        if(IsPlayerSpectating[playerid] != -1)
        {
            if(Character_IsLogged(IsPlayerSpectating[playerid]-1))
            {
                IsPlayerSpectating[playerid]--;
                PlayerSpectatePlayer(playerid, IsPlayerSpectating[playerid]);
                //SendAdminMessage
            }
            return 1;
        }      
    }
    return 1;
}
 
flags:reconoff(CMD_JR_MODERATOR);
CMD:reconoff(playerid,params[])
{
    if(IsPlayerSpectating[playerid] == -1)
        return SendClientMessage(playerid, COLOR_ADMIN, "Non stai reconnando alcun giocatore, Usa prima /recon <id>");
 
    TogglePlayerSpectating(playerid, 0);
    return 1;
}
 
hook:OnPlayerClearData(playerid)
{
    IsPlayerSpectating[playerid] = -1;
    return 1;
}*/