
/*

TIP BY Y_LESS FOR RESETTING ENUMS:

stock const RESET[E_THING] = {1, 2, 0, 0, 0, "hi" };
MyVar[d] = RESET;

*/
//#define for_playerobjects(%0:%1) for(new Iter:iter=list_iter(Character_GetObjectsList(%1)), PlayerObject:%0=PlayerObject:iter_get(iter);iter_inside(iter);%0=(iter_move_next(iter) != ITER_NULL ? (PlayerObject:iter_get(iter)) : (PlayerObject:0)))
#define for_playerobjects(%0:%1) for(new Iter:iter=list_iter(Character_GetObjectsList(%1)), PlayerObject:%0=(iter_inside(iter) ? (PlayerObject:iter_get(iter)) : (PlayerObject:0));iter_inside(iter);%0=(iter_move_next(iter) != ITER_NULL ? (PlayerObject:iter_get(iter)) : (PlayerObject:0)))
#define MAX_OBJECTS_PER_PLAYER      (20)

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
	oName[32],
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
	{11736, "Kit primo soccorso verde", OBJECT_FIRE, BONE_RHAND},
	{11738, "Kit primo soccorso rosso", OBJECT_FIRE, BONE_RHAND},
	{11747, "Bandana", 	OBJECT_FIRE, BONE_RHAND}
};

static enum _:E_PLAYER_CUSTOM_OBJECT_DATA
{
    ID,
    PlayerID,
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
    List:PlayerObjects[MAX_PLAYERS],
    SelectedCategory[MAX_PLAYERS char],
    PlayerObject:SelectedPlayerObject[MAX_PLAYERS],
    List:CategoryIndex[MAX_PLAYERS]
;

hook OnGameModeInit()
{
    mysql_query(gMySQL, "CREATE TABLE IF NOT EXISTS character_wear_objects \
						(`id` INT(11) NOT NULL AUTO_INCREMENT, \
                         `character_id` INT(11) NOT NULL, \
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

hook OnCharacterClearData(playerid)
{
    SelectedCategory{playerid} = -1;
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

hook OnPlayerEditAttachedObj(playerid, response, index, modelid, boneid, Float:fOffsetX, Float:fOffsetY, Float:fOffsetZ, Float:fRotX, Float:fRotY, Float:fRotZ, Float:fScaleX, Float:fScaleY, Float:fScaleZ)
{
    new PlayerObject:selected = SelectedPlayerObject[playerid];
    if(!response)
    {
        // Discard changes
        WearObject_Update(playerid, selected);
    }
    else
    {
        if(fScaleX < 0.1 || fScaleY < 0.1 || fScaleZ < 0.1)
            return WearObject_Update(playerid, selected), SendClientMessage(playerid, COLOR_ERROR, "La grandezza minima è 0.1.");
        
        if(fScaleX > 2.0 || fScaleY > 2.0 || fScaleZ > 2.0)
            return WearObject_Update(playerid, selected), SendClientMessage(playerid, COLOR_ERROR, "La grandezza massima è 2.0.");

        WearObject_SetOffset(selected, fOffsetX, fOffsetY, fOffsetZ);
        WearObject_SetRot(selected, fRotX, fRotY, fRotZ);
        WearObject_SetScale(selected, fScaleX, fScaleY, fScaleZ);
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
    new String:str = @("Oggetto\tStato\tSlot\nAggiungi Nuovo Oggetto\t \t \n"), count = 1;
    for_playerobjects(i : playerid)
    {
        str_append_format(str, "%d\t%s\t%s\n", count, WearObject_GetName(i), Character_IsWearing(playerid, i) ? ("{00FF00}Indossato{FFFFFF}") : ("{FF0000}Non Indossato{FFFFFF}"));
        count++;
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
    if(listitem == 0)
    {
        if(list_size(Character_GetObjectsList(playerid)) >= MAX_OBJECTS_PER_PLAYER)
            return SendFormattedMessage(playerid, COLOR_ERROR, "Hai già raggiunto il limite massimo di oggetti indossabili (%d).", MAX_OBJECTS_PER_PLAYER);
        return Internal_ShowCategories(playerid);
    }
    listitem--;
    SelectedPlayerObject[playerid] = WearObject_Find(playerid, listitem);
    WearObject_Edit(playerid, SelectedPlayerObject[playerid]);
    return 1;
}

Dialog:Dialog_WearObjectEdit(playerid, response, listitem, inputtext[])
{
    if(!response)
        return callcmd::oggetto(playerid, "");
    new 
        PlayerObject:selected = SelectedPlayerObject[playerid],
        index = WearObject_GetIndex(selected);
    switch(listitem)
    {
        case 0:
        {
            if(Character_IsWearing(playerid, selected))
            {
                WearObject_Remove(playerid, selected);
                SendFormattedMessage(playerid, COLOR_GREEN, "Hai rimosso l'oggetto (%s) dallo slot %d.", WearObject_GetName(selected), index+1);
            }
            else
            {
                if(WearObject_Use(playerid, selected))
                {
                    SendClientMessage(playerid, COLOR_ORANGE, "Oggetto indossato.");
                }
            }
        }
        case 1:
        {
            if(Character_IsWearing(playerid, selected))
                return SendClientMessage(playerid, COLOR_ERROR, "Non puoi modificare il modello se lo stai indossando.");
            return Internal_ShowCategories(playerid);
        }
        case 2:
        {
            return Dialog_Show(playerid, Dialog_WearObjectBone, DIALOG_STYLE_LIST, "Cambia osso", "Spina dorsale\nTesta\nBraccio sinistro\nBraccio destro\nMano sinistra\nMano destra\nCoscia sinistra\nCoscia destra\nPiede sinistro\nPiede destro\nPolpaccio destro\nPolpaccio sinistro\nAvambraccio sinistro\nAvambraccio destro\nSpalla (sinistra)\nSpalla (destra)\nCollo\nMascella", "Procedi", "Indietro");
        }
        case 3:
        {
            if(!Character_IsWearing(playerid, selected))
                return SendClientMessage(playerid, COLOR_ERROR, "Devi indossare l'oggetto per poterlo modificare.");
            EditObject_Start(playerid, EDIT_TYPE_PLAYER_OBJECT, index, 0);
        }
        case 4:
        {
            return Dialog_Show(playerid, Dialog_WearObjectColor1, DIALOG_STYLE_LIST, "Colore primario", "Colore corrente\nBlu\nBlu chiaro\nRosso\nRosa\nViola\nVerde\nVerde chiaro\nArancione\nBianco\nNero\nGiallo", "Avanti", "Annulla");
        }
        case 5:
        {
            SendClientMessage(playerid, -1, "Hai cancellato definitivamente l'oggetto.");
            WearObject_Delete(playerid, selected);
        }
    }
    return 1;
}

Dialog:Dialog_WearObjectBone(playerid, response, listitem, inputtext[])
{
    if(!response)
        return callcmd::oggetto(playerid, "");
    SendClientMessage(playerid, COLOR_GREEN, "Hai modificato l'osso dell'oggetto.");
    WearObject_SetBone(SelectedPlayerObject[playerid], listitem+1);
    WearObject_Update(playerid, SelectedPlayerObject[playerid]);
    return 1;
}

Dialog:Dialog_WearObjectCategories(playerid, response, listitem, inputtext[])
{
    if(!response)
        return callcmd::oggetto(playerid, "");
    
    if(listitem == 9 && Faction_GetType(Character_GetFaction(playerid)) != FACTION_TYPE_POLICE)
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questa categoria.");
    
    if(listitem == 10 && Faction_GetType(Character_GetFaction(playerid)) != FACTION_TYPE_MEDICAL)
        return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare questa categoria.");

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
 
    new objectid = list_get(CategoryIndex[playerid], listitem);
    SendFormattedMessage(playerid, -1, "Hai modificato l'oggetto. Stai ora indossando: %s.", PlayerObjectsList[objectid][oName]);

    new PlayerObject:playerObject = CreateWearObject(playerid, PlayerObjectsList[objectid][oName], PlayerObjectsList[objectid][Object], PlayerObjectsList[objectid][DefaultBone]);
    #pragma unused playerObject
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
        return WearObject_Edit(playerid, SelectedPlayerObject[playerid]);
    new color = HexToInt("FFFFFFFF");
    switch(listitem)
    {

        case 1: color = HexToInt("0000ccff");
		case 2: color = HexToInt("4d4dffff");
		case 3: color = HexToInt("ff0000ff");
		case 4: color = HexToInt("ffccffff");
		case 5: color = HexToInt("990099ff");
		case 6: color = HexToInt("009933ff");
		case 7: color = HexToInt("33ff77ff");
		case 8: color = HexToInt("ff6600ff");
		case 9: color = HexToInt("ffffffff");
		case 10: color = HexToInt("000000ff");
		case 11: color = HexToInt("ffff00ff");
    }
    WearObject_SetMaterialColor1(SelectedPlayerObject[playerid], color);
    WearObject_Update(playerid, SelectedPlayerObject[playerid]);
    return Dialog_Show(playerid, Dialog_WearObjectColor2, DIALOG_STYLE_LIST, "Colore secondario", "Colore corrente\nBlu\nBlu chiaro\nRosso\nRosa\nViola\nVerde\nVerde chiaro\nArancione\nBianco\nNero\nGiallo", "Avanti", "Annulla");
}

Dialog:Dialog_WearObjectColor2(playerid, response, listitem, inputtext[])
{
    if(!response)
        return WearObject_Edit(playerid, SelectedPlayerObject[playerid]);
    new color = HexToInt("FFFFFFFF");
    switch(listitem)
    {
        case 1: color = HexToInt("0000ccff");
		case 2: color = HexToInt("4d4dffff");
		case 3: color = HexToInt("ff0000ff");
		case 4: color = HexToInt("ffccffff");
		case 5: color = HexToInt("990099ff");
		case 6: color = HexToInt("009933ff");
		case 7: color = HexToInt("33ff77ff");
		case 8: color = HexToInt("ff6600ff");
		case 9: color = HexToInt("ffffffff");
		case 10: color = HexToInt("000000ff");
		case 11: color = HexToInt("ffff00ff");
    }
    WearObject_SetMaterialColor2(SelectedPlayerObject[playerid], color);
    WearObject_Update(playerid, SelectedPlayerObject[playerid]);
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
        if(PlayerObjectsList[i][Type] != category || PlayerObjectsList[i][Object] == 0)
            continue;
        str += str_format("%s\n", PlayerObjectsList[i][oName]);
        list_add(CategoryIndex[playerid], i);
    }
    return Dialog_Show_s(playerid, Dialog_WearObjectSelect, DIALOG_STYLE_LIST, @("Oggetti"), str, "Seleziona", "Chiudi");
}

stock bool:WearObject_Remove(playerid, PlayerObject:playerObject)
{
    new index = WearObject_GetIndex(playerObject);
    if(index != -1)
    {
        RemovePlayerAttachedObject(playerid, index);
        var_set_cell(Variant:playerObject, Index, -1);
        mysql_pquery_f(gMySQL, "UPDATE `character_wear_objects` SET wearing = '0' WHERE id = '%d'", WearObject_GetID(playerObject));
        return true;
    }
    return false;
}

stock Character_LoadWearObjects(playerid)
{
    PlayerObjects[playerid] = list_new();
    inline OnLoad()
    {
        new 
            rows = cache_num_rows(),
            data[E_PLAYER_CUSTOM_OBJECT_DATA];
        for(new i = 0; i < rows; ++i)
        {
            if(i >= MAX_PLAYER_OBJECTS)
            {
                printf("Bug occurred. i >= MAX_PLAYER_OBJECTS when loading Wearable objects.");
                break;
            }
            cache_get_value_name_int(i, "id", data[ID]);
            cache_get_value_name_int(i, "slot_id", data[Index]);
            cache_get_value_name(i, "name", data[Name]);
            cache_get_value_name_int(i, "model_id", data[ModelId]);
            cache_get_value_name_int(i, "bone", data[Bone]);
            
            cache_get_value_name_float(i, "offset_x", data[X]);
            cache_get_value_name_float(i, "offset_y", data[Y]);
            cache_get_value_name_float(i, "offset_z", data[Z]);
            
            cache_get_value_name_float(i, "rot_x", data[RotX]);
            cache_get_value_name_float(i, "rot_y", data[RotY]);
            cache_get_value_name_float(i, "rot_z", data[RotZ]);

            cache_get_value_name_float(i, "scale_x", data[ScaleX]);
            cache_get_value_name_float(i, "scale_y", data[ScaleY]);
            cache_get_value_name_float(i, "scale_z", data[ScaleZ]);

            cache_get_value_name_int(i, "mat_color_1", data[MaterialColor1]);
            cache_get_value_name_int(i, "mat_color_2", data[MaterialColor2]);

            new equipped = 0;
            cache_get_value_name_int(i, "wearing", equipped);

            data[PlayerID] = playerid;
            
            new PlayerObject:playerObject = PlayerObject:var_new_buf(E_PLAYER_CUSTOM_OBJECT_DATA);
            var_acquire(Variant:playerObject);
            
            var_set_cell(Variant:playerObject, ID, data[ID]);

            var_set_cell(Variant:playerObject, PlayerID, playerid);
            var_set_cell(Variant:playerObject, Index, -1);
            var_set_cells(Variant:playerObject, Name, data[Name]);
            var_set_cell(Variant:playerObject, Bone, data[Bone]);
            var_set_cell(Variant:playerObject, ModelId, data[ModelId]);

            var_set_cell(Variant:playerObject, X, data[X]);
            var_set_cell(Variant:playerObject, Y, data[Y]);
            var_set_cell(Variant:playerObject, Z, data[Z]);

            var_set_cell(Variant:playerObject, RotX, data[RotX]);
            var_set_cell(Variant:playerObject, RotY, data[RotY]);
            var_set_cell(Variant:playerObject, RotZ, data[RotZ]);

            var_set_cell(Variant:playerObject, ScaleX, data[ScaleX]);
            var_set_cell(Variant:playerObject, ScaleY, data[ScaleY]);
            var_set_cell(Variant:playerObject, ScaleZ, data[ScaleZ]);

            var_set_cell(Variant:playerObject, MaterialColor1, data[MaterialColor1]);
            var_set_cell(Variant:playerObject, MaterialColor2, data[MaterialColor2]);

            list_add(PlayerObjects[playerid], playerObject);

            if(equipped == 1)
            {
                WearObject_Use(playerid, playerObject);
            }
        }
    }
    MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT * FROM character_wear_objects WHERE character_id = '%d'", Character_GetID(playerid));
}


// ================================================= accessors ==========================================================

stock WearObject_GetName(PlayerObject:playerObject)
{
    new name[32];
    var_get_md_arr(Variant:playerObject, {Name}, name);
    return name;
}

stock WearObject_GetID(PlayerObject:playerObject)
{
    return var_get(Variant:playerObject, ID);
}

stock WearObject_SetIndex(PlayerObject:playerObject, index)
{
    var_set_cell(Variant:playerObject, Index, index);
}

stock WearObject_GetIndex(PlayerObject:playerObject)
{
    return var_get(Variant:playerObject, Index);
}

stock WearObject_GetModelId(PlayerObject:playerObject)
{
    return var_get(Variant:playerObject, ModelId);
}

stock WearObject_SetBone(PlayerObject:playerObject, boneid)
{
    var_set_cell(Variant:playerObject, Bone, boneid);
    mysql_pquery_f(gMySQL, "UPDATE character_wear_objects SET bone = '%d' WHERE id = '%d';", boneid, WearObject_GetID(playerObject));
}

stock WearObject_GetBone(PlayerObject:playerObject)
{
    return var_get(Variant:playerObject, Bone);
}

stock bool:WearObject_SetOffset(PlayerObject:playerObject, Float:x, Float:y, Float:z)
{
    var_set_cell(Variant:playerObject, X, x);
    var_set_cell(Variant:playerObject, Y, y);
    var_set_cell(Variant:playerObject, Z, z);
    mysql_pquery_f(gMySQL, "UPDATE `character_wear_objects` SET offset_x = '%f', offset_y = '%f', offset_z = '%f' WHERE id = '%d'", x, y, z, WearObject_GetID(playerObject));
    return true;
}

stock bool:WearObject_GetOffset(PlayerObject:playerObject, &Float:x, &Float:y, &Float:z)
{
    x = Float:var_get(Variant:playerObject, X);
    y = Float:var_get(Variant:playerObject, Y);
    z = Float:var_get(Variant:playerObject, Z);
    return true;
}

stock bool:WearObject_SetRot(PlayerObject:playerObject, Float:x, Float:y, Float:z)
{
    var_set_cell(Variant:playerObject, RotX, x);
    var_set_cell(Variant:playerObject, RotY, y);
    var_set_cell(Variant:playerObject, RotZ, z);
    mysql_pquery_f(gMySQL, "UPDATE `character_wear_objects` SET rot_x = '%f', rot_y = '%f', rot_z = '%f' WHERE id = '%d'", x, y, z, WearObject_GetID(playerObject));
    return true;
}

stock bool:WearObject_GetRot(PlayerObject:playerObject, &Float:x, &Float:y, &Float:z)
{
    x = Float:var_get(Variant:playerObject, RotX);
    y = Float:var_get(Variant:playerObject, RotY);
    z = Float:var_get(Variant:playerObject, RotZ);
    return true;
}

stock bool:WearObject_SetScale(PlayerObject:playerObject, Float:x, Float:y, Float:z)
{
    var_set_cell(Variant:playerObject, ScaleX, x);
    var_set_cell(Variant:playerObject, ScaleY, y);
    var_set_cell(Variant:playerObject, ScaleZ, z);
    mysql_pquery_f(gMySQL, "UPDATE `character_wear_objects` SET scale_x = '%f', scale_y = '%f', scale_z = '%f' WHERE id = '%d'", x, y, z, WearObject_GetID(playerObject));
    return true;
}

stock bool:WearObject_GetScale(PlayerObject:playerObject, &Float:x, &Float:y, &Float:z)
{
    x = Float:var_get(Variant:playerObject, ScaleX);
    y = Float:var_get(Variant:playerObject, ScaleY);
    z = Float:var_get(Variant:playerObject, ScaleZ);
    return true;
}

stock Character_IsWearing(playerid, PlayerObject:playerObject)
{
    new index = WearObject_GetIndex(playerObject);
    return index != -1 && var_get(Variant:playerObject, PlayerID) == playerid && IsPlayerAttachedObjectSlotUsed(playerid, index);
}

stock WearObject_SetMaterialColor1(PlayerObject:playerObject, color)
{
    var_set_cell(Variant:playerObject, MaterialColor1, color);
    mysql_pquery_f(gMySQL, "UPDATE character_wear_objects SET mat_color_1 = '%d' WHERE id = '%d';", color, WearObject_GetID(playerObject));
}

stock WearObject_GetMaterialColor1(PlayerObject:playerObject)
{
    return var_get(Variant:playerObject, MaterialColor1);
}

stock WearObject_SetMaterialColor2(PlayerObject:playerObject, color)
{
    var_set_cell(Variant:playerObject, MaterialColor2, color);
    mysql_pquery_f(gMySQL, "UPDATE character_wear_objects SET mat_color_2 = '%d' WHERE id = '%d';", color, WearObject_GetID(playerObject));
}

stock WearObject_GetMaterialColor2(PlayerObject:playerObject)
{
    return var_get(Variant:playerObject, MaterialColor2);
}

//========================================= INTERNALS =====================================================

static bool:WearObject_Update(playerid, PlayerObject:playerObject)
{
    new 
        index = WearObject_GetIndex(playerObject)
    ;
    if(index <= -1)
        return false;
    if(IsPlayerAttachedObjectSlotUsed(playerid, index))
        RemovePlayerAttachedObject(playerid, index);
    
    new Float:x, Float:y, Float:z,
        Float:rotX, Float:rotY, Float:rotZ,
        Float:scaleX, Float:scaleY, Float:scaleZ;
    WearObject_GetOffset(playerObject, x, y, z);
    WearObject_GetRot(playerObject, rotX, rotY, rotZ);
    WearObject_GetScale(playerObject, scaleX, scaleY, scaleZ);
    SetPlayerAttachedObject(playerid, index, WearObject_GetModelId(playerObject), WearObject_GetBone(playerObject), 
    x, y, z, rotX, rotY, rotZ, scaleX, scaleY, scaleZ,
    WearObject_GetMaterialColor1(playerObject), WearObject_GetMaterialColor2(playerObject));
    return true;
}

static WearObject_Edit(playerid, PlayerObject:playerObject)
{
    if(Character_IsWearing(playerid, playerObject))
        Dialog_Show(playerid, Dialog_WearObjectEdit, DIALOG_STYLE_LIST, "Modifica oggetto", "{FF0000}Rimuovi{FFFFFF}\nCambia oggetto\nCambia osso\nCambia posizione\nCambia colori\nCancella", "Avanti", "Indietro");
    else
        Dialog_Show(playerid, Dialog_WearObjectEdit, DIALOG_STYLE_LIST, "Modifica oggetto", "{00FF00}Indossa{FFFFFF}\nCambia oggetto\nCambia osso\nCambia posizione\nCambia colori\nCancella", "Avanti", "Indietro");
    return 1;
}

static bool:WearObject_Delete(playerid, PlayerObject:playerObject)
{
    if(!var_valid(Variant:playerObject))
        return false;
    new List:list = Character_GetObjectsList(playerid), slotid = -1;
    for(new i = 0, j = list_size(list); i < j; ++i)
    {
        if(list_get(list, i) == _:playerObject)
        {
            slotid = i;
            break;
        }
    }
    if(slotid != -1)
    {
        mysql_pquery_f(gMySQL, "DELETE FROM character_wear_objects WHERE id = '%d'", WearObject_GetID(playerObject));
        var_release(Variant:playerObject);
        list_remove(list, slotid);
    }
    return true;
}

static Internal_ShowCategories(playerid)
{
    return Dialog_Show(playerid, Dialog_WearObjectCategories, DIALOG_STYLE_LIST, "Categorie", "Oggetti vari\nBandane\nCappelli\nCappelli 2\nOrologi\nOcchiali\nElmetti\nMaschere\nExtra\nLVMPD\nLVFD", "Avanti", "Indietro");
}

stock static PlayerObject:CreateWearObject(playerid, name[], modelid, bone, Float:x = 0.0, Float:y = 0.0, Float:z = 0.0, Float:rotx = 0.0, Float:roty = 0.0, Float:rotz = 0.0, Float:scalex = 1.0, Float:scaley = 1.0, Float:scalez = 1.0, matcolor1 = 0, matcolor2 = 0)
{
    new PlayerObject:playerObject = PlayerObject:var_new_buf(E_PLAYER_CUSTOM_OBJECT_DATA);
    var_acquire(Variant:playerObject);
    var_set_cell(Variant:playerObject, PlayerID, playerid);
    var_set_cell(Variant:playerObject, Index, -1);
    var_set_cells(Variant:playerObject, Name, name, 32);
    var_set_cell(Variant:playerObject, Bone, bone);
    var_set_cell(Variant:playerObject, ModelId, modelid);

    var_set_cell(Variant:playerObject, X, x);
    var_set_cell(Variant:playerObject, Y, y);
    var_set_cell(Variant:playerObject, Z, z);

    var_set_cell(Variant:playerObject, RotX, rotx);
    var_set_cell(Variant:playerObject, RotY, roty);
    var_set_cell(Variant:playerObject, RotZ, rotz);

    var_set_cell(Variant:playerObject, ScaleX, scalex);
    var_set_cell(Variant:playerObject, ScaleY, scaley);
    var_set_cell(Variant:playerObject, ScaleZ, scalez);

    var_set_cell(Variant:playerObject, MaterialColor1, matcolor1);
    var_set_cell(Variant:playerObject, MaterialColor2, matcolor2);

    list_add(Character_GetObjectsList(playerid), playerObject);
    inline OnInsert()
    {
        var_set_cell(Variant:playerObject, ID, cache_insert_id());

        WearObject_Use(playerid, playerObject);
    }
    MySQL_TQueryInline(gMySQL, using inline OnInsert, "INSERT INTO `character_wear_objects` (character_id, name, model_id, bone, offset_x, offset_y, offset_z, rot_x, rot_y, rot_z, scale_x, scale_y, scale_z, mat_color_1, mat_color_2) \
                                                        VALUES('%d', '%e', '%d', '%d', '%f', '%f', '%f', '%f', '%f', '%f', '%f', '%f', '%f', '%d', '%d')", 
                                                        Character_GetID(playerid), name, modelid, bone, x, y, z, rotx, roty, rotz, scalex, scaley, scalez, matcolor1, matcolor2);
    return playerObject;
}

static GetUnusedAttachedObjectSlot(playerid)
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

static bool:WearObject_Use(playerid, PlayerObject:playerObject)
{
    new index = WearObject_GetIndex(playerObject);
    if(index == -1)
    {
        index = GetUnusedAttachedObjectSlot(playerid);
    }

    if(index == -1) 
        return false;
    
    new Float:x, Float:y, Float:z,
        Float:rotX, Float:rotY, Float:rotZ,
        Float:scaleX, Float:scaleY, Float:scaleZ;
    WearObject_GetOffset(playerObject, x, y, z);
    WearObject_GetRot(playerObject, rotX, rotY, rotZ);
    WearObject_GetScale(playerObject, scaleX, scaleY, scaleZ);
    
    SetPlayerAttachedObject(playerid, index, WearObject_GetModelId(playerObject), WearObject_GetBone(playerObject), x, y, z, rotX, rotY, rotZ, scaleX, scaleY, scaleZ,
                            WearObject_GetMaterialColor1(playerObject), WearObject_GetMaterialColor2(playerObject));

    mysql_pquery_f(gMySQL, "UPDATE `character_wear_objects` SET wearing = '1' WHERE id = '%d'", WearObject_GetID(playerObject));

    WearObject_SetIndex(playerObject, index);
    return true;
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

stock PlayerObject:WearObject_Find(playerid, slotid)
{
    return PlayerObject:list_get(Character_GetObjectsList(playerid), slotid);
}

stock List:Character_GetObjectsList(playerid)
{
    if(!list_valid(PlayerObjects[playerid]))
        PlayerObjects[playerid] = list_new();
    return PlayerObjects[playerid];
}