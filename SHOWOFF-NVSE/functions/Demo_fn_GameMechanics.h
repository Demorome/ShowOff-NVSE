#pragma once

#include "GameRTTI.h"
#include "SafeWrite.h"
#include "ShowOffNVSE.h"
#include "internal/StewieMagic.h"
#include "internal/jip_nvse.h"

DEFINE_COMMAND_PLUGIN(SetPlayerCanPickpocketEquippedItems, , 0, 1, kParams_OneOptionalInt);
bool Cmd_SetPlayerCanPickpocketEquippedItems_Execute(COMMAND_ARGS)
{
	UInt32 bOn;
	if (NUM_ARGS && ExtractArgs(EXTRACT_ARGS, &bOn))
	{
		bool bCheck = canPlayerPickpocketEqItems();
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
	*result = canPlayerPickpocketEqItems();
	return true;
}


#if IFYOULIKEBROKENSHIT

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