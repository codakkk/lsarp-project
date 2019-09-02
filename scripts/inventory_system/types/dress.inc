#include <YSI_Coding\y_hooks>

hook OnCharacterUseItem(playerid, slotid, itemid)
{
    if(itemid != gItem_Dress)
        return 0;
    
    if(!Character_HasItem(playerid, itemid, 1))
        return 0;

    Dialog_Show(playerid, Dialog_SetSkin, DIALOG_STYLE_INPUT, "Seleziona skin", "Inserisci l'id della skin che vuoi indossare", "Indossa", "Chiudi");
    return 1;
}

Dialog:Dialog_SetSkin(playerid, response, listitem, inputtext[])
{
    if(!response)
	{
		return 0;
	}
    new skinid = strval(inputtext);

    if(!IsValidSkin(skinid))   
        return Dialog_Show(playerid, Dialog_SetSkin, DIALOG_STYLE_INPUT, "Seleziona skin", "{FF0000}La skin inserita non è valida.{FFFFFF}\nInserisci l'id della skin che vuoi indossare\nSkin id Valide:\n- da 1 a 311 per le skin base\nda 20001 in poi per le custom.\n\n", "Indossa", "Chiudi");

    Character_SetSkin(playerid, skinid);

    SendFormattedMessage(playerid, -1, "Stai ora indossando la skin id %d.", skinid);

    Character_DecreaseItemAmount(playerid, gItem_Dress, 1);
    return 1;
}

stock IsValidSkin(skinid)
{
    return (1 <= skinid <= 311) || (20001 <= skinid <= 20070);
}