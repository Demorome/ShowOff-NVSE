#pragma once

DEFINE_CMD_ALT_COND_PLUGIN(GetNumActorsInRangeFromRef, , "Returns the amount of actors that are a certain distance nearby to the calling reference.", 1, kParams_OneFloat_OneInt);
DEFINE_CMD_ALT_COND_PLUGIN(GetNumCombatActorsFromActor, , "Returns the amount of actors that are allies/targets to the calling actor, with optional filters.", 1, kParams_OneFloat_OneInt);
DEFINE_COMMAND_PLUGIN(GetCreatureTurningSpeed, , 0, 1, kParams_OneOptionalActorBase);  //copied after GetCreatureCombatSkill from JG
DEFINE_COMMAND_PLUGIN(SetCreatureTurningSpeed, , 0, 2, kParams_OneFloat_OneOptionalActorBase);
DEFINE_COMMAND_PLUGIN(GetCreatureFootWeight, , 0, 1, kParams_OneOptionalActorBase);
DEFINE_COMMAND_PLUGIN(SetCreatureFootWeight, , 0, 2, kParams_OneFloat_OneOptionalActorBase);
DEFINE_COMMAND_PLUGIN(SetCreatureReach, , 0, 2, kParams_OneInt_OneOptionalActorBase);
DEFINE_COMMAND_PLUGIN(SetCreatureBaseScale, , 0, 2, kParams_OneFloat_OneOptionalActorBase);
DEFINE_CMD_ALT_COND_PLUGIN(GetNumCompassHostilesInRange, , "Returns the amount of hostile actors that are a certain distance nearby to the player that appear on the compass.", 0, kParams_OneOptionalFloat_OneOptionalInt);



//Code ripped from both JIP (GetActorsByProcessingLevel) and SUP.
UINT32 __fastcall GetNumActorsInRangeFromRefCALL(TESObjectREFR* const thisObj, float const range, UInt32 const flags)
{
	if (range <= 0) return 0;
	if (!thisObj) return 0;

#define DebugGetNumActorsInRangeFromRef 1;
	
	UInt32 numActors = 0;
	bool const noDeadActors = flags & 1;
	//bool const something = flags & 2;
	
	MobileObject** objArray = g_processManager->objects.data, ** arrEnd = objArray;
	objArray += g_processManager->beginOffsets[0];  //Only objects in High process.
	arrEnd += g_processManager->endOffsets[0];

	for (; objArray != arrEnd; objArray++)
	{
		auto actor = (Actor*)*objArray;
		if (actor && actor->IsActor() && actor != thisObj)
		{
#if DebugGetNumActorsInRangeFromRef 
			Console_Print("Current actor >>> %08x (%s)", actor->refID, actor->GetName());
#endif
			
			if (noDeadActors && actor->GetDead())
				continue;

			if (GetDistance3D(thisObj, actor) <= range)
				numActors++;
		}
	}

	// Player is not included in the looped array, so we need to check for it outside the loop.
	if (thisObj != g_thePlayer)
	{
		if (noDeadActors)
		{
			if (g_thePlayer->GetDead())
				return numActors;
		}
		if (GetDistance3D(thisObj, g_thePlayer) <= range)
		{
			numActors++;
		}
	}

#undef DebugGetNumActorsInRangeFromRef
	return numActors; 
}

bool Cmd_GetNumActorsInRangeFromRef_Eval(COMMAND_ARGS_EVAL)
{
	*result = GetNumActorsInRangeFromRefCALL(thisObj, *(float*)&arg1, (UInt32)arg2);
	return true;
}
bool Cmd_GetNumActorsInRangeFromRef_Execute(COMMAND_ARGS)
{
	float range = 0;
	UINT32 flags = 0;
	if (ExtractArgs(EXTRACT_ARGS, &range, &flags))
		*result = GetNumActorsInRangeFromRefCALL(thisObj, range, flags);
	else
		*result = 0;
	return true;
}


// Code ripped off of JIP's GetCombatActors.
// todo: enforce Avoid Repeating Code principle.
// Figure out how to merge all these for loops.

UInt32 __fastcall GetNumCombatActorsFromActorCALL(TESObjectREFR* thisObj, float range, UInt32 flags)
{
	if (!thisObj) return 0;
	if (!thisObj->IsActor()) return 0;
	//Even if the calling actor is dead, they could still have combat targets, so we don't filter that out.

	enum functionFlags
	{
		kFlag_GetAllies = 1,
		kFlag_GetTargets = 2,
		maxFlags = kFlag_GetAllies | kFlag_GetTargets,
	};
	if (!flags) flags = maxFlags;
	bool const getAllies = flags & kFlag_GetAllies;
	bool const getTargets = flags & kFlag_GetTargets;

	UINT32 numActors = 0;
	auto IncrementNumActorsIfChecksPass = [&](Actor* actor)
	{
		if (actor && (actor != thisObj))  //todo: verify if !range (float) check works
		{
			if (range != 0.0F)
			{
				if (GetDistance3D(thisObj, actor) <= range)
				{
					numActors++;
				}
			}
			else
			{
				numActors++;
			}
		}
	};
	
	Actor* actor;
	if (thisObj == g_thePlayer)
	{
		CombatActors* cmbActors = g_thePlayer->combatActors;
		if (!cmbActors) return 0;
		if (getAllies)
		{
			CombatAlly* allies = cmbActors->allies.data;
			for (UInt32 count = cmbActors->allies.size; count; count--, allies++)
			{
				actor = allies->ally;
				IncrementNumActorsIfChecksPass(actor);
			}
		}
		if (getTargets)
		{
			CombatTarget* targets = cmbActors->targets.data;
			for (UInt32 count = cmbActors->targets.size; count; count--, targets++)
			{
				actor = targets->target;
				IncrementNumActorsIfChecksPass(actor);
			}
		}
	}
	else
	{
		actor = (Actor*)thisObj;
		Actor** actorsArr = NULL;
		if (getAllies && actor->combatAllies)
		{
			actorsArr = actor->combatAllies->data; 
			if (actorsArr)
			{
				for (UInt32 count = actor->combatAllies->size; count; count--, actorsArr++)
				{
					actor = *actorsArr;
					IncrementNumActorsIfChecksPass(actor);
				}
			}
		}
		if (getTargets && actor->combatTargets)  
		{
			actorsArr = actor->combatTargets->data;
			if (actorsArr)
			{
				for (UInt32 count = actor->combatTargets->size; count; count--, actorsArr++)
				{
					actor = *actorsArr;
					IncrementNumActorsIfChecksPass(actor);
				}
			}
		}

	}
	return numActors;
}

bool Cmd_GetNumCombatActorsFromActor_Eval(COMMAND_ARGS_EVAL)
{
	*result = GetNumCombatActorsFromActorCALL(thisObj, *(float*)&arg1, (UInt32)arg2);
	return true;
}
bool Cmd_GetNumCombatActorsFromActor_Execute(COMMAND_ARGS)
{
	float range = 0.0F;
	UINT32 flags = 0;
	if (ExtractArgs(EXTRACT_ARGS, &range, &flags))
		*result = GetNumCombatActorsFromActorCALL(thisObj, range, flags);
	else
		*result = 0;
	return true;
}


bool Cmd_GetCreatureTurningSpeed_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESCreature* creature = NULL;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &creature)) return true;
	if (!creature)
	{
		if (!thisObj || !thisObj->IsActor()) return true;
		creature = (TESCreature*)((Actor*)thisObj)->GetActorBase();
	}
	if IS_TYPE(creature, TESCreature)
		*result = creature->turningSpeed;
	return true;
}


bool Cmd_SetCreatureTurningSpeed_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESCreature* creature = NULL;
	float turningSpeed = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &turningSpeed, &creature)) return true;
	if (!creature)
	{
		if (!thisObj || !thisObj->IsActor()) return true;
		creature = (TESCreature*)((Actor*)thisObj)->GetActorBase();
	}
	if IS_TYPE(creature, TESCreature)
	{
		creature->turningSpeed = turningSpeed;
		*result = 1;
	}
	return true;
}


bool Cmd_GetCreatureFootWeight_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESCreature* creature = NULL;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &creature)) return true;
	if (!creature)
	{
		if (!thisObj || !thisObj->IsActor()) return true;
		creature = (TESCreature*)((Actor*)thisObj)->GetActorBase();
	}
	if IS_TYPE(creature, TESCreature)
		*result = creature->footWeight;
	return true;
}


bool Cmd_SetCreatureFootWeight_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESCreature* creature = NULL;
	float footWeight = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &footWeight, &creature)) return true;
	if (!creature)
	{
		if (!thisObj || !thisObj->IsActor()) return true;
		creature = (TESCreature*)((Actor*)thisObj)->GetActorBase();
	}
	if IS_TYPE(creature, TESCreature)
	{
		creature->footWeight = footWeight;
		*result = 1;
	}
	return true;
}


bool Cmd_SetCreatureReach_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESCreature* creature = NULL;
	UInt32 reach = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &reach, &creature)) return true;
	if (!creature)
	{
		if (!thisObj || !thisObj->IsActor()) return true;
		creature = (TESCreature*)((Actor*)thisObj)->GetActorBase();
	}
	if IS_TYPE(creature, TESCreature)
	{
		creature->attackReach = reach;
		*result = 1;
	}
	return true;
}


bool Cmd_SetCreatureBaseScale_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESCreature* creature = NULL;
	float newVal = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &newVal, &creature)) return true;
	if (!creature)
	{
		if (!thisObj || !thisObj->IsActor()) return true;
		creature = (TESCreature*)((Actor*)thisObj)->GetActorBase();
	}
	if IS_TYPE(creature, TESCreature)
	{
		creature->baseScale = newVal;
		*result = 1;
	}
	return true;
}


UInt32 __fastcall GetNumCompassHostilesInRangeCALL(TESObjectREFR* const thisObj, float const range, UInt32 flags)
{
	enum FunctionFlags
	{
		kFlag_SkipInvisible = 1,
		kFlag_Max = kFlag_SkipInvisible,  // | someOtherFlag
	};
	if (!flags) flags = kFlag_Max;

	bool const skipInvisible = flags & kFlag_SkipInvisible;
	UInt32 numHostiles = 0;  //result

	//To avoid counting "compass targets" that are super far away and can't even be seen on compass (I assume).
	float fSneakMaxDistance = *(float*)(0x11CD7D8 + 4);
	float fSneakExteriorDistanceMult = *(float*)(0x11CDCBC + 4);
	bool isInterior = g_thePlayer->GetParentCell()->IsInterior();
	float interiorDistanceSquared = fSneakMaxDistance * fSneakMaxDistance;
	float exteriorDistanceSquared = (fSneakMaxDistance * fSneakExteriorDistanceMult) * (fSneakMaxDistance * fSneakExteriorDistanceMult);
	float maxDist = isInterior ? interiorDistanceSquared : exteriorDistanceSquared;

	NiPoint3* playerPos = g_thePlayer->GetPos();
	auto iter = g_thePlayer->compassTargets->Begin();
	for (; !iter.End(); ++iter)
	{
		PlayerCharacter::CompassTarget* target = iter.Get();
		if (target->isHostile)
		{
			if (skipInvisible && (target->target->avOwner.Fn_02(kAVCode_Invisibility) > 0 || target->target->avOwner.Fn_02(kAVCode_Chameleon) > 0)) {
				continue;
			}
			auto distToPlayer = target->target->GetPos()->CalculateDistSquared(playerPos);
			if (distToPlayer < maxDist)
			{
				if (range) //todo: verify bugprone float check!!
				{
					if (distToPlayer < range)
						numHostiles++;
				}
				else
				{
					numHostiles++;
				}
			}
		}
	}
	return numHostiles;
}

//Copied JG's GetNearestCompassHostile code.
bool Cmd_GetNumCompassHostilesInRange_Eval(COMMAND_ARGS_EVAL)
{
	float range = *(float*)&arg1;
	auto flags = (UInt32)arg2;
	*result = GetNumCompassHostilesInRangeCALL(thisObj,  range, flags);
	return true;
}
bool Cmd_GetNumCompassHostilesInRange_Execute(COMMAND_ARGS)
{
	float range = 0;
	UInt32 flags = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &range, &flags))
		*result = *result = GetNumCompassHostilesInRangeCALL(thisObj, range, flags);
	else
		*result = 0;
	return true;
}




#ifdef _DEBUG



DEFINE_CMD_ALT_COND_PLUGIN(HasAnyScriptPackage, , , 1, NULL);
bool Cmd_HasAnyScriptPackage_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	if (thisObj->IsActor())
	{
		ExtraDataList* xList = &thisObj->extraDataList;
		//*result = ThisStdCall<UINT32>(0x41CB10, xList);
		ExtraPackage* xPackage = GetExtraTypeJIP(&thisObj->extraDataList, Package);
		if (xPackage)
		{
			*result = xPackage->unk10[2];  //kill meh, doesn't work. 0x41CB10 is the best lead I have.
		}
		//bool const bTest = ThisStdCall<bool>(0x674D40, package);
		//Console_Print("ActorHasAnyScriptPackage TEST >> %d", bTest);
		//*result = ?
	} 
	return true;
}
bool Cmd_HasAnyScriptPackage_Execute(COMMAND_ARGS)
{
	return Cmd_HasAnyScriptPackage_Eval(thisObj, 0, 0, result);
}




#endif