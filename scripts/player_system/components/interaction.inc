


stock Character_CharacterAMe(playerid, otherid, text[], GLOBAL_TAG_TYPES:...)
{
	new s[148];
	format(s, sizeof(s), "%s %s.", text, Character_GetRolePlayName(otherid));
	return Character_AMe(playerid, s);
}

stock Character_CharacterMe(playerid, otherid, text[], GLOBAL_TAG_TYPES:...)
{
    if(!Character_IsLogged(playerid) || strlen(text) > 256)
	   return 0;
    new 
	   formattedText[256];
    format(formattedText, sizeof(formattedText), text, ___3);

    format(formattedText, sizeof(formattedText), "* %s %s %s.", Character_GetRolePlayName(playerid), formattedText, Character_GetRolePlayName(otherid));
    ProxDetector(playerid, 15.0, formattedText, 0xC2A2DAFF, 0xC2A2DAFF, 0xC2A2DAFF, 0xC2A2DAFF, 0xC2A2DAFF); //0xD6C3E3FF
    return 1;
}

stock Character_Me(playerid, text[], GLOBAL_TAG_TYPES:...)
{
    if(!Character_IsLogged(playerid) || strlen(text) > 256)
	   return 0;
    new 
	   formattedText[256];
    format(formattedText, sizeof(formattedText), text, ___2);

    format(formattedText, sizeof(formattedText), "* %s %s", Character_GetRolePlayName(playerid), formattedText);
    ProxDetector(playerid, 15.0, formattedText, 0xC2A2DAFF, 0xC2A2DAFF, 0xC2A2DAFF, 0xC2A2DAFF, 0xC2A2DAFF); //0xD6C3E3FF
    return 1;
}

stock Character_MeLow(playerid, text[], GLOBAL_TAG_TYPES:...)
{
    if(!Character_IsLogged(playerid) || strlen(text) > 256)
	   return 0;
    
    new String:str = str_format(text, ___2);
    str = str_format("* %s %S", Character_GetRolePlayName(playerid), str);
    ProxDetectorStr(playerid, 4.0, str, 0xC2A2DAFF, 0xC2A2DAFF, 0xC2A2DAFF, 0xC2A2DAFF, 0xC2A2DAFF); //0xD6C3E3FF
    return 1;
}

stock Character_Do(playerid, text[], GLOBAL_TAG_TYPES:...)
{
    if(!Character_IsLogged(playerid) || strlen(text) > 256)
	   return 0;
    
    new String:str = str_format(text, ___2);
    str += str_format(" (( %s ))", Character_GetRolePlayName(playerid));
    ProxDetectorStr(playerid, 20.0, str, 0xC2A2DAFF, 0xC2A2DAFF, 0xC2A2DAFF, 0xC2A2DAFF, 0xC2A2DAFF);
    return 1;
}

stock Character_DoLow(playerid, text[], GLOBAL_TAG_TYPES:...)
{
    if(!Character_IsLogged(playerid) || strlen(text) > 256)
	   return 0;
    
    new String:str = str_format(text, ___2);
    str += str_format(" (( %s ))", Character_GetRolePlayName(playerid));
    ProxDetectorStr(playerid, 4.0, str, 0xC2A2DAFF, 0xC2A2DAFF, 0xC2A2DAFF, 0xC2A2DAFF, 0xC2A2DAFF);
    return 1;
}