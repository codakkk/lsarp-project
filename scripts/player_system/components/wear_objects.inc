
/*

TIP BY Y_LESS FOR RESETTING ENUMS:

stock const RESET[E_THING] = {1, 2, 0, 0, 0, "hi" };
MyVar[d] = RESET;

*/

#include <YSI_Coding\y_hooks>

#define MAX_PLAYER_OBJECTS              (20)

#define OBJECT_OTHER 					(0)
#define OBJECT_BANDANA					(1)
#define OBJECT_HAT						(2)
#define OBJECT_OTHER_HAT				(3)
#define OBJECT_WATCH					(4)
#define OBJECT_GLASSES					(5)
#define OBJECT_HELMET					(6)
#define OBJECT_MASK						(7)
#define OBJECT_EXTRA					(8)
#define OBJECT_POLICE					(9)
#define OBJECT_FIRE						(10)
#define OBJECT_HALLOWEEN				(11)

#define BONE_SPINE						(1)
#define BONE_HEAD						(2)
#define BONE_RHAND						(6)
#define BONE_RFOOT						(10)

static enum E_OBJECTSLIST
{
	Object,
	Name[32],
	Type,
	DefaultBone
};
// Should I use a Map with Category as key and a list of objects as value?
// Thanks to Sublime (Frex) for this List anyway <3
static const PlayerObjectsList[][E_OBJECTSLIST] =
{
	{0, "Nessuno", OBJECT_OTHER, BONE_RHAND},

	{11746, "Chiave", OBJECT_OTHER, BONE_RHAND},
	{19920, "Telecomando 1", OBJECT_OTHER, BONE_RHAND},
	{2344, "Telecomando 2",	OBJECT_OTHER, BONE_RHAND},
	{19874, "Saponetta", OBJECT_OTHER, BONE_RHAND},
	{19896, "Pacco sigarette", OBJECT_OTHER, BONE_RHAND},
	{19873, "Carta igienica", OBJECT_OTHER, BONE_RHAND},
	{19627, "Chiave inglese", OBJECT_OTHER, BONE_RHAND},
	{19621, "Tanica olio", OBJECT_OTHER, BONE_RHAND},
	{19623, "Macchina fotografica", OBJECT_OTHER, BONE_RHAND},
	{18635, "Martello", OBJECT_OTHER, BONE_RHAND},
	{19583, "Coltello da cucina", OBJECT_OTHER, BONE_RHAND},
	{11719, "Padella", OBJECT_OTHER, BONE_RHAND},
	{2713, "Secchio pulizie", OBJECT_OTHER, BONE_RHAND},
	{18634, "Piede di porco", OBJECT_OTHER, BONE_RHAND},
	{19631, "Martello", OBJECT_OTHER, BONE_RHAND},
	{19878, "Skateboard", OBJECT_OTHER, BONE_RFOOT},
	{19626, "Pala", OBJECT_OTHER, BONE_RHAND},
	{18632, "Canna da pesca", OBJECT_OTHER, BONE_RHAND},

	{18894, "Bandana gialla", OBJECT_BANDANA, BONE_HEAD},
	{18889, "Bandana fucsia", OBJECT_BANDANA, BONE_HEAD},
	{18903, "Bandana marrone", OBJECT_BANDANA, BONE_HEAD},
	{18894, "Bandana sabbia", OBJECT_BANDANA, BONE_HEAD},
	{18905, "Bandana gialla", OBJECT_BANDANA, BONE_HEAD},
	{18894, "Bandana gialla", OBJECT_BANDANA, BONE_HEAD},
	{18907, "Bandana viola & gialla", OBJECT_BANDANA, BONE_HEAD},
	{18910, "Bandana rossa & arancione", OBJECT_BANDANA, BONE_HEAD},
	{18909, "Bandana azzurra", OBJECT_BANDANA, BONE_HEAD},
	{18908, "Bandana blu & azzurra", OBJECT_BANDANA, BONE_HEAD},
	{18906, "Bandana rossa & gialla", OBJECT_BANDANA, BONE_HEAD},
	{18900, "Bandana gialla & viola", OBJECT_BANDANA, BONE_HEAD},
	{18895, "Bandana nera teschi", OBJECT_BANDANA, BONE_HEAD},
	{18893, "Bandana rossa & bianca", OBJECT_BANDANA, BONE_HEAD},
	{18908, "Bandana blu & azzurra", OBJECT_BANDANA, BONE_HEAD},
	{18892, "Bandana rosso scuro", OBJECT_BANDANA, BONE_HEAD},
	{18896, "Bandana nera & bianca", OBJECT_BANDANA, BONE_HEAD},
	{18901, "Bandana tigrata", OBJECT_BANDANA, BONE_HEAD},
	{18891, "Bandana blu & nera", OBJECT_BANDANA, BONE_HEAD},
	{18902, "Bandana giallo chiaro", OBJECT_BANDANA, BONE_HEAD},
	{18904, "Bandana blu & nera", OBJECT_BANDANA, BONE_HEAD},

	{18922, "Berretto rosso", OBJECT_HAT, BONE_HEAD},
	{18923, "Berretto blu", OBJECT_HAT,	BONE_HEAD},
	{18924, "Berretto militare", OBJECT_HAT, BONE_HEAD},
	{18925, "Berretto rosso & bianco", OBJECT_HAT, BONE_HEAD},
	{18939, "Snapback acqua scuro", OBJECT_HAT,	BONE_HEAD},
	{18940, "Snapback acqua chiaro", OBJECT_HAT, BONE_HEAD},
	{18941, "Snapback nero", OBJECT_HAT, BONE_HEAD},
	{18942, "Snapback marrone chiaro", OBJECT_HAT, BONE_HEAD},
	{18943, "Snapback verde", OBJECT_HAT, BONE_HEAD},
	{19160, "Cappello giallo", OBJECT_HAT, BONE_HEAD},
	{18952, "Berretto pugile", OBJECT_HAT, BONE_HEAD},
	{18926, "Cappello militare", OBJECT_HAT, BONE_HEAD},
	{18927, "Cappello acqua militare", OBJECT_HAT, BONE_HEAD},
	{18928, "Cappello fantasia", OBJECT_HAT, BONE_HEAD},
	{18929, "Cappello verde & bianco", OBJECT_HAT, BONE_HEAD},
	{18930, "Cappello fuoco", OBJECT_HAT, BONE_HEAD},
	{18931, "Cappello acqua scuro", OBJECT_HAT,	BONE_HEAD},
	{18932, "Cappello arancione & bianco", OBJECT_HAT, BONE_HEAD},
	{18933, "Cappello marrone & bianco", OBJECT_HAT, BONE_HEAD},
	{18934, "Cappello rosso semplice", OBJECT_HAT, BONE_HEAD},
	{18935, "Cappello giallo semplice", OBJECT_HAT,	BONE_HEAD},
	{18961, "Cappello camionista", OBJECT_HAT, BONE_HEAD},
	{19558, "Cappello pizzaboy", OBJECT_HAT, BONE_HEAD},
	{18967, "Bucket hat nero", OBJECT_HAT, BONE_HEAD},
	{18968, "Bucket hat misto", OBJECT_HAT,	BONE_HEAD},
	{18969, "Bucket hat arancione", OBJECT_HAT,	BONE_HEAD},
	{19064, "Cappello babbo natale", OBJECT_HAT, BONE_HEAD},
	{19094, "Cappello hamburger", OBJECT_HAT, BONE_HEAD},
	{19137, "Cappello cock", OBJECT_HAT, BONE_HEAD},
	{19528, "Cappello a punta", OBJECT_HAT,	BONE_HEAD},

	{19114, "Elmetto don't shoot", OBJECT_OTHER_HAT, BONE_HEAD},
	{19115, "Elmetto teschio", OBJECT_OTHER_HAT, BONE_HEAD},
	{19116, "Elmetto nero", OBJECT_OTHER_HAT, BONE_HEAD},
	{19117, "Elmetto rosso", OBJECT_OTHER_HAT, BONE_HEAD},
	{19118, "Elmetto verde", OBJECT_OTHER_HAT, BONE_HEAD},
	{19119, "Elmetto blu scuro", OBJECT_OTHER_HAT, BONE_HEAD},
	{19120, "Elmetto evidenziatore",  OBJECT_OTHER_HAT, BONE_HEAD},
	{19115, "Elmetto teschio", OBJECT_OTHER_HAT, BONE_HEAD},
	{19107, "Elmetto navy", OBJECT_OTHER_HAT, BONE_HEAD},
	{19108, "Elmetto militare", OBJECT_OTHER_HAT, BONE_HEAD},
	{18638, "Elmetto lavori", OBJECT_OTHER_HAT, BONE_HEAD},
	{18947, "Paglietta nera", OBJECT_OTHER_HAT, BONE_HEAD},
	{18948, "Paglietta blu", OBJECT_OTHER_HAT, BONE_HEAD},
	{18949, "Paglietta verde", OBJECT_OTHER_HAT, BONE_HEAD},
	{18950, "Paglietta rossa", OBJECT_OTHER_HAT, BONE_HEAD},
	{18951, "Paglietta gialla", OBJECT_OTHER_HAT, BONE_HEAD},
	{19488, "Paglietta bianca", OBJECT_OTHER_HAT, BONE_HEAD},
	{18946, "Paglietta scura", OBJECT_OTHER_HAT, BONE_HEAD},
	{18639, "Paglietta nera semplice", OBJECT_OTHER_HAT, BONE_HEAD},

	{19039, "Orologio giallo", OBJECT_WATCH, BONE_RHAND},
	{19040, "Orologio acciaio", OBJECT_WATCH, BONE_RHAND},
	{19041, "Orologio marrone", OBJECT_WATCH, BONE_RHAND},
	{19042, "Orologio oro", OBJECT_WATCH, BONE_RHAND},
	{19043, "Orologio argento", OBJECT_WATCH, BONE_RHAND},
	{19044, "Orologio viola", OBJECT_WATCH, BONE_RHAND},
	{19045, "Orologio rosso", OBJECT_WATCH, BONE_RHAND},
	{19046, "Orologio verde", OBJECT_WATCH, BONE_RHAND},
	{19053, "Orologio militare", OBJECT_WATCH, BONE_RHAND},

	{19030, "Occhiali da vista", OBJECT_GLASSES, BONE_HEAD},
	{19033, "Occhiali neri tondi", OBJECT_GLASSES, BONE_HEAD},
	{19035, "Occhiali blu", OBJECT_GLASSES,	BONE_HEAD},
	{19023, "Aviator blu", OBJECT_GLASSES, BONE_HEAD},
	{19024, "Aviator viola", OBJECT_GLASSES, BONE_HEAD},
	{19026, "Aviator rossi", OBJECT_GLASSES, BONE_HEAD},
	{19027, "Aviator arancioni", OBJECT_GLASSES, BONE_HEAD},
	{19028, "Aviator gialli", OBJECT_GLASSES, BONE_HEAD},
	{19029, "Aviator verdi", OBJECT_GLASSES, BONE_HEAD},

	{18977, "Casco rosso", OBJECT_HELMET, BONE_HEAD},
	{18978, "Casco bianco", OBJECT_HELMET, BONE_HEAD},
	{18979, "Casco viola", 	OBJECT_HELMET, BONE_HEAD},
	{18645, "Casco rosso & bianco", OBJECT_HELMET, BONE_HEAD},
	{18976, "Casco blu & bianco", OBJECT_HELMET, BONE_HEAD},

	{19038, "Maschera verde", OBJECT_MASK, BONE_HEAD},
	{19037, "Maschera rossa", OBJECT_MASK, BONE_HEAD},
	{19036, "Maschera bianca", OBJECT_MASK, BONE_HEAD},
	{19801, "Passamontagna", OBJECT_MASK, BONE_HEAD},

	{19421, "Cuffiette argentate", OBJECT_EXTRA, BONE_HEAD},
	{19422, "Cuffiette nere", OBJECT_EXTRA, BONE_HEAD},
	{19423, "Cuffiette marroni", OBJECT_EXTRA, BONE_HEAD},
	{19472, "Maschera anti gas", OBJECT_EXTRA, BONE_HEAD},
	{19517, "Parrucca bionda", OBJECT_EXTRA, BONE_HEAD},
	{19077, "Parrucca nera", OBJECT_EXTRA, BONE_HEAD},
	{18975, "Parrucca afro nera", OBJECT_EXTRA, BONE_HEAD},
	{19516, "Parrucca afro bionda", OBJECT_EXTRA, BONE_HEAD},
	{19518, "Parrucca afro grigia", OBJECT_EXTRA, BONE_HEAD},
	{19555, "Guantone pugilato sx", OBJECT_EXTRA, BONE_HEAD},
	{19556, "Guantone pugilato dx", OBJECT_EXTRA, BONE_HEAD},

	{19161, "Cappello police nero", OBJECT_POLICE, BONE_HEAD},
	{19162, "Cappello police blu", OBJECT_POLICE, BONE_HEAD},
	{19520, "Cappello semplice", OBJECT_POLICE, BONE_HEAD},
	{19515, "Armatura grigia", 	OBJECT_POLICE, BONE_SPINE},
	{19142, "Armatura nera", OBJECT_POLICE, BONE_SPINE},
	{19904, "Giacca traffico", OBJECT_POLICE, BONE_SPINE},
	{19200, "Elmetto", OBJECT_POLICE, BONE_HEAD},
	{19141, "Elmetto SWAT", OBJECT_POLICE, BONE_HEAD},
	{19138, "Occhiali neri", OBJECT_POLICE, BONE_HEAD},
	{19139, "Occhiali rossi", OBJECT_POLICE, BONE_HEAD},
	{19140, "Occhiali blu", OBJECT_POLICE, BONE_HEAD},
	{19773, "Fondina", OBJECT_POLICE, BONE_SPINE},
	{19775, "Distintivo", OBJECT_POLICE, BONE_SPINE},
	{19942, "Radio", OBJECT_POLICE, BONE_SPINE},
	{11750, "Manette", OBJECT_POLICE, BONE_SPINE},
	{18637, "Scudo", OBJECT_POLICE, BONE_RHAND},

	{1210, 	"Valigetta", OBJECT_OTHER, BONE_RHAND},
	{11745, "Borsone", OBJECT_OTHER, BONE_RHAND},
	{19559, "Zaino da campeggio", OBJECT_OTHER, BONE_SPINE},
	{1550, "Borsa soldi", OBJECT_OTHER, BONE_SPINE},
	{1310, "Zaino", OBJECT_OTHER, BONE_SPINE},
	{19624, "Valigia", 	OBJECT_OTHER, BONE_RHAND},
	{1212, "Denaro", OBJECT_OTHER, BONE_RHAND},
	{19893, "PC portatile", OBJECT_OTHER, BONE_RHAND},
	{1650, "Tanica", OBJECT_OTHER, BONE_RHAND},
	{19592, "Cestino spesa", OBJECT_OTHER, BONE_RHAND},
	{1265, "Sacco spazzatura", OBJECT_OTHER, BONE_RHAND},
	{19610, "Microfono", OBJECT_OTHER, BONE_RHAND},

	{18641, "Torcia", OBJECT_EXTRA, BONE_RHAND},
	{11738, "Kit pronto soccorso", OBJECT_EXTRA, BONE_RHAND},
	{1640, "Telo verde e bianco", OBJECT_EXTRA, BONE_RHAND},
	{1641, "Telo viola rockstar", OBJECT_EXTRA, BONE_RHAND},
	{1642, "Telo rosso & bianco", OBJECT_EXTRA, BONE_RHAND},
	{1643, "Telo giallo & rosso", OBJECT_EXTRA, BONE_RHAND},
	{19636, "Cassa frutta rossa", OBJECT_EXTRA, BONE_RHAND},
	{19637, "Cassa frutta verde", OBJECT_EXTRA, BONE_RHAND},
	{19638, "Cassa frutta gialla", OBJECT_EXTRA, BONE_RHAND},
	{19639, "Cassa frutta vuota", OBJECT_EXTRA, BONE_RHAND},
	{19818, "Bicchiere vetro", OBJECT_EXTRA, BONE_RHAND},
	{19820, "Bottiglia vetro", OBJECT_EXTRA, BONE_RHAND},
	{19823, "Bottiglia vetro quadrata", OBJECT_EXTRA, BONE_RHAND},
	{1485, 	"Sigaro", OBJECT_EXTRA, BONE_RHAND},

	{18912, "Bandana bianca & nera", OBJECT_BANDANA, BONE_HEAD},
	{18913, "Bandana bianca & verde", OBJECT_BANDANA, BONE_HEAD},
	{18914, "Bandana militare", OBJECT_BANDANA, BONE_HEAD},
	{18915, "Bandana fucsia", OBJECT_BANDANA, BONE_HEAD},
	{18916, "Bandana giallo chiaro", OBJECT_BANDANA, BONE_HEAD},
	{18917, "Bandana blu acqua", OBJECT_BANDANA, BONE_HEAD},
	{18918, "Bandana bianca & verde chiaro", OBJECT_BANDANA, BONE_HEAD},
	{18919, "Bandana nera & bianca", OBJECT_BANDANA, BONE_HEAD},
	{18928, "Bandana gialla & marrone", OBJECT_BANDANA, BONE_HEAD},

	{2295, "Zaino blu", OBJECT_OTHER, BONE_SPINE},

	{11704, "Maschera rossa", OBJECT_HALLOWEEN, BONE_HEAD},
	{19136, "Cappello nero", OBJECT_HALLOWEEN, BONE_HEAD},
	{19352, "Cilindro nero", OBJECT_HALLOWEEN, BONE_HEAD},
	{19528, "Cappello a punta", OBJECT_HALLOWEEN, BONE_HEAD},
	{19557, "Maschera volto", OBJECT_HALLOWEEN, BONE_HEAD},
	{19553, "Cappello paglia", OBJECT_HALLOWEEN, BONE_HEAD},
	{18963, "Maschera CJ", OBJECT_HALLOWEEN, BONE_HEAD},
	{19320, "Zucca halloween", OBJECT_HALLOWEEN, BONE_HEAD},

	{366, "Estintore", OBJECT_FIRE, BONE_RHAND},
	{19330, "Elmetto giallo", OBJECT_FIRE, BONE_HEAD},
	{19331, "Elmetto nero", OBJECT_FIRE, BONE_HEAD},
	{11734, "Kit primo soccorso verde", OBJECT_FIRE, BONE_RHAND},
	{11738, "Kit primo soccorso rosso", OBJECT_FIRE, BONE_RHAND},
	{11747, "Bandana", 	OBJECT_FIRE, BONE_RHAND}
};

static enum E_PLAYER_CUSTOM_OBJECT_DATA
{
    Used,
    Name[32],
    Index,
    Bone,
    ModelId,
    Float:X,
    Float:Y,
    Float:Z,
    Float:RotX,
    Float:RotY,
    Float:RotZ,
    Float:ScaleX,
    Float:ScaleY,
    Float:ScaleZ,
    MaterialColor1,
    MaterialColor2
}
static 
    PlayerCustomObject[MAX_PLAYERS][MAX_PLAYER_OBJECTS][E_PLAYER_CUSTOM_OBJECT_DATA],
    DefaultCustomObject[E_PLAYER_CUSTOM_OBJECT_DATA] = {0, "", -1, 0, -1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0},
    SelectedCategory[MAX_PLAYERS char],
    SelectedIndex[MAX_PLAYERS char],
    List:CategoryIndex[MAX_PLAYERS]
;

hook OnGameModeInit()
{
    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS character_wear_objects \
						(`character_id` INT(11) NOT NULL, \
                         `slot_id` INT(4) NOT NULL, \
                         `name` VARCHAR(64) NOT NULL, \
                         `model_id` INT(11) NOT NULL, \
                         `bone` INT(4) NOT NULL, \
                         `offset_x` FLOAT DEFAULT 0.0, \
                         `offset_y` FLOAT DEFAULT 0.0, \
                         `offset_z` FLOAT DEFAULT 0.0, \
                         `rot_x` FLOAT DEFAULT 0.0, \
                         `rot_y` FLOAT DEFAULT 0.0, \
                         `rot_z` FLOAT DEFAULT 0.0, \
                         `scale_x` FLOAT DEFAULT 0.0, \
                         `scale_y` FLOAT DEFAULT 0.0, \
                         `scale_z` FLOAT DEFAULT 0.0, \
                         `mat_color_1` INT(11) DEFAULT 0, \
                         `mat_color_2` INT(11) DEFAULT 0, \
                         `wearing` INT(1) DEFAULT 0, \
                         PRIMARY KEY (`character_id`, `slot_id`), \
						CONSTRAINT `CharacterWearObjects` FOREIGN KEY (`character_id`) REFERENCES `characters` (`id`) ON DELETE CASCADE \
						) ENGINE=InnoDB DEFAULT CHARSET=latin1;", false);
    return 1;
}

hook OnPlayerClearData(playerid)
{
    for(new i = 0; i < MAX_PLAYER_OBJECTS; ++i)
        PlayerCustomObject[playerid][i] = DefaultCustomObject;
    SelectedIndex{playerid} = SelectedCategory{playerid} = -1;
    if(list_valid(CategoryIndex[playerid]))
        list_delete_deep(CategoryIndex[playerid]);
    for(new i = 0; i < 10; ++i)
        RemovePlayerAttachedObject(playerid, i);
    return 1;
}

hook OnCharacterLateLoad(playerid)
{
    Character_LoadWearObjects(playerid);
    return 1;
}

hook OnCharacterSpawn(playerid)
{
    for(new i = 0; i < MAX_PLAYER_OBJECTS; ++i)
    {
        if(WearObject_IsWearing(playerid, i))
            Internal_WearObjectUse(playerid, i);
    }
    return 1;
}

hook OnPlayerEditAttachedObj(playerid, response, index, modelid, boneid, Float:fOffsetX, Float:fOffsetY, Float:fOffsetZ, Float:fRotX, Float:fRotY, Float:fRotZ, Float:fScaleX, Float:fScaleY, Float:fScaleZ)
{
    new selected = SelectedIndex{playerid};
    if(!response)
    {
        // Discard changes
        Internal_UpdateWearObject(playerid, index);
        /*SetPlayerAttachedObject(playerid, index, modelid, boneid, 
        PlayerCustomObject[playerid][selected][X], PlayerCustomObject[playerid][selected][Y], PlayerCustomObject[playerid][selected][Z],
        PlayerCustomObject[playerid][selected][RotX], PlayerCustomObject[playerid][selected][RotY], PlayerCustomObject[playerid][selected][RotZ],
        PlayerCustomObject[playerid][selected][ScaleX], PlayerCustomObject[playerid][selected][ScaleY], PlayerCustomObject[playerid][selected][ScaleZ],
        PlayerCustomObject[playerid][selected][MaterialColor1], PlayerCustomObject[playerid][selected][MaterialColor2]);*/
    }
    else if(selected != -1)
    {
        if(fScaleX < 0.1 || fScaleY < 0.1 || fScaleZ < 0.1)
            return Internal_UpdateWearObject(playerid, index), SendClientMessage(playerid, COLOR_ERROR, "La grandezza minima è 0.1.");
        
        if(fScaleX > 2.0 || fScaleY > 2.0 || fScaleZ > 2.0)
            return Internal_UpdateWearObject(playerid, index), SendClientMessage(playerid, COLOR_ERROR, "La grandezza massima è 2.0.");

        PlayerCustomObject[playerid][selected][X] = fOffsetX;
        PlayerCustomObject[playerid][selected][Y] = fOffsetY;
        PlayerCustomObject[playerid][selected][Z] = fOffsetZ;

        PlayerCustomObject[playerid][selected][RotX] = fRotX;
        PlayerCustomObject[playerid][selected][RotY] = fRotY;
        PlayerCustomObject[playerid][selected][RotZ] = fRotZ;

        PlayerCustomObject[playerid][selected][ScaleX] = fScaleX;
        PlayerCustomObject[playerid][selected][ScaleY] = fScaleY;
        PlayerCustomObject[playerid][selected][ScaleZ] = fScaleZ;
        WearObject_Save(playerid, selected);
    }
    return 1;
}

flags:resetoggetto(CMD_ALIVE_USER);
CMD:resetoggetto(playerid, params[])
{
    for(new i = 0; i < 10; ++i)
        RemovePlayerAttachedObject(playerid, i);
    SendClientMessage(playerid, -1, "Hai rimosso tutti gli oggetti indossati.");    
    return 1;
}

flags:oggetto(CMD_ALIVE_USER);
CMD:oggetto(playerid, params[])
{
    if(EditObject_IsEditing(playerid))
        EditObject_End(playerid);
    new String:str = @("Oggetto\tStato\tSlot\n");
    for(new i = 0; i < MAX_PLAYER_OBJECTS; ++i)
    {
        //if(WearingObjectID[playerid][i] == -1)
        if(WearObject_IsUsed(playerid, i))
        {
            str += str_format("%s\t%s\t%d", PlayerCustomObject[playerid][i][Name], (IsPlayerAttachedObjectSlotUsed(playerid, WearObject_GetAttachedIndex(playerid, i)) ? ("{00FF00}Indossato{FFFFFF}") : ("{FF0000}Non Indossato{FFFFFF}")), i+1);
        }
        else
        {
            str += str_format("Slot Libero\t--\t%d", i+1);
        }
        str += @("\n");
    }
    Dialog_Show_s(playerid, Dialog_WearObjectList, DIALOG_STYLE_TABLIST_HEADERS, @("Lista oggetti"), str, "Avanti", "Chiudi");
    //Dialog_Show(playerid, DialogPObjectsCategory, DIALOG_STYLE_LIST, "Categorie", "Oggetti vari\nBandane\nCappelli\nCappelli 2\nOrologi\nOcchiali\nElmetti\nMaschere\nExtra\nLSPD\nLSFD", "Avanti", "Annulla");
    return 1;
}

//========================================= DIALOGS =====================================================

Dialog:Dialog_WearObjectList(playerid, response, listitem, inputtext[])
{
    if(!response) 
        return 0;
    SelectedIndex{playerid} = listitem;
    Internal_ShowWearObjectOptions(playerid, listitem);
    return 1;
}

Dialog:Dialog_WearObjectEdit(playerid, response, listitem, inputtext[])
{
    if(!response)
        return pc_cmd_oggetto(playerid, "");
    new 
        index = SelectedIndex{playerid},
        slotid = PlayerCustomObject[playerid][index][Index];
    switch(listitem)
    {
        case 0:
        {
            if(IsPlayerAttachedObjectSlotUsed(playerid, slotid))
            {
                WearObject_Remove(playerid, index);
                SendFormattedMessage(playerid, COLOR_GREEN, "Hai rimosso l'oggetto (%s) dallo slot %d.", PlayerCustomObject[playerid][index][Name], index+1);
            }
            else
            {
                if(!WearObject_IsUsed(playerid, index))
                    return SendClientMessage(playerid, COLOR_ERROR, "Non hai inizializzato l'oggetto.");
                WearObject_Use(playerid, index);
            }
        }
        case 1:
        {
            if(slotid != -1 && IsPlayerAttachedObjectSlotUsed(playerid, slotid))
                return SendClientMessage(playerid, COLOR_ERROR, "Non puoi modificare il modello se lo stai indossando.");
            return Internal_ShowCategories(playerid);
        }
        case 2:
        {
            return Dialog_Show(playerid, Dialog_WearObjectBone, DIALOG_STYLE_LIST, "Cambia osso", "Spina dorsale\nTesta\nBraccio sinistro\nBraccio destro\nMano sinistra\nMano destra\nCoscia sinistra\nCoscia destra\nPiede sinistro\nPiede destro\nPolpaccio destro\nPolpaccio sinistro\nAvambraccio sinistro\nAvambraccio destro\nSpalla (sinistra)\nSpalla (destra)\nCollo\nMascella", "Procedi", "Indietro");
        }
        case 3:
        {
            if(slotid == -1 || !IsPlayerAttachedObjectSlotUsed(playerid, slotid))
                return SendClientMessage(playerid, COLOR_ERROR, "Devi indossare l'oggetto per poterlo modificare.");
            EditObject_Start(playerid, EDIT_TYPE_PLAYER_OBJECT, slotid, 0);
        }
        case 4:
        {
            return Dialog_Show(playerid, Dialog_WearObjectColor1, DIALOG_STYLE_LIST, "Colore primario", "Colore corrente\nBlu\nBlu chiaro\nRosso\nRosa\nViola\nVerde\nVerde chiaro\nArancione\nBianco\nNero\nGiallo", "Avanti", "Annulla");
        }
        case 5:
        {
            Internal_DeleteWearObject(playerid, index);
            SendFormattedMessage(playerid, -1, "Hai cancellato l'oggetto %d.", index);
        }
    }
    return 1;
}

Dialog:Dialog_WearObjectBone(playerid, response, listitem, inputtext[])
{
    if(!response)
        return pc_cmd_oggetto(playerid, "");
    mysql_tquery_f(gMySQL, "UPDATE character_wear_objects SET bone = '%d' WHERE character_id = '%d' AND slot_id = '%d';", listitem+1, Character_GetID(playerid), SelectedIndex{playerid});
    SendClientMessage(playerid, COLOR_GREEN, "Hai modificato l'osso dell'oggetto.");
    PlayerCustomObject[playerid][SelectedIndex{playerid}][Bone] = listitem+1;
    Internal_UpdateWearObject(playerid, SelectedIndex{playerid});
    return 1;
}


Dialog:Dialog_WearObjectCategories(playerid, response, listitem, inputtext[])
{
    if(!response)
        return pc_cmd_oggetto(playerid, "");
    
    SelectedCategory{playerid} = listitem;
    WearObject_ShowCategory(playerid, listitem);   
    return 1;
}

Dialog:Dialog_WearObjectSelect(playerid, response, listitem, inputtext[])
{
    if(!response)
    {
        return Internal_ShowCategories(playerid);
    }
 
    new 
        objectid = list_get(CategoryIndex[playerid], listitem);
    SendFormattedMessage(playerid, -1, "Hai modificato l'oggetto %d. Stai ora indossando: %s.", SelectedIndex{playerid} +1, PlayerObjectsList[objectid][Name]);

    Internal_AddWearObject(playerid, SelectedIndex{playerid}, PlayerObjectsList[objectid][Name], PlayerObjectsList[objectid][Object], PlayerObjectsList[objectid][DefaultBone], 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0, 0);
    WearObject_Save(playerid, SelectedIndex{playerid});
    return 1;
}

OnResponse:Dialog_WearObjectSelect(playerid, response, listitem, inputtext[])   
{
    list_delete_deep(CategoryIndex[playerid]);
    return 1;
}

Dialog:Dialog_WearObjectColor1(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Internal_ShowWearObjectOptions(playerid, SelectedIndex{playerid});
    switch(listitem)
    {
        case 1: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor1] = HexToInt("0000ccff");
		case 2: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor1] = HexToInt("4d4dffff");
		case 3: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor1] = HexToInt("ff0000ff");
		case 4: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor1] = HexToInt("ffccffff");
		case 5: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor1] = HexToInt("990099ff");
		case 6: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor1] = HexToInt("009933ff");
		case 7: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor1] = HexToInt("33ff77ff");
		case 8: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor1] = HexToInt("ff6600ff");
		case 9: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor1] = HexToInt("ffffffff");
		case 10: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor1] = HexToInt("000000ff");
		case 11: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor1] = HexToInt("ffff00ff");
    }
    return Dialog_Show(playerid, Dialog_WearObjectColor2, DIALOG_STYLE_LIST, "Colore secondario", "Colore corrente\nBlu\nBlu chiaro\nRosso\nRosa\nViola\nVerde\nVerde chiaro\nArancione\nBianco\nNero\nGiallo", "Avanti", "Annulla");
}

Dialog:Dialog_WearObjectColor2(playerid, response, listitem, inputtext[])
{
    if(!response)
        return Internal_ShowWearObjectOptions(playerid, SelectedIndex{playerid});
    switch(listitem)
    {
        case 1: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor2] = HexToInt("0000ccff");
		case 2: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor2] = HexToInt("4d4dffff");
		case 3: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor2] = HexToInt("ff0000ff");
		case 4: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor2] = HexToInt("ffccffff");
		case 5: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor2] = HexToInt("990099ff");
		case 6: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor2] = HexToInt("009933ff");
		case 7: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor2] = HexToInt("33ff77ff");
		case 8: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor2] = HexToInt("ff6600ff");
		case 9: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor2] = HexToInt("ffffffff");
		case 10: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor2] = HexToInt("000000ff");
		case 11: PlayerCustomObject[playerid][SelectedIndex{playerid}][MaterialColor2] = HexToInt("ffff00ff");
    }
    WearObject_Save(playerid, SelectedIndex{playerid});
    Internal_UpdateWearObject(playerid, SelectedIndex{playerid});
    return 1;
}

//========================================= END DIALOGS =====================================================

stock WearObject_ShowCategory(playerid, category)
{
    if(list_valid(CategoryIndex[playerid]))
        list_clear(CategoryIndex[playerid]);
    else
        CategoryIndex[playerid] = list_new();
    new 
        String:str = @("");
    for(new i = 0, j = sizeof(PlayerObjectsList); i < j; ++i)
    {
        if(PlayerObjectsList[i][Type] != category)
            continue;
        str += str_format("%s\n", PlayerObjectsList[i][Name]);
        list_add(CategoryIndex[playerid], i);
    }
    return Dialog_Show_s(playerid, Dialog_WearObjectSelect, DIALOG_STYLE_LIST, @("Oggetti"), str, "Seleziona", "Chiudi");
}

stock WearObject_Use(playerid, slotid)
{
    if(slotid < 0 || slotid >= MAX_PLAYER_OBJECTS)
        return 0;
    if(!Internal_WearObjectUse(playerid, slotid))
        SendClientMessage(playerid, COLOR_ERROR, "Stai già indossando il numero massimo di oggetti (9).");
    SendFormattedMessage(playerid, -1, "Oggetto (%s) indossato.", PlayerCustomObject[playerid][slotid][Name]);
    return 1;
}

stock WearObject_Remove(playerid, slotid)
{
    if(slotid < 0 || slotid >= MAX_PLAYER_OBJECTS)
        return 0;
    new index = PlayerCustomObject[playerid][slotid][Index];
    if(index == -1)
        return 0;
    RemovePlayerAttachedObject(playerid, index);
    PlayerCustomObject[playerid][slotid][Index] = -1;
    return 1;
}

stock WearObject_Save(playerid, slotid)
{
    if(slotid < 0 || slotid >= MAX_PLAYER_OBJECTS)
        return 0;
    if(WearObject_IsUsed(playerid, slotid))
    {
        mysql_tquery_f(gMySQL, "INSERT INTO character_wear_objects\
        (character_id, slot_id, name, model_id, bone, offset_x, offset_y, offset_z, rot_x, rot_y, rot_z, scale_x, scale_y, scale_z, mat_color_1, mat_color_2, wearing) \
        VALUES('%d', '%d', '%e', '%d', '%d', '%f', '%f', '%f', '%f', '%f', '%f', '%f', '%f', '%f', '%d', '%d', '%d') \
        ON DUPLICATE KEY UPDATE \
        name = VALUES(name), \
        model_id = VALUES(model_id), \
        bone = VALUES(bone), \
        offset_x = VALUES(offset_x), \
        offset_y = VALUES(offset_y), \
        offset_z = VALUES(offset_z), \
        rot_x = VALUES(rot_x), \
        rot_y = VALUES(rot_y), \
        rot_z = VALUES(rot_z), \
        scale_x = VALUES(scale_x), \
        scale_y = VALUES(scale_y), \
        scale_z = VALUES(scale_z), \
        mat_color_1 = VALUES(mat_color_1), \
        mat_color_2 = VALUES(mat_color_2), \
        wearing = VALUES(wearing);",
        Character_GetID(playerid), slotid, PlayerCustomObject[playerid][slotid][Name], 
        PlayerCustomObject[playerid][slotid][ModelId], PlayerCustomObject[playerid][slotid][Bone], 
        PlayerCustomObject[playerid][slotid][X], PlayerCustomObject[playerid][slotid][Y], PlayerCustomObject[playerid][slotid][Z],
        PlayerCustomObject[playerid][slotid][RotX], PlayerCustomObject[playerid][slotid][RotY], PlayerCustomObject[playerid][slotid][RotZ], 
        PlayerCustomObject[playerid][slotid][ScaleX], PlayerCustomObject[playerid][slotid][ScaleY], PlayerCustomObject[playerid][slotid][ScaleZ],
        PlayerCustomObject[playerid][slotid][MaterialColor1], PlayerCustomObject[playerid][slotid][MaterialColor2],
        WearObject_GetAttachedIndex(playerid, slotid) != -1 ? 1 : 0
        );
    }
    return 1;
}

stock WearObject_SaveAll(playerid)
{
    mysql_tquery(gMySQL, "START TRANSACTION;");
    for(new i = 0; i < MAX_PLAYER_OBJECTS; ++i)
    {
        WearObject_Save(playerid, i);
    }
    mysql_tquery(gMySQL, "COMMIT;");
}

stock Character_LoadWearObjects(playerid)
{
    inline OnLoad()
    {
        new 
            rows = cache_num_rows(),
            slotid = 0,
            name[32], modelid, bone,
            Float:x = 0.0, Float:y = 0.0, Float:z = 0.0,
            Float:rotx = 0.0, Float:roty = 0.0, Float:rotz = 0.0,
            Float:scalex = 0.0, Float:scaley = 0.0, Float:scalez = 0.0,
            color1 = 0, color2 = 0;
        printf("Player %d loaded %d wear objects.", playerid, rows);
        for(new i = 0; i < rows; ++i)
        {
            if(i >= MAX_PLAYER_OBJECTS)
            {
                printf("Bug occurred. i >= MAX_PLAYER_OBJECTS when loading Wearable objects.");
                break;
            }
            cache_get_value_index_int(i, 1, slotid);
            cache_get_value_index(i, 2, name);
            cache_get_value_index_int(i, 3, modelid);
            cache_get_value_index_int(i, 4, bone);
            
            cache_get_value_index_float(i, 5, x);
            cache_get_value_index_float(i, 6, y);
            cache_get_value_index_float(i, 7, z);
            
            cache_get_value_index_float(i, 8, rotx);
            cache_get_value_index_float(i, 9, roty);
            cache_get_value_index_float(i, 10, rotz);

            cache_get_value_index_float(i, 11, scalex);
            cache_get_value_index_float(i, 12, scaley);
            cache_get_value_index_float(i, 13, scalez);

            cache_get_value_index_int(i, 14, color1);
            cache_get_value_index_int(i, 15, color2);

            new equipped = 0;
            cache_get_value_index_int(i, 16, equipped);

            Internal_AddWearObject(playerid, slotid, name, modelid, bone, x, y, z, rotx, roty, rotz, scalex, scaley, scalez, color1, color2);

            if(equipped == 1)
                Internal_WearObjectUse(playerid, slotid);
        }
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM character_wear_objects WHERE character_id = '%d' AND slot_id != '-1'", Character_GetID(playerid));
}


// ================================================= accessors ==========================================================
stock WearObject_IsUsed(playerid, slotid)
{
    return PlayerCustomObject[playerid][slotid][Used];
}

stock WearObject_GetAttachedIndex(playerid, slotid)
{
    return PlayerCustomObject[playerid][slotid][Index];
}

stock WearObject_GetModelId(playerid, slotid)
{
    return PlayerCustomObject[playerid][slotid][ModelId];
}

stock WearObject_GetBone(playerid, index)
{
    return PlayerCustomObject[playerid][index][Bone];
}

stock WearObject_IsWearing(playerid, index)
{
    return WearObject_GetAttachedIndex(playerid, index) != -1;
}

//========================================= INTERNALS =====================================================

static Internal_UpdateWearObject(playerid, slotid)
{
    if(IsPlayerAttachedObjectSlotUsed(playerid, PlayerCustomObject[playerid][slotid][Index]))
        RemovePlayerAttachedObject(playerid, PlayerCustomObject[playerid][slotid][Index]);
    SetPlayerAttachedObject(playerid, PlayerCustomObject[playerid][slotid][Index], PlayerCustomObject[playerid][slotid][ModelId], PlayerCustomObject[playerid][slotid][Bone], 
    PlayerCustomObject[playerid][slotid][X], PlayerCustomObject[playerid][slotid][Y], PlayerCustomObject[playerid][slotid][Z], 
    PlayerCustomObject[playerid][slotid][RotX], PlayerCustomObject[playerid][slotid][RotY], PlayerCustomObject[playerid][slotid][RotZ], 
    PlayerCustomObject[playerid][slotid][ScaleX], PlayerCustomObject[playerid][slotid][ScaleY], PlayerCustomObject[playerid][slotid][ScaleZ], 
    PlayerCustomObject[playerid][slotid][MaterialColor1], PlayerCustomObject[playerid][slotid][MaterialColor2]);
    return 1;
}

static Internal_ShowWearObjectOptions(playerid, slotid)
{
    if(WearObject_IsUsed(playerid, slotid))
    {
        if(WearObject_GetAttachedIndex(playerid, slotid) == -1)
            Dialog_Show(playerid, Dialog_WearObjectEdit, DIALOG_STYLE_LIST, "Modifica oggetto", "{00FF00}Indossa{FFFFFF}\nCambia oggetto\nCambia osso\nCambia posizione\nCambia colori\nCancella", "Avanti", "Indietro");
        else
            Dialog_Show(playerid, Dialog_WearObjectEdit, DIALOG_STYLE_LIST, "Modifica oggetto", "{FF0000}Rimuovi{FFFFFF}\nCambia oggetto\nCambia osso\nCambia posizione\nCambia colori\nCancella", "Avanti", "Indietro");
    }
    else
    {
        return Internal_ShowCategories(playerid);
    }
    return 1;
}

static Internal_DeleteWearObject(playerid, slotid)
{
    if(slotid < 0 || slotid >= MAX_PLAYER_OBJECTS)
        return 0;
    PlayerCustomObject[playerid][slotid] = DefaultCustomObject;
    mysql_tquery_f(gMySQL, "DELETE FROM character_wear_objects WHERE character_id = '%d' AND slot_id = '%d'", Character_GetID(playerid), slotid);
    WearObject_Save(playerid, slotid);
    return 1;
}

static Internal_ShowCategories(playerid)
{
    return Dialog_Show(playerid, Dialog_WearObjectCategories, DIALOG_STYLE_LIST, "Categorie", "Oggetti vari\nBandane\nCappelli\nCappelli 2\nOrologi\nOcchiali\nElmetti\nMaschere\nExtra\nLSPD\nLSFD", "Avanti", "Indietro");
}

static Internal_AddWearObject(playerid, slotid, const name[], modelid, bone, Float:x = 0.0, Float:y = 0.0, Float:z = 0.0, Float:rotx = 0.0, Float:roty = 0.0, Float:rotz = 0.0, Float:scalex = 1.0, Float:scaley = 1.0, Float:scalez = 1.0, matcolor1 = 0, matcolor2 = 0)
{
    PlayerCustomObject[playerid][slotid][Used] = 1;
    PlayerCustomObject[playerid][slotid][Index] = -1;
    set(PlayerCustomObject[playerid][slotid][Name], name);
    PlayerCustomObject[playerid][slotid][Bone] = bone;
    PlayerCustomObject[playerid][slotid][ModelId] = modelid;

    PlayerCustomObject[playerid][slotid][X] = x;
    PlayerCustomObject[playerid][slotid][Y] = y;
    PlayerCustomObject[playerid][slotid][Z] = z;

    PlayerCustomObject[playerid][slotid][RotX] = rotx;
    PlayerCustomObject[playerid][slotid][RotY] = roty;
    PlayerCustomObject[playerid][slotid][RotZ] = rotz;

    PlayerCustomObject[playerid][slotid][ScaleX] = scalex;
    PlayerCustomObject[playerid][slotid][ScaleY] = scaley;
    PlayerCustomObject[playerid][slotid][ScaleZ] = scalez;

    PlayerCustomObject[playerid][slotid][MaterialColor1] = matcolor1;
    PlayerCustomObject[playerid][slotid][MaterialColor2] = matcolor2;

    /*printf("Custom object debug: %d - %d - %d - X: %f - Y: %f - Z: %f - RX: %f - RY: %f - RZ: %f - SX: %f - SY: %f - SZ: %f - MatCol1: %d - MatCol2: %d", 
            PlayerCustomObject[playerid][slotid][Used], PlayerCustomObject[playerid][slotid][Bone], PlayerCustomObject[playerid][slotid][ModelId],
            PlayerCustomObject[playerid][slotid][X], PlayerCustomObject[playerid][slotid][Y], PlayerCustomObject[playerid][slotid][Z],
            PlayerCustomObject[playerid][slotid][RotX], PlayerCustomObject[playerid][slotid][RotY], PlayerCustomObject[playerid][slotid][RotZ],
            PlayerCustomObject[playerid][slotid][ScaleX], PlayerCustomObject[playerid][slotid][ScaleY], PlayerCustomObject[playerid][slotid][ScaleZ],
            PlayerCustomObject[playerid][slotid][MaterialColor1], PlayerCustomObject[playerid][slotid][MaterialColor2]);
    */
    return 1;
}

static Internal_GetFreeAttachIndex(playerid)
{
    new slot = -1;
    for(new i = 0; i < 10; ++i)
    {
        if(IsPlayerAttachedObjectSlotUsed(playerid, i))
            continue;
        slot = i;
        break;
    }
    return slot;
}

static Internal_WearObjectUse(playerid, slotid)
{
    if(slotid < 0 || slotid >= MAX_PLAYER_OBJECTS)
        return 0;
    new index = Internal_GetFreeAttachIndex(playerid);
    if(index == -1) 
        return 0;
    SetPlayerAttachedObject(playerid, index, WearObject_GetModelId(playerid, slotid), WearObject_GetBone(playerid, slotid), 
    PlayerCustomObject[playerid][slotid][X], PlayerCustomObject[playerid][slotid][Y], PlayerCustomObject[playerid][slotid][Z], 
    PlayerCustomObject[playerid][slotid][RotX], PlayerCustomObject[playerid][slotid][RotY], PlayerCustomObject[playerid][slotid][RotZ], 
    PlayerCustomObject[playerid][slotid][ScaleX], PlayerCustomObject[playerid][slotid][ScaleY], PlayerCustomObject[playerid][slotid][ScaleZ], 
    PlayerCustomObject[playerid][slotid][MaterialColor1], PlayerCustomObject[playerid][slotid][MaterialColor2]);
    PlayerCustomObject[playerid][slotid][Index] = index;
    return 1;
}

static HexToInt(string[])
{
    if(!string[0]) return 0;
    new cur = 1, res = 0;
    for(new i = strlen(string); i > 0; i--)
    {
        res += cur * (string[i - 1] - ((string[i - 1] < 58) ? (48) : (55)));
        cur = cur * 16;
    }
    return res;
}