
#include <YSI_Coding\y_hooks>

// Cigarettes object positions taken from Sublime gamemode. Thanks Frax :)

static const 
    SMOKING_POSITION_NONE = 0,
    SMOKING_POSITION_HAND   = 1,
    SMOKING_POSITION_MOUTH  = 2
;

static const
    SMOKING_TYPE_NONE = 0,
    SMOKING_TYPE_CIGARETTE = 1,
    SMOKING_TYPE_DRUG = 2
;

static 
    IsSmoking[MAX_PLAYERS char],
    SmokingType[MAX_PLAYERS char],
    SmokingShots[MAX_PLAYERS char],
    LastShotTime[MAX_PLAYERS],
    DrugEffectTime[MAX_PLAYERS char]
;

hook OnCharacterClearData(playerid)
{
    IsSmoking{playerid} = SMOKING_POSITION_NONE;
    SmokingShots{playerid} = 0;
    SmokingType{playerid} = SMOKING_TYPE_NONE;
    DrugEffectTime{playerid} = 0;
    LastShotTime[playerid] = 0;
    return 1;
}

ptask OnSmokeDrugTimer[60000](playerid) 
{
    if(SmokingType{playerid} == SMOKING_TYPE_DRUG && SmokingShots{playerid} <= 5)
    {
        DrugEffectTime{playerid}++;
        if(DrugEffectTime{playerid} >= 5)
        {
            DrugEffectTime{playerid} = 0;
            SetPlayerDrunkLevel(playerid, GetPlayerDrunkLevel(playerid) - 2500);
        }
    }   
}

static IsSmokeable(itemid)
{
    return itemid == gItem_Cigarette || itemid == gItem_Marijuana || itemid == gItem_Hashish;
}

hook OnCharacterUseItem(playerid, slotid, itemid)
{
    if(!IsSmokeable(itemid))
        return 1;
    
    if(IsSmoking{playerid})
        return SendClientMessage(playerid, COLOR_ERROR, "Stai già fumando.");
    
    new amount = Character_GetItemAmount(playerid, itemid);
    if(amount <= 0)
        return 1;

    Character_DecreaseItemAmount(playerid, itemid, 1);
    
    Character_AMe(playerid, ServerItem_IsDrug(itemid) ? ("prende una canna e l'accende.") : ("prende una sigaretta e l'accende."));
    SmokingType{playerid} = ServerItem_IsDrug(itemid) ? SMOKING_TYPE_DRUG : SMOKING_TYPE_CIGARETTE;

    IsSmoking{playerid} = SMOKING_POSITION_HAND;
    SmokingShots{playerid} = 10;
    LastShotTime[playerid] = GetTickCount();

    Character_CreateSmokingObject(playerid);
    
    ApplyAnimation(playerid, "SMOKING", "M_smk_in", 4.1, 0, 0, 0, 0, 0, 0);
    SendFormattedMessage(playerid, COLOR_WHITE, "Usa N per cambiare posizione, Y per fare un tiro e H per buttarla.");
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    if(!IsSmoking{playerid})
        return 0;
    
    if( ! ((GetTickCount() - LastShotTime[playerid]) > 3000))
        return 0;
    
    if(PRESSED(KEY_YES))
    {
        if(SmokingShots{playerid} == 0)
            return Character_StopSmoking(playerid);
        
        ApplyAnimation(playerid, "SMOKING", "M_smk_out", 4.1, 0, 0, 0, 0, 0);

        if(SmokingType{playerid} == SMOKING_TYPE_DRUG && SmokingShots{playerid} <= 5)
        {
            DrugEffectTime{playerid} = 0;
            SetPlayerDrunkLevel(playerid, GetPlayerDrunkLevel(playerid) + 3500);
        }

        SmokingShots{playerid}--;
        LastShotTime[playerid] = GetTickCount();
    }
    else if(PRESSED(KEY_NO))
    {
        if(IsSmoking{playerid} == SMOKING_POSITION_HAND)
        {
            RemovePlayerAttachedObject(playerid, 2);
            RemovePlayerAttachedObject(playerid, 1);
            SetPlayerAttachedObject(playerid, 2, 18673, 2, 0.0169, 0.1182+0.1, -1.6320, 0.0000, 0.0000, 0.0000, 1.0000, 1.0000,1.0000);
            SetPlayerAttachedObject(playerid, 1, 3027, 2, 0.0201, 0.0983, 0.0081, 262.2250, 353.1968, 65.1797, 1.0000, 1.0000, 1.0000);
            ApplyAnimation(playerid, "SMOKING", "M_smk_drag", 4.1, 0, 0, 0, 0, 0);
            IsSmoking{playerid} = SMOKING_POSITION_MOUTH;
        }
        else if(IsSmoking{playerid} == SMOKING_POSITION_MOUTH)
        {
            RemovePlayerAttachedObject(playerid, 2);
            RemovePlayerAttachedObject(playerid, 1);
            SetPlayerAttachedObject(playerid, 2, 18673, 6, 0.1570, -0.0588, -1.6079, 0.0000, 0.0000, 0.0000, 1.0000, 1.0000, 1.0000);
            SetPlayerAttachedObject(playerid, 1, 3027, 6, 0.0852, 0.0303, 0.0194, 88.7970, 53.3082, 162.5791);
            ApplyAnimation(playerid, "SMOKING", "M_smk_out", 4.1, 0, 0, 0, 0, 0);
            IsSmoking{playerid} = SMOKING_POSITION_HAND;
        }
    }
    else if(PRESSED(KEY_CTRL_BACK))
    {
        if(!IsPlayerInAnyVehicle(playerid))
        {
            Character_StopSmoking(playerid);
        }
    }
    return 1;
}

static Character_StopSmoking(playerid)
{
    ApplyAnimation(playerid, "Flowers", "Flower_attack_M", 4.1, 0, 0, 0, 0, 0);

    RemovePlayerAttachedObject(playerid, 1);
    RemovePlayerAttachedObject(playerid, 2);

    Character_AMe(playerid, (SmokingType{playerid} != SMOKING_TYPE_CIGARETTE) ? ("finisce di fumare la canna e la getta per terra.") :
																						 ("finisce di fumare la sigaretta e la getta per terra"));

    IsSmoking{playerid} = SMOKING_POSITION_NONE;
    SmokingShots{playerid} = 0;
    SmokingType{playerid} = SMOKING_TYPE_NONE;
    return 1;
}

static Character_CreateSmokingObject(playerid)
{
    if(SmokingType{playerid} == SMOKING_TYPE_NONE)
        return 0;
    
    if(SmokingType{playerid} == SMOKING_TYPE_DRUG)
    {
        SetPlayerAttachedObject(playerid, 1, 3027, 6, 0.0852, 0.0303, 0.0194, 88.7970, 53.3082, 162.5791);
        SetPlayerAttachedObject(playerid, 2, 18673, 6, 0.1570, -0.0588, -1.6079, 0.0000, 0.0000, 0.0000, 1.0000, 1.0000, 1.0000);
    }
    else if(SmokingType{playerid} == SMOKING_TYPE_CIGARETTE)
    {
        SetPlayerAttachedObject(playerid, 1, 3027, 6, 0.0852, 0.0303, 0.0194, 88.7970, 53.3082, 162.5791);
        SetPlayerAttachedObject(playerid, 2, 18673, 6, 0.1570, -0.0588, -1.6079, 0.0000, 0.0000, 0.0000, 1.0000, 1.0000, 1.0000);
    }
    return 1;
}

flags:fuma(CMD_ALIVE_USER);
CMD:fuma(playerid, params[])
{
    if(IsSmoking{playerid})
        return SendClientMessage(playerid, COLOR_ERROR, "Stai già fumando.");
    
    new Inventory:inventory = Character_GetInventory(playerid),
        slot = Inventory_GetFirstItem(inventory, gItem_Cigarette);
    
    if(slot == -1)
        return SendClientMessage(playerid, COLOR_ERROR, "Non hai una sigaretta nell'inventario.");
    
    if(!Character_CanInteract(playerid))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando ora.");
    if(Request_IsPending(playerid, REQUEST_TYPE_ITEM))
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questo comando se hai una richiesta d'oggetto attiva.");
    
    Character_UseInventoryItem(playerid, slot);
    return 1;
}