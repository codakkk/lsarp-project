
#include <pickup_system\core>
#include <buildings\core>

#include <YSI_Coding\y_hooks>

hook OnPlayerPickUpElmPickup(playerid, pickupid, Building:building, E_ELEMENT_TYPE:type)
{
    if(Building_IsValid(building) && type == ELEMENT_TYPE_BUILDING_ENTRANCE)
    {
		new String:string = str_new("");
		pawn_guard(string);
		if(Account_GetAdminLevel(playerid) > 1)
		{
			str_append_format(string, "~r~ID:~w~ %d~n~", Building_GetID(building));
		}

		new bname[MAX_BUILDING_NAME];
		Building_GetName(building, bname);
		str_append_format(string, "~y~%s~n~", bname);

		if(Building_IsOwnable(building))
		{
			if(Building_GetOwnerID(building) > 0)
			{
				await mysql_aquery_s(gMySQL, str_format("SELECT name FROM `characters` WHERE id = '%d'", Building_GetOwnerID(building)), .parallel=true);
				if(cache_num_rows() > 0)
				{
					new ownerName[MAX_PLAYER_NAME];
					cache_get_value_name(0, "name", ownerName);
					str_append_format(string, "~w~Proprietario: ~y~%s~w~.~n~", ownerName);
				}

				if(Building_GetOwnerID(building) == Character_GetID(playerid))
				{
					str_append_format(string, "Digita ~b~/biz~w~ per il menu.~n~");
				}
			}
			else
			{
				new taxedPrice = Tax_CalculateByValue(TAX_TYPE_VAT, Building_GetPrice(building));
				str_append_format(string, "~g~Prezzo:~w~ $%s~n~", FormatNumber(taxedPrice));
			}
		}
		if(Building_GetEntrancePrice(building) > 0)
			str_append_format(string, "~g~Entrata:~w~ $%s~n~", FormatNumber(Building_GetEntrancePrice(building)));
		
		GameTextForPlayerStr(playerid, string, 2500, 5);
	}
    return 1;
}