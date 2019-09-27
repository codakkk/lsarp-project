
#include <pickup_system\core>
#include <building_system\core>

#include <YSI_Coding\y_hooks>

hook OnPlayerPickUpElmPickup(playerid, pickupid, Building:building, E_ELEMENT_TYPE:type)
{
    if(Building_IsValid(building) && type == ELEMENT_TYPE_BUILDING_ENTRANCE)
    {
		new String:string;
		if(Account_GetAdminLevel(playerid) > 1)
		{
			string += str_format("~r~ID:~w~ %d~n~", Building_GetID(building));
		}
		if(Building_IsOwnable(building))
		{
			if(Building_GetOwnerID(building) != 0)
			{
				new bname[MAX_BUILDING_NAME];
				Building_GetName(building, bname);
				//Building_GetOwnerName(buildingid, ownerName);
				string += str_format("~y~%s~n~~w~Proprietario: ~y~%s~w~.~n~", bname, Character_GetNameFromDatabase(Building_GetOwnerID(building), true));
				if(Building_GetOwnerID(building) == Character_GetID(playerid))
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
				Building_GetName(building, bname);
				string += str_format("~y~%s~n~~g~Prezzo:~w~ $%d~n~", bname, Building_GetPrice(building));
			}
		}
		else
		{
			new bname[MAX_BUILDING_NAME];
			Building_GetName(building, bname);
			string += str_format("~y~%s~n~", bname);
		}
		GameTextForPlayerStr(playerid, string, 2000, 5);
	}
    return 1;
}