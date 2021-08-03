#pragma once
#include "Hooks.h"
#include "SafeWrite.h"

DEFINE_COMMAND_ALT_PLUGIN(SetPlayerCanPickpocketEquippedItems, SetPCCanStealEqItems, "Toggles the ability to pickpocket equipped items.", 0, 1, kParams_OneInt);
DEFINE_CMD_ALT_COND_PLUGIN(GetPlayerCanPickpocketEquippedItems, GetPCCanStealEqItems, "Checks if the player can pickpocket equipped items.", 0, NULL);
DEFINE_CMD_ALT_COND_PLUGIN(GetPCHasSleepWaitOverride, , "Returns whether or not the player has a Sleep/Wait prevention override", 0, NULL);
DEFINE_COMMAND_PLUGIN(SetPCHasSleepWaitOverride, "Sets whether or not the player has a Sleep/Wait prevention override", 0, 1, kParams_OneInt);
DEFINE_CMD_ALT_COND_PLUGIN(IsWeaponMelee, , "Returns 1 if the weapon's base form is of one of the three weapon types belonging to melee-range weapons.", 1, kParams_OneOptionalObjectID);
DEFINE_CMD_ALT_COND_PLUGIN(IsEquippedWeaponMelee, , "Returns 1 if the calling actor's equipped weapon's base form is of one of the three weapon types belonging to melee-range weapons.", 1, NULL);
DEFINE_CMD_ALT_COND_PLUGIN(IsWeaponRanged, , "Returns 1 if the weapon's base form is one of the weapon types belonging to NON melee-range weapons.", 1, kParams_OneOptionalObjectID);
DEFINE_CMD_ALT_COND_PLUGIN(IsEquippedWeaponRanged, , "Returns 1 if the calling actor's equipped weapon's base form is one of the weapon types belonging to NON melee-range weapons.", 1, NULL);
DEFINE_CMD_ALT_COND_PLUGIN(GetChallengeProgress, , "Returns the progress made on a challenge.", 0, kParams_OneChallenge)
DEFINE_COMMAND_PLUGIN(UnequipItems, , true, 4, kParams_FourOptionalInts);
DEFINE_COMMAND_PLUGIN(GetEquippedItems, , true, 1, kParams_OneOptionalInt);
DEFINE_CMD_ALT_COND_PLUGIN(GetPCHasScriptedFastTravelOverride, , "Returns whether or not the player is restricted by EnableFastTravel", 0, NULL);
DEFINE_CMD_ALT_COND_PLUGIN(GetPCCanFastTravel, , , false, NULL);
DEFINE_CMD_ALT_COND_PLUGIN(GetWeaponHasFlag, WeaponHasFlag, , false, kParams_OneInt_OneOptionalObjectID);
DEFINE_CMD_ALT_COND_PLUGIN(GetActorHasBaseFlag, ActorHasBaseFlag, , false, kParams_OneInt_OneOptionalActorBase);
DEFINE_COMMAND_PLUGIN(RemoveAllItemsShowOff, , true, 4, kParams_TwoOptionalInts_OneOptionalContainerRef_OneOptionalList);
DEFINE_COMMAND_PLUGIN(ForceWeaponJamAnim, ForceJamAnim, true, 0, NULL);
DEFINE_CMD_ALT_COND_PLUGIN(GetCalculatedSkillPoints, GetCalculatedSkillPointsEarnedPerLevel, "Gets the amount of skill points the player would get each level.", false, kParams_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(GetLevelUpMenuPoints, , false, 2, kParams_TwoOptionalInts);


bool Cmd_SetPlayerCanPickpocketEquippedItems_Execute(COMMAND_ARGS)
{
	UInt32 bOn;
	if (NUM_ARGS && ExtractArgsEx(EXTRACT_ARGS_EX, &bOn))
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
	return true;
}

bool Cmd_GetPlayerCanPickpocketEquippedItems_Eval(COMMAND_ARGS_EVAL)
{
	*result = canPlayerPickpocketEqItems();
	return true;
}
bool Cmd_GetPlayerCanPickpocketEquippedItems_Execute(COMMAND_ARGS)
{
	*result = canPlayerPickpocketEqItems();
	return true;
}

bool Cmd_GetPCHasSleepWaitOverride_Eval(COMMAND_ARGS_EVAL)
{
	*result = !g_thePlayer->canSleepWait;
	return true;
}
bool Cmd_GetPCHasSleepWaitOverride_Execute(COMMAND_ARGS)
{
	Cmd_GetPCHasSleepWaitOverride_Eval(thisObj, NULL, NULL, result);
	return true;
}

bool Cmd_SetPCHasSleepWaitOverride_Execute(COMMAND_ARGS)
{
	UInt32 bOn;
	if (ExtractArgs(EXTRACT_ARGS, &bOn))
		g_thePlayer->canSleepWait = !bOn;
	return true;
}

bool Cmd_IsWeaponMelee_Eval(COMMAND_ARGS_EVAL)
{
	//Console_Print("thisObj: [%0.8X]", thisObj->baseForm->GetId());
	*result = 0;
	TESForm* form;
	if (arg1)
	{
		form = (TESForm*)arg1;
	}
	else if (thisObj)
	{
		form = thisObj->baseForm;
	}
	else return true;

	auto const weapon = DYNAMIC_CAST(form, TESForm, TESObjectWEAP);
	if (!weapon) return true;

	UINT8 weapType = weapon->eWeaponType;
	*result = weapType <= 2;
	return true;
}
bool Cmd_IsWeaponMelee_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESObjectWEAP* weapon = NULL;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &weapon)) return true;

	return Cmd_IsWeaponMelee_Eval(thisObj, weapon, 0, result);
}

bool Cmd_IsEquippedWeaponMelee_Eval(COMMAND_ARGS_EVAL)
{
	// Not recommended to use this function for certain perk effects, like Calculate Weap. Damage;
	// since it will affect the DAM that appears in the UI for other weapons, as long as the currently equipped weapon is a melee weap.
	*result = 0;
	if (thisObj)
	{
		if (!thisObj->IsActor()) return true;
		TESObjectWEAP* weapon = ((Actor*)thisObj)->GetEquippedWeapon();
		if (weapon)
		{
			*result = weapon->eWeaponType <= 2;
		}
		else
		{
			*result = 1;
			if (g_ShowFuncDebug)
				Console_Print("IsEquippedWeaponMelee >> 1 >> No equipped weapon found, assuming that the default unarmed is equipped.");
		}
	}
	return true;
}
bool Cmd_IsEquippedWeaponMelee_Execute(COMMAND_ARGS)
{
	return Cmd_IsEquippedWeaponMelee_Eval(thisObj, nullptr, nullptr, result);
}

bool Cmd_IsWeaponRanged_Eval(COMMAND_ARGS_EVAL)
{
	//Console_Print("thisObj: [%0.8X]", thisObj->baseForm->GetId());
	*result = 0;
	TESForm* form;
	if (arg1)
	{
		form = (TESForm*)arg1;
	}
	else if (thisObj)
	{
		form = thisObj->baseForm;
	}
	else return true;

	auto const weapon = DYNAMIC_CAST(form, TESForm, TESObjectWEAP);
	if (!weapon) return true;

	UINT8 weapType = weapon->eWeaponType;
	*result = weapType >= 3 && weapType <= 13;

	return true;
}
bool Cmd_IsWeaponRanged_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESObjectWEAP* weapon = NULL;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &weapon)) return true;

	return Cmd_IsWeaponRanged_Eval(thisObj, weapon, 0, result);
}

bool Cmd_IsEquippedWeaponRanged_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	if (thisObj)
	{
		if (!thisObj->IsActor()) return true;
		if (TESObjectWEAP* weapon = ((Actor*)thisObj)->GetEquippedWeapon())
		{
			UINT8 weapType = weapon->eWeaponType;
			*result = weapType >= 3 && weapType <= 13;
		}
	}
	return true;
}
bool Cmd_IsEquippedWeaponRanged_Execute(COMMAND_ARGS)
{
	return Cmd_IsEquippedWeaponRanged_Eval(thisObj, nullptr, nullptr, result);
}


bool Cmd_GetChallengeProgress_Execute(COMMAND_ARGS)
{
	TESChallenge* challenge;
	if (ExtractArgs(EXTRACT_ARGS, &challenge) && IS_TYPE(challenge, TESChallenge))
		*result = challenge->progress;
	else *result = 0;
	return true;
}
bool Cmd_GetChallengeProgress_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	if (arg1)
	{
		auto challenge = (TESChallenge*)arg1;
		if (IS_TYPE(challenge, TESChallenge))
			*result = challenge->progress;
	}
#if _DEBUG
	Console_Print("GetChallengeProgress >> %f", *result);
#endif
	return true;
}

//todo: Could use a lot more testing
bool Cmd_UnequipItems_Execute(COMMAND_ARGS)
{
	UInt32 flags = 0, noEquip = 0, hideMessage = 0, triggerOnUnequip = 1;
	if (!ExtractArgs(EXTRACT_ARGS, &flags, &noEquip, &hideMessage, &triggerOnUnequip) || NOT_ACTOR(thisObj)) return true;
	auto eqItems = GetEquippedItems(thisObj, flags);
	for (auto const& iter : eqItems)
	{
		ExtraDataList* xData = triggerOnUnequip ? iter.pExtraData : nullptr;
		((Actor*)thisObj)->UnequipItem(iter.pForm, 1, xData, 1, noEquip != 0, hideMessage != 0);
	}
	return true;
}

bool Cmd_GetEquippedItems_Execute(COMMAND_ARGS)
{
	UInt32 flags = 0;
	*result = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &flags) || NOT_ACTOR(thisObj)) return true;
	Vector<ArrayElementR> elems;
	auto eqItems = GetEquippedItems(thisObj, flags);
	for (auto const& iter : eqItems)
	{
		ArrayElementR elem = iter.pForm;
		elems.Append(elem);
	}
	auto const array = CreateArray(elems.Data(), elems.Size(), scriptObj);
	AssignArrayResult(array, result);
	return true;
}

bool Cmd_GetPCHasScriptedFastTravelOverride_Eval(COMMAND_ARGS_EVAL)
{
	*result = (g_thePlayer->byte66D & 1) == 0;
	return true;
}
bool Cmd_GetPCHasScriptedFastTravelOverride_Execute(COMMAND_ARGS)
{
	*result = (g_thePlayer->byte66D & 1) == 0;
	return true;
}

bool Cmd_GetPCCanFastTravel_Eval(COMMAND_ARGS_EVAL)
{
	// Credits to Jazz for the "silence QueueUIMessage" trick (see AddNoteNS).
	SafeWrite8((UInt32)QueueUIMessage, 0xC3);	// RETN
	auto canFastTravelAddr = GetRelJumpAddr(0x798026); // call the function indirectly for compatibility with Stewie tweaks, kudos to Stewie.
	*result = ThisStdCall<bool>(canFastTravelAddr, g_thePlayer);
	//*result = ThisStdCall<bool>((UInt32)0x93D660, g_thePlayer);
	SafeWrite8((UInt32)QueueUIMessage, 0x55);	// PUSH EBP
	return true;
}
bool Cmd_GetPCCanFastTravel_Execute(COMMAND_ARGS)
{
	return Cmd_GetPCCanFastTravel_Eval(thisObj, 0, 0, result);
}

bool Cmd_GetWeaponHasFlag_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	UInt32 flagToCheck = (UInt32)arg1;
	if (flagToCheck > 21) return true;
	TESForm* form;
	if (arg2) form = (TESForm*)arg2;
	else if (thisObj) form = thisObj->baseForm;
	else return true;
	auto const weapon = DYNAMIC_CAST(form, TESForm, TESObjectWEAP);
	if (!weapon) return true;
	if (flagToCheck < 8)  //check Flags1 (0-7)
	{
		*result = weapon->weaponFlags1.Extract(flagToCheck);
	}
	else  //check Flags2 (0-13)
	{
		flagToCheck -= 8;  //set the base to 0. At flagToCheck == 8, this equals 0.
		*result = weapon->weaponFlags2.Extract(flagToCheck);
	}
	return true;
}
bool Cmd_GetWeaponHasFlag_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 flagToCheck;
	TESObjectWEAP* weapon = NULL;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &flagToCheck, &weapon)) return true;
	return Cmd_GetWeaponHasFlag_Eval(thisObj, (void*)flagToCheck, weapon, result);
}

bool Cmd_GetActorHasBaseFlag_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	UInt32 flagToCheck = (UInt32)arg1;
	if (flagToCheck > 31) return true;
	TESForm* form;
	if (arg2) form = (TESForm*)arg2;
	else if (thisObj) form = thisObj->baseForm;
	else return true;
	auto const actor = DYNAMIC_CAST(form, TESForm, TESActorBase);
	if (!actor) return true;
	if (flagToCheck < 16)  //check FlagsLow (0-15)
	{
		UInt32 lowFlags = actor->baseData.flags & 0xFFFF;  //copied from NVSE's GetActorBaseFlagsLow
		*result = (lowFlags >> flagToCheck) & 1;
	}
	else  //check FlagsHigh (0-15)
	{
		flagToCheck -= 16;  //set the base to 0. At flagToCheck == 16, this equals 0.
		UInt32 highFlags = (actor->baseData.flags >> 16) & 0xFFFF;  //copied from NVSE's GetActorBaseFlagsHigh
		*result = (highFlags >> flagToCheck) & 1;
	}
	return true;
}
bool Cmd_GetActorHasBaseFlag_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 flagToCheck;
	TESActorBase* actor = NULL;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &flagToCheck, &actor)) return true;
	return Cmd_GetActorHasBaseFlag_Eval(thisObj, (void*)flagToCheck, actor, result);
}

bool Cmd_RemoveAllItemsShowOff_Execute(COMMAND_ARGS)
{
	*result = 0;
	enum FunctionFlags
	{
		kFlag_None = 0,
		kFlag_RetainOwnership =						1 << 0,  //only applies if a target is specified
		kFlag_SuppressMessages =					1 << 1,  //only applies if a target is specified
		kFlag_AllowRemovalOfQuestItemsFromPlayer =	1 << 2,
		kFlag_AllowRemovalOfUnplayableBipedItems =	1 << 3,
		kFlag_Unk1 =								1 << 4,  // todo: figure out what these two bools do for the vanilla function.
		kFlag_Unk2 =								1 << 5,
		kFlag_IgnoreAllUnplayableItems =			1 << 6  // Overrides kFlag_AllowRemovalOfUnplayableBipedItems. TODO: not yet implemented.
	};
	UInt32 flags = kFlag_None;  // Default
	SInt32 typeCode = -1;  //-1 = all
	TESObjectREFR* targetContainer = nullptr;
	BGSListForm* exceptionFormList = nullptr;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &flags, &typeCode, &targetContainer, &exceptionFormList)) return true;
	if (!thisObj || !typeCode || !thisObj->GetContainer() || (targetContainer && !targetContainer->GetContainer()))
		return true;
	//ExtraContainerChanges* xChanges = (ExtraContainerChanges*)thisObj->extraDataList.GetByType(kExtraData_ContainerChanges);  //ripped from NVSE's EquipItem2
	void* xChanges = ThisStdCall<void*>(0x418520, &thisObj->extraDataList);
	if (!xChanges) return true;

	// Extract flags
	bool retainOwnership = flags & kFlag_RetainOwnership;
	bool suppressMessages = flags & kFlag_SuppressMessages;
	bool removeQuestItemsIfPlayer = flags & kFlag_AllowRemovalOfQuestItemsFromPlayer;
	bool removeUnplayableBipedItems = flags & kFlag_AllowRemovalOfUnplayableBipedItems;
	bool unk1 = flags & kFlag_Unk1;
	bool unk2 = flags & kFlag_Unk2;
	bool ignoreAllUnplayableItems = flags & kFlag_IgnoreAllUnplayableItems;

	// Modify the code for RemoveAllItems
	if (removeQuestItemsIfPlayer)
	{
		WriteRelJump(0x4CE4B8, 0x4CE559);  // make the long jump at 0x4CE4B8 unconditional
		WriteRelJump(0x4CEDCE, 0x4CEE75);  // same concept
	}
	if (!ignoreAllUnplayableItems)
	{
		if (removeUnplayableBipedItems)
		{
			SafeWrite8(0x4CE571, 0xEB);  // make short jump unconditional
			PatchMemoryNop(0x4CED5D, 6);  // make the long jump never happen
		}
	}
	else
	{
		//todo: Write a jmp to extract the form mid-loop.

		
		// Check the form via IsItemPlayable. If it passes, jmp back to removal, otherwise jmp to go to the next form.
	}


	// Call RemoveAllItems with the new modifications
	ThisStdCall<void>(0x4CE340, xChanges, thisObj, targetContainer, unk1, retainOwnership, unk2, suppressMessages, typeCode, exceptionFormList);

	// Revert code modifications
	if (removeQuestItemsIfPlayer)
	{
		WriteRelJe(0x4CE4B8, 0x4CE559); // revert back to long Jump if Zero
		WriteRelJe(0x4CEDCE, 0x4CEE75); // same concept
	}
	if (!ignoreAllUnplayableItems)
	{
		if (removeUnplayableBipedItems)
		{
			SafeWrite8(0x4CE571, 0x74); // revert back to short Jump if Zero
			WriteRelJe(0x4CED5D, 0x4CFBCA); // re-write long Jump if Zero
		}
	}
	else
	{
		// TODO
	}
	
	// Wrap up
	ThisStdCall<void>(0x952C30, g_thePlayer, thisObj); // ComputeShouldRecalculateQuestTargets()
	*result = 1; //function worked as expected.
	return true;
}

bool Cmd_ForceWeaponJamAnim_Execute(COMMAND_ARGS)
{
	*result = false;
	if (IS_ACTOR(thisObj))
	{
		auto actor = (Actor*)thisObj;
		if (auto const weapn = actor->GetEquippedWeapon())
		{
			// Copies the code at 0x89667E for post-reload jamming.
			auto animGroupID = ThisStdCall<UInt32>(0x51E2A0, weapn, 0) + 23;  // TESObjectWEAP::GetReloadAnimGroup + 23
			auto animKey = ThisStdCall<UInt16>(0x897910, actor, animGroupID, 0, 0, 0);  //Actor__GetAnimKey
			if (CdeclCall<UInt32>(0x5F2440, animKey) == animGroupID)  // calls AnimGroupID::GetGroupID, which gets the lowers bits of animKey.
			{
				if (auto animData = actor->GetAnimData())
				{
					ThisStdCall<void*>(0x8B28C0, actor, animGroupID, animData);  // Actor::8B28C0
					auto const animSeqElem = animData->animSequence[4];  // 4 = kSequence_Weapon
					actor->SetAnimActionAndSequence(9, animSeqElem);  // 9 = kAnimAction_Reload
					actor->Unk_12C(animKey, true);
					*result = true;
				}
			}
		}
	}
	return true;
}


bool Cmd_GetCalculatedSkillPoints_Eval(COMMAND_ARGS_EVAL)
{
	// Vanilla code at 0x648BC0 replicated thanks to Nukem and lStewieAl's efforts (which I slightly tweaked).
	UInt32 levelOverride = 0;
	if (arg1) levelOverride = (UInt32)arg1;

	auto avOwner = &g_thePlayer->avOwner;
	auto level = levelOverride ? levelOverride : avOwner->GetLevel();
	level += LevelUpMenu::GetSingleton() ? 0 : 1;  // Add +1 level to accurately predict the outcome for the next level up, if not in levelup menu.
	auto intelligence = avOwner->GetNormalizedPermanentAV(kAVCode_Intelligence);
	intelligence = min(intelligence, 10);

	auto const calculateSkillPointsAddr = GetRelJumpAddr(0x648BF0); // get the function address indirectly for compatibility with lStewieAl's tweaks (see patchCustomSkillPointFormula())
	float skillPoints = CdeclCall<int>(calculateSkillPointsAddr, intelligence, level);
	ApplyPerkModifiers(kPerkEntry_AdjustGainedSkillPoints, g_thePlayer, &skillPoints);
	*result = skillPoints;
	return true;
}
bool Cmd_GetCalculatedSkillPoints_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 levelOverride = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &levelOverride))
		return true;
	return Cmd_GetCalculatedSkillPoints_Eval(thisObj, (void*)levelOverride, 0, result);
}


bool Cmd_GetLevelUpMenuPoints_Execute(COMMAND_ARGS)
{
	*result = -1;
	UInt32 bCheckPerks = false;  // if false, will check for Skills instead.
	UInt32 bCheckAssigned = false;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &bCheckPerks, &bCheckAssigned))
		return true;

	if (auto const menu = LevelUpMenu::GetSingleton())
	{
		if (bCheckPerks)
		{
			if (bCheckAssigned)
				*result = menu->numAssignedPerks;
			else
			{
				// menu->numPerksToAssign is always set to something in vanilla, it's the availablePerks.Empty() that determines if no perk menu is shown.
				*result = menu->availablePerks.Empty() ? 0 : menu->numPerksToAssign;
			}
		}
		else  // Check for Skills
		{
			if (bCheckAssigned)
				*result = menu->numAssignedSkillPoints;
			else
				*result = menu->numSkillPointsToAssign;
		}
	}
	return true;
}









#ifdef _DEBUG










// Kinda pointless in the face of JIP's IsMobile
DEFINE_CMD_ALT_COND_PLUGIN(CanBeMoved, , , true, NULL);
bool Cmd_CanBeMoved_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	if (!thisObj) return true;
	*result = ThisStdCall<bool>(0x572C80, thisObj);  //just does some formType check + allows any dynamic object
	return true;
}
bool Cmd_CanBeMoved_Execute(COMMAND_ARGS)
{
	return Cmd_CanBeMoved_Eval(thisObj, 0, 0, result);
}


DEFINE_COMMAND_PLUGIN(GetActorPreferredWeapon, , true, 1, kParams_OneOptionalInt);
bool Cmd_GetActorPreferredWeapon_Execute(COMMAND_ARGS)
{
	//todo: current configuration gives garbage forms, need to fix that. Perhaps not calling it right?
	*result = 0;
	UInt32 combatWeaponType = 6;
	if (!thisObj || NOT_ACTOR(thisObj) || !ExtractArgsEx(EXTRACT_ARGS_EX, &combatWeaponType))
		return true;
	auto weapForm = ThisStdCall<TESObjectWEAP*>(0x891C80, thisObj, combatWeaponType); //Actor::GetPreferredWeapon
	if (weapForm)
		REFR_RES = weapForm->refID;  
	return true;
}

DEFINE_COMMAND_PLUGIN(TryDropWeapon, , true, 0, NULL);
bool Cmd_TryDropWeapon_Execute(COMMAND_ARGS)
{
	*result = 0;
	if (!thisObj || NOT_ACTOR(thisObj))
		return true;
	Actor* actor = (Actor*)thisObj;
	if (!actor->baseProcess) return true;
	//actor->GetEquippedWeapon()
	// Retrieve info about actor's weapon, for comparison later.
	ContChangesEntry* weaponInfo = actor->baseProcess->GetWeaponInfo();
	if (!weaponInfo || !weaponInfo->type) return true;  //actor has no weapon to unequip
	SInt32 prevCount = weaponInfo->countDelta;
#if _DEBUG
	Console_Print("TryDropWeapon >> Count delta: %i", prevCount);
#endif

	ThisStdCall<void>(0x89F580, actor);  //Actor::TryDropWeapon. Triggers OnDrop blocktype.

	// Check if a weapon has been dropped.
	weaponInfo = actor->baseProcess->GetWeaponInfo();
	if (!weaponInfo || !weaponInfo->type || weaponInfo->countDelta != prevCount) *result = 1;  //weapon has been removed.
	return true;
}


DEFINE_CMD_ALT_COND_PLUGIN(IsWeaponTrowable, , , true, kParams_OneOptionalObjectID);
bool Cmd_IsWeaponTrowable_Eval(COMMAND_ARGS_EVAL)
{
	//Console_Print("thisObj: [%0.8X]", thisObj->baseForm->GetId());
	*result = 0;
	TESForm* form;
	if (arg1) form = (TESForm*)arg1;
	else if (thisObj) form = thisObj->baseForm;
	else return true;
	auto const weapon = DYNAMIC_CAST(form, TESForm, TESObjectWEAP);
	if (!weapon) return true;
	UINT8 weapType = weapon->eWeaponType;
	//*result = weapType <= 2;
	return true;
}
bool Cmd_IsWeaponTrowable_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESObjectWEAP* weapon = NULL;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &weapon)) return true;
	return Cmd_IsWeaponTrowable_Eval(thisObj, weapon, 0, result);
}

DEFINE_CMD_ALT_COND_PLUGIN(GetPCCanSleepWait, , , 0, NULL);
bool Cmd_GetPCCanSleepWait_Eval(COMMAND_ARGS_EVAL)
{
	//todo: verify if it works with script overrides and stewie features.
	*result = g_thePlayer->canSleepWait;
	return true;
}
bool Cmd_GetPCCanSleepWait_Execute(COMMAND_ARGS)
{
	*result = g_thePlayer->canSleepWait;
	return true;
}


DEFINE_CMD_ALT_COND_PLUGIN(GetPCCanSleepInOwnedBeds, , , 0, NULL);
bool Cmd_GetPCCanSleepInOwnedBeds_Eval(COMMAND_ARGS_EVAL)
{
	*result = GetCanSleepInOwnedBeds();
	return true;
}
bool Cmd_GetPCCanSleepInOwnedBeds_Execute(COMMAND_ARGS)
{
	*result = GetCanSleepInOwnedBeds();
	return true;
}

DEFINE_COMMAND_PLUGIN(SetPCCanSleepInOwnedBeds, , false, 1, kParams_OneInt);
bool Cmd_SetPCCanSleepInOwnedBeds_Execute(COMMAND_ARGS)
{
	UInt32 bOn;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &bOn)) return true;
	SetCanSleepInOwnedBeds(bOn);
	return true;
}

/*Notes on GetChallengeProgress w/ other funcs:*/
//If the challenge is beaten...
//...and is NOT set to "Recurring", will return the max Threshold value.
//......If the value was changed using SetChallengeProgress / ModChallengeProgress, its "progress" will change, but it still won't budge naturally.
//...and is set to "Recurring", will return 0. Afterwards, will increment as normal.
//......When going over the threshold using Mod/SetChallengeProgress, leftover progress is kept to increase the progress. This amount can even go over the threshold.
//......However, those functions DO NOT manually trigger challenge completion.
//......Afterwards, once triggered normally, the challenge will be completed once more, doing "Progress -= Threshold". After this, you could still have Progress > Threshold.


DEFINE_COMMAND_PLUGIN(SetChallengeProgress, "Changes the progress made on a challenge to a specific value.", 0, 2, kParams_OneForm_OneInt)
DEFINE_COMMAND_PLUGIN(ModChallengeProgress, "Modifies the progress made on a challenge.", 0, 2, kParams_OneForm_OneInt)

//Going into negative seems to work fine, it just delays the challenge progress.
//It also looks weird since it shows "0/x" progress when it returns to 0.

//FLAW TO FIX: Find a way to forcefully activate the Challenge completion.



bool Cmd_SetChallengeProgress_Execute(COMMAND_ARGS)
{
	TESChallenge* challenge;
	UInt32 value;
	if (ExtractArgs(EXTRACT_ARGS, &challenge, &value) && IS_TYPE(challenge, TESChallenge))
	{
		//if (value > challenge->threshold )
		challenge->progress = value;
		*result = 1;
	}
	else *result = 0;
	return true;
}

bool Cmd_ModChallengeProgress_Execute(COMMAND_ARGS)
{
	TESChallenge* challenge;
	UInt32 value;
	if (ExtractArgs(EXTRACT_ARGS, &challenge, &value) && IS_TYPE(challenge, TESChallenge))
	{
		//UInt32 const test_amount = challenge->amount + value;
		challenge->progress += value;
		*result = 1;
	}
	else *result = 0;
	return true;
}

DEFINE_COMMAND_PLUGIN(CompleteChallenge, "Completes a challenge.", 0, 1, kParams_OneForm)
bool Cmd_CompleteChallenge_Execute(COMMAND_ARGS)
{
	TESChallenge* challenge;
	UInt32 value;
	if (ExtractArgs(EXTRACT_ARGS, &challenge, &value) && IS_TYPE(challenge, TESChallenge))
	{
		challenge->challengeflags |= 2;
		*result = 1; //success
	}
	else *result = 0;
	return true;
}



DEFINE_COMMAND_PLUGIN(GetProjectileRefFlag, , 1, 0, NULL);
bool Cmd_GetProjectileRefFlag_Execute(COMMAND_ARGS)
{
	GrenadeProjectile* projectile = (GrenadeProjectile*)thisObj;
	//if (!(projectile->projFlags & 0x200) && (projectile->sourceRef != g_thePlayer) && ((((BGSProjectile*)thisObj->baseForm)->projFlags & 0x426) == 0x26))
	//if (!(projectile->projFlags & 0x200) && ((((BGSProjectile*)thisObj->baseForm)->projFlags & 0x426) == 0x26))
	//if (!(projectile->projFlags & 0x200))
	*result = ((Projectile*)thisObj)->projFlags;
	
	return true;
}

DEFINE_COMMAND_PLUGIN(SetProjectileRefFlag, , 1, 1, kParams_OneInt);
bool Cmd_SetProjectileRefFlag_Execute(COMMAND_ARGS)
{
	UInt32 flag;
	if (ExtractArgs(EXTRACT_ARGS, &flag))
		((Projectile*)thisObj)->projFlags = flag;

	//if ((((BGSProjectile*)thisObj->baseForm)->projFlags & 0x426) == 0x26);
	//	Console_Print("Flag was set");
	return true;
}



DEFINE_COMMAND_PLUGIN(SetPCCanPickpocketInCombat, , 0, 1, kParams_OneInt);
bool Cmd_SetPCCanPickpocketInCombat_Execute(COMMAND_ARGS)
{
	UInt32 bOn;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &bOn)) return true;
	g_canPlayerPickpocketInCombat = bOn;
	return true;
}

// Ripped code from JIPLN's "SetNoUnequip"
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

/*
DEFINE_COMMAND_PLUGIN(GetFastTravelFlags, , 0, 0, NULL);
bool Cmd_GetFastTravelFlags_Execute(COMMAND_ARGS)
{
	*result = (g_thePlayer->byte66D & 1) != 0;
	return true;
}
*/

/*
DEFINE_COMMAND_PLUGIN(SetParentRef, , 1, 1, kParams_OneOptionalForm);
bool Cmd_SetParentRef_Execute(COMMAND_ARGS)
{
	ExtraDataList* xData = thisObj ? thisObj->xData : NULL;
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
	
	return true;
}
*/

//DEFINE_CMD_ALT_COND_PLUGIN(GetEquippedWeaponType,, "Returns the type of weapon equipped by the calling actor, 0 if the calling ref isn't an actor.", 0, 1, kParams_OneOptionalObjectID);
bool GetEquippedWeaponInfo(TESObjectREFR* thisObj, float range, UInt32 flags)
{
	return true;
}




#endif