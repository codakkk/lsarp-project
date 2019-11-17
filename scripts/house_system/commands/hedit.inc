
#include <YSI_Coding\y_hooks>
flags:hedit(CMD_ADMIN);
CMD:hedit(playerid, params[])
{
    new houseid, command[16], value, value2;
	new String:commandInfo = @("USO: /hedit <houseid> <pos - interior - price - lock - delete - rmowner - rmfurnitures>");
    if(sscanf(params, "ds[16]D(-1)D(-1)", houseid, command, value, value2))
	   return SendClientMessageStr(playerid, COLOR_COMMAND_ERROR, commandInfo);
    
	new House:house = House_Find(houseid);
	if(!House_IsValid(house))
	   return SendClientMessage(playerid, COLOR_ERROR, "L'id della casa inserita non è valido.");

	new hash = YHash(command);

    if(hash == _I<pos>)
    {
	   SendFormattedMessage(playerid, COLOR_GREEN, "Hai spostato le coordinate d'entrata della casa %d.", houseid);
	   new Float:x, Float:y, Float:z;
	   GetPlayerPos(playerid, x, y, z);
	   House_SetEnterPosition(house, x, y, z);
	   House_SetEnterInterior(house, GetPlayerInterior(playerid));
	   House_SetEnterWorld(house, Character_GetVirtualWorld(playerid));
	   House_CreateObjects(house);
    }
    else if(hash == _I<interior>)
    {
	   	if(value == -1 || value2 == -1)
		{
			SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /hedit <houseid> <interior type> <interior id>");
			SendClientMessage(playerid, COLOR_GREY, "Usa /hinttypes per una lista");
			return 0;
		}
	   	if(!HouseInterior_IsValidType(value))
			return SendClientMessage(playerid, COLOR_ERROR, "Tipo interior non valido");
		
	   	if(House_SetInterior(house, value, value2))
		{
	   		SendFormattedMessage(playerid, COLOR_GREEN, "Hai cambiato l'interior della casa %d. Tipo: %s - ID: %d", houseid, GetInteriorTypeName(value), value2);
		}
		else
			SendClientMessage(playerid, COLOR_ERROR, "Non è stato possibile settare l'interior.");
    }
    else if(hash == _I<price>)
    {
	   if(value == -1)
		  return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /hedit <houseid> <price> <price>");
	   if(value <= 0)
		  return SendClientMessage(playerid, COLOR_ERROR, "Il prezzo della casa deve essere maggiore di 0.");
	   SendFormattedMessage(playerid, COLOR_GREEN, "Hai cambiato il prezzo di vendita della casa %d. Nuovo prezzo: %d", houseid, value);
	   House_SetPrice(house, value);
    }
    else if(hash == _I<lock>)
    {
	   if(value != 0 && value != 1)
		  return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /hedit <houseid> <lock> <0: open - 1: lock>");
	   new s[8];
	   if(value)
		  s = "Chiusa";
	   else 
		  s = "Aperta";
	   SendFormattedMessage(playerid, COLOR_GREEN, "Hai cambiato lo stato della porta della casa %d. Stato: %s", houseid, s);
	   House_SetLocked(house, value?true:false);
    }
	else if(hash == _I<delete>)
	{
		if(House_Delete(house))
		{
			SendMessageToAdmins(true, COLOR_ORANGE, "AdmCmd: %s (%d) ha rimosso la casa id %d.", Account_GetName(playerid), playerid, houseid);
		}
		else
		{
			SendClientMessage(playerid, COLOR_ERROR, "La casa non esiste o non puo' essere cancellata.");
		}
	}
	else if(hash == _I<rmowner>)
	{
		if(House_ResetOwner(house))
		{
			SendFormattedMessage(playerid, COLOR_YELLOW, "Hai resettato il proprietario della casa id %d.", houseid);
			SendMessageToAdmins(1, COLOR_ORANGE, "AdmCmd: %s (%d) ha resettato il proprietario della casa id %d.", Account_GetName(playerid), playerid, houseid);
		}
	}
	else if(hash == _I<rmfurnitures>)
	{
		if(House_RemoveAllfurnitures(house))
			SendFormattedMessage(playerid, COLOR_GREEN, "Mobili rimossi dalla casa id %d", House_GetID(house));
		else
			SendFormattedMessage(playerid, COLOR_ERROR, "Non è stato possibile rimuovere i mobili dalla casa id %d", House_GetID(house));
	}
    else SendClientMessageStr(playerid, COLOR_COMMAND_ERROR, commandInfo);
    return 1;
}