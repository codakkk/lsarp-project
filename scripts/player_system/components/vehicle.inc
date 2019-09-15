
#include <vehicles_system\core>
#include <YSI_Coding\y_hooks>

/*static enum e_PlayerVehicle
{
	E_VEHICLE_ID,
	E_VEHICLE_MODEL_ID,
	Float:E_VEHICLE_PARK_X,
	Float:E_VEHICLE_PARK_Y,
	Float:E_VEHICLE_PARK_Z,
	Float:E_VEHICLE_PARK_A,
	E_VEHICLE_SPAWNED,
};
static Map:PlayerVehicles; //<PlayerID, List:e_PlayerVehicle>
*/

static SelectedVehicle[MAX_PLAYERS];

hook OnPlayerEnterCheckpoint(playerid)
{
	DisablePlayerCheckpoint(playerid);
	return 1;
}

flags:vehicle(CMD_ALIVE_USER);
CMD:vehicle(playerid, params[])
{
	if(Character_IsBuyingVehicle(playerid))
		return 0;
	
	new command[16], id, vehicleid = GetPlayerVehicleID(playerid);
	if(sscanf(params, "s[16]D(-1)", command, id))
	{
		SendClientMessage(playerid, 0xFFEC8BFF, "USA: (/v)ehicle [azione]");
		SendClientMessage(playerid, 0xFFEC8BFF, "[Azioni] spawn, parcheggia, vendi, luci, sicura, respawn, motore");
		SendClientMessage(playerid, 0xFFEC8BFF, "[Azioni] lista, bagagliaio, despawn");
		SendClientMessage(playerid, 0xFFEC8BFF, "[Elimina] rottama (attenzione: il tuo veicolo verrà venduto definitivamente)");
		return 0;
	}
	if(!strcmp(command, "sicura", true))
	{
		if(id != -1)
			vehicleid = id;
		if(id <= 0)
			vehicleid = Character_GetClosestVehicle(playerid, 3.5);
		
		if(vehicleid <= 0)
			return 0;
		
		if(Vehicle_IsOwner(vehicleid, playerid, false))
		{

			if( (IsABike(vehicleid) || IsAMotorBike(vehicleid)) && GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
				Character_AMe(playerid, Vehicle_IsLocked(vehicleid) ? ("toglie la catena dalla ruota del suo veicolo.") : ("attacca la catena dalla ruota del suo veicolo."));
			else
				Character_AMe(playerid, Vehicle_IsLocked(vehicleid) ? ("prende le chiavi e apre il suo veicolo.") : ("prende le chiavi e chiude il suo veicolo."));
			
			Vehicle_SetLocked(vehicleid, !Vehicle_IsLocked(vehicleid));
			
			if(Vehicle_IsLocked(vehicleid))
				SendFormattedMessage(playerid, COLOR_GREEN, "Hai chiuso il tuo veicolo (%s).", Vehicle_GetName(vehicleid));
			else
				SendFormattedMessage(playerid, COLOR_GREEN, "Hai aperto il tuo veicolo (%s).", Vehicle_GetName(vehicleid));

			return 1;
		}
	}
	else if(!strcmp(command, "motore", true))
	{
    	if(vehicleid == 0 || GetPlayerState(playerid) != PLAYER_STATE_DRIVER)	
        	return 0;
		
		if(!Vehicle_IsOwner(vehicleid, playerid, false))
			return SendClientMessage(playerid, COLOR_ERROR, "Non hai le chiavi di questo veicolo.");

		if(Vehicle_IsEngineOff(vehicleid))
		{
			if(Vehicle_GetFuel(vehicleid) < 0.5)
				return SendClientMessage(playerid, COLOR_ERROR, "Non c'è benzina nel veicolo.");
			if(Vehicle_GetHealth(vehicleid) <= VEHICLE_MIN_HEALTH_FOR_ENGINE && !Player_IsAdminDuty(playerid))
				return SendClientMessage(playerid, COLOR_ERROR, "Il motore del veicolo è danneggiato.");
			if(Character_IsDoingGas(playerid))
				return SendClientMessage(playerid, COLOR_ERROR, "Non puoi accendere il motore mentre stai rifornendo il veicolo.");
		}

		Vehicle_SetEngineStatus(vehicleid, !Vehicle_GetEngineStatus(vehicleid));
		Vehicle_SetLightState(vehicleid, Vehicle_IsEngineOn(vehicleid) ? Vehicle_GetLightState(vehicleid) : VEHICLE_PARAMS_OFF);
		Character_AMe(playerid, Vehicle_IsEngineOn(vehicleid) ? ("accende il veicolo.") : ("spegne il veicolo."));
	}
	else if(!strcmp(command, "parcheggia", true))
	{
		
		if(vehicleid <= 0 || !Vehicle_IsOwner(vehicleid, playerid, true))
			return 0;
		new 
			Float:x, Float:y, Float:z, Float:a;
		GetVehiclePos(vehicleid, x, y, z);
		GetVehicleZAngle(vehicleid, a);
		Vehicle_Park(vehicleid, x, y, z, a);
		SendFormattedMessage(playerid, COLOR_GREEN, "Hai parcheggiato qui la tua %s.", Vehicle_GetName(vehicleid));
	}
	else if(!strcmp(command, "luci", true))
	{
		if(IsABike(vehicleid) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
			return 0;
		Vehicle_SetLightState(vehicleid, !Vehicle_GetLightState(vehicleid));
	}
	else if(!strcmp(command, "bagagliaio", true))
	{
		if(id <= 0)
			vehicleid = Character_GetClosestVehicle(playerid, 5.0);
		
		if(Vehicle_IsTrunkOpened(vehicleid))
			Vehicle_CloseTrunk(vehicleid);
		else
			Vehicle_OpenTrunk(vehicleid);
	}
	else if(!strcmp(command, "inventario", true) || !strcmp(command, "inv", true))
	{
		if(id <= 0)
			vehicleid = Character_GetClosestVehicle(playerid, 5.0);
		if(!Vehicle_IsOwner(vehicleid, playerid, false))
			return SendClientMessage(playerid, COLOR_ERROR, "Non sei il proprietario di questo veicolo.");
	
		Vehicle_ShowInventory(vehicleid, playerid);
	}
	else if(!strcmp(command, "lista", true) || !strcmp(command, "list", true))
	{
		return Character_ShowVehicleList(playerid, playerid);
	}
	else if(!strcmp(command, "spawn", true))
	{
		if(id <= 0)
			return SendClientMessage(playerid, COLOR_ERROR, "(/v)eicolo spawn <slot>");
		
		if(Character_GetVehicleCount(playerid) < id)
			return SendClientMessage(playerid, COLOR_ERROR, "Non c'è un veicolo un questo slot.");
		
		new spawnedCount = Character_GetSpawnedVehCount(playerid);
		
		if(spawnedCount >= Character_GetSpawnedVehLimit(playerid))
			return SendClientMessage(playerid, COLOR_ERROR, "Hai già superato il limite di veicoli spawnati.");
		
		Character_SpawnVehicle(playerid, id);
	}
	else if(!strcmp(command, "despawn", true))
	{
		if(id > 0 && Character_IsNearVehicle(playerid, id)) vehicleid = id;
		
		if(vehicleid <= 0)
			return SendClientMessage(playerid, COLOR_ERROR, "(/v)eicolo despawn <vehicleid>");
		
		if(!Vehicle_IsOwner(vehicleid, playerid, true))
			return 0;
		
		SendFormattedMessage(playerid, COLOR_LIGHTBLUE, "Hai despawnato il tuo veicolo (%s) [%d].", Vehicle_GetNameFromModel(Vehicle_GetModel(vehicleid)), vehicleid);
		
		Vehicle_Despawn(vehicleid);
	}
	else if(!strcmp(command, "respawn", true))
	{
		if(id > 0 && Character_IsNearVehicle(playerid, id)) vehicleid = id;
		
		if(vehicleid <= 0)
			return SendClientMessage(playerid, COLOR_ERROR, "(/v)eicolo respawn <vehicleid>");
		
		if(Vehicle_GetFaction(vehicleid) != Character_GetFaction(playerid))
			return SendClientMessage(playerid, COLOR_ERROR, "Il veicolo non fa parte della tua fazione.");
		
		new Float:x, Float:y, Float:z;
		Vehicle_GetSpawnPos(vehicleid, x, y, z);

		if(!IsPlayerInRangeOfPoint(playerid, 5.0, x, y, z))
		{
			SetPlayerCheckpoint(playerid, x, y, z, 5.0);
			SendClientMessage(playerid, COLOR_ORANGE, "Dirigiti nel checkpoint per respawnare il veicolo.");
			return 1;
		}

		Vehicle_Respawn(vehicleid);

		SendFormattedMessage(playerid, COLOR_LIGHTBLUE, "Hai respawnato il veicolo (%s) [%d].", Vehicle_GetNameFromModel(Vehicle_GetModel(vehicleid)), vehicleid);
	}
	else if(!strcmp(command, "vendi", true))
	{
		if(id > 0 && Character_IsNearVehicle(playerid, id)) vehicleid = id;
		
		if(vehicleid <= 0)
			return SendClientMessage(playerid, COLOR_ERROR, "(/v)eicolo vendi <vehicleid (opzionale)>");
		
		if(!Vehicle_IsOwner(vehicleid, playerid, true))
			return 0;
		
		SelectedVehicle[playerid] = vehicleid;

		Dialog_Show(playerid, Dialog_SellVehicleToPlayer, DIALOG_STYLE_INPUT, "Vendi a giocatore", "Inserisci il nome o l'id del giocatore a cui vuoi vendere il veicolo, seguito dal prezzo.\n-- Esempi: --\n\n- Pino_Peppe 10000.\n- 1 10000.", "Invia", "Chiudi");
	}
	else if(!strcmp(command, "rottama", true))
	{
		if(id > 0 && Character_IsNearVehicle(playerid, id)) vehicleid = id;
		
		if(vehicleid <= 0)
			return SendClientMessage(playerid, COLOR_ERROR, "(/v)eicolo vendi <vehicleid (opzionale)>");
		
		if(!Vehicle_IsOwner(vehicleid, playerid, true))
			return 0;
		
		SelectedVehicle[playerid] = vehicleid;

		Dialog_Show(playerid, Dialog_SellVehicle, DIALOG_STYLE_MSGBOX, "Vendi veicolo", "Sei sicuro di voler rottamare il tuo veicolo (%s) per $%d?", "Rottama", "Chiudi", Vehicle_GetNameFromModel(Vehicle_GetModel(vehicleid)), Vehicle_GetPrice(vehicleid)/2);
	}
	return 1;
}
alias:vehicle("v");

Dialog:Dialog_SellVehicle(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;
	new vehicleid = SelectedVehicle[playerid];
	
	if(!Vehicle_IsValid(vehicleid))
		return 0;

	if(!Character_IsNearVehicle(playerid, vehicleid))
		return 0;

	if(!Vehicle_IsOwner(vehicleid, playerid, true))
		return 0;
	
	Character_GiveMoney(playerid, Vehicle_GetPrice(vehicleid)/2, "sell_vehicle");
	SendFormattedMessage(playerid, COLOR_GREEN, "Hai venduto il tuo veicolo (%s) per $%d.", Vehicle_GetNameFromModel(Vehicle_GetModel(vehicleid)), Vehicle_GetPrice(vehicleid)/2);
	
	Vehicle_Delete(SelectedVehicle[playerid]);
	
	SelectedVehicle[playerid] = 0;
	return 1;
}

Dialog:Dialog_SellVehicleToPlayer(playerid, response, listitem, inputtext[])
{
	if(!response)
		return 0;

	new vehicleid = SelectedVehicle[playerid];

	if(!Vehicle_IsOwner(vehicleid, playerid, true))
		return 0;

	new id, price;
	if(sscanf(inputtext, "k<u>d", id, price) || price < 0)
		return Dialog_Show(playerid, Dialog_SellVehicleToPlayer, DIALOG_STYLE_INPUT, "Vendi a giocatore", "{FF0000}I dati inseriti non sono validi.{FFFFFF}\nInserisci il nome o l'id del giocatore a cui vuoi vendere il veicolo, seguito dal prezzo.\n-- Esempi: --\n\n- Pino_Peppe 10000.\n- 1 10000.", "Invia", "Chiudi");
	
	if(!Character_IsLogged(id) || !IsPlayerInRangeOfPlayer(playerid, id, 10.0) || id == playerid)
		return Dialog_Show(playerid, Dialog_SellVehicleToPlayer, DIALOG_STYLE_INPUT, "Vendi a giocatore", "{FF0000}Il giocatore non è collegato o è troppo lontano.{FFFFFF}\nInserisci il nome o l'id del giocatore a cui vuoi vendere il veicolo, seguito dal prezzo.\n-- Esempi: --\n\n- Pino_Peppe 10000.\n- 1 10000.", "Invia", "Chiudi");
	
	if(price > Character_GetMoney(id))
		return Dialog_Show(playerid, Dialog_SellVehicleToPlayer, DIALOG_STYLE_INPUT, "Vendi a giocatore", "{FF0000}Il giocatore non possiede tutto questo denaro.{FFFFFF}\nInserisci il nome o l'id del giocatore a cui vuoi vendere il veicolo, seguito dal prezzo.\n-- Esempi: --\n\n- Pino_Peppe 10000.\n- 1 10000.", "Invia", "Chiudi");
	
	if(Request_IsPending(playerid, REQUEST_TYPE_VEHICLE) || Request_IsPending(id, REQUEST_TYPE_VEHICLE))
		return Dialog_Show(playerid, Dialog_SellVehicleToPlayer, DIALOG_STYLE_INPUT, "Vendi a giocatore", "{FF0000}Tu o il giocatore avete già una richiesta veicolo attiva.{FFFFFF}\nInserisci il nome o l'id del giocatore a cui vuoi vendere il veicolo, seguito dal prezzo.\n-- Esempi: --\n\n- Pino_Peppe 10000.\n- 1 10000.", "Invia", "Chiudi");
	
	Character_SetRequest(playerid, id, REQUEST_TYPE_VEHICLE, vehicleid, price);
	
	SendFormattedMessage(id, COLOR_RECEIVE_REQUEST, "%s vuole venderti il suo veicolo (%s) per $%d.", Character_GetRolePlayName(playerid), Vehicle_GetName(vehicleid), price);
    SendClientMessage(id, COLOR_RECEIVE_REQUEST, "Digita \"/accetta veicolo\" per accettare o \"/rifiuta veicolo\" per rifiutare.");
    
    SendFormattedMessage(playerid, COLOR_SEND_REQUEST, "Hai inviato la richiesta di vendita a %s per $%d. Digita \"/annulla veicolo\" per annullare.", Character_GetRolePlayName(id), price);
	return 1;
}

stock Character_AcceptVehicleRequest(playerid)
{
	if(!Request_IsPending(playerid, REQUEST_TYPE_VEHICLE))
		return SendClientMessage(playerid, COLOR_ERROR, "Non hai una richiesta attiva.");
	new 
		senderid = Request_GetSender(playerid, REQUEST_TYPE_VEHICLE),
		receiver = Request_GetReceiver(playerid, REQUEST_TYPE_VEHICLE),
		vehicleid = Request_GetItem(playerid, REQUEST_TYPE_VEHICLE),
		price = Request_GetAmount(playerid, REQUEST_TYPE_VEHICLE)
	;

	if(!IsPlayerInRangeOfPlayer(senderid, receiver, 10.0))
		return SendClientMessage(receiver, COLOR_ERROR, "Non sei vicino al giocatore che ti ha inviato la richiesta.");

	if(Character_GetMoney(receiver) < price)
		return SendClientMessage(receiver, COLOR_ERROR, "Non hai abbastanza soldi.");

	Character_GiveMoney(senderid, price, "vehicle_sell_to_player");
	Character_GiveMoney(-receiver, -price, "vehicle_buy_by_player");

	SendFormattedMessage(senderid, COLOR_ORANGE, "%s ha accettato la tua richiesta ed è ora proprietario del tuo veicolo (%s). Hai guadagnato: $%d.", Character_GetRolePlayName(receiver), Vehicle_GetName(vehicleid), price);
	SendFormattedMessage(receiver, COLOR_ORANGE, "Hai accettato il veicolo (%s) di %s per $%d.", Vehicle_GetName(vehicleid), Character_GetRolePlayName(receiver), price);
	
	Vehicle_SetOwner(vehicleid, receiver);
	
	Request_Reset(senderid, REQUEST_TYPE_VEHICLE);
	Request_Reset(receiver, REQUEST_TYPE_VEHICLE);
	return 1;
}

flags:vehmode(CMD_USER);
CMD:vehmode(playerid, params[])
{
	Player_SetVehModeEnabled(playerid, !Player_HasVehModeEnabled(playerid));
	if(Player_HasVehModeEnabled(playerid))
	{
		SendClientMessage(playerid, COLOR_GREEN, "Hai impostato il sistema veicoli in chat mode.");
	}
	else
	{
		SendClientMessage(playerid, COLOR_GREEN, "Hai impostato il sistema veicoli in dialog mode.");
	}
	Dialog_Close(playerid);
	return 1;
}

stock Character_SpawnVehicle(playerid, slotid)
{
	new query[160];
	mysql_format(gMySQL, query, sizeof(query), "SELECT id, spawned, model, x, y, z FROM `vehicles` WHERE owner_id = '%d'", Character_GetID(playerid));
	mysql_tquery(gMySQL, query, #LoadCharacterVehicle, "dd", playerid, slotid);
	return 1;
}

forward LoadCharacterVehicle(playerid, slotid);
public LoadCharacterVehicle(playerid, slotid)
{
	new rows = cache_num_rows();
	if(rows <= 0 || rows < slotid)
		return SendClientMessage(playerid, COLOR_ERROR, "Non c'è un veicolo in questo slot.");

	slotid--;

	new id, modelid, spawned, Float:x, Float:y, Float:z;

	cache_get_value_name_int(slotid, "id", id);
	cache_get_value_name_int(slotid, "spawned", spawned);
	cache_get_value_name_int(slotid, "model", modelid);
	cache_get_value_name_float(slotid, "x", x);
	cache_get_value_name_float(slotid, "y", y);
	cache_get_value_name_float(slotid, "z", z);

	new bool:canSpawn = true;

	if(spawned)
	{
		foreach(new v : Vehicle)
		{
			if(Vehicle_GetID(v) == id)
			{
				canSpawn = false;
				SendClientMessage(playerid, COLOR_ERROR, "Il veicolo risulta già spawnato.");
				break;
			}
		}
	}
	
	if(!canSpawn)
		return 0;

	printf("SELECT * FROM `vehicles` WHERE id = '%d'", id);
	mysql_tquery_f_callback(gMySQL, #Vehicle_LoadAll, "", "SELECT * FROM `vehicles` WHERE id = '%d'", id);

	SendFormattedMessage(playerid, COLOR_ORANGE, "Hai spawnato il tuo veicolo (%s). Segui il checkpoint per trovarlo.", Vehicle_GetNameFromModel(modelid));

	SetPlayerCheckpoint(playerid, x, y, z, 5.0);

	return 1;
}

stock Character_ShowVehicleList(showToPlayer, playerid)
{
	inline OnLoad()
	{
		new rows = cache_num_rows();
		
		if(rows <= 0)
			return SendClientMessage(showToPlayer, COLOR_ERROR, "Non sono stati trovati veicoli.");

		if(playerid != showToPlayer)
			SendFormattedMessage(showToPlayer, -1, "Veicoli di %s (%d)", Character_GetRolePlayName(playerid), playerid);

		new 
			dbid, modelid, plate[9], vehicleid = 0,
			String:string;
		for(new i = 0; i < rows; ++i)
		{
			vehicleid = 0;

			cache_get_value_name_int(i, "id", dbid);
			cache_get_value_name_int(i, "model", modelid);
			cache_get_value_name(i, "plate", plate);

			foreach(new v : Vehicle)
			{
				if(Vehicle_GetID(v) == dbid)
				{
					vehicleid = v;
					break;
				}
			}

			if(Player_HasVehModeEnabled(playerid))
			{
				if(vehicleid != 0)
					SendFormattedMessage(playerid, COLOR_LIGHTBLUE, "[Slot %d] %s [%s] - Spawnato - ID: %d", i + 1, Vehicle_GetNameFromModel(modelid), plate, vehicleid);
				else
					SendFormattedMessage(playerid, COLOR_LIGHTBLUE, "[Slot %d] %s [%s] - Non Spawnato", i + 1, Vehicle_GetNameFromModel(modelid), plate);
			}
			else
			{
				if(vehicleid == 0)
					string += str_format("[Slot %d] %s [%s] - Non Spawnato\n", i + 1, Vehicle_GetNameFromModel(modelid), plate);
				else
					string += str_format("[Slot %d] %s [%s] - Spawnato - ID: %d\n", i + 1, Vehicle_GetNameFromModel(modelid), plate, vehicleid);
			}
		}

		if(!Player_HasInvModeEnabled(playerid))
			Dialog_Show_s(playerid, DialogNull, DIALOG_STYLE_LIST, @("Lista veicoli"), string, "Seleziona", "Chiudi");
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM `vehicles` WHERE owner_id = '%d'", Character_GetID(playerid));
	return 1;
}

hook OnCharacterLoad(playerid)
{
	return 1;
}

hook OnPlayerClearData(playerid)
{
	PlayerTextDrawHide(playerid, pVehicleFuelText[playerid]);	
	return Y_HOOKS_CONTINUE_RETURN_1;
}

ptask OnVehicleFuelTextDrawTimer[1000](playerid)
{
	if(!Character_IsLogged(playerid))
		return 0;
	new vehicleid = GetPlayerVehicleID(playerid);
	if(vehicleid > 0)
	{
		Internal_UpdateFuelText(playerid, vehicleid);
	}
	
	return 1;
}

hook OnPlayerExitVehicle(playerid, vehicleid)
{
	new seat = GetPlayerVehicleSeat(playerid);
	if(Vehicle_IsLocked(vehicleid)&& !Player_IsAdminDuty(playerid))
	{
		if(Vehicle_GetOwnerID(vehicleid) != Character_GetID(playerid))// && (!IsABike(vehicleid) && !IsAMotorBike(vehicleid)))
		{
			PutPlayerInVehicle(playerid, vehicleid, seat);
			GameTextForPlayer(playerid, "~r~Veicolo chiuso", 1000, 1);
		}
		/*else
		{
			new r[8];
			valstr(r, vehicleid);
			pc_cmd_vapri(playerid, r);
		}*/
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	if(Vehicle_IsLocked(vehicleid) && !Player_IsAdminDuty(playerid))
	{
		/*if(Vehicle_IsOwner(vehicleid, playerid, false))
		{
			new r[8];
			valstr(r, vehicleid);
			pc_cmd_vapri(playerid, r);
		}
		else
		{*/
			new Float:x, Float:y, Float:z;
			GetPlayerPos(playerid, x, y, z);
			SetPlayerPos(playerid, x, y, z);
			GameTextForPlayer(playerid, "~r~Veicolo chiuso", 5000, 1);
			//return 0;
		//}
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerStateChange(playerid, newstate, oldstate)
{
	if(newstate == PLAYER_STATE_DRIVER)
    {
		new vehicleid = GetPlayerVehicleID(playerid);
		if(Vehicle_IsValid(vehicleid) && Vehicle_GetModel(vehicleid) != 0) // Probably a useless check
		{
			if(!IsABike(vehicleid))
			{
				Internal_UpdateFuelText(playerid, vehicleid);
				PlayerTextDrawShow(playerid, pVehicleFuelText[playerid]);
			}
			if(Vehicle_GetOwnerID(vehicleid) > 0)
			{
				SendFormattedMessage(playerid, COLOR_GREEN, "(( Questo veicolo (%s) appartiene a %s ))", Vehicle_GetName(vehicleid), Vehicle_GetOwnerName(vehicleid));
			}
			if(Vehicle_IsEngineBroken(vehicleid))
			{
				new Float:hp;
				GetVehicleHealth(vehicleid, hp);
				if(hp >= VEHICLE_MIN_HEALTH_FOR_ENGINE)
					SendClientMessage(playerid, COLOR_ERROR, "(( Il veicolo è stato distrutto di recente. Il motore è danneggiato ma ancora funzionante. ))");
				
			}
		}
		SetPlayerArmedWeapon(playerid, 0);
	}
	else if(oldstate == PLAYER_STATE_DRIVER && newstate != PLAYER_STATE_DRIVER)
	{
		PlayerTextDrawHide(playerid, pVehicleFuelText[playerid]);
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(!Character_IsAlive(playerid))
		return Y_HOOKS_CONTINUE_RETURN_0;
	if(Player_HasHotKeysEnabled(playerid))
	{
		if(!Character_IsBuyingVehicle(playerid))
		{
			if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
			{
				new vehicleid = GetPlayerVehicleID(playerid);
				if( (PRESSED(KEY_YES) ) )
				{
					pc_cmd_motore(playerid, "");
				}
				else if( (PRESSED(KEY_LOOK_BEHIND)) && Vehicle_IsEngineOn(vehicleid))
				{
					Vehicle_SetLightState(vehicleid, !Vehicle_IsLightOn(vehicleid));
				}
			}
			if(PRESSED(KEY_NO))
			{
				pc_cmd_vehicle(playerid, "sicura");
			}
		}
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

stock Character_GetOwnedVehicleCount(playerid)
{
	new count = 0;
	foreach(new i : Vehicle)
	{
		if(Vehicle_GetOwnerID(i) == Character_GetID(playerid))
			count++;
	}
	return count;
}

stock Character_AddOwnedVehicle(playerid, vehicleid)
{
	if(!Vehicle_IsValid(vehicleid) || Character_GetOwnedVehicleCount(playerid) == MAX_VEHICLES_PER_PLAYER)
		return 0;
	

	// If vehicle is temporary
	Vehicle_SetOwner(vehicleid, playerid);
	
	if(Vehicle_GetID(vehicleid) == 0 || Vehicle_IsTemporary(vehicleid))
	{
		Vehicle_InsertInDatabase(vehicleid);
	}
	
	return 1;
}

stock Character_GetClosestVehicle(playerid, Float:radius)
{
	if(IsPlayerInAnyVehicle(playerid))
		return GetPlayerVehicleID(playerid);

	new 
		Float:x, Float:y, Float:z;
	foreach(new i : Vehicle)
	{
		GetVehiclePos(i, x, y, z);
		if(Character_GetVirtualWorld(playerid) == GetVehicleVirtualWorld(i) && IsPlayerInRangeOfPoint(playerid, radius, x, y, z))
		{
			return i;
		}
	}
    return 0;
}

static Internal_UpdateFuelText(playerid, vehicleid)
{
	new string[18];
	format(string, sizeof(string), "~y~%0.1f%s", Vehicle_GetFuel(vehicleid), "%");
	PlayerTextDrawSetString(playerid, pVehicleFuelText[playerid], string);
}

stock Character_GetVehicleCount(playerid)
{
	new count = 0;
	
	new Cache:cache = mysql_query_f(gMySQL, true, "SELECT COUNT(*) FROM `vehicles` WHERE owner_id = '%d'", Character_GetID(playerid));
	cache_get_value_index_int(0, 0, count);
	cache_delete(cache);
	return count;
}

stock Character_GetSpawnedVehCount(playerid)
{
	new count = 0;
	foreach(new i : Vehicle)
	{
		if(Vehicle_GetOwnerID(i) == Character_GetID(playerid))
			count++;
	}
	return count;
}

stock Character_GetSpawnedVehLimit(playerid)
{
	static const limites[] = {3, 4, 5, 6};
	return limites[Account_GetPremiumLevel(playerid)];
}

stock Character_IsNearVehicle(playerid, vehicleid, Float:dist = 5.0)
{
	if(vehicleid <= 0 || !IsValidVehicle(vehicleid))
		return 0;
	
	new Float:x, Float:y, Float:z;
	GetVehiclePos(vehicleid, x, y, z);
	return IsPlayerInRangeOfPoint(playerid, dist, x, y, z);
}

stock bool:Character_IsVehicleWindowOpen(playerid)
{
	new status[4], seat = GetPlayerVehicleSeat(playerid);
	GetVehicleParamsCarWindows(GetPlayerVehicleID(playerid), status[0], status[1], status[2], status[3]);
	if(0 <= seat <= 3)
		return !status[seat];
	return false;
}