
#include <pickup_system\core>
#include <building_system\core>

#include <YSI_Coding\y_hooks>

hook OnPlayerPickUpElmPickup(playerid, pickupid, elementid, E_ELEMENT_TYPE:type)
{
    if(type == ELEMENT_TYPE_BUILDING_ENTRANCE)
    {
		new buildingid = elementid,
			String:string;
		if(Account_GetAdminLevel(playerid) > 1)
		{
			string += str_format("~r~ID:~w~ %d~n~", elementid);
		}
		if(Building_IsOwnable(buildingid))
		{
			if(Building_GetOwnerID(buildingid) != 0)
			{
				new bname[MAX_BUILDING_NAME];
				Building_GetName(buildingid, bname);
				//Building_GetOwnerName(buildingid, ownerName);
				string += str_format("~y~%s~n~~w~Proprietario: ~y~%s~w~.~n~", bname, Character_GetNameFromDatabase(Building_GetOwnerID(buildingid), true));
				if(Building_GetOwnerID(buildingid) == Character_GetID(playerid))
				{
					if(Player_HasHotKeysEnabled(playerid))
						string += @("Premi ~b~~k~~SNEAK_ABOUT~~w~ (/biz) per il menu.~n~");
					else
						string += @("Digita ~b~/biz~w~ per il menu.~n~");
				}
			}
			else
			{
				new bname[MAX_BUILDING_NAME];
				Building_GetName(buildingid, bname);
				string += str_format("~y~%s~n~~g~Prezzo:~w~ $%d~n~", bname, Building_GetPrice(buildingid));
			}
		}
		else
		{
			new bname[MAX_BUILDING_NAME];
			Building_GetName(buildingid, bname);
			string += str_format("~y~%s~n~", bname);
		}
		GameTextForPlayerStr(playerid, string, 2000, 5);
	}
	/*else if(type == ELEMENT_TYPE_HOUSE_ENTRANCE)
	{
		new houseid = elementid, String:string;
		if(Account_GetAdminLevel(playerid) > 1)
		{
			string += str_format("~r~ID:~w~ %d~n~", houseid);
		}
		if(House_IsOwned(houseid))
		{
			//static name[MAX_PLAYER_NAME];
			
			string += str_format("~y~Proprietario: ~w~%s~n~", Character_GetNameFromDatabase(House_GetOwnerID(houseid), true));
			if(House_GetOwnerID(houseid) == Character_GetID(playerid))
			{
				if(Player_HasHotKeysEnabled(playerid))
					string += @("Premi ~b~~k~~SNEAK_ABOUT~~w~ (/casa) per il menu.~n~");
				else
					string += @("Digita ~b~/casa~w~ per il menu.~n~");
			}
		}
		else
		{
			string += str_format("~g~In vendita!~n~~g~Prezzo: ~w~$%d~n~", House_GetPrice(houseid));
		}
		string += str_format("~r~Interno:~w~ %d~n~", House_GetInteriorID(houseid));
		GameTextForPlayerStr(playerid, string, 2000, 5);
	}*/
    return 1;
}