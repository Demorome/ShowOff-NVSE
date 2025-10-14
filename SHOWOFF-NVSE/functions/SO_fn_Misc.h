﻿#pragma once

#include <random>
#include <unordered_set>

#include "AuxTimers.h"
#include "GameExtraData.h"
#include "GameRTTI.h"
#include "SafeWrite.h"
#include "Utilities.h"
#include "MiscHooks.h"
#include <ShowOffNVSE.h>


DEFINE_COMMAND_ALT_PLUGIN(ShowingOffDisable, DisableIFYouDidntNotice, "Does the same thing as vanilla Disable. For showing off!", true, kParams_OneOptionalInt);
DEFINE_COMMAND_ALT_PLUGIN(ShowingOffEnable, EnableIFYouDidntNotice, "Does the same thing as vanilla Enable. For showing off!", true, kParams_OneOptionalInt);
DEFINE_COMMAND_ALT_PLUGIN(DisableAlt, SnigOff, "Ignores the EnableParent limitation.", true, kParams_OneOptionalInt);
DEFINE_COMMAND_ALT_PLUGIN(EnableAlt, SnigOn, "Ignores the EnableParent limitation.", true, kParams_OneOptionalInt);
DEFINE_COMMAND_ALT_PLUGIN(ListAddList, AddFormListToFormList, "", false, kParams_TwoFormLists_OneOptionalIndex);
DEFINE_COMMAND_PLUGIN(MessageExAltShowoff, "", false, kParams_JIP_OneFloat_OneInt_OneFormatString);
DEFINE_CMD_COND_PLUGIN(IsCornerMessageDisplayed, "Returns 1/0 depending on if a corner message is displayed.", false, NULL);
DEFINE_CMD_COND_PLUGIN(GetNumQueuedCornerMessages, "", false, NULL);
DEFINE_CMD_COND_PLUGIN(IsAnimPlayingExCond, "Same as IsAnimPlayingEx, but available as a condition. Had to cut the variationFlags filter.", true, kParams_JIP_OneInt_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(GetRadiationExtraData, "", true, NULL);
DEFINE_COMMAND_PLUGIN(SetRadiationExtraData, "", true, kParams_OneFloat);
DEFINE_CMD_COND_PLUGIN(PlayerHasNightVisionActive, "", false, NULL);
DEFINE_CMD_COND_PLUGIN(PlayerIsUsingTurbo, "", false, NULL);
DEFINE_CMD_COND_PLUGIN(PlayerHasCateyeEnabled, "", false, NULL);
DEFINE_CMD_COND_PLUGIN(PlayerHasImprovedSpottingActive, "", false, NULL);
DEFINE_CMD_COND_PLUGIN(PlayerIsDrinkingPlacedWater, "", false, NULL);
DEFINE_COMMAND_ALT_PLUGIN(SetIsPCAMurderer, YoureAMurderer, "", false , kParams_OneInt);
DEFINE_CMD_ALT_COND_PLUGIN(IsNight, BloodyTears, "Returns true if it's night according to the current (or specified) climate.", false, kParams_OneOptionalForm);
DEFINE_CMD_COND_PLUGIN(IsLimbCrippled, "If no args are passed / arg is -1, returns true if actor has any crippled limbs. Otherwise, checks if the specified limb is crippled.", true, kParams_TwoOptionalInts);
DEFINE_CMD_COND_PLUGIN(GetNumCrippledLimbs, "", true, kParams_OneOptionalInt);
DEFINE_CMD_COND_PLUGIN(GetCrippledLimbsAsBitMask, "", true, kParams_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(ClearShowoffSavedData, "", false, kParams_TwoInts);
DEFINE_CMD_ALT_COND_PLUGIN(GetCalculatedMaxCarryWeight, GetMaxCarryWeightPerkModified, "Accounts for GetMaxCarryWeight perk entry.", true, NULL);
DEFINE_COMMAND_ALT_PLUGIN(SetRandomizerSeed, SetSeed, "", false, kParams_OneOptionalInt);
DEFINE_COMMAND_ALT_PLUGIN(SetSeedUsingForm, SetFormSeed, "", false, kParams_OneOptionalForm);
DEFINE_COMMAND_ALT_PLUGIN(GetRandomizerSeed, GetSeed, "", false, NULL);
DEFINE_COMMAND_ALT_PLUGIN(RandSeeded, GenerateSeededRandomNumber, "", false, kParams_TwoInts);
DEFINE_COMMAND_PLUGIN(GetRandomPercentSeeded, "", false, NULL);
DEFINE_COMMAND_PLUGIN(AdvanceSeed, "Discards would-be generated numbers to advance in the seed generation pattern.", false, kParams_OneInt);
DEFINE_CMD_ALT_COND_PLUGIN(IsReferenceCloned, IsRefrCloned, "Checks if the reference's modIndex is 0xFF", true, NULL);
DEFINE_CMD_ALT_COND_PLUGIN(IsTemporaryReference, IsTempRefr, "Checks if the reference does not persist in the savegame.", false, NULL); //todo: Set equivalents?
DEFINE_COMMAND_PLUGIN(ToggleQuestMessages, "", false, kParams_OneOptionalInt);
DEFINE_COMMAND_PLUGIN(GetPipboyRadioVoiceEntryData, "", false, kParams_OneInt_OneOptionalInt);
DEFINE_COMMAND_ALT_PLUGIN(FormListRemoveForm, RemoveFormFromFormList, "", false, kParams_OneFormList_OneForm);
DEFINE_COMMAND_PLUGIN(GetZoneRespawns, "Returns if an Encounter Zone has the NoRespawn flag set or not.", false, kParams_OneForm);
DEFINE_COMMAND_ALT_PLUGIN(ClearCinematicTextQueue, ClearQuestMessageQueue, "", false, NULL);
DEFINE_COMMAND_ALT_PLUGIN(GetCellEncounterZone, GetCellZone, "", false, kParams_OneForm);
DEFINE_COMMAND_ALT_PLUGIN(RemoveFormFromLeveledList, SorryYoureNotOnTheListAnymore, "", false, kParams_TwoForms);
DEFINE_COMMAND_ALT_PLUGIN(ResetInteriorAlt, IWantThisRoomCleanRightNow, "", false, kParams_OneInteriorCell);
DEFINE_COMMAND_ALT_PLUGIN(GetActorValueName, GetAVName, "", false, kParams_OneActorValue_OneOptionalInt);


bool(__cdecl* Cmd_Disable)(COMMAND_ARGS) = (bool(__cdecl*)(COMMAND_ARGS)) 0x5C45E0;
bool Cmd_ShowingOffDisable_Execute(COMMAND_ARGS) {
	return Cmd_Disable(PASS_COMMAND_ARGS);
}
bool(__cdecl* Cmd_Enable)(COMMAND_ARGS) = (bool(__cdecl*)(COMMAND_ARGS)) 0x5C43D0;
bool Cmd_ShowingOffEnable_Execute(COMMAND_ARGS) {
	return Cmd_Enable(PASS_COMMAND_ARGS);
}

bool Cmd_DisableAlt_Execute(COMMAND_ARGS)
{
	// Modify code to skip over the "If this has an EnableParent" check.
	WriteRelJump(0x5C465D, 0x5C4740);

	bool const success = Cmd_Disable(PASS_COMMAND_ARGS);

	// Undo code modification.
	WriteRelJe(0x5C465D, 0x5C4740);

	return success;
}

bool Cmd_EnableAlt_Execute(COMMAND_ARGS)
{
	// Modify code to skip over the "If this has an EnableParent" check.
	WriteRelJump(0x5C4437, 0x5C451D);

	bool const success = Cmd_Enable(PASS_COMMAND_ARGS);

	// Undo code modification.
	WriteRelJe(0x5C4437, 0x5C451D);

	return success;
}

//ripped code from FOSE's ListAddForm
bool Cmd_ListAddList_Execute(COMMAND_ARGS)
{
	*result = 1;
	BGSListForm* pListForm = nullptr;
	BGSListForm* pToAppendList = nullptr;
	UInt32 addAtIndex = eListEnd;

	ExtractArgsEx(EXTRACT_ARGS_EX, &pListForm, &pToAppendList, &addAtIndex);
	if (!pListForm || !pToAppendList) return true;

	auto Try_Adding_Form_From_FormList = [&](TESForm* form)
	{
		UInt32 const addedAtIndex = pListForm->AddAt(form, addAtIndex);
		if (addedAtIndex == eListInvalid)
		{
			*result = 0; //error
			return false;
		}
		return true;
	};

	// Need to append elements to our own array in order to iterate backwards, to append in descending key order.
	std::vector<TESForm*> formArr;

	for (tList<TESForm>::Iterator iter = pToAppendList->list.Begin(); !iter.End(); ++iter)
	{
		TESForm* form = iter.Get();
		if (form)
		{
			if (addAtIndex != eListEnd)
			{
				formArr.push_back(form);
			}
			else
			{
				if (!Try_Adding_Form_From_FormList(form)) break;
			}
		}
		else
		{
			_ERROR("ListAddList - found invalid form in formlist.");
			*result = 0;
			break;
		}
	}

	if (!formArr.empty())
	{
		for (int i = formArr.size() - 1; i >= 0; i--)
		{
			if (!Try_Adding_Form_From_FormList(formArr[i])) break;
		}
	}

	return true;
}

const UInt32 kMsgIconsPathAddr[] = { 0x10208A0, 0x10208E0, 0x1025CDC, 0x1030E78, 0x103A830, 0x1049638, 0x104BFE8 };

//99% ripped from JIP's MessageExAlt.
bool Cmd_MessageExAltShowoff_Execute(COMMAND_ARGS)
{
	float displayTime;
	UINT32 appendToQueue;
	char* buffer = GetStrArgBuffer();
	if (!ExtractFormatStringArgs(2, buffer, EXTRACT_ARGS_EX, kCommandInfo_MessageExAltShowoff.numParams, &displayTime, &appendToQueue))
		return true;

	char* msgPtr = GetNextTokenJIP(buffer, '|');
	msgPtr[0x203] = 0;
	if (*msgPtr)
	{
		if ((buffer[0] == '#') && (buffer[1] >= '0') && (buffer[1] <= '6') && !buffer[2])
			QueueUIMessage(msgPtr, 0, (const char*)kMsgIconsPathAddr[buffer[1] - '0'], NULL, displayTime, appendToQueue != 0);
		else QueueUIMessage(msgPtr, 0, buffer, NULL, displayTime, appendToQueue != 0);
	}
	else QueueUIMessage(buffer, 0, NULL, NULL, displayTime, appendToQueue != 0);

	return true;
}

// Inspired by JIP's "ClearMessageQueue"
bool Cmd_IsCornerMessageDisplayed_Execute(COMMAND_ARGS)
{
	*result = !g_HUDMainMenu->queuedMessages.Empty();
	//*result = (bool)g_HUDMainMenu->currMsgKey;
	//another way to check. Seems to be a bit slower to update when initially adding a message to the queue.
	return true;
}
bool Cmd_IsCornerMessageDisplayed_Eval(COMMAND_ARGS_EVAL)
{
	*result = !g_HUDMainMenu->queuedMessages.Empty();
	return true;
}

bool Cmd_GetNumQueuedCornerMessages_Execute(COMMAND_ARGS)
{
	*result = g_HUDMainMenu->queuedMessages.Count();
	return true;
}
bool Cmd_GetNumQueuedCornerMessages_Eval(COMMAND_ARGS_EVAL)
{
	*result = g_HUDMainMenu->queuedMessages.Count();
	return true;
}


//Code ripped from JIP's IsAnimPlayingEx
bool Cmd_IsAnimPlayingExCond_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	auto const category = (UInt32)arg1;
	auto const subType = (UInt32)arg2;  //optional

	if (!thisObj) return true;
	if (category > 5 || category < 1) return true;
	if (subType > 23) return true;

	AnimData* animData = thisObj->GetAnimData();
	if (!animData) return true;
	const AnimGroupClassify* classify;
	for (UInt16 groupID : animData->animGroupIDs)
	{
		UInt32 const animID = groupID & 0xFF;
		if (animID >= 245) continue;
		classify = &s_animGroupClassify[animID];
		if (classify->category == category && (category >= 4 || (!subType || classify->subType == subType)))
		{
			*result = 1;
			break;
		}
	}

	return true;
}
bool Cmd_IsAnimPlayingExCond_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 category, subType = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &category, &subType)) return true;

	return Cmd_IsAnimPlayingExCond_Eval(thisObj, (void*)category, (void*)subType, result);
}

bool Cmd_GetRadiationExtraData_Execute(COMMAND_ARGS)
{
	*result = 0;
	if (thisObj)
	{
		ExtraRadiation* xRadius = GetExtraTypeJIP(&thisObj->extraDataList, Radiation);
		if (xRadius) *result = xRadius->radiation;
	}
	return true;
}
bool Cmd_SetRadiationExtraData_Execute(COMMAND_ARGS)
{
	float fNewVal = 0;
	*result = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &fNewVal)) return true;
	if (thisObj)
	{
		ThisStdCall(0x422350, &thisObj->extraDataList, fNewVal);  //credits to c6 for pointing this address out.
		*result = 1;
	}
	return true;
}


bool Cmd_PlayerHasNightVisionActive_Execute(COMMAND_ARGS)
{
	*result = g_thePlayer->hasNightVisionApplied;
	return true;
}
bool Cmd_PlayerHasNightVisionActive_Eval(COMMAND_ARGS_EVAL)
{
	*result = g_thePlayer->hasNightVisionApplied;
	return true;
}


bool Cmd_PlayerIsUsingTurbo_Execute(COMMAND_ARGS)
{
	*result = g_thePlayer->isUsingTurbo;
	return true;
}
bool Cmd_PlayerIsUsingTurbo_Eval(COMMAND_ARGS_EVAL)
{
	*result = g_thePlayer->isUsingTurbo;
	return true;
}

bool Cmd_PlayerHasCateyeEnabled_Execute(COMMAND_ARGS)
{
	*result = g_thePlayer->isCateyeEnabled;
	return true;
}
bool Cmd_PlayerHasCateyeEnabled_Eval(COMMAND_ARGS_EVAL)
{
	*result = g_thePlayer->isCateyeEnabled;
	return true;
}

bool Cmd_PlayerHasImprovedSpottingActive_Execute(COMMAND_ARGS)
{
	*result = g_thePlayer->isSpottingImprovedActive;
	return true;
}
bool Cmd_PlayerHasImprovedSpottingActive_Eval(COMMAND_ARGS_EVAL)
{
	*result = g_thePlayer->isSpottingImprovedActive;
	return true;
}

bool Cmd_PlayerIsDrinkingPlacedWater_Execute(COMMAND_ARGS)
{
	*result = g_thePlayer->isDrinkingPlacedWater;
	return true;
}
bool Cmd_PlayerIsDrinkingPlacedWater_Eval(COMMAND_ARGS_EVAL)
{
	*result = g_thePlayer->isDrinkingPlacedWater;
	return true;
}

bool Cmd_SetIsPCAMurderer_Execute(COMMAND_ARGS)
{
	UInt32 bIsMurderer = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &bIsMurderer)) return true;
	g_thePlayer->bIsAMurderer = (bIsMurderer != 0);
	return true;
}


bool Cmd_IsNight_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	TESClimate* climate = (TESClimate*)arg1;
	Sky* sky = *g_currentSky;
	float const gameHour = ThisStdCall<double>(0x966A20, sky);
	float sunrise, sunset;
	if (climate && IS_TYPE(climate, TESClimate))
	{
		sunrise = ThisStdCall<UInt8>(0x595F10, climate, 1) / 6.0F;  //sunrise begin sprinkled with adjustments.
		sunset = ThisStdCall<UInt8>(0x595F10, climate, 2) / 6.0F;  //Second arg determines which type of time to check.
	}
	else
	{
		sunrise = ThisStdCall<double>(0x595F50, sky);
		sunset = ThisStdCall<double>(0x595FC0, sky);
	}
	if (sunset <= gameHour || (sunrise >= gameHour))
		*result = 1;
	return true;
}
bool Cmd_IsNight_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESClimate* climate = NULL;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &climate)) return true;
	return Cmd_IsNight_Eval(0, climate, 0, result);
}

//credits to JIP LN's "CrippleLimb" for much of the structure.
bool Cmd_IsLimbCrippled_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	if (!IS_ACTOR(thisObj)) return true;
	Actor* const actor = (Actor*)thisObj;
	UInt32 limbID = (UInt32)arg1;
	UInt32 const threshold = (UInt32)arg2;
	if (limbID == -1)
	{
		//loop through all limb health AVs and break if a single one is at 0 health (or below threshold).
		for (limbID = kAVCode_PerceptionCondition; limbID <= kAVCode_BrainCondition; limbID++)
		{
			if (actor->avOwner.GetActorValue(limbID) <= threshold)
			{
				*result = 1;
				break;
			}
		}
	}
	else if (limbID <= 6)
	{
		limbID += kAVCode_PerceptionCondition;
		if (actor->avOwner.GetActorValue(limbID) <= threshold)
			*result = 1;
	}
	return true;
}
bool Cmd_IsLimbCrippled_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 limbID = -1;
	UInt32 threshold = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &limbID, &threshold)) return true;
	return Cmd_IsLimbCrippled_Eval(thisObj, (void*)limbID, (void*)threshold, result);
}

bool Cmd_GetNumCrippledLimbs_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	if (!IS_ACTOR(thisObj)) return true;
	Actor* const actor = (Actor*)thisObj;
	UInt32 const threshold = (UInt32)arg1;
	UInt32 numCrippledLimbs = 0;
	for (UInt32 limbID = kAVCode_PerceptionCondition; limbID <= kAVCode_BrainCondition; limbID++)
	{
		if (actor->avOwner.GetActorValue(limbID) <= threshold)
		{
			numCrippledLimbs++;
		}
	}
	*result = numCrippledLimbs;
	return true;
}
bool Cmd_GetNumCrippledLimbs_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 threshold = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &threshold)) return true;
	return Cmd_GetNumCrippledLimbs_Eval(thisObj, (void*)threshold, 0, result);
}


bool Cmd_GetCrippledLimbsAsBitMask_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	if (!IS_ACTOR(thisObj)) return true;
	Actor* const actor = (Actor*)thisObj;
	UInt32 const threshold = (UInt32)arg1;

	enum LimbGoneFlags
	{
		kFlag_Head = 1,
		kFlag_Torso = 2,
		kFlag_LeftArm = 4,
		kFlag_RightArm = 8,
		kFlag_LeftLeg = 0x10,
		kFlag_RightLeg = 0x20,
		kFlag_Brain = 0x40,
	};
	UInt32 flagsArr[7] = { kFlag_Head, kFlag_Torso, kFlag_LeftArm, kFlag_RightArm, kFlag_LeftLeg, kFlag_RightLeg, kFlag_Brain };

	UInt32 CrippledLimbsMask = 0;
	for (UInt32 limbID = kAVCode_PerceptionCondition; limbID <= kAVCode_BrainCondition; limbID++)
	{
		if (actor->avOwner.GetActorValue(limbID) <= threshold)
		{
			CrippledLimbsMask |= flagsArr[limbID - kAVCode_PerceptionCondition];
		}
	}
	*result = CrippledLimbsMask;
	return true;
}
bool Cmd_GetCrippledLimbsAsBitMask_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 threshold = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &threshold)) return true;
	return Cmd_GetCrippledLimbsAsBitMask_Eval(thisObj, (void*)threshold, 0, result);
}

// Copied ClearJIPSavedData
bool Cmd_ClearShowoffSavedData_Execute(COMMAND_ARGS)
{
	UInt32 auxStringMaps, auxTimerMaps;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &auxStringMaps, &auxTimerMaps)) return true;
	UInt8 modIdx = scriptObj->GetOverridingModIdx();
	if (auxStringMaps && s_auxStringMapArraysPerm.Erase((auxStringMaps == 2) ? 0xFF : modIdx))  //todo: fix .Erase not doing anything!
		s_dataChangedFlags |= kChangedFlag_AuxStringMaps;
	if (auxTimerMaps && AuxTimer::s_auxTimerMapArraysPerm.Erase((auxTimerMaps == 2) ? 0xFF : modIdx))
		s_dataChangedFlags |= kChangedFlag_AuxTimerMaps;
	return true;
}

bool Cmd_GetCalculatedMaxCarryWeight_Eval(COMMAND_ARGS_EVAL)
{
	*result = 0;
	if (!IS_ACTOR(thisObj)) return true;
	*result = ThisStdCall<double>(0x8A0C20, (Actor*)thisObj);
	return true;
}
bool Cmd_GetCalculatedMaxCarryWeight_Execute(COMMAND_ARGS)
{
	return Cmd_GetCalculatedMaxCarryWeight_Eval(thisObj, 0, 0, result);
}


// Just so I can store the initial seed.
// https://www.cplusplus.com/reference/random/default_random_engine/
class Random_Engine : public std::default_random_engine
{
public:
	enum SeedStuff
	{
		kInvalid_Seed = -1,
	};

	UInt32 seed;
	// constructor that sets seed
	Random_Engine(UInt32 newSeed) : std::default_random_engine(newSeed), seed(newSeed)
	{}
};

UnorderedMap<UInt32, Random_Engine*> g_ModsAndSeedsMap;

void SetSeedForMod(UInt32 seed, Script* scriptObj)
{
	if (!seed) seed = std::default_random_engine::default_seed;
	UInt8 const modIdx = scriptObj->GetOverridingModIdx();
	ScopedLock lock(g_Lock);
	// Change the engine's seed by just deleting the old engine and creating a new one.
	auto const oldGen = g_ModsAndSeedsMap.Get(modIdx);
	g_ModsAndSeedsMap.Erase(modIdx);
	delete oldGen;  //safe to do even if the pointer is null
	auto newGen = new Random_Engine(seed);
	g_ModsAndSeedsMap.Emplace(modIdx, newGen);
}

bool Cmd_SetRandomizerSeed_Execute(COMMAND_ARGS)
{
	UInt32 seed = 0;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &seed)) return true;
	SetSeedForMod(seed, scriptObj);
	return true;
}

bool Cmd_SetSeedUsingForm_Execute(COMMAND_ARGS)
{
	TESForm* seedForm = nullptr;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &seedForm)) return true;
	UInt32 const seed = seedForm ? seedForm->refID : 0;
	SetSeedForMod(seed, scriptObj);
	return true;
}

bool Cmd_GetRandomizerSeed_Execute(COMMAND_ARGS)
{
	*result = Random_Engine::kInvalid_Seed;
	UInt8 const modIdx = scriptObj->GetOverridingModIdx();
	if (auto const currGen = g_ModsAndSeedsMap.Get(modIdx))
	{
		*result = currGen->seed;
	}
	return true;
}

UInt32 RandSeeded_Call(UInt32 min, UInt32 max, Script* scriptObj)
{
	UInt8 const modIdx = scriptObj->GetOverridingModIdx();
	ScopedLock lock(g_Lock);
	Random_Engine* generator = g_ModsAndSeedsMap.Get(modIdx);
	if (!generator)
	{
		// Make a new generator for the calling Mod w/ default seed, and use that.
		generator = new Random_Engine(std::default_random_engine::default_seed);
		g_ModsAndSeedsMap.Emplace(modIdx, generator);
	}
	std::uniform_int_distribution distribution(min, max);
	return distribution(*generator);
}

bool Cmd_RandSeeded_Execute(COMMAND_ARGS)
{
	*result = Random_Engine::kInvalid_Seed;
	UInt32 min, max;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &min, &max) || min > max) return true;
	*result = RandSeeded_Call(min, max, scriptObj);
	return true;
}

bool Cmd_GetRandomPercentSeeded_Execute(COMMAND_ARGS)
{
	//NOTE: GetRandomPercent uses 0, 99 as min/max, but I dislike that.
	*result = RandSeeded_Call(1, 100, scriptObj);
	return true;
}

bool Cmd_AdvanceSeed_Execute(COMMAND_ARGS)
{
	*result = false;
	UInt32 discardAmount;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &discardAmount)) return true;
	UInt8 const modIdx = scriptObj->GetOverridingModIdx();
	if (auto currGen = g_ModsAndSeedsMap.Get(modIdx))
	{
		ScopedLock lock(g_Lock);
		currGen->discard(discardAmount);
		*result = true;
	}
	return true;
}

bool Cmd_IsReferenceCloned_Execute(COMMAND_ARGS)
{
	*result = thisObj->IsCloned();  // technically should also work if thisObj is a baseForm? But maybe don't do that.
	return true;
}
bool Cmd_IsReferenceCloned_Eval(COMMAND_ARGS_EVAL)
{
	*result = thisObj->IsCloned();
	return true;
}

bool Cmd_IsTemporaryReference_Execute(COMMAND_ARGS)
{
	*result = thisObj->IsTemporary();
	return true;
}
bool Cmd_IsTemporaryReference_Eval(COMMAND_ARGS_EVAL)
{
	*result = thisObj->IsTemporary();
	return true;
}


std::atomic<bool> questMsgEnabled = true;

bool Cmd_ToggleQuestMessages_Execute(COMMAND_ARGS)
{
	*result = 0;
	SInt32 bOn = -1;  // If -1 (default), return the toggle status.
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &bOn))
		return true;
	
	if (bOn == -1)
	{
		*result = questMsgEnabled;
		return true;
	}
	
	if (!bOn && questMsgEnabled)
	{
		// Kill the QuestUpdateManager::Append/PrependToQueue functions
		NopFunctionCall(0x76BADC, 1);
		NopFunctionCall(0x76BAEB, 1);

		NopFunctionCall(0x76BAAC, 1);
		NopFunctionCall(0x76BABB, 1);

		WriteRelJump(0x77A5BD, 0x77A646);  // Make return jmp for HUDMainMenu::SetQuestUpdateText always true (does nothing).
		// Was able to account for this thanks to JIP's SuppressQuestMessages function.
		
		questMsgEnabled = false;
		*result = true;
	}
	else if (bOn && !questMsgEnabled)
	{
		// Restore the Calls
		WriteRelCall(0x76BADC, 0x905820);
		WriteRelCall(0x76BAEB, 0x83FD60);

		WriteRelCall(0x76BAAC, 0x5AE3D0);
		WriteRelCall(0x76BABB, 0x83FD60);

		WriteRelJe(0x77A5BD, 0x77A646);
		
		questMsgEnabled = true;
		*result = true;
	}
	return true;
}


namespace PipboyRadioFunctions
{
	enum RadioInfo
	{
		kData_TopicInfos = 1,
		kData_Topics,
		kData_Quests,
		kData_Actors,
		kData_ResponseFilename,
		kData_ResponseString,
	};
}

bool Cmd_GetPipboyRadioVoiceEntryData_Execute(COMMAND_ARGS)
{
	using namespace PipboyRadioFunctions;
	*result = 0;
	UInt32 info;
	UInt32 bGetDeep = 0;  // if true, return a multidimensional array with the info for each individual VoiceEntryList that is queued.
	if (!ExtractArgs(EXTRACT_ARGS, &info, &bGetDeep))
		return true;

	auto GetAndAppendVoiceEntryData = [&](tList<VoiceEntry> const &voiceEntries, NVSEArrayVar* arrToAppendTo)
	{
		for (auto iter = voiceEntries.Begin(); !iter.End(); iter.Next())
		{
			ArrayElementR elem;
			auto const voiceEntry = iter.Get();
			if (!voiceEntry) continue;
			switch (info)
			{
			case kData_TopicInfos:
				if (voiceEntry->topicInfo)  // NOTE: seems to give an invalid TESForm pointer sometimes, if looping through all voicelists, causing an error.
					elem = voiceEntry->topicInfo;
				break;
			case kData_Topics:
				if (voiceEntry->topic)
					elem = voiceEntry->topic;
				break;
			case kData_Quests:
				elem = voiceEntry->quest;
				break;
			case kData_Actors:
				elem = voiceEntry->actor;
				break;
			case kData_ResponseFilename:
				if (voiceEntry->response)
					elem = voiceEntry->response->fileName;
				break;
			case kData_ResponseString:
				if (voiceEntry->response)
					elem = voiceEntry->response->str.CStr();
				break;
			}
			if (elem.IsValid())
				g_arrInterface->AppendElement(arrToAppendTo, elem);  //todo: fix crash here!!!
		}
	};

	auto outArr = g_arrInterface->CreateArray(nullptr, 0, scriptObj);
	if (auto const radio = Radio::RadioEntry::GetSingleton())
	{
		Radio::VoiceEntryList* voiceList = radio->unk04.unk00;
		if (!voiceList) return true;
		if (!bGetDeep)
		{
			GetAndAppendVoiceEntryData(voiceList->voiceEntries, outArr);
		}
		else 
		{
			//for (; voiceList; voiceList = voiceList->next)
			do {
				auto innerArr = g_arrInterface->CreateArray(nullptr, 0, scriptObj);
				GetAndAppendVoiceEntryData(voiceList->voiceEntries, innerArr); // TODO: fix crash that occurs around here!!!
				if (g_arrInterface->GetArraySize(innerArr) > 0)
					g_arrInterface->AppendElement(outArr, ArrayElementR(innerArr));
				voiceList = voiceList->next;
			} while (voiceList);
		}
	}
	g_arrInterface->AssignCommandResult(outArr, result);
	return true;
}

// Credits to NVSE's ListRemoveForm for most of the code.
// Savebaked version of the above function.
bool Cmd_FormListRemoveForm_Execute(COMMAND_ARGS)
{
	*result = eListInvalid;	//index of removed form.
	BGSListForm* pListForm;
	TESForm* pForm;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &pListForm, &pForm))
		return true;
	SInt32 const index = pListForm->RemoveForm(pForm);
	pListForm->MarkAsModified(0x80000000);
	*result = index;
	return true;
}

bool Cmd_GetZoneRespawns_Execute(COMMAND_ARGS)
{
	*result = -1;	//bRespawns. -1 if could not extract the form.
	TESForm* form;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &form))
		return true;
	if (auto const zone = DYNAMIC_CAST(form, TESForm, BGSEncounterZone))
	{
		*result = (zone->zoneFlags & BGSEncounterZone::kEncounterZone_NoRespawns) == 0;
	}
	return true;
}

bool Cmd_ClearCinematicTextQueue_Execute(COMMAND_ARGS)
{
	CdeclCall(0x77F500); // HUDMainMenu::HideQuestLocationText
	// Idea stolen from JG's QueueCinematicText
	return true;
}

//Credits to LN's GetZone function for most of the code here.
bool Cmd_GetCellEncounterZone_Execute(COMMAND_ARGS)
{
	*result = 0;	//zone form
	TESForm* form;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &form))
		return true;
	if (auto const cell = DYNAMIC_CAST(form, TESForm, TESObjectCELL)) {
		ExtraEncounterZone* xEncZone = GetExtraTypeJIP(&cell->extraDataList, EncounterZone);
		if (xEncZone && xEncZone->zone) 
			REFR_RES = xEncZone->zone->refID;
	}
	return true;
}

DEFINE_COMMAND_PLUGIN(ShowPauseMenu, "", false, kParams_OneOptionalInt);
bool Cmd_ShowPauseMenu_Execute(COMMAND_ARGS)
{
	enum PauseMode : UInt32
	{
		kJustPause = 0,
		kLoad, kSave, kSettings, kHelp
	};
	PauseMode pauseMode = kJustPause;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &pauseMode))
		return true;
	auto menu = StartMenu::GetSingleton();
	if (!menu) {
		StartMenu::Create(true, false);
		menu = StartMenu::GetSingleton();
	}

	if (pauseMode != kJustPause)
	{
		//Make every main option disabled.
		menu->main_options084.SetParentEnabled(false);		
		UInt32 callback_addr;
		switch (pauseMode)
		{
		case kLoad:
			callback_addr = StartMenu::kLoadAddr;
			break;
		case kSave:
			callback_addr = StartMenu::kSaveAddr;
			break;
		case kSettings:
			ThisStdCall(0x7D5F80, menu);	//StartMenu::HideSubSettings
			callback_addr = StartMenu::kSettingsAddr;
			break;
		case kHelp:
			callback_addr = StartMenu::kHelpAddr;
			break;
		}
		CdeclCall(callback_addr);
		
		// Make the appropriate tile "selected", to make it have its opacity back.
		auto iter = menu->main_options084.list.Head();
		do {
			if (auto item = iter->data)
			{
				if ((UInt32)item->object->followupOption_callback == callback_addr)
				{
					item->tile->SetFloat(TraitNameToID("_selected"), 1, true);
					break;
				}
			}
		} while (iter = iter->next);
	}
	return true;
}

//Credits to JIP's LeveledListRemoveForm for most of the code.
bool Cmd_RemoveFormFromLeveledList_Execute(COMMAND_ARGS)
{
	TESForm* list, * form;
	TESLeveledList* lvlList;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &list, &form) && (lvlList = list->GetLvlList()))
	{
		if (*result = (int)lvlList->RemoveItem(form))
		{
			//todo: modify lvList->extraDatas (?)
			list->MarkAsModified(0x80000000);
		}
	}
	else *result = 0;
	return true;
}

bool Cmd_ResetInteriorAlt_Execute(COMMAND_ARGS)
{
	*result = false;	//success
	TESForm* form;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &form))
		return true;
	if (auto const cell = DYNAMIC_CAST(form, TESForm, TESObjectCELL)) {
		ThisStdCall_B(0x546B10, cell, -2, false);	//TESObjectCELL::updateDetachTime
		//(hooks are in place to handle the -2 detachTime correctly)
		*result = true;
	}
	return true;
}

DEFINE_COMMAND_ALT_PLUGIN(SetEnableParent, SetParentRef, "", true, kParams_OneOptionalForm);
bool Cmd_SetEnableParent_Execute(COMMAND_ARGS)
{
	TESObjectREFR* newParent = nullptr;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &newParent) || !thisObj)
		return true;

	auto xParentRef = GetExtraTypeJIP(&thisObj->extraDataList, EnableStateParent);

	if (xParentRef)
	{
		if (auto const parent = xParentRef->parent;
			parent != newParent)
		{
			// remove EnableChildren extraData
			auto const xChildrenRef = GetExtraTypeJIP(&parent->extraDataList, EnableStateChildren);
			xChildrenRef->children.Remove(thisObj);
			if (xChildrenRef->children.Empty())
				parent->extraDataList.Remove(xChildrenRef, true);
		}
	}

	if (!newParent)
	{
		if (!xParentRef)
			return true;

		// remove EnableParent extraData
		thisObj->extraDataList.Remove(xParentRef, true);
	}
	else
	{
		// add EnableChildren xData to newParent
		auto xChildrenRef = GetExtraTypeJIP(&newParent->extraDataList, EnableStateChildren);
		if (!xChildrenRef)
		{
			xChildrenRef = ExtraEnableStateChildren::Create();
			newParent->extraDataList.Add(xChildrenRef);
		}
		xChildrenRef->children.Append(thisObj);

		// add EnableParent xData to thisObj
		if (!xParentRef)
		{
			xParentRef = ExtraEnableStateParent::Create();
			thisObj->extraDataList.Add(xParentRef);
		}
		xParentRef->parent = newParent;
	}

	return true;
}

bool Cmd_GetActorValueName_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 av;
	enum Mode {
		kMode_GetEditorName = 0,
		kMode_GetFullLocalizedName,
		kMode_GetShortLocalizedName,
	} mode = kMode_GetEditorName;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &av, &mode))
		return true;
	const char* name = "(null)";
	if (mode == kMode_GetEditorName) {
		name = GetActorValueEditorIDName(av);
	}
	else if (mode == kMode_GetFullLocalizedName) {
		if (auto* info = GetActorValueInfo(av)) {
			name = info->fullName.name.CStr();
		}
	}
	else if (mode == kMode_GetShortLocalizedName) {
		name = GetActorValueShortLocalizedName(av);
	}
	g_strInterface->Assign(PASS_COMMAND_ARGS, name);
	return true;
}

enum AmmoNameType
{
	kAmmoName_Full = 0,
	kAmmoName_Short,
	kAmmoName_Abbreviation
};

// Copied a bit from xNVSE's SetName
DEFINE_COMMAND_PLUGIN(SetAmmoName, "", false, kParams_OneString_OneOptionalInt_OneOptionalForm);
bool Cmd_SetAmmoName_Execute(COMMAND_ARGS)
{
	*result = 0; // success
	char string[256];
	UInt32 nameTypeToChange = 0;
	TESForm* form = nullptr;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &string, &nameTypeToChange, &form))
		return true;

	form = form->TryGetREFRParent();
	if (!form) {
		if (!thisObj) return true;
		form = thisObj->baseForm;
	}
	if (!IS_TYPE(form, TESAmmo))
		return true;

	*result = 1;
	if (nameTypeToChange == kAmmoName_Full) {
		if (TESFullName* name = form->GetFullName()) {
			name->name.Set(string);
		}
	}
	else if (nameTypeToChange == kAmmoName_Short) {
		((TESAmmo*)form)->shortName.Set(string);
	}
	else if (nameTypeToChange == kAmmoName_Abbreviation) {
		((TESAmmo*)form)->abbreviation.Set(string);
	}
	else
		*result = 0;
	return true;
}

// Copied a bit from xNVSE's GetName
DEFINE_COMMAND_PLUGIN(GetAmmoName, "", false, kParams_OneOptionalInt_OneOptionalForm);
bool Cmd_GetAmmoName_Execute(COMMAND_ARGS)
{
	const char* name = ""; // result

	UInt32 nameTypeToChange = 0;
	TESForm* form = nullptr;
	if (ExtractArgsEx(EXTRACT_ARGS_EX, &nameTypeToChange, &form))
	{
		form = form->TryGetREFRParent();
		if (!form) {
			if (!thisObj) return true;
			form = thisObj->baseForm;
		}
		if (!IS_TYPE(form, TESAmmo))
			return true;

		String* nameString = nullptr;
		if (nameTypeToChange == kAmmoName_Full) {
			if (TESFullName* name = form->GetFullName()) {
				nameString = &name->name;
			}
		}
		else if (nameTypeToChange == kAmmoName_Short) {
			nameString = &((TESAmmo*)form)->shortName;
		}
		else if (nameTypeToChange == kAmmoName_Abbreviation) {
			nameString = &((TESAmmo*)form)->abbreviation;
		}

		if (nameString && nameString->m_data && nameString->m_dataLen)
			name = nameString->m_data;
		else
			name = "<no name>"; // mimick xNVSE's GetFullName
	}
	g_strInterface->Assign(PASS_COMMAND_ARGS, name);
	return true;
}

DEFINE_COMMAND_PLUGIN(SetForceDrawHitscanProjectiles, "", false, kParams_OneOptionalInt);
bool Cmd_SetForceDrawHitscanProjectiles_Execute(COMMAND_ARGS)
{
	*result = SetForceDrawHitscanProjectiles::g_isForcingProjectilesToDraw;
	int bForceDraw = -1;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &bForceDraw))
		return true;
	if (bForceDraw != -1)
		SetForceDrawHitscanProjectiles::g_isForcingProjectilesToDraw = bForceDraw != 0;
	return true;
}

DEFINE_COMMAND_PLUGIN(SetProjectileTracerChanceOverride, "", false, kParams_OneOptionalInt);
bool Cmd_SetProjectileTracerChanceOverride_Execute(COMMAND_ARGS)
{
	*result = SetProjectileTracerChanceOverride::g_tracerChanceOverride;
	int overrideVal = -2;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &overrideVal))
		return true;
	if (overrideVal >= -1)
		SetProjectileTracerChanceOverride::g_tracerChanceOverride = overrideVal;
	return true;
}

DEFINE_COMMAND_PLUGIN(SpawnTracingProjectile, "Spawns a projectile following the calling projectile reference.", 
	true, kParams_OneForm_TwoOptionalInts);
bool Cmd_SpawnTracingProjectile_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESForm* baseProjToSpawn;
	UInt32 ignoreGravity = 1;
	UInt32 bCopyData = false;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &baseProjToSpawn, &ignoreGravity, &bCopyData))
		return true;
	if (!thisObj || !IS_PROJECTILE(thisObj) || !IS_TYPE(baseProjToSpawn, BGSProjectile))
		return true;

	auto* projectileToTrail = static_cast<Projectile*>(thisObj);

	Actor* sourceActor = nullptr;
	//CombatController* combatCntrl = nullptr;
	TESObjectWEAP* weap = nullptr;
	if (bCopyData)
	{
		sourceActor = static_cast<Actor*>(projectileToTrail->sourceRef);
		//combatCntrl = projectileToTrail->comba
		weap = projectileToTrail->sourceWeap;
	}

	// Code copied from Tweaks' PlaceTrailAt function.
	auto* newProj = Projectile::Spawn(static_cast<BGSProjectile*>(baseProjToSpawn), sourceActor, nullptr,
		weap, *projectileToTrail->GetPos(), projectileToTrail->rotZ, projectileToTrail->rotX,
		0, 0, projectileToTrail->parentCell, ignoreGravity);

	REFR_RES = newProj->refID;
	return true;
}

DEFINE_CMD_COND_PLUGIN(IsPlayerLookingAround, "", false, nullptr);
bool Cmd_IsPlayerLookingAround_Execute(COMMAND_ARGS)
{
	//TODO: add IsMenuMode, IsVATSMode checks
	*result = IsPlayerLookingAround::g_isPlayerLookingAround;
	return true;
}
bool Cmd_IsPlayerLookingAround_Eval(COMMAND_ARGS_EVAL)
{
	*result = IsPlayerLookingAround::g_isPlayerLookingAround;
	return true;
}

DEFINE_COMMAND_PLUGIN(IsFormLoading, "", false, kParams_OneForm);
bool Cmd_IsFormLoading_Execute(COMMAND_ARGS)
{
	*result = false;
	TESForm* form;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &form) || !form)
		return true;
	*result = form->IsStillLoading();
	return true;
}

DEFINE_COMMAND_PLUGIN(PatchFreezeTime, "", false, nullptr);
bool Cmd_PatchFreezeTime_Execute(COMMAND_ARGS)
{
	Experimental::PatchFreezeTime::WriteHooks();
	return true;
}

DEFINE_COMMAND_PLUGIN(ToANSIChar, "", false, kParams_OneInt_OneOptionalInt);
bool Cmd_ToANSIChar_Execute(COMMAND_ARGS)
{
	UInt32 scancode = 0;
	bool ignoreShift = true;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &scancode, &ignoreShift))
	{
		g_strInterface->Assign(PASS_COMMAND_ARGS, "");
		return true;
	}

	BYTE keyboardState[256];
	if (!GetKeyboardState(keyboardState)) return true;

	HKL layout = GetKeyboardLayout(0);

	// When ignoreShift is true, we explicitly clear the Shift state instead of forcing it pressed.
	// Some keyboard layouts (e.g. French AZERTY) use lowercase positions for special letters (é, à, etc.)
	// and uppercase/Shifted positions for digits. Forcing Shift on such layouts would return numbers
	// instead of letters, so Shift must remain disabled when ignoreShift = true.
	if (ignoreShift) {
		keyboardState[VK_SHIFT] = 0;
		keyboardState[VK_LSHIFT] = 0;
		keyboardState[VK_RSHIFT] = 0;
		keyboardState[VK_CAPITAL] = 0;
	}

	UINT vk = MapVirtualKeyEx(scancode, MAPVK_VSC_TO_VK, layout);
	if (vk == 0) return true;

	WCHAR unicodeChar[16];
	int len = ToUnicodeEx(vk, scancode, keyboardState, unicodeChar, _countof(unicodeChar), 0, layout);
	if (len <= 0) return true;

	char ansiChar[16];
	int bytes = WideCharToMultiByte(CP_ACP, 0, unicodeChar, len, ansiChar, sizeof(ansiChar), NULL, NULL);
	if (bytes <= 0) return true;
	ansiChar[bytes] = '\0';

	g_strInterface->Assign(PASS_COMMAND_ARGS, ansiChar);
	return true;
}

#if _DEBUG



DEFINE_COMMAND_PLUGIN(CaravanDeckGetCards, "", false, kParams_OneForm);
bool Cmd_CaravanDeckGetCards_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESForm* deckForm;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &deckForm) || !IS_TYPE(deckForm, TESCaravanDeck))
		return true;
	const auto* deck = static_cast<TESCaravanDeck*>(deckForm);
	if (deck->cards->Empty())
		return true;
	auto data = std::make_unique<ArrayElementL[]>(deck->count);
	size_t i = 0;
	for (auto iter = deck->cards->Begin(); !iter.End(); ++iter) {
		data[i] = iter.Get();
		i++;
	}
	NVSEArrayVar* resArr = g_arrInterface->CreateArray(data.get(), deck->count, scriptObj);
	g_arrInterface->AssignCommandResult(resArr, result);
	return true;
}

DEFINE_COMMAND_PLUGIN(CaravanDeckRemoveCard, "", false, kParams_TwoForms);
bool Cmd_CaravanDeckRemoveCard_Execute(COMMAND_ARGS)
{
	*result = 0; //numRemoved
	TESForm* deckForm;
	TESForm* cardForm;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &deckForm, &cardForm) 
		|| !IS_TYPE(deckForm, TESCaravanDeck) || !IS_TYPE(cardForm, TESCaravanCard))
	{
		return true;
	}
	auto* deck = static_cast<TESCaravanDeck*>(deckForm);
	auto* card = static_cast<TESCaravanCard*>(cardForm);
	if (deck->cards->Empty())
		return true;
	*result = deck->cards->Remove(card);
	deck->count -= *result;
	return true;
}

DEFINE_COMMAND_PLUGIN(CaravanDeckAddCard, "", false, kParams_TwoForms_OneOptionalInt);
bool Cmd_CaravanDeckAddCard_Execute(COMMAND_ARGS)
{
	*result = eListInvalid;
	TESForm* deckForm;
	TESForm* cardForm;
	SInt32 n = eListEnd;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &deckForm, &cardForm, &n)
		|| !IS_TYPE(deckForm, TESCaravanDeck) || !IS_TYPE(cardForm, TESCaravanCard))
	{
		return true;
	}
	auto* deck = static_cast<TESCaravanDeck*>(deckForm);
	auto* card = static_cast<TESCaravanCard*>(cardForm);
	*result = deck->cards->AddAt(card, n);
	++deck->count;
	return true;
}

DEFINE_COMMAND_PLUGIN(CaravanDeckGetCount, "", false, kParams_OneForm);
bool Cmd_CaravanDeckGetCount_Execute(COMMAND_ARGS)
{
	*result = -1;
	TESForm* deckForm;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &deckForm) || !IS_TYPE(deckForm, TESCaravanDeck) )
		return true;
	auto* deck = static_cast<TESCaravanDeck*>(deckForm);
	*result = deck->count;
	return true;
}

DEFINE_COMMAND_PLUGIN(CaravanDeckGetCardIndex, "", false, kParams_TwoForms);
bool Cmd_CaravanDeckGetCardIndex_Execute(COMMAND_ARGS)
{
	*result = eListInvalid;
	TESForm* deckForm;
	TESForm* cardForm;
	if (!ExtractArgsEx(EXTRACT_ARGS_EX, &deckForm, &cardForm)
		|| !IS_TYPE(deckForm, TESCaravanDeck) || !IS_TYPE(cardForm, TESCaravanCard))
	{
		return true;
	}
	auto* deck = static_cast<TESCaravanDeck*>(deckForm);
	auto* card = static_cast<TESCaravanCard*>(cardForm);
	*result = deck->cards->GetIndexOf(card);
	return true;
}



// Copied from Tweaks
double dAtan(double value)
{
	bool sign = (value < 0);
	if (sign) value = -value;

	bool complement = (value > 1.0);
	if (complement) value = 1.0 / value;

	bool region = (value > kDblTanPId12);
	if (region)
		value = (value - kDblTanPId6) / (1.0 + kDblTanPId6 * value);

	double res = value;
	value *= value;
	res *= (1.6867629106 + value * 0.4378497304) / (1.6867633134 + value);

	if (region) res += kDblPId6;
	if (complement) res = kDblPId2 - res;

	return sign ? -res : res;
}

// Copied from Tweaks.
double GetVectorAngle2D(NiPoint2* pt)
{
	double angle;
	if (pt->y == 0)
	{
		if (pt->x <= 0)
		{
			angle = kDblPIx3d2;
		}
		else
		{
			angle = kDblPId2;
		}
	}
	else
	{
		double ratio = pt->x / pt->y;
		angle = dAtan(ratio);
		if (pt->y < 0.0)
		{
			angle += kDblPI;
		}
	}

	return angle;
}


// Copied from Tweaks
// Returns a subtly different result than vanilla's Cmd_GetHeadingAngle...
double GetAngleBetweenPoints(NiPoint3* actorPos, NiPoint3* playerPos, float offset = 0)
{
	NiPoint3 diff;
	diff.Init(actorPos);
	diff.Subtract(playerPos);

	double angle = GetVectorAngle2D((NiPoint2*)&diff) - offset;
	if (angle > -kDblPI)
	{
		if (angle > kDblPI)
		{
			angle = kDblPIx2 - angle;
		}
	}
	else
	{
		angle += kDblPIx2;
	}
	return angle * kDblRad2Deg;
}

DEFINE_COMMAND_PLUGIN(GetHeadingAngleTEST, "", false, kParams_OneForm);
bool Cmd_GetHeadingAngleTEST_Execute(COMMAND_ARGS)
{
	TESObjectREFR* ref;
	ExtractArgsEx(EXTRACT_ARGS_EX, &ref);
	auto playerRotation = ((Actor*)thisObj)->AdjustRot(0);
	*result = GetAngleBetweenPoints(ref->GetPos(), thisObj->GetPos(), playerRotation);
	return true;
}

DEFINE_COMMAND_PLUGIN(GetPipboyRadioSounds, "", false, NULL);
bool Cmd_GetPipboyRadioSounds_Execute(COMMAND_ARGS)
{
	*result = 0;
	if (auto const radio = Radio::RadioEntry::GetSingleton())
	{
		//Sound* test;
		auto const outArr = g_arrInterface->CreateArray(nullptr, 0, scriptObj);
		for (auto iter = radio->unk04.sounds.Begin(); !iter.End(); iter.Next())
		{
			//todo: find a way to get meaningful info from a Sound*
			//g_arrInterface->AppendElement(outArr, ArrayElementR(*iter.Get()));
		}
		g_arrInterface->AssignCommandResult(outArr, result);
	}
	return true;
}


// Result fluctuates wildly, can't be used to predict anything.
DEFINE_COMMAND_PLUGIN(GetPipBoyRadioSoundTimeRemaining, "", false, NULL);
bool Cmd_GetPipBoyRadioSoundTimeRemaining_Execute(COMMAND_ARGS)
{
	if (auto const radio = Radio::RadioEntry::GetSingleton())
		*result = radio->unk04.soundTimeRemaining0C;
	else
		*result = 0;
	return true;
}




DEFINE_COMMAND_PLUGIN(SetCellFullNameAlt, "Like SetCellFullName but accepts a string.", false, kParams_JIP_OneCell_OneString);
bool Cmd_SetCellFullNameAlt_Execute(COMMAND_ARGS)
{
	TESObjectCELL* cell;
	char newName[256];

	ExtractArgsEx(EXTRACT_ARGS_EX, &cell, &newName);
	if (!cell) return true;

	TESFullName* fullName = &cell->fullName;  //wtf. Seems to work, idk why
	ThisStdCall(0x489100, fullName, newName);  //rip, needs something more to save-bake
	//fullName->name.Set(newName); //crashes

	return true;
}

// Takes from JIP's ClearMessageQueue.
DEFINE_COMMAND_PLUGIN(GetQueuedCornerMessages, "Returns the queued corner messages as a multidimensional array.", false, NULL);
bool Cmd_GetQueuedCornerMessages_Execute(COMMAND_ARGS)
{
	NVSEArrayVar* msgArr = g_arrInterface->CreateArray(NULL, 0, scriptObj);

	for (UINT32 iIndex = g_HUDMainMenu->queuedMessages.Count() + 1; ; --iIndex)
	{
		if (iIndex == 0) break;
		g_HUDMainMenu->queuedMessages.GetNthItem(iIndex);
		//no idea what to do with this :snig:
	}

	g_arrInterface->AssignCommandResult(msgArr, result);
	return true;
}

// Takes from JIP's "ModBaseAV"
DEFINE_COMMAND_ALT_PLUGIN(SetBaseActorValue, SetBaseAV, "", false, kParams_JIP_OneActorValue_OneFloat_OneOptionalActorBase);
bool Cmd_SetBaseActorValue_Execute(COMMAND_ARGS)
{
	UInt32 actorVal;
	float valueToSet;
	TESActorBase* actorBase = NULL;
	if (!ExtractArgs(EXTRACT_ARGS, &actorVal, &valueToSet, &actorBase)) return true;
	if (!actorBase)
	{
		if (!thisObj || !thisObj->IsActor()) return true;
		actorBase = (TESActorBase*)thisObj->baseForm;
	}
	UInt32 currentValue = *result = actorBase->avOwner.GetActorValue(actorVal);
	actorBase->ModActorValue(actorVal, (valueToSet - currentValue));
	return true;
}

#endif
