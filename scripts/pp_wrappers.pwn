
#define MAX_FUNCTION_NAME 32
native SendClientMessageStr(playerid, color, AmxString:message) = SendClientMessage;
native SendClientMessageToAllStr(color, AmxString:message) = SendClientMessageToAll;
native ShowPlayerDialog_s1(playerid, dialogid, style, const caption[], AmxString:info, const button1[], const button2[]) = ShowPlayerDialog;
native ShowPlayerDialog_s(playerid, dialogid, style, AmxString:caption, AmxString:info, const button1[], const button2[]) = ShowPlayerDialog;

native mysql_pquery_s(MySQL:handle, AmxString:query, const callback[] = "", const format[] = "", {Float,_}:...) = mysql_pquery;
native mysql_tquery_s(MySQL:handle, AmxString:query, const callback[] = "", const format[] = "", {Float,_}:...) = mysql_tquery;
native mysql_format_s_impl(MySQL:handle, AmxString:query, max_len, const format[], {Float,_}:...) = mysql_format;

native GameTextForPlayerStr(playerid, AmxString:string, time, style) = GameTextForPlayer;

stock mysql_format_s(MySQL:handle, StringTag:dest, const query[], {Float,_}:...)
{
    mysql_format_s_impl(handle, dest, str_len(dest) + 1, query, ___4);
}
