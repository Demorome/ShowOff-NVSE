#pragma once

#include "GameRTTI.h"
#include "SafeWrite.h"
#include "ShowOffNVSE.h"
#include "internal/StewieMagic.h"
#include "internal/jip_nvse.h"

DEFINE_COMMAND_ALT_PLUGIN(SetPlayerCanPickpocketEquippedItems, SetPCCanStealEqItems, "Toggles the ability to pickpocket equipped items.", 0, 1, kParams_OneInt);
bool Cmd_SetPlayerCanPickpocketEquippedItems_Execute(COMMAND_ARGS)
{
	UInt32 bOn;
	if (NUM_ARGS && ExtractArgs(EXTRACT_ARGS, &bOn))
	{
		bool bCheck = g_canPlayerPickpocketEqItems();
		if (bOn && !bCheck)
		{
			// replace check in ContainerMenu::ShouldHideItem while pickpocketting for item being worn, with a check the target is a child
			//Courtesy of lStewieAl!
			WriteRelCall(0x75E87A, UInt32(ContainerMenuCheckIsTargetChild));
		}
		else if (!bOn && bCheck)
		{
			// revert the change to the check in ContainerMenu::ShouldHideItem.
			WriteRelCall(0x75E87A, 0x4BDDD0);
		}
	}
	return true;
}

DEFINE_CMD_ALT_COND_PLUGIN(GetPlayerCanPickpocketEquippedItems, GetPCCanStealEqItems, "Checks if the player can pickpocket equipped items.", 0, NULL);
bool Cmd_GetPlayerCanPickpocketEquippedItems_Eval(COMMAND_ARGS_EVAL)
{
	*result = g_canPlayerPickpocketEqItems();
	return true;
}
bool Cmd_GetPlayerCanPickpocketEquippedItems_Execute(COMMAND_ARGS)
{
	*result = g_canPlayerPickpocketEqItems();
	return true;
}

DEFINE_CMD_ALT_COND_PLUGIN(GetPCCanFastTravel, GetCanFastTravel, "Returns whether or not the player can Fast Travel", 0, NULL);
bool Cmd_GetPCCanFastTravel_Eval(COMMAND_ARGS_EVAL)
{
	*result = (g_thePlayer->byte66D & 1) != 0;
	return true;
}
bool Cmd_GetPCCanFastTravel_Execute(COMMAND_ARGS)
{
	*result = (g_thePlayer->byte66D & 1) != 0;
	return true;
}

DEFINE_CMD_ALT_COND_PLUGIN(GetPCCanSleepWait, GetCanSleepWait, "Returns whether or not the player can Sleep/Wait", 0, NULL);
bool Cmd_GetPCCanSleepWait_Eval(COMMAND_ARGS_EVAL)
{
	*result = g_thePlayer->canSleepWait;
	return true;
}
bool Cmd_GetPCCanSleepWait_Execute(COMMAND_ARGS)
{
	Cmd_GetPCCanSleepWait_Eval(thisObj, NULL, NULL, result);
	return true;
}

DEFINE_COMMAND_ALT_PLUGIN(SetPCCanSleepWait, SetCanSleepWait, "Sets whether or not the player can Sleep/Wait", 0, 1, kParams_OneInt);
bool Cmd_SetPCCanSleepWait_Execute(COMMAND_ARGS)
{
	UInt32 bOn;
	if (ExtractArgs(EXTRACT_ARGS, &bOn))
		g_thePlayer->canSleepWait = (bOn != 0);
	return true;
}


#if IFYOULIKEBROKENSHIT



DEFINE_COMMAND_PLUGIN(SetPCCanPickpocketInCombat, , 0, 1, kParams_OneInt);
bool Cmd_SetPCCanPickpocketInCombat_Execute(COMMAND_ARGS)
{
	UInt32 bOn;
	if (ExtractArgs(EXTRACT_ARGS, &bOn))
		g_canPlayerPickpocketInCombat = bOn;
	
	return true;
}


DEFINE_COMMAND_PLUGIN(SetNoEquip, "Returns 1 if the inventory ref was sucessfully set to NoEquip (or the flag was properly cleared). Returns 2 if it was already set.", 1, 1, kParams_OneInt);
bool Cmd_SetNoEquip_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 noEquip;
	if (ExtractArgs(EXTRACT_ARGS, &noEquip))
	{
		InventoryRef* invRef = InventoryRefGetForID(thisObj->refID);
		if (!invRef) return true;
		ExtraDataList* xData = invRef ? invRef->xData : NULL;
		if (xData)
		{
			if (!noEquip)
			{
				RemoveExtraType(xData, kExtraData_CannotWear);
				*result = 1;
			}
			else if (!xData->HasType(kExtraData_CannotWear) && !xData->HasType(kExtraData_Worn))  //I just replaced the "IsEquipped" extra data check from JIP's SetNoUnequip.
			{
				AddExtraData(xData, ExtraCannotWear::Create());
				*result = 1;
			}
			else if (xData->HasType(kExtraData_CannotWear))
				*result = 2;
		}
	} 
	return true;
}

DEFINE_COMMAND_PLUGIN(GetFastTravelFlags, , 0, 0, NULL);
bool Cmd_GetFastTravelFlags_Execute(COMMAND_ARGS)
{
	*result = (g_thePlayer->byte66D & 1) != 0;
	return true;
}


//not so ez...
bool __fastcall ez(TESAnimGroup* entry, void* edx)
{
	return false;
}

DEFINE_COMMAND_PLUGIN(MultiJump, , 0, 1, kParams_OneOptionalInt);
bool Cmd_MultiJump_Execute(COMMAND_ARGS)
{
	UInt32 bOn;
	if (ExtractArgs(EXTRACT_ARGS, &bOn))
	{
		//bool bCheck = ();
		if (bOn )//&& !bCheck)
		{
			WriteRelCall(0x9422F5, UInt32(ez));
		}
		/*
		else if (!bOn && bCheck)
		{
			WriteRelCall(0x75E87A, 0x4BDDD0);
		}
		*result = ();
		*/
	}
	return true;
}

#endif