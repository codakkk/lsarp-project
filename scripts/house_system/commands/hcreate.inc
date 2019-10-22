
flags:hcreate(CMD_JR_MODERATOR);
CMD:hcreate(playerid, params[])
{
    new type;
    if(sscanf(params, "d", type))
	{
		SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /hcreate <tipo interno>");
		SendClientMessage(playerid, COLOR_GREY, "Usa /hinttypes per una lista.");
		return 1;
	}
    
    if(!HouseInterior_IsValidType(type))
	   return SendClientMessage(playerid, COLOR_ERROR, "Tipo interno non valido.");


    new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	
	new id = House_Create(x, y, z, GetPlayerInterior(playerid), Character_GetVirtualWorld(playerid), type);
	
	if(id <= 0)
		return SendClientMessage(playerid, COLOR_ERROR, "Non è stato possibile creare la casa. Contatta Coda.");
	

    SendFormattedMessage(playerid, COLOR_GREEN, "Casa id %d creata. Tipo Interno: %s - ID Interno: %d.", id, GetInteriorTypeName(type), 0);
	SendMessageToAdmins(true, COLOR_ORANGE, "AmdCmd: %s (%d) ha creato la casa id %d. Tipo Interno: %s - ID Interno: %d", Account_GetName(playerid), playerid, id, GetInteriorTypeName(type), 0);
    return 1;
}