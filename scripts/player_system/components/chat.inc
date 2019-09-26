#include <YSI_Coding\y_hooks>

static 
	PlayerChatStyle[MAX_PLAYERS char],
	Timer:pChatTimer[MAX_PLAYERS];

hook OnPlayerClearData(playerid)
{
	PlayerChatStyle{playerid} = 0;
	stop pChatTimer[playerid];
	return Y_HOOKS_CONTINUE_RETURN_1;
}

flags:stilechat(CMD_USER);
CMD:stilechat(playerid, params[])
{
	new style;
	if(sscanf(params, "d", style))
		return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /stilechat <0 - 8> (0 per disattivare)");
	
	if(style < 0 || style > 8)
		return SendClientMessage(playerid, COLOR_ERROR, "Stile non valido. (0 - 8)");
	
	if(style == 0)
		SendClientMessage(playerid, -1, "Hai disattivato l'animazione della chat.");
	else
		SendFormattedMessage(playerid, -1, "Hai cambiato l'animazione della chat. Stile: {00FF00}%d{FFFFFF}.", style);
	Character_SetChatStyle(playerid, style);
	return 1;
}

hook OnPlayerText(playerid, text[])
{
	if(isnull(text))
		return 0;
	if(Player_IsAdminDuty(playerid))
	{
		pc_cmd_b(playerid, text);
	}
	else
	{
		if(Character_IsDead(playerid))
			return Y_HOOKS_CONTINUE_RETURN_0;
		if(Character_IsAlive(playerid) && !Character_CanInteract(playerid) && !Character_IsFreezed(playerid) && Character_GetChatStyle(playerid) > 0 && strlen(text) > 5)
		{
			switch(Character_GetChatStyle(playerid))
			{
				case 1: ApplyAnimation(playerid, "GANGS", "prtial_gngtlkA", 4.1, 1, 0, 0, 1, 1, 1);
				case 2: ApplyAnimation(playerid, "GANGS", "prtial_gngtlkB", 4.1, 1, 0, 0, 1, 1, 1);
				case 3: ApplyAnimation(playerid, "GANGS", "prtial_gngtlkE", 4.1, 1, 0, 0, 1, 1, 1);
				case 4: ApplyAnimation(playerid, "GANGS", "prtial_gngtlkF", 4.1, 1, 0, 0, 1, 1, 1);
				case 5: ApplyAnimation(playerid, "GANGS", "prtial_gngtlkG", 4.1, 1, 0, 0, 1, 1, 1);
				case 6: ApplyAnimation(playerid, "GANGS", "prtial_gngtlkH", 4.1, 1, 0, 0, 1, 1, 1);
				case 7: ApplyAnimation(playerid, "GANGS", "prtial_gngtlkG", 4.1, 1, 0, 0, 1, 1, 1);
				case 8: ApplyAnimation(playerid, "GANGS", "prtial_gngtlkH", 4.1, 1, 0, 0, 1, 1, 1);
			}
			stop pChatTimer[playerid];
			pChatTimer[playerid] = defer StopChatAnim(playerid, strlen(text));
		}
		if(Character_IsInjured(playerid))
		{
			pc_cmd_low(playerid, text);
		}
		else
		{
			new 
				String:prefix = @(""),
				Float:range = 20.0;
			if(IsPlayerInAnyVehicle(playerid) && !IsABike(GetPlayerVehicleID(playerid)) && !IsAMotorBike(GetPlayerVehicleID(playerid)))
			{
				if(Character_IsVehicleWindowOpen(playerid))
				{
					prefix = @("Finestrino Aperto");
					range = 20.0;
				}
				else
				{
					prefix = @("Finestrino Chiuso");
					range = 5.0;
				}
			}
			
			new String:string;

			if(Character_IsInCall(playerid))
			{
				prefix = @("Telefono");
			}

			
			if(str_len(prefix) > 0)
				string = str_format("[%S] %s dice: %s", prefix, Character_GetRolePlayName(playerid), text);
			else
				string = str_format("%s dice: %s", Character_GetRolePlayName(playerid), text);
			
			ProxDetectorStr(playerid, range, string, COLOR_FADE1, COLOR_FADE2, COLOR_FADE3, COLOR_FADE4, COLOR_FADE5);
			
			CallLocalFunction(#OnCharacterTalk, "ds", playerid, text);
		}
	}
	return Y_HOOKS_CONTINUE_RETURN_0;
}

timer StopChatAnim[150 * len](playerid, len) 
{
	#pragma unused len
	ApplyAnimation(playerid, "CARRY", "crry_prtial", 4.0, 0, 0, 0, 0, 0, 1);
	stop pChatTimer[playerid];
	pChatTimer[playerid] = Timer:0;
}

stock Character_SetChatStyle(playerid, style)
{
	if(!Character_IsLogged(playerid))
		return 0;
	PlayerChatStyle{playerid} = style;
	mysql_tquery_f(gMySQL, "UPDATE `characters` SET chat_style = '%d' WHERE id = '%d'", Character_GetChatStyle(playerid), Character_GetID(playerid));
	return 1;
}

stock Character_GetChatStyle(playerid)
{
	return PlayerChatStyle{playerid};
}