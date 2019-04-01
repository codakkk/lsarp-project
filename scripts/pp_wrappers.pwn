
native SendClientMessageStr(playerid, color, AmxString:message) = SendClientMessage;

native ShowPlayerDialog_s1(playerid, dialogid, style, const caption[], AmxString:info, const button1[], const button2[]) = ShowPlayerDialog;
native ShowPlayerDialog_s(playerid, dialogid, style, AmxString:caption, AmxString:info, const button1[], const button2[]) = ShowPlayerDialog;

native mysql_pquery_s(MySQL:handle, AmxString:query, const callback[] = "", const format[] = "", {Float,_}:...) = mysql_pquery;
native mysql_tquery_s(MySQL:handle, AmxString:query, const callback[] = "", const format[] = "", {Float,_}:...) = mysql_tquery;
native mysql_format_s(MySQL:handle, AmxString:query, max_len, const format[], {Float,_}:...) = mysql_format;

stock mysql_tquery_inline_s(MySQL:handle, String:query, callback:inline_callback, const format[] = "", {Float,_}:...)
{
    new ptr[1][] = {{}}, size = str_len(query) + 1, Var:var = amx_alloc(size);
    amx_to_ref(var, ptr);
    str_get(query, ptr[0], .size=size);

    new new_format[MAX_FUNCTION_NAME] = "";
    strcat(new_format, format);
    new result = mysql_tquery_inline(handle, ptr[0], inline_callback, new_format, ___4);
    amx_free(var);
    amx_delete(var);
    return result;
}

stock mysql_pquery_inline_s(MySQL:handle, String:query, callback:inline_callback, const format[] = "", {Float,_}:...)
{
    new ptr[1][] = {{}}, size = str_len(query) + 1, Var:var = amx_alloc(size);
    amx_to_ref(var, ptr);
    str_get(query, ptr[0], .size=size);

    new new_format[MAX_FUNCTION_NAME] = "";
    strcat(new_format, format);
    new result = mysql_pquery_inline(handle, ptr[0], inline_callback, new_format, ___4);

    amx_free(var);
    amx_delete(var);
    return result;
}
