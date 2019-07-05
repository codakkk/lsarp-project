stock Character_CharacterAMe(playerid, otherid, text[], GLOBAL_TAG_TYPES:...)
{
	new s[148];
	format(s, sizeof(s), "%s %s", text, Character_GetRolePlayName(otherid));
	return Character_AMe(playerid, s);
}

stock Character_Me(playerid, text[], GLOBAL_TAG_TYPES:...)
{
    if(!Character_IsLogged(playerid) || strlen(text) > 256)
	   return 0;
    new 
	   formattedText[256];
    format(formattedText, sizeof(formattedText), text, ___2);

    format(formattedText, sizeof(formattedText), "* %s %s", Character_GetRolePlayName(playerid), formattedText);
    ProxDetector(playerid, 15.0, formattedText, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF); //0xD6C3E3FF
    return 1;
}

stock Character_MeLow(playerid, text[], GLOBAL_TAG_TYPES:...)
{
    if(!Character_IsLogged(playerid) || strlen(text) > 256)
	   return 0;
    
    new String:str = str_format(text, ___2);
    str = str_format("* %s %S", Character_GetRolePlayName(playerid), str);
    ProxDetectorStr(playerid, 4.0, str, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF); //0xD6C3E3FF
    return 1;
}

stock Character_Do(playerid, text[], GLOBAL_TAG_TYPES:...)
{
    if(!Character_IsLogged(playerid) || strlen(text) > 256)
	   return 0;
    
    new String:str = str_format(text, ___2);
    str += str_format(" (( %s ))", Character_GetRolePlayName(playerid));
    ProxDetectorStr(playerid, 20.0, str, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF);
    return 1;
}

stock Character_DoLow(playerid, text[], GLOBAL_TAG_TYPES:...)
{
    if(!Character_IsLogged(playerid) || strlen(text) > 256)
	   return 0;
    
    new String:str = str_format(text, ___2);
    str += str_format(" (( %s ))", Character_GetRolePlayName(playerid));
    ProxDetectorStr(playerid, 4.0, str, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF, 0xD0AEEBFF);
    return 1;
}

stock Player_Info(playerid, text[], bool:forced = false, time = 2500)
{
	if(pInfoBoxShown{playerid} == true && !forced)
		return 1;

	PlayerTextDrawHide(playerid, pInfoText[playerid]);
	PlayerTextDrawSetString(playerid, pInfoText[playerid], text);
	PlayerTextDrawShow(playerid, pInfoText[playerid]);

	SetTimerEx("DeleteInfoBox", time, false, "d", playerid);
	pInfoBoxShown{playerid} = true;

	return 1;
}

stock Player_InfoStr(playerid, String:string, bool:forced = false, time = 2500)
{
	new ptr[1][] = {{}}, size = str_len(string) + 1, Var:var = amx_alloc(size);
	amx_to_ref(var, ptr);
	str_get(string, ptr[0], .size=size);

	new result = Player_Info(playerid, ptr[0], forced, time);

	amx_free(var);
	amx_delete(var);
	return result;
}

forward DeleteInfoBox(playerid); 
public DeleteInfoBox(playerid)
{
	PlayerTextDrawHide(playerid, pInfoText[playerid]);

	pInfoBoxShown{playerid} = false;

	return 1;
}