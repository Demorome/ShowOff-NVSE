#pragma once


DEFINE_CMD_ALT_COND_PLUGIN(GetNumActorsInRangeFromRef,, "Returns the amount of actors that are a certain distance nearby to the calling reference.", 1, kParams_OneFloat_OneInt);
//Code ripped from both JIP (GetActorsByProcessingLevel) and SUP.
UINT32 __fastcall GetNumActorsInRangeFromRefCALL(TESObjectREFR* thisObj, float range, UInt32 flags)
{
	if (range <= 0) return 0;
	if (!thisObj) return 0;
	
	UInt32 numActors = 0;
	bool const noDeadActors = flags & 0x1;
	//bool const something = flags & 0x2;
	
	MobileObject** objArray = g_processManager->objects.data, ** arrEnd = objArray;
	objArray += g_processManager->beginOffsets[0];  //Only objects in High process.
	arrEnd += g_processManager->endOffsets[0];
	Actor* actor;

	if (thisObj->IsActor())  //To avoid redundant (actor != thisObj) checks.
	{
		for (; objArray != arrEnd; objArray++)
		{
			actor = (Actor*)*objArray;
			//Console_Print("Current actor >>>%08x", actor->refID);

			if (actor && actor->IsActor() && actor != thisObj)
			{
				if (noDeadActors && actor->GetDead())
					continue;

				if (GetDistance3D(thisObj, actor) <= range)
					numActors++;
			}
		}
	}
	else if (flags)  //To avoid redundant flag checks.
	{
		for (; objArray != arrEnd; objArray++)
		{
			actor = (Actor*)*objArray;
			//Console_Print("Current actor >>>%08x", actor->refID);

			if (actor && actor->IsActor() && actor != thisObj)
			{
				if (noDeadActors && actor->GetDead())
					continue;

				if (GetDistance3D(thisObj, actor) <= range)
					numActors++;
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
				{
					numActors++;
				}
			}
		}
	}

	// Player is not included in the looped array, so we need to check for it outside the loop.
	if (thisObj != g_thePlayer)
	{
		actor = (Actor*)g_thePlayer;
		if (noDeadActors)
		{
			if (actor->GetDead())
				return numActors;
		}
		if (GetDistance3D(thisObj, actor) <= range)
		{
			numActors++;
		}
	}

	//Console_Print("Result: %d", numActors);
	return numActors; 
}

bool Cmd_GetNumActorsInRangeFromRef_Eval(COMMAND_ARGS_EVAL)
{
	*result = GetNumActorsInRangeFromRefCALL(thisObj, *(float*)&arg1, (UInt32)arg2);
	return true;
}
bool Cmd_GetNumActorsInRangeFromRef_Execute(COMMAND_ARGS)
{
	float range;
	UINT32 flags;
	if (ExtractArgs(EXTRACT_ARGS, &range, &flags))
		*result = GetNumActorsInRangeFromRefCALL(thisObj, range, flags);
	else
		*result = 0;
	return true;
}


DEFINE_CMD_ALT_COND_PLUGIN(GetNumCombatActorsFromActor, , "Returns the amount of actors that are allies/targets to the calling actor, with optional filters.", 1, kParams_OneFloat_OneInt);
//Code ripped off of JIP's GetCombatActors.
UINT32 __fastcall GetNumCombatActorsFromActorCALL(TESObjectREFR* thisObj, float range, UInt32 flags)
{
	if (!thisObj) return 0;
	if (!thisObj->IsActor()) return 0;
	if (!flags) return 0;
	//Even if the calling actor is dead, they could still have combat targets, so we don't filter that out.
	
	UINT32 numActors = 0;
	bool const getAllies = flags & 0x1;
	bool const getTargets = flags & 0x2;
	
	Actor* actor;
	if (range <= 0)  //ignore distance.
	{
		if (thisObj == g_thePlayer)
		{
			CombatActors* cmbActors = g_thePlayer->combatActors;
			if (!cmbActors) return numActors;
			if (getAllies)
			{
				numActors += cmbActors->allies.size;   //thisObj is its own combat ally, for whatever reason...
				numActors--;                           //So we decrement it by one to get rid of that.
			}
			if (getTargets)
			{
				numActors += cmbActors->targets.size;
			}
		}
		else
		{
			actor = (Actor*)thisObj;
			if (getAllies && actor->combatAllies)
			{
				numActors += actor->combatAllies->size;
			}
			if (getTargets && actor->combatTargets)
			{
				numActors += actor->combatTargets->size;
			}
		}
	}
	else  //---Account for distance.
	{
		UINT32 count;
		
		if (thisObj == g_thePlayer)
		{
			CombatActors* cmbActors = g_thePlayer->combatActors;
			if (!cmbActors) return numActors;
			if (getAllies)
			{
				CombatAlly* allies = cmbActors->allies.data;
				for (count = cmbActors->allies.size; count; count--, allies++)
				{
					actor = allies->ally;
					if (actor && (actor != thisObj))
					{
						if (GetDistance3D(thisObj, actor) <= range)
						{
							numActors++;
						}
					}
				}
			}
			if (getTargets)
			{
				CombatTarget* targets = cmbActors->targets.data;
				for (count = cmbActors->targets.size; count; count--, targets++)
				{
					actor = targets->target;
					if (actor)
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
			actor = (Actor*)thisObj;
			Actor** actorsArr = NULL;
			if (getAllies && actor->combatAllies)
			{
				actorsArr = actor->combatAllies->data;
				if (actorsArr)
				{
					count = actor->combatAllies->size;
					for (; count; count--, actorsArr++)   //can I merge these two loops, to be easier to debug?
					{
						actor = *actorsArr;
						if (actor && (actor != thisObj))  //thisObj is its own combat ally, for whatever reason...
						{
							if (GetDistance3D(thisObj, actor) <= range)
							{
								numActors++;
							}
						}
					}
				}
			}
			if (getTargets && actor->combatTargets)  
			{
				actorsArr = actor->combatTargets->data;
				if (actorsArr)
				{
					count = actor->combatTargets->size;
					for (; count; count--, actorsArr++)   //can I merge these two loops, to be easier to debug?
					{
						actor = *actorsArr;
						if (actor)  //thisObj cannot be its own target, no need to check against that.
						{
							if (GetDistance3D(thisObj, actor) <= range)
							{
								numActors++;
							}
						}
					}
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
	float range;
	UINT32 flags;
	if (ExtractArgs(EXTRACT_ARGS, &range, &flags))
		*result = GetNumCombatActorsFromActorCALL(thisObj, range, flags);
	else
		*result = 0;
	return true;
}

DEFINE_COMMAND_PLUGIN(GetCreatureTurningSpeed, , 0, 1, kParams_OneOptionalActorBase);  //copied after GetCreatureCombatSkill from JG
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

DEFINE_COMMAND_PLUGIN(SetCreatureTurningSpeed, , 0, 2, kParams_OneFloat_OneOptionalActorBase);
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

DEFINE_COMMAND_PLUGIN(GetCreatureFootWeight, , 0, 1, kParams_OneOptionalActorBase);
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

DEFINE_COMMAND_PLUGIN(SetCreatureFootWeight, , 0, 2, kParams_OneFloat_OneOptionalActorBase);
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

DEFINE_COMMAND_PLUGIN(GetCreatureAttackReach, , 0, 1, kParams_OneOptionalActorBase);
bool Cmd_GetCreatureAttackReach_Execute(COMMAND_ARGS)
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
		*result = creature->attackReach;
	return true;
}

DEFINE_COMMAND_PLUGIN(SetCreatureAttackReach, , 0, 2, kParams_OneFloat_OneOptionalActorBase);
bool Cmd_SetCreatureAttackReach_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESCreature* creature = NULL;
	float attackReach = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &attackReach, &creature)) return true;
	if (!creature)
	{
		if (!thisObj || !thisObj->IsActor()) return true;
		creature = (TESCreature*)((Actor*)thisObj)->GetActorBase();
	}
	if IS_TYPE(creature, TESCreature)
	{
		creature->attackReach = attackReach;
		*result = 1;
	}
	return true;
}

DEFINE_COMMAND_PLUGIN(SetCreatureBaseScale, , 0, 2, kParams_OneFloat_OneOptionalActorBase);
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