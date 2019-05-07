// Thanks to Kristo for this easyDialogs.inc edit.
#include <YSI_Coding\y_hooks>
#define MAX_FUNCTION_NAME 32

#define Dialog:%0(%1) forward public Dlg@%0(%1); public Dlg@%0(%1)

#define Dialog_Show(%0,%1, Dialog_Open(%0, #%1,
#define Dialog_Show_s(%0,%1, Dialog_Open_s(%0, #%1,


static DialogName[MAX_PLAYERS][MAX_FUNCTION_NAME char];


stock Dialog_Opened(playerid) {
	return DialogName[playerid]{0} != EOS;
}


stock Dialog_Close(playerid) {
	DialogName[playerid]{0} = EOS;

	return ShowPlayerDialog(playerid, -1, DIALOG_STYLE_MSGBOX, " ", " ", " ", "");
}

stock Dialog_Open(playerid, const function[], style, const caption[], const info[], const button1[], const button2[], {Float, _}:...) 
{
	new name[MAX_FUNCTION_NAME] = "Dlg@";
	strcat(name, function);
	strpack(DialogName[playerid], name);

	new String:string;

	if (isnull(info)) {
		string = str_new(" ");
	} else {
		string = str_format(info, ___7);
	}
	ShowPlayerDialog_s1(playerid, 32700, style, caption, string, button1, button2);
	return 1;
}

// made by CodaKKK
stock Dialog_Open_s(playerid, const function[], style, String:caption, String:info, const button1[], const button2[])
{
	new name[MAX_FUNCTION_NAME] = "Dlg@";
	strcat(name, function);
	strpack(DialogName[playerid], name);

	ShowPlayerDialog_s(playerid, 32700, style, caption, info, button1, button2);
	return 1;
}

hook OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]) {
	for (new i, j = strlen(inputtext); i < j; i++) {
		if (inputtext[i] == '%') {
			inputtext[i] = '#';
		}
	}

	if (dialogid == 32700 && Dialog_Opened(playerid)) {
	   new name[MAX_FUNCTION_NAME];
	   strunpack(name, DialogName[playerid]);

	   Dialog_Close(playerid);

	   CallLocalFunction(name, "ddds", playerid, response, listitem, (!inputtext[0]) ? ("\1") : (inputtext));

	   return Y_HOOKS_BREAK_RETURN_1;
	}

	return Y_HOOKS_CONTINUE_RETURN_1;
}


hook OnPlayerConnect(playerid) {
	DialogName[playerid]{0} = EOS;

	return Y_HOOKS_CONTINUE_RETURN_1;
}