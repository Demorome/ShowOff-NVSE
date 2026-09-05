#pragma once

//Code ripped from both JIP (GetActorsByProcessingLevel) and SUP (FindClosestActorFromRef).
uint32_t __fastcall GetNumActorsInRangeFromRef_Call(TESObjectREFR* const thisObj, float const range, uint32_t const flags)
{
	if (range <= 0) return 0;
	if (!thisObj) return 0;
	bool const isThisObjActor = thisObj->IsActor();
	
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
		_MESSAGE("DebugGetNumActorsInRangeFromRef - begin dump for thisObj %s (%08x)", thisObj->GetName(), thisObj->GetFormID());
	
	MobileObject** objArray = ProcessLists::GetSingleton()->objects.data, ** arrEnd = objArray;
	objArray += ProcessLists::GetSingleton()->beginOffsets[0];  //Only objects in High process.
	arrEnd += ProcessLists::GetSingleton()->endOffsets[0];
	uint32_t numActors = 0;  //return value
	for (; objArray != arrEnd; objArray++)
	{
		auto actor = (Actor*)*objArray;
		if (actor && actor->IsActor() && actor != thisObj)
		{
			if (g_ShowFuncDebug)
				_MESSAGE("Current actor >>> %08x (%s). isDead: %d, distance: %f", actor->GetFormID(), actor->GetName(), actor->GetDead(), GetDistance3D(thisObj, actor));
		
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
	if (thisObj != PlayerCharacter::GetSingleton())
	{
		if (noDeadActors && PlayerCharacter::GetSingleton()->GetDead())
			return numActors;
		if (GetDistance3D(thisObj, PlayerCharacter::GetSingleton()) <= range)
			numActors++;
	}

	return numActors; 
}

DEFINE_CMD_COND_PLUGIN(GetNumActorsInRangeFromRef, "Returns the amount of actors that are a certain distance nearby to the calling reference.", 
	true, kParams_OneFloat_OneOptionalInt);
bool Cmd_GetNumActorsInRangeFromRef_Eval(COMMAND_ARGS_EVAL)
{
	*result = GetNumActorsInRangeFromRef_Call(thisObj, *(float*)&arg1, (uint32_t)arg2);
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

uint32_t __fastcall GetNumCombatActorsFromActorCALL(TESObjectREFR* thisObj, float range, uint32_t flags)
{
	if (!thisObj) return 0;
	if (!thisObj->IsActor()) return 0;
	//Even if the calling actor is dead, they could still have combat targets, so we don't filter that out.

	if (g_ShowFuncDebug)
		_MESSAGE("DebugGetNumActorsInRangeFromRef - begin dump for thisObj %s (%08x)", thisObj->GetName(), thisObj->GetFormID());

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
				_MESSAGE("Current actor >>> %08x (%s). isDead: %d, distance: %f", actor->GetFormID(), actor->GetName(), actor->GetDead(), GetDistance3D(thisObj, actor));

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
	if (thisObj == PlayerCharacter::GetSingleton())
	{
		CombatActors* cmbActors = PlayerCharacter::GetSingleton()->combatActors;
		if (!cmbActors) return 0;
		if (getAllies)
		{
			CombatAlly* allies = cmbActors->allies.data;
			for (uint32_t count = cmbActors->allies.size; count; count--, allies++)
			{
				actor = allies->ally;
				IncrementNumActorsIfChecksPass(actor);
			}
		}
		if (getTargets)
		{
			CombatTarget* targets = cmbActors->targets.data;
			for (uint32_t count = cmbActors->targets.size; count; count--, targets++)
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
				for (uint32_t count = actor->combatAllies->size; count; count--, actorsArr++)
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
				for (uint32_t count = actor->combatTargets->size; count; count--, actorsArr++)
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
	*result = GetNumCombatActorsFromActorCALL(thisObj, *(float*)&arg1, (uint32_t)arg2);
	return true;
}
bool Cmd_GetNumCombatActorsFromActor_Execute(COMMAND_ARGS)
{
	float range = 0.0F;
	uint32_t flags = 0;
	if (ExtractArgs(EXTRACT_ARGS, &range, &flags))
		*result = GetNumCombatActorsFromActorCALL(thisObj, range, flags);
	else
		*result = 0;
	return true;
}

//copied after GetCreatureCombatSkill from JG
DEFINE_CMD_COND_PLUGIN(GetCreatureTurningSpeed, "", false, kParams_OneOptionalActorBase); 
bool Cmd_GetCreatureTurningSpeed_Eval(COMMAND_ARGS_EVAL) {
	*result = -1;

	TESForm* pForm = nullptr;
	if (arg1)
		pForm = (TESForm*)arg1;
	else if (thisObj)
		pForm = thisObj->baseForm;

	if (pForm && IS_ID(pForm, TESCreature))
		*result = static_cast<TESCreature*>(pForm)->turningSpeed;

	return true;
}

bool Cmd_GetCreatureTurningSpeed_Execute(COMMAND_ARGS) {
	*result = -1;
	TESCreature* pCreature = nullptr;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &pCreature)) 
		return true;

	return Cmd_GetCreatureTurningSpeed_Eval(thisObj, pCreature, nullptr, result);
}

// Credits to JIP LN for the SetCreature__ code format.
DEFINE_COMMAND_PLUGIN(SetCreatureTurningSpeed, "", false, kParams_OneFloat_OneOptionalActorBase);
bool Cmd_SetCreatureTurningSpeed_Execute(COMMAND_ARGS) {
	*result = 0;
	TESCreature* pCreature = nullptr;
	float fTurningSpeed = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &fTurningSpeed, &pCreature) || fTurningSpeed < 0) 
		return true;

	if (!pCreature && thisObj && thisObj->IsCreature())
		pCreature = static_cast<TESCreature*>(static_cast<Creature*>(thisObj)->GetActorBase());

	if (pCreature && IS_ID(pCreature, TESCreature)) {
		pCreature->turningSpeed = fTurningSpeed;
		*result = 1;
	}
	return true;
}

DEFINE_CMD_COND_PLUGIN(GetCreatureFootWeight, "", false, kParams_OneOptionalActorBase);
bool Cmd_GetCreatureFootWeight_Eval(COMMAND_ARGS_EVAL) {
	*result = -1;

	TESForm* pForm = nullptr;
	if (arg1)
		pForm = (TESForm*)arg1;
	else if (thisObj)
		pForm = thisObj->baseForm;

	if (pForm && IS_ID(pForm, TESCreature))
		*result = static_cast<TESCreature*>(pForm)->footWeight;

	return true;
}

bool Cmd_GetCreatureFootWeight_Execute(COMMAND_ARGS) {
	*result = -1;
	TESCreature* pCreature = nullptr;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &pCreature)) 
		return true;

	return Cmd_GetCreatureFootWeight_Eval(thisObj, pCreature, nullptr, result);
}

DEFINE_COMMAND_PLUGIN(SetCreatureFootWeight, "", false, kParams_OneFloat_OneOptionalActorBase);
bool Cmd_SetCreatureFootWeight_Execute(COMMAND_ARGS) {
	*result = 0;
	TESCreature* pCreature = nullptr;
	float fFootWeight = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &fFootWeight, &pCreature) || fFootWeight < 0) 
		return true;

	if (!pCreature && thisObj && thisObj->IsCreature())
		pCreature = static_cast<TESCreature*>(static_cast<Creature*>(thisObj)->GetActorBase());

	if (pCreature && IS_ID(pCreature, TESCreature)) {
		pCreature->footWeight = fFootWeight;
		*result = 1;
	}
	return true;
}

DEFINE_COMMAND_PLUGIN(SetCreatureReach, "", false, kParams_OneInt_OneOptionalActorBase);
bool Cmd_SetCreatureReach_Execute(COMMAND_ARGS) {
	*result = 0;
	TESCreature* pCreature = nullptr;
	uint32_t uiReach = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &uiReach, &pCreature) || uiReach < 0) 
		return true;

	if (!pCreature && thisObj && thisObj->IsCreature())
		pCreature = static_cast<TESCreature*>(static_cast<Creature*>(thisObj)->GetActorBase());

	if (pCreature && IS_ID(pCreature, TESCreature)) {
		pCreature->attackReach = uiReach;
		*result = 1;
	}
	return true;
}

DEFINE_COMMAND_PLUGIN(SetCreatureBaseScale, "", false, kParams_OneFloat_OneOptionalActorBase);
bool Cmd_SetCreatureBaseScale_Execute(COMMAND_ARGS) {
	*result = 0;
	TESCreature* pCreature = nullptr;
	float fScale = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &fScale, &pCreature) || fScale < 0)
		return true;

	if (!pCreature && thisObj && thisObj->IsCreature())
		pCreature = static_cast<TESCreature*>(static_cast<Creature*>(thisObj)->GetActorBase());

	if (pCreature && IS_ID(pCreature, TESCreature)) {
		pCreature->baseScale = fScale;
		*result = 1;
	}
	return true;
}

//Copied JG's GetNearestCompassHostile code.
uint32_t __fastcall GetNumCompassHostiles_Call(TESObjectREFR* const thisObj, float const maxRange, uint32_t flags)
{
	enum FunctionFlags
	{
		kFlag_SkipInvisible = 1,
		kFlag_Max = kFlag_SkipInvisible,  // | someOtherFlag
	};
	if (!flags) flags = kFlag_Max;

	bool const skipInvisible = flags & kFlag_SkipInvisible;
	uint32_t numHostiles = 0;  //result

	//To avoid counting "compass targets" that are super far away and can't even be seen on compass (I assume).
	//todo: learn why this stuff is checked!
	float fSneakMaxDistance = *(float*)(0x11CD7D8 + 4);
	float fSneakExteriorDistanceMult = *(float*)(0x11CDCBC + 4);
	bool isInterior = PlayerCharacter::GetSingleton()->GetParentCell()->IsInterior();
	float interiorDistanceSquared = fSneakMaxDistance * fSneakMaxDistance;
	float exteriorDistanceSquared = (fSneakMaxDistance * fSneakExteriorDistanceMult) * (fSneakMaxDistance * fSneakExteriorDistanceMult);
	float maxDist = isInterior ? interiorDistanceSquared : exteriorDistanceSquared;

	NiPoint3* playerPos = PlayerCharacter::GetSingleton()->GetPos();
	auto iter = PlayerCharacter::GetSingleton()->compassTargets->Begin();
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
	auto const flags = (uint32_t)arg2;
	*result = GetNumCompassHostiles_Call(thisObj, max_range, flags);
	return true;
}
bool Cmd_GetNumCompassHostiles_Execute(COMMAND_ARGS)
{
	float max_range = 0;
	uint32_t flags = 0;
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
	if (thisObj && thisObj->IsActor()) {
		Actor* pActor = static_cast<Actor*>(thisObj);
		uint32_t avCode = (uint32_t)arg1;
		float fDamage = pActor->avOwner.GetActorValueDamage(avCode);
		if (fDamage != 0.0F)  // avoid having -0.0
			fDamage *= -1; // multiply by -1 to invert the sign, since it otherwise gives the negative damage modifier.
		*result = fDamage;  
	}
	return true;
}
bool Cmd_GetActorValueDamage_Execute(COMMAND_ARGS)
{
	*result = -1;
	uint32_t avCode;
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
		if (!thisObj->IsActor()) 
			return true;
		
		ExtraSayToTopicInfo* xSayTo = GetExtraTypeJIP(&thisObj->extraDataList, SayToTopicInfo);
		if (xSayTo) {
			switch (request)
			{
			case kData_Speaker:
				REFR_RES = xSayTo->speaker->GetFormID();
				break;
			case kData_Topic:
				REFR_RES = xSayTo->topic->GetFormID();
				break;
			case kData_TopicInfo:
				REFR_RES = xSayTo->info->GetFormID();
				break;
			case kData_Quest:
				REFR_RES = xSayTo->quest->GetFormID();
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
	TESForm* pBase = nullptr;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &pBase)) 
		return true;
	if (auto const form = TryGetBaseFormOrREFR(pBase, thisObj))
	{
		*result = form->IsActorAlt();
	}
	return true;
}

//bug: despite not calling the savebake function, this is how being savebaked still.
DEFINE_COMMAND_ALT_PLUGIN(SetSecuritronExpressionTemp, SetSecExpTemp, "", false, kParams_OneReference_TwoStrings);
bool Cmd_SetSecuritronExpressionTemp_Execute(COMMAND_ARGS)
{
	*result = 0;
	Actor* pActor = nullptr;
	char cPersonality[512];
	char cMood[512];
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &pActor, cPersonality, cMood) || !pActor->IsActor())
		return true;

	NiNode* pRoot = pActor->GetNiNode();
	if (!pRoot)
		return true;

	ExtraSecuritronFace* pFaceData = GetExtraTypeJIP(&pActor->extraDataList, SecuritronFace);
	if (!pFaceData) {
		pFaceData = ExtraSecuritronFace::Create();
		pActor->extraDataList.Add(pFaceData);
	}
	pFaceData->SetPersonality(cPersonality);
	pFaceData->SetMood(cMood);
	pFaceData->ApplyFace(pRoot);
	*result = 1;
	return true;
}

DEFINE_CMD_ALT_COND_PLUGIN(GetIsPlayerOverencumbered, IsPCOverencumbered, 
	"An alternative to SUP's IsPlayerOverencumbered", false, kParams_OneOptionalInt);
bool Cmd_GetIsPlayerOverencumbered_Eval(COMMAND_ARGS_EVAL)
{
	// Imitate PlayerCharacter::GetIsOverencumbered at 0x954CC0.
	if (bool const ignoreGodMode = reinterpret_cast<uint32_t>(arg1) != 0; 
		!ignoreGodMode)
	{
		if (GetIsGodMode()) {
			*result = 0;
			return true;
		}
	}
	auto const weight = PlayerCharacter::GetSingleton()->avOwner.GetActorValueInt(kAVCode_InventoryWeight);
	// Actor::GetMaxCarryWeightPerkModified
	auto const maxWeight = PlayerCharacter::GetSingleton()->GetMaxCarryWeight();
	*result = maxWeight < weight;
	return true;
}
bool Cmd_GetIsPlayerOverencumbered_Execute(COMMAND_ARGS)
{
	uint32_t ignoreGodMode = false;
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

	uint32_t spreadMode = kSpreadMode_INVALID;
	ExtractArgsEx(EXTRACT_ARGS_EX, &spreadMode);

	if (!thisObj || !thisObj->IsActor())
		return true;

	auto* actor = static_cast<Actor*>(thisObj);
	auto* weap = actor->GetEquippedWeapon();

	if (!weap || weap->IsMelee())
	{
		*result = 1.0;
		return true;
	}

	if (!actor->baseProcess || !actor->baseProcess->IsWeaponOut())
	{
		*result = 1.0;
		return true;
	}
	
	if (spreadMode == kSpreadMode_INVALID)
	{
		// Determine proper spread mode to check given current game context
		spreadMode = kSpreadMode_2; // most commonly used

		if (actor == PlayerCharacter::GetSingleton())
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
					(weapInfo && ThisCall<bool>(0x4BD8D0, weapInfo, TESObjectWEAP::kWeaponModEffect_IncreaseZoom, &filler))) 
					// ApplyWeaponModModifiers
				{
					spreadMode = kSpreadMode_Scoped;
				}
			}
		}
	}

	*result = ThisCall<double>(0x8B0DD0, thisObj, spreadMode);

	if (actor == PlayerCharacter::GetSingleton())
	{
		if (spreadMode == kSpreadMode_Scoped)
		{
			*result *= GetFltGameSetting(0x11CF718); // gs_fGunWobbleMultScope
		}
		else
		{
			if (!actor->IsAttacking())
			{
				*result *= GetFltGameSetting(0x11CEC34); // gs_fNonAttackGunWobbleMult
			}
			// Copying weird code starting at 0x96309E
			const auto& unkGunWobbleDriftGlobal = *reinterpret_cast<float*>(0x11A3B2C);
			float v60 = *result - unkGunWobbleDriftGlobal;
			float v61 = TimeGlobal::GetSingleton()->secondsPassed * GetFltGameSetting(0x11CF588); // fGunWobbleChaseDriftTime
			if (v60 != 0.0)
			{
				if (v61 < fabs(v60))
				{
					if (v60 <= 0.0)
						*result = unkGunWobbleDriftGlobal - v61;
					else if (!actor->IsAttacking())
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

bool(*Cmd_HighlightAdditionalReference)(COMMAND_ARGS) = (bool (*)(COMMAND_ARGS)) 0x5BB610;

DEFINE_COMMAND_PLUGIN(HighlightAdditionalReferenceAlt, "", true, kParams_FourOptionalFloats);
bool Cmd_HighlightAdditionalReferenceAlt_Execute(COMMAND_ARGS)
{
	*result = 0; // bSuccess

	float checkInvisibility = 1.0f;
	float setFlashing = 1.0f;
	float filler[2];
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &checkInvisibility, &setFlashing, &filler))
		return true;

	if (!thisObj)
		return true;

	if (thisObj->IsActor() && checkInvisibility)
	{
		auto* actor = static_cast<Actor*>(thisObj);
		if (actor->avOwner.GetActorValue(kAVCode_Invisibility) > 0 || actor->avOwner.GetActorValue(kAVCode_Chameleon) > 0)
		{
			float hasPerk = 0.0;
			ApplyPerkModifiers(kPerkEntry_HasImprovedDetection, PlayerCharacter::GetSingleton(), &hasPerk);
			if (!hasPerk)
				return true;
		}
	}

	if (!setFlashing)
	{
		// Prevent flashing
		NopFunctionCall(0x5BB693, 1); // VATSHighlightData::SetFlashingRef

#if 0	
		// Doesn't work as expected: tried to prevent a crash when game sets highlighted projectile to be target ref,
		// ...but if there's no target ref, no highlighting happens!

		// Avoid setting a target ref
		SafeWrite8(0x800DFF, 0xEB); // replace jnz with short jmp
#endif

		auto* manager = InterfaceManager::GetSingleton();
		if (manager)
		{
			// If ref was already flashing, make it stop.
			if (manager->vatsHighlightData.flashingRefIndex != -1)
			{
				for (int i = 0; i < manager->vatsHighlightData.numHighlightedRefs; ++i)
				{
					if (manager->vatsHighlightData.highlightedRefs[i].ref == thisObj)
					{
						manager->vatsHighlightData.flashingRefIndex = -1;
						break;
					}
				}
			}
		}
	}

	*result = thisObj->Get3DSimple() != nullptr;
	Cmd_HighlightAdditionalReference(PASS_COMMAND_ARGS);

	if (!setFlashing)
	{
		// Restore being able to set flashing
		WriteRelCall(0x5BB693, 0x800E50);

#if 0
		// Restore setting a default target ref
		SafeWrite8(0x800DFF, 0x75);
#endif
	}

	if (!IsInVATS())
	{
		// Force the target to be set as player.
		// This will protect against crashes where target becomes destroyed (i.e. trying to highlight a projectile that explodes).
		// For example, I was getting a crash at 0x800483 sometimes when highlighting was enabled outside of VATS on a dynamite, right after it blew up.
		// This was likely because the game was storing the target.ref of the now-deleted projectile reference, and attempting to check stuff on it.
		// Target has no visual effect outside of VATS, so we don't lose anything.
		auto& vatsHighlightData = InterfaceManager::GetSingleton()->vatsHighlightData;
		vatsHighlightData.target.ref = PlayerCharacter::GetSingleton();
		NiRefObject::Replace((NiRefObject**)&vatsHighlightData.target.node, PlayerCharacter::GetSingleton()->Get3DSimple());

		// NOTE: still highly recommended to flush the VATS highlight targets and re-create the list every frame.
		// Especially since having the player stay as a target for VATS would be weird...
	}

	return true;

}

DEFINE_COMMAND_PLUGIN(ForceRecoilAnim, "", true, nullptr);
bool Cmd_ForceRecoilAnim_Execute(COMMAND_ARGS)
{
	*result = 0; // bSuccess
	if (!thisObj || !thisObj->IsActor())
		return true;

	auto* weap = ((Actor*)thisObj)->GetEquippedWeapon();
	if (!weap || (weap->IsMelee() && !weap->projectile))
	{
		static_cast<Actor*>(thisObj)->Recoil();
		*result = 1;
	}
	return true;
}

DEFINE_COMMAND_ALT_PLUGIN(ForceHitStaggerReaction, ForceHitReaction, "Only works inside OnHit event handlers.",
	true, kParams_OneOptionalInt);
bool Cmd_ForceHitStaggerReaction_Execute(COMMAND_ARGS)
{
	*result = 0; // bSuccess

	if (!thisObj || !thisObj->IsActor())
		return true;

	uint32_t checkForIgnoreCrippledLimbsAV = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &checkForIgnoreCrippledLimbsAV))
		return true;
	auto* actor = static_cast<Actor*>(thisObj);
	if (!actor->baseProcess)
		return true;

	if (auto* hitData = actor->baseProcess->GetHitData();
		hitData && hitData->hitLocation != -1)
	{
		if (actor->baseProcess->GetKnockState())
			return true;
		if (checkForIgnoreCrippledLimbsAV && actor->avOwner.GetActorValueInt(eActorVal_IgnoreCrippledLimbs))
			return true;

		// Needed so that the idle anims with conditions for GetForceHitReaction will play.
		actor->forceHit = true;

		constexpr uint32_t g_idleAnimsDirectoryMap_Addr = 0x11CB6A0;
		auto* idle = ThisCall<TESIdleForm*>(0x600950, *(void**)g_idleAnimsDirectoryMap_Addr, actor, hitData->projectile);
		if (!idle)
			return true;

		auto* animData = actor->GetAnimData();
		if (!animData || ThisCall<bool>(0x498D30, animData, idle)) // checks for queued anims and VATS
			return true;

		// copying code at 0x89C2DA; credits to lStewieAl for pointing this out!
		if (actor == PlayerCharacter::GetSingleton())
		{
			animData->SpecialIdleAuto(idle, actor, idle->GetSequenceID(), 2);
			// todo: maybe destroy VATS camera structs like how 0x89C370 does it.
		}
		else
		{
			actor->baseProcess->SetCurrentProcessIdle(idle);
			actor->baseProcess->AddPostAnimationAction(0x10); // kIdleFlag_CrippledLimb
		}
		actor->forceHit = false;
		*result = 1;
	}
	return true;
}

DEFINE_COMMAND_PLUGIN(GetActorFadeState, "", true, nullptr);
bool Cmd_GetActorFadeState_Execute(COMMAND_ARGS)
{
	*result = -1;
	if (!thisObj || !thisObj->IsActor())
		return true;

	auto* actor = static_cast<Actor*>(thisObj);
	if (actor->baseProcess && actor->baseProcess->processLevel == 0) // high process
	{
		auto* highProc = static_cast<HighProcess*>(actor->baseProcess);
		*result = highProc->fadeType;
	}
	return true;
}

DEFINE_CMD_COND_PLUGIN(IsActorInvisibleToPlayer, "", true, nullptr);
bool Cmd_IsActorInvisibleToPlayer_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	if (!thisObj || !thisObj->IsActor())
		return true;
	auto* actor = static_cast<Actor*>(thisObj);
	if (!actor->IsInvisible())
		return true;
	float hasImprovedDetection = 0.0f;
	ApplyPerkModifiers(kPerkEntry_HasImprovedDetection, PlayerCharacter::GetSingleton(), &hasImprovedDetection);
	*result = !hasImprovedDetection;
	return true;
}
bool Cmd_IsActorInvisibleToPlayer_Execute(COMMAND_ARGS)
{
	return Cmd_IsActorInvisibleToPlayer_Eval(thisObj, nullptr, nullptr, result);
}

DEFINE_COMMAND_PLUGIN(BaseCopyFaceGenFrom, "", false, kParams_TwoActorBases);
bool Cmd_BaseCopyFaceGenFrom_Execute(COMMAND_ARGS)
{
	TESNPC* srcNPC = nullptr;
	TESNPC* destNPC = nullptr;
	*result = 0;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &srcNPC, &destNPC) && srcNPC && IS_ID(srcNPC, TESNPC) && destNPC && IS_ID(destNPC, TESNPC))
	{
		destNPC->SetSex(srcNPC->baseData.flags);
		destNPC->SetRace(srcNPC->race.race);
		destNPC->CopyAppearance(srcNPC);
		*result = 1;
	}
	return true;
}