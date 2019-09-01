#define inputlength(%1,%2) if(strlen(%1) > %2) %1[%2 - 1] = EOS; if(%1[0] < 0) %1[0] = EOS

stock GetSexName(sex_id)
{
	new name[16];
	if(sex_id == 0)
		format(name, sizeof(name), "Maschio");
	else if(sex_id == 1)
		format(name, sizeof(name), "Femmina");
	else
		format(name, sizeof(name), "Unknown");
	return name;
}

Dialog:DialogNull(playerid, response, listitem, inputtext[])
{
	return 1;
}

stock KickEx(playerid)
{
	defer KickTimer(playerid);
	return 1;
}

timer KickTimer[100](playerid)
{
	Kick(playerid);
}

stock FixName(const src[], dst[])
{
	set(dst, src);
	for(new i = 0, j = MAX_PLAYER_NAME; i < j; ++i)
	{
		if(dst[i] == '_') dst[i] = ' ';
	}
}

stock IsValidRPName(name[],ret_first[],ret_last[])
{
	
	new len = strlen(name),
		point = -1,
		bool:done = false;
	for(new i = 0; i < len; i++)
	{
		if(name[i] == '_')
		{
			if(point != -1) return 0;
			else 
			{
				if(i == 0) return 0;
				point = i + 1;
			}
		} 
		else if(point == -1) 
				ret_first[i] = name[i];
			else 
		{
			ret_last[i - point] = name[i];
			done = true;
		}
		if(name[i] != '_' && (name[i] < 'a' || name[i] > 'z') && (name[i] < 'A' || name[i] > 'Z'))
		{
			return 0;
		}
	}
	if(ret_first[0] < 'A' || ret_first[0] > 'Z') return 0;
	if(ret_last[0] < 'A' || ret_last[0] > 'Z') return 0;
	if(!done) return 0;
	return 1;
}

stock set(dest[], const source[]) 
{
	new count = strlen(source);
	for (new i = 0; i < count; i++) 
	{
		dest[i] = source[i];
	}
	dest[count] = 0;
}

/*stock SendFormattedMessage(playerid, color, form[], {Float, _}: ...) 
{
    #pragma unused form

    static
	   tmp[145]
    ;
    new
	   t1 = playerid,
	   t2 = color
    ;
    const
	   n4 = -4,
	   n16 = -16,
	   size = sizeof tmp
    ;
    #emit stack 28
    #emit push.c size
    #emit push.c tmp
    #emit stack n4
    #emit sysreq.c format
    #emit stack n16

    return SendClientMessage(t1, t2, tmp);
}*/

stock SendFormattedMessage(playerid, color, form[], {AmxString, Float, _}: ...)
{
    new String:string;
    if(isnull(form))
	   string = str_new(" ");
    else
	   string = str_format(form, ___3);
    return SendClientMessageStr(playerid, color, string);
}
#define TIME_ZONE (0)
#define TIME_ZONE_STAMP (2)
stock date( timestamp, _form=1 )
{
    /*
	   ~ convert a Timestamp to a Date.
	   ~ 10.07.2009

	   date( 1247182451 )  will print >> 09.07.2009-23:34:11
	   date( 1247182451, 1) will print >> 09/07/2009, 23:34:11
	   date( 1247182451, 2) will print >> July 09, 2009, 23:34:11
	   date( 1247182451, 3) will print >> 9 Jul 2009, 23:34
    */
    
    timestamp += 3600 * TIME_ZONE_STAMP;
    
    new year=1970, day=0, month=0, hour=0, mins=0, sec=0;

    new days_of_month[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    new names_of_month[12][10] = {"Gennaio", "Febbraio", "Marzo", "Aprile", "Maggio", "Giugno", "Luglio", "Agosto","Settembre", "Ottobre", "Novembre", "Dicembre"};
    new returnstring[32];

    while(timestamp>31622400)
	{
	   timestamp -= 31536000;
	   if ( ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0) ) timestamp -= 86400;
	   year++;
    }

    if ( ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0) )
	   days_of_month[1] = 29;
    else
	   days_of_month[1] = 28;


    while(timestamp>86400)
	{
	   timestamp -= 86400, day++;
	   if(day==days_of_month[month]) day=0, month++;
    }

    while(timestamp>60)
	{
	   timestamp -= 60, mins++;
	   if( mins == 60) mins=0, hour++;
    }

    sec=timestamp;

    switch( _form )
	{
	   case 1: format(returnstring, 31, "%02d/%02d/%d", day+1, month+1, year);
	   case 2: format(returnstring, 31, "%s %02d, %d, %02d:%02d:%02d", names_of_month[month],day+1,year, hour, mins, sec);
	   case 3: format(returnstring, 31, "%d %c%c%c %d, %02d:%02d", day+1,names_of_month[month][0],names_of_month[month][1],names_of_month[month][2], year,hour,mins);
	   case 4: {format(returnstring, 31, "%02d.%02d.%d-%02d:%02d", day+1, month+1, year, hour, mins); print("we");}

	   default: format(returnstring, 31, "%02d.%02d.%d-%02d:%02d:%02d", day+1, month+1, year, hour, mins, sec);
    }

    return returnstring;
}

// Thanks to Slice (Samp)
/*stock memset(aArray[], iValue, iSize = sizeof(aArray)) {
    new
	   iAddress
    ;
    
    // Store the address of the array
    #emit LOAD.S.pri 12
    #emit STOR.S.pri iAddress
    
    // Convert the size from cells to bytes
    iSize *= 4;
    
    // Loop until there is nothing more to fill
    while (iSize > 0) {
	   // I have to do this because the FILL instruction doesn't accept a dynamic number.
	   if (iSize >= 4096) {
		  #emit LOAD.S.alt iAddress
		  #emit LOAD.S.pri iValue
		  #emit FILL 4096
	   
		  iSize    -= 4096;
		  iAddress += 4096;
	   } else if (iSize >= 1024) {
		  #emit LOAD.S.alt iAddress
		  #emit LOAD.S.pri iValue
		  #emit FILL 1024

		  iSize    -= 1024;
		  iAddress += 1024;
	   } else if (iSize >= 256) {
		  #emit LOAD.S.alt iAddress
		  #emit LOAD.S.pri iValue
		  #emit FILL 256

		  iSize    -= 256;
		  iAddress += 256;
	   } else if (iSize >= 64) {
		  #emit LOAD.S.alt iAddress
		  #emit LOAD.S.pri iValue
		  #emit FILL 64

		  iSize    -= 64;
		  iAddress += 64;
	   } else if (iSize >= 16) {
		  #emit LOAD.S.alt iAddress
		  #emit LOAD.S.pri iValue
		  #emit FILL 16

		  iSize    -= 16;
		  iAddress += 16;
	   } else {
		  #emit LOAD.S.alt iAddress
		  #emit LOAD.S.pri iValue
		  #emit FILL 4

		  iSize    -= 4;
		  iAddress += 4;
	   }
    }
    
    // aArray is used, just not by its symbol name
    #pragma unused aArray
}*/

new VehicleNames[][] =
{
    "Landstalker", "Bravura", "Buffalo", "Linerunner", "Perrenial", "Sentinel",
    "Dumper", "Firetruck", "Trashmaster", "Stretch", "Manana", "Infernus",
    "Voodoo", "Pony", "Mule", "Cheetah", "Ambulance", "Leviathan", "Moonbeam",
    "Esperanto", "Taxi", "Washington", "Bobcat", "Whoopee", "BF Injection",
    "Hunter", "Premier", "Enforcer", "Securicar", "Banshee", "Predator", "Bus",
    "Rhino", "Barracks", "Hotknife", "Trailer", "Previon", "Coach", "Cabbie",
    "Stallion", "Rumpo", "RC Bandit", "Romero", "Packer", "Monster", "Admiral",
    "Squalo", "Seasparrow", "Pizzaboy", "Tram", "Trailer", "Turismo", "Speeder",
    "Reefer", "Tropic", "Flatbed", "Yankee", "Caddy", "Solair", "Berkley's RC Van",
    "Skimmer", "PCJ-600", "Faggio", "Freeway", "RC Baron", "RC Raider", "Glendale",
    "Oceanic","Sanchez", "Sparrow", "Patriot", "Quad", "Coastguard", "Dinghy",
    "Hermes", "Sabre", "Rustler", "ZR-350", "Walton", "Regina", "Comet", "BMX",
    "Burrito", "Camper", "Marquis", "Baggage", "Dozer", "Maverick", "News Chopper",
    "Rancher", "FBI Rancher", "Virgo", "Greenwood", "Jetmax", "Hotring", "Sandking",
    "Blista Compact", "Police Maverick", "Boxville", "Benson", "Mesa", "RC Goblin",
    "Hotring Racer A", "Hotring Racer B", "Bloodring Banger", "Rancher", "Super GT",
    "Elegant", "Journey", "Bike", "Mountain Bike", "Beagle", "Cropduster", "Stunt",
    "Tanker", "Roadtrain", "Nebula", "Majestic", "Buccaneer", "Shamal", "Hydra",
    "FCR-900", "NRG-500", "HPV1000", "Cement Truck", "Tow Truck", "Fortune",
    "Cadrona", "FBI Truck", "Willard", "Forklift", "Tractor", "Combine", "Feltzer",
    "Remington", "Slamvan", "Blade", "Freight", "Streak", "Vortex", "Vincent",
    "Bullet", "Clover", "Sadler", "Firetruck", "Hustler", "Intruder", "Primo",
    "Cargobob", "Tampa", "Sunrise", "Merit", "Utility", "Nevada", "Yosemite",
    "Windsor", "Monster", "Monster", "Uranus", "Jester", "Sultan", "Stratium",
    "Elegy", "Raindance", "RC Tiger", "Flash", "Tahoma", "Savanna", "Bandito",
    "Freight Flat", "Streak Carriage", "Kart", "Mower", "Dune", "Sweeper",
    "Broadway", "Tornado", "AT-400", "DFT-30", "Huntley", "Stafford", "BF-400",
    "News Van", "Tug", "Trailer", "Emperor", "Wayfarer", "Euros", "Hotdog", "Club",
    "Freight Box", "Trailer", "Andromada", "Dodo", "RC Cam", "Launch", "Police Car",
    "Police Car", "Police Car", "Police Ranger", "Picador", "S.W.A.T", "Alpha",
    "Phoenix", "Glendale", "Sadler", "Luggage", "Luggage", "Stairs", "Boxville",
    "Tiller", "Utility Trailer"
};

stock Vehicle_GetName(vehicleid)
{
    new string[32];
    format(string,sizeof(string),"%s", VehicleNames[GetVehicleModel(vehicleid) - 400]);
    return string;
}

stock Vehicle_GetNameFromModel(modelid)
{
    new
	   string[32];
    format(string, sizeof(string), "%s", VehicleNames[modelid - 400]);
    return string;
}

stock IsATruck(vehicleid)
{
    new Trucks[] = { 403, 408, 406, 407, 433, 443, 456, 486, 498, 499, 514, 515, 524, 525, 544, 573, 578, 609};
    new model = GetVehicleModel(vehicleid);
    for(new i = 0; i < sizeof(Trucks); i++)
    {
	   if(model == Trucks[i]) return 1;
    }
    return 0;
}

stock IsABus(vehicleid)
{
    new Bus[] = { 431, 437 };
    new model = GetVehicleModel(vehicleid);
    for(new i = 0; i < sizeof(Bus); i++)
    {
	   if(model == Bus[i]) return 1;
    }
    return 0;
}

stock IsABike(carid)
{
    new Bikes[] = { 509, 481, 510 };
    new model = GetVehicleModel(carid);
    for(new i = 0; i < sizeof(Bikes); i++)
    {
	   if(model == Bikes[i]) return 1;
    }
    return 0;
}

stock IsAMotorBike(vehicleid)
{
    new model = GetVehicleModel(vehicleid);
	if(model == 523 || model == 571 || model == 457 || model == 581 || model == 509 || model == 481 || model == 462 || model == 521 || model == 463 || model == 510 || model == 522 || model == 461 || model == 448 || model == 471 || model == 468 || model == 586)
	{
		return 1;
	}
	return 0;
}

stock IsAPlane(vehicleid)
{
    new model = GetVehicleModel(vehicleid);
	if(model == 592 || model == 577 || model == 511 || model == 512 || model == 593 || model == 520 || model == 553 || model == 476 || model == 519 || model == 460 || model == 513)
	{
		return 1;
	}
	return 0;
}

stock IsAHelicopter(vehicleid)
{
    new model = GetVehicleModel(vehicleid);
	if(model == 548 || model == 417 || model == 487 || model == 488 || model == 497 || model == 563 || model == 447 || model == 469)
	{
		return 1;
	}
	return 0;
}

stock IsABoat(vehicleid)
{
	new model = GetVehicleModel(vehicleid);
	if(model == 472 || model == 473 || model == 493 || model == 595 || model == 484 || model == 430 || model == 453 || model == 452 || model == 446 || model == 454)
	{
		return 1;
	}
	return 0;
}

stock ProxDetectorS(Float:radius, playerid, targetid)
{
    if(!IsPlayerConnected(playerid) || !IsPlayerConnected(targetid) || GetPlayerState(targetid) == PLAYER_STATE_SPECTATING)
	   return 0;
    new Float:x, Float:y, Float:z;
    GetPlayerPos(targetid, x, y, z);
    if(Character_GetVirtualWorld(playerid) == Character_GetVirtualWorld(targetid) && IsPlayerInRangeOfPlayer(playerid, targetid, radius))
	   return 1;
    return 0;
}

stock ProxDetectorStr(playerid, Float:radius, String:string, col1, col2, col3, col4, col5, bool:ooc = false, bool:receiveSelf = true)
{
    /*new ptr[1][] = {{}}, size = str_len(string) + 1, Var:var = amx_alloc(size);
    amx_to_ref(var, ptr);
    str_get(string, ptr[0], .size=size);*/

	new buffer[256];
	str_get(string, buffer);
    new result = ProxDetector(playerid, radius, /*ptr[0]*/buffer, col1, col2, col3, col4, col5, ooc, receiveSelf);

    /*amx_free(var);
    amx_delete(var);*/
    return result;
}

stock ProxDetector(playerid, Float:radius, const string[], col1, col2, col3, col4, col5, bool:ooc = false, bool:receiveSelf = true)
{
    if(!IsPlayerConnected(playerid) || !Character_IsLogged(playerid))
	   return 0;
    new 
	   int = GetPlayerInterior(playerid), vw = Character_GetVirtualWorld(playerid),
	   Float:x, Float:y, Float:z;/* Float:dist,
	   Float:ox, Float:oy, Float:oz,
	   Float:tx, Float:ty, Float:tz;*/
	GetPlayerPos(playerid, x, y, z);
	foreach(new i : Player)
	{
		if(!receiveSelf && i == playerid)
			continue;
		if(GetPlayerInterior(i) != int || Character_GetVirtualWorld(i) != vw)
			continue;
		if( (!Account_HasOOCEnabled(i) || !Account_HasOOCEnabledForPlayer(i, playerid)) && ooc)
			continue;
		new Float:distance = GetPlayerDistanceFromPoint(i, x, y, z);
		if(distance < radius/16) SendTwoLinesMessage(i, col1, string);
		else if(distance < radius/8) SendTwoLinesMessage(i, col2, string);
		else if(distance < radius/4) SendTwoLinesMessage(i, col3, string);
		else if(distance < radius/2) SendTwoLinesMessage(i, col4, string);
		else if(distance < radius) SendTwoLinesMessage(i, col5, string);
	}
    return 1;
}

stock SendTwoLinesMessageStr(playerid, color, String:string)
{
	/*new ptr[1][] = {{}}, size = str_len(string) + 1, Var:var = amx_alloc(size);
    amx_to_ref(var, ptr);
    str_get(string, ptr[0], .size=size);*/

	new buffer[256];
	str_get(string, buffer);
    new result = SendTwoLinesMessage(playerid, color, /*ptr[0]*/ buffer);

    /*amx_free(var);
    amx_delete(var);*/
	return result;
}

stock SendTwoLinesMessage(playerid, color, const message[], GLOBAL_TAG_TYPES:...)
{
	#define maxRowLength 120
	//static const maxRowLength = 120; // 120 characters.

	new string[256];
	format(string, sizeof(string), message, ___3);
	new len = strlen(string);
	if(len > maxRowLength + 4)
	{
		new tmp1[maxRowLength + 4], tmp2[maxRowLength + 4];
		
		strmid(tmp2, string, maxRowLength, len);
		strins(tmp2, ".. ", 0);
		strmid(tmp1, string, 0, maxRowLength);
		strins(tmp1, " ..", maxRowLength);
		SendClientMessage(playerid, color, tmp1);
		SendClientMessage(playerid, color, tmp2);
	}
	else
	{
		SendClientMessage(playerid, color, string);
	}
    return 1;
}

// I must increase two lines characters.
/*stock SendTwoLinesMessage(playerid, color, const message[], GLOBAL_TAG_TYPES:...)
{
	new string[256];
	format(string, sizeof(string), message, ___3);
	new len = strlen(string);
	if(len > 84)
	{
		new tmp1[85], tmp2[85];
		
		strmid(tmp2, string, 80, (len > 149 ? 149 : len));
		strins(tmp2, ".. ", 0, 84);
		strmid(tmp1, string, 0, 80);
		strins(tmp1, " ..", 80, 83);
		SendClientMessage(playerid, color, tmp1);
		SendClientMessage(playerid, color, tmp2);
	}
	else
	{
		SendClientMessage(playerid, color, string);
	}
    return 1;
}*/

stock SendTwoLinesMessageToAllStr(color, String:string)
{
	/*new ptr[1][] = {{}}, size = str_len(string) + 1, Var:var = amx_alloc(size);
    amx_to_ref(var, ptr);
    str_get(string, ptr[0], .size=size);*/

	new buffer[256];
	str_get(string, buffer);
    new result = SendTwoLinesMessageToAll(color, /*ptr[0]*/ buffer);

    /*amx_free(var);
    amx_delete(var);*/
	return result;
}

stock SendTwoLinesMessageToAll(color, const text[])
{
	new string[256];
    format(string, sizeof(string), text, ___1);
    new
	    len = strlen(string);
	if(len > 84)
	{
	    new
	        tmp1[85],
			tmp2[85];
		strmid(tmp2, string, 80, (len > 149 ? 149 : len));
		strins(tmp2, ".. ", 0, 84);
		strmid(tmp1, string, 0, 80);
		strins(tmp1, " ..", 80, 83);
		SendClientMessageToAll(color, tmp1);
		SendClientMessageToAll(color, tmp2);
		return 1;
	}
	else
		return SendClientMessageToAll(color, string);
}

stock IsValidEMail(email[]) 
{
	new len = strlen(email);
	new cstate = 0;
	new i;
	for(i = 0; i < len; i++)
	{
		if ((cstate==0 || cstate==1) && (email[i]>='A' && email[i]<='Z') || (email[i]>='a' && email[i]<='z') || (email[i]>='0' && email[i]<='9') || (email[i]=='.')  || (email[i]=='-')  || (email[i]=='_'))
		{
		} else {
		// Ok no A..Z,a..z,0..9,_,.,-
			if ((cstate==0) &&(email[i]=='@')) {
				// its an @ after the name, ok state=1;
				cstate=1;
			} else {
				// Its stuff which is not allowed
				return false;
			}
		}
	}
	if (cstate < 1) return false;
	if (len < 6) return false;
	new mail[65], domain[16];
	printf("domain: %s", domain); // probably stremid between @ and . ?
	// A toplevel domain has only 3 to 4 signs :-)
	if ((email[len-3] == '.') || (email[len-4] == '.') || (email[len-5] == '.')) return true;
	return false;
}

stock DestroyDynamic3DTextLabelEx(&Text3D:labelid)
{
	DestroyDynamic3DTextLabel(labelid);
	labelid = Text3D:INVALID_3DTEXT_ID; 
	return 1;
}