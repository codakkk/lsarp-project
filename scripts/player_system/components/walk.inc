#include <YSI_Coding\y_hooks>

static
	PlayerWalkStyle[MAX_PLAYERS],
	Timer:pWalkTimer[MAX_PLAYERS];

static enum 
{
	WALK_DEFAULT = 0,
 	WALK_NORMAL,
	WALK_PED,
	WALK_GANGSTA,
	WALK_GANGSTA2,
	WALK_OLD,
	WALK_FAT_OLD,
	WALK_FAT,
	WALK_LADY,
	WALK_LADY2,
	WALK_WHORE,
	WALK_WHORE2,
	WALK_DRUNK,
	WALK_BLIND,

	WALK_LAST
}

hook OnCharacterLoad(playerid)
{
	inline OnLoad()
	{
		cache_get_value_index_int(0, 0, PlayerWalkStyle[playerid]);
	}
	MySQL_TQueryInline(gMySQL, using inline OnLoad, "SELECT walk_style FROM `characters` WHERE id = '%d'", Character_GetID(playerid));
	return 1;
}

static Internal_Save(playerid)
{
	mysql_tquery_f(gMySQL, "UPDATE `characters` SET walk_style = '%d' WHERE id = '%d'", PlayerWalkStyle[playerid], Character_GetID(playerid));
	return 1;
}

flags:stilecamminata(CMD_ALIVE_USER);
CMD:stilecamminata(playerid, params[])
{
	new style;
	if(sscanf(params, "d", style) || (style-- && (style < 0 || style >= WALK_LAST)))
		return SendFormattedMessage(playerid, COLOR_COMMAND_ERROR, "USO: /stilecamminata <style (1 - %d)> (0 per disattivare).", WALK_LAST);
	Character_SetWalkingStyle(playerid, style);
	SendFormattedMessage(playerid, COLOR_GREEN, "Hai cambiato il tuo stile camminata. Stile: %d", style+1);
	return 1;
}

stock Character_GetWalkingStyle(playerid)
{
	return PlayerWalkStyle[playerid];
}

stock Character_SetWalkingStyle(playerid, style)
{
	PlayerWalkStyle[playerid] = style;
	Internal_Save(playerid);
	return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(!Character_IsAlive(playerid))
		return Y_HOOKS_CONTINUE_RETURN_0;
	if (((newkeys & KEY_WALK && newkeys & KEY_UP) || (newkeys & KEY_WALK && newkeys & KEY_DOWN) || (newkeys & KEY_WALK && newkeys & KEY_LEFT) || (newkeys & KEY_WALK && newkeys & KEY_RIGHT))
		|| ((oldkeys & KEY_WALK && newkeys & KEY_UP) || (oldkeys & KEY_WALK && newkeys & KEY_DOWN) || (oldkeys & KEY_WALK && newkeys & KEY_LEFT) || (oldkeys & KEY_WALK && newkeys & KEY_RIGHT))
		|| ((newkeys & KEY_WALK && oldkeys & KEY_UP) || (newkeys & KEY_WALK && oldkeys & KEY_DOWN) || (newkeys & KEY_WALK && oldkeys & KEY_LEFT) || (newkeys & KEY_WALK && oldkeys & KEY_RIGHT))
		&& GetPlayerState(playerid) == PLAYER_STATE_ONFOOT)
	{
		pWalkTimer[playerid] = defer Timer_WalkAnim(playerid);
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

timer Timer_WalkAnim[200](playerid) 
{
	if(IsPlayerInAnyVehicle(playerid)) return 1;
	new keys, updown, leftright;
	GetPlayerKeys(playerid,keys,updown,leftright);
	if(keys & KEY_JUMP) return 1;
	if (Character_GetWalkingStyle(playerid) == WALK_NORMAL)
	{
 	 	if ((keys & KEY_WALK && updown & KEY_UP) || (keys & KEY_WALK && updown & KEY_DOWN) || (keys & KEY_WALK && leftright & KEY_LEFT) || (keys & KEY_WALK && leftright & KEY_RIGHT))
		{
		   	stop pWalkTimer[playerid];
			ApplyAnimation(playerid,"PED","WALK_player",4.1,1,1,1,1,1);
			pWalkTimer[playerid] = defer Timer_WalkAnim(playerid);
		}
		else
		{
			ApplyAnimation(playerid,"PED","WALK_player",4.0,0,0,0,0,1);
			stop pWalkTimer[playerid];
		}
	}
	else if (Character_GetWalkingStyle(playerid) == WALK_PED)
	{
 	 	if ((keys & KEY_WALK && updown & KEY_UP) || (keys & KEY_WALK && updown & KEY_DOWN) || (keys & KEY_WALK && leftright & KEY_LEFT) || (keys & KEY_WALK && leftright & KEY_RIGHT))
		{
		   	stop pWalkTimer[playerid];
			ApplyAnimation(playerid,"PED","WALK_civi",4.1,1,1,1,1,1);
			pWalkTimer[playerid] = defer Timer_WalkAnim(playerid);
		}
		else 
		{
			ApplyAnimation(playerid,"PED","WALK_civi",4.0,0,0,0,0,1); 
			stop pWalkTimer[playerid];
		}
	}
 	else if (Character_GetWalkingStyle(playerid) == WALK_GANGSTA)
	{
 	 	if ((keys & KEY_WALK && updown & KEY_UP) || (keys & KEY_WALK && updown & KEY_DOWN) || (keys & KEY_WALK && leftright & KEY_LEFT) || (keys & KEY_WALK && leftright & KEY_RIGHT))
		{
		   	stop pWalkTimer[playerid];
			ApplyAnimation(playerid,"PED","WALK_gang1",4.1,1,1,1,1,1);
			pWalkTimer[playerid] = defer Timer_WalkAnim(playerid);
		}
		else 
		{
			ApplyAnimation(playerid,"PED","WALK_gang1",4.0,0,0,0,0,1); 
			stop pWalkTimer[playerid];
		}
	}
	else if (Character_GetWalkingStyle(playerid) == WALK_GANGSTA2)
	{
 	 	if ((keys & KEY_WALK && updown & KEY_UP) || (keys & KEY_WALK && updown & KEY_DOWN) || (keys & KEY_WALK && leftright & KEY_LEFT) || (keys & KEY_WALK && leftright & KEY_RIGHT))
		{
		   	stop pWalkTimer[playerid];
			ApplyAnimation(playerid,"PED","WALK_gang2",4.1,1,1,1,1,1);
			pWalkTimer[playerid] = defer Timer_WalkAnim(playerid);
		}
		else { ApplyAnimation(playerid,"PED","WALK_gang2",4.0,0,0,0,0,1); stop pWalkTimer[playerid]; }
	}
	else if (Character_GetWalkingStyle(playerid) == WALK_OLD)
	{
 	 	if ((keys & KEY_WALK && updown & KEY_UP) || (keys & KEY_WALK && updown & KEY_DOWN) || (keys & KEY_WALK && leftright & KEY_LEFT) || (keys & KEY_WALK && leftright & KEY_RIGHT))
		{
		   	stop pWalkTimer[playerid];
			ApplyAnimation(playerid,"PED","WALK_old",4.1,1,1,1,1,1);
			pWalkTimer[playerid] = defer Timer_WalkAnim(playerid);
		}
		else { ApplyAnimation(playerid,"PED","WALK_old",4.0,0,0,0,0,1); stop pWalkTimer[playerid]; }
	}
	else if (Character_GetWalkingStyle(playerid) == WALK_FAT_OLD)
	{
 	 	if ((keys & KEY_WALK && updown & KEY_UP) || (keys & KEY_WALK && updown & KEY_DOWN) || (keys & KEY_WALK && leftright & KEY_LEFT) || (keys & KEY_WALK && leftright & KEY_RIGHT))
		{
		   	stop pWalkTimer[playerid];
			ApplyAnimation(playerid,"PED","WALK_fatold",4.1,1,1,1,1,1);
			pWalkTimer[playerid] = defer Timer_WalkAnim(playerid);
		}
		else { ApplyAnimation(playerid,"PED","WALK_fatold",4.0,0,0,0,0,1); stop pWalkTimer[playerid]; }
	}
	else if (Character_GetWalkingStyle(playerid) == WALK_FAT)
	{
 	 	if ((keys & KEY_WALK && updown & KEY_UP) || (keys & KEY_WALK && updown & KEY_DOWN) || (keys & KEY_WALK && leftright & KEY_LEFT) || (keys & KEY_WALK && leftright & KEY_RIGHT))
		{
		   	stop pWalkTimer[playerid];
			ApplyAnimation(playerid,"PED","WALK_fat",4.1,1,1,1,1,1);
			pWalkTimer[playerid] = defer Timer_WalkAnim(playerid);
		}
		else { ApplyAnimation(playerid,"PED","WALK_fat",4.0,0,0,0,0,1); stop pWalkTimer[playerid]; }
	}
	else if (Character_GetWalkingStyle(playerid) == WALK_LADY)
	{
 	 	if ((keys & KEY_WALK && updown & KEY_UP) || (keys & KEY_WALK && updown & KEY_DOWN) || (keys & KEY_WALK && leftright & KEY_LEFT) || (keys & KEY_WALK && leftright & KEY_RIGHT))
		{
		   	stop pWalkTimer[playerid];
			ApplyAnimation(playerid,"PED","WOMAN_walknorm",4.1,1,1,1,1,1);
			pWalkTimer[playerid] = defer Timer_WalkAnim(playerid);
		}
		else { ApplyAnimation(playerid,"PED","WOMAN_walknorm",4.0,0,0,0,0,1); stop pWalkTimer[playerid]; }
	}
	else if (Character_GetWalkingStyle(playerid) == WALK_LADY2)
	{
 	 	if ((keys & KEY_WALK && updown & KEY_UP) || (keys & KEY_WALK && updown & KEY_DOWN) || (keys & KEY_WALK && leftright & KEY_LEFT) || (keys & KEY_WALK && leftright & KEY_RIGHT))
		{
		   	stop pWalkTimer[playerid];
			ApplyAnimation(playerid,"PED","WOMAN_walkbusy",4.1,1,1,1,1,1);
			pWalkTimer[playerid] = defer Timer_WalkAnim(playerid);
		}
		else { ApplyAnimation(playerid,"PED","WOMAN_walkbusy",4.0,0,0,0,0,1); stop pWalkTimer[playerid]; }
	}
	else if (Character_GetWalkingStyle(playerid) == WALK_WHORE)
	{
 	 	if ((keys & KEY_WALK && updown & KEY_UP) || (keys & KEY_WALK && updown & KEY_DOWN) || (keys & KEY_WALK && leftright & KEY_LEFT) || (keys & KEY_WALK && leftright & KEY_RIGHT))
		{
		   	stop pWalkTimer[playerid];
			ApplyAnimation(playerid,"PED","WOMAN_walkpro",4.1,1,1,1,1,1);
			pWalkTimer[playerid] = defer Timer_WalkAnim(playerid);
		}
		else { ApplyAnimation(playerid,"PED","WOMAN_walkpro",4.0,0,0,0,0,1); stop pWalkTimer[playerid]; }
	}
	else if (Character_GetWalkingStyle(playerid) == WALK_WHORE2)
	{
 	 	if ((keys & KEY_WALK && updown & KEY_UP) || (keys & KEY_WALK && updown & KEY_DOWN) || (keys & KEY_WALK && leftright & KEY_LEFT) || (keys & KEY_WALK && leftright & KEY_RIGHT))
		{
		   	stop pWalkTimer[playerid];
			ApplyAnimation(playerid,"PED","WOMAN_walksexy",4.1,1,1,1,1,1);
			pWalkTimer[playerid] = defer Timer_WalkAnim(playerid);
		}
		else { ApplyAnimation(playerid,"PED","WOMAN_walksexy",4.0,0,0,0,0,1); stop pWalkTimer[playerid]; }
	}
	else if (Character_GetWalkingStyle(playerid) == WALK_DRUNK)
	{
 	 	if ((keys & KEY_WALK && updown & KEY_UP) || (keys & KEY_WALK && updown & KEY_DOWN) || (keys & KEY_WALK && leftright & KEY_LEFT) || (keys & KEY_WALK && leftright & KEY_RIGHT))
		{
		   	stop pWalkTimer[playerid];
			ApplyAnimation(playerid,"PED","WALK_drunk",4.1,1,1,1,1,1);
			pWalkTimer[playerid] = defer Timer_WalkAnim(playerid);
		}
		else { ApplyAnimation(playerid,"PED","WALK_drunk",4.0,0,0,0,0,1); stop pWalkTimer[playerid]; }
	}
	else if (Character_GetWalkingStyle(playerid) == WALK_BLIND)
	{
 	 	if ((keys & KEY_WALK && updown & KEY_UP) || (keys & KEY_WALK && updown & KEY_DOWN) || (keys & KEY_WALK && leftright & KEY_LEFT) || (keys & KEY_WALK && leftright & KEY_RIGHT))
		{
		   	stop pWalkTimer[playerid];
			ApplyAnimation(playerid,"PED","Walk_Wuzi",4.1,1,1,1,1,1);
			pWalkTimer[playerid] = defer Timer_WalkAnim(playerid);
		}
		else { ApplyAnimation(playerid,"PED","Walk_Wuzi",4.0,0,0,0,0,1); stop pWalkTimer[playerid]; }
	}
	return 1;
}