
#include <YSI_Coding\y_hooks>


hook function String:GetItemExtraStr(itemid, quantity, extra)
{
    if(itemid != gItem_Phone)
        return String:continue(itemid, quantity, extra);
	return str_val(extra);
}
