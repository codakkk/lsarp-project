
CMD:animlist(playerid, params[])
{
	SendClientMessage(playerid, COLOR_GREEN, "== Lista Animazioni ==");
	SendClientMessage(playerid, COLOR_WHITE, "/dance - /handsup - /bat - /bar - /wash - /lay - /workout - /blowjob - /bomb - /stopanim");
	SendClientMessage(playerid, COLOR_WHITE, "/carry - /crack - /sleep - /jump - /deal - /eating - /puke - /gsign - /chat - /crouch");
	SendClientMessage(playerid, COLOR_WHITE, "/goggles - /spray - /throw - /swipe - /office - /kiss - /knife - /cpr - /scratch - /point");
	SendClientMessage(playerid, COLOR_WHITE, "/cheer - /wave - /strip - /smoke - /reload - /taichi - /wank - /cower - /skate - /drunk");
	SendClientMessage(playerid, COLOR_WHITE, "/cry - /tired - /sit - /crossarms - /fucku - /walk - /piss - /fall - /lean - /attenti");

	return 1;
}

flags:stopanim(CMD_ALIVE_USER);
CMD:stopanim(playerid, params[])
{
	if(!AnimationCheck(playerid))return 1;

	ClearAnimationsEx(playerid);
	return 1;
}
alias:stopanim("sa");

flags:dance(CMD_ALIVE_USER);
CMD:dance(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/dance <1 - 15>");

	if(type < 1 || type > 15)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: SetPlayerSpecialAction(playerid, SPECIAL_ACTION_DANCE1);
		case 2: SetPlayerSpecialAction(playerid, SPECIAL_ACTION_DANCE2);
		case 3: SetPlayerSpecialAction(playerid, SPECIAL_ACTION_DANCE3);
		case 4: SetPlayerSpecialAction(playerid, SPECIAL_ACTION_DANCE4);
		case 5: ApplyAnimationEx(playerid, "DANCING", "dance_loop", 4.1, 1, 0, 0, 0, 0, 1);
		case 6: ApplyAnimationEx(playerid, "DANCING", "DAN_Left_A", 4.1, 1, 0, 0, 0, 0, 1);
		case 7: ApplyAnimationEx(playerid, "DANCING", "DAN_Right_A", 4.1, 1, 0, 0, 0, 0, 1);
		case 8: ApplyAnimationEx(playerid, "DANCING", "DAN_Loop_A", 4.1, 1, 0, 0, 0, 0, 1);
		case 9: ApplyAnimationEx(playerid, "DANCING", "DAN_Up_A", 4.1, 1, 0, 0, 0, 0, 1);
		case 10: ApplyAnimationEx(playerid, "DANCING", "DAN_Down_A", 4.1, 1, 0, 0, 0, 0, 1);
		case 11: ApplyAnimationEx(playerid, "DANCING", "dnce_M_a", 4.1, 1, 0, 0, 0, 0, 1);
		case 12: ApplyAnimationEx(playerid, "DANCING", "dnce_M_e", 4.1, 1, 0, 0, 0, 0, 1);
		case 13: ApplyAnimationEx(playerid, "DANCING", "dnce_M_b", 4.1, 1, 0, 0, 0, 0, 1);
		case 15: ApplyAnimationEx(playerid, "DANCING", "dnce_M_c", 4.1, 1, 0, 0, 0, 0, 1);
	}

	return 1;
}
flags:handsup(CMD_ALIVE_USER);
CMD:handsup(playerid, params[])
{
	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	SetPlayerSpecialAction(playerid, SPECIAL_ACTION_HANDSUP);

	return 1;
}

flags:piss(CMD_ALIVE_USER);
CMD:piss(playerid, params[])
{
	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	SetPlayerSpecialAction(playerid, SPECIAL_ACTION_PISSING);

	return 1;
}
flags:bat(CMD_ALIVE_USER);
CMD:bat(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/bat <1 - 5>");

	if(type < 1 || type > 5)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimation(playerid, "BASEBALL", "Bat_1", 4.1, 0, 1, 1, 0, 0, 1);
		case 2: ApplyAnimation(playerid, "BASEBALL", "Bat_2", 4.1, 0, 1, 1, 0, 0, 1);
		case 3: ApplyAnimation(playerid, "BASEBALL", "Bat_3", 4.1, 0, 1, 1, 0, 0, 1);
		case 4: ApplyAnimation(playerid, "BASEBALL", "Bat_4", 4.1, 0, 0, 0, 0, 0, 1);
		case 5: ApplyAnimationEx(playerid, "BASEBALL", "Bat_IDLE", 4.1, 1, 0, 0, 0, 0, 1);
	}

	return 1;
}
flags:bar(CMD_ALIVE_USER);
CMD:bar(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/bar <1 - 8>");

	if(type < 1 || type > 8)	return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimation(playerid, "BAR", "Barserve_bottle", 4.1, 0, 0, 0, 0, 0, 1);
		case 2: ApplyAnimation(playerid, "BAR", "Barserve_give", 4.1, 0, 0, 0, 0, 0, 1);
		case 3: ApplyAnimation(playerid, "BAR", "Barserve_glass", 4.1, 0, 0, 0, 0, 0, 1);
		case 4: ApplyAnimation(playerid, "BAR", "Barserve_in", 4.1, 0, 0, 0, 0, 0, 1);
		case 5: ApplyAnimation(playerid, "BAR", "Barserve_order", 4.1, 0, 0, 0, 0, 0, 1);
		case 6: ApplyAnimationEx(playerid, "BAR", "BARman_idle", 4.1, 1, 0, 0, 0, 0, 1);
		case 7: ApplyAnimationEx(playerid, "BAR", "dnk_stndM_loop", 4.1, 0, 0, 0, 0, 0, 1);
		case 8: ApplyAnimationEx(playerid, "BAR", "dnk_stndF_loop", 4.1, 0, 0, 0, 0, 0, 1);
	}

	return 1;
}
flags:wash(CMD_ALIVE_USER);
CMD:wash(playerid, params[])
{
	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	ApplyAnimation(playerid, "BD_FIRE", "wash_up", 4.1, 0, 0, 0, 0, 0, 1);

	return 1;
}
flags:lay(CMD_ALIVE_USER);
CMD:lay(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/lay <1 - 5>");

	if(type < 1 || type > 5)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimationEx(playerid, "BEACH", "bather", 4.1, 1, 0, 0, 0, 0, 1);
		case 2: ApplyAnimationEx(playerid, "BEACH", "Lay_Bac_Loop", 4.1, 1, 0, 0, 0, 0, 1);
		case 3: ApplyAnimationEx(playerid, "BEACH", "ParkSit_M_loop", 4.1, 1, 0, 0, 0, 0, 1);
		case 4: ApplyAnimationEx(playerid, "BEACH", "ParkSit_W_loop", 4.1, 1, 0, 0, 0, 0, 1);
		case 5: ApplyAnimationEx(playerid, "BEACH", "SitnWait_loop_W", 4.1, 1, 0, 0, 0, 0, 1);
	}

	return 1;
}
flags:workout(CMD_ALIVE_USER);
CMD:workout(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/workout <1 - 7>");

	if(type < 1 || type > 7)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimation(playerid, "benchpress", "gym_bp_celebrate", 4.1, 0, 0, 0, 0, 0, 1);
		case 2: ApplyAnimationEx(playerid, "benchpress", "gym_bp_down", 4.1, 0, 0, 0, 1, 0, 1);
		case 3: ApplyAnimation(playerid, "benchpress", "gym_bp_getoff", 4.1, 0, 0, 0, 0, 0, 1);
		case 4: ApplyAnimationEx(playerid, "benchpress", "gym_bp_geton", 4.1, 0, 0, 0, 1, 0, 1);
		case 5: ApplyAnimationEx(playerid, "benchpress", "gym_bp_up_A", 4.1, 0, 0, 0, 1, 0, 1);
		case 6: ApplyAnimationEx(playerid, "benchpress", "gym_bp_up_B", 4.1, 0, 0, 0, 1, 0, 1);
		case 7: ApplyAnimationEx(playerid, "benchpress", "gym_bp_up_smooth", 4.1, 0, 0, 0, 1, 0, 1);
	}

	return 1;
}

flags:blowjob(CMD_ALIVE_USER);
CMD:blowjob(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/blowjob <1 - 4>");

	if(type < 1 || type > 4)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimationEx(playerid, "BLOWJOBZ", "BJ_COUCH_LOOP_W", 4.1, 1, 0, 0, 0, 0, 1);
		case 2: ApplyAnimationEx(playerid, "BLOWJOBZ", "BJ_COUCH_LOOP_P", 4.1, 1, 0, 0, 0, 0, 1);
		case 3: ApplyAnimationEx(playerid, "BLOWJOBZ", "BJ_STAND_LOOP_W", 4.1, 1, 0, 0, 0, 0, 1);
		case 4: ApplyAnimationEx(playerid, "BLOWJOBZ", "BJ_STAND_LOOP_P", 4.1, 1, 0, 0, 0, 0, 1);
	}

	return 1;
}
flags:bomb(CMD_ALIVE_USER);
CMD:bomb(playerid, params[])
{
	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	ApplyAnimation(playerid, "BOMBER", "BOM_Plant_Loop", 4.1, 0, 0, 0, 0, 0, 1);

	return 1;
}

flags:carry(CMD_ALIVE_USER);
CMD:carry(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/carry <1 - 6>");

	if(type < 1 || type > 6) return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimation(playerid, "CARRY", "liftup", 4.1, 0, 0, 0, 0, 0, 1);
		case 2: ApplyAnimation(playerid, "CARRY", "liftup05", 4.1, 0, 0, 0, 0, 0, 1);
		case 3: ApplyAnimation(playerid, "CARRY", "liftup105", 4.1, 0, 0, 0, 0, 0, 1);
		case 4: ApplyAnimation(playerid, "CARRY", "putdwn", 4.1, 0, 0, 0, 0, 0, 1);
		case 5: ApplyAnimation(playerid, "CARRY", "putdwn05", 4.1, 0, 0, 0, 0, 0, 1);
		case 6: ApplyAnimation(playerid, "CARRY", "putdwn105", 4.1, 0, 0, 0, 0, 0, 1);
	}

	return 1;
}

flags:crack(CMD_ALIVE_USER);
CMD:crack(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
			SendClientMessage(playerid, COLOR_GREY, "/crack <1 - 6>");

	if(type < 1 || type > 6)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimationEx(playerid, "CRACK", "crckdeth1", 4.1, 0, 0, 0, 1, 0, 1);
		case 2: ApplyAnimationEx(playerid, "CRACK", "crckdeth2", 4.1, 1, 0, 0, 0, 0, 1);
		case 3: ApplyAnimationEx(playerid, "CRACK", "crckdeth3", 4.1, 0, 0, 0, 1, 0, 1);
		case 4: ApplyAnimationEx(playerid, "CRACK", "crckidle1", 4.1, 0, 0, 0, 1, 0, 1);
		case 5: ApplyAnimationEx(playerid, "CRACK", "crckidle2", 4.1, 0, 0, 0, 1, 0, 1);
		case 6: ApplyAnimationEx(playerid, "CRACK", "crckidle3", 4.1, 0, 0, 0, 1, 0, 1);
	}

	return 1;
}

// flags:sleep(CMD_ALIVE_USER);
CMD:sleep(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/sleep <1-2>");

	if(type < 1 || type > 2)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimationEx(playerid, "CRACK", "crckdeth4", 4.1, 0, 0, 0, 1, 0, 1);
		case 2: ApplyAnimationEx(playerid, "CRACK", "crckidle4", 4.1, 0, 0, 0, 1, 0, 1);
	}

	return 1;
}

flags:jump(CMD_ALIVE_USER);
CMD:jump(playerid, params[])
{
	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	ApplyAnimation(playerid, "DODGE", "Crush_Jump", 4.1, 0, 1, 1, 0, 0, 1);

	return 1;
}

flags:deal(CMD_ALIVE_USER);
CMD:deal(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/deal <1 - 7>");

	if(type < 1 || type > 7)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimation(playerid, "DEALER", "DEALER_DEAL", 4.1, 0, 0, 0, 0, 0, 1);
		case 2: ApplyAnimation(playerid, "DEALER", "DRUGS_BUY", 4.1, 0, 0, 0, 0, 0, 1);
		case 3: ApplyAnimation(playerid, "DEALER", "shop_pay", 4.1, 0, 0, 0, 0, 0, 1);
		case 4: ApplyAnimationEx(playerid, "DEALER", "DEALER_IDLE_01", 4.1, 1, 0, 0, 0, 0, 1);
		case 5: ApplyAnimationEx(playerid, "DEALER", "DEALER_IDLE_02", 4.1, 1, 0, 0, 0, 0, 1);
		case 6: ApplyAnimationEx(playerid, "DEALER", "DEALER_IDLE_03", 4.1, 1, 0, 0, 0, 0, 1);
		case 7: ApplyAnimation(playerid, "GANGS", "Invite_Yes", 4.1, 0, 0, 0, 0, 0, 1);
	}

	return 1;
}

flags:eating(CMD_ALIVE_USER);
CMD:eating(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/eating <1 - 3>");

	if(type < 1 || type > 3)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimation(playerid, "FOOD", "EAT_Burger", 4.1, 0, 0, 0, 0, 0, 1);
		case 2: ApplyAnimation(playerid, "FOOD", "EAT_Chicken", 4.1, 0, 0, 0, 0, 0, 1);
		case 3: ApplyAnimation(playerid, "FOOD", "EAT_Pizza", 4.1, 0, 0, 0, 0, 0, 1);
	}

	return 1;
}

flags:puke(CMD_ALIVE_USER);
CMD:puke(playerid, params[])
{
	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	ApplyAnimation(playerid, "FOOD", "EAT_Vomit_P", 4.1, 0, 0, 0, 0, 0, 1);

	return 1;
}

flags:crouch(CMD_ALIVE_USER);
CMD:crouch(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/crouch <1 - 2>");

	if(type < 1 || type > 2)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimation(playerid, "CAMERA", "camstnd_to_camcrch", 4.1, 0, 1, 1, 1, 0);
	 	case 2: ApplyAnimation(playerid, "CAMERA", "camcrch_cmon", 4.1, 0, 1, 1, 1, 0);
	}

	return 1;
}

flags:gsign(CMD_ALIVE_USER);
CMD:gsign(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/gsign <1 - 10>");

	if(type < 1 || type > 10)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimation(playerid, "GHANDS", "gsign1", 4.1, 0, 0, 0, 0, 0, 1);
		case 2: ApplyAnimation(playerid, "GHANDS", "gsign1LH", 4.1, 0, 0, 0, 0, 0, 1);
		case 3: ApplyAnimation(playerid, "GHANDS", "gsign2", 4.1, 0, 0, 0, 0, 0, 1);
		case 4: ApplyAnimation(playerid, "GHANDS", "gsign2LH", 4.1, 0, 0, 0, 0, 0, 1);
		case 5: ApplyAnimation(playerid, "GHANDS", "gsign3", 4.1, 0, 0, 0, 0, 0, 1);
		case 6: ApplyAnimation(playerid, "GHANDS", "gsign3LH", 4.1, 0, 0, 0, 0, 0, 1);
		case 7: ApplyAnimation(playerid, "GHANDS", "gsign4", 4.1, 0, 0, 0, 0, 0, 1);
		case 8: ApplyAnimation(playerid, "GHANDS", "gsign4LH", 4.1, 0, 0, 0, 0, 0, 1);
		case 9: ApplyAnimation(playerid, "GHANDS", "gsign5", 4.1, 0, 0, 0, 0, 0, 1);
		case 10: ApplyAnimation(playerid, "GHANDS", "gsign5", 4.1, 0, 0, 0, 0, 0, 1);
		//case 11: ApplyAnimation(playerid, "GHANDS", "gsign5LH", 4.1, 0, 0, 0, 0, 0, 1);
		//case 11: ApplyAnimation(playerid, "GANGS", "Invite_No", 4.1, 0, 0, 0, 0, 0, 1);
		//case 11: ApplyAnimation(playerid, "GANGS", "Invite_Yes", 4.1, 0, 0, 0, 0, 0, 1);
		//case 12: ApplyAnimation(playerid, "GANGS", "prtial_gngtlkD", 4.1, 0, 0, 0, 0, 0, 1);
		//case 13: ApplyAnimation(playerid, "GANGS", "smkcig_prtl", 4.1, 0, 0, 0, 0, 0, 1);
	}

	return 1;
}

flags:no(CMD_ALIVE_USER);
CMD:no(playerid, params[])
{
	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");
	ApplyAnimation(playerid, "GANGS", "Invite_No", 4.1, 0, 0, 0, 0, 0, 1);
	return 1;
}

flags:chat(CMD_ALIVE_USER);
CMD:chat(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/chat <1 - 8>");

	if(type < 1 || type > 8)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
	   case 1: ApplyAnimation(playerid, "GANGS", "prtial_gngtlkA", 3.1,1,1,1,1,1);
	   case 2: ApplyAnimation(playerid, "GANGS", "prtial_gngtlkB", 3.1,1,1,1,1,1);
	   case 3: ApplyAnimation(playerid, "GANGS", "prtial_gngtlkC", 3.1,1,1,1,1,1);
	   case 4: ApplyAnimation(playerid, "GANGS", "prtial_gngtlkD", 3.1,1,1,1,1,1);
	   case 5: ApplyAnimation(playerid, "GANGS", "prtial_gngtlkE", 3.1,1,1,1,1,1);
	   case 6: ApplyAnimation(playerid, "GANGS", "prtial_gngtlkF", 3.1,1,1,1,1,1);
	   case 7: ApplyAnimation(playerid, "GANGS", "prtial_gngtlkG", 3.1,1,1,1,1,1);
	   case 8: ApplyAnimation(playerid, "GANGS", "prtial_gngtlkH", 3.1,1,1,1,1,1);
	}

	return 1;
}

flags:goggles(CMD_ALIVE_USER);
CMD:goggles(playerid, params[])
{
	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	ApplyAnimation(playerid, "goggles", "goggles_put_on", 4.1, 0, 0, 0, 0, 0, 1);

	return 1;
}

flags:spray(CMD_ALIVE_USER);
CMD:spray(playerid, params[])
{
	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

 	ApplyAnimationEx(playerid, "GRAFFITI", "spraycan_fire", 4.1, 1, 0, 0, 0, 0, 1);

	return 1;
}

flags:throw(CMD_ALIVE_USER);
CMD:throw(playerid, params[])
{
	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	ApplyAnimation(playerid, "GRENADE", "WEAPON_throw", 4.1, 0, 0, 0, 0, 0, 1);

	return 1;
}

flags:swipe(CMD_ALIVE_USER);
CMD:swipe(playerid, params[])
{
	if(!AnimationCheck(playerid))return
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	ApplyAnimation(playerid, "HEIST9", "Use_SwipeCard", 4.1, 0, 0, 0, 0, 0, 1);

	return 1;
}

flags:office(CMD_ALIVE_USER);
CMD:office(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/office <1 - 6>");

	if(type < 1 || type > 6)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimationEx(playerid, "INT_OFFICE", "OFF_Sit_Bored_Loop", 4.1, 1, 0, 0, 0, 0, 1);
		case 2: ApplyAnimationEx(playerid, "INT_OFFICE", "OFF_Sit_Crash", 4.1, 1, 0, 0, 0, 0, 1);
		case 3: ApplyAnimationEx(playerid, "INT_OFFICE", "OFF_Sit_Drink", 4.1, 1, 0, 0, 0, 0, 1);
		case 4: ApplyAnimationEx(playerid, "INT_OFFICE", "OFF_Sit_Read", 4.1, 1, 0, 0, 0, 0, 1);
		case 5: ApplyAnimationEx(playerid, "INT_OFFICE", "OFF_Sit_Type_Loop", 4.1, 1, 0, 0, 0, 0, 1);
		case 6: ApplyAnimationEx(playerid, "INT_OFFICE", "OFF_Sit_Watch", 4.1, 1, 0, 0, 0, 0, 1);
	}

	return 1;
}

flags:kiss(CMD_ALIVE_USER);
CMD:kiss(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/kiss <1 - 6>");

	if(type < 1 || type > 6)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimation(playerid, "KISSING", "Grlfrd_Kiss_01", 4.1, 0, 0, 0, 0, 0, 1);
		case 2: ApplyAnimation(playerid, "KISSING", "Grlfrd_Kiss_02", 4.1, 0, 0, 0, 0, 0, 1);
		case 3: ApplyAnimation(playerid, "KISSING", "Grlfrd_Kiss_03", 4.1, 0, 0, 0, 0, 0, 1);
		case 4: ApplyAnimation(playerid, "KISSING", "Playa_Kiss_01", 4.1, 0, 0, 0, 0, 0, 1);
		case 5: ApplyAnimation(playerid, "KISSING", "Playa_Kiss_02", 4.1, 0, 0, 0, 0, 0, 1);
		case 6: ApplyAnimation(playerid, "KISSING", "Playa_Kiss_03", 4.1, 0, 0, 0, 0, 0, 1);
	}

	return 1;
}

flags:knife(CMD_ALIVE_USER);
CMD:knife(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/knife <1 - 8>");

	if(type < 1 || type > 8)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimation(playerid, "KNIFE", "knife_1", 4.1, 0, 1, 1, 0, 0, 1);
		case 2: ApplyAnimation(playerid, "KNIFE", "knife_2", 4.1, 0, 1, 1, 0, 0, 1);
		case 3: ApplyAnimation(playerid, "KNIFE", "knife_3", 4.1, 0, 1, 1, 0, 0, 1);
		case 4: ApplyAnimation(playerid, "KNIFE", "knife_4", 4.1, 0, 1, 1, 0, 0, 1);
		case 5: ApplyAnimationEx(playerid, "KNIFE", "WEAPON_knifeidle", 4.1, 1, 0, 0, 0, 0, 1);
		case 6: ApplyAnimation(playerid, "KNIFE", "KILL_Knife_Player", 4.1, 0, 0, 0, 0, 0, 1);
		case 7: ApplyAnimation(playerid, "KNIFE", "KILL_Knife_Ped_Damage", 4.1, 0, 0, 0, 0, 0, 1);
		case 8: ApplyAnimation(playerid, "KNIFE", "KILL_Knife_Ped_Die", 4.1, 0, 0, 0, 0, 0, 1);
	}

	return 1;
}

flags:cpr(CMD_ALIVE_USER);
CMD:cpr(playerid, params[])
{
	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	ApplyAnimation(playerid, "MEDIC", "CPR", 4.1, 0, 0, 0, 0, 0, 1);

	return 1;
}

flags:scratch(CMD_ALIVE_USER);
CMD:scratch(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/scratch <1 - 5>");

	if(type < 1 || type > 5)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimationEx(playerid, "SCRATCHING", "scdldlp", 4.1, 1, 0, 0, 0, 0, 1);
		case 2: ApplyAnimationEx(playerid, "SCRATCHING", "scdlulp", 4.1, 1, 0, 0, 0, 0, 1);
		case 3: ApplyAnimationEx(playerid, "SCRATCHING", "scdrdlp", 4.1, 1, 0, 0, 0, 0, 1);
		case 4: ApplyAnimationEx(playerid, "SCRATCHING", "scdrulp", 4.1, 1, 0, 0, 0, 0, 1);
		case 5: ApplyAnimationEx(playerid,"MISC","Scratchballs_01", 4.0, 1, 0, 0, 0, 0);
	}

	return 1;
}

flags:point(CMD_ALIVE_USER);
CMD:point(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/point <1 - 4>");

	if(type < 1 || type > 4)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimationEx(playerid, "PED", "ARRESTgun", 4.1, 0, 0, 0, 1, 0, 1);
		case 2: ApplyAnimationEx(playerid, "SHOP", "ROB_Loop_Threat", 4.1, 1, 0, 0, 0, 0, 1);
		case 3: ApplyAnimationEx(playerid, "ON_LOOKERS", "point_loop", 4.1, 1, 0, 0, 0, 0, 1);
		case 4: ApplyAnimationEx(playerid, "ON_LOOKERS", "Pointup_loop", 4.1, 1, 0, 0, 0, 0, 1);
	}

	return 1;
}
alias:point("aim");

flags:cheer(CMD_ALIVE_USER);
CMD:cheer(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/cheer <1 - 8>");

	if(type < 1 || type > 8)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimation(playerid, "ON_LOOKERS", "shout_01", 4.1, 0, 0, 0, 0, 0, 1);
		case 2: ApplyAnimation(playerid, "ON_LOOKERS", "shout_02", 4.1, 0, 0, 0, 0, 0, 1);
		case 3: ApplyAnimation(playerid, "ON_LOOKERS", "shout_in", 4.1, 0, 0, 0, 0, 0, 1);
		case 4: ApplyAnimationEx(playerid, "RIOT", "RIOT_ANGRY_B", 4.1, 1, 0, 0, 0, 0, 1);
		case 5: ApplyAnimation(playerid, "RIOT", "RIOT_CHANT", 4.1, 0, 0, 0, 0, 0, 1);
		case 6: ApplyAnimation(playerid, "RIOT", "RIOT_shout", 4.1, 0, 0, 0, 0, 0, 1);
		case 7: ApplyAnimation(playerid, "STRIP", "PUN_HOLLER", 4.1, 0, 0, 0, 0, 0, 1);
		case 8: ApplyAnimation(playerid, "OTB", "wtchrace_win", 4.1, 0, 0, 0, 0, 0, 1);
	}

	return 1;
}

flags:strip(CMD_ALIVE_USER);
CMD:strip(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/strip <1 - 7>");

	if(type < 1 || type > 7)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimationEx(playerid, "STRIP", "strip_A", 4.1, 1, 0, 0, 0, 0, 1);
		case 2: ApplyAnimationEx(playerid, "STRIP", "strip_B", 4.1, 1, 0, 0, 0, 0, 1);
		case 3: ApplyAnimationEx(playerid, "STRIP", "strip_C", 4.1, 1, 0, 0, 0, 0, 1);
		case 4: ApplyAnimationEx(playerid, "STRIP", "strip_D", 4.1, 1, 0, 0, 0, 0, 1);
		case 5: ApplyAnimationEx(playerid, "STRIP", "strip_E", 4.1, 1, 0, 0, 0, 0, 1);
		case 6: ApplyAnimationEx(playerid, "STRIP", "strip_F", 4.1, 1, 0, 0, 0, 0, 1);
		case 7: ApplyAnimationEx(playerid, "STRIP", "strip_G", 4.1, 1, 0, 0, 0, 0, 1);
	}

	return 1;
}

flags:wave(CMD_ALIVE_USER);
CMD:wave(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/wave <1 - 4>");

	if(type < 1 || type > 4)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimation(playerid, "PED", "endchat_03", 4.1, 0, 0, 0, 0, 0, 1);
		case 2: ApplyAnimation(playerid, "KISSING", "gfwave2", 4.1, 0, 0, 0, 0, 0, 1);
		case 3: ApplyAnimationEx(playerid, "ON_LOOKERS", "wave_loop", 4.1, 1, 0, 0, 0, 0, 1);
		case 4: ApplyAnimation(playerid, "GHANDS", "gsign5LH", 4.1, 0, 0, 0, 0, 0, 1);
	}

	return 1;
}

flags:smoke(CMD_ALIVE_USER);
CMD:smoke(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/smoke <1 - 4>");

	if(type < 1 || type > 4)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimation(playerid, "SMOKING", "M_smk_drag", 4.1, 0, 0, 0, 0, 0, 1);
		case 2: ApplyAnimationEx(playerid, "SMOKING", "M_smklean_loop", 4.1, 1, 0, 0, 0, 0, 1);
		case 3: ApplyAnimation(playerid, "SMOKING", "M_smkstnd_loop", 4.1, 0, 0, 0, 0, 0, 1);
		case 4: ApplyAnimation(playerid, "GANGS", "smkcig_prtl", 4.1, 0, 0, 0, 0, 0, 1);
	}

	return 1;
}

flags:reload(CMD_ALIVE_USER);
CMD:reload(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/reload <1 - 4>");

	if(type < 1 || type > 4)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimation(playerid, "BUDDY", "buddy_reload", 4.1, 0, 0, 0, 0, 0, 1);
		case 2: ApplyAnimation(playerid, "UZI", "UZI_reload", 4.1, 0, 0, 0, 0, 0, 1);
		case 3: ApplyAnimation(playerid, "COLT45", "colt45_reload", 4.1, 0, 0, 0, 0, 0, 1);
		case 4: ApplyAnimation(playerid, "RIFLE", "rifle_load", 4.1, 0, 0, 0, 0, 0, 1);
	}

	return 1;
}

flags:taichi(CMD_ALIVE_USER);
CMD:taichi(playerid, params[])
{
	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	ApplyAnimationEx(playerid, "PARK", "Tai_Chi_Loop", 4.1, 1, 0, 0, 0, 0, 1);

	return 1;
}

flags:wank(CMD_ALIVE_USER);
CMD:wank(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/wank <1 - 3>");

	if(type < 1 || type > 3)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimationEx(playerid, "PAULNMAC", "wank_loop", 4.1, 1, 0, 0, 0, 0, 1);
		case 2: ApplyAnimation(playerid, "PAULNMAC", "wank_in", 4.1, 0, 0, 0, 0, 0, 1);
		case 3: ApplyAnimation(playerid, "PAULNMAC", "wank_out", 4.1, 0, 0, 0, 0, 0, 1);
	}

	return 1;
}

flags:cower(CMD_ALIVE_USER);
CMD:cower(playerid, params[])
{
	if(!AnimationCheck(playerid))return
			SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	ApplyAnimationEx(playerid, "PED", "cower", 4.1, 0, 0, 0, 1, 0, 1);

	return 1;
}

flags:skate(CMD_ALIVE_USER);
CMD:skate(playerid, params[])
{
	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	ApplyAnimationEx(playerid, "SKATE", "skate_idle", 4.1, 1, 0, 0, 0, 0, 1);

	return 1;
}

flags:drunk(CMD_ALIVE_USER);
CMD:drunk(playerid, params[])
{
	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	ApplyAnimationEx(playerid, "PED", "WALK_drunk", 4.1, 1, 1, 1, 1, 1, 1);

	return 1;
}

flags:cry(CMD_ALIVE_USER);
CMD:cry(playerid, params[])
{
	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	ApplyAnimationEx(playerid, "GRAVEYARD", "mrnF_loop", 4.1, 1, 0, 0, 0, 0, 1);

	return 1;
}

flags:tired(CMD_ALIVE_USER);
CMD:tired(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/tired <1 - 2>");

	if(type < 1 || type > 2)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimationEx(playerid, "PED", "IDLE_tired", 4.1, 1, 0, 0, 0, 0, 1);
		case 2: ApplyAnimationEx(playerid, "FAT", "IDLE_tired", 4.1, 1, 0, 0, 0, 0, 1);
	}

	return 1;
}

flags:sit(CMD_ALIVE_USER);
CMD:sit(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/sit <1 - 6>");

	if(type < 1 || type > 6)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimationEx(playerid, "CRIB", "PED_Console_Loop", 4.1, 1, 0, 0, 0, 0);
		case 2: ApplyAnimationEx(playerid, "INT_HOUSE", "LOU_In", 4.1, 0, 0, 0, 1, 0);
		case 3: ApplyAnimationEx(playerid, "MISC", "SEAT_LR", 4.1, 1, 0, 0, 0, 0);
		case 4: ApplyAnimationEx(playerid, "MISC", "Seat_talk_01", 4.1, 1, 0, 0, 0, 0);
		case 5: ApplyAnimationEx(playerid, "MISC", "Seat_talk_02", 4.1, 1, 0, 0, 0, 0);
		case 6: ApplyAnimationEx(playerid, "ped", "SEAT_down", 4.1, 0, 0, 0, 1, 0);
	}

	return 1;
}

flags:crossarms(CMD_ALIVE_USER);
CMD:crossarms(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/crossarms <1 - 4>");

	if(type < 1 || type > 4)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimationEx(playerid, "COP_AMBIENT", "Coplook_loop", 4.1, 0, 1, 1, 1, 0, 1);
		case 2: ApplyAnimationEx(playerid, "GRAVEYARD", "prst_loopa", 4.1, 1, 0, 0, 0, 0, 1);
		case 3: ApplyAnimationEx(playerid, "GRAVEYARD", "mrnM_loop", 4.1, 1, 0, 0, 0, 0, 1);
		case 4: ApplyAnimationEx(playerid, "DEALER", "DEALER_IDLE", 4.1, 0, 1, 1, 1, 0, 1);
	}

	return 1;
}

flags:fucku(CMD_ALIVE_USER);
CMD:fucku(playerid, params[])
{
	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	ApplyAnimation(playerid, "PED", "fucku", 4.1, 0, 0, 0, 0, 0);

	return 1;
}

flags:walk(CMD_ALIVE_USER);
CMD:walk(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/walk <1 - 16>");

	if(type < 1 || type > 16)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
		case 1: ApplyAnimationEx(playerid, "FAT", "FatWalk", 4.1, 1, 1, 1, 1, 1, 1);
		case 2: ApplyAnimationEx(playerid, "MUSCULAR", "MuscleWalk", 4.1, 1, 1, 1, 1, 1, 1);
		case 3: ApplyAnimationEx(playerid, "PED", "WALK_armed", 4.1, 1, 1, 1, 1, 1, 1);
		case 4: ApplyAnimationEx(playerid, "PED", "WALK_civi", 4.1, 1, 1, 1, 1, 1, 1);
		case 5: ApplyAnimationEx(playerid, "PED", "WALK_fat", 4.1, 1, 1, 1, 1, 1, 1);
		case 6: ApplyAnimationEx(playerid, "PED", "WALK_fatold", 4.1, 1, 1, 1, 1, 1, 1);
		case 7: ApplyAnimationEx(playerid, "PED", "WALK_gang1", 4.1, 1, 1, 1, 1, 1, 1);
		case 8: ApplyAnimationEx(playerid, "PED", "WALK_gang2", 4.1, 1, 1, 1, 1, 1, 1);
		case 9: ApplyAnimationEx(playerid, "PED", "WALK_player", 4.1, 1, 1, 1, 1, 1, 1);
		case 10: ApplyAnimationEx(playerid, "PED", "WALK_old", 4.1, 1, 1, 1, 1, 1, 1);
		case 11: ApplyAnimationEx(playerid, "PED", "WALK_wuzi", 4.1, 1, 1, 1, 1, 1, 1);
		case 12: ApplyAnimationEx(playerid, "PED", "WOMAN_walkbusy", 4.1, 1, 1, 1, 1, 1, 1);
		case 13: ApplyAnimationEx(playerid, "PED", "WOMAN_walkfatold", 4.1, 1, 1, 1, 1, 1, 1);
		case 14: ApplyAnimationEx(playerid, "PED", "WOMAN_walknorm", 4.1, 1, 1, 1, 1, 1, 1);
		case 15: ApplyAnimationEx(playerid, "PED", "WOMAN_walksexy", 4.1, 1, 1, 1, 1, 1, 1);
		case 16: ApplyAnimationEx(playerid, "PED", "WOMAN_walkshop", 4.1, 1, 1, 1, 1, 1, 1);
	}

	return 1;
}

flags:fall(CMD_ALIVE_USER);
CMD:fall(playerid, params[])
{
	new type;

	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	if(sscanf(params, "d", type))return
		SendClientMessage(playerid, COLOR_GREY, "/fall <1 - 5>");

	if(type < 1 || type > 5)return
		SendClientMessage(playerid, COLOR_ERROR, "Animazione inesistente.");

	switch(type)
	{
	   case 1: ApplyAnimationEx(playerid, "KNIFE", "KILL_Knife_Ped_Die", 4.0, 0, 1, 1, 1, 0, 1);
	   case 2: ApplyAnimationEx(playerid, "PED", "KO_shot_face", 4.0, 0, 1, 1, 1, 0, 1);
	   case 3: ApplyAnimationEx(playerid, "PED", "KO_shot_stom", 4.0, 0, 1, 1, 1, 0, 1);
	   case 4: ApplyAnimationEx(playerid, "PED", "BIKE_fallR", 4.1, 0, 1, 1, 1, 0, 1);
	   case 5: ApplyAnimationEx(playerid, "PED", "BIKE_fall_off", 4.1, 0, 1, 1, 1, 0, 1);
	}

	return 1;
}

flags:lean(CMD_ALIVE_USER);
CMD:lean(playerid, params[])
{
	if(!AnimationCheck(playerid))return
		SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");

	ApplyAnimationEx(playerid,"GANGS","leanIDLE", 4.0, 1, 0, 0, 0, 0);

	return 1;
}

flags:attenti(CMD_ALIVE_USER);
CMD:attenti(playerid, params[])
{
	if(!AnimationCheck(playerid))
		return SendClientMessage(playerid, COLOR_ERROR, "Non puoi utilizzare un'animazione al momento.");
	ApplyAnimationEx(playerid, "RAPPING","RAP_A_OUT", 4.1, 0, 0, 0, 0, 0);
	return 1;
}