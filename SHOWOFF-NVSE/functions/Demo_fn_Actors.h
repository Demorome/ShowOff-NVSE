#pragma once
#include "internal/jip_nvse.h"


#if IFYOULIKEBROKENSHIT


DEFINE_CMD_ALT_COND_PLUGIN(GetNumActorsInRange,, "Returns the amount of actors that are a certain distance nearby to the calling reference.", 1, kParams_OneFloat_OneInt);
UINT32 __fastcall GetNumActorsInRangeCALL(TESObjectREFR* thisObj, float range, bool onlyCombatTargets)
{
	//Console_Print("Range (float): %f", range);

	if (range <= 0) return 0;
	if (!thisObj) return 0;
	UInt32 numActors = 0;
	
	MobileObject** objArray = g_processManager->objects.data, ** arrEnd = objArray;
	objArray += g_processManager->beginOffsets[0];
	arrEnd += g_processManager->endOffsets[0];
	Actor* actor;
	bool const thisObjIsActor = thisObj->IsActor();
	if (onlyCombatTargets && thisObjIsActor)
	{
		for (; objArray != arrEnd; objArray++)
		{
			actor = (Actor*)*objArray;
			if (actor && actor->IsActor())
			{
				if (((Actor*)thisObj)->IsInCombatWith(actor))  //verify that this works with the player, JIP uses a big loop for the player in IsInCombatWith!
				{
					if (GetDistance3D(thisObj, actor) <= range)
					{
						numActors++;
					}
				}
			}
		}
	}
	else
	{
		for (; objArray != arrEnd; objArray++)
		{
			actor = (Actor*)*objArray;
			if (actor && actor->IsActor())
			{
				if (GetDistance3D(thisObj, actor) <= range)
					numActors++;
			}
		}
	}

	//Console_Print("Result: %d", numActors);
	return numActors; 
}

bool Cmd_GetNumActorsInRange_Eval(COMMAND_ARGS_EVAL)  
{
	*result = GetNumActorsInRangeCALL(thisObj, *(float*)&arg1, arg2);
	return true;
}
bool Cmd_GetNumActorsInRange_Execute(COMMAND_ARGS)
{
	*result = 0;
	float range;
	UINT32 onlyCombatTargets;
	if (ExtractArgs(EXTRACT_ARGS, &range, &onlyCombatTargets))
		*result = GetNumActorsInRangeCALL(thisObj, range, onlyCombatTargets);
	return true;
}


#endif