
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
    {"Tassista", 0.0, 0.0, 0.0, JOB_TYPE_TAXI},
    {"Meccanico", 0.0, 0.0, 0.0, JOB_TYPE_MECHANIC}
};

static 
    JobType:PlayerJob[MAX_PLAYERS char] = {JOB_TYPE_NONE, ...},
    bool:JobDuty[MAX_PLAYERS char] = {false, ...};

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

hook OnCharacterClearData(playerid)
{
    PlayerJob{playerid} = JOB_TYPE_NONE;
    JobDuty{playerid} = false;
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
        if(Character_EnterJobDuty(playerid))
        {
            SendClientMessage(playerid, jobColor, "Sei entrato in servizio.");
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

    Character_SetJobType(playerid, JOB_TYPE_NONE);

    SendClientMessage(playerid, COLOR_ORANGE, "Hai lasciato il lavoro.");

    CallLocalFunction(#OnCharacterLeaveJob, "d", playerid);
    return 1;
}
alias:leavejob("lascialavoro");

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
    return 1;
}

stock Character_EnterJobDuty(playerid)
{
    #pragma unused playerid
    return false;
}

stock bool:Character_ExitJobDuty(playerid)
{
    #pragma unused playerid
    return false;
}
