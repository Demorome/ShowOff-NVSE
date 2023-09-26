#pragma once

//Code ripped from both JIP (GetActorsByProcessingLevel) and SUP (FindClosestActorFromRef).
UInt32 __fastcall GetNumActorsInRangeFromRef_Call(TESObjectREFR* const thisObj, float const range, UInt32 const flags)
{
	if (range <= 0) return 0;
	if (!thisObj) return 0;
	bool const isThisObjActor = IS_ACTOR(thisObj);
	
	enum functionFlags
	{
		kFlag_noDeadActors =		1 << 0,
		kFlag_noInvisibleActors =	1 << 1,
		kFlag_onlyDetectedActors =	1 << 2,
	};
	bool const noDeadActors = flags & kFlag_noDeadActors;
	bool const noInvisibleActors = flags & kFlag_noInvisibleActors;
	bool const onlyDetectedActors = (flags & kFlag_onlyDetectedActors) && isThisObjActor;

	if (g_ShowFuncDebug)
		_MESSAGE("DebugGetNumActorsInRangeFromRef - begin dump for thisObj %s (%08x)", thisObj->GetName(), thisObj->refID);
	
	MobileObject** objArray = g_processManager->objects.data, ** arrEnd = objArray;
	objArray += g_processManager->beginOffsets[0];  //Only objects in High process.
	arrEnd += g_processManager->endOffsets[0];
	UInt32 numActors = 0;  //return value
	for (; objArray != arrEnd; objArray++)
	{
		auto actor = (Actor*)*objArray;
		if (actor && actor->IsActor() && actor != thisObj)
		{
			if (g_ShowFuncDebug)
				_MESSAGE("Current actor >>> %08x (%s). isDead: %d, distance: %f", actor->refID, actor->GetName(), actor->GetDead(), GetDistance3D(thisObj, actor));
		
			if (noDeadActors && actor->GetDead())
				continue;
			if (noInvisibleActors && actor->IsInvisible())
				continue;
			if (onlyDetectedActors && !((Actor*)thisObj)->Detects(actor))
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

	return numActors; 
}

DEFINE_CMD_COND_PLUGIN(GetNumActorsInRangeFromRef, "Returns the amount of actors that are a certain distance nearby to the calling reference.", 
	true, kParams_OneFloat_OneOptionalInt);
bool Cmd_GetNumActorsInRangeFromRef_Eval(COMMAND_ARGS_EVAL)
{
	*result = GetNumActorsInRangeFromRef_Call(thisObj, *(float*)&arg1, (UInt32)arg2);
	return true;
}
bool Cmd_GetNumActorsInRangeFromRef_Execute(COMMAND_ARGS)
{
	*result = 0;
	float range = 0;
	UINT32 flags = 0;
	if (ExtractArgs(EXTRACT_ARGS, &range, &flags))
		*result = GetNumActorsInRangeFromRef_Call(thisObj, range, flags);
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

	if (g_ShowFuncDebug)
		_MESSAGE("DebugGetNumActorsInRangeFromRef - begin dump for thisObj %s (%08x)", thisObj->GetName(), thisObj->refID);

	enum functionFlags
	{
		kFlag_GetAllies = 1 << 0,
		kFlag_GetTargets = 1 << 1,
		kFlag_AlliesAndTargets = kFlag_GetAllies | kFlag_GetTargets,  // default flag.
		kFlag_NoInvisibleActors = 1 << 2,
		kFlag_OnlyDetectedActors = 1 << 3,
	};
	if (!flags) flags = kFlag_AlliesAndTargets;
	bool const getAllies = flags & kFlag_GetAllies;
	bool const getTargets = flags & kFlag_GetTargets;
	bool const noInvisibleActors = flags & kFlag_NoInvisibleActors;
	bool const onlyDetectedActors = flags & kFlag_OnlyDetectedActors;

	UINT32 numActors = 0;
	auto IncrementNumActorsIfChecksPass = [&](Actor* actor)
	{
		if (actor && (actor != thisObj))  
		{
			if (g_ShowFuncDebug)
				_MESSAGE("Current actor >>> %08x (%s). isDead: %d, distance: %f", actor->refID, actor->GetName(), actor->GetDead(), GetDistance3D(thisObj, actor));

			if (noInvisibleActors && actor->IsInvisible())
				return;

			if (onlyDetectedActors && !((Actor*)thisObj)->Detects(actor))
				return;
			
			if (range > 0.0F)
			{
				if (GetDistance3D(thisObj, actor) <= range)
					numActors++;
			}
			else
				numActors++;
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
					actor = *actorsArr;  // actor is redefined, so be careful! It is no longer thisObj in the loop.
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

DEFINE_CMD_COND_PLUGIN(GetNumCombatActorsFromActor, "Returns the amount of actors that are allies/targets to the calling actor, with optional filters.", 
	true, kParams_OneOptionalFloat_OneOptionalInt);
bool Cmd_GetNumCombatActorsFromActor_Eval(COMMAND_ARGS_EVAL)
{
	*result = GetNumCombatActorsFromActorCALL(thisObj, *(float*)&arg1, (UInt32)arg2);
	return true;
}
bool Cmd_GetNumCombatActorsFromActor_Execute(COMMAND_ARGS)
{
	float range = 0.0F;
	UInt32 flags = 0;
	if (ExtractArgs(EXTRACT_ARGS, &range, &flags))
		*result = GetNumCombatActorsFromActorCALL(thisObj, range, flags);
	else
		*result = 0;
	return true;
}

//copied after GetCreatureCombatSkill from JG
DEFINE_CMD_COND_PLUGIN(GetCreatureTurningSpeed, "", false, kParams_OneOptionalActorBase); 
bool Cmd_GetCreatureTurningSpeed_Eval(COMMAND_ARGS_EVAL)
{
	*result = -1;
	TESForm* form;
	if (arg1)
		form = (TESForm*)arg1;
	else if (thisObj)
		form = thisObj->baseForm;
	else return true;
	if (auto const creature = DYNAMIC_CAST(form, TESForm, TESCreature))
		*result = creature->turningSpeed;
	return true;
}
bool Cmd_GetCreatureTurningSpeed_Execute(COMMAND_ARGS)
{
	*result = -1;
	TESCreature* creature = NULL;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &creature)) return true;
	return Cmd_GetCreatureTurningSpeed_Eval(thisObj, creature, 0, result);
}

// Credits to JIP LN for the SetCreature__ code format.
DEFINE_COMMAND_PLUGIN(SetCreatureTurningSpeed, "", false, kParams_OneFloat_OneOptionalActorBase);
bool Cmd_SetCreatureTurningSpeed_Execute(COMMAND_ARGS)
{
	*result = false;
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
		*result = true;
	}
	return true;
}

DEFINE_CMD_COND_PLUGIN(GetCreatureFootWeight, "", false, kParams_OneOptionalActorBase);
bool Cmd_GetCreatureFootWeight_Eval(COMMAND_ARGS_EVAL)
{
	*result = -1;
	TESForm* form;
	if (arg1)
		form = (TESForm*)arg1;
	else if (thisObj)
		form = thisObj->baseForm;
	else return true;
	if (auto const creature = DYNAMIC_CAST(form, TESForm, TESCreature))
		*result = creature->footWeight;
	return true;
}
bool Cmd_GetCreatureFootWeight_Execute(COMMAND_ARGS)
{
	*result = -1;
	TESCreature* creature = NULL;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &creature)) return true;
	return Cmd_GetCreatureFootWeight_Eval(thisObj, creature, 0, result);
}

DEFINE_COMMAND_PLUGIN(SetCreatureFootWeight, "", false, kParams_OneFloat_OneOptionalActorBase);
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

DEFINE_COMMAND_PLUGIN(SetCreatureReach, "", false, kParams_OneInt_OneOptionalActorBase);
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

DEFINE_COMMAND_PLUGIN(SetCreatureBaseScale, "", false, kParams_OneFloat_OneOptionalActorBase);
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
			if (skipInvisible && target->target->IsInvisible()) {
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

DEFINE_CMD_COND_PLUGIN(GetNumCompassHostiles, "Returns the amount of hostile actors on compass, w/ optional filters.", 
	false, kParams_OneOptionalFloat_OneOptionalInt);
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

DEFINE_CMD_ALT_COND_PLUGIN(GetActorValueDamage, GetAVDamage, 
	"Returns the damage modifier applied to the actor's AV", true, kParams_OneActorValue);
bool Cmd_GetActorValueDamage_Eval(COMMAND_ARGS_EVAL)
{
	*result = -1;
	UInt32 avCode = (UInt32)arg1;
	if (auto const actor = DYNAMIC_CAST(thisObj, TESObjectREFR, Actor))
	{
		auto damage = actor->avOwner.GetActorValueDamage(avCode);
		if (damage != 0.0F)  // avoid having -0.0
			damage *= -1; // multiply by -1 to invert the sign, since it otherwise gives the negative damage modifier.
		*result = damage;  
	}
	return true;
}
bool Cmd_GetActorValueDamage_Execute(COMMAND_ARGS)
{
	*result = -1;
	UInt32 avCode;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &avCode))
		return true;
	return Cmd_GetActorValueDamage_Eval(thisObj, (void*)avCode, 0, result);
}

namespace SayTo	//functions related to SayTo
{
	enum GetExtraData_Request
	{
		kData_Topic,
		kData_TopicInfo,
		kData_Speaker,
		kData_Quest
	};

	// Credits: copies after JIP's CCCSayTo
	bool GetExtraData_Call(COMMAND_ARGS, GetExtraData_Request request)
	{
		*result = 0;
		if (NOT_ACTOR(thisObj)) return true;
		ExtraSayToTopicInfo* xSayTo = GetExtraTypeJIP(&thisObj->extraDataList, SayToTopicInfo);
		if (xSayTo) {
			switch (request)
			{
			case kData_Speaker:
				REFR_RES = xSayTo->speaker->refID;
				break;
			case kData_Topic:
				REFR_RES = xSayTo->topic->refID;
				break;
			case kData_TopicInfo:
				REFR_RES = xSayTo->info->refID;
				break;
			case kData_Quest:
				REFR_RES = xSayTo->quest->refID;
				break;
			}
		}
		return true;
	}
}

DEFINE_COMMAND_PLUGIN(SayTo_GetSpeakingActor, "", true, NULL);
bool Cmd_SayTo_GetSpeakingActor_Execute(COMMAND_ARGS)
{
	return SayTo::GetExtraData_Call(PASS_COMMAND_ARGS, SayTo::kData_Speaker);
}

DEFINE_COMMAND_PLUGIN(SayTo_GetTopic, "", true, NULL);
bool Cmd_SayTo_GetTopic_Execute(COMMAND_ARGS)
{
	return SayTo::GetExtraData_Call(PASS_COMMAND_ARGS, SayTo::kData_Topic);
}

DEFINE_COMMAND_PLUGIN(SayTo_GetTopicInfo, "", true, NULL);
bool Cmd_SayTo_GetTopicInfo_Execute(COMMAND_ARGS)
{
	return SayTo::GetExtraData_Call(PASS_COMMAND_ARGS, SayTo::kData_TopicInfo);
}

DEFINE_COMMAND_PLUGIN(SayTo_GetQuest, "", true, NULL);
bool Cmd_SayTo_GetQuest_Execute(COMMAND_ARGS)
{
	return SayTo::GetExtraData_Call(PASS_COMMAND_ARGS, SayTo::kData_Quest);
}

DEFINE_COMMAND_PLUGIN(IsActorAlt, "Same as IsActor but accepts an optional baseform.", false, kParams_OneOptionalActorBase);
bool Cmd_IsActorAlt_Execute(COMMAND_ARGS)
{
	*result = false;
	TESForm* baseForm = nullptr;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &baseForm)) 
		return true;
	if (auto const form = TryGetBaseFormOrREFR(baseForm, thisObj))
	{
		*result = form->IsActorAlt();
	}
	return true;
}

//bug: despite not calling the savebake function, this is how being savebaked still.
DEFINE_COMMAND_ALT_PLUGIN(SetSecuritronExpressionTemp, SetSecExpTemp, "", true, kParams_OneReference_TwoStrings);
bool Cmd_SetSecuritronExpressionTemp_Execute(COMMAND_ARGS)
{
	*result = false;	//bSuccess
	Actor* actor;
	char faceStr[516];
	char expressionStr[516];
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &actor, faceStr, expressionStr)
		|| !thisObj || NOT_ACTOR(thisObj))
	{
		return true;
	}
	auto const niNode = actor->GetNiNode();
	if (!niNode) return true;
	ExtraSecuritronFace* xFace = GetExtraTypeJIP(&thisObj->extraDataList, SecuritronFace);
	if (!xFace)
	{
		xFace = ExtraSecuritronFace::Create();
		thisObj->extraDataList.Add(xFace);
	}
	xFace->face.Set(faceStr);
	xFace->expression.Set(expressionStr);
	ThisStdCall(0x437F90, xFace, niNode);	//ExtraSecuritronFace_ApplyChangesToNiObject
	*result = true;
	return true;
}

DEFINE_CMD_ALT_COND_PLUGIN(GetIsPlayerOverencumbered, IsPCOverencumbered, 
	"An alternative to SUP's IsPlayerOverencumbered", false, kParams_OneOptionalInt);
bool Cmd_GetIsPlayerOverencumbered_Eval(COMMAND_ARGS_EVAL)
{
	// Imitate PlayerCharacter::GetIsOverencumbered at 0x954CC0.
	if (bool const ignoreGodMode = reinterpret_cast<UInt32>(arg1) != 0; 
		!ignoreGodMode)
	{
		if (GetIsGodMode()) {
			*result = 0;
			return true;
		}
	}
	auto const weight = g_thePlayer->avOwner.GetActorValueInt(kAVCode_InventoryWeight);
	// Actor::GetMaxCarryWeightPerkModified
	auto const maxWeight = ThisStdCall<double>(0x8A0C20, g_thePlayer);
	*result = maxWeight < weight;
	return true;
}
bool Cmd_GetIsPlayerOverencumbered_Execute(COMMAND_ARGS)
{
	UInt32 ignoreGodMode = false;
	ExtractArgsEx(EXTRACT_ARGS_EX, &ignoreGodMode);
	return Cmd_GetIsPlayerOverencumbered_Eval(nullptr, reinterpret_cast<void*>(ignoreGodMode), nullptr, result);
}

// Credits to lStewieAl
enum SpreadMode
{
	kSpreadMode_Scoped = 0,
	kSpreadMode_AlwaysUseCachedValue,
	kSpreadMode_2,
	kSpreadMode_VATS,
	kSpreadMode_INVALID
};

DEFINE_COMMAND_PLUGIN(GetCalculatedActorSpread, "", true, kParams_OneOptionalInt);
bool Cmd_GetCalculatedActorSpread_Execute(COMMAND_ARGS)
{
	*result = -1.0;
	if (!thisObj || !IS_ACTOR(thisObj))
		return true;

	UInt32 spreadMode = kSpreadMode_INVALID;
	ExtractArgsEx(EXTRACT_ARGS_EX, &spreadMode);

	auto* actor = static_cast<Actor*>(thisObj);
	auto* weap = actor->GetEquippedWeapon();

	if (!weap || weap->IsMelee())
	{
		*result = 1.0;
		return true;
	}

	if (!actor->baseProcess || actor->baseProcess->IsWeaponOut())
	{
		*result = 1.0;
		return true;
	}
	
	if (spreadMode == kSpreadMode_INVALID)
	{
		// Determine proper spread mode to check given current game context
		spreadMode = kSpreadMode_2; // most commonly used

		if (actor == g_thePlayer)
		{
			/*
			auto* vatsMenu = VATSMenu::GetSingleton();
			if (VATSCameraData->Get()->mode == 3)
			{
				spreadMode = kSpreadMode_VATS;
			}
			*/
			if (actor->baseProcess->IsAiming() && weap->targetNIF.nifPath.GetLength())
			{
				float filler = 0.0;
				auto* weapInfo = actor->baseProcess->GetWeaponInfo();
				if (!weap->IsDontHidePlayerWhileAiming() || 
					(weapInfo && ThisStdCall<bool>(0x4BD8D0, weapInfo, TESObjectWEAP::kWeaponModEffect_IncreaseZoom, &filler))) 
					// ApplyWeaponModModifiers
				{
					spreadMode = kSpreadMode_Scoped;
				}
			}
		}
	}

	*result = ThisStdCall<double>(0x8B0DD0, thisObj, spreadMode);

	if (actor == g_thePlayer)
	{
		if (spreadMode == kSpreadMode_Scoped)
		{
			*result *= GetFltGameSetting(0x11CF718); // gs_fGunWobbleMultScope
		}
		else
		{
			if (!actor->IsDoingAttackAnimation())
			{
				*result *= GetFltGameSetting(0x11CEC34); // gs_fNonAttackGunWobbleMult
			}
			// Copying weird code starting at 0x96309E
			const auto& unkGunWobbleDriftGlobal = *reinterpret_cast<float*>(0x11A3B2C);
			float v60 = *result - unkGunWobbleDriftGlobal;
			float v61 = g_timeGlobal->secondsPassed * GetFltGameSetting(0x11CF588); // fGunWobbleChaseDriftTime
			if (v60 != 0.0)
			{
				if (v61 < fabs(v60))
				{
					if (v60 <= 0.0)
						*result = unkGunWobbleDriftGlobal - v61;
					else if (!actor->IsDoingAttackAnimation())
						*result = unkGunWobbleDriftGlobal + v61;
				}
			}
		}
	}
	else // actor is an NPC
	{
		*result *= GetFltGameSetting(0x11CE034); // gs_fNPCMaxGunWobbleAngle
	}

	return true;
}






#ifdef _DEBUG










DEFINE_CMD_ALT_COND_PLUGIN(GetHealthExtraData, GetExtraHealth, "", true, NULL);
bool Cmd_GetHealthExtraData_Eval(COMMAND_ARGS_EVAL)
{
	*result = -1;
	if (!thisObj) return true;
	if (auto xHealth = (ExtraHealth*)thisObj->extraDataList.GetByType(kExtraData_Health))
	{
		*result = xHealth->health;
	}
	return true;
}
bool Cmd_GetHealthExtraData_Execute(COMMAND_ARGS)
{
	return Cmd_GetHealthExtraData_Eval(thisObj, 0, 0, result);
}


DEFINE_CMD_COND_PLUGIN(HasAnyScriptPackage, "", true, NULL);
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

DEFINE_COMMAND_PLUGIN(SayTo_GetUnk, "", true, NULL);
bool Cmd_SayTo_GetUnk_Execute(COMMAND_ARGS)
{
	*result = 0;
	if (NOT_ACTOR(thisObj)) return true;
	if (ExtraSayToTopicInfo* xSayTo = GetExtraTypeJIP(&thisObj->extraDataList, SayToTopicInfo)) 
	{
		//REFR_RES = ThisStdCall<Actor*>(0x8A553C, ((Actor*)thisObj))->refID;
	}
	return true;
}


#endif