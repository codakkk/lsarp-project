#include <discord-connector>
#include <strlib>
#include <YSI_Coding\y_hooks>

static 
    DCC_Channel:BanLogsChannel, 
    DCC_Channel:KickLogsChannel, 
    DCC_Channel:MoneyLogsChannel,
    DCC_Channel:AdminLogsChannel
;

hook OnGameModeInit()
{
    BanLogsChannel = DCC_FindChannelById("620900796321497088");
    KickLogsChannel = DCC_FindChannelById("620900766076502026");
    MoneyLogsChannel = DCC_FindChannelById("620901733815549963");
    AdminLogsChannel = DCC_FindChannelById("620946825746186244");
    return 1;
}

stock Discord_SendMessage(DCC_Channel:channel, const msg[], GLOBAL_TAG_TYPES:...)
{
    format(YSI_UNSAFE_HUGE_STRING, YSI_UNSAFE_HUGE_LENGTH, msg, ___(2));
    utf8encode(YSI_UNSAFE_HUGE_STRING, msg, YSI_UNSAFE_HUGE_LENGTH);
    DCC_SendChannelMessage(channel, YSI_UNSAFE_HUGE_STRING);
    return 1;
}

stock Discord_SendBanMessage(const msg[], GLOBAL_TAG_TYPES:...)
{
    Discord_SendMessage(BanLogsChannel, msg, ___(1));
    return 1;
}

stock Discord_SendKickMessage(const msg[], GLOBAL_TAG_TYPES:...)
{
    Discord_SendMessage(KickLogsChannel, msg, ___(1));
    return 1;
}

stock Discord_SendMoneyMessage(const msg[], GLOBAL_TAG_TYPES:...)
{
    Discord_SendMessage(MoneyLogsChannel, msg, ___(1));
    return 1;
}

stock Discord_SendPlayerMoneyMessage(playerid, giveid, money, const extra[] = "")
{
    if(strlen(extra) > 0)
        Discord_SendMoneyMessage("**%s** (**%s**) ha dato **$%d** a **%s** (**%s**)nMotivo: **%s**", Character_GetOOCName(playerid), Account_GetName(playerid), money, Character_GetOOCName(giveid), Account_GetID(giveid), extra);
    else
        Discord_SendMoneyMessage("**%s** (**%s**) ha dato **$%d** a **%s** (**%s**).", Character_GetOOCName(playerid), Account_GetName(playerid), money, Character_GetOOCName(giveid), Account_GetID(giveid));
}

stock Discord_SendAdminMessage(const msg[], GLOBAL_TAG_TYPES:...)
{
    Discord_SendMessage(AdminLogsChannel, msg, ___(1));
    return 1;
}
