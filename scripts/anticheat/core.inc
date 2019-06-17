#include <YSI_Coding\y_hooks>

static pAmmoSync[MAX_PLAYERS char];

// Should I use Nex-AC? To add a better layer of security?

hook OnAntiCheatDetected(playerid, code)
{
	if(code == AC_SPEED_HACK)
	{
		SendClientMessage(playerid, COLOR_ERROR, "Sei stato kickato dal server. Motivo: Speed Hack.");
		KickEx(playerid);
		return 1;
	}
	return 1;
}

public OnPlayerAirbreak(playerid)
{
	AC_Detect(playerid, AC_AIR_BREAK);
	return 1;
}

// Is this Timer expansive?
// probably yes, but still better than OnPlayerUpdate (called each tick)
ptask AntiCheatTimer[250](playerid) 
{
	if(IsPlayerNPC(playerid) || !Character_IsLogged(playerid) /*|| AccountInfo[playerid][aAdmin] > 2*/)
		return Y_HOOKS_BREAK_RETURN_1;

	if(GetPlayerMoney(playerid) != Character_GetMoney(playerid))
	{
		Character_GiveMoney(playerid, 0); // Resets player money
	}

	if(!Character_IsDead(playerid))
	{
		new currentWeapon = GetPlayerWeapon(playerid),
		currentAmmo = GetPlayerAmmo(playerid),
		slot = Weapon_GetSlot(currentWeapon),
		anticheatWeapon = ACInfo[playerid][acWeapons][slot],
		anticheatAmmo = ACInfo[playerid][acAmmo][slot];
		if(currentWeapon != anticheatWeapon && slot != 0 && currentWeapon != 0 && currentWeapon != WEAPON_BOMB && currentWeapon != WEAPON_PARACHUTE)
		{
			if(35 <= currentWeapon <= 39)
			{
				KickEx(playerid);
			}
			else if(slot == 1 || slot == 10)
				AC_RemovePlayerWeapon(playerid, currentWeapon);
			else
				SetPlayerAmmo(playerid, currentWeapon, 0);
			AC_Detect(playerid, AC_WEAPON_HACK);
			SetPlayerArmedWeapon(playerid, 0);
			if(!IsPlayerInAnyVehicle(playerid))
				ClearAnimations(playerid);
		}
		else if(currentWeapon == anticheatWeapon && currentWeapon != 0 && 2 <= slot <= 8 && !IsPlayerInAnyVehicle(playerid))
		{
			if(currentAmmo > anticheatAmmo)
			{
				if(anticheatAmmo > 0)
				{
					SetPlayerArmedWeapon(playerid, 0);
					SetPlayerAmmo(playerid, currentWeapon, anticheatAmmo);
				}
				else
				{
					SetPlayerAmmo(playerid, slot, 0);
				}
				AC_Detect(playerid, AC_AMMO_HACK);
			}
			else if(currentAmmo < anticheatAmmo)
			{
				pAmmoSync{playerid}++;
				if(pAmmoSync{playerid} >= 5)
				{
					ACInfo[playerid][acAmmo][slot] = currentAmmo;
					pAmmoSync{playerid} = 0;
				}
			}
			else
				pAmmoSync{playerid} = 0;
		}

		if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
		{
			new vid = GetPlayerVehicleID(playerid),
				speed = GetPlayerSpeed(playerid, true);
			if(speed > 300 && !IsAPlane(vid) && !IsAHelicopter(vid))
			{
				AC_Detect(playerid, AC_SPEED_HACK);
			}
		}
	}

	// Anti-cheats we can pass-on if we're admins goes here.
	if(AccountInfo[playerid][aAdmin] > 1)
		return Y_HOOKS_CONTINUE_RETURN_1;
	
	if(GetPlayerSpecialAction(playerid) == SPECIAL_ACTION_USEJETPACK && !gPlayerJetpack[playerid])
	{
		SetPlayerSpecialAction(playerid, SPECIAL_ACTION_NONE);
	}
	return Y_HOOKS_CONTINUE_RETURN_1;
}

CMD:trigger(playerid, params[])
{
	GivePlayerWeapon(playerid, 24, 1);
	return 1;
}



#include <anticheat\cheats\money>
#include <anticheat\cheats\weapons>
#include <anticheat\cheats\health_armour>
#include <anticheat\cheats\no_reload>
