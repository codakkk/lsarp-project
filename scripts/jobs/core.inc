
#if defined _lvrp_jobs_included
	#endinput
#endif
#define _lvrp_jobs_included

forward OnCharacterLeaveJob(playerid);
forward OnCharacterTakeJob(playerid, JobType:jobType);
forward OnCharacterJobDutyOff(playerid);
forward OnCharacterJobDutyOn(playerid);

enum JobType
{
    JOB_TYPE_NONE = 0,
    JOB_TYPE_TAXI = 1,
    JOB_TYPE_MECHANIC = 2,
};

#include "taxi"
#include <jobs\mechanic>
#include <YSI_Coding\y_hooks>

static enum E_JOB_DATA
{
    Name[32],
    Float:X,
    Float:Y,
    Float:Z,
    JobType:Type
};
static JobInfo[][E_JOB_DATA] =
{
    {"Nessuno", 0.0, 0.0, 0.0, JOB_TYPE_NONE},
    {"Tassista", 1909.5219,2350.9900,10.9799, JOB_TYPE_TAXI},
    {"Meccanico", 1670.1516,2209.6016,10.8203, JOB_TYPE_MECHANIC}
};

static 
    JobType:PlayerJob[MAX_PLAYERS char] = {JOB_TYPE_NONE, ...},
    bool:JobDuty[MAX_PLAYERS char] = {false, ...};

static JobType:VehicleJobType[MAX_VEHICLES char] = {JOB_TYPE_NONE, ...};
static 
    JobType:RequestedService[MAX_PLAYERS char] = {JOB_TYPE_NONE, ...},
    FollowingPlayer[MAX_PLAYERS] = {INVALID_PLAYER_ID, ...},
    FollowingPlayerCP[MAX_PLAYERS] = {0, ...};

hook OnScriptInit()
{
    for(new i = 0, j = sizeof(JobInfo); i < j; ++i)
    {
        if(JobInfo[i][Type] == JOB_TYPE_NONE)
            continue;
        Pickup_Create(1239, i, JobInfo[i][X], JobInfo[i][Y], JobInfo[i][Z], ELEMENT_TYPE_JOB, 0, 0);
        CreateDynamic3DTextLabel(JobInfo[i][Name], -1, JobInfo[i][X], JobInfo[i][Y], JobInfo[i][Z] + 0.4, 30.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 1, 0, 0, -1);
    }
    return 1;
}

hook OnPlayerPickUpElmPickup(playerid, pickupid, elementid, E_ELEMENT_TYPE:type)
{
    if(type != ELEMENT_TYPE_JOB)
        return 1;
    Player_InfoStr(playerid, str_format("-- ~y~%s~w~ --~n~Digita ~y~~h~/prendilavoro~w~ per prendere il lavoro", JobInfo[elementid][Name]), true, 2500);
    return 1;
}

hook OnCharacterDisconnected(playerid, reason)
{
    if(Character_IsLogged(FollowingPlayer[playerid]))
    {
        SendClientMessage(FollowingPlayer[playerid], COLOR_ORANGE, "[!] Il giocatore che doveva fornirti il servizio si è disconnesso.");
    }
    foreach(new i : Player)
    {
        if(FollowingPlayer[i] == playerid)
        {
            SendClientMessage(i, COLOR_ORANGE, "[!] Il giocatore a cui dovevi fornire il servizio non è più disponibile.");
            break;
        }
    }
    return 1;
}

hook OnCharacterClearData(playerid)
{
    PlayerJob{playerid} = JOB_TYPE_NONE;
    RequestedService{playerid} = JOB_TYPE_NONE;
    FollowingPlayer[playerid] = INVALID_PLAYER_ID;
    JobDuty{playerid} = false;
    if(IsValidDynamicCP(FollowingPlayerCP[playerid]))
        DestroyDynamicCP(FollowingPlayerCP[playerid]);
    FollowingPlayerCP[playerid] = 0;
    return 1;
}

hook OnCharacterLoad(playerid)
{
    inline OnLoad()
    {
        new i = 0;
        cache_get_value_name_int(0, "job_type", i);
        PlayerJob{playerid} = JobType:i;
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT job_type FROM `characters` WHERE id = '%d'", Character_GetID(playerid));
    return 1;
}

hook OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	if(ispassenger || Player_IsAdminDuty(playerid))
		return 1;
	if(vehicleid > 0)
	{
		new JobType:vehicleJob = Vehicle_GetJob(vehicleid);
		if(vehicleJob != JOB_TYPE_NONE && Character_GetJobType(playerid) != vehicleJob)
		{
			ClearAnimations(playerid, 1);
			SendClientMessage(playerid, COLOR_ERROR, "Questo veicolo appartiene ad un lavoro che non hai.");
		}
	}
	return 1;
}

hook OnPlayerStateChange(playerid, newstate, oldstate)
{
    if(Player_IsAdminDuty(playerid))
        return 1;
	if(oldstate != PLAYER_STATE_DRIVER && newstate == PLAYER_STATE_DRIVER)
	{
		new JobType:vehicleJob = Vehicle_GetJob(GetPlayerVehicleID(playerid));
		if(vehicleJob != JOB_TYPE_NONE && Character_GetJobType(playerid) != vehicleJob)
		{
			RemovePlayerFromVehicle(playerid);
			SendClientMessage(playerid, COLOR_ERROR, "Questo veicolo appartiene ad un lavoro che non hai.");
		}
	}
	return 1;
}
#include <YSI_Coding\y_hooks>
 

hook OnPlayerEnterDynamicCP(playerid, checkpointid)
{
    if(FollowingPlayerCP[playerid] == checkpointid)
    {
        DestroyDynamicCP(FollowingPlayerCP[playerid]);
        SendClientMessage(playerid, COLOR_ORANGE, "[!] Hai raggiunto il giocatore.");
    }
    return 1;
}

CMD:jobcmds(playerid, params[])
{
    ShowJobCommands(playerid);    
    return 1;
}
alias:jobcmds("lavorocmds");

CMD:jobduty(playerid, params[])
{
    new JobType:jobType = Character_GetJobType(playerid);
    if(jobType == JOB_TYPE_NONE)
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un lavoro.");
    
    if(Character_IsFactionDuty(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi entrare in servizio lavoro se sei in servizio fazione.");

    new jobColor = Job_GetColor(jobType);
    if(Character_IsJobDuty(playerid))
    {
        if(Character_ExitJobDuty(playerid))
        {
            SendClientMessage(playerid, jobColor, "Sei uscito dal servizio.");
            SetPlayerColor(playerid, COLOR_WHITE);
            Character_SetJobDuty(playerid, false);

            CallLocalFunction(#OnCharacterJobDutyOff, "d", playerid);
        }
    }
    else
    {
        if(EnterJobDuty(playerid))
        {
            SendClientMessage(playerid, jobColor, "[Lavoro] Sei entrato in servizio.");
            SetPlayerColor(playerid, jobColor);
            Character_SetJobDuty(playerid, true);

            CallLocalFunction(#OnCharacterJobDutyOn, "d", playerid);
        }
    }
    return 1;
}
alias:jobduty("serviziolavoro");

CMD:takejob(playerid, params[])
{
    if(Character_GetJobType(playerid) != JOB_TYPE_NONE)
        return SendClientMessage(playerid, COLOR_ERROR, "Hai già un lavoro, prima di poterne prendere un altro, usa /lascialavoro.");
    
    new job, E_ELEMENT_TYPE:type;
    if(!Character_GetNearestPickup(playerid, job, type) || type != ELEMENT_TYPE_JOB)
        return SendClientMessage(playerid, COLOR_ERROR, "Non sei su nessun posto di lavoro.");
    
    Character_SetJobType(playerid, JobInfo[job][Type]);

    SendFormattedMessage(playerid, -1, "Ora sei un %s. Digita /jobcmds o /lavorocmds per i comandi.", JobInfo[job][Name]);

    CallLocalFunction(#OnCharacterTakeJob, "dd", playerid, _:JobInfo[job][Type]);
    return 1;
}
alias:takejob("prendilavoro");

CMD:leavejob(playerid, params[])
{
    if(Character_GetJobType(playerid) == JOB_TYPE_NONE)
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se non hai un lavoro.");

    if(Character_IsJobDuty(playerid))
        Character_ExitJobDuty(playerid);

    if(Character_IsLogged(FollowingPlayer[playerid]))
    {
        SendClientMessage(FollowingPlayer[playerid], COLOR_ORANGE, "[!] Il giocatore che doveva fornirti il servizio non è più disponibile.");
        FollowingPlayer[playerid] = INVALID_PLAYER_ID;
    }

    Character_SetJobType(playerid, JOB_TYPE_NONE);

    SendClientMessage(playerid, COLOR_ORANGE, "Hai lasciato il lavoro.");

    CallLocalFunction(#OnCharacterLeaveJob, "d", playerid);
    return 1;
}
alias:leavejob("lascialavoro");

CMD:centralino(playerid, params[])
{
    new JobType:jobType = Character_GetJobType(playerid);
    if(jobType == JOB_TYPE_NONE || !Job_AllowContactCenter(jobType))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai un lavoro con centralino.");
    
    new id;
    if(sscanf(params, "k<u>", id))
        return SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /centralino <playerid/partofname>");
    
    if(!Character_IsLogged(id))
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non è collegato.");

    if(Character_GetRequestedService(id) != jobType)
        return SendClientMessage(playerid, COLOR_ERROR, "Il giocatore non ha richiesto questo servizio.");

    if(Character_IsLogged(FollowingPlayer[playerid]))
        return SendClientMessage(playerid, COLOR_ERROR, "Hai già una richiesta di servizio attiva.");

    if(IsValidDynamicCP(FollowingPlayerCP[playerid]))
        DestroyDynamicCP(FollowingPlayerCP[playerid]);

    SendClientMessage(id, COLOR_ORANGE, "[!] Un operatore ha accettato la tua richiesta.");
    SendClientMessage(playerid, COLOR_ORANGE, "[!] Hai accettato la richiesta del servizio dell'utente. Usa /annullacentralino per annullarla.");

    new Float:x, Float:y, Float:z;
    Entity_GetWorldPosition(ENTITY_TYPE_PLAYER, id, x, y, z);
    FollowingPlayerCP[playerid] = CreateDynamicCP(x, y, z, 2.5, 0, 0, playerid, 99999.0);

    Character_SetRequestedService(id, JOB_TYPE_NONE);
    return 1;
}

CMD:annullacentralino(playerid, params[])
{
    if(!Character_IsLogged(FollowingPlayer[playerid]))
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai una richiesta di servizio attiva.");
    
    SendClientMessage(FollowingPlayer[playerid], COLOR_ORANGE, "[!] L'operatore ha annullato il servizio.");
    SendClientMessage(playerid, COLOR_ORANGE, "Hai annullato la richiesta dell'utente.");

    FollowingPlayer[playerid] = INVALID_PLAYER_ID;
    if(IsValidDynamicCP(FollowingPlayerCP[playerid]))
        DestroyDynamicCP(FollowingPlayerCP[playerid]);
    FollowingPlayerCP[playerid] = 0;
    return 1;
}

stock JobType:Character_GetJobType(playerid)
{
    return PlayerJob{playerid};
}

stock Character_SetJobType(playerid, JobType:jobType)
{
    PlayerJob{playerid} = jobType;
    mysql_pquery_f(gMySQL, "UPDATE `characters` SET job_type = '%d' WHERE id = '%d'", _:jobType, Character_GetID(playerid));
    return 1;
}

stock Character_SetJobDuty(playerid, bool:isDuty)
{
    JobDuty{playerid} = isDuty;
    return 1;
}

stock bool:Character_IsJobDuty(playerid)
{
    return JobDuty{playerid};
}

stock Job_GetColor(JobType:jobType)
{
    #pragma unused jobType
    return -1;
}

stock Job_RequireDuty(JobType:jobType)
{
    #pragma unused jobType
    return false;
}

stock bool:Job_GetName(JobType:jobType, name[])
{
    for(new i = 0, j = sizeof(JobInfo); i < j; ++i)
    {
        if(JobInfo[i][Type] == jobType)
        {
            strcat(name, JobInfo[i][Name], 32);
            break;
        }       
    }
    return true;
}

stock ShowJobCommands(playerid)
{
    if(Character_GetJobType(playerid) == JOB_TYPE_NONE)
        return SendClientMessage(playerid, COLOR_ERROR, "Devi possedere un lavoro per poter utilizzare questo comando."), 0;
    if(Job_AllowContactCenter(Character_GetJobType(playerid)))
        SendClientMessage(playerid, COLOR_ORANGE, "[CENTRALINO]: /centralino /annullacentralino");
    return 1;
}

stock EnterJobDuty(playerid)
{
    if(Character_GetJobType(playerid) == JOB_TYPE_TAXI)
    {
        new vehicleid = GetPlayerVehicleID(playerid);

        if(!Character_IsValidJobVehicle(playerid, vehicleid))
        {
            SendClientMessage(playerid, COLOR_ERROR, "Devi essere su un tuo taxi o uno convenzionato per poter utilizzare questo comando.");
            return false;
        }
        return true;
    }
    else if(Character_GetJobType(playerid) == JOB_TYPE_MECHANIC)
    {
        return true;

    }
    return false;
}

stock bool:Character_ExitJobDuty(playerid)
{
    #pragma unused playerid
    return false;
}

hook OnVehicleLoaded(vehicleid)
{
    await mysql_aquery_s(gMySQL, str_format("SELECT job FROM `vehicles` WHERE id = '%d'", Vehicle_GetID(vehicleid)));
    if(cache_num_rows() > 0)
    {
        new jobtype;
        cache_get_value_name_int(0, "job", jobtype);
        VehicleJobType{vehicleid} = JobType:jobtype;
    }
    return 1;
}

flags:setvehiclejob(CMD_MODERATOR);
CMD:setvehiclejob(playerid, params[])
{
    new vehicleid, jobtype;
    if(sscanf(params, "dd", vehicleid, jobtype))
    {
        SendClientMessage(playerid, COLOR_COMMAND_ERROR, "USO: /setvehiclejob <vehicleid> <jobtype>");
        SendClientMessage(playerid, COLOR_COMMAND_ERROR, "Tipi: 0: nessuno, 1: tassista, 2: meccanico.");
        return 1;
    }

    if(!Vehicle_IsValid(vehicleid))
        return SendClientMessage(playerid, COLOR_ERROR, "Il veicolo inserito non esiste.");
    
    if(Vehicle_IsTemporary(vehicleid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su un veicolo temporaneo.");

    if(Vehicle_GetOwnerID(vehicleid) != 0)
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando su un veicolo non proprietario.");

    if(!(_:JOB_TYPE_NONE <= jobtype <= _:JobType))
        return SendClientMessage(playerid, COLOR_ERROR, "Jobtype invalido.");

    new name[32];
    Job_GetName(JobType:jobtype, name);
    Vehicle_SetJob(vehicleid, JobType:jobtype);
    SendMessageToAdmins(1, COLOR_ADMIN, "AdmCmd: %s ha settato il lavoro \"%s\" al veicolo id %d.", Account_GetName(playerid), name, vehicleid);
    return 1;
}

stock bool:Vehicle_SetJob(vehicleid, JobType:job)
{
    if(!Vehicle_IsValid(vehicleid))
        return false;
    VehicleJobType{vehicleid} = job;
    if(Vehicle_GetID(vehicleid) > 0)
    {
        mysql_pquery_f(gMySQL, "UPDATE `vehicles` SET job = '%d' WHERE id = '%d'", _:job, Vehicle_GetID(vehicleid));
    }
    return true;
}

stock JobType:Vehicle_GetJob(vehicleid)
{
    if(!Vehicle_IsValid(vehicleid))
        return JOB_TYPE_NONE;
    return VehicleJobType{vehicleid};
}

stock bool:SendMessageToJob(JobType:jobType, color, String:message)
{
    if(jobType == JOB_TYPE_NONE)
        return false;
    foreach(new i : Player)
    {
        if(!Character_IsLogged(i) || Character_GetJobType(i) != jobType || !Character_IsJobDuty(i))
            continue;
        SendTwoLinesMessageStr(i, color, message);
    }
    return true;
}

stock Character_SetRequestedService(playerid, JobType:type)
{
    RequestedService{playerid} = type;
}

stock JobType:Character_GetRequestedService(playerid)
{
    return RequestedService{playerid};
}

stock bool:Job_AllowContactCenter(JobType:jobType)
{
    return jobType == JOB_TYPE_TAXI || jobType == JOB_TYPE_MECHANIC;
}