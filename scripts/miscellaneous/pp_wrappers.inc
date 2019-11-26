
#define MAX_FUNCTION_NAME 32
native SendClientMessageStr(playerid, color, AmxString:message) = SendClientMessage;
native SendClientMessageToAllStr(color, AmxString:message) = SendClientMessageToAll;
native ShowPlayerDialog_s1(playerid, dialogid, style, const caption[], AmxString:info, const button1[], const button2[]) = ShowPlayerDialog;
native ShowPlayerDialog_s(playerid, dialogid, style, AmxString:caption, AmxString:info, const button1[], const button2[]) = ShowPlayerDialog;

native GameTextForPlayerStr(playerid, AmxString:string, time, style) = GameTextForPlayer;

native PlayerTextDrawSetStringStr(playerid, PlayerText:text, AmxString:string) = PlayerTextDrawSetString;
native TextDrawSetStringStr(Text:text, AmxString:string) = TextDrawSetString;

native UpdateDynamic3DTextLabelTextStr(STREAMER_TAG_3D_TEXT_LABEL:id, color, AmxString:text) = UpdateDynamic3DTextLabelText;
native STREAMER_TAG_3D_TEXT_LABEL:CreateDynamic3DTextLabelStr(ConstAmxString:text, color, Float:x, Float:y, Float:z, Float:drawdistance, attachedplayer = INVALID_PLAYER_ID, attachedvehicle = INVALID_VEHICLE_ID, testlos = 0, worldid = -1, interiorid = -1, playerid = -1, Float:streamdistance = STREAMER_3D_TEXT_LABEL_SD, STREAMER_TAG_AREA:areaid = STREAMER_TAG_AREA:-1, priority = 0) = CreateDynamic3DTextLabel;
native HTTPStr(index, type, url[], AmxString:data, callback[]) = HTTP;

