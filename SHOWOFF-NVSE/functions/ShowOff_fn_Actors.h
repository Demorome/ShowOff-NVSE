#pragma once

DEFINE_CMD_ALT_COND_PLUGIN(GetNumActorsInRangeFromRef, , "Returns the amount of actors that are a certain distance nearby to the calling reference.", 1, kParams_OneFloat_OneOptionalInt);
DEFINE_CMD_ALT_COND_PLUGIN(GetNumCombatActorsFromActor, , "Returns the amount of actors that are allies/targets to the calling actor, with optional filters.", 1, kParams_OneOptionalFloat_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(GetCreatureTurningSpeed, , 0, 1, kParams_OneOptionalActorBase);  //copied after GetCreatureCombatSkill from JG
DEFINE_COMMAND_PLUGIN(SetCreatureTurningSpeed, , 0, 2, kParams_OneFloat_OneOptionalActorBase);
DEFINE_COMMAND_PLUGIN(GetCreatureFootWeight, , 0, 1, kParams_OneOptionalActorBase);
DEFINE_COMMAND_PLUGIN(SetCreatureFootWeight, , 0, 2, kParams_OneFloat_OneOptionalActorBase);
DEFINE_COMMAND_PLUGIN(SetCreatureReach, , 0, 2, kParams_OneInt_OneOptionalActorBase);
DEFINE_COMMAND_PLUGIN(SetCreatureBaseScale, , 0, 2, kParams_OneFloat_OneOptionalActorBase);
DEFINE_CMD_ALT_COND_PLUGIN(GetNumCompassHostiles, , "Returns the amount of hostile actors on compass, w/ optional filters.", 0, kParams_OneOptionalFloat_OneOptionalInt);


//No Invisible Actor filter - credits to JG for that bit of code.


//Code ripped from both JIP (GetActorsByProcessingLevel) and SUP (FindClosestActorFromRef).
UInt32 __fastcall GetNumActorsInRangeFromRef_Call(TESObjectREFR* const thisObj, float const range, UInt32 const flags)
{
	if (range <= 0) return 0;
	if (!thisObj) return 0;

#define DebugGetNumActorsInRangeFromRef _DEBUG

	enum functionFlags
	{
		kFlag_noDeadActors = 1,
		kFlag_noInvisibleActors = 2,
		maxFlags = kFlag_noDeadActors | kFlag_noInvisibleActors,
	};
	
	bool const noDeadActors = flags & kFlag_noDeadActors;
	bool const noInvisibleActors = flags & kFlag_noInvisibleActors;

#if DebugGetNumActorsInRangeFromRef 
	_MESSAGE("DebugGetNumActorsInRangeFromRef - begin dump for thisObj %s (%08x)", thisObj->GetName(), thisObj->refID);
#endif
	
	MobileObject** objArray = g_processManager->objects.data, ** arrEnd = objArray;
	objArray += g_processManager->beginOffsets[0];  //Only objects in High process.
	arrEnd += g_processManager->endOffsets[0];
	UInt32 numActors = 0;  //return value
	for (; objArray != arrEnd; objArray++)
	{
		auto actor = (Actor*)*objArray;
		if (actor && actor->IsActor() && actor != thisObj)
		{
#if DebugGetNumActorsInRangeFromRef 
			_MESSAGE("Current actor >>> %08x (%s). isDead: %d, distance: %f", actor->refID, actor->GetName(), actor->GetDead(), GetDistance3D(thisObj, actor));
#endif
			
			if (noDeadActors && actor->GetDead())
				continue;

			if (noInvisibleActors && actor->avOwner.Fn_02(kAVCode_Invisibility) > 0 || actor->avOwner.Fn_02(kAVCode_Chameleon) > 0)
				continue;
			
			if (GetDistance3D(thisObj, actor) <= range)
				numActors++;
		}
	}

	// Player is not included in the looped array, so we need to check for it outside the loop.
	if (thisObj != g_thePlayer)
	{
		if (noDeadActors && g_thePlayer->GetDead())
			return numActors;
		
		if (GetDistance3D(thisObj, g_thePlayer) <= range)
			numActors++;
	}

#undef DebugGetNumActorsInRangeFromRef
	return numActors; 
}

bool Cmd_GetNumActorsInRangeFromRef_Eval(COMMAND_ARGS_EVAL)
{
	*result = GetNumActorsInRangeFromRef_Call(thisObj, *(float*)&arg1, (UInt32)arg2);
	return true;
}
bool Cmd_GetNumActorsInRangeFromRef_Execute(COMMAND_ARGS)
{
	float range = 0;
	UINT32 flags = 0;
	if (ExtractArgs(EXTRACT_ARGS, &range, &flags))
		*result = GetNumActorsInRangeFromRef_Call(thisObj, range, flags);
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

#define DebugGetNumCombatActorsFromActor _DEBUG

#if DebugGetNumCombatActorsFromActor 
	_MESSAGE("DebugGetNumActorsInRangeFromRef - begin dump for thisObj %s (%08x)", thisObj->GetName(), thisObj->refID);
#endif

	enum functionFlags
	{
		kFlag_GetAllies = 1,
		kFlag_GetTargets = 2,
		kFlag_AlliesAndTargets = kFlag_GetAllies | kFlag_GetTargets,
		kFlag_NoInvisibleActors = 4,
	};
	if (!flags) flags = kFlag_AlliesAndTargets;
	bool const getAllies = flags & kFlag_GetAllies;
	bool const getTargets = flags & kFlag_GetTargets;
	bool const noInvisibleActors = flags & kFlag_NoInvisibleActors;

	UINT32 numActors = 0;
	auto IncrementNumActorsIfChecksPass = [&](Actor* actor)
	{
		if (actor && (actor != thisObj))  
		{
#if DebugGetNumCombatActorsFromActor 
			_MESSAGE("Current actor >>> %08x (%s). isDead: %d, distance: %f", actor->refID, actor->GetName(), actor->GetDead(), GetDistance3D(thisObj, actor));
#endif
			if (noInvisibleActors && actor->avOwner.Fn_02(kAVCode_Invisibility) > 0 || actor->avOwner.Fn_02(kAVCode_Chameleon) > 0)
				return;
			
			if (range > 0.0F)
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
#undef DebugGetNumCombatActorsFromActor
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

// Credits to JIP LN for the GetCreature__ code format.
bool Cmd_GetCreatureTurningSpeed_Execute(COMMAND_ARGS)
{
	*result = -1;
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
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &turningSpeed, &creature) || turningSpeed < 0) return true;
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
	*result = -1;
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
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &footWeight, &creature) || footWeight < 0) return true;
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
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &reach, &creature) || reach < 0) return true;
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
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &newVal, &creature) || newVal < 0) return true;
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

//Copied JG's GetNearestCompassHostile code.
UInt32 __fastcall GetNumCompassHostiles_Call(TESObjectREFR* const thisObj, float const maxRange, UInt32 flags)
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
	//todo: learn why this stuff is checked!
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
				if (maxRange > 0.0F)
				{
					if (distToPlayer <= maxRange)
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

bool Cmd_GetNumCompassHostiles_Eval(COMMAND_ARGS_EVAL)
{
	float const max_range = *(float*)&arg1;
	auto const flags = (UInt32)arg2;
	*result = GetNumCompassHostiles_Call(thisObj, max_range, flags);
	return true;
}
bool Cmd_GetNumCompassHostiles_Execute(COMMAND_ARGS)
{
	float max_range = 0;
	UInt32 flags = 0;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &max_range, &flags))
		*result = GetNumCompassHostiles_Call(thisObj, max_range, flags);
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