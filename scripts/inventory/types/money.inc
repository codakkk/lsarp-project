#include <YSI_Coding\y_hooks>

hook OnCharacterUseItem(playerid, slotid, itemid)
{
    if(!ServerItem_IsMoney(itemid))
        return 0;
    new moneyQuant = Character_GetItemAmount(playerid, gItem_Money);
    Dialog_Show(playerid, Dialog_InvUseMoney, DIALOG_STYLE_INPUT, "Inserisci la quantità", "Immetti la quantità di denaro che vuoi utilizzare.\nQuantità: {00FF00}${FFFFFF}%d.", "Usa", "Annulla", moneyQuant);
    return 1;
}

Dialog:Dialog_InvUseMoney(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;
	new 
		val = strval(inputtext),
		amount = Character_GetItemAmount(playerid, gItem_Money)
	;
	if(val <= 0 || val > amount)
		return Dialog_Show(playerid, Dialog_InvUseMoney, DIALOG_STYLE_INPUT, "Inserisci la quantità", "{FF0000}La quantità inserita non è valida.{FFFFFF}\nImmetti la quantità di denaro che vuoi utilizzare.\nQuantità: {00FF00}${FFFFFF}%d.", "Usa", "Annulla", amount);
	Character_GiveMoney(playerid, val, "use_money");
	Character_DecreaseItemAmount(playerid, gItem_Money, val);
	Player_InfoStr(playerid, str_format("~g~$~w~%d", val), true, 2500);
	return 1;
}