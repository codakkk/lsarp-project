
#define MAX_FUNCTION_NAME 32
native SendClientMessageStr(playerid, color, AmxString:message) = SendClientMessage;
native SendClientMessageToAllStr(color, AmxString:message) = SendClientMessageToAll;
native ShowPlayerDialog_s1(playerid, dialogid, style, const caption[], AmxString:info, const button1[], const button2[]) = ShowPlayerDialog;
native ShowPlayerDialog_s(playerid, dialogid, style, AmxString:caption, AmxString:info, const button1[], const button2[]) = ShowPlayerDialog;

native mysql_format_s_impl(MySQL:handle, AmxString:query, max_len, const format[], {Float,_}:...) = mysql_format;

native GameTextForPlayerStr(playerid, AmxString:string, time, style) = GameTextForPlayer;

native PlayerTextDrawSetStringStr(playerid, PlayerText:text, AmxString:string) = PlayerTextDrawSetString;
native TextDrawSetStringStr(Text:text, AmxString:string) = TextDrawSetString;

native UpdateDynamic3DTextLabelTextStr(STREAMER_TAG_3D_TEXT_LABEL:id, color, AmxString:text) = UpdateDynamic3DTextLabelText;

native HTTPStr(index, type, url[], AmxString:data, callback[]) = HTTP;

stock mysql_format_s(MySQL:handle, StringTag:dest, const query[], {Float,_}:...)
{
	mysql_format_s_impl(handle, dest, str_len(dest) + 1, query, ___4);
}
