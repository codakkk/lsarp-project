
#include "core"

#include <YSI_Coding\y_hooks>

static 
    TaxiPassenger[MAX_PLAYERS] = {INVALID_PLAYER_ID, ...},
    PlayerTaxi[MAX_PLAYERS] = {INVALID_PLAYER_ID, ...},
    TaxiPrice[MAX_PLAYERS] = {0, ...},
    TaxiPayment[MAX_PLAYERS] = {0, ...},
    Float:StartingPosX[MAX_PLAYERS] = {0.0, ...},
    Float:StartingPosY[MAX_PLAYERS] = {0.0, ...},
    Float:StartingPosZ[MAX_PLAYERS] = {0.0, ...},
    Timer:TaxiTimer[MAX_PLAYERS] = {Timer:0, ...};

hook OnCharacterClearData(playerid)
{
    if(PlayerTaxi[playerid] != INVALID_PLAYER_ID)
        StopTaxi(PlayerTaxi[playerid], playerid);
    
    TaxiPassenger[playerid] = INVALID_PLAYER_ID;
    PlayerTaxi[playerid] = INVALID_PLAYER_ID;
    TaxiPrice[playerid] = TaxiPayment[playerid] = 0;
    StartingPosX[playerid] = StartingPosY[playerid] = StartingPosZ[playerid] = 0.0;

    if(TaxiTimer[playerid] != Timer:0)
        stop TaxiTimer[playerid];
    TaxiTimer[playerid] = Timer:0;
    return 1;
}

hook OnPlayerExitVehicle(playerid, vehicleid)
{
    if(TaxiPassenger[playerid] != INVALID_PLAYER_ID)
        StopTaxi(playerid, TaxiPassenger[playerid]);
    else if(PlayerTaxi[playerid] != INVALID_PLAYER_ID)
        StopTaxi(PlayerTaxi[playerid], playerid);
    return 1;
}

timer EachSecondTaxiUpdate[1000](playerDoingTaxi, playerPassenger)
{
    new vehicleid = GetPlayerVehicleID(playerDoingTaxi);
    if(vehicleid == GetPlayerVehicleID(playerPassenger) && !IsPlayerInRangeOfPoint(playerPassenger, 60.0, StartingPosX[playerPassenger], StartingPosY[playerPassenger], StartingPosZ[playerPassenger]))
    {
        TaxiPayment[playerDoingTaxi] += TaxiPrice[playerPassenger];

        if(Character_GetMoney(playerPassenger) < TaxiPrice[playerPassenger])
        {
            RemovePlayerFromVehicle(playerPassenger);
            
            Character_GiveMoney(playerDoingTaxi, TaxiPayment[playerDoingTaxi], "/tassametro");
            Character_GiveMoney(playerPassenger, -TaxiPayment[playerDoingTaxi], "/tassametro");

            SendFormattedMessage(playerDoingTaxi, -1, "%s non ha abbastanza soldi per continuare la corsa in taxi.", Character_GetRolePlayName(playerPassenger));
            SendClientMessage(playerPassenger, -1, "Non hai abbastanza soldi per continuare la corsa in taxi.");

            TaxiPassenger[playerDoingTaxi] = INVALID_PLAYER_ID;
            PlayerTaxi[playerPassenger] = INVALID_PLAYER_ID;
            TaxiPrice[playerPassenger] = TaxiPayment[playerDoingTaxi] = 0;
            StartingPosX[playerPassenger] = StartingPosY[playerPassenger] = StartingPosZ[playerPassenger] = 0.0;

            stop TaxiTimer[playerDoingTaxi];
        }
    }
}

hook function Character_EnterJobDuty(playerid)
{
    if(Character_GetJobType(playerid) != JOB_TYPE_TAXI)
        return continue(playerid);
    
    new vehicleid = GetPlayerVehicleID(playerid);

    if(vehicleid <= 0 || IsTaxiVehicle(vehicleid) || !Vehicle_IsOwner(vehicleid, playerid, false))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Devi essere su un tuo taxi per poter utilizzare questo comando.");
        return false;
    }
    return true;
}

hook function Character_ExitJobDuty(playerid)
{
    if(Character_GetJobType(playerid) != JOB_TYPE_TAXI)
        return continue(playerid);
    
    return true;
}

hook function ShowJobCommands(playerid)
{
    if(Character_GetJobType(playerid) == JOB_TYPE_TAXI)
    {
        return true;
    }
    return continue(playerid);
}

hook function Job_GetColor(JobType:jobType)
{
    if(jobType == JOB_TYPE_TAXI)
        return COLOR_YELLOW;
    return continue(jobType);
}

hook function Job_RequireDuty(JobType:jobType)
{
    return jobType == JOB_TYPE_TAXI;
}

static bool:IsTaxiVehicle(vehicleid)
{
    return vehicleid > 0 && GetVehicleModel(vehicleid) == 420;
}

CMD:tassametro(playerid, params[])
{
    if(Character_GetJobType(playerid) != JOB_TYPE_TAXI || !Character_IsJobDuty(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei un tassista in servizio.");
    
    new vehicleid = GetPlayerVehicleID(playerid);
    
    if(vehicleid < 0 || !IsTaxiVehicle(vehicleid) || !Vehicle_IsOwner(vehicleid, playerid, false) || GetPlayerVehicleSeat(playerid) != 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Devi essere alla guida di un tuo taxi per poter utilizzare questo comando.");

    if(TaxiPassenger[playerid] != INVALID_PLAYER_ID)
        return StopTaxi(playerid, TaxiPassenger[playerid]);

    new id, price;
    if(sscanf(params, "k<u>d", id, price))
        return SendClientMessage(playerid, COLOR_ERROR, "/tassametro <playerid/partofname/####> <prezzo / KM>");
    
    if(!Character_IsLogged(id) || id == playerid)
        return SendClientMessage(playerid, COLOR_ERROR, "L'ID inserito non è valido o non è collegato.");

    if(GetPlayerVehicleID(id) != vehicleid)
        return SendClientMessage(playerid, COLOR_ERROR, "Questo giocatore non è nel tuo taxi.");

    if(price < 0 || price > 30)
        return SendClientMessage(playerid, COLOR_ERROR, "Prezzo non valido. ($0 - $30).");

    SendFormattedMessage(id, COLOR_RECEIVE_REQUEST, "%s vuole avviare il tassametro con un costo di $%d.", Character_GetRolePlayName(playerid), price);
    SendClientMessage(id, COLOR_RECEIVE_REQUEST, "Digita \"/accetta taxi\" per accettare o \"/rifiuta taxi\" per rifiutare.");
    
    SendFormattedMessage(playerid, COLOR_SEND_REQUEST, "Hai inviato la richiesta del tassametro per $%d a %s.", price, Character_GetRolePlayName(id));
	SendClientMessage(playerid, COLOR_SEND_REQUEST, "Digita \"/annulla taxi\" per annullare.");

    Character_SetRequest(playerid, id, REQUEST_TYPE_TAXI, price);
    return 1;
}

static StopTaxi(driver, passenger)
{
    if(Character_IsLogged(TaxiPassenger[driver]) && Character_IsLogged(PlayerTaxi[passenger]) && TaxiPassenger[driver] == passenger && PlayerTaxi[passenger] == driver)
    {
        Character_GiveMoney(driver, TaxiPayment[driver], "/tassametro");
        Character_GiveMoney(passenger, -TaxiPayment[driver], "/tassametro");
        SendFormattedMessage(driver, COLOR_ORANGE, "Tu o il passeggero siete usciti dal veicolo, per tanto la corsa è terminata. Hai guadagnato $%d.", TaxiPayment[driver]);
        SendFormattedMessage(passenger, COLOR_ORANGE, "Tu o il conducente siete usciti dal veicolo, per tanto la corsa è terminata. Hai speso $%d.", TaxiPayment[driver]);

        TaxiPassenger[driver] = PlayerTaxi[passenger] = INVALID_PLAYER_ID;
        TaxiPrice[passenger] = TaxiPayment[driver] = 0;
        StartingPosX[passenger] = StartingPosY[passenger] = StartingPosZ[passenger] = 0.0;

        if(TaxiTimer[driver] != Timer:0)
            stop TaxiTimer[driver];
        TaxiTimer[driver] = Timer:0;
    }
    return 1;
}

hook OnCharacterAccept(playerid, hash)
{
    if(hash != _I<taxi>)
        return 0;
    
    if(!Request_IsPending(playerid, REQUEST_TYPE_TAXI))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai una richiesta taxi attiva.");
    
    new senderid = Request_GetSender(playerid, REQUEST_TYPE_TAXI);
    
    if(!Character_IsLogged(senderid))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore che ti ha inviato la richiesta non è più online.");

    if(GetPlayerVehicleID(playerid) != GetPlayerVehicleID(senderid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei nello stesso veicolo del giocatore che ti ha inviato la richiesta.");

    if(TaxiPassenger[playerid] != INVALID_PLAYER_ID)
        return SendClientMessage(playerid, COLOR_ERROR, "Questo giocatore è già in una corsa.");

    new price = Request_GetItem(playerid, REQUEST_TYPE_TAXI);

    if(Character_GetMoney(playerid) < price)
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai abbastanza soldi per iniziare la corsa.");
    
    SendFormattedMessage(senderid, COLOR_ACCEPT_REQUEST, "%s ha accettato la tua richiesta per il costo da te scelto. $%d per KM.", Character_GetRolePlayName(playerid), price);
    SendFormattedMessage(playerid, COLOR_ACCEPT_REQUEST, "Hai accettato la corsa nel taxi di %s per $%d per KM. I soldi saranno sottratti quando uscirai dal veicolo.", Character_GetRolePlayName(senderid), price);

    PlayerTaxi[playerid] = senderid;
    TaxiPassenger[senderid] = playerid;

    TaxiPayment[senderid] = 0;
    TaxiPrice[playerid] = price;

    GetPlayerPos(playerid, StartingPosX[playerid], StartingPosY[playerid], StartingPosZ[playerid]);

    defer EachSecondTaxiUpdate(senderid, playerid);

    Request_Reset(playerid, REQUEST_TYPE_TAXI);
    Request_Reset(senderid, REQUEST_TYPE_TAXI);    
    return 1;
}